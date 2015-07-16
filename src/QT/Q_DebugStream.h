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
