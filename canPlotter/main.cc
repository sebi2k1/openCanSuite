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
#include <QCommandLineParser>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCommandLineParser parser;

    parser.addHelpOption();

    QCommandLineOption channelOption ("channel",
                "Specify physical/virtul SocketCAN channel (e.g. vcan0)", "channel");
    parser.addOption(channelOption);

    QCommandLineOption kcdFileOption("kcd-file",
                "Path to KCD (Kayak CAN definition file)", "file");
    parser.addOption(kcdFileOption);

    QCommandLineOption busnameOption("busname",
                "Name of the bus the channel belongs to (must match busname in KCD file)", "busname");
    parser.addOption(busnameOption);

    QCommandLineOption leftScaleNameOption("left-scale-name",
                "Name and unit of left scale",
                "scale-name");
    parser.addOption(leftScaleNameOption);

    QCommandLineOption leftScaleSignalsOption("left-scale-signals",
                "List of the signals to draw curve for left scale in the form of:\n"\
                " MESSAGE.SIGNAL/COLOR,MESSAGE.SIGNAL/COLOR e.g. KSM1.ReqSpeed/red,EEC1.EngSpeed/yellow",
                "scale-signals");
    parser.addOption(leftScaleSignalsOption);

    QCommandLineOption rightScaleNameOption("right-scale-name",
                "Name and unit of right scale",
                "scale-name");
    parser.addOption(rightScaleNameOption);

    QCommandLineOption rightScaleSignalsOption("right-scale-signals",
                "List of the signals to draw curve for right scale in the form of:\n"\
                " MESSAGE.SIGNAL/COLOR,MESSAGE.SIGNAL/COLOR e.g. KSM1.ReqSpeed/red,EEC1.EngSpeed/yellow",
                "scale-signals");
    parser.addOption(rightScaleSignalsOption);

    parser.process(a);

    if (!parser.isSet(kcdFileOption)) {
        qWarning("No signal definition file (e.g. Kayak) found");
        return -1;
    }

    if (!parser.isSet(busnameOption)) {
        qWarning("No bus name given");
        return -1;
    }

    QString channel("can0");
    QString kcdfile = parser.value(kcdFileOption);
    QString busname = parser.value(busnameOption);

    if (parser.isSet(channelOption)) {
        channel = parser.value(channelOption);
    }

    QString leftScaleName = parser.value(leftScaleNameOption);
    QString leftScaleSignals = parser.value(leftScaleSignalsOption);
    QString rightScaleName = parser.value(rightScaleNameOption);
    QString rightScaleSignals = parser.value(rightScaleSignalsOption);

    MainWindow vBox(channel,
                    kcdfile,
                    busname);

    ScaleDescription *left_scale = ScaleDescription::CreateScaleDescriptionFromString(
                                    leftScaleName,
                                    leftScaleSignals);
    ScaleDescription *right_scale = ScaleDescription::CreateScaleDescriptionFromString(
                                    rightScaleName,
                                    rightScaleSignals);

    vBox.addPlot(*left_scale, *right_scale);

    vBox.show();

    return a.exec();
}
