#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mavlinkrouter.h"
#include "vehiclemodel.h"
#include "healthmodel.h"
#include <mavlink/common/mavlink.h>
#include <QDebug>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_mavlinkRouter = new MavlinkRouter(this);
    m_vehicleModel = new VehicleModel(this);
    m_healthModel = new HealthModel(this);
    m_udpSocket = new QUdpSocket(this);
    m_kickstartTimer = new QTimer(this);

    m_udpSocket->bind(QHostAddress::AnyIPv4, 14551);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &MainWindow::readPendingDatagrams);

    connect(m_mavlinkRouter, &MavlinkRouter::vehicleArmed, m_vehicleModel, &VehicleModel::setArmed);


    connect(m_vehicleModel, &VehicleModel::armedChanged, this, [](bool armed){
        qDebug() << "--- [MODEL DATA] --- Vehicle Armed Status Updated:" << (armed ? "ARMED" : "DISARMED");
    });

    connect(m_kickstartTimer, &QTimer::timeout, this, &MainWindow::sendKickstartPacket);
        m_kickstartTimer->start(1000);

    connect(m_healthModel, &HealthModel::gpsFixTypeChanged, this, [](int fix){
        qDebug() << "--- [MODEL DATA] --- GPS Fix Type Updated:" << fix;
    });

    qDebug() << "FlightScope Initialized. Waiting for SITL connection...";

    QByteArray dummyData("hello");
    m_udpSocket->writeDatagram(dummyData, QHostAddress::LocalHost, 14550);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendKickstartPacket()
{
    qDebug() << "Sending kickstart packet to SITL...";
    QByteArray dummyData("hello");
    m_udpSocket->writeDatagram(dummyData, QHostAddress::LocalHost, 14550);
}

void MainWindow::readPendingDatagrams()
{

    if (m_kickstartTimer->isActive()) {
        m_kickstartTimer->stop();
        qDebug() << "Connection established! Stopping kickstart timer.";
    }

    while (m_udpSocket->hasPendingDatagrams()) {

        QNetworkDatagram datagram = m_udpSocket->receiveDatagram();
        QByteArray data = datagram.data();

        mavlink_message_t msg;
        mavlink_status_t status;

        for (int i = 0; i < data.size(); ++i) {
            if (mavlink_parse_char(MAVLINK_COMM_0, static_cast<uint8_t>(data.at(i)), &msg, &status)) {
                m_mavlinkRouter->parseMessage(msg);
            }
        }
    }
}
