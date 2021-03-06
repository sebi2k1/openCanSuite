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

#ifndef QCANSIGNALS_H_
#define QCANSIGNALS_H_

#include <QVector>
#include <QString>

#include <QObject>

#include <QDomElement>
#include <QMetaType>

#include <limits.h>

class QCanChannel;
struct QCanMessage;

typedef enum ENDIANESS
{
    ENDIANESS_MOTOROLA = 0,
    ENDIANESS_INTEL
} ENDIANESS;

Q_DECLARE_METATYPE(struct timeval);

/**
 * A QCanSignal represent a physical value transmitted in a CAN message.
 */
class QCanSignal : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double value
               READ getPhysicalValue
               WRITE setPhysicalValue
               NOTIFY valueHasChanged);

signals:
    void valueChanged(const struct timeval & val, double value);
    void valueHasChanged();
    void canMessageValueSend(quint32, quint32, ENDIANESS, quint64);

public:
    QCanSignal(QString & name, quint8 offset, quint32 length, ENDIANESS order)
     : m_Name(name), m_Offset(offset), m_Length(length), m_Order(order),
       m_Slope(1.0), m_Intercept(0.0), m_RawValue(ULONG_MAX), m_PhysicalValue(0),
       m_IsSigned(false) {
        m_Lower = 0.0;
        m_Upper = (1 << m_Length) - 1; 
    }

    ~QCanSignal() {}

    void setLimit(double lower, double upper) { m_Lower = lower; m_Upper = upper; }
    void getLimit(double & lower, double & upper) { lower = m_Lower; upper = m_Upper; }

    void setEquationOperands(double slope, double intercept) {
        m_Slope = slope;
        m_Intercept = intercept;
    }

    void decodeFromMessage(const QCanMessage & message);

    double getPhysicalValue() { return m_PhysicalValue; }
    void setPhysicalValue(double val);
    quint64 getRawValue() { return m_RawValue; }

    const QString & getName() { return m_Name; }

    void setIsSigned(bool isSigned) { m_IsSigned = isSigned; }

private:
    QString m_Name;
    const quint8 m_Offset;
    const quint32 m_Length;
    const ENDIANESS m_Order;

    double m_Lower;
    double m_Upper;

    double m_Slope;
    double m_Intercept;

    quint64 m_RawValue;
    double m_PhysicalValue;

    bool m_IsSigned;
};

/**
 * This object describes a CAN message holding certain CAN signals
 */
class QCanSignalContainer : public QObject
{
    Q_OBJECT

public:
    QCanSignalContainer(QString & name, quint32 id, bool isExt)
     : m_Name(name), m_CanId(id), m_IsExt(isExt), m_Length(0), m_Data() {}
    ~QCanSignalContainer() {}

    const QString & getName() { return m_Name; }

    void addSignal(QCanSignal* signal) { m_Signals.push_back(signal); }

    void dispatchMessage(const QCanMessage & frame);

    void setLength(quint32 length) { m_Length = length; }

    QCanSignal * operator[](const QString & name) {
        QVector<QCanSignal*>::iterator iter = m_Signals.begin();
        while(iter != m_Signals.end()) {
            if ((*iter)->getName().compare(name) == 0)
                return *iter;
            ++iter;
        }

        return NULL;
    }

    QVector<QCanSignal*> & getSignalList() { return m_Signals; }

signals:
    void canMessageSend(const QCanMessage & frame);

private slots:
    void canMessageValueSend(quint32, quint32, ENDIANESS, quint64);

private:
    QString m_Name;
    const quint32 m_CanId;
    const bool m_IsExt;
    quint32 m_Length;
    quint8 m_Data[8];

    QVector<QCanSignal*> m_Signals;
};

/**
 * Manage a set of message/signals on a CAN channel.
 */
class QCanSignals : public QObject
{
    Q_OBJECT

public:
    QCanSignals(QCanChannel* channel);
    ~QCanSignals();

    /**
     * Create CAN signals from a channel and KCD DOM bus description
     * @param channel CAN channel to attached to
     * @param e "bus" root level DOM element
     */
    static QCanSignals* createFromKCD(QCanChannel* channel, const QDomElement & e);

    /**
     * Create CAN signals from a channel and KCD DOM bus description
     * @param channel CAN channel to attached to
     * @param kcdfile path to KCD XML file
     * @param bus name of the bus to use defined in KCD XML file path to KCD XML file
     */
    static QCanSignals* createFromKCD(QCanChannel* channel, const QString & kcdfile, const QString & bus);

    void addMessage(QCanSignalContainer* message) { m_Messages.push_back(message); }

    QCanSignalContainer * operator[](const QString & name) {
        QVector<QCanSignalContainer*>::iterator iter = m_Messages.begin();
        while(iter != m_Messages.end()) {
            if ((*iter)->getName().compare(name) == 0)
                return *iter;
            ++iter;
        }

        return NULL;
    }

    QVector<QCanSignalContainer*> & getMessageList() { return m_Messages; }

private slots:
    void canMessageReceived(const QCanMessage & frame);

private:
    QCanChannel* m_CanChannel;

    QVector<QCanSignalContainer*> m_Messages;
};

#endif /* QCANSIGNALS_H_ */
