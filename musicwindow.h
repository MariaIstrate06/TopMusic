#ifndef MUSICWINDOW_H
#define MUSICWINDOW_H

#include <QMainWindow>

namespace Ui {
class MusicWindow;
}

class MusicWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MusicWindow(QWidget *parent = nullptr,const int& = 0);
    ~MusicWindow();

private slots:
    void pushAddSong();
private:
    Ui::MusicWindow *ui;
    int sd;
};

#endif // MUSICWINDOW_H
