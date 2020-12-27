#ifndef CLIENT_H
#define CLIENT_H

#include <QtCore>
#include <QtNetwork>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit    Client(QObject *parent = 0);
    bool        send_json(QJsonObject &obj);
public slots:
    bool        connectToHost(QString host, int nPort);
    bool        writeData(QString &str);
private slots:
    void        connection_error();
private:
    QUdpSocket  *socket;
    QTimer      *timer;
    QString     host;
    int         nPort;
};

#endif // CLIENT_H
