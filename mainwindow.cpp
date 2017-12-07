#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QColor>
#include <QMessageBox>
#include <QPalette>
#include <QFileDialog>
#include <QDateTime>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("CZ Serial"));
    myCom = NULL;
    readTimer = new QTimer(this);
    sendTimer = new QTimer(this);
    //信号和槽函数关联，延时一段时间，进行读串口操作
    connect(readTimer, SIGNAL(timeout()), this, SLOT(readMyCom()));
    connect(sendTimer, SIGNAL(timeout()), this, SLOT(sendMyCom()));
}

MainWindow::~MainWindow()
{
    //销毁串口对象
    if(myCom != NULL)
    {
        if(myCom->isOpen())
        {
            myCom->close();
        }
        delete myCom;
    }
    delete ui;
}

//打开串口
void MainWindow::on_but_open_serial_clicked()
{
    if(ui->but_open_serial->text()=="打开串口") //打开串口
    {
        //获取串口号
        QString portName = ui->cob_select_serial->currentText();
        //定义串口对象，指定串口名和查询模式，这里使用轮询模式
        myCom = new Win_QextSerialPort(portName,QextSerialBase::Polling);

        if(myCom ->open(QIODevice::ReadWrite))//成功打开串口
        {
            //波特率设置
            switch(ui->cob_select_baudrate->currentText().toInt())
            {
                case 110:myCom->setBaudRate(BAUD110);break;
                case 300:myCom->setBaudRate(BAUD300);break;
                case 600:myCom->setBaudRate(BAUD600);break;
                case 1200:myCom->setBaudRate(BAUD1200);break;
                case 2400:myCom->setBaudRate(BAUD2400);break;
                case 4800:myCom->setBaudRate(BAUD4800);break;
                case 9600:myCom->setBaudRate(BAUD9600);break;
                case 14400:myCom->setBaudRate(BAUD14400);break;
                case 19200:myCom->setBaudRate(BAUD19200);break;
                case 38400:myCom->setBaudRate(BAUD38400);break;
                case 56000:myCom->setBaudRate(BAUD56000);break;
                case 57600:myCom->setBaudRate(BAUD57600);break;
                case 115200:myCom->setBaudRate(BAUD115200);break;
                case 128000:myCom->setBaudRate(BAUD128000);break;
                case 256000:myCom->setBaudRate(BAUD256000);break;
                default: myCom->setBaudRate(BAUD9600);break;
            }
            myCom->setBaudRate((BaudRateType)ui->cob_select_baudrate->currentText().toInt());
            //数据位设置
            myCom->setDataBits((DataBitsType)ui->cob_select_databits->currentText().toInt());
            //奇偶校验设置
            switch(ui->cob_select_parity->currentIndex())
            {
                case 0:  myCom->setParity(PAR_NONE);break;//无校验
                case 1:  myCom->setParity(PAR_ODD); break;//奇校验
                case 2:  myCom->setParity(PAR_EVEN);break;//偶校验
                default: myCom->setParity(PAR_NONE);break;//无校验
            }
            //停止位设置
            switch(ui->cob_select_stopbits->currentIndex())
            {
                case 0:  myCom->setStopBits(STOP_1);break;  //1
                case 1:  myCom->setStopBits(STOP_1_5);break;//1.5
                case 2:  myCom->setStopBits(STOP_2);break;  //2
                default: myCom->setStopBits(STOP_1);break;  //1
            }
            myCom->setFlowControl(FLOW_OFF);//数据流控制设置，无数据流控制
            myCom->setTimeout(5);          //延时设置，设置为延时5ms
            readTimer->start(10);          //设置延时为10ms
            ui->but_open_serial->setText(tr("关闭串口"));
            ui->flag_rg->setIcon(QIcon(":/image/green.png"));
            ui->statusBar->showMessage(tr("打开串口成功!"),2000);
        }
        else
        {
            ui->statusBar->showMessage(tr("打开串口失败!"),2000);
            QMessageBox::critical(this, tr("错误提示"), tr("打开串口 ") + portName + tr(" 失败！") + tr("\n该串口设备不存在或已被占用！"), QMessageBox::Ok);
            return;
        }
    }
    else            //关闭串口
    {
        if(myCom != NULL)
        {
            if(myCom->isOpen())
            {
                myCom->close();
            }
            readTimer->stop();
            ui->but_open_serial->setText(tr("打开串口"));
            ui->flag_rg->setIcon(QIcon(":/image/red.png"));
            ui->statusBar->showMessage(tr("串口成功关闭!"),2000);
        }
        else
        {
            ui->statusBar->showMessage(tr("串口已经关闭!"),2000);
        }
    }
}

//黑底绿字的实现（修改样式表）
void MainWindow::on_cob_receive_background_clicked(bool checked)
{
    QPalette p = ui->edit_receive->palette();
    if(checked)//如果选中黑底绿字
    {
        ui->edit_receive->setStyleSheet("background-color: rgb(0, 0, 0)");//设置背景
        ui->edit_send->setStyleSheet("background-color: rgb(0, 0, 0)");//设置背景
        p.setColor(QPalette::Text, Qt::green);//设置字体颜色
        ui->edit_receive->setPalette(p);
        ui->edit_send->setPalette(p);
    }
    else
    {
        ui->edit_receive->setStyleSheet("background-color: rgb(255, 255, 255)");//设置背景
        ui->edit_send->setStyleSheet("background-color: rgb(255, 255, 255)");//设置背景
        p.setColor(QPalette::Text, Qt::black);//设置字体颜色
        ui->edit_receive->setPalette(p);
        ui->edit_send->setPalette(p);
    }
}

//清除接收Edit中的数据
void MainWindow::on_but_receive_clear_clicked()
{
    ui->edit_receive->clear();
    if(myCom != NULL)
    {
        if(myCom->isOpen())
        {
            myCom->flush(); //清除串口中的数据
        }
    }
    ui->statusBar->showMessage(tr("接收数据已经清空"),2000);
}

//清除发送Edit中的数据
void MainWindow::on_but_send_clear_clicked()
{
    ui->edit_send->clear();
}

//软件信息对话框
void MainWindow::on_action_triggered()
{
    QMessageBox box;
    box.setWindowTitle(tr("关于"));
    box.setIconPixmap(QPixmap(":/image/logo.png"));
    box.setText(tr("\n\nMy Serial V1.0\n\n作者：ChenZhe\n时间：2017-12-07"));
    box.addButton(tr("确定"),QMessageBox::AcceptRole);//加入自定义的按钮
    if(box.exec() == QMessageBox::Accepted)
        box.close();
}

//读取串口数据并显示出来
void MainWindow::readMyCom()
{
    QString buf;
    //读取串口缓冲区的所有数据给临时变量temp
    QByteArray temp = myCom->readAll();
    if(!temp.isEmpty())
    {
        if(ui->cob_receive16->isChecked())
        {

            for(int i = 0; i < temp.count(); i++)
            {
                QString s;
                s.sprintf("%02x ", (unsigned char)temp.at(i));
                buf += s;
            }
        }
        else
        {
            QTextCodec *tc = QTextCodec::codecForName("GBK");
            buf = tc->toUnicode(temp);
            if(ui->cob_time_stamp->isChecked())
            {
                QDateTime current_date_time = QDateTime::currentDateTime();
                buf = buf.replace(QString("\r\n"),QString("[" + current_date_time.toString("yyyy-MM-dd hh:mm:ss")+"]\r\n"));
            }
        }
        //将串口的数据追加在窗口的文本框中
        ui->edit_receive->insertPlainText(buf);
    }
}

//向串口发送数据
void MainWindow::on_but_send_clicked()
{
    sendMyCom();
}

//保存窗口
void MainWindow::on_but_save_win_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("保存为"), tr("未命名.txt"));
    QFile file(filename);
    //如果用户取消了保存则直接退出函数
    if(file.fileName().isEmpty()){
        return;
    }
    //如果打开失败则给出提示并退出函数
    if(!file.open(QFile::WriteOnly | QIODevice::Text)){
        QMessageBox::warning(this, tr("保存文件"), tr("打开文件 %1 失败, 无法保存\n%2").arg(filename).arg(file.errorString()), QMessageBox::Ok);
        return;
    }
    //写数据到文件
    QTextStream out(&file);
    out << ui->edit_receive->toPlainText();
    file.close();
}

void MainWindow::on_cob_send_timer_clicked(bool checked)
{
    if(checked)//定时发送
    {
        ui->edit_cycle_time->setDisabled(true);
        sendTimer->start(ui->edit_cycle_time->text().toInt());
    }
    else    //停止定时发送
    {
        sendTimer->stop();
        ui->edit_cycle_time->setDisabled(false);
    }
}

//向串口发送数据
void MainWindow::sendMyCom()
{
    QByteArray temp;
    if((myCom!=NULL)&&(myCom->isOpen()))
    {
        if(ui->cob_send16->isChecked())
        {
            bool ok;
            char data;
            QString str;
            QStringList list;
            str = ui->edit_send->toPlainText();
            list = str.split(" ");
            for(int i = 0; i < list.count(); i++)
            {
                if(list.at(i) == " ")
                    continue;
                if(list.at(i).isEmpty())
                    continue;
                data = (char)list.at(i).toInt(&ok, 16);
                if(!ok){
                    QMessageBox::information(this, tr("提示消息"), tr("输入的数据格式有错误！"), QMessageBox::Ok);
                    return;
                }
                temp.append(data);
            }
        }
        else
        {
            temp = ui->edit_send->toPlainText().toLocal8Bit();
            if(ui->cob_send_newline->isChecked())   //发送新行
                temp = temp + "\r\n";
        }
        myCom->write(temp);
    }
}

//打开文件
void MainWindow::on_but_open_file_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("打开文件"), 0, tr("保存为 (*.txt *.log)"));  //添加更多的文件类型
    QFile file(filename);

    //如果取消打开则退出函数
    if(file.fileName().isEmpty()){
        return;
    }
    ui->progressBar->setValue(0);
    ui->edit_file_path->setText(filename);
}

//发送文件
void MainWindow::on_but_send_file_clicked()
{
    //未完成
//    if(myCom->isOpen())
//    {
//        QFile file(ui->edit_file_path->text());

//        //如果取消打开则退出函数
//        if(file.fileName().isEmpty()){
//            return;
//        }

//        //如果打开失败则提示并退出函数
//        if(!file.open(QFile::ReadOnly | QIODevice::Text)){
//            QMessageBox::warning(this, tr("打开失败"),
//                                 tr("抱歉!文件无法打开！"),
//                                 QMessageBox::Ok);
//            return;
//        }

//        QByteArray line;//读取数据并加入到发送数据编辑框
//        int filesize = file.size();
//        int count = 0;
//        while(!file.atEnd()){
//            line = file.read(512);
//            while(!myCom->isWritable());
//            myCom->write(line);//发送数据
//            int percen = (++count)*51200/filesize;
//            ui->progressBar->setValue(percen);//计算百分比
//        }
//        ui->progressBar->setValue(100);//计算百分比
//        file.close();
//    }
}
