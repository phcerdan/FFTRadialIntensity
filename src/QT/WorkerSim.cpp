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
#include "WorkerSim.h"
using namespace std;

void WorkerSim::runSim(std::string imgName, std::string outputPlotName,
        int num_threads, bool saveToFile)
{
    qRegisterMetaType<std::shared_ptr<SAXSsim> >();
    qRegisterMetaType<QString>();
    // qRegisterMetaType<QString*>();
    try {
        m_sim = make_shared<SAXSsim>(imgName, outputPlotName, num_threads, saveToFile);
    } catch(std::exception &e){
        std::cout << e.what() << std::endl;
    }
    emit(onFinishRun(m_sim));
    emit(onFinish());

}
void WorkerSim::runSimWithMessenger(std::string imgName, std::string outputPlotName,
        int num_threads, bool saveToFile, QPlainTextEdit* box)
{
    qRegisterMetaType<std::shared_ptr<SAXSsim> >();
    qRegisterMetaType<QString>();
    // qRegisterMetaType<QString*>();
    m_sim = make_shared<SAXSsim>(imgName, outputPlotName, num_threads, saveToFile, true);
    connect(m_sim->m_messenger, &QtMessenger::message,
            box, &QPlainTextEdit::appendPlainText);
    try {
        m_sim->Initialize();
    } catch(std::exception &e){
        std::cout << e.what() << std::endl;
    }
    emit(onFinishRun(m_sim));
    emit(onFinish());

}

