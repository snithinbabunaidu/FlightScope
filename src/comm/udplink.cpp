#include "udplink.h"
#include <QDebug>
#include <QThread>

UdpLink::UdpLink(const Configuration& config, QObject* parent)
    : LinkInterface(parent), m_config(config), m_socket(nullptr),
      m_status(LinkStatus::Disconnected) {}

UdpLink::~UdpLink() {
    disconnectLink();
}

QString UdpLink::name() const {
    return m_config.name;
}

UdpLink::LinkStatus UdpLink::status() const {
    return m_status;
}

bool UdpLink::isConnected() const {
    return m_status == LinkStatus::Connected;
}

void UdpLink::connectLink() {
    qDebug() << "UdpLink::connectLink() called on thread:" << QThread::currentThread();

    if (m_status == LinkStatus::Connected || m_status == LinkStatus::Connecting) {
        qWarning() << "UdpLink::connectLink() - Already connected or connecting";
        return;
    }

    m_status = LinkStatus::Connecting;
    emit statusChanged(m_status);

    // Create socket on the current thread (should be worker thread)
    m_socket = new QUdpSocket(this);
    qDebug() << "Socket created, thread:" << m_socket->thread();

    // Connect signals
    connect(m_socket, &QUdpSocket::readyRead, this, &UdpLink::onReadyRead);
    connect(m_socket, &QUdpSocket::errorOccurred, this, &UdpLink::onErrorOccurred);

    // Bind to local port
    // Note: ReuseAddressHint allows multiple applications to bind to the same port
    bool bindSuccess = m_socket->bind(m_config.localAddress, m_config.localPort,
                                       QUdpSocket::ReuseAddressHint);

    if (bindSuccess) {
        m_status = LinkStatus::Connected;
        emit statusChanged(m_status);
        qDebug() << "UDP Link connected:" << m_config.name << "listening on"
                 << m_config.localAddress.toString() << ":" << m_config.localPort;
        qDebug() << "Socket state:" << m_socket->state()
                 << "Local address:" << m_socket->localAddress()
                 << "Local port:" << m_socket->localPort();
    } else {
        m_status = LinkStatus::Error;
        QString error = QString("Failed to bind UDP socket: %1").arg(m_socket->errorString());
        emit statusChanged(m_status);
        emit errorOccurred(error);
        qCritical() << error;
    }
}

void UdpLink::disconnectLink() {
    if (m_socket) {
        m_socket->close();
        m_socket->deleteLater();
        m_socket = nullptr;
    }

    m_status = LinkStatus::Disconnected;
    emit statusChanged(m_status);
    qDebug() << "UDP Link disconnected:" << m_config.name;
}

void UdpLink::writeBytes(const QByteArray& data) {
    if (!isConnected() || !m_socket) {
        qWarning() << "UdpLink::writeBytes() - Not connected";
        return;
    }

    qint64 written =
        m_socket->writeDatagram(data, m_config.remoteAddress, m_config.remotePort);

    if (written == -1) {
        QString error = QString("Failed to write UDP datagram: %1").arg(m_socket->errorString());
        emit errorOccurred(error);
        qWarning() << error;
    } else {
        emit bytesWritten(written);
    }
}

void UdpLink::onReadyRead() {
    while (m_socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(static_cast<int>(m_socket->pendingDatagramSize()));

        QHostAddress senderAddress;
        quint16 senderPort;

        qint64 bytesRead =
            m_socket->readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);

        if (bytesRead > 0) {
            // Always update remote address from sender (for server mode)
            // This is necessary because Mission Planner uses random source ports
            if (m_config.isServer) {
                // Only log when address changes to reduce noise
                bool addressChanged = (m_config.remoteAddress != senderAddress || m_config.remotePort != senderPort);
                m_config.remoteAddress = senderAddress;
                m_config.remotePort = senderPort;

                if (addressChanged) {
                    qInfo() << "UDP Link remote address updated to:" << senderAddress.toString() << ":" << senderPort;
                }
            }

            datagram.resize(static_cast<int>(bytesRead));
            emit bytesReceived(datagram);
        }
    }
}

void UdpLink::onErrorOccurred(QAbstractSocket::SocketError socketError) {
    Q_UNUSED(socketError)
    QString error = QString("UDP socket error: %1").arg(m_socket->errorString());
    m_status = LinkStatus::Error;
    emit statusChanged(m_status);
    emit errorOccurred(error);
    qWarning() << error;
}
