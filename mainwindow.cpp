#include "mainwindow.h"
#include <QBoxLayout>
#include <QHeaderView>
#include <QToolBar>
#include <QString>
#include <QMenuBar>
#include <QFileDialog>
#include <QTime>
#include "tfile.h"
#include <QTextStream>
#include <QFont>
#include <QFileInfo>
#include "tag.h"
#include "fileref.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setAudioRole(QAudio::MusicRole);

    mediaPlaylist = new QMediaPlaylist(this);
    mediaPlayer->setPlaylist(mediaPlaylist);
    mediaButtons = new MediaButtons(this);

    combinedLayout = new QWidget(this);

    //Menu Bar creation
    open = new QAction(tr("Load Playlist"),this);
    save = new QAction(tr("Save Playlist"),this);
    add = new QAction(tr("Add Song"),this);
    remove = new QAction(tr("Remove Song"),this);
    clear = new QAction(tr("Clear Songs"),this);

    connect(open,SIGNAL(triggered()),this,SLOT(loadPlaylist()));
    connect(save,SIGNAL(triggered()),this,SLOT(savePlaylist())); 
    connect(add,SIGNAL(triggered()),this,SLOT(addSong()));
    connect(remove,SIGNAL(triggered()),this,SLOT(removeSong()));
    connect(clear,SIGNAL(triggered()),this,SLOT(clearSong()));

    file = menuBar()->addMenu("File");
    file->addAction(save);
    file->addAction(open);

    edit = menuBar()->addMenu("Edit");
    edit->addAction(add);
    edit->addAction(remove);
    edit->addAction(clear);
    //Menu Bar creation end


    playlistTable = new QTableWidget(0,4,this);
    QStringList labels;
    labels << tr("Title") << tr("Artist") <<tr("Album") <<tr("Length");
    playlistTable->setHorizontalHeaderLabels(labels);
    playlistTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    playlistTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
    playlistTable->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);
    playlistTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    playlistTable->setSelectionMode(QAbstractItemView::SingleSelection);
    playlistTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    playlistTable->verticalHeader()->hide();

    seekSlider =  new QSlider(Qt::Horizontal,this);
    seekSlider->setRange(0,mediaPlayer->duration() / 1000);

    currentSongDuration = new QLabel(this);
    currentSongName = new QLabel(this);
    currentSongName->setText("");
    QFont *k = new QFont();
    k->setPixelSize(20);
    currentSongName->setFont(*k);


    //CONNECTORS
    connect(mediaPlayer, &QMediaPlayer::stateChanged, mediaButtons, &MediaButtons::setState);
    connect(mediaPlaylist,&QMediaPlaylist::currentIndexChanged,[&](int pos){
        (mediaPlaylist->currentIndex() != -1)?  currentSongName->setText(playlistTable->item(pos,0)->text()) :  currentSongName->setText("");
    });

    connect(mediaButtons, &MediaButtons::play,[&](){
        if(mediaPlaylist->currentIndex() == playlistTable->currentIndex().row())
            mediaPlayer->play();
        else
        {
            mediaPlaylist->setCurrentIndex(playlistTable->currentIndex().row());
            mediaPlayer->play();
        }

            });
    connect(mediaButtons, &MediaButtons::pause,mediaPlayer, &QMediaPlayer::pause);
    connect(mediaButtons, &MediaButtons::stop,[&](){
        //mediaPlayer->stop();
        currentSongName->setText("");
    });
    connect(mediaButtons, &MediaButtons::next,mediaPlaylist, &QMediaPlaylist::next);
    connect(mediaButtons, &MediaButtons::previous,mediaPlaylist, &QMediaPlaylist::previous);
    connect(mediaButtons,&MediaButtons::muteToggle,mediaPlayer,&QMediaPlayer::setMuted);
    connect(mediaButtons,&MediaButtons::volumeLevel,mediaPlayer,&QMediaPlayer::setVolume);
    connect(seekSlider,&QSlider::sliderMoved,this,&MainWindow::seek);
    connect(mediaPlayer,&QMediaPlayer::positionChanged,[&](quint64 pos){
        if(!seekSlider->isSliderDown())
            seekSlider->setValue(pos/1000);

        qint64 currentInfo = pos/1000;  
        currentSongDuration->setText(formatIntoTime(currentInfo));
    });
    connect(mediaPlayer,&QMediaPlayer::durationChanged,[&](quint64 dur){
        seekSlider->setMaximum(dur/1000);
    });
    connect(playlistTable,&QTableWidget::doubleClicked,[&](const QModelIndex &index){
        int t = index.row();
       mediaPlaylist->setCurrentIndex(t);
       mediaPlayer->play();
    });
    //
    //LAYOUT ARRANGEMENT
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(seekSlider);
    hLayout->addWidget(currentSongDuration);

    QBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->addWidget(playlistTable);

    QBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setMargin(0);
    buttonLayout->addWidget(mediaButtons);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(currentSongName);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(buttonLayout);
    layout->addLayout(hLayout);
    layout->addLayout(displayLayout);

    combinedLayout->setLayout(layout);
    setCentralWidget(combinedLayout);
}

MainWindow::~MainWindow()
{}

void MainWindow::seek(int seconds)
{
    mediaPlayer->setPosition(seconds * 1000);
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
          mediaPlaylist->addMedia(QUrl(f.absoluteFilePath()));
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

        for(int i = 0 ; i < mediaPlaylist->mediaCount();i++)
        {
            stream<<mediaPlaylist->media(i).canonicalUrl().path()<<endl;
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
        mediaPlaylist->addMedia(QUrl(f.absoluteFilePath()));
        parseMetadata(files.at(i));
    }
    //int index = playlistTable->currentIndex().row();
    //playlistTable->setCurrentRow(index);
}

void MainWindow::removeSong()
{
    int index = playlistTable->currentRow();
    mediaPlaylist->removeMedia(index);
    playlistTable->removeRow(index);
    //updateSongLabel();
}

void MainWindow::clearSong()
{
    mediaPlaylist->clear();
    playlistTable->setRowCount(0);
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
    playlistTable->insertRow(playlistTable->rowCount());
    TagLib::FileRef fileRef( reinterpret_cast<const wchar_t*>(s.constData()) );

    playlistTable->setItem(playlistTable->rowCount()-1,0,new QTableWidgetItem(fileRef.tag()->title().toCString()));
    playlistTable->setItem(playlistTable->rowCount()-1,1,new QTableWidgetItem(fileRef.tag()->artist().toCString()));
    playlistTable->setItem(playlistTable->rowCount()-1,2,new QTableWidgetItem(fileRef.tag()->album().toCString()));
    qint64 seconds = fileRef.audioProperties()->lengthInSeconds();
    playlistTable->setItem(playlistTable->rowCount()-1,3,new QTableWidgetItem(formatIntoTime(seconds)));
}
