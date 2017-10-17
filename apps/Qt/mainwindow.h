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
#include <QDialog>
#include <QFutureWatcher>
#include <QMainWindow>
#include <QString>
#include <QToolButton>
#include <QVTKWidget.h>
#include <QVector>
#include <itkImageToVTKImageFilter.h>
#include <memory>
#include <string>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkImageViewer.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

  private:
    QToolButton *simToolButton;
    QMenu *simActiveMenu;
    QHash<int, QAction *> simActionMap;
    QFutureWatcher<void> fftWatcher;

    void createActions();
    void createToolBars();
    void createStatusBar();
    // void createContextMenus();

    QAction *openAct;
    QAction *exitAct;
    QThread *thread_ = 0; // TODO:Use global pool thread?
    Ui::MainWindow *ui;
  private slots:
    void createNewDialog();
    void newSim(std::string imgName);
};
#endif /* MAINWINDOW_H_ */
