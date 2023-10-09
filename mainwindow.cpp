#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pb_stop->setEnabled(false);

    udpWorker = new UDPworker(this);
    udpWorker->InitSocket();

    connect(udpWorker, &UDPworker::sig_sendTimeToGUI, this, &MainWindow::DisplayTime);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]{

        QDateTime dateTime = QDateTime::currentDateTime();

        QByteArray dataToSend;
        QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

        outStr << dateTime;

        udpWorker->SendDatagram(dataToSend, socketType::service);
        timer->start(TIMER_DELAY);

    });

    connect(udpWorker, &UDPworker::sig_sendMsgDataToGUI, this, &MainWindow::DisplayMsg);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pb_start_clicked()
{
    timer->start(TIMER_DELAY);
    ui->pb_stop->setEnabled(true);
    ui->pb_start->setEnabled(false);
}


void MainWindow::DisplayTime(QDateTime data)
{
    counterPck++;
    if(counterPck % 20 == 0){
        ui->te_result->clear();
    }

    ui->te_result->append("Текущее время: " + data.toString() + ". "
                "Принято пакетов " + QString::number(counterPck));


}

void MainWindow::DisplayMsg(QMap<QString, QString> msgData)
{
    ui->te_result->append("Принято сообщение от " + msgData["Adress"] +
                          "\n Размер сообщения "  + msgData["MsgSize"] + " байт" +
                          "\n Длина сообщения "   + msgData["MsgLength"] + " символов");

}


void MainWindow::on_pb_stop_clicked()
{
    timer->stop();
    ui->pb_stop->setEnabled(false);
    ui->pb_start->setEnabled(true);
}


void MainWindow::on_pb_send_clicked()
{
    QString text = ui->te_input->toPlainText();

    QByteArray dataToSend;
    QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

    outStr << text;

    udpWorker->SendDatagram(dataToSend, socketType::attendant);
}

