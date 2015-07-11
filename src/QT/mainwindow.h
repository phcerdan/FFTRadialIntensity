#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QMainWindow>
#include <../2D/SAXSsim.h>
#include <string>
#include <memory>
#include <QVector>
#include <QString>
#include <QDialog>
#include "newdialog.h"
// #define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeTypeOpenGL,vtkRenderingOpenGL)
// #define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL)
// #include <itkVTKImageExport.h>
#include <itkImageToVTKImageFilter.h>
#include <QVTKWidget.h>
// #include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
// #include "vtkImageViewer.h"
#include "vtkImageMapper3D.h"
#include "vtkImageActor.h"
#include "vtkInteractorStyleImage.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
/// typedefs hard copied from SAXSsim. Image must be unsigned.
    const static unsigned int  Dimension = 2;
    typedef unsigned int       InputPixelType;
    typedef unsigned short     OutputPixelType;
    typedef itk::Image< InputPixelType, Dimension>  InputImageType;
    typedef itk::Image< OutputPixelType, Dimension>   OutputImageType;
    typedef InputImageType::Pointer                 InputTypeP;
    typedef OutputImageType::Pointer                  OutputTypeP;

// itkImageToVTKImageExport
    typedef itk::ImageToVTKImageFilter<InputImageType>     ConnectorInputType;
    typedef itk::ImageToVTKImageFilter<OutputImageType>      ConnectorOutputType;
    typedef ConnectorInputType::Pointer             ConnectorInputPointer;
    typedef ConnectorOutputType::Pointer              ConnectorOutputPointer;
// VTK
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

private slots:
    void newSim(std::string imgName, std::string outputPlotName, int num_threads = 1, bool saveToFile = 1);
    void createNewDialog();
    void renderInputTypeImage();
    void renderOutputTypeImage();

private:
    QVector<std::shared_ptr<SAXSsim>> simVector;
    QVector<vtkSmartPointer<vtkRenderWindow>> renWinVector;
private:
    void createActions();
    void createToolBars();
    void createStatusBar();

    NewDialog *newDialog;
    QToolBar *fileToolBar;
    QAction *newSimAct;
    QAction *exitAct;

private:
    Ui::MainWindow *ui;
};
#endif /* MAINWINDOW_H_ */
