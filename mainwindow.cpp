#include "mainwindow.h"
#include "mediabuttons.h"
#include <QBoxLayout>
#include <QMenuBar>
#include <QWidget>
#include <QHeaderView>
#include <QToolBar>
#include <QString>
#include <QMessageBox>
#include <QMenuBar>
#include <QFileDialog>
#include <QTime>
#include "tfile.h"
#include <QTextStream>
#include <QFileInfo>
#include "tag.h"
#include "fileref.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_player = new QMediaPlayer(this);
    m_player->setAudioRole(QAudio::MusicRole);
    m_playlist = new QMediaPlaylist();
    m_player->setPlaylist(m_playlist);

    QMenu *file;
    QMenu *file1;

    QAction *open = new QAction(tr("Load Playlist"),this);
    connect(open,SIGNAL(triggered()),this,SLOT(loadPlaylist()));

    QAction *save = new QAction(tr("Save Playlist"),this);
    connect(save,SIGNAL(triggered()),this,SLOT(savePlaylist()));

    file = menuBar()->addMenu("File");

    file->addAction(save);
    file->addAction(open);

    QAction *add = new QAction(tr("Add Song"),this);
    connect(add,SIGNAL(triggered()),this,SLOT(addSong()));

    QAction *remove = new QAction(tr("Remove Song"),this);
    connect(remove,SIGNAL(triggered()),this,SLOT(removeSong()));

    QAction *clear = new QAction(tr("Clear Songs"),this);
    connect(clear,SIGNAL(triggered()),this,SLOT(clearSong()));

    file1 = menuBar()->addMenu("Edit");
    file1->addAction(add);
    file1->addAction(remove);
    file1->addAction(clear);

    QWidget *wdg = new QWidget(this);
    MediaButtons *mButtons = new MediaButtons(this);

    songPlaylist = new QTableWidget(0,4,this);
    QStringList labels;
    labels << tr("Title") << tr("Artist") <<tr("Album") <<tr("Length");
    songPlaylist->setHorizontalHeaderLabels(labels);
    songPlaylist->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    songPlaylist->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    songPlaylist->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    songPlaylist->setEditTriggers(QAbstractItemView::NoEditTriggers);
    songPlaylist->setSelectionMode(QAbstractItemView::SingleSelection);
    songPlaylist->setSelectionBehavior(QAbstractItemView::SelectRows);
    songPlaylist->verticalHeader()->hide();

    m_Slider =  new QSlider(Qt::Horizontal,this);
    m_Slider->setRange(0,m_player->duration() / 1000);

    currentSongDuration = new QLabel(this);
    currentSongName = new QLabel(this);
    currentSongName->setText("");

    connect(m_player, &QMediaPlayer::stateChanged, mButtons, &MediaButtons::setState);
    connect(m_playlist,&QMediaPlaylist::currentIndexChanged,[&](int pos){
        (m_playlist->currentIndex() != -1)?  currentSongName->setText(songPlaylist->item(pos,0)->text()) :  currentSongName->setText("");
    });
    connect(mButtons, &MediaButtons::play,[&](){
                    m_playlist->setCurrentIndex(songPlaylist->currentIndex().row());
                    m_player->play();
            });
    connect(mButtons, &MediaButtons::pause,m_player, &QMediaPlayer::pause);
    connect(mButtons, &MediaButtons::stop,[&](){
        m_player->stop();
        currentSongName->setText("");
    });
    connect(mButtons, &MediaButtons::next,m_playlist, &QMediaPlaylist::next);
    connect(mButtons, &MediaButtons::previous,m_playlist, &QMediaPlaylist::previous);
    connect(mButtons,&MediaButtons::muteToggle,m_player,&QMediaPlayer::setMuted);
    connect(mButtons,&MediaButtons::volumeLevel,m_player,&QMediaPlayer::setVolume);
    connect(m_Slider,&QSlider::sliderMoved,this,&MainWindow::seek);
    connect(m_player,&QMediaPlayer::positionChanged,[&](quint64 pos){
        if(!m_Slider->isSliderDown())
            m_Slider->setValue(pos/1000);

        qint64 currentInfo = pos/1000;  
        currentSongDuration->setText(formatIntoTime(currentInfo));
    });
    connect(m_player,&QMediaPlayer::durationChanged,[&](quint64 dur){
        m_Slider->setMaximum(dur/1000);
    });

    connect(songPlaylist,&QTableWidget::doubleClicked,[&](const QModelIndex &index){
        int t = index.row();
       m_playlist->setCurrentIndex(t);
       m_player->play();
    });

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

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(buttonLayout);
    layout->addLayout(hLayout);
    layout->addLayout(displayLayout);

    wdg->setLayout(layout);
    setCentralWidget(wdg);
}

MainWindow::~MainWindow()
{}

void MainWindow::seek(int seconds)
{
    m_player->setPosition(seconds * 1000);
}

void MainWindow::loadPlaylist()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Load File"),"Playlists",tr("Text files (*.txt)"));
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
          parseMetadata(line);
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
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),"",tr("Audio files (*.mp3)"));
    if(files.empty())
          return;
    for(int i = 0; i < files.count();i++)
    {
        QFileInfo f(files.at(i));
        m_playlist->addMedia(QUrl(f.absoluteFilePath()));
        parseMetadata(files.at(i));
    }
    //int index = songPlaylist->currentIndex().row();
    //songPlaylist->setCurrentRow(index);
}

void MainWindow::removeSong()
{
    int index = songPlaylist->currentRow();
    m_playlist->removeMedia(index);
    songPlaylist->removeRow(index);
    //updateSongLabel();
}

void MainWindow::clearSong()
{
    m_playlist->clear();
    //songPlaylist->clear(); might be unneeded also clears headers which is a problem
    songPlaylist->setRowCount(0);
    currentSongName->setText("");
    //ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));

}

QString MainWindow::formatIntoTime(quint64 input)
{
    //HH:MM:SS
     QTime currentTime((input / 3600) % 60, (input / 60) % 60,input % 60);
     QString format = "mm:ss";
     QString outputString = currentTime.toString(format);
     return outputString;
}

void MainWindow::parseMetadata(QString s)
{
    songPlaylist->insertRow(songPlaylist->rowCount());
    TagLib::FileRef fileRef( reinterpret_cast<const wchar_t*>(s.constData()) );

    songPlaylist->setItem(songPlaylist->rowCount()-1,0,new QTableWidgetItem(fileRef.tag()->title().toCString()));
    songPlaylist->setItem(songPlaylist->rowCount()-1,1,new QTableWidgetItem(fileRef.tag()->artist().toCString()));
    songPlaylist->setItem(songPlaylist->rowCount()-1,2,new QTableWidgetItem(fileRef.tag()->album().toCString()));
    qint64 seconds = fileRef.audioProperties()->lengthInSeconds();
    songPlaylist->setItem(songPlaylist->rowCount()-1,3,new QTableWidgetItem(formatIntoTime(seconds)));
}
