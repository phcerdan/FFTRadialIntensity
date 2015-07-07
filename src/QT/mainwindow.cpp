/*
 * MainWindow.cpp
 *
 *  Created on: 25/06/2014
 *      Author: phc
 */
#include <QtWidgets>
//#include "visualization_vtkGraph.hpp"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVector>
using namespace std;

// template<typename TInputImage>
// typename itk::VTKImageExport<TInputImage>::Pointer ITKtoVTKConnector(const TInputImage* itkImg)
// {
//     typedef itk::VTKImageExport<TInputImage> ConnectorType;
//     typename itk::VTKImageExport<TInputImage>::Pointer connector =
//         ConnectorType::New();
//     connector->SetInput(itkImg);
//     return connector;
// }

void MainWindow::createNewDialog()
{

    newDialog = new NewDialog(this);
    newDialog->show();
    connect(newDialog, &NewDialog::newSimFromDialog, this, &MainWindow::newSim);
}
void MainWindow::newSim(string imgName, string outputPlotName, int num_threads, bool saveToFile)
{

    simVector.push_back(
            make_shared<SAXSsim>(imgName, outputPlotName,
                num_threads, saveToFile)
            );
    renderInputTypeImage();
    renderOutputTypeImage();

}
void MainWindow::renderInputTypeImage()
{

    auto &sim = simVector.back();
    auto connector   = ConnectorInputType::New();
    connector->SetInput(sim->inputImg_);
    connector->Update();
    auto actor = vtkSmartPointer<vtkImageActor>::New();
    actor->GetMapper()->SetInputData(connector->GetOutput());
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renWinVector.push_back(vtkSmartPointer<vtkRenderWindow>::New());
    auto &renWin = renWinVector.back();
    renWin->AddRenderer(renderer);

    renWin->SetInteractor(ui->qvtkWidget->GetInteractor());
    ui->qvtkWidget->SetRenderWindow(renderer->GetRenderWindow());
    auto style = vtkSmartPointer<vtkInteractorStyle>::New();
    renWin->GetInteractor()->SetInteractorStyle(style);
    renderer->Render();

}

void MainWindow::renderOutputTypeImage()
{
    auto &sim = simVector.back();
    auto connector   = ConnectorOutputType::New();
    connector->SetInput(sim->RescaleFFTModulus(sim->fftImg_));
    connector->Update();
    auto actor = vtkSmartPointer<vtkImageActor>::New();
    actor->GetMapper()->SetInputData(connector->GetOutput());
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renWinVector.push_back(vtkSmartPointer<vtkRenderWindow>::New());
    auto &renWin = renWinVector.back();
    renWin->AddRenderer(renderer);

    renWin->SetInteractor(ui->qvtkWidget_2->GetInteractor());
    ui->qvtkWidget_2->SetRenderWindow(renderer->GetRenderWindow());
    auto style = vtkSmartPointer<vtkInteractorStyle>::New();
    renWin->GetInteractor()->SetInteractorStyle(style);
    renderer->Render();

}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createActions();
    createToolBars();
    createStatusBar();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete newDialog;
}

void MainWindow::createActions()
{
    newSimAct = new QAction(QIcon(":/resources/open.png"), tr("&Create a new FFT From Image."), this);
    newSimAct->setShortcuts(QKeySequence::Open);
    newSimAct->setStatusTip(tr("Create a new FFT From Image"));
    connect(newSimAct, SIGNAL(triggered()), this, SLOT(createNewDialog()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}
void MainWindow::createToolBars()
{
    ui->toolBar->addAction(newSimAct);
}

void MainWindow::createStatusBar()
{
    ui->statusbar->showMessage(tr("Ready"));
}


