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
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "QCanChannel.h"

QCanChannel::QCanChannel(const QString & name)
{
    m_SocketFd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    m_TerminationRequested = false;

    if (m_SocketFd > 0) {
        struct ifreq ifr;

        strcpy(ifr.ifr_name, name.toStdString().c_str());
        ioctl(m_SocketFd, SIOCGIFINDEX, &ifr);

        m_SocketAddr.can_family = AF_CAN;
        m_SocketAddr.can_ifindex = ifr.ifr_ifindex;

        if (bind(m_SocketFd, (struct sockaddr *)&m_SocketAddr, sizeof(m_SocketAddr)) < 0) {
            close(m_SocketFd);
            m_SocketFd = -1;
        }
    }

    qRegisterMetaType<QCanMessage>("QCanMessage");
}

QCanChannel::~QCanChannel()
{
    Stop();
}

bool QCanChannel::Start()
{
    if (!IsValid())
        return false;

    // Start thread
    QThread::start();

    return true;
}

void QCanChannel::Stop()
{
    m_TerminationRequested = true;

    if (m_SocketFd > 0)
        close(m_SocketFd);

    wait();
}

void QCanChannel::run()
{
    while (!m_TerminationRequested) {
        fd_set rdfs;
        int ret;
        struct timeval tv;
        FD_ZERO(&rdfs);

        FD_SET(m_SocketFd, &rdfs);

        tv.tv_sec = 0;
        tv.tv_usec = 10 * 1000;

        ret = select(m_SocketFd + 1, &rdfs, NULL, NULL, &tv);

        if (ret < 0)
            break;

        if (FD_ISSET(m_SocketFd, &rdfs)) {
            QCanMessage message;

            struct can_frame frame;

            if (recv(m_SocketFd, &frame, sizeof(struct can_frame), MSG_DONTWAIT) > 0) {
                ioctl(m_SocketFd, SIOCGSTAMP, &message.tv);

                message.isExt = (frame.can_id & CAN_EFF_FLAG) ? true : false;
                message.id = frame.can_id & (message.isExt ? CAN_EFF_MASK : CAN_SFF_MASK);
                message.dlc = frame.can_dlc;

                ::memcpy(&message.data[0], &frame.data[0], 8);

                canMessageReceived(message);
            }
        }
    }
}
