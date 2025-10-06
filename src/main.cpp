#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QtMessageHandler>
#include "ui/mainwindow.h"

// Global log file
QFile* g_logFile = nullptr;

// Custom message handler to write to file
void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    if (g_logFile && g_logFile->isOpen()) {
        QTextStream out(g_logFile);
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
        QString typeStr;

        switch (type) {
        case QtDebugMsg:    typeStr = "DEBUG"; break;
        case QtInfoMsg:     typeStr = "INFO "; break;
        case QtWarningMsg:  typeStr = "WARN "; break;
        case QtCriticalMsg: typeStr = "CRIT "; break;
        case QtFatalMsg:    typeStr = "FATAL"; break;
        }

        out << timestamp << " [" << typeStr << "] " << msg << "\n";
        out.flush();
    }
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Setup file logging
    g_logFile = new QFile("flightscope_debug.log");
    if (g_logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        qInstallMessageHandler(messageHandler);
        qDebug() << "FlightScope started - logging to flightscope_debug.log";
    }
    
    // Set application metadata
    QCoreApplication::setOrganizationName("FlightScope");
    QCoreApplication::setOrganizationDomain("flightscope.org");
    QCoreApplication::setApplicationName("FlightScope GCS");
    QCoreApplication::setApplicationVersion("0.1.0");
    
    MainWindow window;
    window.show();

    int result = app.exec();

    // Cleanup
    if (g_logFile) {
        qDebug() << "FlightScope shutting down";
        g_logFile->close();
        delete g_logFile;
    }

    return result;
}