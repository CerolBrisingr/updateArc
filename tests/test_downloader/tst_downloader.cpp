#include <QtTest>
#include "downloader.h"

// add necessary includes here

class downloader : public QObject
{
    Q_OBJECT

public:
    downloader();
    ~downloader() override;

private slots:
    void test_request();
};

downloader::downloader() {}

downloader::~downloader() {}

void downloader::test_request() {
    DownloadRequest test("http://www.neorice.com", DownloadType::HTMLBODY);
    QCOMPARE(test.getRequestType(), DownloadType::HTMLBODY);
    QCOMPARE(test.getRequestAddress(), "http://www.neorice.com");
    QCOMPARE(test.getTargetFilename(), "");
    QCOMPARE(test.getAllowedForwards(), 3);
}

QTEST_APPLESS_MAIN(downloader)

#include "tst_downloader.moc"
