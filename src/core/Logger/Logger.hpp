#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>

namespace nlm {

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    std::string component;  // Component name for hierarchical logging
    std::string parentComponent; // Parent component name for hierarchical logging
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
};

// Console logger implementation
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
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
};

// Forward declaration
class ComponentLogger;

// HierarchicalLogger class for managing component hierarchy
class HierarchicalLogger;

// Main Logger class - now uses hierarchical logging system
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink (for backward compatibility)
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers (backward compatibility)
    void setLevel(LogLevel level);
    
    // Global logging functions with component support
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "",
             const std::string& component = "",
             const std::string& parentComponent = "");
    
    // Convenience methods with component support
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
               const std::string& component = "", const std::string& parentComponent = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
              const std::string& component = "", const std::string& parentComponent = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                 const std::string& component = "", const std::string& parentComponent = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
               const std::string& component = "", const std::string& parentComponent = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "",
                  const std::string& component = "", const std::string& parentComponent = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // New component-specific logging methods
    void enableComponent(const std::string& componentName, bool enabled = true);
    
    // Set component-specific level
    void setComponentLevel(const std::string& componentName, LogLevel level);
    
    // Set component-specific filter
    void setComponentFilter(const std::string& componentName, 
                           const std::function<bool(const std::string&, const LogEntry&)>& filter);
    
    // Register a component logger
    void registerComponentLogger(const std::string& componentName,
                                 const std::string& parentComponent = "",
                                 LogLevel level = LogLevel::Info);
    
    // Get or create a component logger
    std::shared_ptr<ComponentLogger> getComponentLogger(const std::string& componentName,
                                                         const std::string& parentComponent = "");
    
    // Get hierarchical logger instance
    std::shared_ptr<HierarchicalLogger> getHierarchicalLogger() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info (backward compatibility)
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, "")
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, "")
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, "")
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, "")
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__, "")

// Component-specific macros
#define NLM_LOG_COMPONENT_DEBUG(component, msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__, component)
#define NLM_LOG_COMPONENT_INFO(component, msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__, component)
#define NLM_LOG_COMPONENT_WARNING(component, msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__, component)
#define NLM_LOG_COMPONENT_ERROR(component, msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__, component)
#define NLM_LOG_COMPONENT_CRITICAL(component, msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__, component)

// Stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Set component for this stream
    void setComponent(const std::string& component);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
    std::string component_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

// Hierarchical logging macros
#define NLM_LOG_HIERARCHICAL(component, level, msg) nlm::Logger::getGlobal().log(level, msg, __FILE__, __LINE__, __func__, component, "")

} // namespace nlm
