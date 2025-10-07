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
                width: 40
                height: 40

                // Drone shape (triangle pointing in heading direction)
                Canvas {
                    id: droneCanvas
                    anchors.fill: parent
                    rotation: vehicleHeading

                    onPaint: {
                        var ctx = getContext("2d");
                        ctx.clearRect(0, 0, width, height);

                        // Draw drone triangle
                        ctx.fillStyle = "#FF4444";
                        ctx.strokeStyle = "#FFFFFF";
                        ctx.lineWidth = 2;

                        ctx.beginPath();
                        ctx.moveTo(width / 2, 5);        // Top point (nose)
                        ctx.lineTo(width - 5, height - 5); // Bottom right
                        ctx.lineTo(5, height - 5);         // Bottom left
                        ctx.closePath();

                        ctx.fill();
                        ctx.stroke();
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

        // Map controls overlay
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
    }

    // Functions callable from C++
    function updateVehiclePosition(lat, lon, heading) {
        vehicleLat = lat;
        vehicleLon = lon;
        vehicleHeading = heading;
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
