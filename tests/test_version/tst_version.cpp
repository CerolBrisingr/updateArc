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
    void test_vector_constructor1();
    void test_vector_constructor2();
    void test_vector_constructor3();

    void test_parse_constructor1();
    void test_parse_constructor2();
    void test_parse_constructor3();
    void test_parse_constructor4();
};

version::version() {}

version::~version() {}

void version::test_vector_constructor1() {
    std::vector<int> values = {0, 1, 2};
    Version test(values);
    QCOMPARE(test.getVersion(), values);
    QCOMPARE(test.toString(), "0.1.2");
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

void version::test_vector_constructor3() {
    std::vector<int> values = {4, 3, 2, 1};
    Version test(values);
    QCOMPARE(test, values);
    QCOMPARE(test.toString(), "4.3.2.1");
    QVERIFY(test.isClean());
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

QTEST_APPLESS_MAIN(version)

#include "tst_version.moc"
