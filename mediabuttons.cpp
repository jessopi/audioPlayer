#include "mediabuttons.h"
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
    muteButton->setFocusPolicy(Qt::NoFocus);
    volumeSlider = new QSlider(Qt::Horizontal,this);

    QIcon icon2;
    icon2.addFile("Icons/repeat.png");
    repeatButton = new QPushButton(this);
    repeatButton->setIcon(icon2);
    repeatButton->setCheckable(true);

    /*******CONNECTORS******/
    connect(repeatButton,&QAbstractButton::clicked,[&](){
       if(repeatButton->isChecked())
           emit repeatBegin();
       else
           emit repeatEnd();
    });
    connect(previousButton, &QPushButton::clicked,this,&MediaButtons::previous);
    connect(playButton, &QPushButton::clicked,this,&MediaButtons::playClicked);
    connect(stopButton, &QPushButton::clicked,this,&MediaButtons::stop);
    connect(nextButton, &QPushButton::clicked,this,&MediaButtons::next);

    connect(muteButton,&QPushButton::clicked,this,&MediaButtons::muteClicked);

    connect(volumeSlider,&QSlider::valueChanged,[&](){
        emit volumeLevel(volumeSlider->value());
    });

    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(100);

    QBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(previousButton);
    layout->addWidget(playButton);
    layout->addWidget(stopButton);
    layout->addWidget(nextButton);
    layout->addWidget(repeatButton);
    layout->addWidget(muteButton);
    layout->addWidget(volumeSlider);
    setLayout(layout);

    muted = false;
}

//updates playerstate and changes icons accordingly
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

void MediaButtons::repeat()
{
    repeatButton->toggle();
}

//toggles play and paused when clicked and changes icon
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
}
//toggles mute when clicked and changes mute icon
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
