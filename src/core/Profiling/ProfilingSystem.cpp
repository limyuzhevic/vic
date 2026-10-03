#include "ProfilingSystem.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <thread>
#include <chrono>
#include <ctime>
#include <cmath>

namespace nlm {

// Profiler implementation
class Profiler::Impl {
private:
    ThreadLocalProfiler threadLocalProfiler;
    bool active = true;
    std::map<std::string, std::chrono::system_clock::time_point> profileStartTimes;
    std::mutex mutex;
    
public:
    void start(const std::string& name) {
        if (!active) return;
        threadLocalProfiler.startProfile(name);
        std::lock_guard<std::mutex> lock(mutex);
        profileStartTimes[name] = std::chrono::system_clock::now();
    }
    
    void stop(const std::string& name) {
        if (!active) return;
        threadLocalProfiler.stopProfile(name);
        std::lock_guard<std::mutex> lock(mutex);
        profileStartTimes.erase(name);
    }
    
    ThreadLocalProfiler::ProfileData* getProfile(const std::string& name) {
        return threadLocalProfiler.getProfile(name);
    }
    
    std::vector<ThreadLocalProfiler::ProfileData> getAllProfiles() const {
        return threadLocalProfiler.getAllProfiles();
    }
    
    void clear() {
        threadLocalProfiler.clear();
        std::lock_guard<std::mutex> lock(mutex);
        profileStartTimes.clear();
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "Profiling Report\n";
        oss << "=================\n\n";
        
        auto profiles = threadLocalProfiler.getAllProfiles();
        if (profiles.empty()) {
            oss << "No profiles recorded.\n";
            return oss.str();
        }
        
        // Sort by total time
        std::sort(profiles.begin(), profiles.end(),
                 [](const auto& a, const auto& b) { return a.totalTime > b.totalTime; });
        
        oss << "Profile Summary (sorted by total time):\n";
        for (const auto& profile : profiles) {
            oss << "  " << profile.name << ": "
                << profile.callCount << " calls, "
                << std::fixed << std::setprecision(6) << profile.totalTime << "s total, "
                << std::setprecision(3) << (profile.totalTime / profile.callCount) << "s avg, "
                << std::setprecision(3) << profile.minTime << "s min, "
                << std::setprecision(3) << profile.maxTime << "s max\n";
        }
        
        oss << "\n";
        return oss.str();
    }
    
    bool saveReport(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport();
        return true;
    }
    
    void merge(const Profiler& other) {
        // Implementation would merge with another profiler's data
        // For now, just copy references
        threadLocalProfiler = other.pImpl->threadLocalProfiler;
    }
    
    double getTotalTime() const {
        double total = 0.0;
        auto profiles = threadLocalProfiler.getAllProfiles();
        for (const auto& profile : profiles) {
            total += profile.totalTime;
        }
        return total;
    }
    
    uint64_t getCallCount(const std::string& name) const {
        auto profile = threadLocalProfiler.getProfile(name);
        return profile ? profile->callCount : 0;
    }
    
    bool isActive() const {
        return active;
    }
    
    void setActive(bool active) {
        this->active = active;
    }
};

Profiler::Profiler() : pImpl(std::make_unique<Impl>()) {}
Profiler::~Profiler() = default;

void Profiler::start(const std::string& name) {
    pImpl->start(name);
}

void Profiler::stop(const std::string& name) {
    pImpl->stop(name);
}

Profiler::ScopedTimer Profiler::scoped(const std::string& name) {
    return ScopedTimer(getInstance(), name);
}

ThreadLocalProfiler::ProfileData* Profiler::getProfile(const std::string& name) {
    return pImpl->getProfile(name);
}

std::vector<ThreadLocalProfiler::ProfileData> Profiler::getAllProfiles() const {
    return pImpl->getAllProfiles();
}

void Profiler::clear() {
    pImpl->clear();
}

std::string Profiler::generateReport() const {
    return pImpl->generateReport();
}

bool Profiler::saveReport(const std::string& filepath) const {
    return pImpl->saveReport(filepath);
}

void Profiler::merge(const Profiler& other) {
    pImpl->merge(other);
}

 double Profiler::getTotalTime() const {
    return pImpl->getTotalTime();
}

uint64_t Profiler::getCallCount(const std::string& name) const {
    return pImpl->getCallCount(name);
}

bool Profiler::isActive() const {
    return pImpl->isActive();
}

void Profiler::setActive(bool active) {
    pImpl->setActive(active);
}

// MetricsCollector implementation
class MetricsCollector::Impl {
private:
    struct MetricData {
        std::string name;
        std::vector<std::pair<std::chrono::system_clock::time_point, double>> values;
        std::map<std::string, std::vector<std::pair<std::chrono::system_clock::time_point, double>>> byLabel;
    };
    
    std::map<std::string, MetricData> metrics;
    std::mutex mutex;
    
public:
    void record(const std::string& name, double value,
                const std::map<std::string, std::string>& labels) {
        std::lock_guard<std::mutex> lock(mutex);
        auto now = std::chrono::system_clock::now();
        
        if (metrics.find(name) == metrics.end()) {
            metrics[name] = {name, {}, {}};
        }
        
        auto& data = metrics[name];
        data.values.push_back({now, value});
        
        // Store by labels
        for (const auto& label : labels) {
            if (data.byLabel.find(label.first) == data.byLabel.end()) {
                data.byLabel[label.first] = {};
            }
            data.byLabel[label.first].push_back({now, value});
        }
    }
    
    void record(const std::string& name, double value,
                std::chrono::system_clock::time_point timestamp,
                const std::map<std::string, std::string>& labels) {
        std::lock_guard<std::mutex> lock(mutex);
        
        if (metrics.find(name) == metrics.end()) {
            metrics[name] = {name, {}, {}};
        }
        
        auto& data = metrics[name];
        data.values.push_back({timestamp, value});
        
        // Store by labels
        for (const auto& label : labels) {
            if (data.byLabel.find(label.first) == data.byLabel.end()) {
                data.byLabel[label.first] = {};
            }
            data.byLabel[label.first].push_back({timestamp, value});
        }
    }
    
    std::optional<double> getLatest(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = metrics.find(name);
        if (it == metrics.end() || it->second.values.empty()) {
            return std::nullopt;
        }
        return it->second.values.back().second;
    }
    
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> 
    getAll(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = metrics.find(name);
        if (it == metrics.end()) {
            return {};
        }
        return it->second.values;
    }
    
    std::vector<std::string> getMetricNames() const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::string> names;
        names.reserve(metrics.size());
        for (const auto& pair : metrics) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::vector<std::pair<std::string, double>> 
    getByLabel(const std::string& labelName, const std::string& labelValue) const {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<std::pair<std::string, double>> result;
        
        for (const auto& pair : metrics) {
            const auto& data = pair.second;
            
            // Note: This is a simplified implementation
            // In a real system, labels would be key-value pairs per metric
            for (const auto& valuePair : data.values) {
                result.push_back({pair.first, valuePair.second});
            }
        }
        
        return result;
    }
    
    MetricStats calculateStats(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex);
        MetricStats stats;
        auto it = metrics.find(name);
        if (it == metrics.end() || it->second.values.empty()) {
            return stats;
        }
        
        const auto& values = it->second.values;
        stats.count = values.size();
        
        if (stats.count == 0) return stats;
        
        stats.min = values[0].second;
        stats.max = values[0].second;
        stats.sum = 0.0;
        stats.sumSquared = 0.0;
        
        for (const auto& valuePair : values) {
            double value = valuePair.second;
            stats.min = std::min(stats.min, value);
            stats.max = std::max(stats.max, value);
            stats.sum += value;
            stats.sumSquared += value * value;
        }
        
        stats.mean = stats.sum / stats.count;
        stats.stddev = std::sqrt((stats.sumSquared - stats.sum * stats.sum / stats.count) / (stats.count - 1));
        
        return stats;
    }
    
    void cleanupOldMetrics(std::chrono::system_clock::time_point cutoff) {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& pair : metrics) {
            auto& values = pair.second.values;
            values.erase(std::remove_if(values.begin(), values.end(),
                                       [cutoff](const auto& valuePair) {
                                           return valuePair.first < cutoff;
                                       }),
                        values.end());
            
            // Also clean by-label data
            for (auto& labelPair : pair.second.byLabel) {
                auto& labelValues = labelPair.second;
                labelValues.erase(std::remove_if(labelValues.begin(), labelValues.end(),
                                                [cutoff](const auto& valuePair) {
                                                    return valuePair.first < cutoff;
                                                }),
                                labelValues.end());
            }
        }
    }
    
    std::string exportToCSV() const {
        std::ostringstream oss;
        oss << "Metric,Timestamp,Value\n";
        
        for (const auto& pair : metrics) {
            for (const auto& valuePair : pair.second.values) {
                auto time = std::chrono::system_clock::to_time_t(valuePair.first);
                oss << pair.first << ","
                    << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << ","
                    << valuePair.second << "\n";
            }
        }
        
        return oss.str();
    }
    
    std::string exportToJSON() const {
        std::ostringstream oss;
        oss << "{\"metrics\": [{\"name\": \"metric\", \"values\": []}]}";
        // TODO: Implement proper JSON export
        return oss.str();
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        metrics.clear();
    }
    
    size_t getTotalCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t total = 0;
        for (const auto& pair : metrics) {
            total += pair.second.values.size();
        }
        return total;
    }
};

MetricsCollector::MetricsCollector() : pImpl(std::make_unique<Impl>()) {}
MetricsCollector::~MetricsCollector() = default;

void MetricsCollector::record(const std::string& name, double value,
                             const std::map<std::string, std::string>& labels) {
    pImpl->record(name, value, labels);
}

void MetricsCollector::record(const std::string& name, double value,
                             std::chrono::system_clock::time_point timestamp,
                             const std::map<std::string, std::string>& labels) {
    pImpl->record(name, value, timestamp, labels);
}

std::optional<double> MetricsCollector::getLatest(const std::string& name) const {
    return pImpl->getLatest(name);
}

std::vector<std::pair<std::chrono::system_clock::time_point, double>> 
MetricsCollector::getAll(const std::string& name) const {
    return pImpl->getAll(name);
}

std::vector<std::string> MetricsCollector::getMetricNames() const {
    return pImpl->getMetricNames();
}

std::vector<std::pair<std::string, double>> 
MetricsCollector::getByLabel(const std::string& labelName, const std::string& labelValue) const {
    return pImpl->getByLabel(labelName, labelValue);
}

MetricsCollector::MetricStats MetricsCollector::calculateStats(const std::string& name) const {
    return pImpl->calculateStats(name);
}

void MetricsCollector::cleanupOldMetrics(std::chrono::system_clock::time_point cutoff) {
    pImpl->cleanupOldMetrics(cutoff);
}

std::string MetricsCollector::exportToCSV() const {
    return pImpl->exportToCSV();
}

std::string MetricsCollector::exportToJSON() const {
    return pImpl->exportToJSON();
}

void MetricsCollector::clear() {
    pImpl->clear();
}

size_t MetricsCollector::getTotalCount() const {
    return pImpl->getTotalCount();
}

// MemoryProfiler implementation
class MemoryProfiler::Impl {
private:
    struct AllocationRecord {
        void* ptr;
        size_t size;
        std::string label;
        std::chrono::system_clock::time_point timestamp;
        std::thread::id threadId;
    };
    
    std::vector<AllocationRecord> allocations;
    std::mutex mutex;
    bool tracking = false;
    size_t sampleIntervalMs = 1000;
    std::chrono::system_clock::time_point lastSample;
    
public:
    void trackAllocation(void* ptr, size_t size, const std::string& label) {
        std::lock_guard<std::mutex> lock(mutex);
        allocations.push_back({ptr, size, label, std::chrono::system_clock::now(), std::this_thread::get_id()});
    }
    
    void trackDeallocation(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex);
        allocations.erase(std::remove_if(allocations.begin(), allocations.end(),
                                       [ptr](const AllocationRecord& rec) {
                                           return rec.ptr == ptr;
                                       }),
                        allocations.end());
    }
    
    size_t getCurrentMemoryUsage() const {
        // Simplified implementation - return estimated memory usage
        size_t total = 0;
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& alloc : allocations) {
            total += alloc.size;
        }
        return total;
    }
    
    size_t getPeakMemoryUsage() const {
        // Simplified implementation - return current usage as peak
        return getCurrentMemoryUsage();
    }
    
    std::vector<std::pair<std::chrono::system_clock::time_point, size_t>> 
    getMemoryHistory() const {
        std::vector<std::pair<std::chrono::system_clock::time_point, size_t>> history;
        std::lock_guard<std::mutex> lock(mutex);
        
        // Group allocations by timestamp (simplified)
        size_t currentUsage = 0;
        for (const auto& alloc : allocations) {
            currentUsage += alloc.size;
        }
        
        history.push_back({std::chrono::system_clock::now(), currentUsage});
        return history;
    }
    
    void startTracking(size_t sampleIntervalMs) {
        this->sampleIntervalMs = sampleIntervalMs;
        tracking = true;
        lastSample = std::chrono::system_clock::now();
    }
    
    void stopTracking() {
        tracking = false;
    }
    
    std::string generateMemoryReport() const {
        std::ostringstream oss;
        oss << "Memory Report\n";
        oss << "============\n\n";
        
        size_t current = getCurrentMemoryUsage();
        size_t peak = getPeakMemoryUsage();
        
        oss << "Current memory usage: " << current << " bytes ("
            << (current / 1024.0 / 1024.0) << " MB)\n";
        oss << "Peak memory usage: " << peak << " bytes ("
            << (peak / 1024.0 / 1024.0) << " MB)\n";
        
        oss << "\nAllocation details:\n";
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& alloc : allocations) {
            oss << "  " << alloc.ptr << ": " << alloc.size << " bytes"
                << " label: " << alloc.label
                << " thread: " << alloc.threadId
                << " time: " << std::chrono::system_clock::to_time_t(alloc.timestamp)
                << "\n";
        }
        
        return oss.str();
    }
    
    AllocationStats getAllocationStats() const {
        AllocationStats stats;
        std::lock_guard<std::mutex> lock(mutex);
        
        for (const auto& alloc : allocations) {
            stats.totalAllocated += alloc.size;
            stats.allocationsByLabel[alloc.label] += alloc.size;
        }
        
        stats.current = getCurrentMemoryUsage();
        stats.peak = getPeakMemoryUsage();
        
        return stats;
    }
    
    std::vector<void*> getPotentialLeaks() const {
        std::vector<void*> leaks;
        std::lock_guard<std::mutex> lock(mutex);
        
        // Simple leak detection - any allocation without deallocation
        // This is a naive implementation
        for (const auto& alloc : allocations) {
            leaks.push_back(alloc.ptr);
        }
        
        return leaks;
    }
    
    void cleanup() {
        std::lock_guard<std::mutex> lock(mutex);
        allocations.clear();
    }
};

MemoryProfiler::MemoryProfiler() : pImpl(std::make_unique<Impl>()) {}
MemoryProfiler::~MemoryProfiler() = default;

void MemoryProfiler::trackAllocation(void* ptr, size_t size, const std::string& label) {
    pImpl->trackAllocation(ptr, size, label);
}

void MemoryProfiler::trackDeallocation(void* ptr) {
    pImpl->trackDeallocation(ptr);
}

size_t MemoryProfiler::getCurrentMemoryUsage() const {
    return pImpl->getCurrentMemoryUsage();
}

size_t MemoryProfiler::getPeakMemoryUsage() const {
    return pImpl->getPeakMemoryUsage();
}

std::vector<std::pair<std::chrono::system_clock::time_point, size_t>> 
MemoryProfiler::getMemoryHistory() const {
    return pImpl->getMemoryHistory();
}

void MemoryProfiler::startTracking(size_t sampleIntervalMs) {
    pImpl->startTracking(sampleIntervalMs);
}

void MemoryProfiler::stopTracking() {
    pImpl->stopTracking();
}

std::string MemoryProfiler::generateMemoryReport() const {
    return pImpl->generateMemoryReport();
}

MemoryProfiler::AllocationStats MemoryProfiler::getAllocationStats() const {
    return pImpl->getAllocationStats();
}

std::vector<void*> MemoryProfiler::getPotentialLeaks() const {
    return pImpl->getPotentialLeaks();
}

void MemoryProfiler::cleanup() {
    pImpl->cleanup();
}

// CPUProfiler implementation
class CPUProfiler::Impl {
private:
    struct ThreadCPUInfo {
        std::string threadName;
        double userTime = 0.0;
        double systemTime = 0.0;
        std::chrono::system_clock::time_point lastUpdate;
    };
    
    std::map<std::thread::id, ThreadCPUInfo> threadCPUInfo;
    CPUUsageInfo lastSystemUsage;
    std::chrono::system_clock::time_point lastCPUUpdate;
    bool profiling = false;
    
public:
    void startThreadProfiling(const std::string& threadName) {
        std::lock_guard<std::mutex> lock(mutex);
        std::thread::id threadId = std::this_thread::get_id();
        
        if (!threadName.empty()) {
            threadCPUInfo[threadId].threadName = threadName;
        }
    }
    
    void stopThreadProfiling() {
        std::lock_guard<std::mutex> lock(mutex);
        threadCPUInfo.erase(std::this_thread::get_id());
    }
    
    double getThreadCPUUsage(const std::string& threadName) const {
        // Simplified implementation
        return 0.0;
    }
    
    double getSystemCPUUsage() const {
        // Simplified implementation
        return 0.0;
    }
    
    double getProcessCPUUsage() const {
        // Simplified implementation
        return 0.0;
    }
    
    CPUUsageInfo getDetailedCPUUsage() const {
        // Simplified implementation
        return CPUUsageInfo();
    }
    
    void startProfiling() {
        profiling = true;
        lastCPUUpdate = std::chrono::system_clock::now();
    }
    
    void stopProfiling() {
        profiling = false;
    }
    
    std::string getProfilingResults() const {
        std::ostringstream oss;
        oss << "CPU Profiling Results\n";
        oss << "=====================\n\n";
        oss << "System CPU usage: " << getSystemCPUUsage() * 100 << "%\n";
        oss << "Process CPU usage: " << getProcessCPUUsage() * 100 << "%\n";
        return oss.str();
    }
    
    std::string generateCPUReport() const {
        return getProfilingResults();
    }
    
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> 
    getCPUUsageHistory(const std::string& threadName) const {
        return std::vector<std::pair<std::chrono::system_clock::time_point, double>>();
    }
};

CPUProfiler::CPUProfiler() : pImpl(std::make_unique<Impl>()) {}
CPUProfiler::~CPUProfiler() = default;

void CPUProfiler::startThreadProfiling(const std::string& threadName) {
    pImpl->startThreadProfiling(threadName);
}

void CPUProfiler::stopThreadProfiling() {
    pImpl->stopThreadProfiling();
}

double CPUProfiler::getThreadCPUUsage(const std::string& threadName) const {
    return pImpl->getThreadCPUUsage(threadName);
}

double CPUProfiler::getSystemCPUUsage() const {
    return pImpl->getSystemCPUUsage();
}

double CPUProfiler::getProcessCPUUsage() const {
    return pImpl->getProcessCPUUsage();
}

CPUProfiler::CPUUsageInfo CPUProfiler::getDetailedCPUUsage() const {
    return pImpl->getDetailedCPUUsage();
}

void CPUProfiler::startProfiling() {
    pImpl->startProfiling();
}

void CPUProfiler::stopProfiling() {
    pImpl->stopProfiling();
}

std::string CPUProfiler::getProfilingResults() const {
    return pImpl->getProfilingResults();
}

std::string CPUProfiler::generateCPUReport() const {
    return pImpl->generateCPUReport();
}

std::vector<std::pair<std::chrono::system_clock::time_point, double>> 
CPUProfiler::getCPUUsageHistory(const std::string& threadName) const {
    return pImpl->getCPUUsageHistory(threadName);
}

// Debugger implementation
class Debugger::Impl {
private:
    bool debugMode = false;
    bool stackTraceEnabled = false;
    std::vector<std::string> recentStackTraces;
    std::ostream* output = &std::cerr;
    
public:
    void setBreakpoint(const std::string& location) {
        // TODO: Implement breakpoint setting
    }
    
    void clearBreakpoints() {
        // TODO: Implement breakpoint clearing
    }
    
    void addWatchVariable(const std::string& expression) {
        // TODO: Implement watch variable adding
    }
    
    void printState(const std::string& context) const {
        *output << "Debug state: " << context << "\n";
        // TODO: Print actual state
    }
    
    void dumpMemory(const std::string& description) const {
        *output << "Memory dump: " << description << "\n";
        // TODO: Implement memory dump
    }
    
    void dumpStackTrace() const {
        *output << "Stack trace:\n";
        // TODO: Implement stack trace
        if (stackTraceEnabled) {
            for (const auto& trace : recentStackTraces) {
                *output << "  " << trace << "\n";
            }
        }
    }
    
    void setDebugMode(bool enabled) {
        debugMode = enabled;
    }
    
    bool isDebugMode() const {
        return debugMode;
    }
    
    void debug(const std::string& message) const {
        if (debugMode) {
            *output << "DEBUG: " << message << "\n";
        }
    }
    
    bool assert(bool condition, const std::string& message) const {
        if (!condition) {
            *output << "ASSERTION FAILED: " << message << "\n";
            dumpStackTrace();
        }
        return condition;
    }
    
    void log(const std::string& message) const {
        if (debugMode) {
            *output << "DEBUG: " << message << "\n";
        }
    }
    
    std::string getDebugInfo() const {
        std::ostringstream oss;
        oss << "Debug mode: " << (debugMode ? "enabled" : "disabled") << "\n";
        oss << "Stack trace: " << (stackTraceEnabled ? "enabled" : "disabled") << "\n";
        return oss.str();
    }
    
    void setDebugOutput(std::ostream& output) {
        this->output = &output;
    }
    
    void enableStackTrace(bool enabled) {
        stackTraceEnabled = enabled;
    }
    
    std::vector<std::string> getRecentStackTraces(size_t maxCount) const {
        if (maxCount >= recentStackTraces.size()) {
            return recentStackTraces;
        }
        
        std::vector<std::string> result;
        result.reserve(maxCount);
        size_t start = recentStackTraces.size() - maxCount;
        for (size_t i = start; i < recentStackTraces.size(); ++i) {
            result.push_back(recentStackTraces[i]);
        }
        return result;
    }
};

Debugger::Debugger() : pImpl(std::make_unique<Impl>()) {}
Debugger::~Debugger() = default;

void Debugger::setBreakpoint(const std::string& location) {
    pImpl->setBreakpoint(location);
}

void Debugger::clearBreakpoints() {
    pImpl->clearBreakpoints();
}

void Debugger::addWatchVariable(const std::string& expression) {
    pImpl->addWatchVariable(expression);
}

void Debugger::printState(const std::string& context) const {
    pImpl->printState(context);
}

void Debugger::dumpMemory(const std::string& description) const {
    pImpl->dumpMemory(description);
}

void Debugger::dumpStackTrace() const {
    pImpl->dumpStackTrace();
}

void Debugger::setDebugMode(bool enabled) {
    pImpl->setDebugMode(enabled);
}

bool Debugger::isDebugMode() const {
    return pImpl->isDebugMode();
}

void Debugger::debug(const std::string& message) const {
    pImpl->debug(message);
}

bool Debugger::assert(bool condition, const std::string& message) const {
    return pImpl->assert(condition, message);
}

void Debugger::log(const std::string& message) const {
    pImpl->log(message);
}

std::string Debugger::getDebugInfo() const {
    return pImpl->getDebugInfo();
}

void Debugger::setDebugOutput(std::ostream& output) {
    pImpl->setDebugOutput(output);
}

void Debugger::enableStackTrace(bool enabled) {
    pImpl->enableStackTrace(enabled);
}

std::vector<std::string> Debugger::getRecentStackTraces(size_t maxCount) const {
    return pImpl->getRecentStackTraces(maxCount);
}

// ProfilingSystem implementation
class ProfilingSystem::Impl {
private:
    bool initialized = false;
    Profiler profiler;
    MetricsCollector metricsCollector;
    MemoryProfiler memoryProfiler;
    CPUProfiler cpuProfiler;
    Debugger debugger;
    ProfilingConfig config;
    std::shared_ptr<Logger> logger;
    
public:
    void initialize(const std::shared_ptr<Logger>& logger) {
        this->logger = logger;
        initialized = true;
        
        if (config.enableProfiler) {
            // Profiler is always available
        }
        
        if (config.enableMetrics) {
            // Metrics collector is always available
        }
        
        if (config.enableMemoryProfiler) {
            if (config.memorySampleIntervalMs > 0) {
                memoryProfiler.startTracking(config.memorySampleIntervalMs);
            }
        }
        
        if (config.enableCPUProfiler) {
            if (config.cpuSampleIntervalS > 0) {
                cpuProfiler.startProfiling();
            }
        }
        
        if (config.enableDebugger) {
            debugger.setDebugMode(true);
        }
        
        if (config.enableStackTrace) {
            debugger.enableStackTrace(true);
        }
        
        if (logger) {
            logger->info("Profiling system initialized");
        }
    }
    
    Profiler& getProfiler() {
        return profiler;
    }
    
    MetricsCollector& getMetricsCollector() {
        return metricsCollector;
    }
    
    MemoryProfiler& getMemoryProfiler() {
        return memoryProfiler;
    }
    
    CPUProfiler& getCPUProfiler() {
        return cpuProfiler;
    }
    
    Debugger& getDebugger() {
        return debugger;
    }
    
    void startAllProfiling() {
        profiler.setActive(true);
        if (config.enableMemoryProfiler) {
            memoryProfiler.startTracking(config.memorySampleIntervalMs);
        }
        if (config.enableCPUProfiler && config.cpuSampleIntervalS > 0) {
            cpuProfiler.startProfiling();
        }
    }
    
    void stopAllProfiling() {
        profiler.setActive(false);
        memoryProfiler.stopTracking();
        cpuProfiler.stopProfiling();
    }
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "NLM Profiling System Report\n";
        oss << "==========================\n\n";
        
        oss << "Profiler:" << std::endl;
        oss << profiler.generateReport() << std::endl;
        
        oss << "Metrics: " << metricsCollector.getTotalCount() << " collected\n";
        
        oss << "Memory Profiler:" << std::endl;
        oss << memoryProfiler.generateMemoryReport() << std::endl;
        
        oss << "CPU Profiler:" << std::endl;
        oss << cpuProfiler.getProfilingResults() << std::endl;
        
        oss << "Debugger:" << std::endl;
        oss << debugger.getDebugInfo() << std::endl;
        
        return oss.str();
    }
    
    bool saveReports(const std::string& directory) const {
        if (!std::filesystem::exists(directory)) {
            std::filesystem::create_directories(directory);
        }
        
        bool success = true;
        
        // Save profiler report
        success &= profiler.saveReport(directory + "/profiler.txt");
        
        // Save metrics
        std::string csvPath = directory + "/metrics.csv";
        std::ofstream csvFile(csvPath);
        if (csvFile.is_open()) {
            csvFile << metricsCollector.exportToCSV();
        } else {
            success = false;
        }
        
        // Save memory report
        std::string memoryPath = directory + "/memory.txt";
        std::ofstream memoryFile(memoryPath);
        if (memoryFile.is_open()) {
            memoryFile << memoryProfiler.generateMemoryReport();
        } else {
            success = false;
        }
        
        // Save CPU report
        std::string cpuPath = directory + "/cpu.txt";
        std::ofstream cpuFile(cpuPath);
        if (cpuFile.is_open()) {
            cpuFile << cpuProfiler.generateCPUReport();
        } else {
            success = false;
        }
        
        return success;
    }
    
    std::string exportData() const {
        std::ostringstream oss;
        oss << "{\n";
        oss << "  \"profiler\": " << profiler.generateReport() << ",\n";
        oss << "  \"metrics\": \"" << metricsCollector.exportToCSV() << "\",\n";
        oss << "  \"memory\": \"" << memoryProfiler.generateMemoryReport() << "\",\n";
        oss << "  \"cpu\": \"" << cpuProfiler.getProfilingResults() << "\",\n";
        oss << "  \"debug\": \"" << debugger.getDebugInfo() << "\"\n";
        oss << "}\n";
        return oss.str();
    }
    
    void cleanup() {
        stopAllProfiling();
        metricsCollector.clear();
        memoryProfiler.cleanup();
        cpuProfiler.stopProfiling();
        
        if (logger) {
            logger->info("Profiling system cleaned up");
        }
    }
    
    bool isInitialized() const {
        return initialized;
    }
    
    void configure(const ProfilingConfig& config) {
        this->config = config;
        if (initialized) {
            initialize(logger);
        }
    }
};

ProfilingSystem::ProfilingSystem() : pImpl(std::make_unique<Impl>()) {}
ProfilingSystem::~ProfilingSystem() = default;

void ProfilingSystem::initialize(const std::shared_ptr<Logger>& logger) {
    pImpl->initialize(logger);
}

Profiler& ProfilingSystem::getProfiler() {
    return pImpl->getProfiler();
}

MetricsCollector& ProfilingSystem::getMetricsCollector() {
    return pImpl->getMetricsCollector();
}

MemoryProfiler& ProfilingSystem::getMemoryProfiler() {
    return pImpl->getMemoryProfiler();
}

CPUProfiler& ProfilingSystem::getCPUProfiler() {
    return pImpl->getCPUProfiler();
}

Debugger& ProfilingSystem::getDebugger() {
    return pImpl->getDebugger();
}

void ProfilingSystem::startAllProfiling() {
    pImpl->startAllProfiling();
}

void ProfilingSystem::stopAllProfiling() {
    pImpl->stopAllProfiling();
}

std::string ProfilingSystem::generateReport() const {
    return pImpl->generateReport();
}

bool ProfilingSystem::saveReports(const std::string& directory) const {
    return pImpl->saveReports(directory);
}

std::string ProfilingSystem::exportData() const {
    return pImpl->exportData();
}

void ProfilingSystem::cleanup() {
    pImpl->cleanup();
}

bool ProfilingSystem::isInitialized() const {
    return pImpl->isInitialized();
}

std::string ProfilingSystem::getVersion() {
    return "1.0.0";
}

ProfilingSystem& ProfilingSystem::getInstance() {
    static ProfilingSystem instance;
    return instance;
}

void ProfilingSystem::configure(const ProfilingConfig& config) {
    pImpl->configure(config);
}

namespace Profiling {
    
    void timeFunction(const std::string& name, std::function<void()> func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        ProfilingSystem::getInstance().getProfiler().start(name);
        ProfilingSystem::getInstance().getProfiler().stop(name);
        
        NLM_LOG_DEBUG("Function '" + name + "': " + std::to_string(duration) + "μs");
    }
    
    void trackMemory(const std::string& label, size_t size) {
        ProfilingSystem::getInstance().getMemoryProfiler().trackAllocation(nullptr, size, label);
    }
    
    bool assertPerformance(const std::string& testName, double maxTimeSeconds,
                          std::function<void()> testFunc) {
        auto start = std::chrono::high_resolution_clock::now();
        testFunc();
        auto end = std::chrono::high_resolution_clock::now();
        
        double elapsed = std::chrono::duration<double>(end - start).count();
        
        if (elapsed > maxTimeSeconds) {
            NLM_LOG_WARNING("Performance assertion failed: '" + testName + "' took " + 
                           std::to_string(elapsed) + "s (max: " + std::to_string(maxTimeSeconds) + "s)");
            return false;
        }
        
        return true;
    }
    
    BenchmarkResult benchmark(const std::string& name, std::function<void()> func,
                             size_t iterations) {
        BenchmarkResult result;
        result.name = name;
        result.iterations = iterations;
        
        for (size_t i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            func();
            auto end = std::chrono::high_resolution_clock::now();
            
            double elapsed = std::chrono::duration<double>(end - start).count();
            
            if (i == 0) {
                result.minTime = elapsed;
                result.maxTime = elapsed;
            } else {
                result.minTime = std::min(result.minTime, elapsed);
                result.maxTime = std::max(result.maxTime, elapsed);
            }
            
            result.timeSeconds += elapsed;
        }
        
        result.averageTime = result.timeSeconds / iterations;
        result.stddev = std::sqrt((result.timeSeconds - iterations * result.averageTime * result.averageTime) / (iterations - 1));
        
        return result;
    }
    
    std::string generateReport() {
        return ProfilingSystem::getInstance().generateReport();
    }
    
    bool exportResults(const std::string& filepath) {
        return ProfilingSystem::getInstance().saveReports(filepath);
    }
    
    bool isProfilingActive() {
        return ProfilingSystem::getInstance().getProfiler().isActive();
    }
    
    bool isMetricsEnabled() {
        return ProfilingSystem::getInstance().getMetricsCollector().getTotalCount() > 0;
    }
    
    bool isDebuggingEnabled() {
        return ProfilingSystem::getInstance().getDebugger().isDebugMode();
    }
}

} // namespace nlm