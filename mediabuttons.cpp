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
    connect(playButton, &QAbstractButton::clicked,this,&MediaButtons::playClicked);

    stopButton = new QPushButton(this);
    stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

    nextButton = new QPushButton(this);
    nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

    muteButton = new QPushButton(this);
    muteButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));


    volumeSlider = new QSlider(Qt::Horizontal,this);

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
    currentPlayerState = state;


}

void MediaButtons::playClicked()
{
    if(currentPlayerState == 0 || currentPlayerState  == 1)
    {
        emit play();
    }
    else if(currentPlayerState == 2)
    {
        emit pause();
    }
    else
        return;
}

void MediaButtons::muteClicked()
{

    //emit muteToggle(true);
}

void MediaButtons::volumeSliderChanged()
{
    emit volumeSliderChanged();
}
