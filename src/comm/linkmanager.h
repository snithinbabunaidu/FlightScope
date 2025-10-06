#ifndef LINKMANAGER_H
#define LINKMANAGER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "linkinterface.h"

/**
 * @brief Manages communication links and their lifecycle
 *
 * Handles:
 * - Link creation and destruction
 * - Moving links to worker threads
 * - Reconnection logic with exponential backoff
 * - Heartbeat monitoring
 */
class LinkManager : public QObject {
    Q_OBJECT

public:
    explicit LinkManager(QObject* parent = nullptr);
    ~LinkManager() override;

    /**
     * @brief Get the currently active link
     */
    LinkInterface* activeLink() const { return m_activeLink; }

    /**
     * @brief Check if we have an active connection
     */
    bool isConnected() const;

public slots:
    /**
     * @brief Set and activate a link
     * @param link The link to activate (takes ownership)
     */
    void setActiveLink(LinkInterface* link);

    /**
     * @brief Close the active link
     */
    void closeActiveLink();

    /**
     * @brief Manually trigger reconnection
     */
    void reconnect();

signals:
    /**
     * @brief Emitted when bytes are received from the active link
     */
    void bytesReceived(QByteArray data);

    /**
     * @brief Emitted when connection status changes
     */
    void connectionStatusChanged(bool connected);

    /**
     * @brief Emitted when a link error occurs
     */
    void linkError(QString errorString);

    /**
     * @brief Emitted when attempting to reconnect
     */
    void reconnecting(int attemptNumber, int delayMs);

private slots:
    void onLinkStatusChanged(LinkInterface::LinkStatus status);
    void onLinkError(QString errorString);
    void onHeartbeatTimeout();
    void onReconnectTimeout();

private:
    void startHeartbeatMonitor();
    void stopHeartbeatMonitor();
    void startReconnectTimer();
    void stopReconnectTimer();
    void resetReconnectBackoff();

    LinkInterface* m_activeLink;
    QThread* m_linkThread;
    QTimer* m_heartbeatTimer;
    QTimer* m_reconnectTimer;

    // Reconnection state
    int m_reconnectAttempt;
    int m_reconnectDelay;  // in milliseconds
    static constexpr int INITIAL_RECONNECT_DELAY_MS = 1000;      // 1 second
    static constexpr int MAX_RECONNECT_DELAY_MS = 30000;         // 30 seconds
    static constexpr int HEARTBEAT_TIMEOUT_MS = 5000;            // 5 seconds
};

#endif  // LINKMANAGER_H
