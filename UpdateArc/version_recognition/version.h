#ifndef VERSION_H
#define VERSION_H

#include <vector>
#include <QString>
#include <QRegularExpression>
#include "logger.h"

class Version
{
public:
    Version(std::vector<int> version_number);
    Version(QString tag, size_t n_digits, QString prefix="", QString suffix="");

    bool operator==(const Version& compareVersion) const;
    bool operator<(const Version& compareVersion) const;
    bool operator>(const Version& compareVersion) const;
    bool operator<=(const Version& compareVersion) const;
    bool operator>=(const Version& compareVersion) const;

    QString toString() const;
    const std::vector<int> &getVersion() const;

private:
    std::vector<int> _version_number;
    size_t _number_digits;

    QRegularExpression buildRegexp(QString prefix="", QString suffix="");
    void setVersion(QString &tag, QString& prefix, QString& suffix);
    static bool biggerThan(const Version& left, const Version& right);
};

#endif // VERSION_H
