#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QCheckBox>

class Settings
{
public:
    Settings(QString ini_path);

    bool hasKey(QString key);
    bool readBinary(QString key, QString s_true, QString s_default = "");
    bool readCreateBinary(QString key, QString s_true, QString s_false, QString s_default);
    QString getValue(QString key, QString default_value = "");
    QString getValueWrite(QString key, QString default_value = "");
    void setValue(QString key, QString value);
    void removeKey(QString key);

private:
    const QString _ini_path;
};

// Fuse checkbox to settings key value
class CheckBoxSetting:  public QObject
{
    Q_OBJECT
public:
    CheckBoxSetting(QCheckBox* checkbox, Settings* _settings, QString key);
    ~CheckBoxSetting();

public slots:
    void checkbox_changed(int state);

private:
    QCheckBox* _checkbox;
    Settings* _settings;
    QString _key;

};

#endif // SETTINGS_H