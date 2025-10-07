#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QAction>
#include <QMenu>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFormLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_connectionStatusLabel(nullptr),
      m_linkStatsLabel(nullptr), m_linkManager(nullptr), m_mavlinkRouter(nullptr),
      m_commandBus(nullptr), m_vehicleModel(nullptr), m_healthModel(nullptr),
      m_missionModel(nullptr), m_telemetryDock(nullptr), m_healthDock(nullptr),
      m_missionDock(nullptr), m_telemetryWidget(nullptr), m_healthWidget(nullptr),
      m_missionEditor(nullptr), m_mapWidget(nullptr), m_disconnectAction(nullptr),
      m_disconnectToolAction(nullptr), m_updateTimer(nullptr) {
    ui->setupUi(this);

    // Create core components
    m_linkManager = new LinkManager(this);
    m_mavlinkRouter = new MavlinkRouter(this);
    m_vehicleModel = new VehicleModel(this);
    m_healthModel = new HealthModel(this);
    m_missionModel = new MissionModel(this);
    m_commandBus = new CommandBus(m_mavlinkRouter, m_vehicleModel, this);
    m_updateTimer = new QTimer(this);

    setupUi();
    setupMenus();
    setupToolbars();
    setupStatusBar();
    setupDockWidgets();
    setupConnections();

    setWindowTitle("FlightScope - Ground Control Station");
    resize(1280, 720);

    // Start update timer (100ms = 10Hz UI update)
    m_updateTimer->setInterval(100);
    m_updateTimer->start();
}

MainWindow::~MainWindow() {
    if (m_linkManager) {
        m_linkManager->closeActiveLink();
    }
    delete ui;
}

void MainWindow::setupUi() {
    // Create map widget as central widget
    m_mapWidget = new MapWidget(this);
    setCentralWidget(m_mapWidget);

    // Connect map to models
    m_mapWidget->setVehicleModel(m_vehicleModel);
    m_mapWidget->setMissionModel(m_missionModel);
}

void MainWindow::setupMenus() {
    // File Menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

    QAction* connectAction = new QAction(tr("&Connect..."), this);
    connectAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    connect(connectAction, &QAction::triggered, this, &MainWindow::onConnectTriggered);
    fileMenu->addAction(connectAction);

    m_disconnectAction = new QAction(tr("&Disconnect"), this);
    m_disconnectAction->setEnabled(false);
    connect(m_disconnectAction, &QAction::triggered, this, &MainWindow::onDisconnectTriggered);
    fileMenu->addAction(m_disconnectAction);

    fileMenu->addSeparator();

    QAction* exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);

    // Help Menu
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction* aboutAction = new QAction(tr("&About FlightScope"), this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAboutTriggered);
    helpMenu->addAction(aboutAction);

    QAction* aboutQtAction = new QAction(tr("About &Qt"), this);
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::setupToolbars() {
    QToolBar* mainToolbar = addToolBar(tr("Main"));
    mainToolbar->setObjectName("MainToolbar");

    QAction* connectToolAction = new QAction(tr("Connect"), this);
    connect(connectToolAction, &QAction::triggered, this, &MainWindow::onConnectTriggered);
    mainToolbar->addAction(connectToolAction);

    m_disconnectToolAction = new QAction(tr("Disconnect"), this);
    m_disconnectToolAction->setEnabled(false);
    connect(m_disconnectToolAction, &QAction::triggered, this, &MainWindow::onDisconnectTriggered);
    mainToolbar->addAction(m_disconnectToolAction);

    mainToolbar->addSeparator();

    // Flight control toolbar
    QToolBar* flightToolbar = addToolBar(tr("Flight Control"));
    flightToolbar->setObjectName("FlightControlToolbar");

    m_guidedAction = new QAction(tr("Guided Mode"), this);
    m_guidedAction->setEnabled(false);
    connect(m_guidedAction, &QAction::triggered, this, &MainWindow::onGuidedTriggered);
    flightToolbar->addAction(m_guidedAction);

    m_autoAction = new QAction(tr("Auto Mode"), this);
    m_autoAction->setEnabled(false);
    connect(m_autoAction, &QAction::triggered, this, &MainWindow::onAutoTriggered);
    flightToolbar->addAction(m_autoAction);

    flightToolbar->addSeparator();

    m_armAction = new QAction(tr("Arm"), this);
    m_armAction->setEnabled(false);
    connect(m_armAction, &QAction::triggered, this, &MainWindow::onArmTriggered);
    flightToolbar->addAction(m_armAction);

    m_disarmAction = new QAction(tr("Disarm"), this);
    m_disarmAction->setEnabled(false);
    connect(m_disarmAction, &QAction::triggered, this, &MainWindow::onDisarmTriggered);
    flightToolbar->addAction(m_disarmAction);

    flightToolbar->addSeparator();

    m_takeoffAction = new QAction(tr("Takeoff"), this);
    m_takeoffAction->setEnabled(false);
    connect(m_takeoffAction, &QAction::triggered, this, &MainWindow::onTakeoffTriggered);
    flightToolbar->addAction(m_takeoffAction);

    m_landAction = new QAction(tr("Land"), this);
    m_landAction->setEnabled(false);
    connect(m_landAction, &QAction::triggered, this, &MainWindow::onLandTriggered);
    flightToolbar->addAction(m_landAction);

    m_rtlAction = new QAction(tr("RTL"), this);
    m_rtlAction->setEnabled(false);
    connect(m_rtlAction, &QAction::triggered, this, &MainWindow::onRtlTriggered);
    flightToolbar->addAction(m_rtlAction);

    flightToolbar->addSeparator();

    m_startMissionAction = new QAction(tr("Start Mission"), this);
    m_startMissionAction->setEnabled(false);
    connect(m_startMissionAction, &QAction::triggered, this, &MainWindow::onStartMissionTriggered);
    flightToolbar->addAction(m_startMissionAction);
}

void MainWindow::setupStatusBar() {
    m_connectionStatusLabel = new QLabel(tr("Not Connected"), this);
    m_connectionStatusLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusBar()->addPermanentWidget(m_connectionStatusLabel);
    
    m_linkStatsLabel = new QLabel(tr("No Link Stats"), this);
    m_linkStatsLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusBar()->addPermanentWidget(m_linkStatsLabel);
    
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setupDockWidgets() {
    // Telemetry Dock
    m_telemetryDock = new QDockWidget(tr("Telemetry"), this);
    m_telemetryWidget = new QWidget();
    auto* telemetryLayout = new QFormLayout(m_telemetryWidget);

    telemetryLayout->addRow(tr("Armed:"), new QLabel("Unknown"));
    telemetryLayout->addRow(tr("Flight Mode:"), new QLabel("Unknown"));
    telemetryLayout->addRow(tr("Altitude:"), new QLabel("0.0 m"));
    telemetryLayout->addRow(tr("Ground Speed:"), new QLabel("0.0 m/s"));
    telemetryLayout->addRow(tr("Battery:"), new QLabel("0.0 V"));

    m_telemetryDock->setWidget(m_telemetryWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_telemetryDock);

    // Health Dock
    m_healthDock = new QDockWidget(tr("System Health"), this);
    m_healthWidget = new QWidget();
    auto* healthLayout = new QFormLayout(m_healthWidget);

    healthLayout->addRow(tr("GPS Fix:"), new QLabel("No Fix"));
    healthLayout->addRow(tr("Satellites:"), new QLabel("0"));
    healthLayout->addRow(tr("GPS HDOP:"), new QLabel("0.0"));

    m_healthDock->setWidget(m_healthWidget);
    addDockWidget(Qt::RightDockWidgetArea, m_healthDock);

    // Mission Editor Dock
    m_missionDock = new QDockWidget(tr("Mission Editor"), this);
    m_missionEditor = new MissionEditor(m_missionModel, m_mavlinkRouter, m_vehicleModel, this);
    m_missionDock->setWidget(m_missionEditor);
    addDockWidget(Qt::LeftDockWidgetArea, m_missionDock);
}

void MainWindow::setupConnections() {
    // Link Manager <-> MAVLink Router
    connect(m_linkManager, &LinkManager::bytesReceived, m_mavlinkRouter,
            &MavlinkRouter::receiveBytes);
    connect(m_mavlinkRouter, &MavlinkRouter::bytesToSend, this,
            [this](QByteArray data) {
                if (m_linkManager->activeLink()) {
                    QMetaObject::invokeMethod(m_linkManager->activeLink(), "writeBytes",
                                              Qt::QueuedConnection, Q_ARG(QByteArray, data));
                }
            });

    // MAVLink Router -> Vehicle Model
    connect(m_mavlinkRouter, &MavlinkRouter::heartbeatReceived, m_vehicleModel,
            &VehicleModel::handleHeartbeat);
    connect(m_mavlinkRouter, &MavlinkRouter::attitudeReceived, m_vehicleModel,
            &VehicleModel::handleAttitude);
    connect(m_mavlinkRouter, &MavlinkRouter::globalPositionReceived, m_vehicleModel,
            &VehicleModel::handleGlobalPosition);
    connect(m_mavlinkRouter, &MavlinkRouter::vfrHudReceived, m_vehicleModel,
            &VehicleModel::handleVfrHud);
    connect(m_mavlinkRouter, &MavlinkRouter::batteryStatusReceived, m_vehicleModel,
            &VehicleModel::handleBatteryStatus);

    // MAVLink Router -> Health Model
    connect(m_mavlinkRouter, &MavlinkRouter::gpsRawReceived, m_healthModel,
            &HealthModel::handleGpsRaw);
    connect(m_mavlinkRouter, &MavlinkRouter::systemStatusReceived, m_healthModel,
            &HealthModel::handleSystemStatus);

    // MAVLink Router heartbeat -> Link Manager (reset timeout)
    connect(m_mavlinkRouter, &MavlinkRouter::heartbeatReceived, m_linkManager,
            &LinkManager::resetHeartbeatTimeout);

    // Link Manager status
    connect(m_linkManager, &LinkManager::connectionStatusChanged, this,
            &MainWindow::onConnectionStatusChanged);
    connect(m_linkManager, &LinkManager::linkError, this, &MainWindow::onLinkError);
    connect(m_linkManager, &LinkManager::reconnecting, this, &MainWindow::onReconnecting);

    // Update timer
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::updateTelemetryDisplay);
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::updateLinkStats);

    // CommandBus -> MainWindow (command acknowledgments)
    connect(m_mavlinkRouter, &MavlinkRouter::commandAckReceived, this,
            &MainWindow::onCommandAck);
}

void MainWindow::onConnectTriggered() {
    ConnectDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        UdpLink* link = dialog.getConfiguredLink();
        if (link) {
            m_linkManager->setActiveLink(link);
            statusBar()->showMessage(tr("Connecting to %1...").arg(link->name()));
        }
    }
}

void MainWindow::onDisconnectTriggered() {
    if (m_linkManager) {
        m_linkManager->closeActiveLink();
        statusBar()->showMessage(tr("Disconnected"));
    }
}

void MainWindow::onConnectionStatusChanged(bool connected) {
    if (connected) {
        m_connectionStatusLabel->setText(tr("Connected"));
        m_connectionStatusLabel->setStyleSheet("QLabel { background-color: green; color: white; }");
        m_disconnectAction->setEnabled(true);
        m_disconnectToolAction->setEnabled(true);

        // Enable flight control buttons
        m_guidedAction->setEnabled(true);
        m_autoAction->setEnabled(true);
        m_armAction->setEnabled(true);
        m_disarmAction->setEnabled(true);
        m_takeoffAction->setEnabled(true);
        m_landAction->setEnabled(true);
        m_rtlAction->setEnabled(true);
        m_startMissionAction->setEnabled(true);

        statusBar()->showMessage(tr("Connected successfully"), 3000);
    } else {
        m_connectionStatusLabel->setText(tr("Not Connected"));
        m_connectionStatusLabel->setStyleSheet("");
        m_disconnectAction->setEnabled(false);
        m_disconnectToolAction->setEnabled(false);

        // Disable flight control buttons
        m_guidedAction->setEnabled(false);
        m_autoAction->setEnabled(false);
        m_armAction->setEnabled(false);
        m_disarmAction->setEnabled(false);
        m_takeoffAction->setEnabled(false);
        m_landAction->setEnabled(false);
        m_rtlAction->setEnabled(false);
        m_startMissionAction->setEnabled(false);
    }
}

void MainWindow::onLinkError(QString errorString) {
    statusBar()->showMessage(tr("Link Error: %1").arg(errorString), 5000);
    QMessageBox::warning(this, tr("Link Error"), errorString);
}

void MainWindow::onReconnecting(int attemptNumber, int delayMs) {
    statusBar()->showMessage(
        tr("Reconnecting... (Attempt %1, waiting %2ms)").arg(attemptNumber).arg(delayMs));
}

void MainWindow::updateTelemetryDisplay() {
    // Update telemetry labels
    if (m_telemetryWidget && m_vehicleModel) {
        auto* layout = qobject_cast<QFormLayout*>(m_telemetryWidget->layout());
        if (layout) {
            // Update Armed status
            qobject_cast<QLabel*>(layout->itemAt(0, QFormLayout::FieldRole)->widget())
                ->setText(m_vehicleModel->armed() ? tr("ARMED") : tr("Disarmed"));

            // Update Flight Mode
            qobject_cast<QLabel*>(layout->itemAt(1, QFormLayout::FieldRole)->widget())
                ->setText(m_vehicleModel->flightMode());

            // Update Altitude
            qobject_cast<QLabel*>(layout->itemAt(2, QFormLayout::FieldRole)->widget())
                ->setText(QString("%1 m").arg(m_vehicleModel->relativeAltitude(), 0, 'f', 1));

            // Update Ground Speed
            qobject_cast<QLabel*>(layout->itemAt(3, QFormLayout::FieldRole)->widget())
                ->setText(QString("%1 m/s").arg(m_vehicleModel->groundSpeed(), 0, 'f', 1));

            // Update Battery
            qobject_cast<QLabel*>(layout->itemAt(4, QFormLayout::FieldRole)->widget())
                ->setText(QString("%1 V (%2%)")
                              .arg(m_vehicleModel->batteryVoltage(), 0, 'f', 1)
                              .arg(m_vehicleModel->batteryRemaining()));
        }
    }

    // Update health labels
    if (m_healthWidget && m_healthModel) {
        auto* layout = qobject_cast<QFormLayout*>(m_healthWidget->layout());
        if (layout) {
            // Update GPS Fix
            qobject_cast<QLabel*>(layout->itemAt(0, QFormLayout::FieldRole)->widget())
                ->setText(m_healthModel->gpsFixType());

            // Update Satellites
            qobject_cast<QLabel*>(layout->itemAt(1, QFormLayout::FieldRole)->widget())
                ->setText(QString::number(m_healthModel->satelliteCount()));

            // Update HDOP
            qobject_cast<QLabel*>(layout->itemAt(2, QFormLayout::FieldRole)->widget())
                ->setText(QString::number(m_healthModel->gpsHdop(), 'f', 1));
        }
    }
}

void MainWindow::updateLinkStats() {
    if (m_mavlinkRouter) {
        qint64 timeSince = m_mavlinkRouter->timeSinceLastMessage();
        float packetLoss = m_mavlinkRouter->packetLoss();
        qint64 rtt = m_mavlinkRouter->roundTripTime();

        m_linkStatsLabel->setText(
            QString("RTT: %1ms | Loss: %2% | Last msg: %3s ago")
                .arg(rtt)
                .arg(packetLoss, 0, 'f', 1)
                .arg(timeSince / 1000.0, 0, 'f', 1));
    }
}

void MainWindow::onAboutTriggered() {
    QMessageBox::about(
        this, tr("About FlightScope"),
        tr("<h2>FlightScope Ground Control Station</h2>"
           "<p>Version 0.1.0 - Phase 1 MVP</p>"
           "<p>A modular, fail-safe, operator-first GCS for UAV operations.</p>"
           "<p>Built with Qt/C++ for cross-platform performance.</p>"
           "<p><b>Features:</b></p>"
           "<ul>"
           "<li>MAVLink protocol support</li>"
           "<li>Real-time telemetry display</li>"
           "<li>Automatic reconnection with exponential backoff</li>"
           "<li>Link statistics monitoring</li>"
           "</ul>"
           "<p><b>Note:</b> This application uses OpenStreetMap tiles. "
           "Please respect their tile usage policy.</p>"));
}

void MainWindow::onGuidedTriggered() {
    m_commandBus->switchToGuided();
    statusBar()->showMessage(tr("Switching to GUIDED mode..."), 3000);
}

void MainWindow::onAutoTriggered() {
    m_commandBus->setMode(3);  // ArduCopter AUTO = 3
    statusBar()->showMessage(tr("Switching to AUTO mode..."), 3000);
}

void MainWindow::onArmTriggered() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Arm Vehicle"), tr("Are you sure you want to ARM the vehicle?"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_commandBus->arm();
        statusBar()->showMessage(tr("Sending ARM command..."), 3000);
    }
}

void MainWindow::onDisarmTriggered() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Disarm Vehicle"), tr("Are you sure you want to DISARM the vehicle?"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_commandBus->disarm();
        statusBar()->showMessage(tr("Sending DISARM command..."), 3000);
    }
}

void MainWindow::onTakeoffTriggered() {
    bool ok;
    double altitude =
        QInputDialog::getDouble(this, tr("Takeoff"), tr("Enter takeoff altitude (meters):"), 10.0,
                                1.0, 100.0, 1, &ok);

    if (ok) {
        m_commandBus->takeoff(static_cast<float>(altitude));
        statusBar()->showMessage(tr("Sending TAKEOFF command (altitude: %1m)...").arg(altitude),
                                 3000);
    }
}

void MainWindow::onLandTriggered() {
    QMessageBox::StandardButton reply =
        QMessageBox::question(this, tr("Land Vehicle"), tr("Are you sure you want to LAND?"),
                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_commandBus->land();
        statusBar()->showMessage(tr("Sending LAND command..."), 3000);
    }
}

void MainWindow::onRtlTriggered() {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Return to Launch"), tr("Are you sure you want to Return to Launch (RTL)?"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_commandBus->returnToLaunch();
        statusBar()->showMessage(tr("Sending RTL command..."), 3000);
    }
}

void MainWindow::onStartMissionTriggered() {
    // Just switch to AUTO mode - this will start the mission automatically
    m_commandBus->setMode(3);  // ArduCopter AUTO = 3
    statusBar()->showMessage(tr("Switching to AUTO mode to start mission..."), 3000);
}

void MainWindow::onCommandAck(uint16_t command, uint8_t result) {
    QString commandName;
    switch (command) {
        case MAV_CMD_COMPONENT_ARM_DISARM:
            commandName = "ARM/DISARM";
            break;
        case MAV_CMD_NAV_TAKEOFF:
            commandName = "TAKEOFF";
            break;
        case MAV_CMD_NAV_LAND:
            commandName = "LAND";
            break;
        case MAV_CMD_NAV_RETURN_TO_LAUNCH:
            commandName = "RTL";
            break;
        case MAV_CMD_DO_SET_MODE:
            commandName = "SET_MODE";
            break;
        case MAV_CMD_MISSION_START:
            commandName = "MISSION_START";
            break;
        default:
            commandName = QString::number(command);
            break;
    }

    QString resultStr;
    switch (result) {
        case MAV_RESULT_ACCEPTED:
            resultStr = "ACCEPTED";
            statusBar()->showMessage(
                tr("Command %1 accepted").arg(commandName), 3000);
            break;
        case MAV_RESULT_TEMPORARILY_REJECTED:
            resultStr = "TEMPORARILY REJECTED";
            QMessageBox::warning(this, tr("Command Rejected"),
                                 tr("Command %1 was temporarily rejected").arg(commandName));
            break;
        case MAV_RESULT_DENIED:
            resultStr = "DENIED";
            QMessageBox::warning(this, tr("Command Denied"),
                                 tr("Command %1 was denied").arg(commandName));
            break;
        case MAV_RESULT_UNSUPPORTED:
            resultStr = "UNSUPPORTED";
            QMessageBox::warning(this, tr("Command Unsupported"),
                                 tr("Command %1 is unsupported").arg(commandName));
            break;
        case MAV_RESULT_FAILED:
            resultStr = "FAILED";
            QMessageBox::warning(this, tr("Command Failed"),
                                 tr("Command %1 failed").arg(commandName));
            break;
        default:
            resultStr = QString::number(result);
            break;
    }

    qInfo() << "MainWindow: Command" << commandName << "result:" << resultStr;
}