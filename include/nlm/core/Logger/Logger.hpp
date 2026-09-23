#ifndef NLM_CORE_LOGGER_LOGGER_HPP
#define NLM_CORE_LOGGER_LOGGER_HPP

#include <string>
#include <memory>
#include <vector>
#include <mutex>

namespace nlm {

enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};

class LogMessage {
public:
    LogLevel level;
    std::string message;
    std::string timestamp;
    std::string file;
    int line;
    
    LogMessage(LogLevel lvl, const std::string& msg, const std::string& file = "", int line = 0);
};

class Logger;
using LoggerPtr = std::shared_ptr<Logger>;

class Logger {
public:
    static LoggerPtr getInstance();
    static void setGlobal(LoggerPtr logger);
    
    Logger();
    ~Logger();
    
    void setLevel(LogLevel level);
    LogLevel getLevel() const;
    
    void addLogger(LoggerPtr logger);
    void removeLogger(LoggerPtr logger);
    
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0);
    
    void info(const std::string& message, const std::string& file = "", int line = 0);
    void debug(const std::string& message, const std::string& file = "", int line = 0);
    void warn(const std::string& message, const std::string& file = "", int line = 0);
    void error(const std::string& message, const std::string& file = "", int line = 0);
    void critical(const std::string& message, const std::string& file = "", int line = 0);
    
    // Convenience macros (would be in a separate header)
    void trace(const std::string& message, const std::string& file = "", int line = 0);

protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class ConsoleLogger : public Logger {
public:
    ConsoleLogger(LogLevel level = LogLevel::Info);
    ~ConsoleLogger();
    
    void log(const LogMessage& message) override;
    
    void setFormat(const std::string& format);
    void setColorOutput(bool enable);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global convenience functions
inline void NLM_LOG_TRACE(const std::string& msg) {
    Logger::getInstance()->trace(msg);
}

inline void NLM_LOG_DEBUG(const std::string& msg) {
    Logger::getInstance()->debug(msg);
}

inline void NLM_LOG_INFO(const std::string& msg) {
    Logger::getInstance()->info(msg);
}

inline void NLM_LOG_WARN(const std::string& msg) {
    Logger::getInstance()->warn(msg);
}

inline void NLM_LOG_ERROR(const std::string& msg) {
    Logger::getInstance()->error(msg);
}

inline void NLM_LOG_CRITICAL(const std::string& msg) {
    Logger::getInstance()->critical(msg);
}

} // namespace nlm

#endif // NLM_CORE_LOGGER_LOGGER_HPP
