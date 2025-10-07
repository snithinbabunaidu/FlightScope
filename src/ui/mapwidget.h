#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QQuickWidget>
#include <QGeoCoordinate>
#include <QVariantList>

class VehicleModel;
class MissionModel;

class MapWidget : public QQuickWidget {
    Q_OBJECT

public:
    explicit MapWidget(QWidget* parent = nullptr);
    ~MapWidget() override;

    // Set references to models
    void setVehicleModel(VehicleModel* model);
    void setMissionModel(MissionModel* model);

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

signals:
    // Map interaction signals
    void mapClicked(double lat, double lon);
    void waypointClicked(int index);
    void waypointMoved(int index, double lat, double lon);

private slots:
    void onQmlStatusChanged(QQuickWidget::Status status);
    void onVehiclePositionChanged();
    void onMissionModelChanged();

private:
    void setupQmlContext();
    void connectModelSignals();

    VehicleModel* m_vehicleModel;
    MissionModel* m_missionModel;

    bool m_followVehicle;
    QGeoCoordinate m_homePosition;
    QVariantList m_trailPoints;
};

#endif // MAPWIDGET_H
