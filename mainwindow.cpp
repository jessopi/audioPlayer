#include "mainwindow.h"
#include <QDebug>
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

    //playlist table creation and properties
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

    //seek slider
    seekSlider =  new QSlider(Qt::Horizontal,this);
    seekSlider->setRange(0,mediaPlayer->duration() / 1000);

    //labels for songName & duration
    currentSongDuration = new QLabel(this);
    currentSongName = new QLabel(this);
    currentSongName->setText("");

    QFont k;
    k.setPixelSize(20);
    currentSongName->setFont(k);


    /******************Connections**********************/


    connect(mediaPlayer, &QMediaPlayer::stateChanged, mediaButtons, &MediaButtons::setState);

    connect(mediaButtons, &MediaButtons::play,[&](){
            mediaPlayer->play();
            updateSongLabel();
            });
    connect(mediaButtons, &MediaButtons::pause,mediaPlayer, &QMediaPlayer::pause);
    connect(mediaButtons, &MediaButtons::stop,[&](){
        mediaPlayer->stop();
        updateSongLabel();
    });
    connect(mediaButtons, &MediaButtons::next,[&](){
        mediaPlaylist->next();
        updateSongLabel();
        playlistTable->selectRow(mediaPlaylist->currentIndex());
    });
    connect(mediaButtons, &MediaButtons::previous,[&](){
                mediaPlaylist->previous();
                updateSongLabel();
                playlistTable->selectRow(mediaPlaylist->currentIndex());
            });
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
       mediaPlaylist->setCurrentIndex(index.row());
       mediaPlayer->play();
       updateSongLabel();
    });

    /*****************Layout creation and arrangement***************/

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(seekSlider);
    hLayout->addWidget(currentSongDuration);

    QBoxLayout *displayLayout = new QHBoxLayout();
    displayLayout->addWidget(playlistTable);

    QBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setMargin(0);
    buttonLayout->addWidget(mediaButtons);
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(currentSongName);

    QBoxLayout *layout = new QVBoxLayout();
    layout->addLayout(buttonLayout);
    layout->addLayout(hLayout);
    layout->addLayout(displayLayout);

    combinedLayout->setLayout(layout);
    setCentralWidget(combinedLayout);
}

MainWindow::~MainWindow()
{}

void MainWindow::updateSongLabel()
{
    if(mediaPlaylist->currentIndex() != -1 && mediaPlayer->state() != QMediaPlayer::StoppedState)
         currentSongName->setText(playlistTable->item(mediaPlaylist->currentIndex(),0)->text());
    else
        currentSongName->setText("");

}

//sets position of song according to where seek slider is moved
void MainWindow::seek(int seconds)
{
    mediaPlayer->setPosition(seconds * 1000);
}

//opens chosen textfile located in Playlist folder and adds contents to mediaplaylist and table
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
    playlistTable->selectRow(mediaPlaylist->currentIndex() + 1);

}

//Opens dialog and saves playlist to playlist folder in UTF-8 format
void MainWindow::savePlaylist()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),"Playlists",tr("Text files (*.txt)"));
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

//Opens dialog to open X amount of media files and add them to playlist & table
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
}

//Removes selected index from mediaPlaylist & table
void MainWindow::removeSong()
{
    int index = playlistTable->currentRow();
    mediaPlaylist->removeMedia(index);
    playlistTable->removeRow(index);
    updateSongLabel();
}

//Removes all songs from mediaPlaylist and table
void MainWindow::clearSong()
{
    mediaPlaylist->clear();
    updateSongLabel();
    //use setrowcount over clear, since clear removes header labels.
    playlistTable->setRowCount(0);
}

//Formats input from seekSlider and parser and returns formatted string.
QString MainWindow::formatIntoTime(quint64 input)
{
    //HH:MM:SS
     QTime currentTime((input / 3600) % 60, (input / 60) % 60,input % 60);
     QString format = "mm:ss";
     QString outputString = currentTime.toString(format);
     return outputString;
}

//Parses metadata of mp3 files only.
//To include other file parsing need to add functions for that specific file.
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
