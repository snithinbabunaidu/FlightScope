#include <QApplication>
#include "ui/mainwindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Set application metadata
    QCoreApplication::setOrganizationName("FlightScope");
    QCoreApplication::setOrganizationDomain("flightscope.org");
    QCoreApplication::setApplicationName("FlightScope GCS");
    QCoreApplication::setApplicationVersion("0.1.0");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}