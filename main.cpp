#include "mainwindow.h"
#include "DarkStyle.h"
#include "framelesswindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle(new DarkStyle);
    FramelessWindow frameless;

    frameless.setWindowTitle("audioPlayer");
    MainWindow *w = new MainWindow;
    frameless.setContent(w);
    frameless.show();

    return a.exec();
}
