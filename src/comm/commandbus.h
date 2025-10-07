#ifndef COMMANDBUS_H
#define COMMANDBUS_H

#include <QObject>
#include "mavlinkrouter.h"
#include "models/vehiclemodel.h"

/**
 * @brief Central command bus for sending MAVLink commands to the vehicle
 *
 * This class provides a high-level interface for common vehicle commands:
 * - Arm/Disarm
 * - Takeoff/Land
 * - Return to Launch (RTL)
 * - Set flight mode
 * - Change speed
 *
 * All commands are sent via COMMAND_LONG with proper acknowledgment tracking.
 */
class CommandBus : public QObject {
    Q_OBJECT

public:
    explicit CommandBus(MavlinkRouter* mavlinkRouter, VehicleModel* vehicleModel,
                        QObject* parent = nullptr);
    ~CommandBus() override = default;

    /**
     * @brief Get the last command result
     */
    uint8_t lastCommandResult() const { return m_lastCommandResult; }

public slots:
    /**
     * @brief Arm the vehicle
     * @param force Force arming even if pre-arm checks fail
     */
    void arm(bool force = false);

    /**
     * @brief Disarm the vehicle
     * @param force Force disarming
     */
    void disarm(bool force = false);

    /**
     * @brief Command takeoff
     * @param altitude Target altitude in meters (relative to home)
     */
    void takeoff(float altitude);

    /**
     * @brief Command landing
     */
    void land();

    /**
     * @brief Return to launch position
     */
    void returnToLaunch();

    /**
     * @brief Set flight mode
     * @param mode Custom mode number (ArduPilot specific)
     */
    void setMode(uint32_t customMode);

    /**
     * @brief Set target airspeed
     * @param speed Speed in m/s
     */
    void setSpeed(float speed);

    /**
     * @brief Start the mission (AUTO mode)
     */
    void startMission();

    /**
     * @brief Pause the mission
     */
    void pauseMission();

    /**
     * @brief Switch to GUIDED mode
     */
    void switchToGuided();

    /**
     * @brief Handle COMMAND_ACK from vehicle
     */
    void handleCommandAck(uint16_t command, uint8_t result);

signals:
    /**
     * @brief Emitted when a command is acknowledged
     * @param command The command that was acknowledged
     * @param result MAV_RESULT enum value
     */
    void commandAcknowledged(uint16_t command, uint8_t result);

    /**
     * @brief Emitted when arm command completes
     */
    void armComplete(bool success);

    /**
     * @brief Emitted when disarm command completes
     */
    void disarmComplete(bool success);

    /**
     * @brief Emitted when takeoff command is sent
     */
    void takeoffCommandSent();

    /**
     * @brief Emitted when land command is sent
     */
    void landCommandSent();

    /**
     * @brief Emitted when RTL command is sent
     */
    void rtlCommandSent();

    /**
     * @brief Emitted when mode change is requested
     */
    void modeChangeRequested(uint32_t mode);

private:
    void sendCommand(uint16_t command, float param1 = 0, float param2 = 0, float param3 = 0,
                     float param4 = 0, float param5 = 0, float param6 = 0, float param7 = 0);

    MavlinkRouter* m_mavlinkRouter;
    VehicleModel* m_vehicleModel;
    uint16_t m_lastCommand;
    uint8_t m_lastCommandResult;
};

#endif  // COMMANDBUS_H
