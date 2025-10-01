#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Forward declarations are better practice in header files
// This tells the compiler these classes exist without including the full header
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QUdpSocket;       // <-- Forward declare QUdpSocket
class MavlinkRouter;    // <-- Forward declare MavlinkRouter

class MainWindow : public QMainWindow
{
    Q_OBJECT // <-- This macro is ESSENTIAL for signals and slots to work

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots: // <-- The slot must be in a 'slots' section
    void readPendingDatagrams();

  private:
    Ui::MainWindow *ui;
    QUdpSocket *m_udpSocket = nullptr;
    MavlinkRouter *m_mavlinkRouter = nullptr;
};
#endif // MAINWINDOW_H
