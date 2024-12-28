#include <QtTest>

// add necessary includes here

class experience : public QObject
{
    Q_OBJECT

public:
    experience();
    ~experience();

private slots:
    void test_case1();
};

experience::experience() {}

experience::~experience() {}

void experience::test_case1() {}

QTEST_APPLESS_MAIN(experience)

#include "tst_experience.moc"
