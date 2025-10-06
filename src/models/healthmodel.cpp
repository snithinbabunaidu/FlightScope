#include "healthmodel.h"
#include "mavlink/common/mavlink.h"
#include <QDateTime>

HealthModel::HealthModel(QObject* parent)
    : QObject(parent), m_gpsFixTypeEnum(0), m_satelliteCount(0), m_gpsHdop(0), m_gpsVdop(0),
      m_gpsHealthy(false), m_ekfHealthy(false), m_preArmChecksPassed(false), m_clockSkew(0),
      m_timeSource("Unknown") {
    m_gpsFixType = "No Fix";
    m_systemStatus = "Unknown";
}

void HealthModel::handleGpsRaw(uint8_t fixType, int32_t lat, int32_t lon, int32_t alt,
                               uint16_t eph, uint16_t epv, uint16_t vel, uint16_t cog,
                               uint8_t satellitesVisible) {
    Q_UNUSED(lat)
    Q_UNUSED(lon)
    Q_UNUSED(alt)
    Q_UNUSED(vel)
    Q_UNUSED(cog)

    // Update GPS fix type
    if (m_gpsFixTypeEnum != fixType) {
        m_gpsFixTypeEnum = fixType;
        m_gpsFixType = decodeGpsFixType(fixType);
        emit gpsFixTypeChanged(m_gpsFixType);
        updateGpsHealth();
    }

    // Update satellite count
    if (m_satelliteCount != satellitesVisible) {
        m_satelliteCount = satellitesVisible;
        emit satelliteCountChanged(m_satelliteCount);
        updateGpsHealth();
    }

    // Update HDOP (horizontal dilution of precision)
    if (m_gpsHdop != eph) {
        m_gpsHdop = eph;
        emit gpsHdopChanged(m_gpsHdop);
        updateGpsHealth();
    }

    // Update VDOP (vertical dilution of precision)
    if (m_gpsVdop != epv) {
        m_gpsVdop = epv;
        emit gpsVdopChanged(m_gpsVdop);
    }
}

void HealthModel::handleSystemStatus(uint16_t voltage, int16_t currentBattery,
                                     int8_t batteryRemaining) {
    Q_UNUSED(voltage)
    Q_UNUSED(currentBattery)
    Q_UNUSED(batteryRemaining)
    // Basic system status handling
    // More detailed status from SYS_STATUS message if needed
}

void HealthModel::handleHeartbeat(uint8_t systemId, uint8_t componentId, uint8_t autopilot,
                                  uint8_t type, uint8_t systemStatus, uint8_t baseMode,
                                  uint32_t customMode) {
    Q_UNUSED(systemId)
    Q_UNUSED(componentId)
    Q_UNUSED(autopilot)
    Q_UNUSED(type)
    Q_UNUSED(baseMode)
    Q_UNUSED(customMode)

    // Update system status
    QString status = decodeSystemStatus(systemStatus);
    if (m_systemStatus != status) {
        m_systemStatus = status;
        emit systemStatusChanged(m_systemStatus);
    }

    updatePreArmChecks();
}

void HealthModel::handleTimesync(int64_t tc1, int64_t ts1) {
    // Calculate clock skew
    if (tc1 != 0) {
        // This is a response, calculate skew
        qint64 now = QDateTime::currentMSecsSinceEpoch() * 1000;  // Convert to microseconds
        qint64 skew = now - ts1;

        if (m_clockSkew != skew) {
            m_clockSkew = skew / 1000;  // Convert to milliseconds
            emit clockSkewChanged(m_clockSkew);

            if (m_timeSource != "TIMESYNC") {
                m_timeSource = "TIMESYNC";
                emit timeSourceChanged(m_timeSource);
            }
        }
    }
}

QString HealthModel::decodeGpsFixType(uint8_t fixType) {
    switch (fixType) {
        case GPS_FIX_TYPE_NO_GPS:
            return "No GPS";
        case GPS_FIX_TYPE_NO_FIX:
            return "No Fix";
        case GPS_FIX_TYPE_2D_FIX:
            return "2D Fix";
        case GPS_FIX_TYPE_3D_FIX:
            return "3D Fix";
        case GPS_FIX_TYPE_DGPS:
            return "DGPS";
        case GPS_FIX_TYPE_RTK_FLOAT:
            return "RTK Float";
        case GPS_FIX_TYPE_RTK_FIXED:
            return "RTK Fixed";
        case GPS_FIX_TYPE_STATIC:
            return "Static";
        case GPS_FIX_TYPE_PPP:
            return "PPP";
        default:
            return QString("Unknown (%1)").arg(fixType);
    }
}

QString HealthModel::decodeSystemStatus(uint8_t status) {
    switch (status) {
        case MAV_STATE_UNINIT:
            return "Uninitialized";
        case MAV_STATE_BOOT:
            return "Booting";
        case MAV_STATE_CALIBRATING:
            return "Calibrating";
        case MAV_STATE_STANDBY:
            return "Standby";
        case MAV_STATE_ACTIVE:
            return "Active";
        case MAV_STATE_CRITICAL:
            return "Critical";
        case MAV_STATE_EMERGENCY:
            return "Emergency";
        case MAV_STATE_POWEROFF:
            return "Power Off";
        case MAV_STATE_FLIGHT_TERMINATION:
            return "Flight Termination";
        default:
            return QString("Unknown (%1)").arg(status);
    }
}

void HealthModel::updateGpsHealth() {
    // GPS is healthy if we have 3D fix or better and at least 6 satellites
    bool healthy = (m_gpsFixTypeEnum >= GPS_FIX_TYPE_3D_FIX) && (m_satelliteCount >= 6);

    if (m_gpsHealthy != healthy) {
        m_gpsHealthy = healthy;
        emit gpsHealthyChanged(m_gpsHealthy);
        updatePreArmChecks();
    }
}

void HealthModel::updatePreArmChecks() {
    // Simple pre-arm check: GPS must be healthy and system must be in standby or active
    bool passed = m_gpsHealthy &&
                  (m_systemStatus == "Standby" || m_systemStatus == "Active" ||
                   m_systemStatus == "Critical");

    if (m_preArmChecksPassed != passed) {
        m_preArmChecksPassed = passed;
        emit preArmChecksPassedChanged(m_preArmChecksPassed);
    }
}
