#ifndef VERSION_H
#define VERSION_H

#include <vector>
#include <QString>
#include <QRegularExpression>
#include "logger.h"

class Version
{
public:
    Version(const std::vector<int>& version_number);
    Version(const QString& tag, size_t n_digits, const QString& prefix="", const QString& suffix="");

    bool operator==(const Version& compareVersion) const;
    bool operator<(const Version& compareVersion) const;
    bool operator>(const Version& compareVersion) const;
    bool operator<=(const Version& compareVersion) const;
    bool operator>=(const Version& compareVersion) const;

    QString toString() const;
    const std::vector<int> &getVersion() const;
    bool isClean() const;

private:
    std::vector<int> _version_number;
    size_t _number_digits;
    bool _init_clean = true;

    QRegularExpression buildRegexp(const QString& prefix="", const QString& suffix="");
    void setVersion(const QString &tag, const QString &prefix, const QString &suffix);
    static bool biggerThan(const Version& left, const Version& right);
};

#endif // VERSION_H
