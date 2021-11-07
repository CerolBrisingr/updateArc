#include "simple_json.h"

namespace SimpleJson{

QJsonDocument toJsonDoc(QString& json_data) {
    return QJsonDocument::fromJson(json_data.toUtf8());
}

QJsonObject toJsonObject(QString& json_data){
    return toJsonDoc(json_data).object();
}

QJsonArray toJsonArray(QString& json_data) {
    return toJsonDoc(json_data).array();
}

bool targetValid(QJsonArray& array, int index)
{
    return (index >= 0 && index < array.size());
}

bool targetValid(QJsonObject& object, QString key)
{
    return object.contains(key);
}

}
