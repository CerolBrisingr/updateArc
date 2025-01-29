#include <QtTest>
#include <QCoreApplication>

#include "fileinteractions.h"
#include "settings.h"
#include "window.h"
#include <QSettings>
#include <QString>

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
    MainWindow testWindow;

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
    testWindow.setWindowTitle("Test settings.cpp");
    testWindow.show();

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

    QVERIFY(!settings.hasKey(checkedProperty));

    CheckBoxSetting(testWindow.ptrCheckBox, checkedProperty, _ini_path);
    QCOMPARE(testWindow.ptrCheckBox->checkState(), Qt::Unchecked);
    QVERIFY(settings.hasKey(checkedProperty));
    QCOMPARE(settings.readBinary(checkedProperty), false);

    QCOMPARE(testWindow.ptrCheckBox->checkState(), Qt::Unchecked);
    QTest::mouseClick(testWindow.ptrCheckBox, Qt::LeftButton );
    //window.ptrCheckBox->setChecked(true);
    QCOMPARE(testWindow.ptrCheckBox->checkState(), Qt::Checked);
    QTest::qWait(500);
    QCOMPARE(settings.readBinary(checkedProperty), true);
}

QTEST_MAIN(TestSettings)

#include "tst_settings.moc"
