#ifndef MAVLINKROUTER_H
#define MAVLINKROUTER_H

#include <QObject>
#include <QByteArray>
#include <QElapsedTimer>
#include "mavlink/common/mavlink.h"

/**
 * @brief Parses and routes MAVLink messages
 *
 * Handles:
 * - MAVLink message parsing from byte stream
 * - Protocol-specific message handling (HEARTBEAT, TIMESYNC, etc.)
 * - Message statistics (packet loss, message rates)
 * - Emits signals for different message types
 */
class MavlinkRouter : public QObject {
    Q_OBJECT

public:
    explicit MavlinkRouter(QObject* parent = nullptr);
    ~MavlinkRouter() override = default;

    /**
     * @brief Get packet loss percentage
     */
    float packetLoss() const { return m_packetLoss; }

    /**
     * @brief Get round-trip time in milliseconds
     */
    qint64 roundTripTime() const { return m_roundTripTime; }

    /**
     * @brief Get time since last message in milliseconds
     */
    qint64 timeSinceLastMessage() const;

public slots:
    /**
     * @brief Process incoming bytes from the link
     * @param data Raw bytes received
     */
    void receiveBytes(const QByteArray& data);

    /**
     * @brief Send a MAVLink message
     * @param msg The message to send
     */
    void sendMessage(const mavlink_message_t& msg);

signals:
    /**
     * @brief Emitted when bytes need to be sent
     */
    void bytesToSend(QByteArray data);

    /**
     * @brief Emitted when any MAVLink message is received
     */
    void messageReceived(mavlink_message_t msg);

    /**
     * @brief Emitted when a HEARTBEAT message is received
     */
    void heartbeatReceived(uint8_t systemId, uint8_t componentId, uint8_t autopilot,
                           uint8_t type, uint8_t systemStatus, uint8_t baseMode,
                           uint32_t customMode);

    /**
     * @brief Emitted when a TIMESYNC message is received
     */
    void timesyncReceived(int64_t tc1, int64_t ts1);

    /**
     * @brief Emitted when telemetry data is received
     */
    void attitudeReceived(float roll, float pitch, float yaw, float rollspeed, float pitchspeed,
                          float yawspeed);
    void globalPositionReceived(int32_t lat, int32_t lon, int32_t alt, int32_t relativeAlt,
                                int16_t vx, int16_t vy, int16_t vz, uint16_t heading);
    void vfrHudReceived(float airspeed, float groundspeed, int16_t heading, uint16_t throttle,
                        float alt, float climb);
    void batteryStatusReceived(uint16_t voltage, int16_t current, int8_t remaining);
    void gpsRawReceived(uint8_t fixType, int32_t lat, int32_t lon, int32_t alt, uint16_t eph,
                        uint16_t epv, uint16_t vel, uint16_t cog, uint8_t satellitesVisible);
    void systemStatusReceived(uint16_t voltage, int16_t currentBattery, int8_t batteryRemaining);

    /**
     * @brief Emitted when mission protocol messages are received
     */
    void missionCountReceived(uint16_t count, uint8_t missionType);
    void missionRequestReceived(uint16_t seq, uint8_t missionType);
    void missionRequestIntReceived(uint16_t seq, uint8_t missionType);
    void missionItemReceived(const mavlink_mission_item_t& item);
    void missionItemIntReceived(const mavlink_mission_item_int_t& item);
    void missionAckReceived(uint8_t type, uint8_t missionType);
    void missionCurrentReceived(uint16_t seq, uint16_t total);

    /**
     * @brief Emitted when command acknowledgment is received
     */
    void commandAckReceived(uint16_t command, uint8_t result);

    /**
     * @brief Emitted when packet loss changes
     */
    void packetLossChanged(float loss);

    /**
     * @brief Emitted when RTT is measured
     */
    void roundTripTimeChanged(qint64 rtt);

private:
    void parseMessage(const mavlink_message_t& msg);
    void handleHeartbeat(const mavlink_message_t& msg);
    void handleTimesync(const mavlink_message_t& msg);
    void handleAttitude(const mavlink_message_t& msg);
    void handleGlobalPosition(const mavlink_message_t& msg);
    void handleVfrHud(const mavlink_message_t& msg);
    void handleBatteryStatus(const mavlink_message_t& msg);
    void handleGpsRaw(const mavlink_message_t& msg);
    void handleSystemStatus(const mavlink_message_t& msg);
    void handleMissionCount(const mavlink_message_t& msg);
    void handleMissionRequest(const mavlink_message_t& msg);
    void handleMissionRequestInt(const mavlink_message_t& msg);
    void handleMissionItem(const mavlink_message_t& msg);
    void handleMissionItemInt(const mavlink_message_t& msg);
    void handleMissionAck(const mavlink_message_t& msg);
    void handleMissionCurrent(const mavlink_message_t& msg);
    void handleCommandAck(const mavlink_message_t& msg);
    void updatePacketLoss(uint8_t seq);

    mavlink_status_t m_status;
    uint8_t m_systemId;
    uint8_t m_componentId;

    // Statistics
    uint8_t m_lastSeq;
    uint32_t m_receivedPackets;
    uint32_t m_droppedPackets;
    float m_packetLoss;
    qint64 m_roundTripTime;
    QElapsedTimer m_lastMessageTimer;

    // TIMESYNC state
    int64_t m_timesyncTc1;
};

#endif  // MAVLINKROUTER_H
