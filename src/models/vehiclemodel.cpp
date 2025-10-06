#include "vehiclemodel.h"
#include "mavlink/common/mavlink.h"
#include <QtMath>

VehicleModel::VehicleModel(QObject* parent)
    : QObject(parent), m_systemId(0), m_componentId(0), m_armed(false), m_autopilotEnum(0),
      m_roll(0.0f), m_pitch(0.0f), m_yaw(0.0f), m_rollSpeed(0.0f), m_pitchSpeed(0.0f),
      m_yawSpeed(0.0f), m_latitude(0.0), m_longitude(0.0), m_altitude(0.0f),
      m_relativeAltitude(0.0f), m_heading(0), m_groundSpeed(0.0f), m_airSpeed(0.0f),
      m_climbRate(0.0f), m_batteryVoltage(0.0f), m_batteryCurrent(0.0f), m_batteryRemaining(0),
      m_throttle(0) {}

void VehicleModel::setSystemId(uint8_t id) {
    if (m_systemId != id) {
        m_systemId = id;
        emit systemIdChanged(m_systemId);
    }
}

void VehicleModel::setComponentId(uint8_t id) {
    if (m_componentId != id) {
        m_componentId = id;
        emit componentIdChanged(m_componentId);
    }
}

void VehicleModel::handleHeartbeat(uint8_t systemId, uint8_t componentId, uint8_t autopilot,
                                   uint8_t type, uint8_t systemStatus, uint8_t baseMode,
                                   uint32_t customMode) {
    Q_UNUSED(systemStatus)

    setSystemId(systemId);
    setComponentId(componentId);

    // Decode autopilot type
    if (m_autopilotEnum != autopilot) {
        m_autopilotEnum = autopilot;
        m_autopilotType = decodeAutopilotType(autopilot);
        emit autopilotTypeChanged(m_autopilotType);
    }

    // Decode vehicle type
    QString vehicleType = decodeVehicleType(type);
    if (m_vehicleType != vehicleType) {
        m_vehicleType = vehicleType;
        emit vehicleTypeChanged(m_vehicleType);
    }

    // Check armed state
    bool armed = (baseMode & MAV_MODE_FLAG_SAFETY_ARMED) != 0;
    if (m_armed != armed) {
        m_armed = armed;
        emit armedChanged(m_armed);
    }

    // Decode flight mode
    QString flightMode = decodeFlightMode(baseMode, customMode);
    if (m_flightMode != flightMode) {
        m_flightMode = flightMode;
        emit flightModeChanged(m_flightMode);
    }
}

void VehicleModel::handleAttitude(float roll, float pitch, float yaw, float rollspeed,
                                  float pitchspeed, float yawspeed) {
    // Convert from radians to degrees
    float rollDeg = qRadiansToDegrees(roll);
    float pitchDeg = qRadiansToDegrees(pitch);
    float yawDeg = qRadiansToDegrees(yaw);

    if (!qFuzzyCompare(m_roll, rollDeg)) {
        m_roll = rollDeg;
        emit rollChanged(m_roll);
    }

    if (!qFuzzyCompare(m_pitch, pitchDeg)) {
        m_pitch = pitchDeg;
        emit pitchChanged(m_pitch);
    }

    if (!qFuzzyCompare(m_yaw, yawDeg)) {
        m_yaw = yawDeg;
        emit yawChanged(m_yaw);
    }

    if (!qFuzzyCompare(m_rollSpeed, rollspeed)) {
        m_rollSpeed = rollspeed;
        emit rollSpeedChanged(m_rollSpeed);
    }

    if (!qFuzzyCompare(m_pitchSpeed, pitchspeed)) {
        m_pitchSpeed = pitchspeed;
        emit pitchSpeedChanged(m_pitchSpeed);
    }

    if (!qFuzzyCompare(m_yawSpeed, yawspeed)) {
        m_yawSpeed = yawspeed;
        emit yawSpeedChanged(m_yawSpeed);
    }
}

void VehicleModel::handleGlobalPosition(int32_t lat, int32_t lon, int32_t alt, int32_t relativeAlt,
                                        int16_t vx, int16_t vy, int16_t vz, uint16_t heading) {
    Q_UNUSED(vx)
    Q_UNUSED(vy)
    Q_UNUSED(vz)

    // Convert from 1E7 to degrees
    double latitude = lat / 1e7;
    double longitude = lon / 1e7;

    if (!qFuzzyCompare(m_latitude, latitude)) {
        m_latitude = latitude;
        emit latitudeChanged(m_latitude);
    }

    if (!qFuzzyCompare(m_longitude, longitude)) {
        m_longitude = longitude;
        emit longitudeChanged(m_longitude);
    }

    // Convert from millimeters to meters
    float altitude = alt / 1000.0f;
    float relAlt = relativeAlt / 1000.0f;

    if (!qFuzzyCompare(m_altitude, altitude)) {
        m_altitude = altitude;
        emit altitudeChanged(m_altitude);
    }

    if (!qFuzzyCompare(m_relativeAltitude, relAlt)) {
        m_relativeAltitude = relAlt;
        emit relativeAltitudeChanged(m_relativeAltitude);
    }

    // Convert heading from centidegrees to degrees
    uint16_t headingDeg = heading / 100;
    if (m_heading != headingDeg) {
        m_heading = headingDeg;
        emit headingChanged(m_heading);
    }
}

void VehicleModel::handleVfrHud(float airspeed, float groundspeed, int16_t heading,
                                uint16_t throttle, float alt, float climb) {
    if (!qFuzzyCompare(m_airSpeed, airspeed)) {
        m_airSpeed = airspeed;
        emit airSpeedChanged(m_airSpeed);
    }

    if (!qFuzzyCompare(m_groundSpeed, groundspeed)) {
        m_groundSpeed = groundspeed;
        emit groundSpeedChanged(m_groundSpeed);
    }

    if (m_heading != static_cast<uint16_t>(heading)) {
        m_heading = static_cast<uint16_t>(heading);
        emit headingChanged(m_heading);
    }

    if (m_throttle != throttle) {
        m_throttle = throttle;
        emit throttleChanged(m_throttle);
    }

    if (!qFuzzyCompare(m_altitude, alt)) {
        m_altitude = alt;
        emit altitudeChanged(m_altitude);
    }

    if (!qFuzzyCompare(m_climbRate, climb)) {
        m_climbRate = climb;
        emit climbRateChanged(m_climbRate);
    }
}

void VehicleModel::handleBatteryStatus(uint16_t voltage, int16_t current, int8_t remaining) {
    // Convert from millivolts to volts
    float volts = voltage / 1000.0f;
    if (!qFuzzyCompare(m_batteryVoltage, volts)) {
        m_batteryVoltage = volts;
        emit batteryVoltageChanged(m_batteryVoltage);
    }

    // Convert from centiamps to amps
    float amps = current / 100.0f;
    if (!qFuzzyCompare(m_batteryCurrent, amps)) {
        m_batteryCurrent = amps;
        emit batteryCurrentChanged(m_batteryCurrent);
    }

    if (m_batteryRemaining != remaining) {
        m_batteryRemaining = remaining;
        emit batteryRemainingChanged(m_batteryRemaining);
    }
}

QString VehicleModel::decodeAutopilotType(uint8_t autopilot) {
    switch (autopilot) {
        case MAV_AUTOPILOT_GENERIC:
            return "Generic";
        case MAV_AUTOPILOT_RESERVED:
            return "Reserved";
        case MAV_AUTOPILOT_SLUGS:
            return "SLUGS";
        case MAV_AUTOPILOT_ARDUPILOTMEGA:
            return "ArduPilot";
        case MAV_AUTOPILOT_OPENPILOT:
            return "OpenPilot";
        case MAV_AUTOPILOT_GENERIC_WAYPOINTS_ONLY:
            return "Generic Waypoints";
        case MAV_AUTOPILOT_GENERIC_WAYPOINTS_AND_SIMPLE_NAVIGATION_ONLY:
            return "Generic Nav";
        case MAV_AUTOPILOT_GENERIC_MISSION_FULL:
            return "Generic Mission";
        case MAV_AUTOPILOT_INVALID:
            return "Invalid";
        case MAV_AUTOPILOT_PPZ:
            return "PPZ";
        case MAV_AUTOPILOT_UDB:
            return "UDB";
        case MAV_AUTOPILOT_FP:
            return "FlexiPilot";
        case MAV_AUTOPILOT_PX4:
            return "PX4";
        case MAV_AUTOPILOT_SMACCMPILOT:
            return "SMACCM";
        case MAV_AUTOPILOT_AUTOQUAD:
            return "AutoQuad";
        case MAV_AUTOPILOT_ARMAZILA:
            return "Armazila";
        case MAV_AUTOPILOT_AEROB:
            return "Aerob";
        case MAV_AUTOPILOT_ASLUAV:
            return "ASLUAV";
        default:
            return QString("Unknown (%1)").arg(autopilot);
    }
}

QString VehicleModel::decodeVehicleType(uint8_t type) {
    switch (type) {
        case MAV_TYPE_GENERIC:
            return "Generic";
        case MAV_TYPE_FIXED_WING:
            return "Fixed Wing";
        case MAV_TYPE_QUADROTOR:
            return "Quadrotor";
        case MAV_TYPE_COAXIAL:
            return "Coaxial";
        case MAV_TYPE_HELICOPTER:
            return "Helicopter";
        case MAV_TYPE_ANTENNA_TRACKER:
            return "Antenna Tracker";
        case MAV_TYPE_GCS:
            return "GCS";
        case MAV_TYPE_AIRSHIP:
            return "Airship";
        case MAV_TYPE_FREE_BALLOON:
            return "Balloon";
        case MAV_TYPE_ROCKET:
            return "Rocket";
        case MAV_TYPE_GROUND_ROVER:
            return "Ground Rover";
        case MAV_TYPE_SURFACE_BOAT:
            return "Boat";
        case MAV_TYPE_SUBMARINE:
            return "Submarine";
        case MAV_TYPE_HEXAROTOR:
            return "Hexarotor";
        case MAV_TYPE_OCTOROTOR:
            return "Octorotor";
        case MAV_TYPE_TRICOPTER:
            return "Tricopter";
        case MAV_TYPE_FLAPPING_WING:
            return "Flapping Wing";
        case MAV_TYPE_KITE:
            return "Kite";
        default:
            return QString("Unknown (%1)").arg(type);
    }
}

QString VehicleModel::decodeFlightMode(uint8_t baseMode, uint32_t customMode) {
    // For ArduPilot (most common), custom mode directly maps to flight mode
    if (m_autopilotEnum == MAV_AUTOPILOT_ARDUPILOTMEGA) {
        // ArduCopter modes
        switch (customMode) {
            case 0:
                return "Stabilize";
            case 1:
                return "Acro";
            case 2:
                return "Alt Hold";
            case 3:
                return "Auto";
            case 4:
                return "Guided";
            case 5:
                return "Loiter";
            case 6:
                return "RTL";
            case 7:
                return "Circle";
            case 9:
                return "Land";
            case 11:
                return "Drift";
            case 13:
                return "Sport";
            case 14:
                return "Flip";
            case 15:
                return "AutoTune";
            case 16:
                return "PosHold";
            case 17:
                return "Brake";
            case 18:
                return "Throw";
            case 19:
                return "Avoid ADSB";
            case 20:
                return "Guided NoGPS";
            default:
                return QString("Mode %1").arg(customMode);
        }
    }

    // Generic mode based on base mode flags
    if (baseMode & MAV_MODE_FLAG_AUTO_ENABLED) {
        return "Auto";
    } else if (baseMode & MAV_MODE_FLAG_GUIDED_ENABLED) {
        return "Guided";
    } else if (baseMode & MAV_MODE_FLAG_STABILIZE_ENABLED) {
        return "Stabilize";
    } else if (baseMode & MAV_MODE_FLAG_MANUAL_INPUT_ENABLED) {
        return "Manual";
    }

    return QString("Mode %1").arg(customMode);
}
