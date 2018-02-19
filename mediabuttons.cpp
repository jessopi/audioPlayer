#include "mediabuttons.h"
#include <QPushButton>
#include <QBoxLayout>
#include <QSlider>
#include <QStyle>
#include <QToolButton>
#include <QComboBox>
#include <QAudio>

MediaButtons::MediaButtons(QWidget *parent) : QWidget(parent)
{
    previousButton = new QPushButton(this);
    previousButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));

    playButton = new QPushButton(this);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));



    stopButton = new QPushButton(this);
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

    nextButton = new QPushButton(this);
    nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

    muteButton = new QPushButton(this);
    muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));


    volumeSlider = new QSlider(Qt::Horizontal,this);

    connect(previousButton, &QAbstractButton::clicked,this,&MediaButtons::previous);
    connect(playButton, &QAbstractButton::clicked,this,&MediaButtons::playClicked);
    connect(stopButton, &QAbstractButton::clicked,this,&MediaButtons::stop);
    connect(nextButton, &QAbstractButton::clicked,this,&MediaButtons::next);

    connect(muteButton,&QAbstractButton::clicked,this,&MediaButtons::muteClicked);

    connect(volumeSlider,&QAbstractSlider::valueChanged,this,&MediaButtons::volumeSliderChanged);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(100);

    QBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(previousButton);
    layout->addWidget(playButton);
    layout->addWidget(stopButton);
    layout->addWidget(nextButton);
    layout->addWidget(muteButton);
    layout->addWidget(volumeSlider);
    setLayout(layout);

    muted = false;
}
void MediaButtons::setState(QMediaPlayer::State state)
{
    if(currentPlayerState == state)
        return;
    else
    {
        currentPlayerState = state;

        if(currentPlayerState == QMediaPlayer::StoppedState	)
        {
            stopButton->setEnabled(false);
            playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        }
        else if(currentPlayerState == QMediaPlayer::PlayingState)
        {
            stopButton->setEnabled(true);
            playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        }
        else if(currentPlayerState == QMediaPlayer::PausedState)
        {
            stopButton->setEnabled(true);
            playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        }
    }
}

void MediaButtons::playClicked()
{
    if(currentPlayerState == QMediaPlayer::PausedState || currentPlayerState  == QMediaPlayer::StoppedState)
    {
        emit play();
    }
    else if(currentPlayerState == QMediaPlayer::PlayingState)
    {
        emit pause();
    }
    else
        return;
}

void MediaButtons::muteClicked()
{
    if(muted == false)
    {
        muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolumeMuted));
        muted = true;
        emit muteToggle(true);
    }
    else
    {
        muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
        muted = false;
        emit muteToggle(false);
    }
}

void MediaButtons::volumeSliderChanged()
{
    int vol = volumeSlider->value();
    emit volumeLevel(vol);
}
