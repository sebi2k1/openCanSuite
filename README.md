openCanAnalyzer
==

This project is an approach to build a tool for analyzing CAN (Controller Area Network)
networks by providing display elements (charts, numeric indicators) for analyzing the
behaviour of certain nodes.

The project will resemble KCD file format (see Kayak project) to handled network and
message descriptions.

Installation
==
  $ qmake
  $ make clean all

Usage
==
You need to supply the CAN channel, the signal database and scale information:
    $ canPlotter/canPlotter --channel vcan0 --kcd-file ./can_definition_sample.kcd --busname Motor --left-scale-name "Speed" --left-scale-signals="CruiseControlStatus.SpeedKm/red,CruiseControlStatus.SpeedKm/yellow"

