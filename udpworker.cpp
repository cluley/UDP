#include "udpworker.h"

UDPworker::UDPworker(QObject *parent) : QObject(parent)
{



}


/*!
 * @brief Метод инициализирует UDP сервер
 */
void UDPworker::InitSocket()
{

    serviceUdpSocket = new QUdpSocket(this);
    attendantUdpSocket = new QUdpSocket(this);

    serviceUdpSocket->bind(QHostAddress::LocalHost, BIND_PORT_SERV);
    attendantUdpSocket->bind(QHostAddress::LocalHost, BIND_PORT_ATT); //Спросить

    connect(serviceUdpSocket, &QUdpSocket::readyRead, this, &UDPworker::readPendingDatagrams);
    connect(attendantUdpSocket, &QUdpSocket::readyRead, this, &UDPworker::readPendingDatagrams);

}

/*!
 * @brief Метод осуществляет обработку принятой датаграммы
 */
void UDPworker::ReadDatagram(QNetworkDatagram datagram, socketType type)
{
    if(type == socketType::service)
    {
        QByteArray data;
        data = datagram.data();

        QDataStream inStr(&data, QIODevice::ReadOnly);
        QDateTime dateTime;
        inStr >> dateTime;

        emit sig_sendTimeToGUI(dateTime);
    }
    else if(type == socketType::attendant)
    {
        QByteArray data;
        data = datagram.data();

        QDataStream inStr(&data, QIODevice::ReadOnly);
        QString text;
        inStr >> text;

        QMap<QString, QString> msgData;
        msgData["Adress"] = datagram.senderAddress().toString();
        msgData["MsgSize"] = QString::number(data.size());
        msgData["MsgLength"] = QString::number(text.size());


        emit sig_sendMsgDataToGUI(msgData);
    }

}
/*!
 * @brief Метод осуществляет опередачу датаграммы
 */
void UDPworker::SendDatagram(QByteArray data, socketType type)
{
    switch(type)
    {
    case socketType::service:
        serviceUdpSocket->writeDatagram(data, QHostAddress::LocalHost, BIND_PORT_SERV);
        break;
    case socketType::attendant:
        attendantUdpSocket->writeDatagram(data, QHostAddress::LocalHost, BIND_PORT_ATT);
        break;
    default:
        break;
    }

}

/*!
 * @brief Метод осуществляет чтение датаграм из сокета
 */
void UDPworker::readPendingDatagrams( void )
{
    if(sender() == serviceUdpSocket)
    {
        while(serviceUdpSocket->hasPendingDatagrams()){
            QNetworkDatagram datagram = serviceUdpSocket->receiveDatagram();
            ReadDatagram(datagram, socketType::service);
        }
    }
    else if(sender() == attendantUdpSocket)
    {
        while(attendantUdpSocket->hasPendingDatagrams()){
            QNetworkDatagram datagram = attendantUdpSocket->receiveDatagram();
            ReadDatagram(datagram, socketType::attendant);
        }
    }

}
