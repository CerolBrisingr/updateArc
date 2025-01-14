#ifndef SIMPLE_JSON_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>

#define SIMPLE_JSON_H


namespace SimpleJson
{

QJsonDocument toJsonDoc(QString &json_data);
QJsonObject toJsonObject(QString &json_data);
QJsonArray toJsonArray(QString &json_data);

bool targetValid(QJsonArray& array, int index);
bool targetValid(QJsonObject& object, QString key);

template <typename T, typename I>
QJsonObject getObject(T json_element, I specifier)
{
    if (targetValid(json_element, specifier)) {
        return json_element[specifier].toObject();
    }
    return QJsonObject();
};

template <typename T, typename I>
QJsonArray getArray(T json_element, I specifier)
{
    if (targetValid(json_element, specifier)) {
        return json_element[specifier].toArray();
    }
    return QJsonArray();
};

template <typename T, typename I>
bool getBool(T json_element, I specifier, bool default_value = false)
{
    if (targetValid(json_element, specifier)) {
        return json_element[specifier].toBool(default_value);
    }
    return default_value;
};

template <typename T, typename I>
double getDouble(T json_element, I specifier, double default_value = 0)
{
    if (targetValid(json_element, specifier)) {
        return json_element[specifier].toDouble(default_value);
    }
    return default_value;
};

template <typename T, typename I>
int getInt(T json_element, I specifier, int default_value = 0)
{
    if (targetValid(json_element, specifier)) {
        return json_element[specifier].toInt(default_value);
    }
    return default_value;
};

template <typename T, typename I>
QString getString(T json_element, I specifier, QString default_value = "")
{
    if (targetValid(json_element, specifier)) {
        return json_element[specifier].toString(default_value);
    }
    return default_value;
};

template <typename T, typename I>
QVariant getVariant(T json_element, I specifier)
{
    if (targetValid(json_element, specifier)) {
        return json_element[specifier].toVariant();
    }
    return QVariant();
};

};

#endif // SIMPLE_JSON_H
