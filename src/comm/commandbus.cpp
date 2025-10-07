#include "commandbus.h"
#include <QDebug>

CommandBus::CommandBus(MavlinkRouter* mavlinkRouter, VehicleModel* vehicleModel,
                       QObject* parent)
    : QObject(parent),
      m_mavlinkRouter(mavlinkRouter),
      m_vehicleModel(vehicleModel),
      m_lastCommand(0),
      m_lastCommandResult(MAV_RESULT_FAILED) {
}

void CommandBus::arm(bool force) {
    qInfo() << "CommandBus: Arming vehicle" << (force ? "(forced)" : "");
    sendCommand(MAV_CMD_COMPONENT_ARM_DISARM, 1.0f, force ? 21196.0f : 0.0f);
}

void CommandBus::disarm(bool force) {
    qInfo() << "CommandBus: Disarming vehicle" << (force ? "(forced)" : "");
    sendCommand(MAV_CMD_COMPONENT_ARM_DISARM, 0.0f, force ? 21196.0f : 0.0f);
}

void CommandBus::takeoff(float altitude) {
    qInfo() << "CommandBus: Switching to GUIDED mode and taking off to" << altitude << "meters";

    // First, switch to GUIDED mode (ArduCopter GUIDED = 4)
    // This is required for autonomous commands like takeoff
    setMode(4);

    // Small delay to allow mode change, then send takeoff
    // Note: In production, we should wait for mode change confirmation
    // For now, send both commands in sequence

    // MAV_CMD_NAV_TAKEOFF
    // param1: pitch (unused for multicopter)
    // param2: empty
    // param3: empty
    // param4: yaw angle (NaN = current yaw)
    // param5: latitude (0 = current position)
    // param6: longitude (0 = current position)
    // param7: altitude
    sendCommand(MAV_CMD_NAV_TAKEOFF, 0, 0, 0, NAN, 0, 0, altitude);
    emit takeoffCommandSent();
}

void CommandBus::land() {
    qInfo() << "CommandBus: Landing";

    // MAV_CMD_NAV_LAND
    // param1: abort altitude (0 = use default)
    // param2: land mode
    // param3: empty
    // param4: yaw angle (NaN = current yaw)
    // param5: latitude (0 = current position)
    // param6: longitude (0 = current position)
    // param7: altitude (0 = ground level)
    sendCommand(MAV_CMD_NAV_LAND, 0, 0, 0, NAN, 0, 0, 0);
    emit landCommandSent();
}

void CommandBus::returnToLaunch() {
    qInfo() << "CommandBus: Return to Launch";
    sendCommand(MAV_CMD_NAV_RETURN_TO_LAUNCH);
    emit rtlCommandSent();
}

void CommandBus::setMode(uint32_t customMode) {
    qInfo() << "CommandBus: Setting mode to custom mode" << customMode;

    // ArduPilot requires base_mode to have multiple flags set
    // Not just CUSTOM_MODE_ENABLED, but also the current state flags
    mavlink_message_t msg;

    uint8_t targetSystem = m_vehicleModel->systemId();

    // Base mode must include armed state + custom mode flag
    uint8_t baseMode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;

    // If armed, add safety and guided flags
    if (m_vehicleModel->armed()) {
        baseMode |= MAV_MODE_FLAG_SAFETY_ARMED;
    }

    mavlink_msg_set_mode_pack(255, 190, &msg,
                              targetSystem,
                              baseMode,
                              customMode);

    m_mavlinkRouter->sendMessage(msg);

    qInfo() << "CommandBus: Sent SET_MODE - target:" << targetSystem
            << "base_mode:" << (int)baseMode << "custom_mode:" << customMode;

    emit modeChangeRequested(customMode);
}

void CommandBus::setSpeed(float speed) {
    qInfo() << "CommandBus: Set speed to" << speed << "m/s";

    // MAV_CMD_DO_CHANGE_SPEED
    // param1: speed type (0 = airspeed, 1 = ground speed)
    // param2: speed (m/s)
    // param3: throttle (-1 = no change)
    // param4: absolute or relative (0 = absolute, 1 = relative)
    sendCommand(MAV_CMD_DO_CHANGE_SPEED, 1, speed, -1, 0);
}

void CommandBus::startMission() {
    qInfo() << "CommandBus: Starting mission (switching to AUTO mode)";

    // Switch to AUTO mode (ArduCopter AUTO = 3)
    // This automatically starts mission execution
    setMode(3);
}

void CommandBus::pauseMission() {
    qInfo() << "CommandBus: Pausing mission (switching to LOITER)";
    // Switch to LOITER mode to pause
    setMode(5);  // ArduCopter LOITER = 5
}

void CommandBus::switchToGuided() {
    qInfo() << "CommandBus: Switching to GUIDED mode";
    setMode(4);  // ArduCopter GUIDED = 4
}

void CommandBus::handleCommandAck(uint16_t command, uint8_t result) {
    m_lastCommand = command;
    m_lastCommandResult = result;

    const char* resultStr = "UNKNOWN";
    switch (result) {
        case MAV_RESULT_ACCEPTED:
            resultStr = "ACCEPTED";
            break;
        case MAV_RESULT_TEMPORARILY_REJECTED:
            resultStr = "TEMPORARILY_REJECTED";
            break;
        case MAV_RESULT_DENIED:
            resultStr = "DENIED";
            break;
        case MAV_RESULT_UNSUPPORTED:
            resultStr = "UNSUPPORTED";
            break;
        case MAV_RESULT_FAILED:
            resultStr = "FAILED";
            break;
        case MAV_RESULT_IN_PROGRESS:
            resultStr = "IN_PROGRESS";
            break;
    }

    qInfo() << "CommandBus: Command ACK - command:" << command << "result:" << result << "("
            << resultStr << ")";

    emit commandAcknowledged(command, result);

    // Emit specific signals
    if (command == MAV_CMD_COMPONENT_ARM_DISARM) {
        bool success = (result == MAV_RESULT_ACCEPTED);
        if (m_vehicleModel->armed() || result == MAV_RESULT_ACCEPTED) {
            emit armComplete(success);
        } else {
            emit disarmComplete(success);
        }
    }
}

void CommandBus::sendCommand(uint16_t command, float param1, float param2, float param3,
                              float param4, float param5, float param6, float param7) {
    mavlink_message_t msg;
    mavlink_command_long_t cmd{};

    cmd.target_system = m_vehicleModel->systemId();
    cmd.target_component = m_vehicleModel->componentId();
    cmd.command = command;
    cmd.confirmation = 0;
    cmd.param1 = param1;
    cmd.param2 = param2;
    cmd.param3 = param3;
    cmd.param4 = param4;
    cmd.param5 = param5;
    cmd.param6 = param6;
    cmd.param7 = param7;

    mavlink_msg_command_long_encode(255, 190, &msg, &cmd);
    m_mavlinkRouter->sendMessage(msg);

    qDebug() << "CommandBus: Sent command" << command << "to system" << cmd.target_system;
}
