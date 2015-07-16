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
#include <QtWidgets>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVector>
using namespace std;

void MainWindow::createSimButton()
{

    size_t elements = 0;
    if (!simButtonMap.isEmpty())
    {
        elements = simButtonMap.count() ;
    }
    // index starts at 0 to match vector.
    simButtonMap.insert(elements,
            new QAction(QIcon(":/resources/ggplot2.png"), tr("&Select existing Simulation"), this)
            );
    auto &simButton = simButtonMap[elements];
    simButton->setStatusTip(tr("Select simulation"));
    ui->toolBar->addAction(simButton);
    connect(simButton, &QAction::triggered,
            [=](){
            currentSim_ = simVector[elements].get();
            cout << elements <<endl;
            });
}
void MainWindow::createNewDialog()
{

    newDialog = new NewDialog(this);
    connect(newDialog, &NewDialog::newSimFromDialog, newDialog, &NewDialog::hide);
    connect(newDialog, &NewDialog::newSimFromDialog, this, &MainWindow::newSim);
    newDialog->exec();
    delete newDialog;
}
void MainWindow::workerSimHasFinished(std::shared_ptr<SAXSsim> inputSim)
{
    simVector.push_back(inputSim);
    currentSim_ = simVector.back().get();
    createSimButton();
    ui->qvtkWidget->show();
    ui->qvtkWidget_2->show();
    ui->svgWidget->hide();
    currentSim_->ScaleForVisualization();
    renderInputTypeImage();
    renderFFTWindowed();
    newSimAct->setEnabled(true);
#ifdef ENABLE_R
    newRAct->setEnabled(true);
#endif
}

void MainWindow::newSim(string imgName, string outputPlotName, int num_threads, bool saveToFile)
{
    ui->scrollArea->show();
    thread_    = new QThread(this);
    workerSim_ = new WorkerSim;
    qRegisterMetaType<QString>();
    qRegisterMetaType<std::string>();
    qRegisterMetaType<std::shared_ptr<SAXSsim> >();;
    connect(this, &MainWindow::runWorkerSim,
            workerSim_, &WorkerSim::runSim);
    connect(this, &MainWindow::runWorkerSimWithMessenger,
            workerSim_, &WorkerSim::runSimWithMessenger);

    connect(workerSim_, &WorkerSim::onFinishRun,
            this,&MainWindow::workerSimHasFinished, Qt::BlockingQueuedConnection) ;

    connect( workerSim_, SIGNAL(onFinish()), thread_, SLOT(quit())) ;
    connect( workerSim_, SIGNAL(onFinish()), workerSim_, SLOT(deleteLater())) ;
    connect( thread_, SIGNAL(finished()), thread_, SLOT(deleteLater()) );
    workerSim_->moveToThread(thread_);
    thread_->start();

    newSimAct->setEnabled(false);
    // emit(this->runWorkerSim(imgName, outputPlotName, num_threads, saveToFile));
    emit(this->runWorkerSimWithMessenger(imgName, outputPlotName, num_threads, saveToFile, ui->textEdit));

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

    QMenu myMenu;
    myMenu.addAction("Original (Windowed)", this, SLOT(renderFFTWindowed()));
    myMenu.addAction("Write this log(FFTModulus) to disk", this, SLOT(writeFFTImageToDisk()));
    myMenu.exec(globalPos);
}
void MainWindow::renderInputTypeImage()
{

    auto &sim = currentSim_;
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
    auto &sim = currentSim_;
    try {
        sim->WriteFFT(sim->fftModulusSquare_, fileName.toStdString());
    }
    catch (std::exception & e){

        std::cout << e.what() << std::endl;
    }

}
void MainWindow::renderFFTWindowed()
{
    auto &sim = currentSim_;
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
    ui->svgWidget->hide();
    ui->scrollArea->hide();
#ifdef ENABLE_R
    createRMenus();
    newRAct->setEnabled(false);
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
    for(QAction* & act : simButtonMap.values())
    {
        delete act;
    }
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


#ifdef ENABLE_R
void MainWindow::createRMenus()
{

    newRAct = new QAction(QIcon(":/resources/ggplot2.png"), tr("&Use R script to plot current sim"), this);
    // newRAct->setShortcuts(QKeySequence::Open);
    newRAct->setStatusTip(tr("Create a R plot from sim"));
    connect(newRAct, SIGNAL(triggered()), this, SLOT(drawRPlot()));


    ui->toolBar->addAction(newRAct);
}
void MainWindow::drawRPlot()
{
    auto &sim = currentSim_;
    auto fileNameSVG_ = sim->GenerateSVG(sim->GetOutputName(),0.86);
    ui->svgWidget->load(
            filterSVGFile(fileNameSVG_, "./test1.svg"));
    ui->svgWidget->show();
}

QString MainWindow::filterSVGFile(const std::string & inputSVGFile, const std::string &outputFile) {
    // cairoDevice creates richer SVG than Qt can display
    // but per Michaele Lawrence, a simple trick is to s/symbol/g/ which we do here
    QFile infile(QString::fromStdString(inputSVGFile));
    infile.open(QFile::ReadOnly);
    QFile outfile(QString::fromStdString(outputFile));
    outfile.open(QFile::WriteOnly | QFile::Truncate);

    QTextStream in(&infile);
    QTextStream out(&outfile);
    QRegExp rx1("<symbol");
    QRegExp rx2("</symbol");
    while (!in.atEnd()) {
        QString line = in.readLine();
        line.replace(rx1, "<g"); // so '<symbol' becomes '<g ...'
        line.replace(rx2, "</g");// and '</symbol becomes '</g'
        out << line << "\n";
    }
    infile.close();
    outfile.close();
    return QString::fromStdString(outputFile);
}
#endif
