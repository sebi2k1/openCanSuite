var can = require('socketcan');
var fs = require('fs');

var channel = can.createRawChannel("vcan0");

channel.start();

// Parse database
var network = can.parseNetworkDescription("../../can_definition_sample.kcd");

// Create new DB service an pass database
var motor = new can.DatabaseService(channel, network.buses['Motor']);
var instr = new can.DatabaseService(channel, network.buses['Instrumentation']);
var comfort = new can.DatabaseService(channel, network.buses['Comfort']);

// Register change listener (we also receive self-made changes)
motor.messages['CruiseControlStatus'].signals['SpeedKm'].onChange(function(s) {
   console.log("SpeedKm " + s.value);
});

instr.messages['Headlights'].signals['Turnsignal_L'].onChange(function(s) {
   console.log("Turnsignal_L " + s.value);
});

instr.messages['Headlights'].signals['Fog'].onChange(function(s) {
   console.log("Fog " + s.value);
});
instr.messages['Headlights'].signals['High'].onChange(function(s) {
   console.log("High " + s.value);
});

var inc = 1;
// Increment power::klemme15 every 100ms
setInterval(function() {
	var speed = motor.messages['CruiseControlStatus'].signals['SpeedKm'].value + inc;
	if(speed >= 100) { inc = -1; }
	if(speed <= 0) { inc = 1; }

	motor.messages['CruiseControlStatus'].signals['SpeedKm'].update(speed);
	motor.send('CruiseControlStatus');
}, 150)

var turn = 0;
setInterval(function() {
	instr.messages['Headlights'].signals['Turnsignal_L'].update(turn==1?1:0);
	instr.messages['Headlights'].signals['Turnsignal_R'].update(turn==2?1:0);
	instr.send('Headlights');
	turn = (++turn)%3;
}, 3000)

var headlights = 0;
setInterval(function() {
	instr.messages['Headlights'].signals['Fog'].update(headlights==1?1:0);
	instr.messages['Headlights'].signals['High'].update(headlights==2?1:0);
	instr.send('Headlights');
	headlights = ((++headlights)%3);
}, 5000)

var tank = 100;
setInterval(function() {
	tank = (tank-10+100)%100;
	instr.messages['TankController'].signals['TankLevel'].update(tank);
	instr.send('TankController');
}, 2000)
