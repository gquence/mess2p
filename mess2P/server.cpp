z#include "server.h"
#include <QtCore>
#include <QMessageBox>

Server::Server(QList<t_message_data> &messages_data, QObject *parent, int nPort) : QObject(parent), messages_data(messages_data)
{
    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, nPort)) {
        QMessageBox::critical(0,
                              "Server Error",
                              "Unable to start the server:"
                              + server->errorString()
                             );
        server->close();
        return;
    }
    connect(server, SIGNAL(newConnection()), this,
            SLOT(slotNewConnection()));
}

Server::~Server()
{
    delete server;
}

void Server::slotNewConnection()
{
    QTcpSocket* pClientSocket = server->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket,
            SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this,
            SLOT(slotReadClient()));

    sendToClient(pClientSocket, "Server Response: Connected!");
}

void Server::slotReadClient()
{
    const uint32_t  block_size = 1024;
    QTcpSocket      *socket = (QTcpSocket*)(sender());
    QTcpSocket      *Primary_socket = socket;
    int             nextBlockSize = 0;
    QTime           receive_time;
    QString         message;

    socket->setReadBufferSize(block_size - 1);
    for (;;) {
        qint64 curr_bytes_count;
        char buf[block_size];
        QString buffer;

        curr_bytes_count = socket->bytesAvailable();
        if (nextBlockSize == 0 && curr_bytes_count < 12) {
           sendToClient(socket, "500\nStatus: ERROR\nMessage is not in standard format\n");
           break;
        }

        socket->read(buf, block_size - 1);
        buffer = buf;

        if (buffer.count('|') == 2)
        {
            QStringList all_content = buffer.split(QLatin1Char('|'));
            for (auto i : all_content)
            {
                if (i == "")
                {
                    sendToClient(Primary_socket, "500\nStatus: ERROR\nWrong message size\n");
                    break;
                }
            }
            nextBlockSize = all_content[0].toUInt();
            if (nextBlockSize == 0)
            {
                sendToClient(Primary_socket, "200\nStatus OK\n");
                break;
            }

            receive_time = QTime::fromString(all_content[1]);
            message = QString::fromUtf8(all_content[2].toUtf8());
            if (nextBlockSize == (curr_bytes_count - 2 - (all_content[1].length() +  all_content[0].length())))
                message = message.left(nextBlockSize);
            else
            {
                sendToClient(Primary_socket, "500\nStatus: ERROR\nInvalid size of message\n");
                break;
            }
            addMessageInfo(message, receive_time);
            sendToClient(Primary_socket, "200\nStatus OK\n");
            break;
        }
        else if (buffer.count('|') > 2){
            sendToClient(Primary_socket, "500\nStatus: ERROR\n You can't use '|' in user-messages\n");
            break;
        }
        else
        {
            sendToClient(Primary_socket, "500\nStatus: ERROR\nNot enough service-data\n");
            break;
        }
   }
}

QList<t_message_data>& Server::get_message_list()
{
    return messages_data;
}

void Server::sendToClient(QTcpSocket* pSocket, const QString& str)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    QString res_mess;

    res_mess = "|" + QTime::currentTime().toString() + "|" + str;
    res_mess.prepend(QString::number(str.length()));
    out << res_mess.toUtf8();
    pSocket->write(arrBlock);
}


void Server::addMessageInfo(QString &message, const QTime &receive_time)
{
    t_message_data mess;

    mess.message = message;
    mess.receive_time = receive_time;
    messages_data.append(mess);
}
