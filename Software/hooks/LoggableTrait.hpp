#ifndef LOGGABLETRAIT_HPP
#define LOGGABLETRAIT_HPP

#include "Logger.hpp"

class Logger;

class LoggableTrait {
    public:
    LoggableTrait(Logger *logger): m_logger(logger) {}

protected:
    Logger *m_logger;
};

#endif // LOGGABLETRAIT_HPP
