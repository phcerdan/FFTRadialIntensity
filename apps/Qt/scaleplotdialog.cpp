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
#include "scaleplotdialog.h"
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>

ScalePlotDialog::ScalePlotDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ScalePlotDialog) {
    ui->setupUi(this);
    ui->advancedFrame->hide();

    connect(ui->outputBrowseButton, SIGNAL(clicked()), this,
            SLOT(browseSaveFile()));
    connect(ui->advancedBox, SIGNAL(toggled(bool)), ui->advancedFrame,
            SLOT(setVisible(bool)));

    connect(ui->radioSvg, SIGNAL(clicked()), this, SLOT(formatChanged()));
    connect(ui->radioPdf, SIGNAL(clicked()), this, SLOT(formatChanged()));
    connect(ui->radioTikz, SIGNAL(clicked()), this, SLOT(formatChanged()));
    connect(ui->buttonBox, SIGNAL(accepted()), this,
            SLOT(on_acceptedSettings()));
}

ScalePlotDialog::~ScalePlotDialog() { delete ui; }

void ScalePlotDialog::on_acceptedSettings() {
    nm_per_pixel_ = ui->doubleSpinBox->value();
    // outputFile_ = ui->outputTextEdit->toPlainText();
    emit(newPlotFromScalePlotDialog(nm_per_pixel_));
            // , formatString_, outputFile_.toStdString()));
}
// void ScalePlotDialog::browseSaveFile() {
//
//     QString formatQString = QString::fromStdString(formatString_);
//
//     QFileDialog *fileDialog = new QFileDialog(this);
//     fileDialog->setAcceptMode(QFileDialog::AcceptSave);
//     QString filter = tr("Output Formats(*.svg *.pdf *.tikz)");
//     fileDialog->setNameFilter(filter);
//     fileDialog->setDirectory(QDir::currentPath());
//     fileDialog->setDefaultSuffix(formatQString);
//     fileDialog->setWindowTitle(tr("Save Output RPlot"));
//     fileDialog->exec();
//     QString fileName = fileDialog->selectedFiles().first();
//     delete fileDialog;
//
//     ui->outputTextEdit->setPlainText(fileName);
//     formatChanged();
// }
// void ScalePlotDialog::formatChanged() {
//
//     if (ui->radioSvg->isChecked())
//         formatString_ = "svg";
//     else if (ui->radioPdf->isChecked())
//         formatString_ = "pdf";
//     else if (ui->radioTikz->isChecked())
//         formatString_ = "tikz";
//
//     outputFile_ = ui->outputTextEdit->toPlainText();
//
//     QFileInfo fi(outputFile_);
//     QString fresult = QDir(fi.path()).filePath(fi.completeBaseName()) + "." +
//                       QString::fromStdString(formatString_);
//     setOutputTextEdit(fresult.toStdString());
// }

// void ScalePlotDialog::setOutputTextEdit(const std::string &outText) {
//     ui->outputTextEdit->setPlainText(QString::fromStdString(outText));
// }
// void ScalePlotDialog::browseSaveFile() {
//
//     QString formatQString = QString::fromStdString(formatString_);
//
//     QFileDialog *fileDialog = new QFileDialog(this);
//     fileDialog->setAcceptMode(QFileDialog::AcceptSave);
//     QString filter = tr("Output Formats(*.svg *.pdf *.tikz)");
//     fileDialog->setNameFilter(filter);
//     fileDialog->setDirectory(QDir::currentPath());
//     fileDialog->setDefaultSuffix(formatQString);
//     fileDialog->setWindowTitle(tr("Save Output RPlot"));
//     fileDialog->exec();
//     QString fileName = fileDialog->selectedFiles().first();
//     delete fileDialog;
//
//     ui->outputTextEdit->setPlainText(fileName);
//     formatChanged();
// }
// void ScalePlotDialog::formatChanged() {
//
//     if (ui->radioSvg->isChecked())
//         formatString_ = "svg";
//     else if (ui->radioPdf->isChecked())
//         formatString_ = "pdf";
//     else if (ui->radioTikz->isChecked())
//         formatString_ = "tikz";
//
//     outputFile_ = ui->outputTextEdit->toPlainText();
//
//     QFileInfo fi(outputFile_);
//     QString fresult = QDir(fi.path()).filePath(fi.completeBaseName()) + "." +
//                       QString::fromStdString(formatString_);
//     setOutputTextEdit(fresult.toStdString());
// }

// void ScalePlotDialog::setOutputTextEdit(const std::string &outText) {
//     ui->outputTextEdit->setPlainText(QString::fromStdString(outText));
// }
