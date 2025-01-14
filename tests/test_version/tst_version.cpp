#include <QtTest>
#include "version.h"
#include <vector>

class TestVersion : public QObject
{
    Q_OBJECT

public:
    TestVersion();
    ~TestVersion() override;

private slots:
    void test_vector_constructor_data();
    void test_vector_constructor();
    void test_vector_constructor_invalid();

    void test_parse_constructor_data();
    void test_parse_constructor();
    void test_parse_constructor_fail_data();
    void test_parse_constructor_fail();

    void test_compare_equal_data();
    void test_compare_equal();
    void test_compare_not_equal_data();
    void test_compare_not_equal();
};

TestVersion::TestVersion() {}

TestVersion::~TestVersion() {}

typedef std::vector<int> inData;

size_t s_t(int input)
{
    return static_cast<size_t>(input);
}

void TestVersion::test_vector_constructor_data()
{
    QTest::addColumn<inData>("values");
    QTest::addColumn<QString>("string");

    QTest::newRow("0.1.2") << inData({0, 1, 2}) << "0.1.2";
    QTest::newRow("1.2") << inData({1, 2}) << "1.2";
    QTest::newRow("0") << inData({0}) << "0";
}

void TestVersion::test_vector_constructor()
{
    QFETCH(inData, values);
    QFETCH(QString, string);
    Version test(values);
    QCOMPARE(test.getVersion(), values);
    QCOMPARE(test.toString(), string);
    QVERIFY(test.isClean());
}

void TestVersion::test_vector_constructor_invalid()
{
    inData values = {};
    inData fallback = {0};
    Version test(values);
    QCOMPARE(test.getVersion(), fallback);
    QCOMPARE(test.toString(), "0");
    QVERIFY(!test.isClean());
}

void TestVersion::test_parse_constructor_data()
{
    QTest::addColumn<QString>("tag");
    QTest::addColumn<size_t>("columns");
    QTest::addColumn<QString>("prefix");
    QTest::addColumn<QString>("suffix");
    QTest::addColumn<QString>("output");

    QTest::newRow("v4.1.0_release") << "v4.1.0_release" << s_t(3) << "v" << "_release" << "4.1.0";
    QTest::newRow("test4.34stuff") << "test4.34stuff" << s_t(2) << "test" << "stuff" << "4.34";
}

void TestVersion::test_parse_constructor()
{
    QFETCH(QString, tag);
    QFETCH(size_t, columns);
    QFETCH(QString, prefix);
    QFETCH(QString, suffix);
    QFETCH(QString, output);

    Version test(tag, columns, prefix, suffix);
    QCOMPARE(test.toString(), output);
    QVERIFY(test.isClean());
}

void TestVersion::test_parse_constructor_fail_data()
{
    QTest::addColumn<QString>("tag");
    QTest::addColumn<size_t>("columns");
    QTest::addColumn<QString>("prefix");
    QTest::addColumn<QString>("suffix");
    QTest::addColumn<QString>("output");

    QTest::newRow("v4.1.0_buggy") << "v4.1.0_buggy" << s_t(3) << "v" << "_release" << "0.0.0";
    QTest::newRow("v4.1.0_release") << "v4.1.0_release" << s_t(2) << "v" << "_release" << "0.0";
}

void TestVersion::test_parse_constructor_fail()
{
    QFETCH(QString, tag);
    QFETCH(size_t, columns);
    QFETCH(QString, prefix);
    QFETCH(QString, suffix);
    QFETCH(QString, output);

    Version test(tag, columns, prefix, suffix);
    QCOMPARE(test.toString(), output);
    QVERIFY(!test.isClean());
}

void TestVersion::test_compare_equal_data()
{
    QTest::addColumn<inData>("values1");
    QTest::addColumn<inData>("values2");
    QTest::addColumn<QString>("output");

    QTest::newRow("{1, 2, 3}") << inData({1, 2, 3}) << inData({1, 2, 3}) << "1.2.3";
    QTest::newRow("{5, 1}") << inData({5, 1}) << inData({5, 1}) << "5.1";
    QTest::newRow("{0}/{}") << inData({0}) << inData({}) << "0";
}

void TestVersion::test_compare_equal()
{
    QFETCH(inData, values1);
    QFETCH(inData, values2);
    QFETCH(QString, output);

    Version test1(values1);
    Version test2(values2);
    QVERIFY(test1 == test2);
    QVERIFY(test1 >= test2);
    QVERIFY(test1 <= test2);
    QVERIFY(test2 >= test1);
    QVERIFY(test2 <= test1);
    QCOMPARE(test1.toString(), output);
}

void TestVersion::test_compare_not_equal_data()
{
    QTest::addColumn<inData>("values_big");
    QTest::addColumn<inData>("values_small");

    QTest::addRow("{4, 2, 1}/{4, 1, 1}") << inData({4, 2, 1}) << inData({4, 1, 1});
    QTest::addRow("{4, 2, 1, 1}/{4, 2, 1}") << inData({4, 2, 1, 1}) << inData({4, 2, 1});
    QTest::addRow("{4, 3, 1}/{4, 2, 1, 1}") << inData({4, 3, 1}) << inData({4, 2, 1, 1});
}

void TestVersion::test_compare_not_equal()
{
    QFETCH(inData, values_big);
    QFETCH(inData, values_small);

    Version test1(values_big);
    Version test2(values_small);
    QVERIFY(test1 != test2);
    QVERIFY(test1 > test2);
    QVERIFY(test1 >= test2);
    QVERIFY(test2 < test1);
    QVERIFY(test2 <= test1);
}

QTEST_APPLESS_MAIN(TestVersion)

#include "tst_version.moc"
