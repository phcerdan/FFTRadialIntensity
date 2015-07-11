#ifndef WORKERSIM_H_
#define WORKERSIM_H_
#include <QtCore>
#include <../2D/SAXSsim.h>
#include <memory>
#include <Q_DebugStream.h>

Q_DECLARE_METATYPE(std::string)
Q_DECLARE_METATYPE(std::shared_ptr<SAXSsim> )
Q_DECLARE_METATYPE(std::shared_ptr<StreamToQString>)
Q_DECLARE_METATYPE(std::shared_ptr<QString>)
Q_DECLARE_METATYPE(QString)

// Q_DECLARE_METATYPE(QPlainTextEdit*)
class WorkerSim : public QObject
{
    Q_OBJECT
public:
    // WorkerSim()
    // virtual ~WorkerSim();
signals:
    void onFinishRun(std::shared_ptr<SAXSsim>);
    void transmitQString(QString);
    void onFinish();
    void streamChanged(QString const&);
public slots:
    void runSimWithMessenger(std::string imgName, std::string outputPlotName,
            int num_threads, bool saveToFile, QPlainTextEdit* box);
    void runSim(std::string imgName, std::string outputPlotName,
            int num_threads, bool saveToFile);
    void streamChangedSlot(QString const& in){ emit(streamChanged(in)); };
public:
    std::shared_ptr<SAXSsim> m_sim;
    std::shared_ptr<StreamToQString> streamToQ;
    Q_DebugStream* m_debugStream;
    QString coutToQ;
};
#endif
