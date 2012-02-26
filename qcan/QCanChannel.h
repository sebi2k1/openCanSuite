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
#ifndef SOCKETCANCHANNEL_H_
#define SOCKETCANCHANNEL_H_

#include <QThread>
#include <QMetaType>

#include <net/if.h>
#include <linux/can.h>

struct can_message
{
    struct can_frame frame;
    struct timeval tv;
};

Q_DECLARE_METATYPE(can_message);

/**
 * QT based implemenation of a SocketCAN channel.
 */
class QCanChannel : public QThread
{
    Q_OBJECT

signals:
    void canMessageReceived(const can_message & frame);

public:
    /**
     * @param name interface name of CAN interface
     */
    QCanChannel(const char *name);
    ~QCanChannel();

    bool IsValid() { return m_SocketFd > 0; }
    bool Start();
    void Stop();

protected:
    void run();

private:
    int m_SocketFd;
    struct sockaddr_can m_SocketAddr;

    bool m_TerminationRequested;
};

#endif /* SOCKETCANCHANNEL_H_ */
