// Logging system for NLM
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <ctime>

namespace nlm {

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

class Logger {
public:
    Logger();
    virtual ~Logger() = default;
    
    virtual void log(LogLevel level, const std::string& message);
    void setLevel(LogLevel level);
    LogLevel getLevel() const;
    
    static Logger& getGlobalLogger();
    static void setGlobalLogger(Logger* logger);
    static std::string levelToString(LogLevel level);
    
protected:
    LogLevel level_;
    std::mutex mutex_;
};

class ConsoleLogger : public Logger {
public:
    ConsoleLogger(LogLevel level = LogLevel::Info);
    ~ConsoleLogger() override = default;
    
    void log(LogLevel level, const std::string& message) override;
    
private:
    void printWithTimestamp(const std::string& message);
    std::string getCurrentTimestamp() const;
};

class FileLogger : public Logger {
public:
    FileLogger(const std::string& filename, LogLevel level = LogLevel::Info);
    ~FileLogger() override;
    
    void log(LogLevel level, const std::string& message) override;
    
    bool isOpen() const;
    
private:
    std::string filename_;
    std::ofstream file_;
};

// Convenience macros for logging
#define NLM_LOG_TRACE(msg) Logger::getGlobalLogger().log(LogLevel::Trace, msg)
#define NLM_LOG_DEBUG(msg) Logger::getGlobalLogger().log(LogLevel::Debug, msg)
#define NLM_LOG_INFO(msg) Logger::getGlobalLogger().log(LogLevel::Info, msg)
#define NLM_LOG_WARNING(msg) Logger::getGlobalLogger().log(LogLevel::Warning, msg)
#define NLM_LOG_ERROR(msg) Logger::getGlobalLogger().log(LogLevel::Error, msg)
#define NLM_LOG_CRITICAL(msg) Logger::getGlobalLogger().log(LogLevel::Critical, msg)

} // namespace nlm