#include "mapwidget.h"
#include "../models/vehiclemodel.h"
#include "../models/missionmodel.h"
#include "../models/waypoint.h"
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QDebug>

MapWidget::MapWidget(QWidget* parent)
    : QQuickWidget(parent)
    , m_vehicleModel(nullptr)
    , m_missionModel(nullptr)
    , m_followVehicle(false)
    , m_homePosition(-35.3632, 149.1654) // Canberra, SITL default
{
    qDebug() << "MapWidget: Initializing...";

    // Setup QML context BEFORE loading source
    setupQmlContext();

    // Set resize mode
    setResizeMode(QQuickWidget::SizeRootObjectToView);

    // Connect status signal
    connect(this, &QQuickWidget::statusChanged,
            this, &MapWidget::onQmlStatusChanged);

    // Set the QML source - using test map first
    qDebug() << "MapWidget: Loading QML from qrc:/qml/MapView.qml";
    // Temporarily use simple test to debug
    // setSource(QUrl("qrc:/qml/TestMap.qml"));
    setSource(QUrl("qrc:/qml/MapView.qml"));

    qDebug() << "MapWidget: Initial status:" << status();
    if (status() == QQuickWidget::Error) {
        qWarning() << "MapWidget: QML errors:" << errors();
    }
}

MapWidget::~MapWidget() {
}

void MapWidget::setupQmlContext() {
    QQmlContext* context = rootContext();
    if (context) {
        context->setContextProperty("mapWidget", this);
        context->setContextProperty("followVehicle", m_followVehicle);
    }
}

void MapWidget::setVehicleModel(VehicleModel* model) {
    if (m_vehicleModel == model) {
        return;
    }

    m_vehicleModel = model;

    if (m_vehicleModel) {
        connectModelSignals();
    }
}

void MapWidget::setMissionModel(MissionModel* model) {
    if (m_missionModel == model) {
        return;
    }

    // Disconnect old model
    if (m_missionModel) {
        disconnect(m_missionModel, nullptr, this, nullptr);
    }

    m_missionModel = model;

    if (m_missionModel) {
        // Connect to mission model signals
        connect(m_missionModel, &MissionModel::waypointAdded,
                this, &MapWidget::onMissionModelChanged);
        connect(m_missionModel, &MissionModel::waypointRemoved,
                this, &MapWidget::onMissionModelChanged);
        connect(m_missionModel, &MissionModel::waypointUpdated,
                this, &MapWidget::onMissionModelChanged);
        connect(m_missionModel, &MissionModel::missionChanged,
                this, &MapWidget::onMissionModelChanged);

        // Initial update
        updateWaypoints();
    }
}

void MapWidget::connectModelSignals() {
    if (!m_vehicleModel) {
        return;
    }

    // Connect to vehicle position updates
    connect(m_vehicleModel, &VehicleModel::latitudeChanged,
            this, &MapWidget::onVehiclePositionChanged);
    connect(m_vehicleModel, &VehicleModel::longitudeChanged,
            this, &MapWidget::onVehiclePositionChanged);
    connect(m_vehicleModel, &VehicleModel::headingChanged,
            this, &MapWidget::onVehiclePositionChanged);
}

void MapWidget::setVehiclePosition(double lat, double lon, double heading) {
    QQuickItem* rootItem = rootObject();
    if (rootItem) {
        QMetaObject::invokeMethod(rootItem, "updateVehiclePosition",
                                 Q_ARG(QVariant, lat),
                                 Q_ARG(QVariant, lon),
                                 Q_ARG(QVariant, heading));

        // Center map on vehicle if follow mode is enabled
        if (m_followVehicle) {
            QMetaObject::invokeMethod(rootItem, "centerOnCoordinate",
                                     Q_ARG(QVariant, lat),
                                     Q_ARG(QVariant, lon));
        }
    }
}

void MapWidget::setHomePosition(double lat, double lon) {
    m_homePosition = QGeoCoordinate(lat, lon);

    QQuickItem* rootItem = rootObject();
    if (rootItem) {
        QMetaObject::invokeMethod(rootItem, "updateHomePosition",
                                 Q_ARG(QVariant, lat),
                                 Q_ARG(QVariant, lon));
    }
}

void MapWidget::setFollowVehicle(bool follow) {
    m_followVehicle = follow;

    QQmlContext* context = rootContext();
    if (context) {
        context->setContextProperty("followVehicle", m_followVehicle);
    }

    if (m_followVehicle && m_vehicleModel) {
        centerOnVehicle();
    }
}

void MapWidget::centerOnHome() {
    if (m_homePosition.isValid()) {
        QQuickItem* rootItem = rootObject();
        if (rootItem) {
            QMetaObject::invokeMethod(rootItem, "centerOnCoordinate",
                                     Q_ARG(QVariant, m_homePosition.latitude()),
                                     Q_ARG(QVariant, m_homePosition.longitude()));
        }
    }
}

void MapWidget::centerOnVehicle() {
    if (m_vehicleModel) {
        QQuickItem* rootItem = rootObject();
        if (rootItem) {
            QMetaObject::invokeMethod(rootItem, "centerOnCoordinate",
                                     Q_ARG(QVariant, m_vehicleModel->latitude()),
                                     Q_ARG(QVariant, m_vehicleModel->longitude()));
        }
    }
}

void MapWidget::addTrailPoint(double lat, double lon) {
    QVariantMap point;
    point["latitude"] = lat;
    point["longitude"] = lon;
    m_trailPoints.append(point);

    // Limit trail length to last 500 points
    if (m_trailPoints.size() > 500) {
        m_trailPoints.removeFirst();
    }

    QQuickItem* rootItem = rootObject();
    if (rootItem) {
        QMetaObject::invokeMethod(rootItem, "updateTrail",
                                 Q_ARG(QVariant, QVariant::fromValue(m_trailPoints)));
    }
}

void MapWidget::clearTrail() {
    m_trailPoints.clear();
    QQuickItem* rootItem = rootObject();
    if (rootItem) {
        QMetaObject::invokeMethod(rootItem, "updateTrail",
                                 Q_ARG(QVariant, QVariant::fromValue(m_trailPoints)));
    }
}

void MapWidget::updateWaypoints() {
    if (!m_missionModel) {
        return;
    }

    QVariantList waypoints;
    for (int i = 0; i < m_missionModel->count(); ++i) {
        const Waypoint* wp = m_missionModel->waypointAt(i);
        if (wp && wp->command() == MAV_CMD_NAV_WAYPOINT) {
            QVariantMap wpData;
            wpData["index"] = i;
            wpData["latitude"] = wp->latitude();
            wpData["longitude"] = wp->longitude();
            wpData["altitude"] = wp->altitude();
            waypoints.append(wpData);
        }
    }

    QQuickItem* rootItem = rootObject();
    if (rootItem) {
        QMetaObject::invokeMethod(rootItem, "updateWaypoints",
                                 Q_ARG(QVariant, QVariant::fromValue(waypoints)));
    }
}

void MapWidget::addWaypoint(int index, double lat, double lon, double alt) {
    QVariantMap wpData;
    wpData["index"] = index;
    wpData["latitude"] = lat;
    wpData["longitude"] = lon;
    wpData["altitude"] = alt;

    QQuickItem* rootItem = rootObject();
    if (rootItem) {
        QMetaObject::invokeMethod(rootItem, "addWaypointMarker",
                                 Q_ARG(QVariant, QVariant::fromValue(wpData)));
    }
}

void MapWidget::removeWaypoint(int index) {
    QQuickItem* rootItem = rootObject();
    if (rootItem) {
        QMetaObject::invokeMethod(rootItem, "removeWaypointMarker",
                                 Q_ARG(QVariant, index));
    }
}

void MapWidget::updateWaypoint(int index, double lat, double lon, double alt) {
    QVariantMap wpData;
    wpData["index"] = index;
    wpData["latitude"] = lat;
    wpData["longitude"] = lon;
    wpData["altitude"] = alt;

    QQuickItem* rootItem = rootObject();
    if (rootItem) {
        QMetaObject::invokeMethod(rootItem, "updateWaypointMarker",
                                 Q_ARG(QVariant, QVariant::fromValue(wpData)));
    }
}

void MapWidget::onQmlStatusChanged(QQuickWidget::Status status) {
    qDebug() << "MapWidget: QML status changed to:" << status;

    switch (status) {
    case QQuickWidget::Null:
        qDebug() << "MapWidget: QML status is Null";
        break;
    case QQuickWidget::Ready:
        qDebug() << "MapWidget: QML loaded successfully!";
        qDebug() << "MapWidget: Root object:" << rootObject();

        // Initialize with current vehicle position if available
        if (m_vehicleModel) {
            onVehiclePositionChanged();
        }

        // Initialize waypoints if mission model is set
        if (m_missionModel) {
            updateWaypoints();
        }
        break;
    case QQuickWidget::Loading:
        qDebug() << "MapWidget: QML is loading...";
        break;
    case QQuickWidget::Error:
        qWarning() << "MapWidget: QML ERRORS detected!";
        const auto errorList = errors();
        for (const auto& error : errorList) {
            qWarning() << "  - Line" << error.line() << ":" << error.description();
        }
        break;
    }
}

void MapWidget::onVehiclePositionChanged() {
    if (!m_vehicleModel) {
        return;
    }

    double lat = m_vehicleModel->latitude();
    double lon = m_vehicleModel->longitude();
    double heading = m_vehicleModel->heading();

    // Update vehicle position on map
    setVehiclePosition(lat, lon, heading);

    // Add to trail if vehicle is armed (flying)
    if (m_vehicleModel->armed()) {
        addTrailPoint(lat, lon);
    }
}

void MapWidget::onMissionModelChanged() {
    updateWaypoints();
}
