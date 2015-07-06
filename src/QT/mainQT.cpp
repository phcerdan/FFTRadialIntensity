#include "mainwindow.h"
#include "QApplication"
#include <iostream>
int main (int argc, char **argv)
{
//    Q_INIT_RESOURCE(application); ??

    QApplication app(argc, argv);
    app.setApplicationName("FFT-radial Image");
    MainWindow mainWin;

    mainWin.show();
    app.exec();

}
