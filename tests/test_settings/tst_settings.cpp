#include <QtTest>
#include <QCoreApplication>

#include "fileinteractions.h"
#include "settings.h"
#include "window.h"
#include <QSettings>
#include <QString>
#include <QSignalSpy>

// https://doc.qt.io/qt-6/qmainwindow.html
// https://forum.qt.io/topic/84784/qtest-gui-getting-started
// https://doc.qt.io/qt-6/qttestlib-tutorial3-example.html
// https://stackoverflow.com/questions/16710924/qt-gui-unit-test-must-construct-a-qapplication-before-a-qpaintdevice

class TestSettings : public QObject
{
    Q_OBJECT

public:
    TestSettings();
    ~TestSettings() override;

private:
    const QString _ini_path = "test.ini";

private slots:
    void initTestCase();
    void cleanupTestCase();

    void test_hasKey();
    void test_checkbox();
};

TestSettings::TestSettings() {}

TestSettings::~TestSettings() {}

void TestSettings::initTestCase()
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    setting.setValue("key", "value");
}

void TestSettings::cleanupTestCase()
{
    FileInteractions::removeFile("", "test.ini");
}

void TestSettings::test_hasKey()
{
    Settings settings(_ini_path);
    QVERIFY(!settings.hasKey("not_found"));
    QVERIFY(settings.hasKey("key"));
}

void TestSettings::test_checkbox()
{
    const QString checkedProperty = "set_active";
    Settings settings(_ini_path);

    MainWindow testWindow;
    testWindow.show();
    QVERIFY(QTest::qWaitForWindowExposed(&testWindow, 200));

    // Spy on the signal we want to connect on, now because our constructor should NOT issue a signal
    QSignalSpy spy(testWindow.ptrCheckBox, &QCheckBox::checkStateChanged);

    QVERIFY(!settings.hasKey(checkedProperty));

    QVERIFY(testWindow.ptrCheckBox->isVisible());
    QVERIFY(testWindow.ptrCheckBox->isEnabled());

    CheckBoxSetting checkBoxSetting(testWindow.ptrCheckBox, checkedProperty, _ini_path);
    QCOMPARE(testWindow.ptrCheckBox->checkState(), Qt::Unchecked);
    QVERIFY(settings.hasKey(checkedProperty));
    QCOMPARE(settings.readBinary(checkedProperty), false);

    QCOMPARE(spy.count(), 0);
    QCOMPARE(testWindow.ptrCheckBox->checkState(), Qt::Unchecked);
    testWindow.ptrCheckBox->setFocus();
    //QTest::mouseClick(testWindow.ptrCheckBox, Qt::LeftButton, {}, testWindow.ptrCheckBox->rect().center());
    //QCoreApplication::processEvents();
    testWindow.ptrCheckBox->setCheckState(Qt::Checked);
    QCOMPARE(spy.count(), 1);  // We expect the signal to be emitted once
    QCOMPARE(testWindow.ptrCheckBox->checkState(), Qt::Checked);
    QCOMPARE(settings.readBinary(checkedProperty), true);
    QCOMPARE(checkBoxSetting.getSettingState(), true);
}

QTEST_MAIN(TestSettings)
#include "tst_settings.moc"
