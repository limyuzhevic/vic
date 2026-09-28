// Copyright (c) 2026 NLM Project
// Implementation of Debug Console

#pragma once

#include "DebugConsole.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

DebugConsole::DebugConsole() : visible(false), performanceMode(false), debugMode(false), logLevelFilter(LogLevel::Info), activeThreads(0) {
    std::thread consoleThread([this]() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            updateSystemInfo();
        }
    });
    consoleThread.detach();
}

DebugConsole::~DebugConsole() {
    running = false;
}

void DebugConsole::update() {
    updateSystemInfo();
}

void DebugConsole::render() {
    if (!visible) return;
    
    // In a real implementation, this would render a GUI window
    // For now, we'll simulate the console output
    std::cout << "=== NLM Debug Console ===" << std::endl;
    std::cout << "Visibility: " << (visible ? "Visible" : "Hidden") << std::endl;
    std::cout << "Performance Mode: " << (performanceMode ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Debug Mode: " << (debugMode ? "Enabled" : "Disabled") << std::endl;
    std::cout << "Active Threads: " << activeThreads << std::endl;
    std::cout << "\nSystem Status:" << std::endl;
    for (const auto& status : systemStatus) {
        std::cout << "  " << status.component << ": " << status.value << " (" << status.status << ")" << std::endl;
    }
    
    std::cout << "\nRecent Logs:" << std::endl;
    for (const auto& log : recentLogs) {
        std::cout << "  [" << log.timestamp << "] " << log.level << ": " << log.message << std::endl;
    }
}

void DebugConsole::logMessage(const std::string& message, LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    ConsoleLogEntry entry;
    entry.message = message;
    entry.level = level;
    entry.timestamp = std::chrono::system_clock::now();
    
    recentLogs.push_back(entry);
    
    // Keep only recent logs
    if (recentLogs.size() > 1000) {
        recentLogs.erase(recentLogs.begin());
    }
    
    // Update system monitor if integrated
    if (systemMonitor) {
        systemMonitor->addCustomMetric("log_message", 1.0, "count");
    }
}

void DebugConsole::addSystemStatus(const std::string& name, const std::string& value, const std::string& status) {
    std::lock_guard<std::mutex> lock(statusMutex);
    
    SystemStatus entry;
    entry.component = name;
    entry.value = value;
    entry.status = status;
    entry.timestamp = std::chrono::system_clock::now();
    
    systemStatus.push_back(entry);
    
    // Keep only recent status entries
    if (systemStatus.size() > 1000) {
        systemStatus.erase(systemStatus.begin());
    }
}

void DebugConsole::addPerformanceMetric(const std::string& name, double value, const std::string& unit) {
    std::lock_guard<std::mutex> lock(metricMutex);
    
    PerformanceMetric entry;
    entry.name = name;
    entry.value = value;
    entry.unit = unit;
    entry.timestamp = std::chrono::system_clock::now();
    
    performanceMetrics.push_back(entry);
    
    // Keep only recent metrics
    if (performanceMetrics.size() > 1000) {
        performanceMetrics.erase(performanceMetrics.begin());
    }
}

void DebugConsole::addMemoryUsage(uint64_t used, uint64_t total) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    
    memoryInfo.used = used;
    memoryInfo.total = total;
    memoryInfo.percent = total > 0 ? (static_cast<double>(used) / total) * 100.0 : 0.0;
    memoryInfo.timestamp = std::chrono::system_clock::now();
    
    setMemoryUsage(memoryInfo.percent);
}

void DebugConsole::setMemoryUsage(double percent) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    memoryInfo.percent = percent;
    memoryInfo.timestamp = std::chrono::system_clock::now();
}

void DebugConsole::setCpuUsage(double percent) {
    std::lock_guard<std::mutex> lock(cpuMutex);
    cpuInfo.usage = percent;
    cpuInfo.timestamp = std::chrono::system_clock::now();
}

void DebugConsole::setActiveThreads(int count) {
    std::lock_guard<std::mutex> lock(threadMutex);
    activeThreads = count;
}

void DebugConsole::setPerformanceMode(bool enabled) {
    std::lock_guard<std::mutex> lock(modeMutex);
    performanceMode = enabled;
}

void DebugConsole::setDebugMode(bool enabled) {
    std::lock_guard<std::mutex> lock(modeMutex);
    debugMode = enabled;
}

void DebugConsole::clear() {
    std::lock_guard<std::mutex> logLock(logMutex);
    std::lock_guard<std::mutex> statusLock(statusMutex);
    std::lock_guard<std::mutex> metricLock(metricMutex);
    
    recentLogs.clear();
    systemStatus.clear();
    performanceMetrics.clear();
}

std::string DebugConsole::getLastOutput() const {
    std::lock_guard<std::mutex> lock(logMutex);
    if (recentLogs.empty()) {
        return "";
    }
    
    std::stringstream ss;
    ss << "=== Recent Logs ===\n";
    for (const auto& log : recentLogs) {
        ss << "[" << formatTime(log.timestamp) << "] " << log.level << ": " << log.message << "\n";
    }
    return ss.str();
}

bool DebugConsole::isVisible() const {
    std::lock_guard<std::mutex> lock(visibleMutex);
    return visible;
}

void DebugConsole::toggleVisibility() {
    std::lock_guard<std::mutex> lock(visibleMutex);
    visible = !visible;
    if (visible) {
        logMessage("Debug console shown", LogLevel::Info);
    } else {
        logMessage("Debug console hidden", LogLevel::Info);
    }
}

void DebugConsole::show() {
    std::lock_guard<std::mutex> lock(visibleMutex);
    visible = true;
    logMessage("Debug console shown", LogLevel::Info);
}

void DebugConsole::hide() {
    std::lock_guard<std::mutex> lock(visibleMutex);
    visible = false;
    logMessage("Debug console hidden", LogLevel::Info);
}

void DebugConsole::minimize() {
    logMessage("Debug console minimized", LogLevel::Info);
}

void DebugConsole::maximize() {
    logMessage("Debug console maximized", LogLevel::Info);
}

void DebugConsole::updateSystemInfo() {
    // Update CPU and memory usage (simplified)
    static double lastCpuUsage = 0.0;
    static double lastMemoryUsage = 0.0;
    
    double currentCpuUsage = getCurrentCpuUsage();
    double currentMemoryUsage = getCurrentMemoryUsage();
    
    if (std::abs(currentCpuUsage - lastCpuUsage) > 1.0) {
        setCpuUsage(currentCpuUsage);
    }
    
    if (std::abs(currentMemoryUsage - lastMemoryUsage) > 1024 * 1024) { // 1MB change
        setMemoryUsage(currentMemoryUsage / 1024 / 1024); // Convert to MB
    }
    
    lastCpuUsage = currentCpuUsage;
    lastMemoryUsage = currentMemoryUsage;
}

void DebugConsole::updateThreadInfo() {
    // Update thread information (simplified)
    // In a real implementation, this would use system APIs to get thread info
    setActiveThreads(0); // Placeholder
}

void DebugConsole::updatePerformanceMetrics() {
    // Update performance metrics (simplified)
    // In a real implementation, this would update based on actual performance data
}

void DebugConsole::setLogLevelFilter(LogLevel level) {
    std::lock_guard<std::mutex> lock(logFilterMutex);
    logLevelFilter = level;
}

void DebugConsole::setModuleFilter(const std::string& module) {
    std::lock_guard<std::mutex> lock(moduleFilterMutex);
    moduleFilter = module;
}

void DebugConsole::addCustomFilter(const std::string& pattern) {
    std::lock_guard<std::mutex> lock(customFilterMutex);
    customFilters.push_back(pattern);
}

void DebugConsole::clearFilters() {
    std::lock_guard<std::mutex> logLock(logFilterMutex);
    std::lock_guard<std::mutex> moduleLock(moduleFilterMutex);
    std::lock_guard<std::mutex> customLock(customFilterMutex);
    
    logLevelFilter = LogLevel::Info;
    moduleFilter.clear();
    customFilters.clear();
}

void DebugConsole::exportLogs(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "Timestamp,Level,Message\n";
    for (const auto& log : recentLogs) {
        file << formatTime(log.timestamp) << ",";
        file << static_cast<int>(log.level) << ",";
        // Escape quotes and newlines
        std::string message = log.message;
        std::replace(message.begin(), message.end(), '"', '""');
        std::replace(message.begin(), message.end(), '\n', ' ');
        file << "\"" << message << "\"" << std::endl;
    }
}

void DebugConsole::exportPerformanceData(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(metricMutex);
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "Timestamp,Metric,Value,Unit\n";
    for (const auto& metric : performanceMetrics) {
        file << formatTime(metric.timestamp) << ",";
        file << metric.name << ",";
        file << metric.value << ",";
        file << metric.unit << std::endl;
    }
}

void DebugConsole::exportSystemInfo(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(statusMutex);
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "Timestamp,Component,Value,Status,Message\n";
    for (const auto& status : systemStatus) {
        file << formatTime(status.timestamp) << ",";
        file << status.component << ",";
        file << status.value << ",";
        file << status.status << ";";
        // Escape quotes and newlines
        std::string message = status.message;
        std::replace(message.begin(), message.end(), '"', '""');
        std::replace(message.begin(), message.end(), '\n', ' ');
        file << "\"" << message << "\"" << std::endl;
    }
}

void DebugConsole::integrateWithLogger(std::shared_ptr<LoggerManager> loggerManager) {
    std::lock_guard<std::mutex> lock(loggerIntegrationMutex);
    this->loggerManager = loggerManager;
}

void DebugConsole::integrateWithPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler) {
    std::lock_guard<std::mutex> lock(profilerIntegrationMutex);
    this->profiler = profiler;
}

void DebugConsole::integrateWithMemoryTracker(std::shared_ptr<MemoryTracker> tracker) {
    std::lock_guard<std::mutex> lock(memoryTrackerIntegrationMutex);
    this->memoryTracker = tracker;
}

void DebugConsole::updateDisplay() {
    // Update display (simplified)
    // In a real implementation, this would update the GUI
}

void DebugConsole::renderLogWindow() {
    // Render log window (simplified)
    // In a real implementation, this would render GUI components
}

void DebugConsole::renderSystemInfoWindow() {
    // Render system info window (simplified)
    // In a real implementation, this would render GUI components
}

void DebugConsole::renderPerformanceWindow() {
    // Render performance window (simplified)
    // In a real implementation, this would render GUI components
}

void DebugConsole::renderMemoryWindow() {
    // Render memory window (simplified)
    // In a real implementation, this would render GUI components
}

void DebugConsole::renderThreadWindow() {
    // Render thread window (simplified)
    // In a real implementation, this would render GUI components
}

void DebugConsole::renderSettingsWindow() {
    // Render settings window (simplified)
    // In a real implementation, this would render GUI components
}

std::string DebugConsole::formatTime(const std::chrono::system_clock::time_point& time) const {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string DebugConsole::formatMemory(uint64_t bytes) const {
    if (bytes >= 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
    } else if (bytes >= 1024 * 1024) {
        return std::to_string(bytes / (1024 * 1024)) + " MB";
    } else if (bytes >= 1024) {
        return std::to_string(bytes / 1024) + " KB";
    } else {
        return std::to_string(bytes) + " bytes";
    }
}

std::string DebugConsole::formatPercentage(double percent) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << percent << "%";
    return ss.str();
}

std::string DebugConsole::getStatusColor(const std::string& status) const {
    if (status == "ok") return "green";
    if (status == "warning") return "yellow";
    if (status == "error") return "red";
    if (status == "critical") return "magenta";
    return "white";
}