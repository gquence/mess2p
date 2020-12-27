#include "client.h"

Client::Client(QObject *parent) : QObject(parent)
{
    socket = new QUdpSocket(this);
}

bool Client::connectToHost(QString host, int nPort)
{
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(connection_error()));
    socket->connectToHost(host, nPort);
    this->host = host;
    this->host = nPort;
    timer->start();
    return socket->waitForConnected();
}


void Client::connection_error()
{
    if (socket->state() == QAbstractSocket::ConnectingState)
        qDebug() << "still connecting to \n";
    else if (socket->state() != QAbstractSocket::ConnectedState)
        qDebug() << "connection error\n";
    timer->stop();
}

bool Client::send_json(QJsonObject &obj)
{
    QJsonDocument doc(obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    return (writeData(strJson));
}


bool Client::writeData(QString &str)
{

    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        QByteArray  arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);
        QString res_mess;

        timer->setInterval(100);
        connect(timer, SIGNAL(timeout()), this, SLOT(connection_error()));
        res_mess = "|" + QTime::currentTime().toString() + "|" + str;
        res_mess.prepend(QString::number(str.length()));
        out << res_mess.toUtf8();
        qDebug() << res_mess.toUtf8();
        socket->write(arrBlock);
        timer->start();

        return socket->waitForBytesWritten();
    }
    else
           return false;
}
