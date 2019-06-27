#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <tools/hexprint.h>

#include <QHexView/document/buffer/qmemorybuffer.h>

#include <iostream>

static const char* typeNameMap[11] = {"audio", "video", "unknown", "unknown", "unknown", "unknown"
    , "unknown", "unknown", "unknown", "unknown", "script"};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableView->verticalHeader()->setHidden(true); // 隐藏行号
    
    QHexDocument* document = QHexDocument::fromMemory<QMemoryBuffer>("", 0);
    hexView_ = new QHexView();
    hexView_->setDocument(document);
    ui->splitter->addWidget(hexView_);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode( QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    setFlvTableHeader();
}

void MainWindow::setFlvTableHeader() {
    
    auto model = dynamic_cast<QStandardItemModel*>(ui->tableView->model());
    bool shouldAdd = false;
    if (model == nullptr) {
        model = new QStandardItemModel(this);
        shouldAdd = true;
    }
#if 1
    model->setColumnCount(6);
    model->setHeaderData(0,Qt::Horizontal, "NO.");
    model->setHeaderData(1,Qt::Horizontal, "type");
    model->setHeaderData(2,Qt::Horizontal, "pts");
    model->setHeaderData(3,Qt::Horizontal, "offset");
    model->setHeaderData(4,Qt::Horizontal, "size");
    model->setHeaderData(5,Qt::Horizontal, "ptag length");
#else
    model->setColumnCount(3);
    model->setHeaderData(0,Qt::Horizontal, "姓名");
    model->setHeaderData(1,Qt::Horizontal, "年龄");
    model->setHeaderData(2,Qt::Horizontal, "性别");
    
    model->setItem(0, 0, new QStandardItem("张三"));
    model->setItem(0, 1, new QStandardItem("3"));
    model->setItem(0, 2, new QStandardItem("男"));
    model->setItem(1, 0, new QStandardItem("张是"));
    model->setItem(1, 1, new QStandardItem("4"));
    model->setItem(1, 2, new QStandardItem("女博士"));
#endif
    
    if (shouldAdd) {
        ui->tableView->setModel(model);
    }
}

void MainWindow::setFlvTreeHeader() {
    
    auto model = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    bool shouldAdd = false;
    if (model == nullptr) {
        model = new QStandardItemModel(this);
        shouldAdd = true;
    }
    if (shouldAdd) {
        ui->treeView->setModel(model);
    }
    model->clear();
    model->setHorizontalHeaderLabels(QStringList()<<QStringLiteral("name") << QStringLiteral("value"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                          tr("文件对话框！"),
                                                          ".");
    ui->textEdit_2->setText(fileName);
    
    
    std::string fname = fileName.toStdString();
    auto fileReader = AVD::Flv::FileReader::NewFileReader(fname);
    if (!fileReader->IsOk()) {
        QMessageBox::information(NULL, "notify", "open" + fileName + "fail",
                                 QMessageBox::Yes);
        return;
    }
    int ret = 0;
    if ((ret = flv_.Parse(fileReader, 10*1024*1024)) == AVD::Flv::OK) {
        showFlv();
    } else {
        printf("flv parse error:%d\n", ret);
    }
    
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    auto model = dynamic_cast<QStandardItemModel*>(ui->tableView->model());
    auto v = model->data(index.siblingAtColumn(0), Qt::UserRole);
    
    showItem(index.row());
    qDebug()<<index<<"=="<<index.row()<<"---"<<v.value<void *>();
}

void MainWindow::showFlv() {
    
    int count = 0;
    
    auto flv = flv_.GetFlv();
    
    auto model = dynamic_cast<QStandardItemModel*>(ui->tableView->model());
    model->clear();
    setFlvTableHeader();
    
    
    model->setItem(count, 0, new QStandardItem(QString::number(count)));
    model->setItem(count, 1, new QStandardItem("header"));
    model->setItem(count, 2, new QStandardItem("0"));
    model->setItem(count, 3, new QStandardItem(QString::number(flv.first->Pos.nOffset)));
    model->setItem(count, 4, new QStandardItem("9"));
    model->setItem(count, 5, new QStandardItem(QString::number(flv.first->PreviousTagSize)));
    model->setData(model->index(count, 0), QVariant::fromValue(reinterpret_cast<void*>(count)), Qt::UserRole);

    count++;
    auto tags = flv.second;
    for (int i = 0; i < tags.size(); i++) {
        model->setItem(count, 0, new QStandardItem(QString::number(count)));
        model->setItem(count, 1, new QStandardItem(typeNameMap[tags[i].TagType-8]));
        model->setItem(count, 2, new QStandardItem(QString::number(tags[i].Timestamp)));
        model->setItem(count, 3, new QStandardItem(QString::number(tags[i].Pos.nOffset)));
        model->setItem(count, 4, new QStandardItem(QString::number(tags[i].DataSize)));
        model->setItem(count, 5, new QStandardItem(QString::number(tags[i].PreviousTagSize)));

        model->setData(model->index(count, 0), QVariant::fromValue(reinterpret_cast<void*>(count)), Qt::UserRole);
        count++;
        
    }
}


void MainWindow::showItem(int idx) {
    
    auto flv = flv_.GetFlv();
    uint8_t* pData = NULL;
    int nDataLen = 0;
    if (idx == 0) {
        pData = flv.first->pData;
        nDataLen = flv.first->Pos.nSize;
        showFlvHeader(flv.first);
    } else {
        auto tags = flv.second;
        pData = tags[idx-1].pData;
        nDataLen = tags[idx-1].Pos.nSize;
        showFlvTag(&tags[idx-1]);
    }
    
    showItemHex(pData, nDataLen);
    return;
}

void MainWindow::showFlvHeader(const AVD::FlvHeader* pHdr) {
    
    setFlvTreeHeader();
    auto model = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    
    QList<QStandardItem*> root;
    QStandardItem* item1 = new QStandardItem(QStringLiteral("Signature"));
    std::string sig((const char*)pHdr->Signature, 3);
    QStandardItem* item2 = new QStandardItem(QString::fromStdString(sig));
    root.append(item1);
    root.append(item2);
    model->appendRow(root);
    
    const char *names[] = {"Version", "TypeFlagsReserved1", "TypeFlagsAudio", "TypeFlagsReserved2",
        "TypeFlagsVideo", "DataOffset", "PreviousTagSize"};
    uint32_t vs[] = {pHdr->Version, pHdr->TypeFlagsReserved1, pHdr->TypeFlagsAudio, pHdr->TypeFlagsReserved2,
        pHdr->TypeFlagsVideo, pHdr->DataOffset, pHdr->PreviousTagSize};
    
    for (int i = 0; i < sizeof(vs)/sizeof(uint32_t); i++) {
        QList<QStandardItem*> root;
        item1 = new QStandardItem(names[i]);
        item2 = new QStandardItem(QString::number(vs[i]));
        root.append(item1);
        root.append(item2);
        model->appendRow(root);
    }
}

void MainWindow::showFlvTag(AVD::FlvTag* pTag) {
    
    setFlvTreeHeader();
    auto model = dynamic_cast<QStandardItemModel*>(ui->treeView->model());
    
    QString typeStr("8-audio");
    if (pTag->TagType == 9) {
        typeStr = "9-video";
    } else if (pTag->TagType == 18) {
        typeStr = "18-script";
    }
    const char *names[] = {"DataSize", "Timestamp", "StreamID", "PreviousTagSize"};
    uint32_t vs[] = {pTag->DataSize, pTag->Timestamp, pTag->StreamID, pTag->PreviousTagSize};
    
    QList<QStandardItem*> root;
    QStandardItem* item1 = new QStandardItem(QStringLiteral("TagType"));
    QStandardItem* item2 = new QStandardItem(typeStr);
    root.append(item1);
    root.append(item2);
    model->appendRow(root);
    
    for (int i = 0; i < sizeof(vs)/sizeof(uint32_t); i++) {
        QList<QStandardItem*> root;
        item1 = new QStandardItem(names[i]);
        item2 = new QStandardItem(QString::number(vs[i]));
        root.append(item1);
        root.append(item2);
        model->appendRow(root);
    }
}

void  MainWindow::showItemHex(uint8_t* pData, int nDataLen) {

    QByteArray barr((const char*)pData, nDataLen);
    QHexDocument* doc = hexView_->document();
    if (doc->canUndo())
        doc->undo();
    
    doc->insert(0, barr);
    
    return;
}
