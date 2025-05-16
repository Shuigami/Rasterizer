#include "logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>

Logger::Logger() : m_level(LogLevel::INFO), m_fileOutputEnabled(false) {
}

Logger::~Logger() {
    if (m_fileOutputEnabled) {
        disableFileOutput();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLevel(LogLevel level) {
    m_level = level;
}

LogLevel Logger::getLevel() const {
    return m_level;
}

bool Logger::enableFileOutput(const std::string& filename) {
    if (m_fileOutputEnabled) {
        disableFileOutput();
    }
    
    m_fileStream.open(filename, std::ios::out | std::ios::app);
    if (m_fileStream.is_open()) {
        m_fileOutputEnabled = true;
        return true;
    }
    return false;
}

void Logger::disableFileOutput() {
    if (m_fileOutputEnabled && m_fileStream.is_open()) {
        m_fileStream.close();
    }
    m_fileOutputEnabled = false;
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::warn(const std::string& message) {
    log(LogLevel::WARN, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::verbose(const std::string& message) {
    log(LogLevel::VERBOSE, message);
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level <= m_level && level != LogLevel::NONE) {
        auto now = std::chrono::system_clock::now();
        auto nowTime = std::chrono::system_clock::to_time_t(now);
        auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count() % 1000;
            
        std::stringstream timeStr;
        timeStr << std::put_time(std::localtime(&nowTime), "%Y-%m-%d %H:%M:%S")
                << '.' << std::setfill('0') << std::setw(3) << nowMs;
                
        std::string levelStr = levelToString(level);
        
        std::stringstream fullMessage;
        fullMessage << '[' << timeStr.str() << "] [" << levelStr << "]: " << message;
        
        std::cout << fullMessage.str() << std::endl;
        
        // Output to file if enabled
        if (m_fileOutputEnabled && m_fileStream.is_open()) {
            m_fileStream << fullMessage.str() << std::endl;
        }
    }
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::WARN:    return "WARN ";
        case LogLevel::INFO:    return "INFO ";
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::VERBOSE: return "VERB ";
        default:                return "NONE ";
    }
}