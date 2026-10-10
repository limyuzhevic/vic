// NLM Advanced CLI - Debug Tools Implementation
#pragma once

#include "tools/debug.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <memory>

namespace nlm {

class DebugLogger {
public:
    DebugLogger(const std::string& name) : name_(name) {
        logFile_ = "debug_" + name + "_log.txt";
        clearLogFile();
    }
    
    ~DebugLogger() {
        close();
    }
    
    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::system_clock::to_time_t(now);
        
        std::ofstream file(logFile_, std::ios::app);
        if (file.is_open()) {
            file << "[" << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S") << "] "
                 << "[" << name_ << "] " << message << std::endl;
        }
        
        // Also print to console if debug mode is enabled
        std::cout << "[" << name_ << "] " << message << std::endl;
    }
    
    void clearLogFile() {
        std::ofstream file(logFile_, std::ios::trunc);
        if (file.is_open()) {
            file << "Debug log started at " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
        }
    }
    
    void close() {
        std::ofstream file(logFile_, std::ios::app);
        if (file.is_open()) {
            file << "Debug log ended at " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
        }
    }
    
    std::string getLogFile() const { return logFile_; }
    
private:
    std::string name_;
    std::string logFile_;
    std::mutex mutex_;
};

class EventTracker {
public:
    struct Event {
        std::chrono::system_clock::time_point timestamp;
        std::string type;
        std::string description;
        std::map<std::string, std::string> metadata;
    };
    
    EventTracker(const std::string& name) : name_(name) {
    }
    
    void trackEvent(const std::string& type, const std::string& description) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        Event event;
        event.timestamp = std::chrono::system_clock::now();
        event.type = type;
        event.description = description;
        
        events_.push_back(event);
        
        std::cout << "[EVENT] " << description << std::endl;
    }
    
    void addMetadata(const std::string& key, const std::string& value) {
        if (!events_.empty()) {
            events_.back().metadata[key] = value;
        }
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        events_.clear();
    }
    
    size_t getEventCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return events_.size();
    }
    
    std::vector<Event> getEvents(const std::string& type = "") const {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (type.empty()) {
            return events_;
        }
        
        std::vector<Event> filteredEvents;
        for (const auto& event : events_) {
            if (event.type == type) {
                filteredEvents.push_back(event);
            }
        }
        
        return filteredEvents;
    }
    
    void printEventReport() const {
        std::cout << "=== Debug Event Report ===\n" << std::endl;
        std::cout << "Total events tracked: " << getEventCount() << std::endl;
        std::cout << "Event types:" << std::endl;
        
        std::map<std::string, size_t> typeCounts;
        for (const auto& event : events_) {
            typeCounts[event.type]++;
        }
        
        for (const auto& pair : typeCounts) {
            std::cout << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }
    
private:
    std::string name_;
    std::vector<Event> events_;
    mutable std::mutex mutex_;
};

class PerformanceProfiler {
public:
    struct PerformanceData {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        double duration;  // in milliseconds
        std::map<std::string, double> metrics;
    };
    
    PerformanceProfiler(const std::string& name) : name_(name) {
    }
    
    void startProfile(const std::string& operation) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        currentOperation_ = operation;
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void endProfile() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(endTime - startTime_);
        
        PerformanceData data;
        data.timestamp = endTime;
        data.operation = currentOperation_;
        data.duration = duration.count();
        
        data.metrics["duration_ms"] = data.duration;
        
        performanceData_.push_back(data);
        
        std::cout << "[PROFILER] " << currentOperation_ << " completed in " << duration.count() << " ms" << std::endl;
        
        currentOperation_.clear();
    }
    
    void addMetric(const std::string& key, double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!performanceData_.empty()) {
            performanceData_.back().metrics[key] = value;
        }
    }
    
    void printProfileReport() const {
        std::cout << "=== Performance Profile Report ===\n" << std::endl;
        std::cout << "Total operations profiled: " << performanceData_.size() << std::endl;
        
        if (performanceData_.empty()) {
            return;
        }
        
        double totalTime = 0.0;
        double maxTime = 0.0;
        double minTime = std::numeric_limits<double>::max();
        
        for (const auto& data : performanceData_) {
            totalTime += data.duration;
            maxTime = std::max(maxTime, data.duration);
            minTime = std::min(minTime, data.duration);
        }
        
        std::cout << "Total time: " << totalTime << " ms" << std::endl;
        std::cout << "Average time per operation: " << (totalTime / performanceData_.size()) << " ms" << std::endl;
        std::cout << "Max time: " << maxTime << " ms" << std::endl;
        std::cout << "Min time: " << minTime << " ms" << std::endl;
        
        std::cout << "\nDetailed performance data:" << std::endl;
        for (const auto& data : performanceData_) {
            std::cout << "  " << data.operation << ": " << data.duration << " ms" << std::endl;
            if (!data.metrics.empty()) {
                for (const auto& metric : data.metrics) {
                    if (metric.first != "duration_ms") {
                        std::cout << "    " << metric.first << ": " << metric.second << std::endl;
                    }
                }
            }
        }
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        performanceData_.clear();
    }
    
private:
    std::string name_;
    std::string currentOperation_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<PerformanceData> performanceData_;
    mutable std::mutex mutex_;
};

// Main debug tools class
class debug_tools : public IDebugTools {
public:
    debug_tools() {
        eventTracker_ = std::make_unique<EventTracker>("Main");
        performanceProfiler_ = std::make_unique<PerformanceProfiler>("Main");
        
        // Create subsystem loggers
        brainLogger_ = std::make_unique<DebugLogger>("Brain");
        configLogger_ = std::make_unique<DebugLogger>("Config");
        visualizationLogger_ = std::make_unique<DebugLogger>("Visualization");
        explorationLogger_ = std::make_unique<DebugLogger>("Exploration");
    }
    
    ~debug_tools() = default;
    
    void setLevel(DebugLevel level) override {
        level_ = level;
        
        switch (level_) {
            case DebugLevel::Off:
                std::cout << "Debug level: Off" << std::endl;
                break;
            case DebugLevel::Error:
                std::cout << "Debug level: Error" << std::endl;
                break;
            case DebugLevel::Warning:
                std::cout << "Debug level: Warning" << std::endl;
                break;
            case DebugLevel::Info:
                std::cout << "Debug level: Info" << std::endl;
                break;
            case DebugLevel::Debug:
                std::cout << "Debug level: Debug" << std::endl;
                break;
            case DebugLevel::Trace:
                std::cout << "Debug level: Trace" << std::endl;
                break;
        }
    }
    
    DebugLevel getLevel() const override {
        return level_;
    }
    
    void logEvent(const std::string& event, const std::map<std::string, std::string>& metadata = {}) override {
        if (level_ >= DebugLevel::Info) {
            eventTracker_->trackEvent("UserEvent", event);
            
            for (const auto& pair : metadata) {
                eventTracker_->addMetadata(pair.first, pair.second);
            }
        }
    }
    
    void logBrain(const std::string& message) override {
        if (level_ >= DebugLevel::Debug) {
            brainLogger_->log(message);
        }
    }
    
    void logConfig(const std::string& message) override {
        if (level_ >= DebugLevel::Info) {
            configLogger_->log(message);
        }
    }
    
    void logVisualization(const std::string& message) override {
        if (level_ >= DebugLevel::Warning) {
            visualizationLogger_->log(message);
        }
    }
    
    void logExploration(const std::string& message) override {
        if (level_ >= DebugLevel::Info) {
            explorationLogger_->log(message);
        }
    }
    
    void startPerformanceMeasurement(const std::string& operation) override {
        if (level_ >= DebugLevel::Debug) {
            performanceProfiler_->startProfile(operation);
        }
    }
    
    void endPerformanceMeasurement() override {
        if (level_ >= DebugLevel::Debug) {
            performanceProfiler_->endProfile();
        }
    }
    
    void addPerformanceMetric(const std::string& key, double value) override {
        if (level_ >= DebugLevel::Debug) {
            performanceProfiler_->addMetric(key, value);
        }
    }
    
    void printDebugReport() override {
        std::cout << "=== Complete Debug Report ===\n" << std::endl;
        
        std::cout << "Debug Level: " << static_cast<int>(level_) << std::endl;
        std::cout << "Log Files:" << std::endl;
        
        if (brainLogger_) {
            std::cout << "  Brain: " << brainLogger_->getLogFile() << std::endl;
        }
        if (configLogger_) {
            std::cout << "  Config: " << configLogger_->getLogFile() << std::endl;
        }
        if (visualizationLogger_) {
            std::cout << "  Visualization: " << visualizationLogger_->getLogFile() << std::endl;
        }
        if (explorationLogger_) {
            std::cout << "  Exploration: " << explorationLogger_->getLogFile() << std::endl;
        }
        
        eventTracker_->printEventReport();
        performanceProfiler_->printProfileReport();
    }
    
    void enableBenchmarkMode() override {
        std::cout << "Benchmark mode enabled." << std::endl;
        benchmarkMode_ = true;
    }
    
    void disableBenchmarkMode() override {
        std::cout << "Benchmark mode disabled." << std::endl;
        benchmarkMode_ = false;
    }
    
    bool isBenchmarkMode() const override {
        return benchmarkMode_;
    }
    
    void enableValidationMode() override {
        std::cout << "Validation mode enabled." << std::endl;
        validationMode_ = true;
    }
    
    void disableValidationMode() override {
        std::cout << "Validation mode disabled." << std::endl;
        validationMode_ = false;
    }
    
    bool isValidationMode() const override {
        return validationMode_;
    }
    
    void clearAllLogs() override {
        std::cout << "Clearing all debug logs..." << std::endl;
        
        if (brainLogger_) {
            brainLogger_->clearLogFile();
        }
        if (configLogger_) {
            configLogger_->clearLogFile();
        }
        if (visualizationLogger_) {
            visualizationLogger_->clearLogFile();
        }
        if (explorationLogger_) {
            explorationLogger_->clearLogFile();
        }
        
        eventTracker_->clear();
        performanceProfiler_->clear();
    }
    
private:
    DebugLevel level_ = DebugLevel::Warning;
    bool benchmarkMode_ = false;
    bool validationMode_ = false;
    
    std::unique_ptr<EventTracker> eventTracker_;
    std::unique_ptr<PerformanceProfiler> performanceProfiler_;
    std::unique_ptr<DebugLogger> brainLogger_;
    std::unique_ptr<DebugLogger> configLogger_;
    std::unique_ptr<DebugLogger> visualizationLogger_;
    std::unique_ptr<DebugLogger> explorationLogger_;
};

} // namespace nlm

