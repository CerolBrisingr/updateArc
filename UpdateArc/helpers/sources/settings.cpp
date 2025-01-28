#include "settings.h"

Settings::Settings(const QString ini_path)
    :_ini_path(ini_path)
{};

bool Settings::hasKey(const QString key) const
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    return setting.contains(key);
}

bool Settings::readBinary(const QString key, const QString s_true, const QString s_default) const
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    QString value = getValue(key, s_default);

    return (value.compare(s_true) == 0);
}

bool Settings::readCreateBinary(const QString key, const QString s_true, const QString s_false, const QString s_default)
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

void Settings::setValue(const QString key, const QString value)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    setting.setValue(key, value);
}

QString Settings::getValueWrite(const QString key, const QString default_value)
{
    if (!hasKey(key)) {
        setValue(key, default_value);
    }

    QSettings setting(_ini_path, QSettings::IniFormat);
    return setting.value(key, default_value).toString();
}

QString Settings::getValue(const QString key, const QString default_value) const
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    return setting.value(key, default_value).toString();
}

void Settings::removeKey(const QString key)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    setting.remove(key);
    return;
}

CheckBoxSetting::CheckBoxSetting(QCheckBox *checkbox, const QString key, const QString iniPath)
    :_checkbox(checkbox)
    ,_settings(iniPath)
    ,_key(key)
{
    // Read setting for checkbox, create if not available, set to "off" if undefined
    bool value = _settings.readCreateBinary(_key, "on", "off", "off");
    Qt::CheckState check_state = Qt::CheckState(value * 2);
    _checkbox->setCheckState(check_state);  // apply checkbox value

    // Connect changes to checkbox to update on settings
    connect(_checkbox, &QCheckBox::checkStateChanged,
            this, &CheckBoxSetting::checkboxChanged);
}

bool CheckBoxSetting::getState() const
{
    return _settings.readBinary(_key, "on", "off");
}

void CheckBoxSetting::checkboxChanged(Qt::CheckState state)
{
    // Checkbox changed, update corresponding value in settings
    switch(state) {
    case Qt::Unchecked:
        _settings.setValue(_key, "off");
        break;
    case Qt::Checked:
        _settings.setValue(_key, "on");
        break;
    default:
        // If it's not "on", it's "off"
        _settings.setValue(_key, "off");
        break;
    }
}

LineEditSettings::LineEditSettings(QLineEdit *lineedit, const QString key, const QString default_entry, const QString iniPath)
    :_lineedit(lineedit)
    ,_settings(iniPath)
    ,_key(key)
    ,_defaultEntry(default_entry)
{
    // Read setting for lineedit, create if not available
    QString text = _settings.getValueWrite(_key, _defaultEntry);
    _lineedit->setText(text);

    // Connect changes on lineedit to update on settings
    connect(_lineedit, &QLineEdit::textChanged,
            this, &LineEditSettings::lineeditChanged);
}

QString LineEditSettings::getValue() const
{
    return _settings.getValue(_key, _defaultEntry);
}

void LineEditSettings::lineeditChanged(const QString &text)
{
    _settings.setValue(_key, text);
}
