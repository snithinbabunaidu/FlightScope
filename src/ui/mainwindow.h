#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include <QToolBar>
#include <QMenuBar>
#include <QLabel>
#include <QTimer>
#include <QDockWidget>
#include "../comm/linkmanager.h"
#include "../comm/mavlinkrouter.h"
#include "../comm/commandbus.h"
#include "../models/vehiclemodel.h"
#include "../models/healthmodel.h"
#include "../models/missionmodel.h"
#include "missioneditor.h"
#include "mapwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onConnectTriggered();
    void onDisconnectTriggered();
    void onAboutTriggered();

    void onConnectionStatusChanged(bool connected);
    void onLinkError(QString errorString);
    void onReconnecting(int attemptNumber, int delayMs);

    void updateTelemetryDisplay();
    void updateLinkStats();

    // Flight control slots
    void onGuidedTriggered();
    void onAutoTriggered();
    void onArmTriggered();
    void onDisarmTriggered();
    void onTakeoffTriggered();
    void onLandTriggered();
    void onRtlTriggered();
    void onStartMissionTriggered();
    void onCommandAck(uint16_t command, uint8_t result);

private:
    void setupUi();
    void setupMenus();
    void setupToolbars();
    void setupStatusBar();
    void setupDockWidgets();
    void setupConnections();

    Ui::MainWindow* ui;

    // Status bar widgets
    QLabel* m_connectionStatusLabel;
    QLabel* m_linkStatsLabel;

    // Core components
    LinkManager* m_linkManager;
    MavlinkRouter* m_mavlinkRouter;
    CommandBus* m_commandBus;
    VehicleModel* m_vehicleModel;
    HealthModel* m_healthModel;
    MissionModel* m_missionModel;

    // UI components
    QDockWidget* m_telemetryDock;
    QDockWidget* m_healthDock;
    QDockWidget* m_missionDock;
    QWidget* m_telemetryWidget;
    QWidget* m_healthWidget;
    MissionEditor* m_missionEditor;
    MapWidget* m_mapWidget;

    QAction* m_disconnectAction;
    QAction* m_disconnectToolAction;

    // Flight control actions
    QAction* m_guidedAction;
    QAction* m_autoAction;
    QAction* m_armAction;
    QAction* m_disarmAction;
    QAction* m_takeoffAction;
    QAction* m_landAction;
    QAction* m_rtlAction;
    QAction* m_startMissionAction;

    QTimer* m_updateTimer;
};

#endif  // MAINWINDOW_H