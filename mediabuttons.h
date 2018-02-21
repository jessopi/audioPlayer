#ifndef MEDIABUTTONS_H
#define MEDIABUTTONS_H

#include <QWidget>
#include <QMediaPlayer>
#include <QAbstractButton>
#include <QAbstractSlider>

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
    void volumeSliderChanged();

private:
    bool muted;
    QAbstractButton *playButton = nullptr;
    QAbstractButton *stopButton = nullptr;
    QAbstractButton *previousButton = nullptr;
    QAbstractButton *nextButton = nullptr;
    QAbstractButton *muteButton = nullptr;
    QAbstractSlider *volumeSlider = nullptr;


    QMediaPlayer::State currentPlayerState = QMediaPlayer::StoppedState;
};

#endif // MEDIABUTTONS_H
