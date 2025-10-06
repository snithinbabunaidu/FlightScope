#include <QtTest>
#include <QApplication>
#include "../src/ui/mainwindow.h"

class SmokeTest : public QObject {
    Q_OBJECT

private slots:
    void testMainWindowCreation();
};

void SmokeTest::testMainWindowCreation() {
    MainWindow window;
    QVERIFY2(true, "MainWindow created successfully");

    // Check window is not null
    QVERIFY(&window != nullptr);

    // Check window has a title
    QVERIFY(!window.windowTitle().isEmpty());
}

QTEST_MAIN(SmokeTest)
#include "test_main.moc"