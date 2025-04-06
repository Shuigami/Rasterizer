#pragma once

#include <string>
#include <mutex>
#include <ctime>

class Logger {
public:
    enum class Level { INFO, WARNING, ERROR, DEBUG };

    static Logger& getInstance();

    void log(Level level, const std::string& message);

private:
    std::mutex mutex_;

    Logger() {}
    ~Logger() {}

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::string levelToString(Level level);

    std::string getCurrentTime();
};
