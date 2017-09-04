/**
 FFT From Image. Apply to a microscopy image, returning a I-q data set,
 allowing comparisson with Small Angle X-ray Scattering experiments.
 Copyright © 2015 Pablo Hernandez-Cerdan

 This library is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "saxs_sim_functional.h"
// #include "WorkerSim.h"
#include <string>
#include <memory>
#include <QMainWindow>
#include <QVector>
#include <QToolButton>
#include <QString>
#include <QDialog>
#include <itkImageToVTKImageFilter.h>
#include <QVTKWidget.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageViewer.h>
#include <vtkImageMapper3D.h>
#include <vtkImageActor.h>
#include <vtkInteractorStyleImage.h>
// #include <QSvgWidget>

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

    template <typename InputImageType, typename OutputImageType = InputImageType>
    struct Result {
        typename InputImageType::Pointer input;
        typename OutputImageType::Pointer fft;
        radial_intensity::IntensitiesHistoMetaTuple data;
    };

signals:
    void inputFilename();
    void runWorkerSim(std::string, std::string, int, bool );
    void currentSimSwitch(size_t);
private slots:
    void newSim(std::string imgName, std::string outputPlotName, int num_threads = 1, bool saveToFile = 1);
    void createNewDialog();
    void renderInputTypeImage();
    void writeFFTImageToDisk();
    void renderFFTWindowed();
    void ShowContextMenuQVTKFFT(const QPoint& pos);
    void workerSimHasFinished(std::shared_ptr<SAXSsim> inputSim);
    void on_currentSimSwitch(size_t);

private:
    QVector<std::shared_ptr<SAXSsim>> simVector;
    // QVector<vtkSmartPointer<vtkRenderer>> renInputVector;
    // QVector<vtkSmartPointer<vtkRenderer>> renFFTVector;
    // QVector<std::string> svgFileNamesVector;
    QToolButton* simToolButton;
    QMenu* simActiveMenu;
    QHash<int, QAction*> simActionMap;
    SAXSsim* currentSim_;

    void createActions();
    void createToolBars();
    // void createStatusBar();
    void createContextMenus();
    void createSimButton();
    void addSimAction();

    QAction *newSimAct;
    QAction *exitAct;
    QThread *thread_      = 0; // TODO:Use global pool thread?
    WorkerSim *workerSim_ = 0;
    Ui::MainWindow *ui;
    QAction *newPlotAct;
    // QString filterSVGFile(
    //         const std::string & inputSVGFile);
private slots:
    // void createPlotDialog();
    // void drawPlot(
    //         double nm_per_pixel,
    //         std::string format,
    //         std::string outputFile);
    // void ShowContextMenuPlot(const QPoint& pos);
};
#endif /* MAINWINDOW_H_ */
