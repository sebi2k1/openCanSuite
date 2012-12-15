openCanSuite
==

This project is an approach to build a set of tools for analyzing, simulating and visualizing
a CAN system (Controller Area Network)

canAnalyzer
===
This tool is still in draft phase. In long term this tool shall allow to monitor the CAN
and send CAN messages.

canPlotter
===
The plotter supports displaying various signals on a simple plot chart.

canHmi
===
Indented to be used as a base for a machine HMI (human machine interface) using QML to describe
the visualization.

The project will resemble KCD file format (see Kayak project) to handled network and
message descriptions.

Prerequisites
==
For building this under Ubuntu 12.04, please install:
  $ apt-get install libqwt-dev libqxt-dev

Installation
==
  $ qmake
  $ make clean all

Usage
==
You need to supply the CAN channel, the signal database and scale information:
    $ canPlotter/canPlotter --channel vcan0 --kcd-file ./can_definition_sample.kcd --busname Motor --left-scale-name "Speed" --left-scale-signals="CruiseControlStatus.SpeedKm/red,CruiseControlStatus.SpeedKm/yellow"

