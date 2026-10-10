// Logger system implementation for NLM
#include "Logger.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <fstream>

namespace nlm {

Logger::Logger() : level_(LogLevel::Info) {
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < level_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    std::string formatted = "[" + levelToString(level) + "] " + message;
    doLog(level, formatted);
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    level_ = level;
}

LogLevel Logger::getLevel() const {
    return level_;
}

Logger& Logger::getGlobalLogger() {
    static ConsoleLogger globalLogger;
    return globalLogger;
}

void Logger::setGlobalLogger(Logger* logger) {
    // This is a simple implementation - in a real system we'd use thread-local storage
    static Logger* global = nullptr;
    global = logger;
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return "TRACE";
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
        default: return "UNKNOWN";
    }
}

ConsoleLogger::ConsoleLogger(LogLevel level) : Logger(level) {
}

void ConsoleLogger::log(LogLevel level, const std::string& message) {
    Logger::log(level, message);
    printWithTimestamp(message);
}

void ConsoleLogger::printWithTimestamp(const std::string& message) {
    std::string timestamp = getCurrentTimestamp();
    std::cout << "[" << timestamp << "] " << message << std::endl;
}

std::string ConsoleLogger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

FileLogger::FileLogger(const std::string& filename, LogLevel level) : Logger(level) {
    file_.open(filename, std::ios::app);
}

FileLogger::~FileLogger() {
    if (file_.is_open()) {
        file_.close();
    }
}

void FileLogger::log(LogLevel level, const std::string& message) {
    Logger::log(level, message);
    
    if (file_.is_open()) {
        file_ << "[" << levelToString(level) << "] " << message << std::endl;
    }
}

bool FileLogger::isOpen() const {
    return file_.is_open();
}

} // namespace nlm