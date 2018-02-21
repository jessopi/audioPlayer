#ifndef MEDIABUTTONS_H
#define MEDIABUTTONS_H

#include <QWidget>
#include <QMediaPlayer>
#include <QSlider>
#include <QPushButton>
#include <QBoxLayout>
#include <QStyle>

class MediaButtons : public QWidget
{
    Q_OBJECT
public:
    explicit MediaButtons(QWidget *parent = nullptr);
public slots:
    void setState(QMediaPlayer::State state);
signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void muteToggle(bool toggle);
    void volumeLevel(int vol);
public slots:
    void playClicked();
    void muteClicked();

private:
    bool muted;
    QPushButton *playButton = nullptr;
    QPushButton *stopButton = nullptr;
    QPushButton *previousButton = nullptr;
    QPushButton *nextButton = nullptr;
    QPushButton *muteButton = nullptr;
    QSlider *volumeSlider = nullptr;


    QMediaPlayer::State currentPlayerState = QMediaPlayer::StoppedState;
};

#endif // MEDIABUTTONS_H
