#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QCheckBox>
#include <QLineEdit>

class Settings
{
public:
    Settings(const QString ini_path);

    // Basic key operations
    bool hasKey(const QString key) const;
    void removeKey(const QString key);

    // Working with string values
    void writeValue(const QString key, const QString value);
    QString readValue(const QString key, const QString default_value = "") const;
    QString readCreateValue(const QString key, const QString default_value = "");

    // Simulating binary values
    void writeBinary(const QString key, const bool value, const QString s_true = "on", const QString s_false = "off");
    bool readBinary(const QString key, const QString s_default = "", const QString s_true = "on") const;
    bool readCreateBinary(const QString key, const bool defaultValue = false, const QString s_true = "on", const QString s_false = "off");

private:
    const QString _ini_path;
};

// Fuse checkbox to settings key value
class CheckBoxSettings:  public QObject
{
    Q_OBJECT
public:
    CheckBoxSettings(QCheckBox* checkbox, const QString key, const QString iniPath = "settings.ini");

    bool getSettingState() const;

private slots:
    void checkboxChanged(Qt::CheckState state);

private:
    Settings _settings;
    QString _key;

};

// Fuse line edit to settings key value
class LineEditSettings: public QObject
{
    Q_OBJECT
public:
    LineEditSettings(QLineEdit* lineedit, const QString key, const QString default_entry = "", const QString iniPath = "settings.ini");

    QString getValue() const;

private slots:
    void lineeditChanged(const QString &text);

private:
    Settings _settings;
    const QString _key;
    const QString _defaultEntry;
};

#endif // SETTINGS_H
