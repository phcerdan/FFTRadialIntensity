#ifndef Q_DEBUGSTREAM_H
#define Q_DEBUGSTREAM_H

#include <iostream>
#include <streambuf>
#include <string>

#include <QtWidgets/QPlainTextEdit>
#include <QTextBlock>
#include <QTextBlock>
// #include <QtCore>
// #include <QTextStream>
/** http://www.qtforum.org/article/39768/redirecting-std-cout-std-cerf-qdebug-to-qtextedit.html
 * https://github.com/viennamos/viennamos-dev/blob/master/framework/include/qdebugstream.h
 * Usage :
    qRegisterMetaType<QTextBlock>();
    qRegisterMetaType<QTextCursor>();
 * new Q_DebugStream(std::cout, ui->TXT_Console); //Redirect Console output to QTextEdit
 * Q_DebugStream::registerQDebugMessageHandler(); //Redirect qDebug() output to QTextEdit
 */
Q_DECLARE_METATYPE(QTextBlock)
Q_DECLARE_METATYPE(QTextCursor)
class StreamToQString : public QObject, public std::basic_streambuf<char>
{
    Q_OBJECT
public:
    StreamToQString(std::ostream &stream, QString& text_edit) :
        m_stream(stream),
        log_window(text_edit)
    {
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
        qRegisterMetaType<QTextBlock>("QTextBlock");
        qRegisterMetaType<QTextCursor>("QTextCursor");
    }

    ~StreamToQString()
    {
        // output anything that is left
        if (!m_string.empty())
            log_window.append(QString::fromUtf8(m_string.c_str()));

        m_stream.rdbuf(m_old_buf);
    }

    static void registerQDebugMessageHandler(){
        qInstallMessageHandler(myQDebugMessageHandler);
    }

private:

    static void myQDebugMessageHandler(QtMsgType, const QMessageLogContext &, const QString &msg)
    {
        std::cout << msg.toStdString().c_str();
    }

signals:
    void streamChanged(QString const&);
protected:

    //This is called when a std::endl has been inserted into the stream
    int_type overflow(int_type v)
    {
        if (v == '\n')
        {
            log_window.append(QString::fromUtf8(m_string.c_str()));
            m_string.erase(m_string.begin(), m_string.end());
        }
        else
            m_string += v;

        return v;
    }

    std::streamsize xsputn(const char *p, std::streamsize n)
    {
        m_string.append(p, p + n);

        std::size_t pos = 0;
        while (pos != std::string::npos)
        {
            pos = m_string.find('\n');
            if (pos != std::string::npos)
            {
                std::string tmp(m_string.begin(), m_string.begin() + pos);

                log_window.append(QString::fromUtf8(tmp.c_str()));

                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }
        emit(streamChanged(log_window));
        return n;
    }

private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;
    QString& log_window;
};


class Q_DebugStream :  public std::basic_streambuf<char>
{

public:
    Q_DebugStream(std::ostream &stream, QPlainTextEdit*& text_edit) : m_stream(stream),
        log_window{text_edit}
    {
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
        qRegisterMetaType<QTextBlock>("QTextBlock");
        qRegisterMetaType<QTextCursor>("QTextCursor");
    }

    ~Q_DebugStream()
    {
        // output anything that is left
        if (!m_string.empty())
            log_window->appendPlainText(m_string.c_str());

        m_stream.rdbuf(m_old_buf);
    }

    static void registerQDebugMessageHandler(){
        qInstallMessageHandler(myQDebugMessageHandler);
    }

private:

    static void myQDebugMessageHandler(QtMsgType, const QMessageLogContext &, const QString &msg)
    {
        std::cout << msg.toStdString().c_str();
    }

protected:

    //This is called when a std::endl has been inserted into the stream
    int_type overflow(int_type v)
    {
        if (v == '\n')
        {
            log_window->appendPlainText(m_string.c_str());
            m_string.erase(m_string.begin(), m_string.end());
        }
        else
            m_string += v;

        return v;
    }

    std::streamsize xsputn(const char *p, std::streamsize n)
    {
        m_string.append(p, p + n);

        std::size_t pos = 0;
        while (pos != std::string::npos)
        {
            pos = m_string.find('\n');
            if (pos != std::string::npos)
            {
                std::string tmp(m_string.begin(), m_string.begin() + pos);

                log_window->appendPlainText(tmp.c_str());

                m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
            }
        }
        return n;
    }

private:
    std::ostream &m_stream;
    std::streambuf *m_old_buf;
    std::string m_string;
    QPlainTextEdit*& log_window;
};


#endif // Q_DEBUGSTREAM_H
