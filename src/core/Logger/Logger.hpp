#pragma once

#include <string>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

namespace nlm {

// Log level enumeration
enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// Log entry structure
struct LogEntry {
    LogLevel level = LogLevel::Info;
    std::string message;
    std::string file;
    int line = 0;
    std::string function;
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();
};

// ILogger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// ConsoleLogger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override = default;

    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();

    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Logger class with multiple loggers
class Logger {
public:
    Logger();
    ~Logger() = default;

    void addLogger(std::shared_ptr<ILogger> logger);
    void clearLoggers();
    void setLevel(LogLevel level);
    void log(LogLevel level, const std::string& message,
             const std::string& file = "", int line = 0,
             const std::string& function = "");
    void debug(const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    void info(const std::string& message,
              const std::string& file = "", int line = 0,
              const std::string& function = "");
    void warning(const std::string& message,
                 const std::string& file = "", int line = 0,
                 const std::string& function = "");
    void error(const std::string& message,
               const std::string& file = "", int line = 0,
               const std::string& function = "");
    void critical(const std::string& message,
                  const std::string& file = "", int line = 0,
                  const std::string& function = "");
    bool isEnabled(LogLevel level) const;

    static Logger& getGlobal();
    void setGlobal(std::shared_ptr<Logger> logger);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// LogStream for easy logging
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file,
              int line, const std::string& function);
    ~LogStream();

    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }

private:
    LogLevel level_;
    std::string file_;
    int line_;
    std::string function_;
    std::ostringstream oss_;
};

// Logging macros
#define NLM_LOG_DEBUG(msg) ::nlm::LogStream(::nlm::LogLevel::Debug, __FILE__, __LINE__, __func__) << msg
#define NLM_LOG_INFO(msg) ::nlm::LogStream(::nlm::LogLevel::Info, __FILE__, __LINE__, __func__) << msg
#define NLM_LOG_WARNING(msg) ::nlm::LogStream(::nlm::LogLevel::Warning, __FILE__, __LINE__, __func__) << msg
#define NLM_LOG_ERROR(msg) ::nlm::LogStream(::nlm::LogLevel::Error, __FILE__, __LINE__, __func__) << msg
#define NLM_LOG_CRITICAL(msg) ::nlm::LogStream(::nlm::LogLevel::Critical, __FILE__, __LINE__, __func__) << msg

} // namespace nlm
