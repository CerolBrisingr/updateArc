#include <QtTest>

#include "fileinteractions.h"
#include <QSettings>
#include "settings.h"
#include <QString>


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

QTEST_APPLESS_MAIN(TestSettings)

#include "tst_settings.moc"
