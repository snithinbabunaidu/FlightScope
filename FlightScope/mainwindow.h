#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Forward declarations
class QUdpSocket;
class MavlinkRouter;
class VehicleModel;
class HealthModel;
class QTimer; // <-- Add QTimer forward declaration

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void readPendingDatagrams();
    void sendKickstartPacket();

  private:
    Ui::MainWindow *ui;
    QUdpSocket *m_udpSocket = nullptr;
    MavlinkRouter *m_mavlinkRouter = nullptr;
    VehicleModel *m_vehicleModel = nullptr;
    HealthModel *m_healthModel = nullptr;
    QTimer *m_kickstartTimer = nullptr;
};
#endif // MAINWINDOW_H
