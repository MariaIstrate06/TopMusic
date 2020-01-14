#ifndef MUSICWINDOW_H
#define MUSICWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSignalMapper>
struct song{
    char artist[256];
    char song[256];
    char genre[256];
    char description[256];
    char link[256];
    int votes;
};
namespace Ui {
class MusicWindow;
}

class MusicWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MusicWindow(QWidget *parent = nullptr,const int& = 0);
    ~MusicWindow();
    void createTable();
    void displayTableSongs();

private slots:
    void startRefresh();
    void pushAddSong();
    void refresh();
    void on_checkedRocc(int);
    void on_checkedRep(int);
    void on_checkedHipHaup(int);
    void on_checkedEDM(int);
    void on_checkedIndie(int);
    void on_buttonVoteClicked(int);
    void on_addSongButtonClicked();
private:
    QTimer *timp ;
    Ui::MusicWindow *ui;
    int sd;
    int nrsongs{0};
    song mySongs[100];
    int checks[5];
    QSignalMapper* buttonsIndicator;
};

#endif // MUSICWINDOW_H
