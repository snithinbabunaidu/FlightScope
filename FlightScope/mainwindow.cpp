#include "mainwindow.h"
#include "mavlinkrouter.h"
#include "ui_mainwindow.h"
#include <mavlink/common/mavlink.h>
#include <QDebug>
#include <QUdpSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

           // 1. Initialize the MavlinkRouter
    m_mavlinkRouter = new MavlinkRouter(this);

           // 2. Initialize the UDP Socket
    m_udpSocket = new QUdpSocket(this);
    m_udpSocket->bind(QHostAddress::Any, 14551); // Listen for replies from the vehicle

           // 3. Connect the socket's readyRead signal to our new slot
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);

           // 4. Connect the router's signals to debug slots for testing
    connect(m_mavlinkRouter, &MavlinkRouter::vehicleArmed, this, [](bool armed){
        qDebug() << "Vehicle Armed Status:" << armed;
    });

    connect(m_mavlinkRouter, &MavlinkRouter::rttCalculated, this, [](qint64 rtt_ms){
        qDebug() << "Round Trip Time:" << rtt_ms << "ms";
    });

    qDebug() << "FlightScope Initialized. SITL should be running and sending to UDP 14550.";

    // To start receiving, we need to send at least one packet to the SITL vehicle
    // so it knows where to send the data. We send to the standard SITL port 14550.
    QByteArray dummyData("1");
    m_udpSocket->writeDatagram(dummyData, QHostAddress::LocalHost, 14550);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readPendingDatagrams()
{
    while (m_udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        m_udpSocket->readDatagram(datagram.data(), datagram.size(),
                                  &sender, &senderPort);

        mavlink_message_t msg;
        mavlink_status_t status;

        for (int i = 0; i < datagram.size(); ++i) {
            if (mavlink_parse_char(MAVLINK_COMM_0, static_cast<uint8_t>(datagram.at(i)), &msg, &status)) {
                // When a complete message is parsed, send it to the router
                m_mavlinkRouter->parseMessage(msg);
            }
        }
    }
}
