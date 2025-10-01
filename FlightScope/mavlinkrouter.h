#ifndef MAVLINKROUTER_H
#define MAVLINKROUTER_H

#include <QObject>

// CORRECT FIX: Include the header that defines mavlink_message_t,
// instead of forward-declaring it. This resolves the conflict.
#include <mavlink/mavlink_types.h>

class MavlinkRouter : public QObject
{
    Q_OBJECT
  public:
    explicit MavlinkRouter(QObject *parent = nullptr);

           // This function now uses a fully-defined type.
    void parseMessage(const mavlink_message_t& message);

  signals:
    // HEARTBEAT signals
    void vehicleArmed(bool armed);
    void systemStatusChanged(int status);

           // TIMESYNC signal
    void rttCalculated(qint64 rtt_ms);

};

#endif // MAVLINKROUTER_H
