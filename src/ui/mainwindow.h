#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStatusBar>
#include <QToolBar>
#include <QMenuBar>
#include <QLabel>

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

private:
    void setupUi();
    void setupMenus();
    void setupToolbars();
    void setupStatusBar();
    
    Ui::MainWindow* ui;
    QLabel* m_connectionStatusLabel;
    QLabel* m_linkStatsLabel;
};

#endif  // MAINWINDOW_H