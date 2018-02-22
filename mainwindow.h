#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QLabel>
#include <QTableWidget>
#include <QSlider>
#include <QBoxLayout>
#include <QHeaderView>

#include <QMenuBar>
#include <QFileDialog>
#include <QTime>
#include <QFont>
#include <QFileInfo>

#include "mediabuttons.h"
#include "tag.h"
#include "fileref.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void updateSongLabel();
    void loadPlaylist();
    void savePlaylist();
    void addSong();
    void removeSong();
    void clearSong();
private:
    QString formatIntoTime(quint64);
    void parseMetadata(QString s);
    void seek(int seconds);

    QMediaPlayer *mediaPlayer = nullptr;
    QMediaPlaylist *mediaPlaylist = nullptr;
    MediaButtons *mediaButtons = nullptr;

    QLabel *currentSongName = nullptr;
    QLabel *currentSongDuration = nullptr;

    QSlider *seekSlider = nullptr;

    QMenu *file = nullptr;
    QMenu *edit = nullptr;

    QTableWidget *playlistTable = nullptr;

    QAction *open = nullptr;
    QAction *save = nullptr;
    QAction *add = nullptr;
    QAction *remove = nullptr;
    QAction *clear = nullptr;

    QWidget *combinedLayout = nullptr;
};

#endif // MAINWINDOW_H
