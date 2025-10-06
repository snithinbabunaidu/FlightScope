#include "udplink.h"
#include <QDebug>

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
    if (m_status == LinkStatus::Connected || m_status == LinkStatus::Connecting) {
        qWarning() << "UdpLink::connectLink() - Already connected or connecting";
        return;
    }

    m_status = LinkStatus::Connecting;
    emit statusChanged(m_status);

    // Create socket
    m_socket = new QUdpSocket(this);

    // Connect signals
    connect(m_socket, &QUdpSocket::readyRead, this, &UdpLink::onReadyRead);
    connect(m_socket, &QUdpSocket::errorOccurred, this, &UdpLink::onErrorOccurred);

    // Bind to local port
    if (m_socket->bind(m_config.localAddress, m_config.localPort,
                       QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        m_status = LinkStatus::Connected;
        emit statusChanged(m_status);
        qDebug() << "UDP Link connected:" << m_config.name << "listening on"
                 << m_config.localAddress.toString() << ":" << m_config.localPort;
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
            // Update remote address if we haven't set it yet (for server mode)
            if (m_config.isServer && m_config.remoteAddress.isNull()) {
                m_config.remoteAddress = senderAddress;
                m_config.remotePort = senderPort;
                qDebug() << "UDP Link learned remote address:" << senderAddress.toString() << ":"
                         << senderPort;
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
