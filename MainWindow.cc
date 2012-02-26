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

#include "MainWindow.h"

MainWindow::MainWindow(QObject* parent)
 : m_CanChannel("vcan0"),
   m_CanSignals(&m_CanChannel)
{
    setWindowTitle("openCanAnalyzer");

    QVBoxLayout *layout = new QVBoxLayout(this);

    QCanSignalContainer *m = new QCanSignalContainer("CruiseControlStatus", 0x37F, false);
    QCanSignal *s = new QCanSignal("SpeedKm", 2, 8, ENDIANESS_INTEL);
    m->addSignal(s);

    m_CanSignals.addMessage(m);

    QObject::connect(s, SIGNAL(valueChanged()), this, SLOT(signalValueChanged()));

    m_CanChannel.Start();
}

MainWindow::~MainWindow()
{
}

void MainWindow::signalValueChanged()
{
    QCanSignal *signal = static_cast<QCanSignal *>(QObject::sender());

    if (signal)
        qDebug("Signal changed: %llu", signal->getRawValue());
}
