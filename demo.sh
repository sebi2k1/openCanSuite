#!/bin/sh

sudo modprobe can
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan 2>/dev/null
sudo ip link set up vcan0

canHmi/canHmi --bus-channel-mapping=vcan0=Motor,vcan0=Instrumentation,vcan0=Comfort --kcd-file ./can_definition_sample.kcd --qml-file=samples/QtAutomotiveClusterDemo/qml_800x480.qml &
pid1=$!
sleep 2
cd samples/simulation ; node simulation.js > /dev/null &
pid2=$!
cd ../..
wait $pid1
kill -s TERM $pid2
