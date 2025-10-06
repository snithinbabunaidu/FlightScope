#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QAction>
#include <QMenu>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_connectionStatusLabel(nullptr),
      m_linkStatsLabel(nullptr) {
    ui->setupUi(this);
    setupUi();
    setupMenus();
    setupToolbars();
    setupStatusBar();
    
    setWindowTitle("FlightScope - Ground Control Station");
    resize(1280, 720);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupUi() {
    // Central widget will be set up in later phases
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
}

void MainWindow::setupMenus() {
    // File Menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    
    QAction* connectAction = new QAction(tr("&Connect..."), this);
    connectAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    connect(connectAction, &QAction::triggered, this, &MainWindow::onConnectTriggered);
    fileMenu->addAction(connectAction);
    
    QAction* disconnectAction = new QAction(tr("&Disconnect"), this);
    disconnectAction->setEnabled(false);
    connect(disconnectAction, &QAction::triggered, this, &MainWindow::onDisconnectTriggered);
    fileMenu->addAction(disconnectAction);
    
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
    
    QAction* disconnectToolAction = new QAction(tr("Disconnect"), this);
    disconnectToolAction->setEnabled(false);
    connect(disconnectToolAction, &QAction::triggered, this, &MainWindow::onDisconnectTriggered);
    mainToolbar->addAction(disconnectToolAction);
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

void MainWindow::onConnectTriggered() {
    // Placeholder - will be implemented in Phase 1, Task 1.1
    statusBar()->showMessage(tr("Connect dialog will be implemented in Phase 1"), 3000);
}

void MainWindow::onDisconnectTriggered() {
    // Placeholder - will be implemented in Phase 1, Task 1.1
    statusBar()->showMessage(tr("Disconnect functionality will be implemented in Phase 1"), 3000);
}

void MainWindow::onAboutTriggered() {
    QMessageBox::about(
        this, tr("About FlightScope"),
        tr("<h2>FlightScope Ground Control Station</h2>"
           "<p>Version 0.1.0</p>"
           "<p>A modular, fail-safe, operator-first GCS for UAV operations.</p>"
           "<p>Built with Qt/C++ for cross-platform performance.</p>"
           "<p><b>Note:</b> This application uses OpenStreetMap tiles. "
           "Please respect their tile usage policy.</p>"));
}