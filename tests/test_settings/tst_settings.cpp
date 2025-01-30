#include <QTest>
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

#include <QStyle>
#include <QStyleOptionButton>

void clickCheckbox(QCheckBox *box)
{
    QStyleOptionButton opt;
    opt.initFrom(box);
    // Find interactive checkbox area
    QRect checkBoxRect = box->style()->subElementRect(QStyle::SE_CheckBoxIndicator, &opt, box);
    // Click into center of interactive area
    QTest::mouseClick(box, Qt::LeftButton, {}, checkBoxRect.center());
}

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
    QCheckBox* const box = testWindow.ptrCheckBox;
    testWindow.show();
    QVERIFY(QTest::qWaitForWindowExposed(&testWindow, 200));

    // Spy on the signal we want to connect on, now because our constructor should NOT issue a signal
    QSignalSpy spy(box, &QCheckBox::checkStateChanged);
    QSignalSpy windowSpy(&testWindow, &MainWindow::onMousePressed);

    QVERIFY(!settings.hasKey(checkedProperty));

    QVERIFY(box->isVisible());
    QVERIFY(box->isEnabled());

    CheckBoxSetting checkBoxSetting(box, checkedProperty, _ini_path);
    QCOMPARE(box->checkState(), Qt::Unchecked);
    QVERIFY(settings.hasKey(checkedProperty));
    QCOMPARE(settings.readBinary(checkedProperty), false);

    QCOMPARE(spy.count(), 0);
    QCOMPARE(windowSpy.count(), 0);
    QCOMPARE(box->checkState(), Qt::Unchecked);
    box->setFocus();

    clickCheckbox(box);

    box->setCheckState(Qt::Checked);
    QCOMPARE(windowSpy.count(), 0);
    QCOMPARE(spy.count(), 1);  // We expect the signal to be emitted once
    QCOMPARE(box->checkState(), Qt::Checked);
    QCOMPARE(settings.readBinary(checkedProperty), true);
    QCOMPARE(checkBoxSetting.getSettingState(), true);
}

QTEST_MAIN(TestSettings)
#include "tst_settings.moc"
