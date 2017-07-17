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
#ifndef WORKERSIM_H_
#define WORKERSIM_H_
#include "SAXSsim.h"
#include <memory>
#include <QtWidgets/QPlainTextEdit>

Q_DECLARE_METATYPE(std::shared_ptr<SAXSsim> )
Q_DECLARE_METATYPE(QString)
Q_DECLARE_METATYPE(std::string)

// Q_DECLARE_METATYPE(QPlainTextEdit*)
class WorkerSim : public QObject
{
    Q_OBJECT
public:
signals:
    void onFinishRun(std::shared_ptr<SAXSsim>);
    void onFinish();
public slots:
    void runSim(std::string imgName, std::string outputPlotName,
            int num_threads, bool saveToFile);
public:
    std::shared_ptr<SAXSsim> m_sim;
};
#endif
