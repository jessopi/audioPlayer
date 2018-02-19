#include "mainwindow.h"
#include "mediabuttons.h"
#include <QBoxLayout>
#include <QMenuBar>
#include <QWidget>
#include <QToolBar>
#include <QString>
#include <QMessageBox>
#include <QMenuBar>
#include <QFileDialog>

#include <QFile>
#include <QTextStream>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_player = new QMediaPlayer(this);
    m_player->setAudioRole(QAudio::MusicRole);
    m_playlist = new QMediaPlaylist();
    m_player->setPlaylist(m_playlist);
    /*
     * Connect slots to signals here
     */
    QMenu *file;
    QMenu *file1;

    QAction *open = new QAction(tr("Load Playlist"),this);
    connect(open,SIGNAL(triggered()),this,SLOT(loadPlaylist()));

    QAction *save = new QAction(tr("Save Playlist"),this);
    connect(save,SIGNAL(triggered()),this,SLOT(savePlaylist()));

    file = menuBar()->addMenu("Playlist");

    file->addAction(save);
    file->addAction(open);

    QAction *add = new QAction(tr("Add Song"),this);
    connect(add,SIGNAL(triggered()),this,SLOT(addSong()));

    QAction *remove = new QAction(tr("Remove Song"),this);
    connect(remove,SIGNAL(triggered()),this,SLOT(removeSong()));

    QAction *clear = new QAction(tr("Clear Songs"),this);
    connect(clear,SIGNAL(triggered()),this,SLOT(clearSong()));

    file1 = menuBar()->addMenu("Actions");
    file1->addAction(add);
    file1->addAction(remove);
    file1->addAction(clear);


    QWidget *wdg = new QWidget(this);
    MediaButtons *mButtons = new MediaButtons(this);
    songPlaylist = new QListWidget(this);

    m_Slider =  new QSlider(Qt::Horizontal,this);
    m_Slider->setRange(0,m_player->duration() / 1000);

    currentSongDuration = new QLabel(this);
    currentSongDuration->setText("00:00");

    currentSongName = new QLabel(this);
    currentSongName->setText("07 Song of the Ancients ／ Devola.mp3");

    connect(m_player, &QMediaPlayer::stateChanged, mButtons, &MediaButtons::setState);

    connect(mButtons, &MediaButtons::play, m_player, &QMediaPlayer::play);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_Slider);
    hLayout->addWidget(currentSongDuration);

    QBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->addWidget(songPlaylist);

    QBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setMargin(0);
    buttonLayout->addWidget(mButtons);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(currentSongName);
    //buttonLayout->addWidget(toolbar);

    QBoxLayout *layout = new QVBoxLayout;
   // layout->addLayout();
    layout->addLayout(buttonLayout);
    layout->addLayout(hLayout);
    layout->addLayout(displayLayout);

    wdg->setLayout(layout);
    setCentralWidget(wdg);
    //setLayout(layout);
}

MainWindow::~MainWindow()
{

}

void MainWindow::loadPlaylist()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Load File"));
    QFile inputFile(fileName);
    QFileInfo f;
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
        in.setCodec("UTF-8");
       while (!in.atEnd())
       {
          QString line = in.readLine();
          f.setFile(line);

          m_playlist->addMedia(QUrl(f.absoluteFilePath()));
          songPlaylist->addItem(f.fileName());

       }
       inputFile.close();
    }
}

void MainWindow::savePlaylist()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),"playlist.txt",tr("Text files (*.txt)"));
    QFile f(fileName);
    if(f.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&f);
        stream.setCodec("UTF-8");

        for(int i = 0 ; i < m_playlist->mediaCount();i++)
        {
            stream<<m_playlist->media(i).canonicalUrl().path()<<endl;
        }
    }
}

void MainWindow::addSong()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"));
    if(files.empty())
          return;
    for(int i = 0; i < files.count();i++)
    {
        QFileInfo f(files.at(i));
        m_playlist->addMedia(QUrl(f.absoluteFilePath()));
        songPlaylist->addItem(f.fileName());
    }
    int index = songPlaylist->currentIndex().row();
    songPlaylist->setCurrentRow(index);
}

void MainWindow::removeSong()
{
    int index = songPlaylist->currentRow();
    m_playlist->removeMedia(index);
    songPlaylist->takeItem(index);
    //updateSongLabel();
}

void MainWindow::clearSong()
{
    m_playlist->clear();
    songPlaylist->clear();
    //ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    //updateSongLabel();
}
