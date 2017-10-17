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
#include "mainwindow.h"
#include "newdialog.h"
#include "ui_mainwindow.h"
#include <QVector>
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    createActions();
    createToolBars();
    createStatusBar();
    // createContextMenus();
    simToolButton = nullptr;
    simActiveMenu = nullptr;
    ui->tabWidget->clear();

    connect(ui->tabWidget, &QTabWidget::tabCloseRequested,
            [=](const int &index) {
                // std::cout << "Closing index: " << index << std::endl;
                ui->tabWidget->removeTab(index);
                // auto page = ui->tabWidget->widget(index);
                // if (page != nullptr)
                // {
                //     page->deleteLater();
                // }
            });
}

MainWindow::~MainWindow() {
    delete ui;
    delete openAct;
    delete exitAct;
    if (simToolButton != nullptr)
        delete simToolButton;
    if (simActiveMenu != nullptr)
        delete simActiveMenu;
    for (QAction *&act : simActionMap.values()) {
        delete act;
    }
}

void MainWindow::createActions() {
    openAct = new QAction(QIcon(":/resources/open.png"),
                          tr("&Create a new FFT From Image."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Create a new FFT From Image"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(createNewDialog()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}
void MainWindow::createToolBars() { ui->toolBar->addAction(openAct); }

void MainWindow::createStatusBar() { ui->statusbar->showMessage(tr("Ready")); }

void MainWindow::createNewDialog() {
    NewDialog *newDialog = new NewDialog(this);
    connect(newDialog, &NewDialog::newSimFromDialog, newDialog,
            &NewDialog::hide);
    connect(newDialog, &NewDialog::newSimFromDialog, this, &MainWindow::newSim);
    newDialog->exec();
    delete newDialog;
}

// Note that widgets (tabs) must live in main thread.
// If multi-thread is required, the computation should be handled in other
// thread, and report back to the tabWidget when finished.
void MainWindow::newSim(std::string inputImageName) {
    // Always add a tab with newSim
    auto tab = new RadialTabWidget();
    // Populate the tab
    connect(&this->fftWatcher, SIGNAL(finished()), tab,
            SLOT(SetRadialPlot2D()));
    {
        tab->SetInput2D(inputImageName);
        QFuture<void> fftFuture =
            QtConcurrent::run(tab, &RadialTabWidget::SetFFT2D);
        fftWatcher.setFuture(fftFuture);
        // tab->SetFFT2D();
        // tab->SetRadialPlot2D();
    }
    auto info = QFileInfo(QString::fromLocal8Bit(inputImageName.c_str()));
    auto filename = info.fileName();

    tab->setObjectName(filename);
    ui->tabWidget->addTab(tab, filename);
    ui->tabWidget->setTabText(
        ui->tabWidget->indexOf(tab),
        QApplication::translate("MainWindow", filename.toStdString().c_str(),
                                Q_NULLPTR));
    ui->tabWidget->setCurrentWidget(tab);
    ui->tabWidget->tabBar()->show();
}

// void MainWindow::ShowContextMenuPlot(const QPoint& pos)
// {
//     QPoint globalPos =ui->qvtkWidgetFFT->mapToGlobal(pos);
//     QMenu myMenu;
//     myMenu.addAction("Replot", this, SLOT(createRDialog())) ;
//     myMenu.exec(globalPos);
// }
//
// void MainWindow::on_currentSimSwitch(size_t element)
// {
//     renderInputTypeImage();
//     renderFFTWindowed();
// }
