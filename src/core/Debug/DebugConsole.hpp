// Copyright (c) 2026 NLM Project
// Debug console for real-time system monitoring

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <mutex>
#include <atomic>

namespace nlm {

class IDebugConsole {
public:
    virtual ~IDebugConsole() = default;
    virtual void update() = 0;
    virtual void render() = 0;
    virtual void logMessage(const std::string& message, LogLevel level = LogLevel::Info) = 0;
    virtual void addSystemStatus(const std::string& name, const std::string& value, const std::string& status = "ok") = 0;
    virtual void addPerformanceMetric(const std::string& name, double value, const std::string& unit = "ms") = 0;
    virtual void addMemoryUsage(uint64_t used, uint64_t total) = 0;
    virtual void setMemoryUsage(double percent) = 0;
    virtual void setCpuUsage(double percent) = 0;
    virtual void setActiveThreads(int count) = 0;
    virtual void setPerformanceMode(bool enabled) = 0;
    virtual void setDebugMode(bool enabled) = 0;
    virtual void clear() = 0;
    virtual std::string getLastOutput() const = 0;
    virtual bool isVisible() const = 0;
    virtual void toggleVisibility() = 0;
};

class SystemStatus {
public:
    std::string component;
    std::string status; // "ok", "warning", "error", "critical"
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    std::string value;
    std::string unit;
    
    SystemStatus(const std::string& comp, const std::string& stat, const std::string& msg)
        : component(comp), status(stat), message(msg), timestamp(std::chrono::system_clock::now()) {}
};

class PerformanceMetric {
public:
    std::string name;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
    double threshold; // Alert threshold
    bool exceeded = false;
    
    PerformanceMetric(const std::string& n, double v, const std::string& u = "ms", double thresh = 0.0)
        : name(n), value(v), unit(u), timestamp(std::chrono::system_clock::now()), threshold(thresh) {}
};

class MemoryInfo {
public:
    uint64_t used = 0;
    uint64_t total = 0;
    double percent = 0.0;
    uint64_t peak = 0;
    std::chrono::system_clock::time_point timestamp;
    
    MemoryInfo() : timestamp(std::chrono::system_clock::now()) {}
};

class CpuInfo {
public:
    double usage = 0.0;
    int coreCount = 0;
    std::vector<double> coreUsages;
    std::chrono::system_clock::time_point timestamp;
    
    CpuInfo() : timestamp(std::chrono::system_clock::now()) {}
};

class ThreadInfo {
public:
    uint64_t threadId;
    std::string name;
    std::string state; // "running", "waiting", "idle"
    std::chrono::system_clock::time_point timestamp;
    
    ThreadInfo(uint64_t id, const std::string& n, const std::string& s)
        : threadId(id), name(n), state(s), timestamp(std::chrono::system_clock::now()) {}
};

class DebugConsole {
public:
    DebugConsole();
    ~DebugConsole();
    
    void update();
    void render();
    void logMessage(const std::string& message, LogLevel level = LogLevel::Info) override;
    void addSystemStatus(const std::string& name, const std::string& value, const std::string& status = "ok") override;
    void addPerformanceMetric(const std::string& name, double value, const std::string& unit = "ms") override;
    void addMemoryUsage(uint64_t used, uint64_t total) override;
    void setMemoryUsage(double percent) override;
    void setCpuUsage(double percent) override;
    void setActiveThreads(int count) override;
    void setPerformanceMode(bool enabled) override;
    void setDebugMode(bool enabled) override;
    void clear() override;
    std::string getLastOutput() const override;
    bool isVisible() const override;
    void toggleVisibility() override;
    
    // Console control
    void show();
    void hide();
    void minimize();
    void maximize();
    
    // System monitoring integration
    void updateSystemInfo();
    void updateThreadInfo();
    void updatePerformanceMetrics();
    
    // Log filtering and display
    void setLogLevelFilter(LogLevel level);
    void setModuleFilter(const std::string& module);
    void addCustomFilter(const std::string& pattern);
    void clearFilters();
    
    // Export functionality
    void exportLogs(const std::string& filepath);
    void exportPerformanceData(const std::string& filepath);
    void exportSystemInfo(const std::string& filepath);
    
    // Integration points
    void integrateWithLogger(std::shared_ptr<LoggerManager> loggerManager);
    void integrateWithPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler);
    void integrateWithMemoryTracker(std::shared_ptr<MemoryTracker> tracker);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void updateDisplay();
    void renderLogWindow();
    void renderSystemInfoWindow();
    void renderPerformanceWindow();
    void renderMemoryWindow();
    void renderThreadWindow();
    void renderSettingsWindow();
    
    std::string formatTime(const std::chrono::system_clock::time_point& time) const;
    std::string formatMemory(uint64_t bytes) const;
    std::string formatPercentage(double percent) const;
    std::string getStatusColor(const std::string& status) const;
};

// Simple console debug implementation
class SimpleDebugConsole : public IDebugConsole {
public:
    SimpleDebugConsole();
    ~SimpleDebugConsole() override;
    
    void update() override;
    void render() override;
    void logMessage(const std::string& message, LogLevel level = LogLevel::Info) override;
    void addSystemStatus(const std::string& name, const std::string& value, const std::string& status = "ok") override;
    void addPerformanceMetric(const std::string& name, double value, const std::string& unit = "ms") override;
    void addMemoryUsage(uint64_t used, uint64_t total) override;
    void setMemoryUsage(double percent) override;
    void setCpuUsage(double percent) override;
    void setActiveThreads(int count) override;
    void setPerformanceMode(bool enabled) override;
    void setDebugMode(bool enabled) override;
    void clear() override;
    std::string getLastOutput() const override;
    bool isVisible() const override;
    void toggleVisibility() override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};