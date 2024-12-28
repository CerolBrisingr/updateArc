#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QCheckBox>
#include <QLineEdit>

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
    CheckBoxSetting(QCheckBox* checkbox, QString key);

private slots:
    void checkboxChanged(int state);

private:
    QCheckBox* _checkbox;
    Settings _settings;
    QString _key;

};

// Fuse line edit to settings key value
class LineEditSettings: public QObject
{
    Q_OBJECT
public:
    LineEditSettings(QLineEdit* lineedit, QString key, QString default_entry = "");

private slots:
    void lineeditChanged(QString text);

private:
    QLineEdit* _lineedit;
    Settings _settings;
    QString _key;
};

#endif // SETTINGS_H
