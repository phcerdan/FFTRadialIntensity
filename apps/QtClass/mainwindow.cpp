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
#include "newdialog.h"
#include "rdialog.h"
#include <QVector>
using namespace std;

void MainWindow::on_currentSimSwitch(size_t element)
{
    // Re-render widgets.
    // ui->qvtkWidgetInput->SetRenderWindow(renInputVector[element]->GetRenderWindow());
    // renInputVector[element]->Render();
    // ui->qvtkWidgetFFT->GetRenderWindow()->AddRenderer(renFFTVector[element]);
    renderInputTypeImage();
    renderFFTWindowed();
#ifdef ENABLE_R
    // If every sim has associated an RPlot;
    if(svgFileNamesVector.size() == simVector.size())
    {
        auto qSvgFile = QString::fromStdString(svgFileNamesVector[element]);
        ui->svgWidgetRplot->load(qSvgFile);
    }

#endif

}
void MainWindow::addSimAction()
{

    size_t elements = simActionMap.count() ;
    simActionMap.insert(elements,

            new QAction(QIcon(":/resources/network-64.png"), QString::number(elements), this)
            );
    auto &simAction = simActionMap[elements];
    simActiveMenu->addAction(simAction);

    connect(simAction, &QAction::triggered,
            [=](){
            currentSim_ = simVector[elements].get();
            emit(currentSimSwitch(elements));
            cout << elements <<endl;
            });
}
void MainWindow::createSimButton()
{

    if (simActionMap.isEmpty()) {
        simActiveMenu = new QMenu;
        addSimAction();
        simToolButton = new QToolButton;
        simToolButton->setPopupMode(QToolButton::MenuButtonPopup);
        simToolButton->setMenu(simActiveMenu);
        auto &simAction = simActionMap[0];
        simToolButton->setDefaultAction(simAction);

        ui->toolBar->addWidget(simToolButton);
    }
    else {
        addSimAction();
    }
    // simToolButton->setStatusTip(tr("Select simulation"));
}
void MainWindow::createNewDialog()
{

    NewDialog* newDialog = new NewDialog(this);
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
    ui->qvtkWidgetInput->show();
    ui->qvtkWidgetFFT->show();
    ui->svgWidgetRplot->hide();
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
    ui->tabWidget->setCurrentIndex(0);
    ui->scrollArea->show();
    thread_    = new QThread(this);
    workerSim_ = new WorkerSim;
    qRegisterMetaType<QString>();
    qRegisterMetaType<std::string>();
    qRegisterMetaType<std::shared_ptr<SAXSsim> >();;
    connect(this, &MainWindow::runWorkerSim,
            workerSim_, &WorkerSim::runSim);
    connect(workerSim_, &WorkerSim::onFinishRun,
            this,&MainWindow::workerSimHasFinished, Qt::BlockingQueuedConnection) ;

    connect( workerSim_, SIGNAL(onFinish()), thread_, SLOT(quit())) ;
    connect( workerSim_, SIGNAL(onFinish()), workerSim_, SLOT(deleteLater())) ;
    connect( thread_, SIGNAL(finished()), thread_, SLOT(deleteLater()) );
    workerSim_->moveToThread(thread_);
    thread_->start();

    newSimAct->setEnabled(false);
    emit(this->runWorkerSim(imgName, outputPlotName, num_threads, saveToFile));
    // emit(this->runWorkerSimWithMessenger(imgName, outputPlotName, num_threads, saveToFile, ui->textEditConsole));

}

void MainWindow::createContextMenus()
{
    ui->qvtkWidgetFFT->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->qvtkWidgetFFT, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenuQVTKFFT(const QPoint&)));

#ifdef ENABLE_R
    ui->svgWidgetRplot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->svgWidgetRplot, SIGNAL(customContextMenuRequested(const QPoint&)),
            this, SLOT(ShowContextMenuRplot(const QPoint&)));
#endif
}

void MainWindow::ShowContextMenuQVTKFFT(const QPoint& pos)
{
    QPoint globalPos =ui->qvtkWidgetFFT->mapToGlobal(pos);

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
    auto style =
        vtkSmartPointer<vtkInteractorStyleImage>::New();
    ui->qvtkWidgetInput->GetRenderWindow()->AddRenderer(renderer);
    ui->qvtkWidgetInput->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
    // Old style:
    // ui->qvtkWidgetInput->SetRenderWindow(renderer->GetRenderWindow());
    // renderer->Render();
    renderer->ResetCamera();
    ui->qvtkWidgetInput->GetRenderWindow()->Render();

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
    auto style =
        vtkSmartPointer<vtkInteractorStyleImage>::New();
    ui->qvtkWidgetFFT->GetRenderWindow()->AddRenderer(renderer);
    ui->qvtkWidgetFFT->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
    renderer->ResetCamera();
    ui->qvtkWidgetFFT->GetRenderWindow()->Render();

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
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createActions();
    createToolBars();
    createStatusBar();
    createContextMenus();
    simToolButton = nullptr;
    simActiveMenu = nullptr;
    ui->qvtkWidgetInput->hide();
    ui->qvtkWidgetFFT->hide();
    ui->svgWidgetRplot->hide();
    ui->scrollArea->hide();
#ifdef ENABLE_R
    createRMenus();
    newRAct->setEnabled(false);
#endif
    connect(this, &MainWindow::currentSimSwitch,
            this, &MainWindow::on_currentSimSwitch);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete newSimAct;
    delete exitAct;
    if(simToolButton!=nullptr) delete simToolButton;
    if(simActiveMenu!=nullptr) delete simActiveMenu;
#ifdef ENABLE_R
    delete newRAct;
#endif
    for(QAction* & act : simActionMap.values())
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
    connect(newRAct, SIGNAL(triggered()), this, SLOT(createRDialog()));

    ui->toolBar->addAction(newRAct);
}

void MainWindow::createRDialog()
{
    RDialog * rdialog = new RDialog(this);
    connect(rdialog, &RDialog::newRplotFromDialog, this, &MainWindow::drawRPlot);
    // Set default outputFile.
    auto &sim = currentSim_;
    string outputFileRplot;
    size_t lastdot = sim->GetOutputName().find_last_of(".");
    if (lastdot == std::string::npos){
        outputFileRplot = sim->GetOutputName() + "." + "svg";
    } else {
        outputFileRplot = sim->GetOutputName().substr(0, lastdot) + "." + "svg";
    }

    rdialog->setOutputTextEdit(outputFileRplot);
    rdialog->exec();
    delete rdialog;
}

void MainWindow::drawRPlot(
        double nm_per_pixel,
        std::string format,
        std::string outputRFile)
{
    auto &sim = currentSim_;

    string modString;
    size_t lastdot = outputRFile.find_last_of(".");
    if (lastdot == std::string::npos){
        modString = outputRFile + "." + format;
    } else {
        modString = outputRFile.substr(0, lastdot) + "." + format;
    }
    sim->GeneratePlotVisualizationFile(
                sim->GetOutputName(),
                nm_per_pixel,
                format,
                modString
                /*RscriptFile*/
                );

    string svgString{modString};
    // Generate extra output (svg is a must to visualize)
    if(format!="svg")
    {
        format = "svg";
        string modString;
        size_t lastdot = outputRFile.find_last_of(".");
        if (lastdot == std::string::npos){
            modString = outputRFile + "." + format;
        } else {
            modString = outputRFile.substr(0, lastdot) + "." + format;
        }
        sim->GeneratePlotVisualizationFile(
                    sim->GetOutputName(),
                    nm_per_pixel,
                    format,
                    modString
                    /*RscriptFile*/
                    );
        svgString = modString;
    }

    svgFileNamesVector.push_back(svgString);
    ui->svgWidgetRplot->load(
            filterSVGFile(svgString));
    ui->svgWidgetRplot->show();
    ui->tabWidget->setCurrentWidget(ui->svgWidgetRplot);

}

QString MainWindow::filterSVGFile(const std::string & inputSVGFile) {
    // cairoDevice creates richer SVG than Qt can display
    // but per Michaele Lawrence, a simple trick is to s/symbol/g/ which we do here
    QFile infile(QString::fromStdString(inputSVGFile));
    infile.open(QFile::ReadOnly);
    QFile outfile(QString::fromStdString(inputSVGFile + "tmp"));
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
    infile.remove();
    outfile.rename(infile.fileName());
    return outfile.fileName();

}
void MainWindow::ShowContextMenuRplot(const QPoint& pos)
{
    QPoint globalPos =ui->qvtkWidgetFFT->mapToGlobal(pos);
    QMenu myMenu;
    myMenu.addAction("Replot", this, SLOT(createRDialog())) ;
    myMenu.exec(globalPos);
}
#endif
