#include "linkmanager.h"
#include <QDebug>

LinkManager::LinkManager(QObject* parent)
    : QObject(parent), m_activeLink(nullptr), m_linkThread(nullptr), m_heartbeatTimer(nullptr),
      m_reconnectTimer(nullptr), m_reconnectAttempt(0), m_reconnectDelay(INITIAL_RECONNECT_DELAY_MS) {
    // Create timers (they run on this object's thread, i.e., main thread)
    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setSingleShot(false);
    m_heartbeatTimer->setInterval(HEARTBEAT_TIMEOUT_MS);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &LinkManager::onHeartbeatTimeout);

    m_reconnectTimer = new QTimer(this);
    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &LinkManager::onReconnectTimeout);
}

LinkManager::~LinkManager() {
    closeActiveLink();
}

bool LinkManager::isConnected() const {
    return m_activeLink && m_activeLink->isConnected();
}

void LinkManager::setActiveLink(LinkInterface* link) {
    if (!link) {
        qWarning() << "LinkManager::setActiveLink() - null link provided";
        return;
    }

    // Close existing link
    closeActiveLink();

    m_activeLink = link;

    // Create worker thread
    m_linkThread = new QThread(this);
    m_activeLink->setParent(nullptr);  // Remove parent before moving to thread
    m_activeLink->moveToThread(m_linkThread);

    // Connect signals
    connect(m_activeLink, &LinkInterface::bytesReceived, this, &LinkManager::bytesReceived);
    connect(m_activeLink, &LinkInterface::statusChanged, this, &LinkManager::onLinkStatusChanged);
    connect(m_activeLink, &LinkInterface::errorOccurred, this, &LinkManager::onLinkError);

    // Connect link when thread starts
    connect(m_linkThread, &QThread::started, m_activeLink, &LinkInterface::connectLink);

    // Clean up when thread finishes
    connect(m_linkThread, &QThread::finished, m_activeLink, &QObject::deleteLater);

    // Start the thread
    m_linkThread->start();

    qDebug() << "LinkManager: Activated link" << link->name();
}

void LinkManager::closeActiveLink() {
    stopHeartbeatMonitor();
    stopReconnectTimer();
    resetReconnectBackoff();

    if (m_activeLink) {
        // Disconnect the link (thread-safe via Qt's signal/slot mechanism)
        QMetaObject::invokeMethod(m_activeLink, "disconnectLink", Qt::QueuedConnection);
    }

    if (m_linkThread) {
        m_linkThread->quit();
        if (!m_linkThread->wait(3000)) {
            qWarning() << "LinkManager: Link thread did not finish in time, terminating";
            m_linkThread->terminate();
            m_linkThread->wait();
        }
        m_linkThread->deleteLater();
        m_linkThread = nullptr;
    }

    m_activeLink = nullptr;  // Will be deleted by thread finished signal
    emit connectionStatusChanged(false);
}

void LinkManager::reconnect() {
    if (!m_activeLink) {
        qWarning() << "LinkManager::reconnect() - No active link to reconnect";
        return;
    }

    qDebug() << "LinkManager: Manual reconnect triggered";
    m_reconnectAttempt++;

    // Disconnect and reconnect
    QMetaObject::invokeMethod(m_activeLink, "disconnectLink", Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_activeLink, "connectLink", Qt::QueuedConnection);
}

void LinkManager::onLinkStatusChanged(LinkInterface::LinkStatus status) {
    switch (status) {
        case LinkInterface::LinkStatus::Connected:
            qDebug() << "LinkManager: Link connected";
            resetReconnectBackoff();
            stopReconnectTimer();
            startHeartbeatMonitor();
            emit connectionStatusChanged(true);
            break;

        case LinkInterface::LinkStatus::Disconnected:
            qDebug() << "LinkManager: Link disconnected";
            stopHeartbeatMonitor();
            emit connectionStatusChanged(false);
            // Don't auto-reconnect on manual disconnect
            break;

        case LinkInterface::LinkStatus::Error:
            qWarning() << "LinkManager: Link error occurred";
            stopHeartbeatMonitor();
            emit connectionStatusChanged(false);
            startReconnectTimer();
            break;

        case LinkInterface::LinkStatus::Connecting:
            qDebug() << "LinkManager: Link connecting...";
            break;
    }
}

void LinkManager::onLinkError(QString errorString) {
    qWarning() << "LinkManager: Link error:" << errorString;
    emit linkError(errorString);
}

void LinkManager::onHeartbeatTimeout() {
    qWarning() << "LinkManager: Heartbeat timeout - no messages received for" << HEARTBEAT_TIMEOUT_MS
               << "ms";
    stopHeartbeatMonitor();
    emit connectionStatusChanged(false);
    startReconnectTimer();
}

void LinkManager::onReconnectTimeout() {
    m_reconnectAttempt++;
    qDebug() << "LinkManager: Reconnection attempt" << m_reconnectAttempt << "after"
             << m_reconnectDelay << "ms";

    emit reconnecting(m_reconnectAttempt, m_reconnectDelay);

    // Attempt reconnection
    if (m_activeLink) {
        QMetaObject::invokeMethod(m_activeLink, "connectLink", Qt::QueuedConnection);
    }

    // Increase delay with exponential backoff
    m_reconnectDelay = qMin(m_reconnectDelay * 2, MAX_RECONNECT_DELAY_MS);

    // Schedule next attempt
    startReconnectTimer();
}

void LinkManager::startHeartbeatMonitor() {
    if (m_heartbeatTimer && !m_heartbeatTimer->isActive()) {
        m_heartbeatTimer->start();
        qDebug() << "LinkManager: Heartbeat monitor started";
    }
}

void LinkManager::stopHeartbeatMonitor() {
    if (m_heartbeatTimer && m_heartbeatTimer->isActive()) {
        m_heartbeatTimer->stop();
        qDebug() << "LinkManager: Heartbeat monitor stopped";
    }
}

void LinkManager::resetHeartbeatTimeout() {
    // Restart the timer to reset the timeout countdown
    if (m_heartbeatTimer && m_heartbeatTimer->isActive()) {
        m_heartbeatTimer->start();
    }
}

void LinkManager::startReconnectTimer() {
    if (m_reconnectTimer && !m_reconnectTimer->isActive()) {
        m_reconnectTimer->setInterval(m_reconnectDelay);
        m_reconnectTimer->start();
        qDebug() << "LinkManager: Reconnect timer started with delay" << m_reconnectDelay << "ms";
    }
}

void LinkManager::stopReconnectTimer() {
    if (m_reconnectTimer && m_reconnectTimer->isActive()) {
        m_reconnectTimer->stop();
        qDebug() << "LinkManager: Reconnect timer stopped";
    }
}

void LinkManager::resetReconnectBackoff() {
    m_reconnectAttempt = 0;
    m_reconnectDelay = INITIAL_RECONNECT_DELAY_MS;
    qDebug() << "LinkManager: Reconnect backoff reset";
}
