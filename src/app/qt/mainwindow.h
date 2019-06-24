#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <memory>
#include <flv/flvparser.h>

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
    void on_pushButton_clicked();

    void on_tableView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    AVD::Flv flv_;
    std::vector<char> hexBuf_;
    
private:
    void showFlv();
    void showHex(int idx);
    void setFlvTableHeader();
};

#endif // MAINWINDOW_H
