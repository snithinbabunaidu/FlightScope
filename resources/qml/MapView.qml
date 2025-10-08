import QtQuick
import QtLocation
import QtPositioning

Item {
    id: root

    // Properties
    property real vehicleLat: -35.3632
    property real vehicleLon: 149.1654
    property real vehicleHeading: 0.0
    property real homeLat: -35.3632
    property real homeLon: 149.1654
    property bool homeSet: false
    property bool followMode: false

    // Plugin for OpenStreetMap
    Plugin {
        id: mapPlugin
        name: "osm"
        PluginParameter {
            name: "osm.mapping.cache.directory"
            value: "MapCache"
        }
        PluginParameter {
            name: "osm.mapping.offline.directory"
            value: "MapCache"
        }
    }

    // Main Map
    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(vehicleLat, vehicleLon)
        zoomLevel: 15

        // Map type (Qt 6 uses mapType instead of activeMapType)
        // Default to first available map type (usually street map for OSM)

        // Vehicle marker
        MapQuickItem {
            id: vehicleMarker
            coordinate: QtPositioning.coordinate(vehicleLat, vehicleLon)
            anchorPoint.x: droneIcon.width / 2
            anchorPoint.y: droneIcon.height / 2
            zoomLevel: 0

            sourceItem: Item {
                id: droneIcon
                width: 48
                height: 48

                // Professional drone icon using SVG-style drawing
                Canvas {
                    id: droneCanvas
                    anchors.fill: parent
                    rotation: vehicleHeading

                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.clearRect(0, 0, width, height);

                        var centerX = width / 2;
                        var centerY = height / 2;

                        // Draw arms
                        ctx.strokeStyle = "#333333";
                        ctx.lineWidth = 2.5;
                        ctx.lineCap = "round";

                        // Front-right arm
                        ctx.beginPath();
                        ctx.moveTo(centerX + 4, centerY - 4);
                        ctx.lineTo(centerX + 14, centerY - 14);
                        ctx.stroke();

                        // Front-left arm
                        ctx.beginPath();
                        ctx.moveTo(centerX - 4, centerY - 4);
                        ctx.lineTo(centerX - 14, centerY - 14);
                        ctx.stroke();

                        // Back-right arm
                        ctx.beginPath();
                        ctx.moveTo(centerX + 4, centerY + 4);
                        ctx.lineTo(centerX + 14, centerY + 14);
                        ctx.stroke();

                        // Back-left arm
                        ctx.beginPath();
                        ctx.moveTo(centerX - 4, centerY + 4);
                        ctx.lineTo(centerX - 14, centerY + 14);
                        ctx.stroke();

                        // Draw motors/propellers
                        ctx.fillStyle = "#666666";
                        ctx.strokeStyle = "#333333";
                        ctx.lineWidth = 1;

                        // Front-right motor
                        ctx.beginPath();
                        ctx.arc(centerX + 14, centerY - 14, 4, 0, 2 * Math.PI);
                        ctx.fill();
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.arc(centerX + 14, centerY - 14, 6, 0, 2 * Math.PI);
                        ctx.strokeStyle = "#999999";
                        ctx.globalAlpha = 0.5;
                        ctx.stroke();
                        ctx.globalAlpha = 1.0;

                        // Front-left motor
                        ctx.fillStyle = "#666666";
                        ctx.strokeStyle = "#333333";
                        ctx.beginPath();
                        ctx.arc(centerX - 14, centerY - 14, 4, 0, 2 * Math.PI);
                        ctx.fill();
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.arc(centerX - 14, centerY - 14, 6, 0, 2 * Math.PI);
                        ctx.strokeStyle = "#999999";
                        ctx.globalAlpha = 0.5;
                        ctx.stroke();
                        ctx.globalAlpha = 1.0;

                        // Back-right motor
                        ctx.fillStyle = "#666666";
                        ctx.strokeStyle = "#333333";
                        ctx.beginPath();
                        ctx.arc(centerX + 14, centerY + 14, 4, 0, 2 * Math.PI);
                        ctx.fill();
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.arc(centerX + 14, centerY + 14, 6, 0, 2 * Math.PI);
                        ctx.strokeStyle = "#999999";
                        ctx.globalAlpha = 0.5;
                        ctx.stroke();
                        ctx.globalAlpha = 1.0;

                        // Back-left motor
                        ctx.fillStyle = "#666666";
                        ctx.strokeStyle = "#333333";
                        ctx.beginPath();
                        ctx.arc(centerX - 14, centerY + 14, 4, 0, 2 * Math.PI);
                        ctx.fill();
                        ctx.stroke();
                        ctx.beginPath();
                        ctx.arc(centerX - 14, centerY + 14, 6, 0, 2 * Math.PI);
                        ctx.strokeStyle = "#999999";
                        ctx.globalAlpha = 0.5;
                        ctx.stroke();
                        ctx.globalAlpha = 1.0;

                        // Draw body (center)
                        ctx.fillStyle = "#FF4444";
                        ctx.strokeStyle = "#CC0000";
                        ctx.lineWidth = 1.5;
                        ctx.beginPath();
                        ctx.arc(centerX, centerY, 6, 0, 2 * Math.PI);
                        ctx.fill();
                        ctx.stroke();

                        // Heading indicator (front) - Green arrow
                        ctx.fillStyle = "#00FF00";
                        ctx.strokeStyle = "#00CC00";
                        ctx.lineWidth = 1;
                        ctx.beginPath();
                        ctx.moveTo(centerX, centerY - 8);      // Arrow tip
                        ctx.lineTo(centerX - 3, centerY - 16);  // Left point
                        ctx.lineTo(centerX, centerY - 13);      // Center notch
                        ctx.lineTo(centerX + 3, centerY - 16);  // Right point
                        ctx.closePath();
                        ctx.fill();
                        ctx.stroke();

                        // Center marker (white dot)
                        ctx.fillStyle = "#FFFFFF";
                        ctx.beginPath();
                        ctx.arc(centerX, centerY, 1.5, 0, 2 * Math.PI);
                        ctx.fill();
                    }

                    Connections {
                        target: root
                        function onVehicleHeadingChanged() {
                            droneCanvas.requestPaint();
                        }
                    }
                }
            }
        }

        // Home marker
        MapQuickItem {
            id: homeMarker
            coordinate: QtPositioning.coordinate(homeLat, homeLon)
            anchorPoint.x: homeIcon.width / 2
            anchorPoint.y: homeIcon.height / 2
            visible: homeSet
            zoomLevel: 0

            sourceItem: Item {
                id: homeIcon
                width: 30
                height: 30

                // Home shape (house icon)
                Canvas {
                    anchors.fill: parent

                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.clearRect(0, 0, width, height);

                        // Draw house
                        ctx.fillStyle = "#4444FF";
                        ctx.strokeStyle = "#FFFFFF";
                        ctx.lineWidth = 2;

                        // Roof (triangle)
                        ctx.beginPath();
                        ctx.moveTo(width / 2, 5);
                        ctx.lineTo(width - 5, height / 2);
                        ctx.lineTo(5, height / 2);
                        ctx.closePath();
                        ctx.fill();
                        ctx.stroke();

                        // Base (rectangle)
                        ctx.fillRect(8, height / 2, width - 16, height / 2 - 5);
                        ctx.strokeRect(8, height / 2, width - 16, height / 2 - 5);
                    }
                }
            }
        }

        // Flight path trail
        MapPolyline {
            id: flightTrail
            line.width: 3
            line.color: "#44FF44"
            opacity: 0.7
        }

        // Mission path (connecting waypoints)
        MapPolyline {
            id: missionPath
            line.width: 2
            line.color: "#FFAA00"
            // Note: line.style was removed in Qt 6 - using solid line
            opacity: 0.8
        }

        // Waypoint markers (will be added dynamically)
        MapItemView {
            id: waypointView
            model: ListModel {
                id: waypointModel
            }

            delegate: MapQuickItem {
                coordinate: QtPositioning.coordinate(model.latitude, model.longitude)
                anchorPoint.x: waypointIcon.width / 2
                anchorPoint.y: waypointIcon.height / 2
                zoomLevel: 0

                sourceItem: Item {
                    id: waypointIcon
                    width: 35
                    height: 35

                    // Waypoint circle with number
                    Rectangle {
                        anchors.fill: parent
                        radius: width / 2
                        color: "#FFAA00"
                        border.color: "#FFFFFF"
                        border.width: 2

                        Text {
                            anchors.centerIn: parent
                            text: model.index + 1
                            color: "#FFFFFF"
                            font.bold: true
                            font.pixelSize: 14
                        }
                    }

                    // Altitude label
                    Rectangle {
                        x: parent.width + 5
                        y: parent.height / 2 - height / 2
                        width: altText.width + 8
                        height: 20
                        color: "#000000"
                        opacity: 0.7
                        radius: 3

                        Text {
                            id: altText
                            anchors.centerIn: parent
                            text: model.altitude.toFixed(1) + "m"
                            color: "#FFFFFF"
                            font.pixelSize: 11
                        }
                    }
                }
            }
        }

        // Map controls overlay (right side - zoom)
        Column {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 10
            spacing: 5

            // Zoom in button
            Rectangle {
                width: 40
                height: 40
                color: "#FFFFFF"
                opacity: 0.9
                radius: 5
                border.color: "#CCCCCC"

                Text {
                    anchors.centerIn: parent
                    text: "+"
                    font.pixelSize: 24
                    font.bold: true
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: map.zoomLevel = Math.min(map.zoomLevel + 1, map.maximumZoomLevel)
                }
            }

            // Zoom out button
            Rectangle {
                width: 40
                height: 40
                color: "#FFFFFF"
                opacity: 0.9
                radius: 5
                border.color: "#CCCCCC"

                Text {
                    anchors.centerIn: parent
                    text: "-"
                    font.pixelSize: 24
                    font.bold: true
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: map.zoomLevel = Math.max(map.zoomLevel - 1, map.minimumZoomLevel)
                }
            }
        }

        // Bottom control toolbar
        Row {
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 15
            spacing: 10

            // Center on Drone button
            Rectangle {
                width: 45
                height: 45
                radius: 23
                color: "#FFFFFF"
                opacity: 0.95
                border.color: "#0277BD"
                border.width: 2

                Text {
                    anchors.centerIn: parent
                    text: "🎯"
                    font.pixelSize: 20
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        map.center = QtPositioning.coordinate(vehicleLat, vehicleLon);
                    }
                }

                // Tooltip effect
                Rectangle {
                    anchors.bottom: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottomMargin: 5
                    width: tooltipText1.width + 12
                    height: 25
                    color: "#000000"
                    opacity: parent.children[1].containsMouse ? 0.8 : 0
                    radius: 4
                    visible: opacity > 0

                    Behavior on opacity { NumberAnimation { duration: 200 } }

                    Text {
                        id: tooltipText1
                        anchors.centerIn: parent
                        text: "Center on Drone"
                        color: "#FFFFFF"
                        font.pixelSize: 11
                    }
                }
            }

            // Center on Home button
            Rectangle {
                width: 45
                height: 45
                radius: 23
                color: "#FFFFFF"
                opacity: homeSet ? 0.95 : 0.5
                border.color: "#4CAF50"
                border.width: 2
                visible: homeSet

                Text {
                    anchors.centerIn: parent
                    text: "🏠"
                    font.pixelSize: 20
                }

                MouseArea {
                    anchors.fill: parent
                    enabled: homeSet
                    onClicked: {
                        map.center = QtPositioning.coordinate(homeLat, homeLon);
                    }
                }

                Rectangle {
                    anchors.bottom: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottomMargin: 5
                    width: tooltipText2.width + 12
                    height: 25
                    color: "#000000"
                    opacity: parent.children[1].containsMouse && homeSet ? 0.8 : 0
                    radius: 4
                    visible: opacity > 0

                    Behavior on opacity { NumberAnimation { duration: 200 } }

                    Text {
                        id: tooltipText2
                        anchors.centerIn: parent
                        text: "Center on Home"
                        color: "#FFFFFF"
                        font.pixelSize: 11
                    }
                }
            }

            // Follow Mode toggle
            Rectangle {
                width: 45
                height: 45
                radius: 23
                color: followMode ? "#FF6F00" : "#FFFFFF"
                opacity: 0.95
                border.color: followMode ? "#F57C00" : "#FF9800"
                border.width: 2

                Text {
                    anchors.centerIn: parent
                    text: "📍"
                    font.pixelSize: 20
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        followMode = !followMode;
                        if (followMode) {
                            map.center = QtPositioning.coordinate(vehicleLat, vehicleLon);
                        }
                    }
                }

                Rectangle {
                    anchors.bottom: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottomMargin: 5
                    width: tooltipText3.width + 12
                    height: 25
                    color: "#000000"
                    opacity: parent.children[1].containsMouse ? 0.8 : 0
                    radius: 4
                    visible: opacity > 0

                    Behavior on opacity { NumberAnimation { duration: 200 } }

                    Text {
                        id: tooltipText3
                        anchors.centerIn: parent
                        text: followMode ? "Follow: ON" : "Follow: OFF"
                        color: "#FFFFFF"
                        font.pixelSize: 11
                    }
                }
            }
        }
    }

    // Functions callable from C++
    function updateVehiclePosition(lat, lon, heading) {
        vehicleLat = lat;
        vehicleLon = lon;
        vehicleHeading = heading;

        // Auto-center map if follow mode is enabled
        if (followMode) {
            map.center = QtPositioning.coordinate(lat, lon);
        }
    }

    function updateHomePosition(lat, lon) {
        homeLat = lat;
        homeLon = lon;
        homeSet = true;
    }

    function centerOnCoordinate(lat, lon) {
        map.center = QtPositioning.coordinate(lat, lon);
    }

    function updateTrail(trailPoints) {
        var path = [];
        for (var i = 0; i < trailPoints.length; i++) {
            path.push(QtPositioning.coordinate(
                trailPoints[i].latitude,
                trailPoints[i].longitude
            ));
        }
        flightTrail.path = path;
    }

    function updateWaypoints(waypoints) {
        waypointModel.clear();
        var missionPathCoords = [];

        for (var i = 0; i < waypoints.length; i++) {
            waypointModel.append({
                "index": waypoints[i].index,
                "latitude": waypoints[i].latitude,
                "longitude": waypoints[i].longitude,
                "altitude": waypoints[i].altitude
            });

            missionPathCoords.push(QtPositioning.coordinate(
                waypoints[i].latitude,
                waypoints[i].longitude
            ));
        }

        missionPath.path = missionPathCoords;
    }

    function addWaypointMarker(wpData) {
        waypointModel.append({
            "index": wpData.index,
            "latitude": wpData.latitude,
            "longitude": wpData.longitude,
            "altitude": wpData.altitude
        });
        updateMissionPath();
    }

    function removeWaypointMarker(index) {
        for (var i = 0; i < waypointModel.count; i++) {
            if (waypointModel.get(i).index === index) {
                waypointModel.remove(i);
                break;
            }
        }
        updateMissionPath();
    }

    function updateWaypointMarker(wpData) {
        for (var i = 0; i < waypointModel.count; i++) {
            if (waypointModel.get(i).index === wpData.index) {
                waypointModel.set(i, {
                    "index": wpData.index,
                    "latitude": wpData.latitude,
                    "longitude": wpData.longitude,
                    "altitude": wpData.altitude
                });
                break;
            }
        }
        updateMissionPath();
    }

    function updateMissionPath() {
        var missionPathCoords = [];
        for (var i = 0; i < waypointModel.count; i++) {
            var wp = waypointModel.get(i);
            missionPathCoords.push(QtPositioning.coordinate(wp.latitude, wp.longitude));
        }
        missionPath.path = missionPathCoords;
    }
}
