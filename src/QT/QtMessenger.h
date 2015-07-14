#ifndef QTMESSENGER_H_
#define QTMESSENGER_H_
#include <QtCore>
#include <QString>


class QtMessenger : public QObject
{
    Q_OBJECT
public:
    // QtMessenger() = default;
    // QtMessenger(const QtMessenger*);
    virtual ~QtMessenger(){};
signals:
    void message(const QString & msg);
};
#endif
