import QtQuick 2.0

Rectangle {
	id: screen
	width: 400; height: 500
	color: "grey"

	Text {
		id: value
		text: "SpeedKm " + Motor_CruiseControlStatus_SpeedKm.value
	}
}
