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

        // Vehicle marker - Using SVG icon
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
                rotation: vehicleHeading

                Image {
                    anchors.fill: parent
                    source: "qrc:/icons/icons/drone.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }
            }
        }

        // Home marker - Using SVG icon
        MapQuickItem {
            id: homeMarker
            coordinate: QtPositioning.coordinate(homeLat, homeLon)
            anchorPoint.x: homeIcon.width / 2
            anchorPoint.y: homeIcon.height / 2
            visible: homeSet
            zoomLevel: 0

            sourceItem: Item {
                id: homeIcon
                width: 35
                height: 35

                Image {
                    anchors.fill: parent
                    source: "qrc:/icons/icons/home.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
                }
            }
        }

        // Flight path trail - Straight green lines
        MapPolyline {
            id: flightTrail
            line.width: 1.5
            line.color: "#4CAF50"
            opacity: 1.0
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
                    width: 40
                    height: 40

                    // Location pin icon
                    Image {
                        anchors.horizontalCenter: parent.horizontalCenter
                        y: 0
                        width: 30
                        height: 30
                        source: "qrc:/icons/icons/location.svg"
                        fillMode: Image.PreserveAspectFit
                        smooth: true
                    }

                    // Waypoint number badge
                    Rectangle {
                        anchors.horizontalCenter: parent.horizontalCenter
                        y: 8
                        width: 18
                        height: 18
                        radius: 9
                        color: "#FFFFFF"
                        border.color: "#3B1E54"
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: model.index + 1
                            color: "#3B1E54"
                            font.bold: true
                            font.pixelSize: 11
                        }
                    }

                    // Altitude label
                    Rectangle {
                        x: parent.width + 2
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

        // Compass widget (top-left corner) - Simple needle with N/E/W/S
        Item {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 15
            width: 50
            height: 50
            z: 100

            // N label (top)
            Text {
                x: parent.width / 2 - width / 2
                y: 0
                text: "N"
                font.pixelSize: 12
                font.bold: true
                color: "#3B1E54"
            }

            // E label (right)
            Text {
                x: parent.width - width
                y: parent.height / 2 - height / 2
                text: "E"
                font.pixelSize: 12
                font.bold: true
                color: "#666666"
            }

            // S label (bottom)
            Text {
                x: parent.width / 2 - width / 2
                y: parent.height - height
                text: "S"
                font.pixelSize: 12
                font.bold: true
                color: "#666666"
            }

            // W label (left)
            Text {
                x: 0
                y: parent.height / 2 - height / 2
                text: "W"
                font.pixelSize: 12
                font.bold: true
                color: "#666666"
            }

            // Compass needle (rotates with heading)
            Image {
                anchors.centerIn: parent
                width: 30
                height: 30
                source: "qrc:/icons/icons/compass-needle.svg"
                rotation: vehicleHeading
                fillMode: Image.PreserveAspectFit
                smooth: true
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
                radius: 5
                color: "#FFFFFF"
                opacity: 0.9
                border.color: "#CCCCCC"
                border.width: 1

                Image {
                    anchors.centerIn: parent
                    width: 28
                    height: 28
                    source: "qrc:/icons/icons/center-on-drone.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
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
                radius: 5
                color: "#FFFFFF"
                opacity: homeSet ? 0.9 : 0.5
                border.color: "#CCCCCC"
                border.width: 1
                visible: homeSet

                Image {
                    anchors.centerIn: parent
                    width: 24
                    height: 24
                    source: "qrc:/icons/icons/home.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
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
                radius: 5
                color: followMode ? "#FF6F00" : "#FFFFFF"
                opacity: 0.9
                border.color: "#CCCCCC"
                border.width: 1

                Image {
                    anchors.centerIn: parent
                    width: 28
                    height: 28
                    source: "qrc:/icons/icons/follow-drone.svg"
                    fillMode: Image.PreserveAspectFit
                    smooth: true
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
