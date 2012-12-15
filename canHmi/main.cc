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

#include <QApplication>
#include <QxtCommandOptions>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QxtCommandOptions options;

    options.add("help", "Show this usage");

    options.add("channel",
                "Specify physical/virtul SocketCAN channel (e.g. vcan0)",
                QxtCommandOptions::ValueRequired);

    options.add("kcd-file",
                "Path to KCD (Kayak CAN definition file)",
                QxtCommandOptions::ValueRequired);

    options.add("busname",
                "Name of the bus the channel belongs to (must match busname in KCD file)",
                QxtCommandOptions::ValueRequired);

    options.parse(a.arguments());

    if (options.count("help") || options.showUnrecognizedWarning()) {
        options.showUsage();
        return -1;
    }

    if (!options.count("kcd-file")) {
        qWarning("No signal definition file (e.g. Kayak) found");
        return -1;
    }

    if (!options.count("busname")) {
        qWarning("No bus name given");
        return -1;
    }

    QString channel("can0");
    QString kcdfile = options.value("kcd-file").toString();

    if (options.count("channel"))
        channel = options.value("channel").toString();

    return a.exec();
}
