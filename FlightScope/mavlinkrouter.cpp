#include "mavlinkrouter.h"

// Include the main MAVLink header
#include "third_party/mavlink/common/mavlink.h"

MavlinkRouter::MavlinkRouter(QObject *parent)
    : QObject{parent}
{
}

void MavlinkRouter::receiveBytes(const QByteArray& data)
{
    mavlink_message_t msg;
    mavlink_status_t status;

    for (int i = 0; i < data.size(); ++i) {
        if (mavlink_parse_char(MAVLINK_COMM_0, data.at(i), &msg, &status)) {
            // A complete MAVLink message has been received.
            parseMessage(msg);
        }
    }
}

void MavlinkRouter::parseMessage(const mavlink_message_t& message)
{
    // This is where we will handle specific messages [cite: 27]
    switch (message.msgid) {
    case MAVLINK_MSG_ID_HEARTBEAT:
        // TODO: Handle HEARTBEAT message
        break;

               // Per the blueprint, we also need to handle TIMESYNC [cite: 29]
    case MAVLINK_MSG_ID_TIMESYNC:
        // TODO: Handle TIMESYNC message [cite: 29]
        break;

    default:
        // Unhandled message
        break;
    }
}
