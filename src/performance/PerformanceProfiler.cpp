// Performance Profiler Implementation

#include "PerformanceProfiler.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct PerformanceProfiler::Impl {
    // Core performance tracking
    bool running_ = false;
    std::chrono::high_resolution_clock::time_point startTime_;
    double totalSimulationTime_ = 0.0;
    uint64_t totalSteps_ = 0;
    
    // Metric storage
    std::unordered_map<std::string, std::vector<double>> metrics_;
    std::unordered_map<std::string, MetricType> metricTypes_;
    
    // Component profiling
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> componentStarts_;
    std::unordered_map<std::string, double> componentTimes_;
    std::unordered_map<std::string, uint64_t> componentCounts_;
    
    // Time series data
    std::vector<PerformanceSample> samples_;
    size_t sampleWindowSize_ = 100;
    
    // Memory profiling
    struct MemorySample {
        std::chrono::high_resolution_clock::time_point timestamp;
        size_t totalBytes;
        size_t usedBytes;
        std::unordered_map<std::string, size_t> allocations;
        std::unordered_map<std::string, size_t> deallocations;
    };
    std::vector<MemorySample> memorySamples_;
    
    // CPU profiling
    struct CPUSample {
        std::chrono::high_resolution_clock::time_point timestamp;
        double totalUsagePercent;
        double systemIdle;
        std::vector<double> coreUsages;
    };
    std::vector<CPUSample> cpuSamples_;
    
    // Simulation time tracking
    struct SimulationSample {
        std::chrono::high_resolution_clock::time_point timestamp;
        double realTime;
        double simulatedTime;
        double speedFactor;
        uint64_t steps;
        size_t neurons;
        size_t synapses;
    };
    std::vector<SimulationSample> simulationSamples_;
    
    // Feature impact tracking
    std::unordered_map<std::string, std::vector<double>> featureTimes_;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> featureStarts_;
    
    // Threshold management
    std::vector<PerformanceThreshold> thresholds_;
    std::vector<std::string> activeAlerts_;
    
    // Real-time monitoring
    bool realTimeMonitoring_ = false;
    std::chrono::high_resolution_clock::time_point realTimeStart_;
    std::chrono::duration<double> realTimeInterval_ = std::chrono::duration<double>(1.0);
    std::thread realTimeThread_;
    std::mutex realTimeMutex_;
    std::condition_variable realTimeCV_;
    
    // Configuration
    ProfilingLevel profilingLevel_ = ProfilingLevel::DETAILED;
    bool recordMemory_ = true;
    bool recordCPU_ = true;
    bool recordSimulationTime_ = true;
};

PerformanceProfiler::PerformanceProfiler() : pImpl_(std::make_unique<Impl>()) {
    // Initialize default metric types
    pImpl_->metricTypes_["step_time"] = MetricType::TIMER;
    pImpl_->metricTypes_["cpu_usage"] = MetricType::GAUGE;
    pImpl_->metricTypes_["memory_usage"] = MetricType::GAUGE;
    pImpl_->metricTypes_["spikes_per_step"] = MetricType::GAUGE;
    pImpl_->metricTypes_["neurons_per_second"] = MetricType::GAUGE;
}

PerformanceProfiler::~PerformanceProfiler() {
    stopRealTimeMonitoring();
}

void PerformanceProfiler::start() {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    if (pImpl_->running_) return;
    
    pImpl_->running_ = true;
    pImpl_->startTime_ = std::chrono::high_resolution_clock::now();
    pImpl_->totalSteps_ = 0;
    
    if (pImpl_->recordMemory_) {
        pImpl_->memorySamples_.clear();
    }
    if (pImpl_->recordCPU_) {
        pImpl_->cpuSamples_.clear();
    }
    if (pImpl_->recordSimulationTime_) {
        pImpl_->simulationSamples_.clear();
    }
    
    startRealTimeMonitoring();
}

void PerformanceProfiler::stop() {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    if (!pImpl_->running_) return;
    
    stopRealTimeMonitoring();
    pImpl_->running_ = false;
    
    // Record final sample
    PerformanceSample sample;
    sample.timestamp = std::chrono::high_resolution_clock::now();
    for (const auto& pair : pImpl_->metrics_) {
        sample.metrics[pair.first] = getMetricMean(pair.first);
    }
    pImpl_->samples_.push_back(sample);
}

void PerformanceProfiler::recordMetric(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    // Store the raw value
    pImpl_->metrics_[name].push_back(value);
    
    // Record sample if recording is enabled
    if (pImpl_->running_ && pImpl_->samples_.size() < pImpl_->sampleWindowSize_) {
        PerformanceSample sample;
        sample.timestamp = std::chrono::high_resolution_clock::now();
        for (const auto& pair : pImpl_->metrics_) {
            if (!pair.second.empty()) {
                sample.metrics[pair.first] = pair.second.back();
            }
        }
        pImpl_->samples_.push_back(sample);
    }
}

void PerformanceProfiler::recordTimer(const std::string& name, double seconds) {
    recordMetric(name, seconds);
    
    // Track component times if component profiling is active
    if (pImpl_->componentStarts_.find(name) != pImpl_->componentStarts_.end()) {
        auto& start = pImpl_->componentStarts_[name];
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();
        pImpl_->componentTimes_[name] += duration;
        pImpl_->componentCounts_[name]++;
        pImpl_->componentStarts_.erase(name);
    }
}

void PerformanceProfiler::incrementCounter(const std::string& name, double delta) {
    recordMetric(name, delta);
}

void PerformanceProfiler::setGauge(const std::string& name, double value) {
    recordMetric(name, value);
}

void PerformanceProfiler::recordMemoryUsage() {
    if (!pImpl_->recordMemory_) return;
    
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    Impl::MemorySample sample;
    sample.timestamp = std::chrono::high_resolution_clock::now();
    
    // Get memory information (placeholder - would need actual memory monitoring implementation)
    sample.totalBytes = 8192 * 1024 * 1024;  // 8GB placeholder
    sample.usedBytes = 2048 * 1024 * 1024;  // 2GB used placeholder
    sample.allocations = pImpl_->memorySamples_.empty() ? 
        std::unordered_map<std::string, size_t>{{ "current", sample.usedBytes }} :
        pImpl_->memorySamples_.back().allocations;
    
    pImpl_->memorySamples_.push_back(sample);
    recordMetric("memory_usage", static_cast<double>(sample.usedBytes));
}

void PerformanceProfiler::recordMemoryAllocation(const std::string& component, size_t bytes) {
    if (!pImpl_->recordMemory_) return;
    
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    if (!pImpl_->memorySamples_.empty()) {
        pImpl_->memorySamples_.back().allocations[component] += bytes;
    }
}

void PerformanceProfiler::recordMemoryDeallocation(const std::string& component, size_t bytes) {
    if (!pImpl_->recordMemory_) return;
    
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    if (!pImpl_->memorySamples_.empty()) {
        auto& allocations = pImpl_->memorySamples_.back().allocations;
        if (allocations.find(component) != allocations.end()) {
            allocations[component] -= bytes;
            if (allocations[component] < 0) allocations[component] = 0;
        }
    }
}

void PerformanceProfiler::recordCPUUsage(double usagePercent) {
    if (!pImpl_->recordCPU_) return;
    
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    Impl::CPUSample sample;
    sample.timestamp = std::chrono::high_resolution_clock::now();
    sample.totalUsagePercent = usagePercent;
    sample.systemIdle = 100.0 - usagePercent;
    
    // Generate some sample core usages (would be actual implementation)
    sample.coreUsages = std::vector<double>(8, usagePercent / 8.0);
    
    pImpl_->cpuSamples_.push_back(sample);
    recordMetric("cpu_usage", usagePercent);
}

void PerformanceProfiler::recordThreadUsage(const std::string& threadName, double usagePercent) {
    recordMetric("thread_usage_" + threadName, usagePercent);
}

void PerformanceProfiler::recordStepTime(double stepTimeSeconds) {
    recordTimer("step_time", stepTimeSeconds);
    pImpl_->totalSteps_++;
    
    // Record simulation sample
    if (pImpl_->recordSimulationTime_) {
        Impl::SimulationSample sample;
        sample.timestamp = std::chrono::high_resolution_clock::now();
        sample.realTime = std::chrono::duration<double>(sample.timestamp - pImpl_->startTime_).count();
        sample.simulatedTime = pImpl_->totalSteps_ * 0.001;  // Assuming 1ms per step
        sample.speedFactor = sample.simulatedTime / sample.realTime;
        sample.steps = pImpl_->totalSteps_;
        
        pImpl_->simulationSamples_.push_back(sample);
        recordMetric("speed_factor", sample.speedFactor);
    }
}

void PerformanceProfiler::recordSimulationTime(double realTime, double simulatedTime) {
    double speedFactor = simulatedTime / realTime;
    recordMetric("speed_factor", speedFactor);
    
    // Record simulation sample
    if (pImpl_->recordSimulationTime_) {
        Impl::SimulationSample sample;
        sample.timestamp = std::chrono::high_resolution_clock::now();
        sample.realTime = realTime;
        sample.simulatedTime = simulatedTime;
        sample.speedFactor = speedFactor;
        sample.steps = pImpl_->totalSteps_;
        
        pImpl_->simulationSamples_.push_back(sample);
    }
}

void PerformanceProfiler::startComponentProfile(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    pImpl_->componentStarts_[componentName] = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::stopComponentProfile(const std::string& componentName) {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    auto it = pImpl_->componentStarts_.find(componentName);
    if (it != pImpl_->componentStarts_.end()) {
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - it->second).count();
        pImpl_->componentTimes_[componentName] += duration;
        pImpl_->componentCounts_[componentName]++;
        pImpl_->componentStarts_.erase(it);
        
        recordTimer(componentName + "_time", duration);
    }
}

void PerformanceProfiler::startStepProfiling() {
    pImpl_->stepStartTime_ = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::endStepProfiling() {
    auto end = std::chrono::high_resolution_clock::now();
    double stepTime = std::chrono::duration<double>(end - pImpl_->stepStartTime_).count();
    recordStepTime(stepTime);
}

std::vector<PerformanceSample> PerformanceProfiler::getSamples(const std::string& component) const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    if (component.empty()) {
        return pImpl_->samples_;
    }
    
    std::vector<PerformanceSample> filteredSamples;
    for (const auto& sample : pImpl_->samples_) {
        if (sample.metrics.find(component) != sample.metrics.end()) {
            filteredSamples.push_back(sample);
        }
    }
    return filteredSamples;
}

PerformanceSample PerformanceProfiler::getCurrentSample() const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    PerformanceSample sample;
    sample.timestamp = std::chrono::high_resolution_clock::now();
    for (const auto& pair : pImpl_->metrics_) {
        if (!pair.second.empty()) {
            sample.metrics[pair.first] = pair.second.back();
        }
    }
    return sample;
}

double PerformanceProfiler::getMetricMean(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    auto it = pImpl_->metrics_.find(name);
    if (it == pImpl_->metrics_.end() || it->second.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
    return sum / it->second.size();
}

double PerformanceProfiler::getMetricStdDev(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    auto it = pImpl_->metrics_.find(name);
    if (it == pImpl_->metrics_.end() || it->second.size() < 2) {
        return 0.0;
    }
    
    double mean = getMetricMean(name);
    double sumSquares = 0.0;
    for (double v : it->second) {
        double diff = v - mean;
        sumSquares += diff * diff;
    }
    return std::sqrt(sumSquares / (it->second.size() - 1));
}

double PerformanceProfiler::getMetricMin(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    auto it = pImpl_->metrics_.find(name);
    if (it == pImpl_->metrics_.end() || it->second.empty()) {
        return 0.0;
    }
    
    return *std::min_element(it->second.begin(), it->second.end());
}

double PerformanceProfiler::getMetricMax(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    auto it = pImpl_->metrics_.find(name);
    if (it == pImpl_->metrics_.end() || it->second.empty()) {
        return 0.0;
    }
    
    return *std::max_element(it->second.begin(), it->second.end());
}

std::vector<double> PerformanceProfiler::getMetricValues(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    auto it = pImpl_->metrics_.find(name);
    if (it == pImpl_->metrics_.end()) {
        return std::vector<double>();
    }
    return it->second;
}

std::string PerformanceProfiler::analyzePerformanceBottlenecks() const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    std::ostringstream oss;
    oss << "=== Performance Bottleneck Analysis ===\n\n";
    
    // Analyze step times
    if (pImpl_->metrics_.find("step_time") != pImpl_->metrics_.end()) {
        double avgStepTime = getMetricMean("step_time");
        double maxStepTime = getMetricMax("step_time");
        double minStepTime = getMetricMin("step_time");
        
        oss << "Step Time Analysis:\n";
        oss << "  Average: " << std::fixed << std::setprecision(6) << avgStepTime << "s\n";
        oss << "  Maximum: " << std::fixed << std::setprecision(6) << maxStepTime << "s\n";
        oss << "  Minimum: " << std::fixed << std::setprecision(6) << minStepTime << "s\n";
        
        if (maxStepTime > avgStepTime * 10) {
            oss << "  ⚠️  WARNING: High step time variance detected!\n";
        }
        oss << "\n";
    }
    
    // Analyze memory usage
    if (pImpl_->metrics_.find("memory_usage") != pImpl_->metrics_.end()) {
        double avgMemory = getMetricMean("memory_usage");
        oss << "Memory Usage:\n";
        oss << "  Average: " << std::fixed << std::setprecision(2) << avgMemory / (1024*1024) << " MB\n";
        oss << "\n";
    }
    
    // Analyze CPU usage
    if (pImpl_->metrics_.find("cpu_usage") != pImpl_->metrics_.end()) {
        double avgCPU = getMetricMean("cpu_usage");
        oss << "CPU Usage:\n";
        oss << "  Average: " << std::fixed << std::setprecision(2) << avgCPU << "%\n";
        
        if (avgCPU > 80.0) {
            oss << "  ⚠️  WARNING: High CPU usage detected!\n";
        }
        oss << "\n";
    }
    
    // Analyze component performance
    oss << "Component Performance:\n";
    for (const auto& pair : pImpl_->componentTimes_) {
        double avgTime = pair.second / pImpl_->componentCounts_.count(pair.first) ?
                         pImpl_->componentTimes_[pair.first] / pImpl_->componentCounts_[pair.first] : 0.0;
        oss << "  " << pair.first << ": " << std::fixed << std::setprecision(6) 
            << avgTime << "s (" << pImpl_->componentCounts_.at(pair.first) << " calls)\n";
    }
    
    return oss.str();
}

std::string PerformanceProfiler::generatePerformanceReport() const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    std::ostringstream oss;
    oss << "=== NLM Performance Report ===\n\n";
    oss << "Total Steps: " << pImpl_->totalSteps_ << "\n";
    oss << "Total Simulation Time: " << std::fixed << std::setprecision(2) << pImpl_->totalSimulationTime_ << "s\n";
    oss << "Number of Samples: " << pImpl_->samples_.size() << "\n\n";
    
    // Metric summary
    oss << "=== Metric Summary ===\n";
    for (const auto& pair : pImpl_->metrics_) {
        if (!pair.second.empty()) {
            double mean = getMetricMean(pair.first);
            double stddev = getMetricStdDev(pair.first);
            double min = getMetricMin(pair.first);
            double max = getMetricMax(pair.first);
            
            oss << pair.first << ": ";
            oss << "mean=" << std::fixed << std::setprecision(6) << mean << ", ";
            oss << "stddev=" << std::fixed << std::setprecision(6) << stddev << ", ";
            oss << "min=" << std::fixed << std::setprecision(6) << min << ", ";
            oss << "max=" << std::fixed << std::setprecision(6) << max << "\n";
        }
    }
    
    oss << "\n";
    
    // Component summary
    oss << "=== Component Summary ===\n";
    for (const auto& pair : pImpl_->componentTimes_) {
        double totalTime = pair.second;
        uint64_t count = pImpl_->componentCounts_.at(pair.first);
        double avgTime = totalTime / count;
        double percentage = (totalTime / pImpl_->totalSimulationTime_) * 100.0;
        
        oss << pair.first << ": " << std::fixed << std::setprecision(2) << percentage << "% "
            << "(" << avgTime << "s per call, " << count << " calls)\n";
    }
    
    return oss.str();
}

void PerformanceProfiler::addThreshold(const PerformanceThreshold& threshold) {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    pImpl_->thresholds_.push_back(threshold);
    updateThresholds();
}

bool PerformanceProfiler::checkThresholds() const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    bool hasActiveAlerts = false;
    
    for (const auto& threshold : pImpl_->thresholds_) {
        double currentValue = getCurrentMetric(threshold.name);
        
        if (currentValue >= threshold.criticalThreshold) {
            pImpl_->activeAlerts_.push_back(
                "CRITICAL: " + threshold.name + " = " + std::to_string(currentValue) +
                " (threshold: " + std::to_string(threshold.criticalThreshold) + ")"
            );
            hasActiveAlerts = true;
        } else if (currentValue >= threshold.errorThreshold) {
            pImpl_->activeAlerts_.push_back(
                "ERROR: " + threshold.name + " = " + std::to_string(currentValue) +
                " (threshold: " + std::to_string(threshold.errorThreshold) + ")"
            );
            hasActiveAlerts = true;
        } else if (currentValue >= threshold.warningThreshold) {
            pImpl_->activeAlerts_.push_back(
                "WARNING: " + threshold.name + " = " + std::to_string(currentValue) +
                " (threshold: " + std::to_string(threshold.warningThreshold) + ")"
            );
            hasActiveAlerts = true;
        }
    }
    
    return hasActiveAlerts;
}

std::vector<std::string> PerformanceProfiler::getActiveAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    return pImpl_->activeAlerts_;
}

bool PerformanceProfiler::saveToCSV(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "timestamp";
    for (const auto& pair : pImpl_->metrics_) {
        file << "," << pair.first;
    }
    file << "\n";
    
    // Write data
    for (const auto& sample : pImpl_->samples_) {
        file << std::chrono::duration<double>(sample.timestamp - pImpl_->startTime_).count();
        for (const auto& pair : pImpl_->metrics_) {
            file << ",";
            if (!pair.second.empty()) {
                file << pair.second.back();
            }
        }
        file << "\n";
    }
    
    file.close();
    return true;
}

bool PerformanceProfiler::saveToJSON(const std::string& filename) const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    nlohmann::json j;
    
    // Add metrics
    j["metrics"] = nlohmann::json::object();
    for (const auto& pair : pImpl_->metrics_) {
        j["metrics"][pair.first] = pair.second;
    }
    
    // Add component times
    j["component_times"] = pImpl_->componentTimes_;
    
    // Add samples
    j["samples"] = nlohmann::json::array();
    for (const auto& sample : pImpl_->samples_) {
        nlohmann::json sampleJson;
        sampleJson["timestamp"] = std::chrono::duration<double>(sample.timestamp - pImpl_->startTime_).count();
        sampleJson["metrics"] = sample.metrics;
        j["samples"].push_back(sampleJson);
    }
    
    // Add thresholds
    j["thresholds"] = nlohmann::json::array();
    for (const auto& threshold : pImpl_->thresholds_) {
        nlohmann::json tJson;
        tJson["name"] = threshold.name;
        tJson["type"] = static_cast<int>(threshold.metricType);
        tJson["alert_level"] = static_cast<int>(threshold.alertLevel);
        tJson["warning_threshold"] = threshold.warningThreshold;
        tJson["error_threshold"] = threshold.errorThreshold;
        tJson["critical_threshold"] = threshold.criticalThreshold;
        tJson["description"] = threshold.description;
        j["thresholds"].push_back(tJson);
    }
    
    // Add active alerts
    j["active_alerts"] = pImpl_->activeAlerts_;
    
    // Write to file
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << j.dump(2);
    file.close();
    return true;
}

bool PerformanceProfiler::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json j;
    file >> j;
    file.close();
    
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    // Clear existing data
    pImpl_->metrics_.clear();
    pImpl_->componentTimes_.clear();
    pImpl_->componentCounts_.clear();
    pImpl_->samples_.clear();
    pImpl_->activeAlerts_.clear();
    
    // Load metrics
    if (j.contains("metrics")) {
        for (const auto& item : j["metrics"].items()) {
            pImpl_->metrics_[item.key()] = item.value().get<std::vector<double>>();
        }
    }
    
    // Load component times
    if (j.contains("component_times")) {
        pImpl_->componentTimes_ = j["component_times"].get<std::unordered_map<std::string, double>>();
    }
    
    // Load samples
    if (j.contains("samples")) {
        for (const auto& sampleJson : j["samples"]) {
            PerformanceSample sample;
            sample.timestamp = pImpl_->startTime_ + std::chrono::duration<double>(sampleJson["timestamp"]);
            
            if (sampleJson.contains("metrics")) {
                sample.metrics = sampleJson["metrics"].get<std::unordered_map<std::string, double>>();
            }
            
            pImpl_->samples_.push_back(sample);
        }
    }
    
    // Load thresholds
    if (j.contains("thresholds")) {
        pImpl_->thresholds_.clear();
        for (const auto& tJson : j["thresholds"]) {
            PerformanceThreshold threshold;
            threshold.name = tJson["name"];
            threshold.metricType = static_cast<MetricType>(tJson["type"]);
            threshold.alertLevel = static_cast<AlertLevel>(tJson["alert_level"]);
            threshold.warningThreshold = tJson["warning_threshold"];
            threshold.errorThreshold = tJson["error_threshold"];
            threshold.criticalThreshold = tJson["critical_threshold"];
            threshold.description = tJson["description"];
            pImpl_->thresholds_.push_back(threshold);
        }
    }
    
    // Load active alerts
    if (j.contains("active_alerts")) {
        pImpl_->activeAlerts_ = j["active_alerts"].get<std::vector<std::string>>();
    }
    
    return true;
}

void PerformanceProfiler::startFeatureProfiling(const std::string& featureName) {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    pImpl_->featureStarts_[featureName] = std::chrono::high_resolution_clock::now();
    pImpl_->featureTimes_[featureName] = 0.0;
}

void PerformanceProfiler::endFeatureProfiling(const std::string& featureName) {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    auto it = pImpl_->featureStarts_.find(featureName);
    if (it != pImpl_->featureStarts_.end()) {
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - it->second).count();
        pImpl_->featureTimes_[featureName] += duration;
        pImpl_->featureStarts_.erase(it);
    }
}

std::string PerformanceProfiler::compareFeaturePerformance(const std::string& featureName1, const std::string& featureName2) const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    std::ostringstream oss;
    oss << "=== Feature Performance Comparison ===\n\n";
    oss << "Feature: " << featureName1 << " vs " << featureName2 << "\n\n";
    
    if (pImpl_->featureTimes_.find(featureName1) != pImpl_->featureTimes_.end() &&
        pImpl_->featureTimes_.find(featureName2) != pImpl_->featureTimes_.end()) {
        
        double time1 = pImpl_->featureTimes_[featureName1];
        double time2 = pImpl_->featureTimes_[featureName2];
        
        oss << featureName1 << ": " << std::fixed << std::setprecision(6) << time1 << "s\n";
        oss << featureName2 << ": " << std::fixed << std::setprecision(6) << time2 << "s\n\n";
        
        if (time2 > 0.0) {
            double improvement = ((time1 - time2) / time2) * 100.0;
            oss << "Improvement: " << std::fixed << std::setprecision(2) << improvement << "%\n";
        }
    }
    
    return oss.str();
}

bool PerformanceProfiler::detectRegression(const std::string& baselineFile, double regressionThreshold) const {
    // Load baseline data
    PerformanceProfiler baselineProfiler;
    if (!baselineProfiler.loadFromFile(baselineFile)) {
        return false;
    }
    
    // Compare metrics
    bool hasRegression = false;
    
    for (const auto& pair : pImpl_->metrics_) {
        if (baselineProfiler.getMetricValues(pair.first).empty()) continue;
        
        double currentMean = getMetricMean(pair.first);
        double baselineMean = baselineProfiler.getMetricMean(pair.first);
        
        if (baselineMean > 0.0) {
            double regression = (currentMean - baselineMean) / baselineMean;
            if (regression > regressionThreshold) {
                hasRegression = true;
            }
        }
    }
    
    return hasRegression;
}

std::string PerformanceProfiler::generateRegressionReport(const std::string& baselineFile) const {
    std::ostringstream oss;
    oss << "=== Performance Regression Report ===\n\n";
    
    if (!detectRegression(baselineFile, 0.1)) {
        oss << "No significant performance regressions detected.\n";
    } else {
        oss << "Performance regressions detected compared to baseline.\n";
        oss << "Review the detailed comparison above.\n";
    }
    
    return oss.str();
}

BenchmarkMetrics PerformanceProfiler::convertToBenchmarkMetrics() const {
    BenchmarkMetrics metrics;
    
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    // Convert key metrics to benchmark metrics
    if (pImpl_->metrics_.find("step_time") != pImpl_->metrics_.end()) {
        metrics.avgStepTimeMs = getMetricMean("step_time") * 1000.0;
        metrics.minStepTimeMs = getMetricMin("step_time") * 1000.0;
        metrics.maxStepTimeMs = getMetricMax("step_time") * 1000.0;
        metrics.stepTimeStdDev = getMetricStdDev("step_time") * 1000.0;
    }
    
    if (pImpl_->metrics_.find("cpu_usage") != pImpl_->metrics_.end()) {
        metrics.cpuUtilizationPercent = getMetricMean("cpu_usage");
    }
    
    if (pImpl_->metrics_.find("memory_usage") != pImpl_->metrics_.end()) {
        metrics.totalMemoryMB = static_cast<size_t>(getMetricMean("memory_usage") / (1024 * 1024));
    }
    
    return metrics;
}

void PerformanceProfiler::startRealTimeMonitoring(double sampleIntervalSeconds) {
    stopRealTimeMonitoring();
    
    pImpl_->realTimeMonitoring_ = true;
    pImpl_->realTimeInterval_ = std::chrono::duration<double>(sampleIntervalSeconds);
    pImpl_->realTimeStart_ = std::chrono::high_resolution_clock::now();
    
    pImpl_->realTimeThread_ = std::thread([this]() {
        while (pImpl_->realTimeMonitoring_) {
            std::unique_lock<std::mutex> lock(pImpl_->realTimeMutex_);
            
            // Record samples
            PerformanceSample sample;
            sample.timestamp = std::chrono::high_resolution_clock::now();
            for (const auto& pair : pImpl_->metrics_) {
                if (!pair.second.empty()) {
                    sample.metrics[pair.first] = pair.second.back();
                }
            }
            pImpl_->samples_.push_back(sample);
            
            // Limit sample size
            if (pImpl_->samples_.size() > pImpl_->sampleWindowSize_) {
                pImpl_->samples_.erase(pImpl_->samples_.begin());
            }
            
            // Check thresholds
            updateThresholds();
            
            lock.unlock();
            std::this_thread::sleep_for(pImpl_->realTimeInterval_);
        }
    });
}

void PerformanceProfiler::stopRealTimeMonitoring() {
    if (pImpl_->realTimeMonitoring_) {
        pImpl_->realTimeMonitoring_ = false;
        pImpl_->realTimeCV_.notify_all();
        
        if (pImpl_->realTimeThread_.joinable()) {
            pImpl_->realTimeThread_.join();
        }
    }
}

BenchmarkMetrics PerformanceProfiler::getCurrentRealTimeMetrics() const {
    BenchmarkMetrics metrics;
    
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    // Get latest metrics
    if (!pImpl_->samples_.empty()) {
        const auto& latestSample = pImpl_->samples_.back();
        
        for (const auto& pair : latestSample.metrics) {
            if (pair.first == "step_time") {
                metrics.avgStepTimeMs = pair.second * 1000.0;
            } else if (pair.first == "cpu_usage") {
                metrics.cpuUtilizationPercent = pair.second;
            } else if (pair.first == "memory_usage") {
                metrics.totalMemoryMB = static_cast<size_t>(pair.second / (1024 * 1024));
            } else if (pair.first == "spikes_per_step") {
                metrics.avgFiringRateHz = pair.second;
            }
        }
    }
    
    return metrics;
}

std::string PerformanceProfiler::compareProfiles(const PerformanceProfiler& otherProfiler) const {
    std::lock_guard<std::mutex> lock1(pImpl_->realTimeMutex_);
    std::lock_guard<std::mutex> lock2(otherProfiler.pImpl_->realTimeMutex_);
    
    std::ostringstream oss;
    oss << "=== Performance Profile Comparison ===\n\n";
    
    // Compare key metrics
    std::vector<std::string> metricNames;
    for (const auto& pair : pImpl_->metrics_) {
        metricNames.push_back(pair.first);
    }
    
    for (const auto& name : metricNames) {
        if (pImpl_->metrics_.find(name) != pImpl_->metrics_.end() &&
            otherProfiler.pImpl_->metrics_.find(name) != otherProfiler.pImpl_->metrics_.end()) {
            
            double currentMean = getMetricMean(name);
            double otherMean = otherProfiler.getMetricMean(name);
            
            if (currentMean > 0.0 && otherMean > 0.0) {
                double difference = currentMean - otherMean;
                double percentage = (difference / otherMean) * 100.0;
                
                oss << name << ": " << std::fixed << std::setprecision(6) << currentMean << " vs "
                    << otherMean << " (" << std::setprecision(2) << percentage << "% "
                    << (difference > 0 ? "slower" : "faster") << ")\n";
            }
        }
    }
    
    return oss.str();
}

void PerformanceProfiler::updateThresholds() {
    pImpl_->activeAlerts_.clear();
    checkThresholds();
}

double PerformanceProfiler::getCurrentMetric(const std::string& name) const {
    auto it = pImpl_->metrics_.find(name);
    if (it == pImpl_->metrics_.end() || it->second.empty()) {
        return 0.0;
    }
    return it->second.back();
}

// CPUMonitor implementation
struct CPUMonitor::Impl {
    bool running_ = false;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<CPUMetrics> history_;
    bool throttling_ = false;
    int currentFrequencyMHz_ = 0;
};

CPUMonitor::CPUMonitor() : pImpl_(std::make_unique<Impl>()) {}
CPUMonitor::~CPUMonitor() {
    stop();
}

void CPUMonitor::start() {
    if (pImpl_->running_) return;
    
    pImpl_->running_ = true;
    pImpl_->startTime_ = std::chrono::high_resolution_clock::now();
    pImpl_->history_.clear();
}

void CPUMonitor::stop() {
    if (!pImpl_->running_) return;
    
    pImpl_->running_ = false;
}

CPUMonitor::CPUMetrics CPUMonitor::getCurrentMetrics() const {
    CPUMetrics metrics;
    metrics.timestamp = std::chrono::high_resolution_clock::now();
    
    // Placeholder implementation - would need actual CPU monitoring
    metrics.totalUsagePercent = 50.0;  // 50% placeholder
    metrics.systemIdle = 50.0;
    metrics.userTime = 30.0;
    metrics.systemTime = 20.0;
    metrics.coreUsages = std::vector<double>(8, 50.0 / 8.0);
    
    return metrics;
}

std::vector<CPUMonitor::CPUMetrics> CPUMonitor::getHistory() const {
    return pImpl_->history_;
}

void CPUMonitor::onCPUThrottling(bool isThrottled) {
    pImpl_->throttling_ = isThrottled;
}

void CPUMonitor::onFrequencyChange(int newFrequencyMHz) {
    pImpl_->currentFrequencyMHz_ = newFrequencyMHz;
}

// MemoryMonitor implementation
struct MemoryMonitor::Impl {
    bool running_ = false;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<MemoryMetrics> history_;
    std::unordered_map<std::string, size_t> allocations_;
    std::unordered_map<std::string, size_t> deallocations_;
    size_t totalAllocated_ = 0;
    size_t totalDeallocated_ = 0;
};

MemoryMonitor::MemoryMonitor() : pImpl_(std::make_unique<Impl>()) {}
MemoryMonitor::~MemoryMonitor() {
    stop();
}

void MemoryMonitor::start() {
    if (pImpl_->running_) return;
    
    pImpl_->running_ = true;
    pImpl_->startTime_ = std::chrono::high_resolution_clock::now();
    pImpl_->history_.clear();
    pImpl_->allocations_.clear();
    pImpl_->deallocations_.clear();
}

void MemoryMonitor::stop() {
    if (!pImpl_->running_) return;
    
    pImpl_->running_ = false;
}

MemoryMonitor::MemoryMetrics MemoryMonitor::getCurrentMetrics() const {
    MemoryMetrics metrics;
    metrics.timestamp = std::chrono::high_resolution_clock::now();
    
    // Placeholder implementation - would need actual memory monitoring
    metrics.totalBytes = 8 * 1024 * 1024 * 1024;  // 8GB total
    metrics.usedBytes = 2 * 1024 * 1024 * 1024;  // 2GB used
    metrics.availableBytes = 6 * 1024 * 1024 * 1024;  // 6GB available
    metrics.residentSetSize = 1.5 * 1024 * 1024 * 1024;  // 1.5GB RSS
    metrics.virtualMemorySize = 3 * 1024 * 1024 * 1024;  // 3GB VMS
    metrics.allocationBreakdown = pImpl_->allocations_;
    
    return metrics;
}

std::vector<MemoryMonitor::MemoryMetrics> MemoryMonitor::getHistory() const {
    return pImpl_->history_;
}

void MemoryMonitor::trackAllocation(size_t bytes, const std::string& source) {
    pImpl_->allocations_[source] += bytes;
    pImpl_->totalAllocated_ += bytes;
}

void MemoryMonitor::trackDeallocation(size_t bytes, const std::string& source) {
    if (pImpl_->allocations_.find(source) != pImpl_->allocations_.end()) {
        pImpl_->allocations_[source] -= bytes;
        if (pImpl_->allocations_[source] < 0) pImpl_->allocations_[source] = 0;
    }
    pImpl_->deallocations_[source] += bytes;
    pImpl_->totalDeallocated_ += bytes;
}

double MemoryMonitor::getFragmentationRatio() const {
    // Placeholder calculation
    return 0.1;  // 10% fragmentation
}

size_t MemoryMonitor::getLargestAllocation() const {
    size_t largest = 0;
    for (const auto& pair : pImpl_->allocations_) {
        if (pair.second > largest) largest = pair.second;
    }
    return largest;
}

// SimulationTimeMonitor implementation
struct SimulationTimeMonitor::Impl {
    bool running_ = false;
    std::chrono::high_resolution_clock::time_point startTime_;
    double simulationTimePerStep_ = 0.001;
    std::vector<SimulationMetrics> history_;
    uint64_t stepsCompleted_ = 0;
    uint64_t neurons_ = 0;
    uint64_t synapses_ = 0;
    double spikesPerSecond_ = 0.0;
    double avgStepTime_ = 0.0;
    double stepTimeStdDev_ = 0.0;
    double stepTimeMin_ = 0.0;
    double stepTimeMax_ = 0.0;
};

SimulationTimeMonitor::SimulationTimeMonitor() : pImpl_(std::make_unique<Impl>()) {}
SimulationTimeMonitor::~SimulationTimeMonitor() {
    stop();
}

void SimulationTimeMonitor::start(double simulationTimePerStep) {
    if (pImpl_->running_) return;
    
    pImpl_->running_ = true;
    pImpl_->startTime_ = std::chrono::high_resolution_clock::now();
    pImpl_->simulationTimePerStep_ = simulationTimePerStep;
    pImpl_->history_.clear();
}

void SimulationTimeMonitor::stop() {
    if (!pImpl_->running_) return;
    
    pImpl_->running_ = false;
}

SimulationTimeMonitor::SimulationMetrics SimulationTimeMonitor::getCurrentMetrics() const {
    SimulationMetrics metrics;
    metrics.timestamp = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    
    metrics.realTimeElapsed = std::chrono::duration<double>(metrics.timestamp - pImpl_->startTime_).count();
    metrics.simulatedTimeElapsed = metrics.realTimeElapsed * pImpl_->simulationTimePerStep_ / pImpl_->avgStepTime_;
    metrics.simulatedTimePerStep = pImpl_->simulationTimePerStep_;
    metrics.realTimePerStep = pImpl_->avgStepTime_;
    metrics.speedFactor = pImpl_->simulatedTimeElapsed / metrics.realTimeElapsed;
    metrics.stepsCompleted = pImpl_->stepsCompleted_;
    metrics.neurons = pImpl_->neurons_;
    metrics.synapses = pImpl_->synapses_;
    metrics.spikesPerSecond = pImpl_->spikesPerSecond_;
    metrics.averageStepTime = pImpl_->avgStepTime_;
    metrics.stepTimeStdDev = pImpl_->stepTimeStdDev_;
    metrics.stepTimeMin = pImpl_->stepTimeMin_;
    metrics.stepTimeMax = pImpl_->stepTimeMax_;
    
    return metrics;
}

double SimulationTimeMonitor::getRealTimeFactor() const {
    std::lock_guard<std::mutex> lock(pImpl_->realTimeMutex_);
    if (pImpl_->avgStepTime_ > 0.0) {
        return pImpl_->simulationTimePerStep_ / pImpl_->avgStepTime_;
    }
    return 1.0;
}

bool SimulationTimeMonitor::isSimulatingInRealTime() const {
    return std::abs(getRealTimeFactor() - 1.0) < 0.1;
}

void SimulationTimeMonitor::onSimulationStart() {
    pImpl_->startTime_ = std::chrono::high_resolution_clock::now();
    pImpl_->stepsCompleted_ = 0;
}

void SimulationTimeMonitor::onSimulationStop() {
    pImpl_->running_ = false;
}

void SimulationTimeMonitor::onStepComplete(double stepTimeSeconds) {
    pImpl_->stepsCompleted_++;
    
    // Update running statistics
    pImpl_->avgStepTime_ = (pImpl_->avgStepTime_ * (pImpl_->stepsCompleted_ - 1) + stepTimeSeconds) / pImpl_->stepsCompleted_;
    
    if (pImpl_->stepsCompleted_ == 1) {
        pImpl_->stepTimeMin_ = stepTimeSeconds;
        pImpl_->stepTimeMax_ = stepTimeSeconds;
    } else {
        pImpl_->stepTimeMin_ = std::min(pImpl_->stepTimeMin_, stepTimeSeconds);
        pImpl_->stepTimeMax_ = std::max(pImpl_->stepTimeMax_, stepTimeSeconds);
    }
}

// PerformanceVisualizer implementation
struct PerformanceVisualizer::Impl {
    // Empty for now - would contain visualization library references
};

PerformanceVisualizer::PerformanceVisualizer() : pImpl_(std::make_unique<Impl>()) {}
PerformanceVisualizer::~PerformanceVisualizer() {}

std::string PerformanceVisualizer::generateTimeSeriesPlot(const PerformanceProfiler& profiler, const std::string& metricName) const {
    return "Time series plot for " + metricName + " (placeholder implementation)";
}

std::string PerformanceVisualizer::generateComparisonChart(const std::vector<PerformanceProfiler>& profilers, const std::vector<std::string>& metricNames) const {
    return "Comparison chart (placeholder implementation)";
}

std::string PerformanceVisualizer::generateBottleneckAnalysis(const PerformanceProfiler& profiler) const {
    return "Bottleneck analysis (placeholder implementation)";
}

std::string PerformanceVisualizer::generateMemoryHeatmap(const PerformanceProfiler& profiler) const {
    return "Memory heatmap (placeholder implementation)";
}

std::string PerformanceVisualizer::generateHTMLReport(const PerformanceProfiler& profiler) const {
    return "<html><body>HTML report (placeholder implementation)</body></html>";
}

std::string PerformanceVisualizer::generateTextReport(const PerformanceProfiler& profiler) const {
    return "Text report (placeholder implementation)";
}

std::string PerformanceVisualizer::generateJSONReport(const PerformanceProfiler& profiler) const {
    nlohmann::json j;
    j["report"] = "JSON report (placeholder implementation)";
    return j.dump(2);
}

bool PerformanceVisualizer::saveChart(const std::string& filename, const std::string& chartType, const std::string& data) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << data;
    file.close();
    return true;
}

bool PerformanceVisualizer::saveReport(const std::string& filename, const std::string& reportType, const std::string& content) const {
    std::ofstream file(filename);
    if (!file.is_open()) return false;
    file << content;
    file.close();
    return true;
}

// PerformanceRegressionDetector implementation
struct PerformanceRegressionDetector::Impl {
    std::vector<BenchmarkRun> baselineRuns_;
};

PerformanceRegressionDetector::PerformanceRegressionDetector() : pImpl_(std::make_unique<Impl>()) {}
PerformanceRegressionDetector::~PerformanceRegressionDetector() {}

bool PerformanceRegressionDetector::loadBaseline(const std::string& filename) {
    // Placeholder implementation - would load baseline data from file
    pImpl_->baselineRuns_.clear();
    return true;
}

PerformanceRegressionDetector::RegressionResult PerformanceRegressionDetector::compareWithBaseline(const PerformanceProfiler& currentProfiler) const {
    RegressionResult result;
    result.hasRegression = false;
    
    // Placeholder implementation - would compare with baseline data
    result.regressions.push_back("Placeholder regression detection");
    result.improvements.push_back("Placeholder improvement detection");
    result.overallRegressionScore = 0.5;
    
    return result;
}

bool PerformanceRegressionDetector::isStatisticallySignificant(double currentValue, double baselineValue, double baselineStdDev) const {
    if (baselineStdDev <= 0.0) return false;
    double zScore = std::abs(currentValue - baselineValue) / baselineStdDev;
    return zScore > 1.96;  // 95% confidence level
}

std::string PerformanceRegressionDetector::generateRegressionReport(const RegressionResult& result) const {
    std::ostringstream oss;
    oss << "=== Performance Regression Report ===\n\n";
    
    if (result.hasRegression) {
        oss << "REGRESSIONS DETECTED:\n";
        for (const auto& regression : result.regressions) {
            oss << "  - " << regression << "\n";
        }
    }
    
    oss << "\nIMPROVEMENTS:\n";
    for (const auto& improvement : result.improvements) {
        oss << "  + " << improvement << "\n";
    }
    
    oss << "\nOverall Regression Score: " << result.overallRegressionScore << "\n";
    
    return oss.str();
}

std::string PerformanceRegressionDetector::generateTrendAnalysis(const std::vector<double>& metricValues) const {
    std::ostringstream oss;
    oss << "=== Trend Analysis ===\n\n";
    
    if (metricValues.size() < 2) {
        oss << "Insufficient data for trend analysis.\n";
        return oss.str();
    }
    
    double sum = std::accumulate(metricValues.begin(), metricValues.end(), 0.0);
    double mean = sum / metricValues.size();
    
    // Simple linear regression
    double numerator = 0.0, denominator = 0.0;
    for (size_t i = 0; i < metricValues.size(); ++i) {
        double x = static_cast<double>(i);
        numerator += (x - i) * (metricValues[i] - mean);
        denominator += (x - i) * (x - i);
    }
    
    double slope = numerator / denominator;
    double intercept = mean - slope * (metricValues.size() - 1) / 2.0;
    
    oss << "Trend: ";
    if (std::abs(slope) < 0.01) {
        oss << "stable\n";
    } else if (slope > 0) {
        oss << "improving (increasing)" << std::fixed << std::setprecision(6) << " (slope: " << slope << ")\n";
    } else {
        oss << "degrading (decreasing)" << std::fixed << std::setprecision(6) << " (slope: " << slope << ")\n";
    }
    
    oss << "Current value: " << std::fixed << std::setprecision(6) << metricValues.back() << "\n";
    oss << "Historical mean: " << std::fixed << std::setprecision(6) << mean << "\n";
    
    return oss.str();
}

// GlobalPerformanceMonitor implementation
struct GlobalPerformanceMonitor::Impl {
    PerformanceProfiler profiler_;
    CPUMonitor cpuMonitor_;
    MemoryMonitor memoryMonitor_;
    SimulationTimeMonitor simulationMonitor_;
    bool allMonitorsRunning_ = false;
};

GlobalPerformanceMonitor::GlobalPerformanceMonitor() : pImpl_(std::make_unique<Impl>()) {}
GlobalPerformanceMonitor::~GlobalPerformanceMonitor() {}

GlobalPerformanceMonitor& GlobalPerformanceMonitor::getInstance() {
    static GlobalPerformanceMonitor instance;
    return instance;
}

PerformanceProfiler& GlobalPerformanceMonitor::getProfiler() {
    return pImpl_->profiler_;
}

CPUMonitor& GlobalPerformanceMonitor::getCPUMonitor() {
    return pImpl_->cpuMonitor_;
}

MemoryMonitor& GlobalPerformanceMonitor::getMemoryMonitor() {
    return pImpl_->memoryMonitor_;
}

SimulationTimeMonitor& GlobalPerformanceMonitor::getSimulationTimeMonitor() {
    return pImpl_->simulationMonitor_;
}

void GlobalPerformanceMonitor::startAllMonitors() {
    pImpl_->profiler_.start();
    pImpl_->cpuMonitor_.start();
    pImpl_->memoryMonitor_.start();
    pImpl_->simulationMonitor_.start();
    
    pImpl_->allMonitorsRunning_ = true;
}

void GlobalPerformanceMonitor::stopAllMonitors() {
    pImpl_->profiler_.stop();
    pImpl_->cpuMonitor_.stop();
    pImpl_->memoryMonitor_.stop();
    pImpl_->simulationMonitor_.stop();
    
    pImpl_->allMonitorsRunning_ = false;
}

bool GlobalPerformanceMonitor::areAllMonitorsRunning() const {
    return pImpl_->allMonitorsRunning_;
}

BenchmarkMetrics GlobalPerformanceMonitor::getGlobalBenchmarkMetrics() const {
    return pImpl_->profiler_.convertToBenchmarkMetrics();
}

std::string GlobalPerformanceMonitor::getPerformanceSummary() const {
    std::ostringstream oss;
    oss << "=== Global Performance Summary ===\n\n";
    oss << "Profiler Active: " << (pImpl_->profiler_.isRunning() ? "Yes" : "No") << "\n";
    oss << "CPU Monitor Active: " << (pImpl_->cpuMonitor_.isRunning() ? "Yes" : "No") << "\n";
    oss << "Memory Monitor Active: " << (pImpl_->memoryMonitor_.isRunning() ? "Yes" : "No") << "\n";
    oss << "Simulation Monitor Active: " << (pImpl_->simulationMonitor_.isRunning() ? "Yes" : "No") << "\n\n";
    
    auto metrics = getGlobalBenchmarkMetrics();
    oss << "Benchmark Metrics:\n";
    oss << "  Average Step Time: " << std::fixed << std::setprecision(6) << metrics.avgStepTimeMs << "ms\n";
    oss << "  CPU Utilization: " << std::fixed << std::setprecision(2) << metrics.cpuUtilizationPercent << "%\n";
    oss << "  Total Memory: " << metrics.totalMemoryMB << " MB\n";
    
    return oss.str();
}

} // namespace nlm
