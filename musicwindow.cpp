#include "musicwindow.h"
#include "ui_musicwindow.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

char existingGenres[10][10]={"Rock", "Indie", "HipHop", "Rap","EDM"};
int existingGenresCount = 5;
int getGenreNo(char*x){
    if(!strcmp(x, "Rock"))
        return 0;
    if(!strcmp(x, "Indie"))
        return 1;
    if(!strcmp(x, "Hip Hop"))
        return 2;
    if(!strcmp(x, "Rap"))
        return 3;
    if(!strcmp(x, "EDM"))
        return 4;
    return 5;
}

MusicWindow::MusicWindow(QWidget *parent,const int& parentDescriptor) :
    QMainWindow(parent),
    ui(new Ui::MusicWindow),
    sd(parentDescriptor)
{
    ui->setupUi(this);
    createTable();
    memset(checks, 0, 20);
    timp = new QTimer;
    connect(timp, SIGNAL(timeout()), this, SLOT(refresh()));
    connect(ui->CheckRock, SIGNAL(stateChanged(int)), this, SLOT(on_checkedRocc(int)));
    connect(ui->CheckRap, SIGNAL(stateChanged(int)), this, SLOT(on_checkedRep(int)));
    connect(ui->CheckIndie, SIGNAL(stateChanged(int)), this, SLOT(on_checkedIndie(int)));
    connect(ui->CheckHip, SIGNAL(stateChanged(int)), this, SLOT(on_checkedHipHaup(int)));
    connect(ui->CheckEDM, SIGNAL(stateChanged(int)), this, SLOT(on_checkedEDM(int)));

    buttonsIndicator = new QSignalMapper(this);
    connect(buttonsIndicator, SIGNAL(mapped(int)), this, SLOT(on_buttonVoteClicked(int)));

    connect(ui->addSongButton, SIGNAL(clicked()), this, SLOT(on_addSongButtonClicked()));
}

void MusicWindow::on_addSongButtonClicked()
{
    if(ui->SongLine->text().isEmpty() || ui->DescriptionLine->text().isEmpty() ||
       ui->ArtistLine->text().isEmpty() || ui->LinkLine->text().isEmpty() ||
       ui->GenreLine->text().isEmpty())
           return;

    bool found = false;

    for(int i = 0; i < existingGenresCount; i++)
    {
        if( !strcmp(ui->GenreLine->text().toStdString().c_str(), existingGenres[i]) )
            found = true;
    }

    if(!found) return;

    write(sd, "addSong", 256);
    write(sd, ui->SongLine->text().toStdString().c_str(), 256);
    write(sd, ui->ArtistLine->text().toStdString().c_str(), 256);
    write(sd, ui->GenreLine->text().toStdString().c_str(), 256);
    write(sd, ui->DescriptionLine->text().toStdString().c_str(), 256);
    write(sd, ui->LinkLine->text().toStdString().c_str(), 256);
}

void MusicWindow::startRefresh()
{

    timp->start(1000);
}

bool comparisonFunction(song first, song second)
{
    if(first.votes > second.votes)
        return true;
    return false;
}

void MusicWindow::refresh(){
    write(sd, "refresh", 256);
    printf("Refresh \n");
    fflush(stdout);
    read(sd, &nrsongs, 4);
    int nrSongsFromSelGenres= 0;
    printf("No songs %d\n", nrsongs);
    fflush(stdout);
    for(int i = 0; i<nrsongs;i++){
        read(sd, mySongs[i].song, 256);
        read(sd, mySongs[i].artist, 256);
        read(sd, mySongs[i].genre, 256);
        read(sd, mySongs[i].description, 256);
        read(sd, mySongs[i].link, 256);
        read(sd, &mySongs[i].votes, 4);
        if(checks[getGenreNo(mySongs[i].genre)])
            nrSongsFromSelGenres++;
   }

    std::sort(mySongs, mySongs + nrsongs, comparisonFunction);

    printf("%d\n", nrSongsFromSelGenres);
    fflush(stdout);
   ui->SongTable->setRowCount(nrSongsFromSelGenres);
   ui->SongTable->clearContents();

   for(int i = 0,  tableRow = 0; i < nrsongs; i++)
   {
       if( checks[getGenreNo(mySongs[i].genre)] )
       {
           QWidget* buttonContainer = new QWidget(this);
           QLayout* buttonContainerLayout = new QHBoxLayout(buttonContainer);

           buttonContainer->setLayout(buttonContainerLayout);

           QPushButton* buttonVote = new QPushButton(buttonContainer);
           buttonVote->setText("Vote Song");

           buttonContainerLayout->addWidget(buttonVote);

           connect(buttonVote, SIGNAL(clicked()), this->buttonsIndicator, SLOT(map()));
           buttonsIndicator->setMapping(buttonVote, tableRow);

           QString votesToString;
           votesToString.setNum(mySongs[i].votes);

           ui->SongTable->setCellWidget(tableRow, 0, buttonContainer);
           ui->SongTable->setItem(tableRow, 1, new QTableWidgetItem(QString(mySongs[i].song)));
           ui->SongTable->setItem(tableRow, 2, new QTableWidgetItem(QString(mySongs[i].artist)));
           ui->SongTable->setItem(tableRow, 3, new QTableWidgetItem(QString(mySongs[i].genre)));
           ui->SongTable->setItem(tableRow, 4, new QTableWidgetItem(QString(mySongs[i].description)));
           ui->SongTable->setItem(tableRow, 5, new QTableWidgetItem(QString(mySongs[i].link)));
           ui->SongTable->setItem(tableRow++, 6, new QTableWidgetItem(votesToString));

       }
   }
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
    write(this->sd, addedSong.artist, 256);

}

void MusicWindow::on_buttonVoteClicked(int whichButton)
{
    write(sd, "vote", 256);
    write(sd, ui->SongTable->item(whichButton, 1)->text().toStdString().c_str(), 256);
    ui->SongTable->item(whichButton, 6)->setText( QString().setNum(ui->SongTable->item(whichButton, 6)->text().toInt()) );
}

void MusicWindow::createTable(){
    QStringList design;
    this->ui->SongTable->setColumnCount(7);
    design <<"Vote"<<"Song"<<"Artist"<<"Genre"<<"Description"<<"Link"<<"Votes";
    this->ui->SongTable->verticalHeader()->setVisible(false);
    this->ui->SongTable->setHorizontalHeaderLabels(design);
    this->ui->SongTable->verticalHeader()->setDefaultSectionSize(50);
    this->ui->SongTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->ui->SongTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->ui->SongTable->setFocusPolicy(Qt::NoFocus);
    this->ui->SongTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    for(int i = 0; i < this->ui->SongTable->columnCount(); i++)
        this->ui->SongTable->setColumnWidth(i, this->ui->SongTable->width()/this->ui->SongTable->columnCount());

}

void MusicWindow::displayTableSongs(){

}

void MusicWindow::on_checkedRocc(int state)
{
    checks[0] = state;
}
void MusicWindow::on_checkedEDM(int state)
{
    checks[4] = state;
}

void MusicWindow::on_checkedRep(int state)
{
    checks[3] = state;
}

void MusicWindow::on_checkedIndie(int state)
{
    checks[1] = state;
}

void MusicWindow::on_checkedHipHaup(int state)
{
    checks[2] = state;
}


















