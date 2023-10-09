#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDateTime>


#define BIND_PORT_SERV 12345
#define BIND_PORT_ATT 12346


enum class socketType{
    service,
    attendant
};

class UDPworker : public QObject
{
    Q_OBJECT
public:
    explicit UDPworker(QObject *parent = nullptr);
    void InitSocket( void );
    void ReadDatagram( QNetworkDatagram datagram, socketType type);
    void SendDatagram(QByteArray data, socketType type);


private slots:
    void readPendingDatagrams(void);

private:
    QUdpSocket* serviceUdpSocket;
    QUdpSocket* attendantUdpSocket;

signals:
    void sig_sendTimeToGUI(QDateTime data);
    void sig_sendMsgDataToGUI(QMap<QString, QString> msgData);

};

#endif // UDPWORKER_H
