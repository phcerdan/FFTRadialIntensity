#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include <../2D/SAXSsim.h>
#include <string>
#include <memory>
#include <QVector>
#include <QString>
#include <QDialog>
#include "Q_DebugStream.h"
#include "newdialog.h"
#include <itkImageToVTKImageFilter.h>
#include <QVTKWidget.h>
#include "vtkSmartPointer.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include "vtkImageViewer.h"
#include "vtkImageMapper3D.h"
#include "vtkImageActor.h"
#include "vtkInteractorStyleImage.h"
#include <WorkerSim.h>

#ifdef R_ENABLED
#include "RInside.h"
#endif

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
/// typedefs hard copied from SAXSsim. Input Image must be unsigned.
    const static unsigned int  Dimension = 2;
    typedef unsigned int       InputPixelType;
    typedef unsigned short     OutputPixelType;
    typedef double             RealPixelType;
    typedef itk::Image< InputPixelType, Dimension>  InputImageType;
    typedef InputImageType::Pointer                 InputTypeP;
    typedef itk::Image< OutputPixelType, Dimension> OutputImageType;
    typedef OutputImageType::Pointer                OutputTypeP;
    typedef itk::Image< RealPixelType, Dimension>   RealImageType;
    typedef RealImageType::Pointer                  RealTypeP;

// itkImageToVTKImageExport
    typedef itk::ImageToVTKImageFilter<InputImageType>  ConnectorInputType;
    typedef ConnectorInputType::Pointer                 ConnectorInputPointer;
    typedef itk::ImageToVTKImageFilter<OutputImageType> ConnectorOutputType;
    typedef ConnectorOutputType::Pointer                ConnectorOutputPointer;
    typedef itk::ImageToVTKImageFilter<RealImageType>   ConnectorRealType;
    typedef ConnectorRealType::Pointer                  ConnectorRealPointer;
// VTK
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    RealTypeP m_fftVisualizationReal;
    OutputTypeP m_fftVisualizationOutput;

signals:
    void runWorkerSim(std::string, std::string, int, bool );
    void runWorkerSimWithMessenger(std::string, std::string, int, bool, QPlainTextEdit* );
private slots:
    void newSim(std::string imgName, std::string outputPlotName, int num_threads = 1, bool saveToFile = 1);
    void createNewDialog();
    void renderInputTypeImage();
    void writeFFTImageToDisk();
    void renderFFTWindowed();
    void ShowContextMenu2(const QPoint& pos);
    void workerSimHasFinished(std::shared_ptr<SAXSsim> inputSim);

private:
    QVector<std::shared_ptr<SAXSsim>> simVector;
    QVector<vtkSmartPointer<vtkRenderWindow>> renWinVector;
private:
    void createActions();
    void createToolBars();
    void createStatusBar();
    void createContextMenus();

    NewDialog *newDialog;
    QToolBar *fileToolBar;
    QAction *newSimAct;
    QAction *exitAct;
    Q_DebugStream *m_debugStream;
    QThread *thread_      = 0;
    WorkerSim *workerSim_ = 0;
private:
    Ui::MainWindow *ui;
};
#endif /* MAINWINDOW_H_ */
