#ifndef SERVER_H
#define SERVER_H


#include <QtCore>
#include <QtNetwork>

typedef struct  s_message_data
{
    QString message;
    QTime   receive_time;
}               t_message_data;

class Server : public QObject
{
    Q_OBJECT
public:
    Server(QList<t_message_data> &messages_data, QObject *parent = 0, int nPort = 0);
    ~Server();
    QList<t_message_data> &get_message_list();
signals:
    void dataReceived(QByteArray);

private slots:
    void slotNewConnection();
    void slotReadClient();

private:
    void sendToClient(QTcpSocket* pSocket, const QString& str);
    void addMessageInfo(QString &mess, const QTime &receive_time);
    QTcpServer *server;
    QHash<QTcpSocket*, QByteArray*> buffers; //We need a buffer to store data until block has completely received
    QHash<QTcpSocket*, qint32*> sizes; //We need to store the size to verify if a block has received completely
    QList<t_message_data> messages_data;
};

#endif // SERVER_H
