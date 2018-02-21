#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QSlider>
#include "mediabuttons.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
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
    MediaButtons *mediaButtons = nullptr;
};

#endif // MAINWINDOW_H
