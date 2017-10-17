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
#ifndef NEWDIALOG_H_
#define NEWDIALOG_H_

#include "ui_newdialog.h"
#include <QDialog>
#include <QFileDialog>
#include <QPlainTextEdit>

namespace Ui {
class NewDialog;
}

class NewDialog : public QDialog {
    Q_OBJECT

  public:
    explicit NewDialog(QWidget *parent = 0);
    virtual ~NewDialog();
  public slots:
    void browseOpenFile();
    void on_acceptedSettings();
    // void browseSaveFile();
  signals:
    void newSimFromDialog(std::string imgName);

  public:
    QString inputImage;
    // QString outputPlot;
    // bool saveToFile;
    // int numThreads;
  private:
    Ui::NewDialog *ui;
};
#endif
