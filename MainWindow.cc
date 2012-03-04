/* Copyright Sebastian Haas <sebastian@sebastianhaas.info>. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <iostream>

#include "MainWindow.h"

#include <QDomDocument>

static QDomElement findBusByName(QDomElement & docElem, const QString & name)
{
    QDomNode n = docElem.firstChild();

    while(!n.isNull()) {
        if (n.nodeName().compare("Bus") == 0) {
            QDomElement e = n.toElement();

            if(!e.isNull()) {
                if (e.attribute("name").compare(name) == 0)
                    return e;
            }
        }

        n = n.nextSibling();
    }

    return QDomElement();
}

ScaleDescription * ScaleDescription::CreateScaleDescriptionFromString(const QString & name, const QString & str)
{
    ScaleDescription *sd = new ScaleDescription();

    sd->m_ScaleName = name;

    QStringList l = str.split(",");
    QString s;

    foreach(s, l) {
        int message_signal_sep = s.indexOf(".");
        int signal_color_sep = s.indexOf("/");

        if (message_signal_sep >= signal_color_sep)
            continue;

        struct Curve curve;

        curve.messsage =  s.left(message_signal_sep);
        curve.signal =  s.mid(message_signal_sep + 1, (signal_color_sep - 1) - message_signal_sep);
        curve.color =  QColor(s.mid(signal_color_sep + 1));

        sd->m_Curves.push_back(curve);
    }

    return sd;
}

MainWindow::MainWindow(const QString & channel, const QString & filename,
                       const QString & busname, QObject* parent)
 : m_CanChannel(channel)
{
    this->setLayout(new QVBoxLayout());

    setWindowTitle("openCanAnalyzer");

    QDomDocument doc;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return;

    if (!doc.setContent(&file)) {
        file.close();
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomElement e = findBusByName(docElem, busname);

    if (!e.isNull())
        m_CanSignals = QCanSignals::createFromKCD(&m_CanChannel, e);

    file.close();

    m_CanChannel.Start();
}

MainWindow::~MainWindow()
{
}

void MainWindow::addScale(QRealtimePlotter::scale_t scale, const ScaleDescription & desc)
{
    double lower = 0.0, upper = 0.0;

    if (desc.getCurves().isEmpty())
        return;

    QVector<ScaleDescription::Curve>::const_iterator iter = desc.getCurves().begin();
    while (iter != desc.getCurves().end()) {
        double tmp_lower = 0.0, tmp_upper = 0.0;
        ScaleDescription::Curve c = *iter;

        QCanSignalContainer & sc = (*m_CanSignals)[c.messsage];
        QCanSignal & s = sc[c.signal];

        s.getLimit(tmp_lower, tmp_upper);

        if (tmp_lower < lower)
            tmp_lower = lower;

        if (tmp_upper > upper)
            upper = tmp_upper;

        m_Plotter->addCurve(scale, s, c.color);

        iter++;
    }

    m_Plotter->changeScale(scale, lower, upper, desc.getScaleName());
}

void MainWindow::addPlot(const ScaleDescription & left, const ScaleDescription & right)
{
    m_Plotter = new QRealtimePlotter(this);

    const int margin = 5;
    m_Plotter->setContentsMargins(margin, margin, margin, margin);

    m_Plotter->setTimeScale(5000.0);

    addScale(QRealtimePlotter::E_SCALE_LEFT, left);
    addScale(QRealtimePlotter::E_SCALE_RIGHT, right);

    m_Plotter->startRecording();

    layout()->addWidget(m_Plotter);
}

