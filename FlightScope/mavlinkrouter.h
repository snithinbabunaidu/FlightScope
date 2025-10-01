#ifndef MAVLINKROUTER_H
#define MAVLINKROUTER_H

#include <QObject>
#include <QByteArray>

// Forward declaration of the MAVLink message struct
struct mavlink_message_t;

class MavlinkRouter : public QObject
{
    Q_OBJECT
  public:
    explicit MavlinkRouter(QObject *parent = nullptr);

  public slots:
    // This slot will connect to a LinkInterface's bytesReceived signal
    void receiveBytes(const QByteArray& data);

  signals:
    // Signals to be emitted after parsing a HEARTBEAT message
    void vehicleArmed(bool armed);
    void flightModeChanged(const QString& flightMode);
    void systemStatusChanged(int status);

  private:
    void parseMessage(const mavlink_message_t& message);
};

#endif // MAVLINKROUTER_H
