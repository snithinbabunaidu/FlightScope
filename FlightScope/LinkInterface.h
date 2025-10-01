#ifndef LINKINTERFACE_H
#define LINKINTERFACE_H

#include <QObject>
#include <QByteArray>

// This is an abstract base class for all communication links.
// It is designed to be moved to a separate thread to avoid blocking the GUI.
class LinkInterface : public QObject
{
    Q_OBJECT
  public:
    explicit LinkInterface(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~LinkInterface() = default;

  signals:
    // This signal is emitted whenever raw data is received from the vehicle.
    void bytesReceived(const QByteArray& data);

  public slots:
    // This slot is called to send raw data to the vehicle.
    virtual void writeBytes(const QByteArray& data) = 0;
};

#endif
