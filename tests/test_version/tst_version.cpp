#include <QtTest>
#include "version.h"
#include <vector>

class version : public QObject
{
    Q_OBJECT

public:
    version();
    ~version() override;

private slots:
    void test_vector_constructor1_data();
    void test_vector_constructor1();
    void test_vector_constructor2();

    void test_parse_constructor1();
    void test_parse_constructor2();
    void test_parse_constructor3();
    void test_parse_constructor4();

    void test_compare_equal1();
    void test_compare_equal2();

    void test_compare_not_equal1();
    void test_compare_not_equal2();
    void test_compare_not_equal3();
};

version::version() {}

version::~version() {}

void version::test_vector_constructor1_data() {
    QTest::addColumn<std::vector<int>>("values");
    QTest::addColumn<QString>("string");

    QTest::newRow("0.1.2") << std::vector<int>({0, 1, 2}) << "0.1.2";
    QTest::newRow("1.2") << std::vector<int>({1, 2}) << "1.2";
    QTest::newRow("0") << std::vector<int>({0}) << "0";
}

void version::test_vector_constructor1() {
    QFETCH(std::vector<int>, values);
    QFETCH(QString, string);
    Version test(values);
    QCOMPARE(test.getVersion(), values);
    QCOMPARE(test.toString(), string);
    QVERIFY(test.isClean());
}

void version::test_vector_constructor2() {
    std::vector<int> values = {};
    std::vector<int> fallback = {0};
    Version test(values);
    QCOMPARE(test.getVersion(), fallback);
    QCOMPARE(test.toString(), "0");
    QVERIFY(!test.isClean());
}

void version::test_parse_constructor1() {
    Version test("v4.1.0_release", 3, "v", "_release");
    QCOMPARE(test.toString(), "4.1.0");
    QVERIFY(test.isClean());
}

void version::test_parse_constructor2() {
    Version test("v4.1.0_buggy", 3, "v", "_realease");
    QCOMPARE(test.toString(), "0.0.0");
    QVERIFY(!test.isClean());
}

void version::test_parse_constructor3() {
    Version test("v4.1.0_release", 2, "v", "_release");
    QCOMPARE(test.toString(), "0.0");
    QVERIFY(!test.isClean());
}

void version::test_parse_constructor4() {
    Version test("test4.34stuff", 2, "test", "stuff");
    QCOMPARE(test.toString(), "4.34");
    QVERIFY(test.isClean());
}

void version::test_compare_equal1() {
    Version test1({1, 2, 3});
    Version test2({1, 2, 3});
    QVERIFY(test1 == test2);
    QVERIFY(test1 >= test2);
    QVERIFY(test1 <= test2);
    QVERIFY(test2 >= test1);
    QVERIFY(test2 <= test1);
    QCOMPARE(test1.toString(), "1.2.3");
}

void version::test_compare_equal2() {
    Version test1({0});
    Version test2({});
    QVERIFY(test1 == test2);
    QCOMPARE(test2.toString(), "0");
}

void version::test_compare_not_equal1() {
    Version test1({4, 2, 1});
    Version test2({4, 1, 1});
    QVERIFY(test1 != test2);
    QVERIFY(test1 > test2);
    QVERIFY(test1 >= test2);
    QVERIFY(test2 < test1);
    QVERIFY(test2 <= test1);
}

void version::test_compare_not_equal2() {
    Version test1({4, 2, 1});
    Version test2({4, 2, 1, 1});
    QVERIFY(test1 != test2);
    QVERIFY(test1 < test2);
    QVERIFY(test1 <= test2);
    QVERIFY(test2 > test1);
    QVERIFY(test2 >= test1);
}

void version::test_compare_not_equal3() {
    Version test1({4, 3, 1});
    Version test2({4, 2, 1, 1});
    QVERIFY(test1 != test2);
    QVERIFY(test1 > test2);
    QVERIFY(test1 >= test2);
    QVERIFY(test2 < test1);
    QVERIFY(test2 <= test1);
}


QTEST_APPLESS_MAIN(version)

#include "tst_version.moc"
