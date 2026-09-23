#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>

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

// ComponentLogger - per-component logger with its own configuration
class ComponentLogger : public ILogger {
public:
    ComponentLogger(const std::string& componentName, const std::string& parentComponent = "");
    ComponentLogger(const std::string& componentName, const std::string& parentComponent, LogLevel level);
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Component-specific configuration
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set custom format string for this component
    void setFormat(const std::string& format);
    
    // Set minimum level for this component
    void setMinimumLevel(LogLevel level);
    
    // Get component name
    const std::string& getComponentName() const;
    const std::string& getParentComponent() const;
    
    // Add child component logger
    void addChildLogger(std::shared_ptr<ComponentLogger> childLogger);
    
    // Remove all child component loggers
    void clearChildLoggers();
    
    // Get all child component loggers
    const std::vector<std::shared_ptr<ComponentLogger>>& getChildLoggers() const;
    
    // Check if a component should log based on hierarchy
    bool shouldLog(const std::string& componentName) const;
    
    // Apply filtering rules
    void setFilter(const std::function<bool(const std::string&, const LogEntry&)>& filter);
    
    // Enable/disable this component's logging
    void setEnabled(bool enabled);
    bool isEnabled() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string formatEntry(const LogEntry& entry) const;
};

// HierarchicalLogger - manages component hierarchy and routing
class HierarchicalLogger : public ILogger {
public:
    HierarchicalLogger();
    ~HierarchicalLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Register a component logger
    void registerComponentLogger(const std::string& componentName, 
                                 const std::string& parentComponent = "",
                                 LogLevel level = LogLevel::Info);
    
    // Get or create a component logger
    std::shared_ptr<ComponentLogger> getComponentLogger(const std::string& componentName,
                                                         const std::string& parentComponent = "");
    
    // Get the root component logger
    std::shared_ptr<ComponentLogger> getRootComponentLogger();
    
    // Remove component logger
    void unregisterComponentLogger(const std::string& componentName);
    
    // Set global filtering rules
    void setGlobalFilter(const std::function<bool(const std::string&, const LogEntry&)>& filter);
    
    // Set minimum level for all components
    void setGlobalLevel(LogLevel level);
    
    // Enable/disable component logging by name
    void setComponentEnabled(const std::string& componentName, bool enabled);
    bool isComponentEnabled(const std::string& componentName) const;
    
    // Check if log should be processed based on global filters
    bool shouldLog(const std::string& componentName, const LogEntry& entry) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
