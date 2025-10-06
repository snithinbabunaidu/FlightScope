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
#include "../models/vehiclemodel.h"
#include "../models/healthmodel.h"

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
    VehicleModel* m_vehicleModel;
    HealthModel* m_healthModel;

    // UI components
    QDockWidget* m_telemetryDock;
    QDockWidget* m_healthDock;
    QWidget* m_telemetryWidget;
    QWidget* m_healthWidget;

    QAction* m_disconnectAction;
    QAction* m_disconnectToolAction;

    QTimer* m_updateTimer;
};

#endif  // MAINWINDOW_H