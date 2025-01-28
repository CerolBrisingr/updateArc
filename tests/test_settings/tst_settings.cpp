#include <QtTest>
#include <QCoreApplication>

#include "fileinteractions.h"
#include <QSettings>
#include "settings.h"
#include <QString>

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLineEdit>

// https://doc.qt.io/qt-6/qmainwindow.html
// https://forum.qt.io/topic/84784/qtest-gui-getting-started
// https://doc.qt.io/qt-6/qttestlib-tutorial3-example.html
// https://stackoverflow.com/questions/16710924/qt-gui-unit-test-must-construct-a-qapplication-before-a-qpaintdevice

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow() {
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *mainLayout = new QVBoxLayout();

        ptrLineEdit = new QLineEdit();
        mainLayout->addWidget(ptrLineEdit);
        ptrCheckBox = new QCheckBox();
        mainLayout->addWidget(ptrCheckBox);

        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);
    }

public:
    QLineEdit *ptrLineEdit;
    QCheckBox *ptrCheckBox;
};

class TestSettings : public QObject
{
    Q_OBJECT

public:
    TestSettings();
    ~TestSettings() override;

private:
    const QString _ini_path = "test.ini";
    MainWindow window;

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
    window.setWindowTitle("Test settings.cpp");
    window.show();

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

    CheckBoxSetting(window.ptrCheckBox, checkedProperty, _ini_path);
    QVERIFY(settings.hasKey(checkedProperty));


}

QTEST_MAIN(TestSettings)

#include "tst_settings.moc"
