#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkInterface>
#include <QNetworkAddressEntry>
#include "peer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_pushButton_2_clicked();

    void on_nickname_line_editingFinished();

private:
    Ui::MainWindow *ui;

    c_peer *my_peer;
    QList<QNetworkInterface> all_interfaces;
    QString account_name = "";
};
#endif // MAINWINDOW_H
