#include "WorkerSim.h"
using namespace std;
// WorkerSim::~WorkerSim() {}

// void WorkerSim::workerSim_on_finish()
// {
//
// }
void WorkerSim::runSim(std::string imgName, std::string outputPlotName,
        int num_threads, bool saveToFile)
{
    qRegisterMetaType<std::string>();
    qRegisterMetaType<std::shared_ptr<SAXSsim> >();
    qRegisterMetaType<std::shared_ptr<QString> >();
    qRegisterMetaType<QString>();
    // streamToQ = make_shared<StreamToQString>(std::cout, coutToQ);
    // connect(streamToQ.get(), &StreamToQString::streamChanged,
    //         this, &WorkerSim::streamChangedSlot);
    m_sim = make_shared<SAXSsim>(imgName, outputPlotName, num_threads, saveToFile);
    // emit(transmitQString(coutToQ))
    emit(onFinishRun(m_sim));
    emit(onFinish());

}
void WorkerSim::runSimWithMessenger(std::string imgName, std::string outputPlotName,
        int num_threads, bool saveToFile, QPlainTextEdit* box)
{
    // qRegisterMetaType<QPlainTextEdit* >();
    m_debugStream = new Q_DebugStream(std::cout, box); //Redirect Console output to QTextEdit
    this->runSim(imgName, outputPlotName, num_threads, saveToFile);
    delete m_debugStream;

}

