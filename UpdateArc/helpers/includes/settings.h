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

    bool hasKey(const QString key) const;
    bool readBinary(const QString key, const QString s_true = "on", const QString s_default = "") const;
    bool readCreateBinary(const QString key, const QString s_true, const QString s_false, const QString s_default);
    QString getValue(const QString key, const QString default_value = "") const;
    QString getValueWrite(const QString key, const QString default_value = "");
    void setValue(const QString key, const QString value);
    void removeKey(const QString key);

private:
    const QString _ini_path;
};

// Fuse checkbox to settings key value
class CheckBoxSetting:  public QObject
{
    Q_OBJECT
public:
    CheckBoxSetting(QCheckBox* checkbox, const QString key, const QString iniPath = "settings.ini");

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
