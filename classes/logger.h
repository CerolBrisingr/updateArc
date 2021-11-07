#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QTextStream>
#include <QObject>

namespace Log {

class Logger;

extern Logger writer;

extern void write(QString line);
extern void write(std::string line);
extern void write(const char* line);
extern void write(QTextStream stream);


class Logger: public QObject
{
    Q_OBJECT
public:
    Logger();

    void emitString(QString text);

signals:
    void sendText(QString text);

private:
};


} // namespace Log

#endif // LOGGER_H
