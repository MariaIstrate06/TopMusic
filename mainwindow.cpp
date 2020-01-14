#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_musicwindow.h"
#include <string.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    {
        ui->setupUi(this);
        init();


        Label_logOk = new QLabel(this);
        Label_logOk->setHidden(true);
        Label_logOk->setText("YOU ARE NOW LOGGED IN");
        ui->loginItemsLayout->addWidget(Label_logOk);

        Label_logWrong = new QLabel(this);
        Label_logWrong->setHidden(true);
        Label_logWrong->setText("Wrong username or password. Try again!");
        ui->loginItemsLayout->addWidget(Label_logWrong);

        Label_signOk = new QLabel(this);
        Label_signOk->setHidden(true);
        Label_signOk->setText("SIGN UP COMPLETE");
        ui->loginItemsLayout->addWidget(Label_signOk);

        musWind = new MusicWindow(nullptr, sd);
        connect(this, SIGNAL(loginSuccess()), musWind, SLOT(startRefresh()));


    }

void MainWindow::init(){
    int port = 3005;
    if((sd=socket(AF_INET, SOCK_STREAM,0))==-1)
        {
            perror("[CLIENT] socket() error.\n");
        }
        /* gunoaie */
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr("127.0.0.1");
        server.sin_port = htons(port);

        if(::connect(sd, (struct sockaddr *)&server, sizeof(sockaddr)) == -1){
            perror("[CLIENT] connect() error.\n");
        }
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    printf("AICI SUNTEM!");
    char user_name[50], password[50];
    memset(user_name, 0, sizeof(user_name)); ///ERA &user_name, &password
    memset(password, 0, sizeof(user_name));
    strcpy(user_name, ui->user->text().toStdString().c_str());
    strcpy(password, ui->pass->text().toStdString().c_str());

   // write(this->sd, "LOG", 256);
    write(this->sd, user_name, 50);
    write(this->sd, password, 50);

    char message[256];
    memset(message, 0, 256);
    read(this->sd, message, 256);

    printf("%s\n", message);
    fflush(stdout);

    if(strcmp(message,"YES")==0)
        logonSuccessful();
    else
        logonUnSuccesful();
}
/*
void MainWindow::on_signup_clicked(){
    char user_name[50], password[50];
    memset(user_name, 0, sizeof(user_name));
    memset(password, 0, sizeof(user_name));
    strcpy(user_name, ui->user->text().toStdString().c_str());
    strcpy(password, ui->pass->text().toStdString().c_str());
    write(this->sd, "SIGN", 256);
    write(this->sd, user_name, 50);
    write(this->sd, password, 50);

    char message[256];
    memset(message, 0, 256);
    read(this->sd, message, 256);

    printf("%s\n", message);
    fflush(stdout);

    if(strcmp(message,"YES")==0)
        signUpSuccessful();
}*/
/*
void MainWindow::signUpSuccessful(){
    Label_signOk->show();
    Label_signOk->setHidden(false);
    this->update();
    musWind->show();
    this->hide();

    QMessageBox popup;
    popup.setText("Wrong username or password!");
    popup.setStandardButtons(QMessageBox::Ok);
    popup.setDefaultButton(QMessageBox::Ok);

    popup.exec();
    musWind->show();
}*/
void MainWindow::logonSuccessful()
{
    Label_logOk->show();
    Label_logOk->setHidden(false);
    this->update();
    musWind->show();
    this->hide();
    emit(loginSuccess());
}

void MainWindow::logonUnSuccesful(){
    ui->loginGroup->hide();
    QMessageBox popup;
    popup.setText("Wrong username or password!");
    popup.setStandardButtons(QMessageBox::Ok);
    popup.setDefaultButton(QMessageBox::Ok);

    popup.exec();
    ui->user->clear();
    ui->pass->clear();
    ui->loginGroup->show();

}


