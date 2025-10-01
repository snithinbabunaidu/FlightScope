#include <mavlink/common/mavlink.h> // MAVLink first!
#include "mavlinkrouter.h"
#include <QDateTime>
#include <QDebug>

MavlinkRouter::MavlinkRouter(QObject *parent) : QObject(parent)
{

}

void MavlinkRouter::parseMessage(const mavlink_message_t& message)
{
    // This is where we will handle specific messages
    switch (message.msgid) {
    case MAVLINK_MSG_ID_HEARTBEAT:
    {
        // Decode the HEARTBEAT message
        mavlink_heartbeat_t heartbeat;
        mavlink_msg_heartbeat_decode(&message, &heartbeat);

               // Check the base_mode flag to see if the vehicle is armed
        bool armed = heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED;
        emit vehicleArmed(armed);

               // Emit the system status
        emit systemStatusChanged(heartbeat.system_status);
        break;
    }

    case MAVLINK_MSG_ID_TIMESYNC:
    {
        mavlink_timesync_t tsync;
        mavlink_msg_timesync_decode(&message, &tsync);

        if (tsync.tc1 == 0) { // This is a request from the vehicle.
            mavlink_message_t response_msg;
            uint64_t now_ns = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch()) * 1000000;

                   // We are the GCS, so our system ID is typically 255. Component ID can be 1.
            uint8_t our_system_id = 255;
            uint8_t our_component_id = 1;

                   // When we reply, the vehicle that sent the request becomes our target.
            uint8_t target_system = message.sysid;
            uint8_t target_component = message.compid;

                   // Pack the response message.
            mavlink_msg_timesync_pack(
                our_system_id,      // The ID of our system (the GCS)
                our_component_id,   // The ID of our component
                &response_msg,      // The message to pack into
                now_ns,             // Our current timestamp (tc1)
                tsync.ts1,          // The vehicle's timestamp that we are echoing back (ts1)
                target_system,      // The vehicle's system ID is now the target
                target_component    // The vehicle's component ID is now the target
                );
            // TODO: emit a signal to send this response_msg back to the vehicle
        } else {
            // This is a response from the vehicle, calculate RTT
            uint64_t now_ns = static_cast<uint64_t>(QDateTime::currentMSecsSinceEpoch()) * 1000000;
            qint64 rtt = now_ns - tsync.tc1;
            emit rttCalculated(rtt / 1000000); // convert to ms
        }
        break;
    }

    default:
        // Unhandled message
        break;
    }
}
