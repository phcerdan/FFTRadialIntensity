#include "mainwindow.h"
#include "QApplication"
#include <iostream>
int main (int argc, char **argv)
{
//    Q_INIT_RESOURCE(application); ??
#ifdef R_ENABLED
    RInside R(argc, argv);  		// create an embedded R instance
#endif
    QApplication app(argc, argv);
    app.setApplicationName("FFT-radial Image");
#ifdef R_ENABLED
    MainWindow mainWin(R);
#else
    MainWindow mainWin;
#endif

    mainWin.show();
    app.exec();

}
