#include <QTest>
#include <QCoreApplication>

#include "fileinteractions.h"
#include "settings.h"
#include "window.h"
#include <QSettings>
#include <QString>
#include <QSignalSpy>

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

void addOntoLineEdit(QLineEdit *edit, const QString &text)
{
    edit->setFocus();
    QTest::keyClicks(edit, text);
}

void clearLineEdit(QLineEdit *edit)
{
    edit->clear();
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
    void test_values();
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
    QCOMPARE(settings.readValue(_test_property), "initialized");

    settings.writeValue(_test_property, "altered");
    QCOMPARE_NE(settings.readValue(_test_property), "initialized");
    QCOMPARE(settings.readValue(_test_property), "altered");

    const QString key = "another_key";
    QVERIFY(!settings.hasKey(key));

    settings.writeValue(key, "stuff");
    QCOMPARE(settings.readValue(key), "stuff");
    QVERIFY(settings.hasKey(key));

    settings.removeKey(key);
    QVERIFY(!settings.hasKey(key));

    // Repetition should work without issues
    settings.removeKey(key);
    QVERIFY(!settings.hasKey(key));
}

void TestSettings::test_values()
{
    Settings settings(_ini_path);
    settings.readCreateValue(_test_property, "stuff");
    QCOMPARE(settings.readValue(_test_property), "initialized");

    settings.removeKey(_test_property);
    settings.readCreateValue(_test_property, "stuff");
    QCOMPARE(settings.readValue(_test_property), "stuff");

    settings.writeValue(_test_property, "other stuff");
    QCOMPARE(settings.readValue(_test_property), "other stuff");
}

void TestSettings::test_binary_property()
{
    Settings settings(_ini_path);
    QVERIFY(!settings.readBinary(_test_property));
    QVERIFY(settings.readBinary(_test_property, "", "initialized"));

    settings.removeKey(_test_property);
    QVERIFY(!settings.readBinary(_test_property, "", "initialized"));

    settings.readCreateBinary(_test_property, true);
    QVERIFY(settings.readBinary(_test_property));

    settings.writeBinary(_test_property, false);
    QVERIFY(!settings.readBinary(_test_property));

    settings.writeBinary(_test_property, true);
    QVERIFY(settings.readBinary(_test_property));

    settings.removeKey(_test_property);
    QVERIFY(!settings.readCreateBinary(_test_property, false, "ok", "nope"));
    QCOMPARE(settings.readValue(_test_property), "nope");
    QVERIFY(!settings.readCreateBinary(_test_property, true, "ok", "nope"));
    QCOMPARE(settings.readValue(_test_property), "nope");

    settings.writeBinary(_test_property, true, "ok", "nope");
    QCOMPARE(settings.readValue(_test_property), "ok");
    settings.writeBinary(_test_property, false, "ok", "nope");
    QCOMPARE(settings.readValue(_test_property), "nope");
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
    CheckBoxSettings checkBoxSetting(box, _test_property, _ini_path);
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
    CheckBoxSettings checkBoxSetting(box, _test_property, _ini_path);
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
    QCOMPARE(settings.readValue(_test_property), text);

    QString text2 = " and more";
    addOntoLineEdit(edit, text2);
    text2 = text + text2;
    QCOMPARE(edit->text(), text2);
    QCOMPARE(lineSettings.getValue(), text2);
    QCOMPARE(settings.readValue(_test_property), text2);

    // Count number of inputs so far
    QCOMPARE(spy.count(), text2.length() + 1);  // Include count after init

    text = "";
    clearLineEdit(edit);
    QCOMPARE(edit->text(), text);
    QCOMPARE(lineSettings.getValue(), text);
    QCOMPARE(settings.readValue(_test_property), text);
}

QTEST_MAIN(TestSettings)
#include "tst_settings.moc"
