#include "peer.h"
#include "AES_ciph.h"
#include <QUuid>
#include <string>
#include <ctime>

static QJsonObject ObjectFromString(const QString& in)
{
    QJsonObject obj;

    QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());

    // check validity of the document
    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            obj = doc.object();
        }
        else
        {
            qDebug() << "Document is not an object\n";
        }
    }
    else
    {
        qDebug() << "Invalid JSON...\n" << in << "\n";
    }

    return obj;
}

c_peer::c_peer(QObject *parent, QHostAddress broadcast, int nPort, QListWidget *encrypted_msgs, QListWidget *decrypted_msgs) : QObject(parent), broadcast(broadcast), nPort(nPort)
{
    srand( (unsigned) time(NULL));
    this->encrypted_msgs = &(*encrypted_msgs);
    this->decrypted_msgs = &(*decrypted_msgs);
    socket = new QUdpSocket(this);
    socket_S = new QUdpSocket(this);
    bool ret_val = socket_S->bind(nPort, QUdpSocket::ShareAddress);

    if (!ret_val)
        qDebug() << "connection error\n";
    connect(socket_S, SIGNAL(readyRead()), this, SLOT(udpReadyRead()));
}


void    c_peer::set_my_new_key(void)
{
    if (account_name == "")
        return;
    if (!key_per_user.contains(account_name))
    {
        std::string key = get_key();
        key_per_user.insert(account_name, key);
    }
    return;
}

void    c_peer::set_new_user_key(QJsonObject &obj)
{
    QString account;
    std::string key;

    account = obj["account"].toString();
    key = obj["key"].toString().toStdString();
    if (!key_per_user.contains(account))
    {
        key_per_user.insert(account, key);
    }
    return;
}

int c_peer::set_account_name(QString str)
{
    if (str == "")
        return 1;
    account_name = str + QUuid::createUuid().toString();
    return 0;
}

int c_peer::send_my_key(void)
{
    if ((account_name == "") || (!key_per_user.contains(account_name)))
        return -1;

    QJsonObject json_msg;

    json_msg.insert("account", QJsonValue::fromVariant(account_name));
    json_msg.insert("key", "");

    QJsonDocument doc(json_msg);
    QByteArray Data = doc.toJson(QJsonDocument::Compact);
    int pos = Data.lastIndexOf("\"}");
    Data = Data.left(pos);
    Data.push_back(key_per_user[account_name].c_str());
    Data.push_back("\"}");

    return (int)(writeData(Data));
}

int c_peer::send_key_req(void)
{
    QJsonObject json_msg;

    json_msg.insert("key_req", QJsonValue::fromVariant("hello"));
    return (int)(send_json(json_msg));
}

int c_peer::send_message(QString &str)
{
    if (account_name == "")
        return -1;

    QJsonObject json_msg;
    QString encr_msg;

    json_msg.insert("account", QJsonValue::fromVariant(account_name));

    if (!key_per_user.contains(account_name))
    {
        set_my_new_key();
        send_my_key();
    }
    std::string ciphertext, plaintext;

    plaintext = str.toUtf8().toStdString();
    encrypt(plaintext, (const unsigned char *)key_per_user[account_name].c_str(), ciphertext);
    QByteArray ciphertext_byte = QByteArray::fromRawData(ciphertext.c_str(), ciphertext.length());
    json_msg.insert("msg", QJsonValue::fromVariant(""));

    QJsonDocument doc(json_msg);
    QByteArray Data = doc.toJson(QJsonDocument::Compact);
    int pos = Data.lastIndexOf("\"}");
    Data = Data.left(pos);
    qDebug() << "\n\n\nciphertext = " <<ciphertext_byte;
    Data.push_back(ciphertext_byte);
    Data.push_back("\"}");
    qDebug() << "\n\n\nData = " <<Data;
    qDebug() << ciphertext.c_str();
    return (int)(writeData(Data));
}

bool c_peer::send_json(QJsonObject &obj)
{
    QJsonDocument doc(obj);
    QByteArray Data = doc.toJson(QJsonDocument::Compact);
    return (writeData(Data));
}

bool c_peer::writeData(QByteArray &Data)
{
    qDebug() << "\n\n\n123 = " <<Data;
    qint64 ret_val = socket->writeDatagram(Data, Data.size(), broadcast, nPort);
    if (ret_val == -1)
    {
        qDebug() << "writing goes wrong\n";
        return false;
    }
    return true;
}

void c_peer::udpReadyRead()
{
    while (socket_S->hasPendingDatagrams())
    {
        QByteArray buffer;
        QHostAddress sender;
        quint16 senderPort;
        buffer.resize(socket_S->pendingDatagramSize());
        socket_S->readDatagram(buffer.data(), buffer.size(),
                             &sender, &senderPort);
        QJsonObject obj = ObjectFromString(buffer.data());

        if (obj.contains("key_req"))
            send_my_key();
        if (obj.contains("key"))
            set_new_user_key(obj);
        if (obj.contains("msg"))
            insert_new_msg(buffer);

        qDebug() << "Message from: " << sender.toString();
        qDebug() << "Message port: " << senderPort;
        qDebug() << "Message: " << buffer;
    }

}

int     c_peer::insert_new_msg(QByteArray &buffer)
{
    QJsonObject obj = ObjectFromString(buffer.data());
    QString encr_msg, decr_msg, account, full_account;

    full_account = obj["account"].toString();
    account = full_account.left(full_account.indexOf("{"));


    QByteArray msg(buffer);
    int msg_start = msg.size() - msg.lastIndexOf("msg\":\"") - 6;
    msg = msg.right(msg_start);
    msg = msg.left(msg.lastIndexOf("\"}"));
    qDebug() << "\n\n\nmsg = "<< msg;

    //encr_msg
    qDebug() << "\n\n\ninsert data=  " <<  buffer.data();
    encr_msg = obj["msg"].toString();
    if (!key_per_user.contains(full_account))
    {
        send_key_req();
        decr_msg = "You have no key right now";
    }
    else
    {
        std::string ciphertext, plaintext;

        ciphertext = msg.toStdString();
        qDebug() << ciphertext.c_str();
        decrypt(ciphertext, (const unsigned char *)key_per_user[full_account].c_str(), plaintext);
        decr_msg = QString::fromStdString(plaintext);
    }
    decrypted_msgs->insertItem(0, "\n");
    decrypted_msgs->insertItem(0, account + ":\n" + decr_msg);
    encrypted_msgs->insertItem(0, "\n");
    encrypted_msgs->insertItem(0, account + ":\n" + encr_msg);
    return 0;
}

c_peer::~c_peer()
{
    delete socket;
}
