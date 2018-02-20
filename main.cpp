#include "mainwindow.h"
#include "DarkStyle.h"
#include "framelesswindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyle(new DarkStyle);
    FramelessWindow frameless;

    frameless.setWindowTitle("musicPlayer");
   // frameless.setWindowState(Qt::WindowMaximized);
    MainWindow *w = new MainWindow;
    frameless.setContent(w);
    frameless.show();





    //MainWindow w;
   // w.show();

    return a.exec();
}
