#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>

namespace nlm {

// Enhanced logging levels
enum class LogLevel {
    Quiet = 0,      // No output
    Error = 1,      // Error messages only
    Warning = 2,    // Warnings and errors
    Info = 3,       // Information (default)
    Debug = 4,      // Debug information
    Verbose = 5     // Detailed output
};

// Output formatter for consistent styling
class OutputFormatter {
public:
    static void printBanner(const std::string& title, const std::string& subtitle = "");
    static void printSection(const std::string& title);
    static void printSubsection(const std::string& title);
    static void printStatus(const std::string& label, const std::string& value);
    static void printSuccess(const std::string& message);
    static void printError(const std::string& message);
    static void printWarning(const std::string& message);
    static void printProgress(const std::string& message);
    static void printDivider();
    static void printTimestamped(const std::string& message);
    static void printFormatted(const std::string& format, ...);
    static void printProgressBar(int current, int total, const std::string& label = "");
};

// Progress indicator for long-running operations
class ProgressIndicator {
public:
    ProgressIndicator(const std::string& label = "Processing", int total = 100);
    ~ProgressIndicator();
    
    void update(int current, const std::string& message = "");
    void increment();
    void setLabel(const std::string& label);
    void complete();
    
private:
    std::string label;
    int total;
    int current;
    std::chrono::steady_clock::time_point startTime;
    bool active;
    void drawProgressBar();
};

// Enhanced console logger with formatting
class ConsoleLogger {
public:
    explicit ConsoleLogger(LogLevel level = LogLevel::Info);
    virtual ~ConsoleLogger();
    
    virtual void log(LogLevel level, const std::string& message);
    virtual LogLevel getLevel() const;
    virtual void setLevel(LogLevel level);
    
protected:
    LogLevel level;
    virtual std::string formatMessage(LogLevel level, const std::string& message) const;
};

// Multi-output logger (file + console)
class MultiLogger {
public:
    MultiLogger();
    ~MultiLogger();
    
    void addLogger(std::shared_ptr<ConsoleLogger> logger);
    void log(LogLevel level, const std::string& message);
    void setGlobalLevel(LogLevel level);
    
private:
    std::vector<std::shared_ptr<ConsoleLogger>> loggers;
    LogLevel globalLevel;
};

// Performance timer for benchmarking
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& label);
    ~PerformanceTimer();
    
    void start();
    void stop();
    double getElapsedSeconds() const;
    double getElapsedMilliseconds() const;
    const std::string& getLabel() const;
    
private:
    std::string label;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    bool running;
};

// Result summary for operations
struct OperationResult {
    bool success;
    std::string message;
    std::chrono::duration<double> elapsedTime;
    std::string details;
    
    OperationResult(bool success, const std::string& message);
    void printSummary() const;
};

// Measurement utilities
namespace Metrics {
    std::string formatBytes(size_t bytes);
    std::string formatDuration(double seconds);
    std::string formatTime(double seconds);
    std::string capitalize(const std::string& str);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    bool startsWith(const std::string& str, const std::string& prefix);
    bool endsWith(const std::string& str, const std::string& suffix);
}

} // namespace nlm