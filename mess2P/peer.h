#ifndef PEER_H
#define PEER_H

#include <QtCore>
#include <QtNetwork>
#include <QListWidget>
#include <string>

class c_peer : public QObject
{
    Q_OBJECT
public:
    explicit c_peer(QObject *parent, QHostAddress broadcast, int nPort,  QListWidget *encrypted_msgs, QListWidget *decrypted_msgs);
    ~c_peer();

    int         set_account_name(QString str);
    int         send_message(QString &str);

signals:
public slots:
    void        udpReadyRead();
private:
    QListWidget             *encrypted_msgs;
    QListWidget             *decrypted_msgs;
    QUdpSocket              *socket_S;
    QUdpSocket              *socket;
    QHostAddress            broadcast;
    int                     nPort;
    QString                 account_name = "";
    QMap<QString, std::string>  key_per_user;

    int     insert_new_msg(QByteArray &buffer);
    bool    send_json(QJsonObject &obj);
    bool    writeData(QByteArray &Data);
    void    set_my_new_key(void);
    void    set_new_user_key(QJsonObject &obj);
    int     send_my_key(void);
    int     send_key_req(void);
};

#endif // PEER_H
