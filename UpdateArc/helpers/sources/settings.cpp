#include "settings.h"

Settings::Settings(const QString ini_path)
    :_ini_path(ini_path)
{};

// Basic key operations
bool Settings::hasKey(const QString key) const
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    return setting.contains(key);
}

void Settings::removeKey(const QString key)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    setting.remove(key);
    return;
}

// Working with string values
void Settings::writeValue(const QString key, const QString value)
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    setting.setValue(key, value);
}

QString Settings::readValue(const QString key, const QString default_value) const
{
    QSettings setting(_ini_path, QSettings::IniFormat);
    return setting.value(key, default_value).toString();
}

QString Settings::readCreateValue(const QString key, const QString default_value)
{
    if (!hasKey(key)) {
        writeValue(key, default_value);
    }

    QSettings setting(_ini_path, QSettings::IniFormat);
    return setting.value(key, default_value).toString();
}

// Simulate binary values
void Settings::writeBinary(const QString key, const bool value, const QString s_true, const QString s_false)
{
    if (value) {
        writeValue(key, s_true);
    } else {
        writeValue(key, s_false);
    }
}

bool Settings::readBinary(const QString key, const QString s_default, const QString s_true) const
{
    QString value = readValue(key, s_default);
    return (value.compare(s_true) == 0);
}

bool Settings::readCreateBinary(const QString key, const bool defaultValue, const QString s_true, const QString s_false)
{
    QString value;
    if (defaultValue) {
        value = readCreateValue(key, s_true);
    } else {
        value = readCreateValue(key, s_false);
    }
    if (value.compare(s_true) == 0) {
        return true;
    } else {
        // If string does not match "true", assume false and make sure the value is now valid
        writeValue(key, s_false);
        return false;
    }
}

CheckBoxSettings::CheckBoxSettings(QCheckBox *checkbox, const QString key, const QString iniPath)
    :_settings(iniPath)
    ,_key(key)
{
    if (checkbox == nullptr) return;

    // Read setting for checkbox, create if not available, set to "off" if undefined
    bool value = _settings.readCreateBinary(_key);

    // No support for tristate (half checked) boxes
    checkbox->setTristate(false);
    const Qt::CheckState check_state = Qt::CheckState(value * 2);
    checkbox->setCheckState(check_state);  // apply checkbox value

    // Connect changes to checkbox to update on settings
    connect(checkbox, &QCheckBox::checkStateChanged,
            this, &CheckBoxSettings::checkboxChanged);
}

bool CheckBoxSettings::getSettingState() const
{
    return _settings.readBinary(_key);
}

void CheckBoxSettings::checkboxChanged(Qt::CheckState state)
{
    // Checkbox changed, update corresponding value in settings
    switch(state) {
    case Qt::Unchecked:
        _settings.writeValue(_key, "off");
        break;
    case Qt::Checked:
        _settings.writeValue(_key, "on");
        break;
    default:
        // If it's not "on", it's "off"
        _settings.writeValue(_key, "off");
        break;
    }
}

LineEditSettings::LineEditSettings(QLineEdit *lineedit, const QString key, const QString default_entry, const QString iniPath)
    :_settings(iniPath)
    ,_key(key)
    ,_defaultEntry(default_entry)
{
    if (lineedit == nullptr) return;

    // Read setting for lineedit, create if not available
    const QString text = _settings.readCreateValue(_key, _defaultEntry);
    lineedit->setText(text);

    // Connect changes on lineedit to update on settings
    connect(lineedit, &QLineEdit::textChanged,
            this, &LineEditSettings::lineeditChanged);

}

QString LineEditSettings::getValue() const
{
    return _settings.readValue(_key, _defaultEntry);
}

void LineEditSettings::lineeditChanged(const QString &text)
{
    _settings.writeValue(_key, text);
}
