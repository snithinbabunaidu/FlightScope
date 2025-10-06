#ifndef HEALTHMODEL_H
#define HEALTHMODEL_H

#include <QObject>
#include <QString>

/**
 * @brief Model representing vehicle health status
 *
 * Tracks GPS fix, satellite count, EKF status, and other health indicators.
 */
class HealthModel : public QObject {
    Q_OBJECT

    // GPS Health
    Q_PROPERTY(QString gpsFixType READ gpsFixType NOTIFY gpsFixTypeChanged)
    Q_PROPERTY(uint8_t satelliteCount READ satelliteCount NOTIFY satelliteCountChanged)
    Q_PROPERTY(uint16_t gpsHdop READ gpsHdop NOTIFY gpsHdopChanged)
    Q_PROPERTY(uint16_t gpsVdop READ gpsVdop NOTIFY gpsVdopChanged)
    Q_PROPERTY(bool gpsHealthy READ gpsHealthy NOTIFY gpsHealthyChanged)

    // EKF Health (simplified for now - full EKF_STATUS_REPORT parsing to be added later)
    Q_PROPERTY(bool ekfHealthy READ ekfHealthy NOTIFY ekfHealthyChanged)

    // System Health
    Q_PROPERTY(QString systemStatus READ systemStatus NOTIFY systemStatusChanged)
    Q_PROPERTY(bool preArmChecksPassed READ preArmChecksPassed NOTIFY preArmChecksPassedChanged)

    // Time Source
    Q_PROPERTY(qint64 clockSkew READ clockSkew NOTIFY clockSkewChanged)
    Q_PROPERTY(QString timeSource READ timeSource NOTIFY timeSourceChanged)

public:
    explicit HealthModel(QObject* parent = nullptr);

    // Getters
    QString gpsFixType() const { return m_gpsFixType; }
    uint8_t satelliteCount() const { return m_satelliteCount; }
    uint16_t gpsHdop() const { return m_gpsHdop; }
    uint16_t gpsVdop() const { return m_gpsVdop; }
    bool gpsHealthy() const { return m_gpsHealthy; }

    bool ekfHealthy() const { return m_ekfHealthy; }

    QString systemStatus() const { return m_systemStatus; }
    bool preArmChecksPassed() const { return m_preArmChecksPassed; }

    qint64 clockSkew() const { return m_clockSkew; }
    QString timeSource() const { return m_timeSource; }

public slots:
    void handleGpsRaw(uint8_t fixType, int32_t lat, int32_t lon, int32_t alt, uint16_t eph,
                      uint16_t epv, uint16_t vel, uint16_t cog, uint8_t satellitesVisible);
    void handleSystemStatus(uint16_t voltage, int16_t currentBattery, int8_t batteryRemaining);
    void handleHeartbeat(uint8_t systemId, uint8_t componentId, uint8_t autopilot, uint8_t type,
                         uint8_t systemStatus, uint8_t baseMode, uint32_t customMode);
    void handleTimesync(int64_t tc1, int64_t ts1);

signals:
    void gpsFixTypeChanged(QString type);
    void satelliteCountChanged(uint8_t count);
    void gpsHdopChanged(uint16_t hdop);
    void gpsVdopChanged(uint16_t vdop);
    void gpsHealthyChanged(bool healthy);

    void ekfHealthyChanged(bool healthy);

    void systemStatusChanged(QString status);
    void preArmChecksPassedChanged(bool passed);

    void clockSkewChanged(qint64 skew);
    void timeSourceChanged(QString source);

private:
    QString decodeGpsFixType(uint8_t fixType);
    QString decodeSystemStatus(uint8_t status);
    void updateGpsHealth();
    void updatePreArmChecks();

    // GPS
    QString m_gpsFixType;
    uint8_t m_gpsFixTypeEnum;
    uint8_t m_satelliteCount;
    uint16_t m_gpsHdop;
    uint16_t m_gpsVdop;
    bool m_gpsHealthy;

    // EKF
    bool m_ekfHealthy;

    // System
    QString m_systemStatus;
    bool m_preArmChecksPassed;

    // Time
    qint64 m_clockSkew;
    QString m_timeSource;
};

#endif  // HEALTHMODEL_H
