#include "version.h"

Version::Version(const std::vector<int>& version_number)
    :_version_number(version_number)
    ,_number_digits(version_number.size())
{
    assureMinimumLength();
}

void Version::assureMinimumLength()
{
    // Version must be at least 1 major version number
    if (_number_digits < 1) {
        _number_digits = 1;
        _version_number = {0};
        _init_clean = false;    // Input modified
    }
}

Version::Version(const QString& tag, size_t n_digits, const QString& prefix, const QString& suffix)
    :_number_digits(n_digits)
{
    setVersion(tag, prefix, suffix);
}

bool Version::operator==(const Version& compareVersion) const
{
    const auto& other_version = compareVersion.getVersion();
    // Version is not the same if number of places differs
    if (other_version.size() != _version_number.size()) {
        return false;
    }
    // Sizes are equal, compare big to small version numbers
    for (size_t iter=0; iter < _version_number.size(); iter++) {
        if(other_version.at(iter) != _version_number.at(iter)) {
            return false;
        }
    }
    return true;
}

bool Version::operator<(const Version &compareVersion) const
{
    return biggerThan(compareVersion, *this);
}

bool Version::operator>(const Version &compareVersion) const
{
    return biggerThan(*this, compareVersion);
}

bool Version::operator<=(const Version &compareVersion) const
{
    if (*this == compareVersion)
        return true;
    return biggerThan(compareVersion, *this);
}

bool Version::operator>=(const Version &compareVersion) const
{
    if (*this == compareVersion)
        return true;
    return biggerThan(*this, compareVersion);
}

QString Version::toString() const
{
    if (_number_digits < 1)
        return "0";
    else {
        QString version_string;
        for (auto it = _version_number.begin(); it != _version_number.end(); it++) {
            if (it != _version_number.begin()) {
                version_string += ".";
            }
            version_string += QString::number(*it);
        }
        return version_string;
    }
}

const std::vector<int> &Version::getVersion() const
{
    return _version_number;
}

bool Version::isClean() const
{
    return _init_clean;
}

QRegularExpression Version::buildRegexp(const QString& prefix, const QString& suffix)
{
    QString expression;
    for (size_t iter = 0; iter < _number_digits; iter++) {
        if (iter > 0) {
            expression += "\\."; // Numbers are seperated by dots
        }
        expression += "(\\d+)";  // Expression for new number
    }
    // Build complete regular expression
    return QRegularExpression("^" + prefix + expression + suffix + "$");
}

void Version::setVersion(const QString& tag, const QString &prefix, const QString &suffix)
{
    _version_number.clear();
    auto re = buildRegexp(prefix, suffix);
    //QRegularExpression re2("v(\\d+)\\.(\\d+)\\.(\\d+)");
    QRegularExpressionMatch match = re.match(tag);
    if (!match.hasMatch()) {
        Log::write("Failed to read version tag \"" + tag + "\"\n");
        _version_number = std::vector<int>(_number_digits, 0); // Set all elements to 0
        _init_clean = false;  // Input not parsed correctly
        return;
    }
    for (int iter=1; iter <= static_cast<int>(_number_digits); iter++) {
        _version_number.push_back(match.captured(iter).toInt());
    }
    _init_clean = true;
    return;
}

bool Version::biggerThan(const Version &left, const Version &right)
{
    // Find longest version number vector length
    const auto& leftVersion = left.getVersion();
    const auto& rightVersion = right.getVersion();
    size_t n_digits = std::min(leftVersion.size(), rightVersion.size());
    // Compare all common places
    for (size_t iter=0; iter < n_digits; iter++) {
        if (leftVersion.at(iter) == rightVersion.at(iter)) {
            continue;
        }
        if (leftVersion.at(iter) > rightVersion.at(iter)) {
            return true;
        } else {
            return false;
        }
    }
    // All common places are equal? Longer vector wins
    if (leftVersion.size() > rightVersion.size()) {
        return true;
    } else {
        return false;
    }

}
