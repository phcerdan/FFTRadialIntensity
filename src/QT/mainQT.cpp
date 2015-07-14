#include "mainwindow.h"
#include "QApplication"
#include <iostream>
#ifdef ENABLE_R
#include "RInside.h"
#endif
int main (int argc, char **argv)
{
//    Q_INIT_RESOURCE(application); ??
#ifdef ENABLE_R
    RInside R(argc, argv);  		// create an embedded R instance
#endif
    QApplication app(argc, argv);
    app.setApplicationName("FFT-radial Image");
#ifdef ENABLE_R
    MainWindow mainWin(R);
#else
    MainWindow mainWin;
#endif

    mainWin.show();
    app.exec();

}
