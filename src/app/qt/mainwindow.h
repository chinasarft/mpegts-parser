#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <memory>
#include <flv/flvparser.h>
#include <QHexView/qhexview.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_tableView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    AVD::Flv flv_;
    QHexView* hexView_;
    
private:
    void showFlv();
    void showItem(int idx);
    void showFlvHeader(const AVD::FlvHeader* pHdr);
    void showFlvTag(AVD::FlvTag* pTag);
    void showItemHex(uint8_t* pData, int nDataLen);
    void setFlvTableHeader();
    void setFlvTreeHeader();
   
// start of menu bar
private:
    QMenu *pFileMenu_;
    QAction *pOpenAct_;
    
private:
    void createFileMenu();
    
private slots:
    void openFile();
//end of menu bar
    
};

#endif // MAINWINDOW_H
