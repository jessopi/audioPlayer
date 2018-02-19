#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QLabel>
#include <QSlider>
#include <QListWidget>

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
    void seek(int seconds);
    QMediaPlayer *m_player = nullptr;
    QMediaPlaylist *m_playlist = nullptr;

    QLabel *currentSongName = nullptr;

    QLabel *currentSongDuration = nullptr;
    QSlider *m_Slider = nullptr;
    QListWidget *songPlaylist = nullptr;
};

#endif // MAINWINDOW_H
