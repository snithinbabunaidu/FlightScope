#ifndef UDPLINK_H
#define UDPLINK_H

#include "linkinterface.h"
#include <QUdpSocket>
#include <QHostAddress>

/**
 * @brief UDP communication link implementation
 *
 * Supports both client (connect to remote) and server (listen) modes.
 * Designed to run on a separate QThread.
 */
class UdpLink : public LinkInterface {
    Q_OBJECT

public:
    struct Configuration {
        QString name;
        QHostAddress localAddress{QHostAddress::Any};
        quint16 localPort{14550};
        QHostAddress remoteAddress{QHostAddress::LocalHost};
        quint16 remotePort{14550};
        bool isServer{true};  // true = listen mode, false = client mode
    };

    explicit UdpLink(const Configuration& config, QObject* parent = nullptr);
    ~UdpLink() override;

    QString name() const override;
    LinkStatus status() const override;
    bool isConnected() const override;

public slots:
    void connectLink() override;
    void disconnectLink() override;
    void writeBytes(const QByteArray& data) override;

private slots:
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    Configuration m_config;
    QUdpSocket* m_socket;
    LinkStatus m_status;
};

#endif  // UDPLINK_H
