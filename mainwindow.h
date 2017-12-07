#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "win_qextserialport.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void readMyCom(); //读取串口
    void sendMyCom(); //串口发送

    void on_but_open_serial_clicked();

    void on_cob_receive_background_clicked(bool checked);

    void on_but_receive_clear_clicked();

    void on_but_send_clear_clicked();

    void on_action_triggered();

    void on_but_send_clicked();

    void on_but_save_win_clicked();

    void on_cob_send_timer_clicked(bool checked);

    void on_but_open_file_clicked();

    void on_but_send_file_clicked();

private:
    Ui::MainWindow *ui;
    QTimer *readTimer;
    QTimer *sendTimer;
    Win_QextSerialPort *myCom;  //串口对象
};

#endif // MAINWINDOW_H
