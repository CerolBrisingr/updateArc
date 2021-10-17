#include "logger.h"
#include <iostream>

namespace Log {

Logger writer;

void write(QString line)
{
    writer.emitString(line);
}

void write(std::string line)
{
    writer.emitString(QString::fromStdString(line));
}

void write(const char* line)
{
    writer.emitString(line);
}

void write(QTextStream stream)
{
    writer.emitString(stream.readAll());
}

/// LOGGER -------------------------

Logger::Logger()
{
    qRegisterMetaType<QString>("QString");
}

Logger::~Logger()
{
}

void Logger::emitString(QString text)
{
    std::cout << text.toStdString();
    emit sendText(text);
}



} // namespace Log
