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

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QtGui>

#include <QCanChannel.h>
#include <QCanSignals.h>

#include <QRealtimePlotter.h>

class ScaleDescription {
private:
    ScaleDescription() {}

public:
    /**
     * Create scale description based on format string
     * @param str e.g. MESSAGE.SIGNAL/COLOR,...
     */
    static ScaleDescription * CreateScaleDescriptionFromString(const QString & name, const QString & str);

    const QString & getScaleName() const { return m_ScaleName; }

    struct Curve {
        QString messsage;
        QString signal;
        QColor color;
    };

    const QVector<struct Curve> & getCurves() const { return m_Curves; }

private:
    QString m_ScaleName;

    QVector<struct Curve> m_Curves;
};

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(const QString & channel, const QString & file,
                        const QString & busname, QObject* parent = NULL);
    virtual ~MainWindow();

    void addPlot(const ScaleDescription & left, const ScaleDescription & right);

protected:
    void addScale(QRealtimePlotter::scale_t scale, const ScaleDescription & desc);

private:
    QCanChannel m_CanChannel;
    QCanSignals* m_CanSignals;

    QRealtimePlotter *m_Plotter;
};

#endif /* MAINWINDOW_H_ */
