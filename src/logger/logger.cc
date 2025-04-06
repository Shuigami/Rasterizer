#include "logger/logger.hh"

#include <iostream>
#include <fstream>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(Level level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ofstream logFile("log.txt", std::ios::app);

    std::string levelStr = levelToString(level);
    std::string logMessage = getCurrentTime() + " [" + levelStr + "] " + message;

    if (logFile) {
        logFile << logMessage << std::endl;
    }

    std::cout << logMessage << std::endl;
}

std::string Logger::levelToString(Level level) {
    switch (level) {
        case Level::INFO: return "INFO";
        case Level::WARNING: return "WARNING";
        case Level::ERROR: return "ERROR";
        case Level::DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

std::string Logger::getCurrentTime() {
    std::time_t now = std::time(nullptr);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
}
