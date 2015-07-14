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

