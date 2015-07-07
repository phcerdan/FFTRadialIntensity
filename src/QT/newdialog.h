#ifndef NEWDIALOG_H_
#define NEWDIALOG_H_

#include "ui_newdialog.h"
#include <QPlainTextEdit>
#include <QDialog>
#include <QFileDialog>

namespace Ui
{
class NewDialog;
}

class NewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewDialog(QWidget *parent = 0);
    virtual  ~NewDialog();
public slots:
    void openInputImage();
    void selectOutputPlot();
    void on_acceptedSettings();
signals:
    void newSimFromDialog(std::string imgName, std::string outputPlotName, int num_threads, bool saveToFile );
public:
    QString inputImage;
    QString outputPlot;
    bool saveToFile;
#ifdef ENABLE_PARALLEL
    int numThreads;
#endif
private:
    Ui::NewDialog *ui;
};
#endif
