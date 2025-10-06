#include "waypoint.h"

Waypoint::Waypoint()
    : m_sequence(0),
      m_frame(MAV_FRAME_GLOBAL_RELATIVE_ALT_INT),
      m_command(MAV_CMD_NAV_WAYPOINT),
      m_current(0),
      m_autocontinue(1),
      m_param1(0.0f),
      m_param2(0.0f),
      m_param3(0.0f),
      m_param4(0.0f),
      m_x(0),
      m_y(0),
      m_z(0.0f) {
}

Waypoint::Waypoint(const mavlink_mission_item_int_t& item)
    : m_sequence(item.seq),
      m_frame(item.frame),
      m_command(item.command),
      m_current(item.current),
      m_autocontinue(item.autocontinue),
      m_param1(item.param1),
      m_param2(item.param2),
      m_param3(item.param3),
      m_param4(item.param4),
      m_x(item.x),
      m_y(item.y),
      m_z(item.z) {
}

mavlink_mission_item_int_t Waypoint::toMavlinkMissionItemInt() const {
    mavlink_mission_item_int_t item{};
    item.seq = m_sequence;
    item.frame = m_frame;
    item.command = m_command;
    item.current = m_current;
    item.autocontinue = m_autocontinue;
    item.param1 = m_param1;
    item.param2 = m_param2;
    item.param3 = m_param3;
    item.param4 = m_param4;
    item.x = m_x;
    item.y = m_y;
    item.z = m_z;
    item.target_system = 0;    // Will be set by MissionManager
    item.target_component = 0; // Will be set by MissionManager
    item.mission_type = MAV_MISSION_TYPE_MISSION;
    return item;
}

const char* Waypoint::commandName(uint16_t command) {
    switch (command) {
        case MAV_CMD_NAV_WAYPOINT:
            return "Waypoint";
        case MAV_CMD_NAV_LOITER_UNLIM:
            return "Loiter Unlimited";
        case MAV_CMD_NAV_LOITER_TURNS:
            return "Loiter Turns";
        case MAV_CMD_NAV_LOITER_TIME:
            return "Loiter Time";
        case MAV_CMD_NAV_RETURN_TO_LAUNCH:
            return "Return to Launch";
        case MAV_CMD_NAV_LAND:
            return "Land";
        case MAV_CMD_NAV_TAKEOFF:
            return "Takeoff";
        case MAV_CMD_NAV_LAND_LOCAL:
            return "Land Local";
        case MAV_CMD_NAV_TAKEOFF_LOCAL:
            return "Takeoff Local";
        case MAV_CMD_DO_SET_ROI:
            return "Set ROI";
        case MAV_CMD_DO_CHANGE_SPEED:
            return "Change Speed";
        case MAV_CMD_DO_SET_HOME:
            return "Set Home";
        case MAV_CMD_DO_JUMP:
            return "Jump";
        case MAV_CMD_DO_DIGICAM_CONTROL:
            return "Camera Control";
        case MAV_CMD_DO_MOUNT_CONTROL:
            return "Mount Control";
        case MAV_CMD_DO_SET_CAM_TRIGG_DIST:
            return "Camera Trigger Distance";
        case MAV_CMD_DO_FENCE_ENABLE:
            return "Enable Fence";
        case MAV_CMD_DO_PARACHUTE:
            return "Parachute";
        case MAV_CMD_DO_INVERTED_FLIGHT:
            return "Inverted Flight";
        case MAV_CMD_DO_GRIPPER:
            return "Gripper";
        case MAV_CMD_DO_VTOL_TRANSITION:
            return "VTOL Transition";
        default:
            return "Unknown";
    }
}

const char* Waypoint::frameName(uint8_t frame) {
    switch (frame) {
        case MAV_FRAME_GLOBAL:
            return "Global (MSL)";
        case MAV_FRAME_LOCAL_NED:
            return "Local NED";
        case MAV_FRAME_MISSION:
            return "Mission";
        case MAV_FRAME_GLOBAL_RELATIVE_ALT:
            return "Global Relative Alt";
        case MAV_FRAME_LOCAL_ENU:
            return "Local ENU";
        case MAV_FRAME_GLOBAL_INT:
            return "Global Int";
        case MAV_FRAME_GLOBAL_RELATIVE_ALT_INT:
            return "Global Relative Alt Int";
        case MAV_FRAME_LOCAL_OFFSET_NED:
            return "Local Offset NED";
        case MAV_FRAME_BODY_NED:
            return "Body NED";
        case MAV_FRAME_BODY_OFFSET_NED:
            return "Body Offset NED";
        case MAV_FRAME_GLOBAL_TERRAIN_ALT:
            return "Global Terrain Alt";
        case MAV_FRAME_GLOBAL_TERRAIN_ALT_INT:
            return "Global Terrain Alt Int";
        default:
            return "Unknown";
    }
}
