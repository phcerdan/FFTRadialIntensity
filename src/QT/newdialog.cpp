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
#include "newdialog.h"
using namespace std;

NewDialog::NewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewDialog)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked()), this, SLOT(openInputImage()));
    connect(ui->pushButton_2,SIGNAL(clicked()), this, SLOT(selectOutputPlot()));
    /// Gray out if parallel is not enabled.
#ifndef ENABLE_PARALLEL
    ui->spinBox->setDisabled(true);
    ui->label_2->setDisabled(true);
#endif
    /// On accept send signal(via SLOT) to parent MainWindow.
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(on_acceptedSettings()));
}

NewDialog::~NewDialog()
{
    delete ui;
}

/**
 * @brief Open a QFileDialog with open properties
 */
void NewDialog::openInputImage()
{

    QFileDialog *fileDialog = new QFileDialog(this);
    QString fileName = fileDialog->getOpenFileName(this,tr("Image Path"), QDir::currentPath());
    delete fileDialog;
    ui->plainTextEdit->setPlainText(fileName);

}

/**
 * @brief Open a QFileDialog with save properties.
 */
void NewDialog::selectOutputPlot()
{

    QFileDialog *fileDialog = new QFileDialog(this);
    QString fileName = fileDialog->getSaveFileName(this,"Save output .plot",
            QDir::currentPath(), tr("Plain text (*.plot)"));
    delete fileDialog;
    ui->plainTextEdit_2->setPlainText(fileName);

}

/**
 * @brief Transform QString to String, set members of dialog, and send newSimFromDialog signal, catched in MainWindow
 */
void NewDialog::on_acceptedSettings()
{
    inputImage = ui->plainTextEdit->toPlainText();
    outputPlot = ui->plainTextEdit_2->toPlainText();
    saveToFile = ui->checkBox;
#ifdef ENABLE_PARALLEL
    numThreads = ui->spinBox->value();
#else
    ui->spinBox->setEnabled(false);
    // The default is 1.
    numThreads = ui->spinBox->value();
#endif


    emit(newSimFromDialog(
                inputImage.toStdString(),
                outputPlot.toStdString(),
                numThreads,
                saveToFile));
}
