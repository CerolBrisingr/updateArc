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

void writeIntoLineEdit(QLineEdit *edit, const QString &text)
{
    QTest::mouseDClick(edit, Qt::LeftButton, {}, edit->rect().center());
    QTest::keyClicks(edit, text);
}

class TestSettings : public QObject
{
    Q_OBJECT

public:
    TestSettings();
    ~TestSettings() override;

private:
    const QString _ini_path = "test.ini";
    const QString _test_property = "test_property";

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void test_hasKey();
    void test_binary_property();

    void test_checkbox_clicks();
    void test_checkbox_set_state();

    void test_line_edit();
};

TestSettings::TestSettings() {}

TestSettings::~TestSettings() {}

// Once before the first test case
void TestSettings::initTestCase()
{
    // Not actually anything in store for this
}

// Before each test case
void TestSettings::init()
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    QVERIFY(!setting.contains(_test_property));
    setting.setValue(_test_property, "initialized");
}

// After each test case
void TestSettings::cleanup()
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    setting.remove(_test_property);
}

// Once after the last test case
void TestSettings::cleanupTestCase()
{
    // Clean up default ini file.
    // Clean up any other file you use by hand!
    FileInteractions::removeFile("", "test.ini");
}

void TestSettings::test_hasKey()
{
    Settings settings(_ini_path);
    QVERIFY(settings.hasKey(_test_property));
    QCOMPARE(settings.getValue(_test_property), "initialized");

    settings.setValue(_test_property, "altered");
    QCOMPARE_NE(settings.getValue(_test_property), "initialized");
    QCOMPARE(settings.getValue(_test_property), "altered");


    const QString key = "another_key";
    QVERIFY(!settings.hasKey(key));

    settings.setValue(key, "stuff");
    QCOMPARE(settings.getValue(key), "stuff");
    QVERIFY(settings.hasKey(key));

    settings.removeKey(key);
    QVERIFY(!settings.hasKey(key));
}

void TestSettings::test_binary_property()
{
    Settings settings(_ini_path);
}

// Info: I'll bundle a few extra things in this test to remind myself of them.
void TestSettings::test_checkbox_clicks()
{
    Settings settings(_ini_path);

    // Create test window
    MainWindow testWindow;
    QCheckBox* const box = testWindow.ptrCheckBox;
    testWindow.show();

    // Make sure UI is in working order
    QVERIFY(QTest::qWaitForWindowExposed(&testWindow, 200));
    QVERIFY(box->isVisible());
    QVERIFY(box->isEnabled());

    // Spy on the signal we want to connect on, now because our constructor should NOT issue a signal
    QSignalSpy spy(box, &QCheckBox::checkStateChanged);
    QSignalSpy windowSpy(&testWindow, &MainWindow::onMousePressed);

    // Set up class to test
    CheckBoxSetting checkBoxSetting(box, _test_property, _ini_path);
    QCOMPARE(box->checkState(), Qt::Unchecked);
    QVERIFY(settings.hasKey(_test_property));
    QCOMPARE(settings.readBinary(_test_property), false);

    // Check spy status before test, verifying some assumptions in this test
    QCOMPARE(spy.count(), 0);
    QCOMPARE(windowSpy.count(), 0);

    // Interact with checkbox
    clickCheckbox(box);

    // Verify all desired reactions
    QCOMPARE(windowSpy.count(), 0);
    QCOMPARE(spy.count(), 1);  // We expect the signal to be emitted once
    QCOMPARE(box->checkState(), Qt::Checked);
    QCOMPARE(settings.readBinary(_test_property), true);
    QCOMPARE(checkBoxSetting.getSettingState(), true);

    // Interact with checkbox again
    clickCheckbox(box);

    // Verify all desired reactions
    QCOMPARE(windowSpy.count(), 0);
    QCOMPARE(spy.count(), 2);  // We expect the signal to be emitted once
    QCOMPARE(box->checkState(), Qt::Unchecked);
    QCOMPARE(settings.readBinary(_test_property), false);
    QCOMPARE(checkBoxSetting.getSettingState(), false);
}

void TestSettings::test_checkbox_set_state()
{
    Settings settings(_ini_path);
    settings.writeBinary(_test_property, true);

    // Create test window
    MainWindow testWindow;
    QCheckBox* const box = testWindow.ptrCheckBox;
    testWindow.show();

    QVERIFY(QTest::qWaitForWindowExposed(&testWindow, 200));
    QSignalSpy spy(box, &QCheckBox::checkStateChanged);

    // Set up class to test
    CheckBoxSetting checkBoxSetting(box, _test_property, _ini_path);
    QCOMPARE(box->checkState(), Qt::Checked);
    QCOMPARE(spy.count(), 1);  // We expect the signal to be emitted on init

    // Verify all desired reactions
    box->setCheckState(Qt::Checked);
    QCOMPARE(spy.count(), 1);  // We expect the signal to be not emitted again
    QCOMPARE(box->checkState(), Qt::Checked);
    QCOMPARE(settings.readBinary(_test_property), true);
    QCOMPARE(checkBoxSetting.getSettingState(), true);

    // Verify all desired reactions
    box->setCheckState(Qt::Unchecked);
    QCOMPARE(spy.count(), 2);  // We expect the signal to be emitted again
    QCOMPARE(box->checkState(), Qt::Unchecked);
    QCOMPARE(settings.readBinary(_test_property), false);
    QCOMPARE(checkBoxSetting.getSettingState(), false);

}

void TestSettings::test_line_edit()
{
    Settings settings(_ini_path);

    // Create test window
    MainWindow testWindow;
    QLineEdit* const edit = testWindow.ptrLineEdit;
    testWindow.show();

    QVERIFY(QTest::qWaitForWindowExposed(&testWindow, 200));
    QSignalSpy spy(edit, &QLineEdit::textChanged);

    // Set up class to test
    LineEditSettings lineSettings(edit, _test_property, "missing_value", _ini_path);
    QCOMPARE(edit->text(), "initialized");
    QCOMPARE(spy.count(), 1);  // We expect the signal to be emitted on init

    QString text = "some text";
    writeIntoLineEdit(edit, text);
    QCOMPARE(edit->text(), text);
    QCOMPARE(lineSettings.getValue(), text);
    QCOMPARE(settings.getValue(_test_property), text);
}

QTEST_MAIN(TestSettings)
#include "tst_settings.moc"
