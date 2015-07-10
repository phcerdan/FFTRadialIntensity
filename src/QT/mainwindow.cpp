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

void MainWindow::createNewDialog()
{

    newDialog = new NewDialog();
    connect(newDialog, &NewDialog::newSimFromDialog, newDialog, &NewDialog::hide);
    connect(newDialog, &NewDialog::newSimFromDialog, this, &MainWindow::newSim);
    newDialog->exec();
    delete newDialog;
}
void MainWindow::newSim(string imgName, string outputPlotName, int num_threads, bool saveToFile)
{
    // newDialog->hide();

    m_debugStream = new Q_DebugStream(std::cout, ui->textEdit); //Redirect Console output to QTextEdit
    try {
        simVector.push_back(make_shared<SAXSsim>());
        auto& sim = simVector.back();
        sim->SetInputParameters(imgName, outputPlotName, num_threads, saveToFile);
        sim->SetQDebugStream(m_debugStream);
        sim->Initialize();

        ui->qvtkWidget->show();
        ui->qvtkWidget_2->show();
        simVector.back()->ScaleForVisualization();
        renderInputTypeImage();
        renderFFTWindowed();
    } catch(std::exception &e){
        std::cout << e.what() << std::endl;
    }


}
void MainWindow::createContextMenus()
{
    ui->qvtkWidget_2->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->qvtkWidget_2, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenu2(const QPoint&)));
}
void MainWindow::ShowContextMenu2(const QPoint& pos)
{
    QPoint globalPos =ui->qvtkWidget_2->mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    QMenu myMenu;
    myMenu.addAction("Original (Windowed)", this, SLOT(renderFFTWindowed()));
    myMenu.addAction("Write this log(FFTModulus) to disk", this, SLOT(writeFFTImageToDisk()));
    myMenu.exec(globalPos);
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
    auto style = vtkSmartPointer<vtkInteractorStyle>::New();
    renWin->GetInteractor()->SetInteractorStyle(style);
    ui->qvtkWidget->SetRenderWindow(renderer->GetRenderWindow());
    renderer->Render();

}
void MainWindow::writeFFTImageToDisk()
{

    // setDefaultSuffix is not working with getSaveFileName shortcut:
    // QString fileName = fileDialog->getSaveFileName(this,tr("Save FFT"), QDir::currentPath(), filter);
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    QString filter = tr("Float supported(*.tiff)");
    fileDialog->setNameFilter(filter);
    fileDialog->setDirectory(QDir::currentPath());
    fileDialog->setDefaultSuffix("tiff");
    fileDialog->setWindowTitle(tr("Save FFT as:"));
    fileDialog->exec();
    QString fileName = fileDialog->selectedFiles().first();
    delete fileDialog;
    auto &sim = simVector.back();
    try {
        sim->WriteFFT(sim->fftModulusSquare_, fileName.toStdString());
    }
    catch (std::exception & e){

        std::cout << e.what() << std::endl;
    }

}
void MainWindow::renderFFTWindowed()
{
    auto &sim = simVector.back();
    auto connector   = ConnectorRealType::New();
    m_fftVisualizationReal  =
        sim->WindowingFFT(
                sim->fftVisualization_, sim->intensitiesVisualization_[1], 255
                );
    connector->SetInput(m_fftVisualizationReal);
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
    auto style = vtkSmartPointer<vtkInteractorStyle>::New();
    renWin->GetInteractor()->SetInteractorStyle(style);
    ui->qvtkWidget_2->SetRenderWindow(renderer->GetRenderWindow());
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
    createContextMenus();
    ui->qvtkWidget->hide();
    ui->qvtkWidget_2->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete newDialog;
    delete m_debugStream;
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


