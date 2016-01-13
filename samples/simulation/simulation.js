var can = require('socketcan');
var fs = require('fs');

var channel = can.createRawChannel("vcan0");

channel.start();

// Parse database
var network = can.parseNetworkDescription("../../can_definition_sample.kcd");

// Create new DB service an pass database
var db = new can.DatabaseService(channel, network.buses['Motor']);

// Register change listener (we also receive self-made changes)
db.messages['CruiseControlStatus'].signals['SpeedKm'].onChange(function(s) {
   console.log("SpeedKm " + s.value);
});

// Increment power::klemme15 every 100ms
setInterval(function() {
	db.messages['CruiseControlStatus'].signals['SpeedKm'].update(db.messages['CruiseControlStatus'].signals['SpeedKm'].value + 1);
	db.send('CruiseControlStatus');
}, 250)
