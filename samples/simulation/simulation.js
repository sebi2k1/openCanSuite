var can = require('can');
var signals = require('can_signals');
var fs = require('fs');

var channel = can.createRawChannel("vcan0");

channel.start();

// Parse database
var network = signals.parseNetworkDescription("../../can_definition_sample.kcd");

// Create new DB service an pass database
var dbService = new signals.DatabaseService(channel, network.buses['Motor'].messages);

// Register change listener (we also receive self-made changes)
dbService.messages['CruiseControlStatus'].signals['SpeedKm'].onChange(function(signal) { console.log("Value " + signal.value); });

// Increment power::klemme15 every 100ms
setInterval(function() {
	dbService.messages['CruiseControlStatus'].signals['SpeedKm'].update(dbService.messages['CruiseControlStatus'].signals['SpeedKm'].value + 2);
	dbService.send('CruiseControlStatus');
}, 100)
