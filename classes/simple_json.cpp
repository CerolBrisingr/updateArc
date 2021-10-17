#include "simple_json.h"

namespace SimpleJson{

QJsonObject toJson(QString json_data)
{
    QJsonDocument json_doc = QJsonDocument::fromJson(json_data.toUtf8());
    return json_doc.object();
}

}
