#include <QtTest>
#include "downloader.h"

class TestDownloader : public QObject
{
    Q_OBJECT

public:
    TestDownloader();
    ~TestDownloader() override;

private slots:
    void test_request();
};

TestDownloader::TestDownloader() {}

TestDownloader::~TestDownloader() {}

void TestDownloader::test_request() {
    DownloadRequest test("http://www.neorice.com", DownloadType::HTMLBODY);
    QCOMPARE(test.getRequestType(), DownloadType::HTMLBODY);
    QCOMPARE(test.getRequestAddress(), "http://www.neorice.com");
    QCOMPARE(test.getTargetFilename(), "");
    QCOMPARE(test.getAllowedForwards(), 3);
}

QTEST_APPLESS_MAIN(TestDownloader)

#include "tst_downloader.moc"
