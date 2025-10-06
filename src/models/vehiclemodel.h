#ifndef VEHICLEMODEL_H
#define VEHICLEMODEL_H

#include <QObject>
#include <QString>

/**
 * @brief Model representing the vehicle's state and telemetry
 *
 * All properties use Q_PROPERTY for automatic notification and QML binding.
 * This class aggregates all primary telemetry data from the vehicle.
 */
class VehicleModel : public QObject {
    Q_OBJECT

    // System Info
    Q_PROPERTY(uint8_t systemId READ systemId WRITE setSystemId NOTIFY systemIdChanged)
    Q_PROPERTY(uint8_t componentId READ componentId WRITE setComponentId NOTIFY componentIdChanged)
    Q_PROPERTY(QString autopilotType READ autopilotType NOTIFY autopilotTypeChanged)
    Q_PROPERTY(QString vehicleType READ vehicleType NOTIFY vehicleTypeChanged)
    Q_PROPERTY(QString flightMode READ flightMode NOTIFY flightModeChanged)
    Q_PROPERTY(bool armed READ armed NOTIFY armedChanged)

    // Attitude
    Q_PROPERTY(float roll READ roll NOTIFY rollChanged)
    Q_PROPERTY(float pitch READ pitch NOTIFY pitchChanged)
    Q_PROPERTY(float yaw READ yaw NOTIFY yawChanged)
    Q_PROPERTY(float rollSpeed READ rollSpeed NOTIFY rollSpeedChanged)
    Q_PROPERTY(float pitchSpeed READ pitchSpeed NOTIFY pitchSpeedChanged)
    Q_PROPERTY(float yawSpeed READ yawSpeed NOTIFY yawSpeedChanged)

    // Position
    Q_PROPERTY(double latitude READ latitude NOTIFY latitudeChanged)
    Q_PROPERTY(double longitude READ longitude NOTIFY longitudeChanged)
    Q_PROPERTY(float altitude READ altitude NOTIFY altitudeChanged)
    Q_PROPERTY(float relativeAltitude READ relativeAltitude NOTIFY relativeAltitudeChanged)
    Q_PROPERTY(uint16_t heading READ heading NOTIFY headingChanged)

    // Velocity
    Q_PROPERTY(float groundSpeed READ groundSpeed NOTIFY groundSpeedChanged)
    Q_PROPERTY(float airSpeed READ airSpeed NOTIFY airSpeedChanged)
    Q_PROPERTY(float climbRate READ climbRate NOTIFY climbRateChanged)

    // Battery
    Q_PROPERTY(float batteryVoltage READ batteryVoltage NOTIFY batteryVoltageChanged)
    Q_PROPERTY(float batteryCurrent READ batteryCurrent NOTIFY batteryCurrentChanged)
    Q_PROPERTY(int batteryRemaining READ batteryRemaining NOTIFY batteryRemainingChanged)

    // Other
    Q_PROPERTY(uint16_t throttle READ throttle NOTIFY throttleChanged)

public:
    explicit VehicleModel(QObject* parent = nullptr);

    // Getters
    uint8_t systemId() const { return m_systemId; }
    uint8_t componentId() const { return m_componentId; }
    QString autopilotType() const { return m_autopilotType; }
    QString vehicleType() const { return m_vehicleType; }
    QString flightMode() const { return m_flightMode; }
    bool armed() const { return m_armed; }

    float roll() const { return m_roll; }
    float pitch() const { return m_pitch; }
    float yaw() const { return m_yaw; }
    float rollSpeed() const { return m_rollSpeed; }
    float pitchSpeed() const { return m_pitchSpeed; }
    float yawSpeed() const { return m_yawSpeed; }

    double latitude() const { return m_latitude; }
    double longitude() const { return m_longitude; }
    float altitude() const { return m_altitude; }
    float relativeAltitude() const { return m_relativeAltitude; }
    uint16_t heading() const { return m_heading; }

    float groundSpeed() const { return m_groundSpeed; }
    float airSpeed() const { return m_airSpeed; }
    float climbRate() const { return m_climbRate; }

    float batteryVoltage() const { return m_batteryVoltage; }
    float batteryCurrent() const { return m_batteryCurrent; }
    int batteryRemaining() const { return m_batteryRemaining; }

    uint16_t throttle() const { return m_throttle; }

    // Setters
    void setSystemId(uint8_t id);
    void setComponentId(uint8_t id);

public slots:
    void handleHeartbeat(uint8_t systemId, uint8_t componentId, uint8_t autopilot, uint8_t type,
                         uint8_t systemStatus, uint8_t baseMode, uint32_t customMode);
    void handleAttitude(float roll, float pitch, float yaw, float rollspeed, float pitchspeed,
                        float yawspeed);
    void handleGlobalPosition(int32_t lat, int32_t lon, int32_t alt, int32_t relativeAlt,
                              int16_t vx, int16_t vy, int16_t vz, uint16_t heading);
    void handleVfrHud(float airspeed, float groundspeed, int16_t heading, uint16_t throttle,
                      float alt, float climb);
    void handleBatteryStatus(uint16_t voltage, int16_t current, int8_t remaining);

signals:
    void systemIdChanged(uint8_t systemId);
    void componentIdChanged(uint8_t componentId);
    void autopilotTypeChanged(QString type);
    void vehicleTypeChanged(QString type);
    void flightModeChanged(QString mode);
    void armedChanged(bool armed);

    void rollChanged(float roll);
    void pitchChanged(float pitch);
    void yawChanged(float yaw);
    void rollSpeedChanged(float speed);
    void pitchSpeedChanged(float speed);
    void yawSpeedChanged(float speed);

    void latitudeChanged(double lat);
    void longitudeChanged(double lon);
    void altitudeChanged(float alt);
    void relativeAltitudeChanged(float alt);
    void headingChanged(uint16_t heading);

    void groundSpeedChanged(float speed);
    void airSpeedChanged(float speed);
    void climbRateChanged(float rate);

    void batteryVoltageChanged(float voltage);
    void batteryCurrentChanged(float current);
    void batteryRemainingChanged(int remaining);

    void throttleChanged(uint16_t throttle);

private:
    QString decodeAutopilotType(uint8_t autopilot);
    QString decodeVehicleType(uint8_t type);
    QString decodeFlightMode(uint8_t baseMode, uint32_t customMode);

    uint8_t m_systemId;
    uint8_t m_componentId;
    QString m_autopilotType;
    QString m_vehicleType;
    QString m_flightMode;
    bool m_armed;
    uint8_t m_autopilotEnum;

    float m_roll;
    float m_pitch;
    float m_yaw;
    float m_rollSpeed;
    float m_pitchSpeed;
    float m_yawSpeed;

    double m_latitude;
    double m_longitude;
    float m_altitude;
    float m_relativeAltitude;
    uint16_t m_heading;

    float m_groundSpeed;
    float m_airSpeed;
    float m_climbRate;

    float m_batteryVoltage;
    float m_batteryCurrent;
    int m_batteryRemaining;

    uint16_t m_throttle;
};

#endif  // VEHICLEMODEL_H
