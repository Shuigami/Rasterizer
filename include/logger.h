#pragma once

#include <string>
#include <fstream>
#include <iostream>

enum class LogLevel {
    NONE = 0,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    VERBOSE
};

class Logger {
public:
    static Logger& getInstance();
    
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void setLevel(LogLevel level);
    
    LogLevel getLevel() const;
    
    bool enableFileOutput(const std::string& filename);
    void disableFileOutput();
    
    void error(const std::string& message);
    void warn(const std::string& message);
    void info(const std::string& message);
    void debug(const std::string& message);
    void verbose(const std::string& message);
    
private:
    Logger();
    ~Logger();
    
    void log(LogLevel level, const std::string& message);
    
    LogLevel m_level;
    
    std::ofstream m_fileStream;
    bool m_fileOutputEnabled;
    
    std::string levelToString(LogLevel level);
};

#define LOG_ERROR(msg) Logger::getInstance().error(msg)
#define LOG_WARN(msg) Logger::getInstance().warn(msg)
#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)
#define LOG_VERBOSE(msg) Logger::getInstance().verbose(msg)