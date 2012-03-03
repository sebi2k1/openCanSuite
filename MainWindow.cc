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

MainWindow::MainWindow(QObject* parent)
 : m_CanChannel("vcan0")
{
    setWindowTitle("openCanAnalyzer");

    //QVBoxLayout *layout = new QVBoxLayout(this);

    QDomDocument doc;

    QFile file("./can_definition_sample.kcd");
    if (!file.open(QIODevice::ReadOnly))
        return;

    if (!doc.setContent(&file)) {
        file.close();
        return;
    }

    QDomElement docElem = doc.documentElement();
    QDomElement e = findBusByName(docElem, "Motor");

    if (!e.isNull())
        m_CanSignals = QCanSignals::createFromKCD(&m_CanChannel, e);

    file.close();

    QObject::connect(&(*m_CanSignals)["CruiseControlStatus"]["SpeedKm"], SIGNAL(valueChanged(const struct timeval &, double)), this, SLOT(signalValueChanged(const struct timeval &, double)));

    m_Plotter = new QRealtimePlotter(this);

    m_Plotter->setTitle( "History" );

    const int margin = 5;
    m_Plotter->setContentsMargins( margin, margin, margin, margin );

    layout()->addWidget(m_Plotter);
    m_Plotter->resize(600, 400);

    m_Plotter->changeScale(QRealtimePlotter::E_SCALE_LEFT, 0.0, 100.0, "km/h");

    m_CanChannel.Start();
}

MainWindow::~MainWindow()
{
}

void MainWindow::signalValueChanged(const struct timeval & tv, double value)
{
    QCanSignal *signal = static_cast<QCanSignal *>(QObject::sender());

    if (signal)
        m_Plotter->newSampleReceived(tv, value, signal->getName());
}
