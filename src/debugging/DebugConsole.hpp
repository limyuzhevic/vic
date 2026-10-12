#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <chrono>
#include <map>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <thread>
#include "../core/Logger.hpp"

namespace nlm {

// Console command definition
struct ConsoleCommand {
    std::string name;
    std::string description;
    std::string usage;
    std::function<void(const std::vector<std::string>&)> handler;
    bool enabled;
    std::string category; // "system", "visualization", "brain", "memory", "debug"
    bool isHidden; // Hidden from help
};

// Log message for console
struct ConsoleLog {
    Timestamp timestamp;
    std::string level; // "DEBUG", "INFO", "WARNING", "ERROR"
    std::string source; // Module or component name
    std::string message;
    std::vector<std::string> tags;
    bool isSystemLog; // From system, not user
};

// System metrics for console display
struct ConsoleMetrics {
    Timestamp timestamp;
    double cpuUsage; // Percentage
    double memoryUsage; // Percentage
    size_t activeConnections;
    size_t totalMemoryAllocated;
    float fps; // Frames per second
    double systemUptime; // seconds
    std::map<std::string, double> componentMetrics;
};

// Runtime parameter configuration
struct RuntimeParameter {
    std::string name;
    std::string type; // "int", "float", "string", "bool"
    std::string value;
    std::string description;
    std::string category;
    bool isReadOnly;
    std::vector<std::string> allowedValues;
    std::function<std::string()> getDisplayValue;
    std::function<void(const std::string&)> setValue;
};

// Console input line
struct ConsoleInput {
    std::string text;
    Timestamp timestamp;
    std::string source; // "user", "system", "plugin"
};

// Console output line
struct ConsoleOutput {
    std::string text;
    std::string level; // "INFO", "WARNING", "ERROR", "SUCCESS"
    Timestamp timestamp;
    bool isSystem; // From system, not user
};

class DebugConsole {
public:
    DebugConsole();
    ~DebugConsole();
    
    // Initialize console
    bool initialize();
    
    // Console input/output
    void processInput(const std::string& input);
    void processCommand(const std::string& commandLine);
    void output(const std::string& message, const std::string& level = "INFO", bool isSystem = false);
    
    // Command management
    void registerCommand(const ConsoleCommand& command);
    void unregisterCommand(const std::string& name);
    void enableCommand(const std::string& name);
    void disableCommand(const std::string& name);
    ConsoleCommand* getCommand(const std::string& name);
    std::vector<ConsoleCommand> getCommands(const std::string& category = "") const;
    std::vector<std::string> getCommandCategories() const;
    
    // Command execution
    bool executeCommand(const std::string& name, const std::vector<std::string>& args);
    bool executeCommand(const ConsoleCommand& command, const std::vector<std::string>& args);
    
    // Command history and completion
    void addToHistory(const std::string& command);
    std::vector<std::string> getCommandHistory(size_t maxHistory = 100) const;
    std::vector<std::string> getCommandSuggestions(const std::string& partialCommand) const;
    
    // Runtime parameter management
    void registerParameter(const RuntimeParameter& parameter);
    void unregisterParameter(const std::string& name);
    void setParameter(const std::string& name, const std::string& value);
    std::string getParameter(const std::string& name) const;
    std::vector<RuntimeParameter> getParameters(const std::string& category = "") const;
    std::vector<std::string> getParameterCategories() const;
    
    // Log management
    void addLog(const ConsoleLog& log);
    void addSystemLog(const std::string& level, const std::string& source, const std::string& message);
    std::vector<ConsoleLog> getRecentLogs(size_t maxLogs = 1000) const;
    std::vector<ConsoleLog> getLogsSince(Timestamp timestamp) const;
    void clearLogs();
    void filterLogs(const std::string& level, const std::string& source);
    
    // Metrics
    void updateMetrics(const ConsoleMetrics& metrics);
    ConsoleMetrics getCurrentMetrics() const;
    std::string getMetricsSummary() const;
    
    // Console control
    void startConsole();
    void stopConsole();
    bool isConsoleActive() const;
    void setConsoleActive(bool active);
    
    // Integration
    void attachToVisualization(class AdvancedVisualization* visualization);
    void attachToProfiler(class Profiler* profiler);
    
    // Output streaming
    void startOutputStream(const std::string& streamName, const std::string& format);
    void stopOutputStream(const std::string& streamName);
    bool isOutputStreamActive(const std::string& streamName) const;
    
    // Export capabilities
    bool exportCommandHistory(const std::string& filename);
    bool exportLogHistory(const std::string& filename);
    bool exportParameterHistory(const std::string& filename);
    
    // System status
    std::string getSystemStatus() const;
    std::string getConsoleStatus() const;
    size_t getActiveCommandCount() const;
    size_t getActiveParameterCount() const;
    size_t getLogCount() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm