#include "mavlinkrouter.h"
#include <QDebug>
#include <QDateTime>

MavlinkRouter::MavlinkRouter(QObject* parent)
    : QObject(parent), m_systemId(255), m_componentId(190), m_lastSeq(0), m_receivedPackets(0),
      m_droppedPackets(0), m_packetLoss(0.0f), m_roundTripTime(0), m_timesyncTc1(0) {
    memset(&m_status, 0, sizeof(m_status));
    m_lastMessageTimer.start();
}

qint64 MavlinkRouter::timeSinceLastMessage() const {
    return m_lastMessageTimer.elapsed();
}

void MavlinkRouter::receiveBytes(const QByteArray& data) {
    mavlink_message_t msg;

    for (int i = 0; i < data.size(); ++i) {
        uint8_t byte = static_cast<uint8_t>(data[i]);

        if (mavlink_parse_char(MAVLINK_COMM_0, byte, &msg, &m_status)) {
            // Successfully parsed a message
            m_lastMessageTimer.restart();
            m_receivedPackets++;

            // Update packet loss statistics
            updatePacketLoss(msg.seq);

            // Parse message
            parseMessage(msg);

            // Emit generic signal
            emit messageReceived(msg);
        }
    }
}

void MavlinkRouter::sendMessage(const mavlink_message_t& msg) {
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    uint16_t len = mavlink_msg_to_send_buffer(buffer, &msg);

    QByteArray data(reinterpret_cast<const char*>(buffer), len);
    emit bytesToSend(data);
}

void MavlinkRouter::parseMessage(const mavlink_message_t& msg) {
    switch (msg.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT:
            handleHeartbeat(msg);
            break;
        case MAVLINK_MSG_ID_TIMESYNC:
            handleTimesync(msg);
            break;
        case MAVLINK_MSG_ID_ATTITUDE:
            handleAttitude(msg);
            break;
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
            handleGlobalPosition(msg);
            break;
        case MAVLINK_MSG_ID_VFR_HUD:
            handleVfrHud(msg);
            break;
        case MAVLINK_MSG_ID_BATTERY_STATUS:
            handleBatteryStatus(msg);
            break;
        case MAVLINK_MSG_ID_GPS_RAW_INT:
            handleGpsRaw(msg);
            break;
        case MAVLINK_MSG_ID_SYS_STATUS:
            handleSystemStatus(msg);
            break;
        default:
            // Other messages are ignored for now
            break;
    }
}

void MavlinkRouter::handleHeartbeat(const mavlink_message_t& msg) {
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&msg, &heartbeat);

    emit heartbeatReceived(msg.sysid, msg.compid, heartbeat.autopilot, heartbeat.type,
                           heartbeat.system_status, heartbeat.base_mode, heartbeat.custom_mode);
}

void MavlinkRouter::handleTimesync(const mavlink_message_t& msg) {
    mavlink_timesync_t timesync;
    mavlink_msg_timesync_decode(&msg, &timesync);

    emit timesyncReceived(timesync.tc1, timesync.ts1);

    // If tc1 is 0, this is a request for us to respond
    if (timesync.tc1 == 0) {
        // Get current time in microseconds
        qint64 now = QDateTime::currentMSecsSinceEpoch() * 1000;

        // Send response
        mavlink_message_t response;
        mavlink_msg_timesync_pack(m_systemId, m_componentId, &response, timesync.ts1, now,
                                  msg.sysid, msg.compid);
        sendMessage(response);

        qDebug() << "MavlinkRouter: Sent TIMESYNC response";
    } else {
        // This is a response to our request - calculate RTT
        qint64 now = QDateTime::currentMSecsSinceEpoch() * 1000;
        m_roundTripTime = (now - timesync.tc1) / 1000;  // Convert to milliseconds
        emit roundTripTimeChanged(m_roundTripTime);
        qDebug() << "MavlinkRouter: RTT =" << m_roundTripTime << "ms";
    }
}

void MavlinkRouter::handleAttitude(const mavlink_message_t& msg) {
    mavlink_attitude_t attitude;
    mavlink_msg_attitude_decode(&msg, &attitude);

    emit attitudeReceived(attitude.roll, attitude.pitch, attitude.yaw, attitude.rollspeed,
                          attitude.pitchspeed, attitude.yawspeed);
}

void MavlinkRouter::handleGlobalPosition(const mavlink_message_t& msg) {
    mavlink_global_position_int_t pos;
    mavlink_msg_global_position_int_decode(&msg, &pos);

    emit globalPositionReceived(pos.lat, pos.lon, pos.alt, pos.relative_alt, pos.vx, pos.vy, pos.vz,
                                pos.hdg);
}

void MavlinkRouter::handleVfrHud(const mavlink_message_t& msg) {
    mavlink_vfr_hud_t hud;
    mavlink_msg_vfr_hud_decode(&msg, &hud);

    emit vfrHudReceived(hud.airspeed, hud.groundspeed, hud.heading, hud.throttle, hud.alt,
                        hud.climb);
}

void MavlinkRouter::handleBatteryStatus(const mavlink_message_t& msg) {
    mavlink_battery_status_t battery;
    mavlink_msg_battery_status_decode(&msg, &battery);

    // Calculate total voltage (in millivolts)
    uint16_t totalVoltage = 0;
    for (int i = 0; i < 10; ++i) {
        if (battery.voltages[i] != UINT16_MAX) {
            totalVoltage += battery.voltages[i];
        }
    }

    emit batteryStatusReceived(totalVoltage, battery.current_battery, battery.battery_remaining);
}

void MavlinkRouter::handleGpsRaw(const mavlink_message_t& msg) {
    mavlink_gps_raw_int_t gps;
    mavlink_msg_gps_raw_int_decode(&msg, &gps);

    emit gpsRawReceived(gps.fix_type, gps.lat, gps.lon, gps.alt, gps.eph, gps.epv, gps.vel, gps.cog,
                        gps.satellites_visible);
}

void MavlinkRouter::handleSystemStatus(const mavlink_message_t& msg) {
    mavlink_sys_status_t status;
    mavlink_msg_sys_status_decode(&msg, &status);

    emit systemStatusReceived(status.voltage_battery, status.current_battery,
                              status.battery_remaining);
}

void MavlinkRouter::updatePacketLoss(uint8_t seq) {
    if (m_receivedPackets > 1) {
        // Calculate expected sequence number
        uint8_t expectedSeq = static_cast<uint8_t>(m_lastSeq + 1);

        // Count dropped packets (handle wraparound)
        if (seq != expectedSeq) {
            uint8_t dropped = 0;
            if (seq > expectedSeq) {
                dropped = seq - expectedSeq;
            } else {
                // Wraparound case
                dropped = (255 - expectedSeq) + seq + 1;
            }

            m_droppedPackets += dropped;
        }
    }

    m_lastSeq = seq;

    // Calculate packet loss percentage
    if (m_receivedPackets > 0) {
        float totalPackets = static_cast<float>(m_receivedPackets + m_droppedPackets);
        m_packetLoss = (static_cast<float>(m_droppedPackets) / totalPackets) * 100.0f;
        emit packetLossChanged(m_packetLoss);
    }
}
