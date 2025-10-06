#ifndef WAYPOINT_H
#define WAYPOINT_H

#include <cstdint>
#include "mavlink/common/mavlink.h"

/**
 * @brief Represents a single waypoint in a mission
 *
 * This class encapsulates all MAVLink mission item data.
 * It uses MAVLink enums and types for direct protocol compatibility.
 */
class Waypoint {
public:
    /**
     * @brief Default constructor - creates a basic waypoint
     */
    Waypoint();

    /**
     * @brief Construct a waypoint from MAVLink MISSION_ITEM_INT
     */
    Waypoint(const mavlink_mission_item_int_t& item);

    /**
     * @brief Convert to MAVLink MISSION_ITEM_INT
     */
    mavlink_mission_item_int_t toMavlinkMissionItemInt() const;

    // Getters
    uint16_t sequence() const { return m_sequence; }
    uint8_t frame() const { return m_frame; }
    uint16_t command() const { return m_command; }
    uint8_t current() const { return m_current; }
    uint8_t autocontinue() const { return m_autocontinue; }
    float param1() const { return m_param1; }
    float param2() const { return m_param2; }
    float param3() const { return m_param3; }
    float param4() const { return m_param4; }
    int32_t x() const { return m_x; }  // Latitude (degrees * 1E7)
    int32_t y() const { return m_y; }  // Longitude (degrees * 1E7)
    float z() const { return m_z; }    // Altitude (meters)

    // Convenience getters with proper units
    double latitude() const { return m_x / 1e7; }
    double longitude() const { return m_y / 1e7; }
    float altitude() const { return m_z; }

    // Setters
    void setSequence(uint16_t seq) { m_sequence = seq; }
    void setFrame(uint8_t frame) { m_frame = frame; }
    void setCommand(uint16_t cmd) { m_command = cmd; }
    void setCurrent(uint8_t current) { m_current = current; }
    void setAutocontinue(uint8_t autocontinue) { m_autocontinue = autocontinue; }
    void setParam1(float p1) { m_param1 = p1; }
    void setParam2(float p2) { m_param2 = p2; }
    void setParam3(float p3) { m_param3 = p3; }
    void setParam4(float p4) { m_param4 = p4; }
    void setX(int32_t x) { m_x = x; }
    void setY(int32_t y) { m_y = y; }
    void setZ(float z) { m_z = z; }

    // Convenience setters with proper units
    void setLatitude(double lat) { m_x = static_cast<int32_t>(lat * 1e7); }
    void setLongitude(double lon) { m_y = static_cast<int32_t>(lon * 1e7); }
    void setAltitude(float alt) { m_z = alt; }

    /**
     * @brief Get human-readable command name
     */
    static const char* commandName(uint16_t command);

    /**
     * @brief Get human-readable frame name
     */
    static const char* frameName(uint8_t frame);

private:
    uint16_t m_sequence;      // Waypoint ID (sequence number)
    uint8_t m_frame;          // MAV_FRAME enum
    uint16_t m_command;       // MAV_CMD enum
    uint8_t m_current;        // 1 if current waypoint, 0 otherwise
    uint8_t m_autocontinue;   // 1 to continue to next waypoint automatically
    float m_param1;           // Command-specific parameter 1
    float m_param2;           // Command-specific parameter 2
    float m_param3;           // Command-specific parameter 3
    float m_param4;           // Command-specific parameter 4 (yaw angle)
    int32_t m_x;              // Latitude * 1E7 (or local X in meters * 1E4)
    int32_t m_y;              // Longitude * 1E7 (or local Y in meters * 1E4)
    float m_z;                // Altitude in meters
};

#endif  // WAYPOINT_H
