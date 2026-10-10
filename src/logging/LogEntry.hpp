#pragma once

#include <string>
#include <chrono>
#include <map>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>

namespace nlm {

// Enhanced log severity levels
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};

// Structured log entry with metadata
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string threadId;
    std::map<std::string, std::string> metadata;
    uint64_t sequenceNumber;
    std::chrono::nanoseconds durationNs;
    
    LogEntry() : level(LogLevel::INFO), line(0), sequenceNumber(0), durationNs(0ns) {}
};

// Exception hierarchy for different error types
class LoggingException : public std::exception {
protected:
    std::string message_;
    LogLevel level_;
    
public:
    explicit LoggingException(const std::string& message, LogLevel level = LogLevel::ERROR)
        : message_(message), level_(level) {}
    
    virtual const char* what() const noexcept override { return message_.c_str(); }
    virtual LogLevel getLevel() const { return level_; }
    virtual const std::string& getMessage() const { return message_; }
};

class ConfigurationError : public LoggingException {
public:
    explicit ConfigurationError(const std::string& message) 
        : LoggingException(message, LogLevel::ERROR) {}
};

class LogFileError : public LoggingException {
public:
    explicit LogFileError(const std::string& message)
        : LoggingException(message, LogLevel::ERROR) {}
};

class NetworkError : public LoggingException {
public:
    explicit NetworkError(const std::string& message)
        : LoggingException(message, LogLevel::ERROR) {}
};

// Log destination interface
class ILogDestination {
public:
    virtual ~ILogDestination() = default;
    virtual void write(const LogEntry& entry) = 0;
    virtual void flush() = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    virtual bool isEnabled(LogLevel level) const = 0;
};

// Memory buffer for debugging
class MemoryBuffer {
public:
    virtual ~MemoryBuffer() = default;
    virtual void addEntry(const LogEntry& entry) = 0;
    virtual void clear() = 0;
    virtual size_t size() const = 0;
    virtual std::vector<LogEntry> getEntries() const = 0;
    virtual void setMaxEntries(size_t max) = 0;
};

// Database interface
class IDatabaseLogger {
public:
    virtual ~IDatabaseLogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual bool connect(const std::string& connectionString) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
};

// Network logger interface
class INetworkLogger {
public:
    virtual ~INetworkLogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual bool connect(const std::string& host, int port) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() const = 0;
    virtual void setProtocol(const std::string& protocol) = 0;
};

} // namespace nlm