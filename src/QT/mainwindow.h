#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include <../2D/SAXSsim.h>
#include <itkVTKImageExport.h>
#include <string>
#include <QString>
template<typename TInputImage>
itk::VTKImageExport<TInputImage>::Pointer ITKToVTKConnector(const TInputImage* itkImg);

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

private slots:
    void newSim(QString imgName, QString outputPlotName, int num_threads = 1, bool saveToFile = 1);
    void createNewDialog();

private:
    SAXSsim sim;
    QString inputImg_;
    QString outputPlotFile_;
    int num_threads_ = 1;
private:
    void createActions();
    void createToolBars();
    void createStatusBar();

    QDialog *newDialog;
    QToolBar *fileToolBar;
    QAction *newSimAct;
    QAction *exitAct;
private:
    Ui::MainWindow *ui;
};
#endif /* MAINWINDOW_H_ */
