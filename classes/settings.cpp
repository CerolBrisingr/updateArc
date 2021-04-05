#include "settings.h"

Settings::Settings(QString ini_path)
    :_ini_path(ini_path)
{};

bool Settings::hasKey(QString key)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    return setting.contains(key);
}

bool Settings::readBinary(QString key, QString s_true, QString s_default)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    QString value = getValue(key, s_default);

    return (value.compare(s_true) == 0);
}

bool Settings::readCreateBinary(QString key, QString s_true, QString s_false, QString s_default)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    QString value = getValueWrite(key, s_default);
    if (value.compare(s_true) == 0) {
        return true;
    } else {
        // If string does not match "true", assume false and make sure the value is now valid
        setValue(key, s_false);
        return false;
    }
}

void Settings::setValue(QString key, QString value)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    setting.setValue(key, value);
}

QString Settings::getValueWrite(QString key, QString default_value)
{
    if (!hasKey(key)) {
        setValue(key, default_value);
    }

    QSettings setting(_ini_path, QSettings::IniFormat);
    return setting.value(key, default_value).toString();
}

QString Settings::getValue(QString key, QString default_value)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    return setting.value(key, default_value).toString();
}

void Settings::removeKey(QString key)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    setting.remove(key);
    return;
}

CheckBoxSetting::CheckBoxSetting(QCheckBox* checkbox, Settings* settings, QString key)
    :_checkbox(checkbox)
    ,_settings(settings)
    ,_key(key)
{
    // Read setting for checkbox, create if not available, set to "off" if undefined
    bool value = _settings->readCreateBinary(_key, "on", "off", "off");
    Qt::CheckState check_state = Qt::CheckState(value * 2);
    _checkbox->setCheckState(check_state);  // apply checkbox value

    // Connect changes to checkbox to update on settings
    connect(_checkbox, SIGNAL(stateChanged(int)),
            this, SLOT(checkbox_changed(int)));
}

CheckBoxSetting::~CheckBoxSetting()
{
}

void CheckBoxSetting::checkbox_changed(int state)
{
    // Checkbox changed, update corresponding value in settings
    if (state == 2) {
        _settings->setValue(_key, "on");
    } else {
        _settings->setValue(_key, "off");
    }
}

LineEditSettings::LineEditSettings(QLineEdit *lineedit, Settings *settings, QString key, QString default_entry)
    :_lineedit(lineedit)
    ,_settings(settings)
    ,_key(key)
{
    // Read setting for lineedit, create if not available
    QString text = _settings->getValueWrite(_key, default_entry);
    _lineedit->setText(text);

    // Connect changes on lineedit to update on settings
    connect(_lineedit, SIGNAL(textChanged(QString)),
            this, SLOT(lineedit_changed(QString)));
}

LineEditSettings::~LineEditSettings()
{
}

void LineEditSettings::lineedit_changed(QString text)
{
    _settings->setValue(_key, text);
}
