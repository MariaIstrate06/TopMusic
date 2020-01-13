#include "musicwindow.h"
#include "ui_musicwindow.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
struct song{
    char artist[50];
    char song[50];
    char genre[50];
    char description[50];
    char link[50];
};
MusicWindow::MusicWindow(QWidget *parent,const int& parentDescriptor) :
    QMainWindow(parent),
    ui(new Ui::MusicWindow),
    sd(parentDescriptor)
{
    ui->setupUi(this);
}

MusicWindow::~MusicWindow()
{
    delete ui;
}

void MusicWindow::pushAddSong(){

    song addedSong;
    memset(addedSong.artist,0,sizeof(addedSong.artist));
    memset(addedSong.song,0,sizeof(addedSong.song));
    memset(addedSong.genre,0,sizeof(addedSong.genre));
    memset(addedSong.description,0,sizeof(addedSong.description));
    memset(addedSong.link,0,sizeof(addedSong.link));

    strcpy(addedSong.artist, ui->ArtistLine->text().toStdString().c_str());
    strcpy(addedSong.song, ui->SongLine->text().toStdString().c_str());
    strcpy(addedSong.genre, ui->GenreLine->text().toStdString().c_str());
    strcpy(addedSong.description, ui->DescriptionLine->text().toStdString().c_str());
    strcpy(addedSong.link, ui->LinkLine->text().toStdString().c_str());
    fflush(stdout);
    printf("%s",addedSong.artist);
    MainWindow aux;
    write(this->sd, addedSong.artist, 256);
}
