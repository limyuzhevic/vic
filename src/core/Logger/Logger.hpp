#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <expected>
#include <compare>
#include <concepts>
#include <optional>
#include <span>

namespace nlm {

// C++20: Concepts for better template constraints
template<typename T>
concept StringLike = std::is_convertible_v<T, std::string>;

// C++20: Type aliases for common patterns
template<typename T>
using Result = std::expected<T, std::string>;

// C++20: Enum class with underlying type for explicit control
enum class LogLevel : uint8_t {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure with C++20 features
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // C++20: Three-way comparison for sorting
    auto operator<=>(const LogEntry&) const = default;
    bool operator==(const LogEntry&) const = default;
    
    // C++20: Structured binding helper
    [[nodiscard]] std::string formattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // C++20: Virtual destructor with noexcept
    virtual ~ILogger() noexcept = default;
};

// Console logger implementation with C++20 improvements
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    Result<void> setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // C++20: Attributes for better documentation
    [[nodiscard]] std::string levelToString(LogLevel level) const;
    [[nodiscard]] std::string levelToColor(LogLevel level) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main Logger class with C++20 features
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying, enable moving for efficiency
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) noexcept = default;
    Logger& operator=(Logger&&) noexcept = default;
    
    // Add logger sink with validation
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions with C++20 error handling
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods with C++20 attributes
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") noexcept;
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") noexcept;
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") noexcept;
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") noexcept;
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "") noexcept;
    
    // Check if level is enabled with validation
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger with error handling
    static Result<void> setGlobal(std::shared_ptr<Logger> logger);
    
    // C++20: Factory method for creating common loggers
    static std::unique_ptr<ConsoleLogger> createConsoleLogger(LogLevel level = LogLevel::Info);
    
    // C++20: Visitor pattern for logger operations
    template<typename Visitor>
    void visit(Visitor&& visitor) & {
        visitor.visitLoggers(*this);
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper with C++20 features
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<StringLike T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // C++20: Helper for numeric types
    LogStream& operator<<(float value) {
        oss_ << value;
        return *this;
    }
    
    LogStream& operator<<(int value) {
        oss_ << value;
        return *this;
    }
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    LogLevel originalLevel_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm
