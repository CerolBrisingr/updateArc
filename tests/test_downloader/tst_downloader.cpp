#include <QtTest>

// add necessary includes here

class downloader : public QObject
{
    Q_OBJECT

public:
    downloader();
    ~downloader();

private slots:
    void test_case1();
};

downloader::downloader() {}

downloader::~downloader() {}

void downloader::test_case1() {}

QTEST_APPLESS_MAIN(downloader)

#include "tst_downloader.moc"
