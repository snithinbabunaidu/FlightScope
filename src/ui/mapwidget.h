#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QQuickWidget>
#include <QGeoCoordinate>
#include <QVariantList>

class VehicleModel;
class MissionModel;
class GeofenceModel;

class MapWidget : public QQuickWidget {
    Q_OBJECT

    Q_PROPERTY(bool geofenceMode READ geofenceMode WRITE setGeofenceMode NOTIFY geofenceModeChanged)

public:
    explicit MapWidget(QWidget* parent = nullptr);
    ~MapWidget() override;

    // Set references to models
    void setVehicleModel(VehicleModel* model);
    void setMissionModel(MissionModel* model);
    void setGeofenceModel(GeofenceModel* model);

    // Geofence mode
    bool geofenceMode() const { return m_geofenceMode; }
    void setGeofenceMode(bool enabled);

public slots:
    // Vehicle position and state
    void setVehiclePosition(double lat, double lon, double heading);
    void setHomePosition(double lat, double lon);
    void setFollowVehicle(bool follow);
    void centerOnHome();
    void centerOnVehicle();

    // Flight path trail
    void addTrailPoint(double lat, double lon);
    void clearTrail();

    // Mission waypoints
    void updateWaypoints();
    void addWaypoint(int index, double lat, double lon, double alt);
    void removeWaypoint(int index);
    void updateWaypoint(int index, double lat, double lon, double alt);

    // Geofence
    void updateGeofence();
    void addGeofenceVertex(int index, double lat, double lon);
    void removeGeofenceVertex(int index);
    void clearGeofence();

    // Map interaction (callable from QML)
    Q_INVOKABLE void onMapClicked(double lat, double lon);

signals:
    // Map interaction signals
    void mapClicked(double lat, double lon);
    void waypointClicked(int index);
    void waypointMoved(int index, double lat, double lon);
    void geofenceModeChanged(bool enabled);

private slots:
    void onQmlStatusChanged(QQuickWidget::Status status);
    void onVehiclePositionChanged();
    void onMissionModelChanged();

private:
    void setupQmlContext();
    void connectModelSignals();
    void connectGeofenceSignals();

    VehicleModel* m_vehicleModel;
    MissionModel* m_missionModel;
    GeofenceModel* m_geofenceModel;

    bool m_followVehicle;
    bool m_geofenceMode;
    QGeoCoordinate m_homePosition;
    QVariantList m_trailPoints;
};

#endif // MAPWIDGET_H
