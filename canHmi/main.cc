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
#include <QDebug>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QxtCommandOptions>

#include <QCanChannel.h>
#include <QCanSignals.h>

struct bus_channel_mapping {
    QString channel;
    QString bus;
};

typedef QList<struct bus_channel_mapping> bus_channel_map_t;

static void CreateBusChannelMappingFromString(const QString & str, bus_channel_map_t & map)
{
    QStringList l = str.split(",");
    QString s;

    foreach(s, l) {
        int channel_sep = s.indexOf("=");

	struct bus_channel_mapping map_entry;

	map_entry.channel = s.left(channel_sep);
	map_entry.bus = s.mid(channel_sep + 1);

	map.push_back(map_entry);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QxtCommandOptions options;

    options.add("help", "Show this usage");

    options.add("bus-channel-mapping",
                "List of bus/channel mappings, e.g.: vcan0=Motor,vcan2=Machine",
                QxtCommandOptions::ValueRequired);

    options.add("kcd-file",
                "Path to KCD (Kayak CAN definition file)",
                QxtCommandOptions::ValueRequired);

    options.add("qml-file",
                "Path to local file which describes the HMI view",
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

    if (!options.count("bus-channel-mapping")) {
        qWarning("No bus/channel mapping given");
        return -1;
    }

    QString mappingstr("");
    QString kcdfile = options.value("kcd-file").toString();
    QString qmlfile = options.value("qml-file").toString();

    if (options.count("bus-channel-mapping"))
        mappingstr = options.value("bus-channel-mapping").toString();

    bus_channel_map_t map;

    // Parse bus-channel-mapping argument and build a list busses and channels
    CreateBusChannelMappingFromString(mappingstr, map);

    // Start all CAN channels and attached signal definitions to busses
    struct bus_channel_mapping m;

    QDeclarativeView view;

    foreach(m, map) {
        QCanChannel *c = new QCanChannel(m.channel);
        QCanSignals *s = QCanSignals::createFromKCD(c, kcdfile, m.bus);

        QCanSignalContainer *sc;
        foreach(sc, s->getMessageList()) {
            QCanSignal *cc;

            foreach(cc, sc->getSignalList()) {
                QString fullName = m.bus;

                fullName.append("_");
                fullName.append(sc->getName());
                fullName.append("_");
                fullName.append(cc->getName());

                view.rootContext()->setContextProperty(fullName, cc);
            }
        }

	c->Start();
    }

    view.setSource(QUrl::fromLocalFile(qmlfile));
    view.show();

    return a.exec();
}

