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
#include <stdint.h>

#include <QFile>

#include "QCanSignals.h"
#include "QCanChannel.h"

//-----------------------------------------------------------------------------
/**
 * QCanSignals
 */
QCanSignals* QCanSignals::createFromKCD(QCanChannel* channel, const QDomElement & e)
{
    QCanSignals *s = new QCanSignals(channel);

    QDomNode messageNode = e.firstChild();

    // Find all "Message" nodes
    while(!messageNode.isNull()) {
       if (messageNode.nodeName().compare("Message") == 0) {
           QDomElement messageElem = messageNode.toElement();

           if (!messageElem.isNull()) {
               QString name = messageElem.attribute("name");
               quint32 id   = messageElem.attribute("id").toLong(NULL, 16);
               bool ext     = messageElem.attribute("format", "standard").compare("extended") == 0;

               QCanSignalContainer *sc = new QCanSignalContainer(name, id, ext);

               // Find all "Signal" nodes
               for (QDomNode signalNode = messageElem.firstChild(); !signalNode.isNull(); signalNode = signalNode.nextSibling()) {
                   if (signalNode.nodeName().compare("Signal") == 0) {
                       QDomElement signalElem = signalNode.toElement();

                       if (signalElem.isNull())
                           continue;

                       name = signalElem.attribute("name");
                       quint32 offset = signalElem.attribute("offset").toLong();
                       quint32 length = signalElem.attribute("length", "1").toLong();
                       ENDIANESS order = signalElem.attribute("endianess", "little").compare("little") == 0 ? ENDIANESS_INTEL : ENDIANESS_MOTOROLA;

                       QCanSignal * signal = new QCanSignal(name, offset, length, order);

                       for (QDomNode valueNode = signalElem.firstChild(); !valueNode.isNull(); valueNode = valueNode.nextSibling()) {
                           if (valueNode.nodeName().compare("Value") == 0) {
                               QDomElement valueElem = valueNode.toElement();

                               if (valueElem.isNull())
                                   continue;

                               double slope = valueElem.attribute("slope", "1.0").toDouble();
                               double intercept = valueElem.attribute("intercept", "0.0").toDouble();

                               signal->setEquationOperands(slope, intercept);

                               QString min = valueElem.attribute("min", "0.0");
                               QString max = valueElem.attribute("max");

                               if (!max.isEmpty())
                                   signal->setLimit(min.toDouble(), max.toDouble());

                               bool isSigned = valueElem.attribute("type", "unsigned").compare("signed") == 0 ? true : false;
                               signal->setIsSigned(isSigned);
                           }
                       }

                       sc->addSignal(signal);
                   }
               }
               
               s->addMessage(sc);
           }
       }

       messageNode = messageNode.nextSibling();
    }

    return s;
}

QCanSignals* QCanSignals::createFromKCD(QCanChannel* channel, const QString & kcdfile, const QString & bus)
{
    QDomDocument doc;

    QFile file(kcdfile);
    if (!file.open(QIODevice::ReadOnly))
        return NULL;

    if (!doc.setContent(&file)) {
        file.close();
        return NULL;
    }

    QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();

    while(!n.isNull()) {
        if (n.nodeName().compare("Bus") == 0) {
            QDomElement e = n.toElement();

            if(!e.isNull()) {
                if (e.attribute("name").compare(bus) == 0) {
                    file.close();

                    return createFromKCD(channel, e);
                }
            }
        }

        n = n.nextSibling();
    }

    file.close();

    return NULL;
}

QCanSignals::QCanSignals(QCanChannel* channel) : m_CanChannel(channel)
{
    QObject::connect(m_CanChannel, SIGNAL(canMessageReceived(const QCanMessage &)), this, SLOT(canMessageReceived(const QCanMessage &)));
}

QCanSignals::~QCanSignals()
{
}

void QCanSignals::canMessageReceived(const QCanMessage & frame)
{
    QVector<QCanSignalContainer*>::iterator iter = m_Messages.begin();

    while(iter != m_Messages.end()) {
        (*iter)->dispatchMessage(frame);
        ++iter;
    }
}

//-----------------------------------------------------------------------------
/**
 * QCanSignalContainer
 */

void QCanSignalContainer::dispatchMessage(const QCanMessage & frame)
{
    if (frame.id == m_CanId && frame.isExt == frame.isExt) {
        QVector<QCanSignal*>::iterator iter = m_Signals.begin();

        while(iter != m_Signals.end())
            (*iter++)->decodeFromMessage(frame);
    }
}

//-----------------------------------------------------------------------------
/**
 * QCanSignal
 */


static quint64 _getvalue(const quint8 * const data, quint32 offset, quint32 length, ENDIANESS byteOrder)
{
    quint64 d;
    quint64 o = 0;

    if (byteOrder == ENDIANESS_INTEL) {
        d = le64toh(*((uint64_t *)&data[0]));
    } else {
        d = be64toh(*((uint64_t *)&data[0]));
    }

    quint64 m = (1 << length) - 1;
    size_t shift;
    if (byteOrder == ENDIANESS_INTEL) {
        shift = offset;
    } else {
        shift = 64 - offset - length;
    }

    o = (d >> shift) & m;

    return o;
}

void QCanSignal::decodeFromMessage(const QCanMessage & message)
{
    quint64 value = _getvalue(&message.data[0], m_Offset, m_Length, m_Order);
    bool changed = value != m_RawValue;

    m_RawValue = value;

    // Convert from 2s complement
    if ((m_RawValue & (1 << (m_Length - 1))) && m_IsSigned) {
        qint32 tmp = -1 * (~((~Q_UINT64_C(0) << m_Length) | m_RawValue) + 1);
        m_PhysicalValue = (tmp * m_Slope) + m_Intercept;
    }
    else {
        m_PhysicalValue = (m_RawValue * m_Slope) + m_Intercept;
    }

    if (m_PhysicalValue < m_Lower)
        m_PhysicalValue = m_Lower;

    if (m_PhysicalValue > m_Upper)
        m_PhysicalValue = m_Upper;

    if (changed)
    {
printf("value has changed\n");
        emit valueChanged(message.tv, m_PhysicalValue);
        emit valueHasChanged();
    }
}

void _setvalue(quint32 offset, quint32 bitLength, ENDIANESS endianess, quint8 data[8], quint64 raw_value)
{
    quint64 o;

    if (endianess == ENDIANESS_INTEL) {
        o = le64toh(*((uint64_t *)&data[0]));
    } else {
        o = be64toh(*((uint64_t *)&data[0]));
    }

    quint64 m = ((1 << bitLength) - 1);
    size_t shift;
    if (endianess == ENDIANESS_INTEL) {
        shift = offset;
    } else {
        shift = 64 - offset - bitLength;
    }

    o &= ~(m << shift);
    o |= (raw_value & m) << shift;

    if (endianess == ENDIANESS_INTEL) {
        o = htole64(o);
    } else {
        o = htobe64(o);
    }

    memcpy(&data[0], &o, 8);
}
