// Copyright (c) 2026 NLM Project
// Performance profiling implementation

#pragma once

#include "PerformanceProfiler.hpp"
#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <mutex>
#include <atomic>
#include <algorithm>

namespace nlm {

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    void startProfile(const std::string& name);
    void stopProfile(const std::string& name);
    void startProfile(const std::string& name, const std::string& context);
    void stopProfile(const std::string& name, const std::string& context);
    
    double getLastMeasurement(const std::string& name) const;
    std::vector<double> getMeasurements(const std::string& name) const;
    double getAverage(const std::string& name) const;
    double getMax(const std::string& name) const;
    double getMin(const std::string& name) const;
    size_t getCount(const std::string& name) const;
    
    void integrateWithLogger(std::shared_ptr<LoggerManager> loggerManager);
    void integrateWithDebugConsole(std::shared_ptr<IDebugConsole> console);
    void integrateWithSystemMonitor(std::shared_ptr<ISystemMonitor> monitor);
    
    void addProfileToCategory(const std::string& name, const std::string& category);
    void removeProfileFromCategory(const std::string& name);
    std::vector<std::string> getProfilesByCategory(const std::string& category) const;
    
    void setPerformanceThreshold(const std::string& name, double threshold);
    void addCategoryThreshold(const std::string& category, double threshold);
    std::vector<std::string> getExceededThresholds() const;
    
    void exportProfiles(const std::string& filepath) const;
    void clearProfiles();
    
private:
    struct ProfileData {
        std::string name;
        std::string context;
        std::string category;
        std::vector<double> measurements;
        double threshold = 0.0;
        bool isActive = false;
        std::chrono::system_clock::time_point startTime;
    };
    
    std::map<std::string, ProfileData> profiles;
    mutable std::mutex mutex;
    std::shared_ptr<LoggerManager> loggerManager;
    std::shared_ptr<IDebugConsole> debugConsole;
    std::shared_ptr<ISystemMonitor> systemMonitor;
    
    void addMeasurement(const std::string& name, double duration);
};

PerformanceProfiler::PerformanceProfiler() : loggerManager(nullptr), debugConsole(nullptr), systemMonitor(nullptr) {}

PerformanceProfiler::~PerformanceProfiler() {
    clearProfiles();
}

void PerformanceProfiler::startProfile(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end()) {
        profiles[name].isActive = true;
        profiles[name].startTime = std::chrono::system_clock::now();
    }
}

void PerformanceProfiler::stopProfile(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end() && profiles[name].isActive) {
        auto endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - profiles[name].startTime).count();
        profiles[name].isActive = false;
        addMeasurement(name, duration);
    }
}

void PerformanceProfiler::startProfile(const std::string& name, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end()) {
        profiles[name].context = context;
        profiles[name].isActive = true;
        profiles[name].startTime = std::chrono::system_clock::now();
    }
}

void PerformanceProfiler::stopProfile(const std::string& name, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end() && profiles[name].isActive) {
        auto endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - profiles[name].startTime).count();
        profiles[name].isActive = false;
        addMeasurement(name, duration);
    }
}

double PerformanceProfiler::getLastMeasurement(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end() && !profiles[name].measurements.empty()) {
        return profiles[name].measurements.back();
    }
    return 0.0;
}

std::vector<double> PerformanceProfiler::getMeasurements(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end()) {
        return profiles[name].measurements;
    }
    return std::vector<double>();
}

double PerformanceProfiler::getAverage(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end() && !profiles[name].measurements.empty()) {
        double sum = 0.0;
        for (double measurement : profiles[name].measurements) {
            sum += measurement;
        }
        return sum / profiles[name].measurements.size();
    }
    return 0.0;
}

double PerformanceProfiler::getMax(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end() && !profiles[name].measurements.empty()) {
        return *std::max_element(profiles[name].measurements.begin(), profiles[name].measurements.end());
    }
    return 0.0;
}

double PerformanceProfiler::getMin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end() && !profiles[name].measurements.empty()) {
        return *std::min_element(profiles[name].measurements.begin(), profiles[name].measurements.end());
    }
    return 0.0;
}

size_t PerformanceProfiler::getCount(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end()) {
        return profiles[name].measurements.size();
    }
    return 0;
}

void PerformanceProfiler::integrateWithLogger(std::shared_ptr<LoggerManager> loggerManager) {
    std::lock_guard<std::mutex> lock(mutex);
    this->loggerManager = loggerManager;
}

void PerformanceProfiler::integrateWithDebugConsole(std::shared_ptr<IDebugConsole> console) {
    std::lock_guard<std::mutex> lock(mutex);
    this->debugConsole = console;
}

void PerformanceProfiler::integrateWithSystemMonitor(std::shared_ptr<ISystemMonitor> monitor) {
    std::lock_guard<std::mutex> lock(mutex);
    this->systemMonitor = monitor;
}

void PerformanceProfiler::addProfileToCategory(const std::string& name, const std::string& category) {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end()) {
        profiles[name].category = category;
    }
}

void PerformanceProfiler::removeProfileFromCategory(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end()) {
        profiles[name].category.clear();
    }
}

std::vector<std::string> PerformanceProfiler::getProfilesByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> result;
    for (const auto& pair : profiles) {
        if (pair.second.category == category) {
            result.push_back(pair.first);
        }
    }
    return result;
}

void PerformanceProfiler::setPerformanceThreshold(const std::string& name, double threshold) {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end()) {
        profiles[name].threshold = threshold;
    }
}

void PerformanceProfiler::addCategoryThreshold(const std::string& category, double threshold) {
    std::lock_guard<std::mutex> lock(mutex);
    for (auto& pair : profiles) {
        if (pair.second.category == category) {
            pair.second.threshold = threshold;
        }
    }
}

std::vector<std::string> PerformanceProfiler::getExceededThresholds() const {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<std::string> exceeded;
    for (const auto& pair : profiles) {
        if (!pair.second.measurements.empty() && pair.second.threshold > 0) {
            double lastMeasurement = pair.second.measurements.back();
            if (lastMeasurement > pair.second.threshold) {
                exceeded.push_back(pair.first);
            }
        }
    }
    return exceeded;
}

void PerformanceProfiler::exportProfiles(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(mutex);
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    file << "Name,Category,Last,Average,Max,Min,Count,Threshold\n";
    for (const auto& pair : profiles) {
        const auto& data = pair.second;
        file << data.name << "," << data.category << ",";
        if (!data.measurements.empty()) {
            file << data.measurements.back() << ",";
            double sum = 0.0;
            for (double m : data.measurements) sum += m;
            file << (sum / data.measurements.size()) << ",";
            file << *std::max_element(data.measurements.begin(), data.measurements.end()) << ",";
            file << *std::min_element(data.measurements.begin(), data.measurements.end()) << ",";
            file << data.measurements.size() << ",";
        } else {
            file << "N/A,N/A,N/A,N/A,N/A,0";
        }
        file << data.threshold << "\n";
    }
}

void PerformanceProfiler::clearProfiles() {
    std::lock_guard<std::mutex> lock(mutex);
    profiles.clear();
}

void PerformanceProfiler::addMeasurement(const std::string& name, double duration) {
    std::lock_guard<std::mutex> lock(mutex);
    if (profiles.find(name) != profiles.end()) {
        profiles[name].measurements.push_back(duration);
        
        // Log the measurement if logger is integrated
        if (loggerManager) {
            LogEntry entry;
            entry.level = LogLevel::Info;
            entry.message = "Performance measurement: " + name + " = " + std::to_string(duration) + " ms";
            entry.module = "performance_profiler";
            entry.isPerformanceEvent = true;
            entry.addMetric(name, duration);
            
            loggerManager->log(entry);
        }
        
        // Update debug console if integrated
        if (debugConsole) {
            debugConsole->addPerformanceMetric(name, duration, "ms");
        }
        
        // Update system monitor if integrated
        if (systemMonitor) {
            systemMonitor->addCustomMetric(name, duration, "ms");
        }
    }
}