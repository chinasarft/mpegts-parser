#include <QStandardItem>
#include <tools/sps.h>
#include <tools/AVCDecoderConfigurationRecord.h>
#include <tools/audioinfo.h>


template <typename T1>
void showTreeViewHelper(QStandardItem *root, std::vector<const char *>& names, std::vector<T1>& values) {
    QStandardItem* item1 = nullptr;
    QStandardItem* item2 = nullptr;
    QList<QStandardItem*> row;
    
    for (int i = 0; i < names.size(); i++) {
        row.clear();
        item1 = new QStandardItem(names[i]);
        item2 = new QStandardItem(QString::number(values[i]));
        row.append(item1);
        row.append(item2);
        root->appendRow(row);
    }
    return;
}

void ShowTreeViewOfSPS(QStandardItem *root, AVD::SPS* sps);
void ShowTreeViewOfAVCDecoderConfigurationRecord(QStandardItem *root, AVD::AVCDecoderConfigurationRecord* r);
void ShowTreeViewOfAudioSpecificConfig(QStandardItem *root, AVD::AudioSpecificConfig* r);

