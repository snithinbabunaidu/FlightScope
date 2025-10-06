#ifndef LINKINTERFACE_H
#define LINKINTERFACE_H

#include <QObject>
#include <QByteArray>
#include <QString>

/**
 * @brief Abstract base class for all communication links
 *
 * This class defines the interface for communication links (UDP, Serial, TCP).
 * All implementations run on a separate QThread to prevent blocking the GUI.
 * Uses the worker-object approach for thread safety.
 */
class LinkInterface : public QObject {
    Q_OBJECT

public:
    enum class LinkStatus { Disconnected, Connecting, Connected, Error };
    Q_ENUM(LinkStatus)

    explicit LinkInterface(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~LinkInterface() = default;

    /**
     * @brief Get the human-readable name of this link
     */
    virtual QString name() const = 0;

    /**
     * @brief Get the current status of the link
     */
    virtual LinkStatus status() const = 0;

    /**
     * @brief Check if the link is currently connected
     */
    virtual bool isConnected() const = 0;

public slots:
    /**
     * @brief Connect the link (non-blocking)
     */
    virtual void connectLink() = 0;

    /**
     * @brief Disconnect the link
     */
    virtual void disconnectLink() = 0;

    /**
     * @brief Write bytes to the link
     * @param data The data to write
     */
    virtual void writeBytes(const QByteArray& data) = 0;

signals:
    /**
     * @brief Emitted when bytes are received from the link
     * @param data The received data
     */
    void bytesReceived(QByteArray data);

    /**
     * @brief Emitted when the link status changes
     * @param status The new status
     */
    void statusChanged(LinkStatus status);

    /**
     * @brief Emitted when an error occurs
     * @param errorString Description of the error
     */
    void errorOccurred(QString errorString);

    /**
     * @brief Emitted when bytes are written
     * @param byteCount Number of bytes written
     */
    void bytesWritten(qint64 byteCount);
};

#endif  // LINKINTERFACE_H
