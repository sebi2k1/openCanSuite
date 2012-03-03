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

signals:
    void valueChanged(const struct timeval & val, double value);

public:
    QCanSignal(QString & name, quint8 offset, quint32 length, ENDIANESS order)
     : m_Name(name), m_Offset(offset), m_Length(length), m_Order(order), m_RawValue(0) {}
    ~QCanSignal() {}

    void decodeFromMessage(const QCanMessage & message);
    void encodeToMessage(QCanMessage & message) const;

    quint64 getRawValue() { return m_RawValue; }
    const QString & getName() { return m_Name; }

private:
    QString m_Name;
    const quint8 m_Offset;
    const quint32 m_Length;
    const ENDIANESS m_Order;

    quint64 m_RawValue;
};

/**
 * This object describes a CAN message holding certain CAN signals
 */
class QCanSignalContainer : public QObject
{
    Q_OBJECT

public:
    QCanSignalContainer(QString & name, quint32 id, bool isExt)
     : m_Name(name), m_CanId(id), m_IsExt(isExt) {}
    ~QCanSignalContainer() {}

    const QString & getName() { return m_Name; }

    void addSignal(QCanSignal* signal) { m_Signals.push_back(signal); }

    void dispatchMessage(const QCanMessage & frame);

    QCanSignal & operator[](const char *name) {
        QVector<QCanSignal*>::iterator iter = m_Signals.begin();
        while(iter != m_Signals.end()) {
            if ((*iter)->getName().compare(name) == 0)
                return *(*iter);
            ++iter;
        }

        // TODO: Return dummy signal without behaviour
    }

private:
    QString m_Name;
    const quint32 m_CanId;
    const bool m_IsExt;

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

    static QCanSignals* createFromKCD(QCanChannel* channel, const QDomElement & e);

    void addMessage(QCanSignalContainer* message) { m_Messages.push_back(message); }

    QCanSignalContainer & operator[](const char *name) {
        QVector<QCanSignalContainer*>::iterator iter = m_Messages.begin();
        while(iter != m_Messages.end()) {
            if ((*iter)->getName().compare(name) == 0)
                return *(*iter);
            ++iter;
        }

        // TODO: Return dummy signal without behaviour
    }

private slots:
    void canMessageReceived(const QCanMessage & frame);

private:
    QCanChannel* m_CanChannel;

    QVector<QCanSignalContainer*> m_Messages;
};

#endif /* QCANSIGNALS_H_ */
