#include <QTest>
#include <QApplication>

#include "../MainWindow.h"

class SmokeTest : public QObject
{
    Q_OBJECT

  private slots:
    void testMainWindowCreation();
};

void SmokeTest::testMainWindowCreation()
{
    // This test checks that the main window is created successfully.
    MainWindow w;
    QVERIFY(w.isWindow());
}

// QTEST_MAIN is used for tests that require a QApplication instance.
QTEST_MAIN(SmokeTest)

#include "tst_smoketest.moc"
