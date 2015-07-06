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

template<typename TInputImage>
itk::VTKImageExport<TInputImage>::Pointer ITKtoVTKConnector(const TInputImage* itkImg)
{
    typedef itk::VTKImageExport<TInputImage> ConnectorType;
    itk::VTKImageExport<TInputImage>::Pointer connector =
        ConnectorType::New();
    connector->SetInput(itkImg);
    return connector;
}

void MainWindow::createNewDialog()
{

    newDialog = new QDialog;
    newDialog->setWindowTitle("New FFT From Image");
    bool ok;

    QString text = QInputDialog::getText(  this,  tr("Input 2DImage"),
            tr("Select an Image"),
            QLineEdit::Normal, "", &ok );
    if( ok && !text.isEmpty() ){
        inputImg_ = text;
    }
    QPushButton *button = new QPushButton("Create", this);
    connect(button, SIGNAL(clicked()), newDialog, SLOT(this->newSim(inputImg_, outputPlotFile_, num_threads_, saveToFile));

}
void MainWindow::newSim(QString imgName, QString outputPlotName, int num_threads, bool saveToFile)
{
    inputImg_       = imgName;
    outputPlotFile_ = outputPlotName;
    num_threads_    = num_threads;
    sim = SAXSsim(imgName.toStdString(), outputPlotName.toStdString(), num_threads, saveToFile);

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
    openNodAndEdgAct->setShortcuts(QKeySequence::Open);
    openNodAndEdgAct->setStatusTip(tr("Create a new FFT From Image"));
    connect(openNodAndEdgAct, SIGNAL(triggered()), this, SLOT(createNewDialog());

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


