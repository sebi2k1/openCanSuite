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
#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_plot_layout.h>
#include <qwt/qwt_scale_widget.h>
#include <qwt/qwt_legend.h>
#include <qwt/qwt_legend_data.h>

#include "QRealtimePlotter.h"

/// Small helper to provide a date/time string for the bottom scale
class TimeScaleDraw : public QwtScaleDraw
{
public:
    TimeScaleDraw() {}

    virtual QwtText label(double v) const {
        return QDateTime::fromMSecsSinceEpoch(static_cast<quint64>(v)).time().toString("hh:mm:ss.zzz");
    }
};

QRealtimePlotter::QRealtimePlotter(double buffer_time_ms, QWidget *parent) : QwtPlot(parent)
{
    setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
    setAxisLabelRotation(QwtPlot::xBottom, -50.0);
    setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);

    QwtScaleWidget *scaleWidget = axisWidget(QwtPlot::xBottom);
    const int fmh = QFontMetrics(scaleWidget->font()).height();
    scaleWidget->setMinBorderDist(0, fmh / 2);

    setAxisTitle(QwtPlot::xBottom, "Time");

    QwtLegend *legend = new QwtLegend;
    legend->setDefaultItemMode( QwtLegendData::Checkable );
    insertLegend( legend, QwtPlot::RightLegend );

    connect( this, SIGNAL( legendChecked( QwtPlotItem *, bool ) ), SLOT( showItem( QwtPlotItem *, bool ) ) ) ;

    setTimeScale(1000.0);

    QObject::connect(&m_UpdateTimer, SIGNAL(timeout()), this, SLOT(updateTimeScale()));

    setFrameStyle(QFrame::NoFrame);
    setLineWidth(0);
    ((QFrame *)canvas())->setLineWidth(2);

    plotLayout()->setAlignCanvasToScales(true);

    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->setMajorPen(QPen( Qt::gray, 0, Qt::DotLine));
    grid->attach(this);

    setCanvasBackground(QColor(0, 0, 0));

    m_BufferTime_ms = buffer_time_ms;
}

void QRealtimePlotter::addCurve(scale_t scale, const QObject & source, const QColor & color)
{
    struct Curve *c = new Curve();

    c->curve = new QwtPlotCurve();
    c->curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    c->curve->setPen(color);
    c->curve->attach(this);

    c->source = &source;

    m_Curves[scale].push_back(c);

    QObject::connect(&source, SIGNAL(valueChanged(const struct timeval &, double)),
                     this, SLOT(newSampleReceived(const struct timeval &, double)));
}

void QRealtimePlotter::changeScale(scale_t scale,
                                   const double & lower_bound,
                                   const double & upper_bound,
                                   const QString & unit)
{
    QwtPlot::Axis axis = scale == E_SCALE_LEFT ? QwtPlot::yLeft : QwtPlot::yRight;
    
    setAxisScale(axis, lower_bound, upper_bound);
    setAxisTitle(axis, unit);

    enableAxis(axis);
}

void QRealtimePlotter::setTimeScale(const double & interval_ms)
{
    m_Interval = interval_ms;
}

void QRealtimePlotter::startRecording()
{
    updateTimeScale();
}

void QRealtimePlotter::suspendRecording()
{
    m_UpdateTimer.stop();
}

void QRealtimePlotter::updateTimeScale()
{
    double now = static_cast<double>(QDateTime::currentDateTime().toMSecsSinceEpoch());

    setAxisScale(QwtPlot::xBottom, now, now + m_Interval);

    deleteOldSamples();

    replot();

    m_UpdateTimer.start(m_Interval);
}

void QRealtimePlotter::deleteOldSamples()
{
    int i;

    for(i = 0; i < E_NUM_SCALES; i++) {
        struct Curve *c = NULL;

        // Find curve of sender and append sample value
        foreach(c, m_Curves[i]) {
            if (c->timedata.empty())
                continue;

            double latest_sample = c->timedata.back();

            QVector<double>::iterator time_iter   = c->timedata.begin();
            QVector<double>::iterator sample_iter = c->sample.begin();

            while (time_iter != c->timedata.end()) {
                if ((latest_sample - *time_iter) > m_BufferTime_ms) {
                    time_iter = c->timedata.erase(time_iter);
                    sample_iter = c->sample.erase(sample_iter);
                    continue;
                } else {
                    // Elements are in order if we stumble about a sample not
                    // older than m_BufferTime_ms we can stop erasing samples.
                    break;
                }

                ++time_iter;
                ++sample_iter;
            }

            c->curve->setSamples(c->timedata, c->sample);
        }
    }
}

void QRealtimePlotter::newSampleReceived(const struct timeval & tv, double sample)
{
    QObject *s = QObject::sender();
    int i;

    for(i = 0; i < E_NUM_SCALES; i++) {
        struct Curve *c = NULL;

        // Find curve of sender and append sample value
        foreach(c, m_Curves[i]) {
            if (c->source == s) {
                c->sample.push_back(sample);
                c->timedata.push_back((tv.tv_sec * 1000.0) + (tv.tv_usec / 1000.0));

                c->curve->setSamples(c->timedata, c->sample);
            }
        }
    }

    replot();
}

