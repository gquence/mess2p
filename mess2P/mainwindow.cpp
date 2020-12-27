#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <QtCore>
#include <string>
#include "AES_ciph.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QList<QNetworkAddressEntry> all_addresses;
    QList<QHostAddress> all_broadcasts;

    all_interfaces = QNetworkInterface::allInterfaces();
    if (!all_interfaces.isEmpty())
    {
        for (int i = 0; i < all_interfaces.length(); i++)
        {
            QNetworkInterface::InterfaceType type =  all_interfaces[i].type();
            if (type != QNetworkInterface::Ethernet)
                continue;

            QList<QNetworkAddressEntry> address_per_interface;
            address_per_interface = all_interfaces[i].addressEntries();

            for (int j = 0; j < address_per_interface.length(); j++)
            {
                QHostAddress broadcastAddress = address_per_interface[j].broadcast();

                if (!broadcastAddress.isNull())
                {
                    all_broadcasts.append(broadcastAddress);
                }
            }
        }
    }
    else
    {
        QMessageBox::warning(this, "", "Interfaces error");
        return;
    }
    my_peer = new c_peer(this,  all_broadcasts[0], 2048, (ui->encrypted_msgs) , (ui->decrypted_msgs));
    ui->lineEdit->setDisabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete my_peer;
}

void MainWindow::on_pushButton_2_clicked()
{
    int checker;
    QString message = ui->lineEdit->text();

    ui->lineEdit->setText("");
    if (message == "")
        return ;
    checker = my_peer->send_message(message);

    if (checker == 0)
    {
        QMessageBox::warning(this, "", "You cant send message. Lost connection!");
    }
    return ;
}

void MainWindow::on_nickname_line_editingFinished()
{
    QString curr_text = ui->nickname_line->text();
    if ((curr_text == "Enter Your Nickname") || (curr_text == ""))
    {
        QMessageBox::warning(this, "Nickname warning", "You cant set nickname like this");
        ui->nickname_line->setText("Enter Your Nickname");
        return ;
    }

    account_name = curr_text;
    ui->nickname_line->setDisabled(true);
    my_peer->set_account_name(account_name);
    ui->lineEdit->setDisabled(false);
    return ;
}
