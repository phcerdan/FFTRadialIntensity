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
#ifndef RDIALOG_H_
#define RDIALOG_H_

#include "ui_rdialog.h"
#include <QDialog>
#include <string>

namespace Ui
{
class RDialog;
}

class RDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RDialog(QWidget *parent = 0);
    virtual  ~RDialog();
    void setOutputTextEdit(const std::string & );
    double nm_per_pixel_{1.00};
    QString outputFile_;
    std::string formatString_{"svg"};
public slots:
    void browseSaveFile();
    void formatChanged();
    void on_acceptedSettings();
signals:
    void newRplotFromDialog(
            double nm_per_pixel,
            std::string format,
            std::string outputFile);
private:
    Ui::RDialog *ui;
};
#endif
