#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <QMessageBox>
#include <QMainWindow>
#include "musicwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    inline int& getDirector(){return sd;}
private slots:

    void on_pushButton_clicked();
    //void on_signup_clicked();
signals:
    void loginSuccess();
private:
    QLabel* Label_logOk;
    QLabel* Label_logWrong;
    QLabel* Label_signOk;
    void logonSuccessful();
    void logonUnSuccesful();
    //void signUpSuccessful();
    void init();
    Ui::MainWindow *ui;
    MusicWindow *musWind;

    int sd;

    struct sockaddr_in server;
};
#endif // MAINWINDOW_H

