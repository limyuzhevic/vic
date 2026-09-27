// Implementation of PerformanceMonitor

#include "nlm/performance/PerformanceMonitor.hpp"
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace nlm {

PerformanceMonitor::PerformanceMonitor(
    size_t maxHistorySize,
    double alertThresholdLow,
    double alertThresholdHigh)
    : maxHistorySize_(maxHistorySize)
    , alertThresholdLow_(alertThresholdLow)
    , alertThresholdHigh_(alertThresholdHigh)
    , simulationStartTime_(std::chrono::high_resolution_clock::now()) {
}

void PerformanceMonitor::start() {
    if (running_.load(std::memory_order_relaxed)) {
        return;
    }
    
    running_.store(true, std::memory_order_relaxed);
    monitoringThread_ = std::thread(&PerformanceMonitor::monitoringThreadFunc, this);
}

void PerformanceMonitor::stop() {
    if (!running_.load(std::memory_order_relaxed)) {
        return;
    }
    
    {
        std::unique_lock<std::mutex> lock(stopMutex_);
        running_.store(false, std::memory_order_relaxed);
        stopCondition_.notify_one();
    }
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void PerformanceMonitor::monitoringThreadFunc() {
    while (running_.load(std::memory_order_relaxed)) {
        std::unique_lock<std::mutex> lock(stopMutex_);
        if (stopCondition_.wait_for(lock, std::chrono::seconds(1), 
                                   [this] { return !running_.load(std::memory_order_relaxed); })) {
            break;
        }
        lock.unlock();
        
        checkThresholds();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void PerformanceMonitor::recordMemoryUsage(size_t memoryAllocated, size_t memoryUsed, size_t peakMemory) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    ResourceAllocation allocation;
    allocation.memoryAllocated = memoryAllocated;
    allocation.memoryUsed = memoryUsed;
    allocation.memoryPeak = peakMemory;
    allocation.memoryReserved = memoryAllocated + peakMemory;
    
    currentResourceAllocation_ = allocation;
    
    MetricValue metric;
    metric.timestamp = std::chrono::high_resolution_clock::now();
    metric.value = static_cast<double>(memoryUsed);
    metric.unit = "bytes";
    metric.metadata["allocation_type"] = "used";
    
    storeMetric("memory_usage", metric);
    
    for (const auto& callback : metricCallbacks_) {
        callback(metric);
    }
}

void PerformanceMonitor::recordCPUUsage(double cpuUsagePercentage, size_t cpuCoresUsed,
                                       std::chrono::high_resolution_clock::duration cpuTime) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    currentResourceAllocation_.cpuCoresUsed = cpuCoresUsed;
    currentResourceAllocation_.cpuTime = cpuTime;
    currentResourceAllocation_.cpuUsagePercentage = cpuUsagePercentage;
    
    MetricValue metric;
    metric.timestamp = std::chrono::high_resolution_clock::now();
    metric.value = cpuUsagePercentage;
    metric.unit = "%";
    metric.metadata["cores_used"] = std::to_string(cpuCoresUsed);
    
    storeMetric("cpu_usage", metric);
    
    for (const auto& callback : metricCallbacks_) {
        callback(metric);
    }
}

void PerformanceMonitor::recordStepMetrics(size_t stepNumber,
                                           std::chrono::high_resolution_clock::duration stepTime,
                                           size_t spikesCount, size_t neuronsUpdated) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    currentStep_.store(stepNumber, std::memory_order_relaxed);
    
    ThroughputMetrics throughput;
    throughput.spikesPerSecond = spikesCount;
    throughput.neuronsPerSecond = neuronsUpdated;
    throughput.totalSteps = stepNumber;
    throughput.totalSpikes = throughput.spikesPerSecond; // This should accumulate
    
    currentThroughput_ = throughput;
    
    MetricValue stepMetric;
    stepMetric.timestamp = std::chrono::high_resolution_clock::now();
    stepMetric.value = static_cast<double>(stepTime.count()) / 1e9; // Convert to seconds
    stepMetric.unit = "seconds";
    stepMetric.metadata["step_number"] = std::to_string(stepNumber);
    stepMetric.metadata["spikes_count"] = std::to_string(spikesCount);
    stepMetric.metadata["neurons_updated"] = std::to_string(neuronsUpdated);
    
    storeMetric("step_time", stepMetric);
    
    for (const auto& callback : metricCallbacks_) {
        callback(stepMetric);
    }
}

void PerformanceMonitor::recordThroughput(const ThroughputMetrics& throughput) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    currentThroughput_ = throughput;
    
    MetricValue throughputMetric;
    throughputMetric.timestamp = std::chrono::high_resolution_clock::now();
    throughputMetric.value = static_cast<double>(throughput.spikesPerSecond);
    throughputMetric.unit = "spikes/second";
    throughputMetric.metadata["neurons_per_second"] = std::to_string(throughput.neuronsPerSecond);
    throughputMetric.metadata["real_time_factor"] = std::to_string(throughput.realTimeFactor);
    
    storeMetric("throughput", throughputMetric);
    
    for (const auto& callback : metricCallbacks_) {
        callback(throughputMetric);
    }
}

void PerformanceMonitor::recordProgress(size_t stepNumber, size_t maxSteps,
                                       std::chrono::high_resolution_clock::duration simulatedTime,
                                       std::chrono::high_resolution_clock::duration realTime) {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    MetricValue progressMetric;
    progressMetric.timestamp = std::chrono::high_resolution_clock::now();
    progressMetric.value = static_cast<double>(stepNumber) / static_cast<double>(maxSteps) * 100.0;
    progressMetric.unit = "%";
    progressMetric.metadata["step_number"] = std::to_string(stepNumber);
    progressMetric.metadata["max_steps"] = std::to_string(maxSteps);
    progressMetric.metadata["simulated_time"] = std::to_string(simulatedTime.count());
    progressMetric.metadata["real_time"] = std::to_string(realTime.count());
    
    storeMetric("progress", progressMetric);
    
    for (const auto& callback : metricCallbacks_) {
        callback(progressMetric);
    }
}

ResourceAllocation PerformanceMonitor::getResourceAllocation() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    return currentResourceAllocation_;
}

ThroughputMetrics PerformanceMonitor::getThroughputMetrics() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    return currentThroughput_;
}

PerformanceStats PerformanceMonitor::getPerformanceStats(const std::string& metricName,
                                                       size_t windowSize) const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    PerformanceStats stats;
    
    auto it = metricHistory_.find(metricName);
    if (it == metricHistory_.end()) {
        return stats;
    }
    
    const auto& metricValues = it->second;
    size_t actualWindowSize = windowSize == 0 ? metricValues.size() : std::min(windowSize, metricValues.size());
    
    if (actualWindowSize == 0) {
        return stats;
    }
    
    std::vector<double> values;
    values.reserve(actualWindowSize);
    
    if (windowSize == 0) {
        // Use all values
        for (const auto& metricValue : metricValues) {
            values.push_back(metricValue.value);
        }
    } else {
        // Use most recent values
        size_t startIndex = metricValues.size() - actualWindowSize;
        for (size_t i = startIndex; i < metricValues.size(); ++i) {
            values.push_back(metricValues[i].value);
        }
    }
    
    stats = calculateStats(values);
    stats.count = actualWindowSize;
    
    return stats;
}

std::vector<MetricValue> PerformanceMonitor::getRecentMetrics(const std::string& metricName,
                                                             size_t count) const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    std::vector<MetricValue> recentMetrics;
    
    auto it = metricHistory_.find(metricName);
    if (it == metricHistory_.end()) {
        return recentMetrics;
    }
    
    const auto& metricValues = it->second;
    size_t actualCount = std::min(count, metricValues.size());
    
    if (actualCount == 0) {
        return recentMetrics;
    }
    
    size_t startIndex = metricValues.size() - actualCount;
    recentMetrics.reserve(actualCount);
    
    for (size_t i = startIndex; i < metricValues.size(); ++i) {
        recentMetrics.push_back(metricValues[i]);
    }
    
    return recentMetrics;
}

std::vector<PerformanceAlert> PerformanceMonitor::getRecentAlerts(size_t count) const {
    std::lock_guard<std::mutex> lock(alertsMutex_);
    
    std::vector<PerformanceAlert> recentAlerts;
    size_t actualCount = std::min(count, alertHistory_.size());
    
    if (actualCount == 0) {
        return recentAlerts;
    }
    
    size_t startIndex = alertHistory_.size() - actualCount;
    recentAlerts.reserve(actualCount);
    
    for (size_t i = startIndex; i < alertHistory_.size(); ++i) {
        recentAlerts.push_back(alertHistory_[i]);
    }
    
    return recentAlerts;
}

void PerformanceMonitor::addMetricCallback(const MetricCallback& callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    metricCallbacks_.push_back(callback);
}

void PerformanceMonitor::addAlertCallback(const AlertCallback& callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    alertCallbacks_.push_back(callback);
}

void PerformanceMonitor::addOptimizationCallback(const OptimizationCallback& callback) {
    std::lock_guard<std::mutex> lock(callbackMutex_);
    optimizationCallbacks_.push_back(callback);
}

std::vector<std::string> PerformanceMonitor::getBottlenecks() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    std::vector<std::string> bottlenecks;
    
    // Analyze memory usage
    auto memoryStats = getPerformanceStats("memory_usage");
    if (memoryStats.max > alertThresholdHigh_ * 1024 * 1024 * 1024) { // 1 GB threshold
        bottlenecks.push_back("High memory usage detected: " + std::to_string(memoryStats.max / (1024 * 1024)) + " MB");
    }
    
    // Analyze CPU usage
    auto cpuStats = getPerformanceStats("cpu_usage");
    if (cpuStats.mean > 90.0) {
        bottlenecks.push_back("High CPU usage detected: " + std::to_string(cpuStats.mean) + "%");
    }
    
    // Analyze step timing
    auto stepStats = getPerformanceStats("step_time");
    if (stepStats.mean > 1.0) { // 1 second threshold
        bottlenecks.push_back("Slow step execution detected: " + std::to_string(stepStats.mean) + " seconds average");
    }
    
    // Analyze throughput
    auto throughputStats = getPerformanceStats("throughput");
    if (throughputStats.mean < 100.0) { // 100 spikes/sec threshold
        bottlenecks.push_back("Low throughput detected: " + std::to_string(throughputStats.mean) + " spikes/sec");
    }
    
    return bottlenecks;
}

std::string PerformanceMonitor::generateReport(const std::string& format,
                                               bool includeDetails) const {
    std::stringstream report;
    
    if (format == "text") {
        report << "=== NLM Performance Report ===" << std::endl;
        report << "Generated: " << std::chrono::high_resolution_clock::now() << std::endl;
        report << "Current Step: " << getCurrentStep() << std::endl;
        report << "Progress: " << getProgressPercentage() << "%" << std::endl;
        report << std::endl;
        
        if (includeDetails) {
            report << "--- Resource Allocation ---" << std::endl;
            auto allocation = getResourceAllocation();
            report << "Memory Allocated: " << allocation.memoryAllocated << " bytes" << std::endl;
            report << "Memory Used: " << allocation.memoryUsed << " bytes" << std::endl;
            report << "Memory Peak: " << allocation.memoryPeak << " bytes" << std::endl;
            report << "CPU Cores Used: " << allocation.cpuCoresUsed << std::endl;
            report << "CPU Usage: " << allocation.cpuUsagePercentage << "%" << std::endl;
            report << std::endl;
            
            report << "--- Throughput Metrics ---" << std::endl;
            auto throughput = getThroughputMetrics();
            report << "Spikes per Second: " << throughput.spikesPerSecond << std::endl;
            report << "Neurons per Second: " << throughput.neuronsPerSecond << std::endl;
            report << "Real-time Factor: " << throughput.realTimeFactor << std::endl;
            report << std::endl;
        }
        
        report << "--- Performance Statistics ---" << std::endl;
        
        std::vector<std::string> metrics = {"memory_usage", "cpu_usage", "step_time", "throughput"};
        for (const auto& metricName : metrics) {
            auto stats = getPerformanceStats(metricName);
            report << metricName << ":" << std::endl;
            report << "  Mean: " << stats.mean << " " << getUnitForMetric(metricName) << std::endl;
            report << "  Min: " << stats.min << " " << getUnitForMetric(metricName) << std::endl;
            report << "  Max: " << stats.max << " " << getUnitForMetric(metricName) << std::endl;
            report << "  Stddev: " << stats.stddev << " " << getUnitForMetric(metricName) << std::endl;
            report << std::endl;
        }
        
        report << "--- Alerts ---" << std::endl;
        auto alerts = getRecentAlerts(10);
        if (alerts.empty()) {
            report << "No alerts." << std::endl;
        } else {
            for (const auto& alert : alerts) {
                report << "[" << alertTimestampToString(alert.timestamp) << "] "
                       << alertLevelToString(alert.level) << ": "
                       << alert.name << " - " << alert.description << std::endl;
            }
        }
    } else if (format == "json") {
        // JSON report generation
        report << "{\"performance_report\": {" << std::endl;
        report << "  \"generated_at\": \"" << timestampToString(std::chrono::high_resolution_clock::now()) << "\"," << std::endl;
        report << "  \"current_step\": " << getCurrentStep() << "," << std::endl;
        report << "  \"progress_percentage\": " << getProgressPercentage() << "," << std::endl;
        
        if (includeDetails) {
            report << "  \"resource_allocation\": {" << std::endl;
            auto allocation = getResourceAllocation();
            report << "    \"memory_allocated\": " << allocation.memoryAllocated << "," << std::endl;
            report << "    \"memory_used\": " << allocation.memoryUsed << "," << std::endl;
            report << "    \"memory_peak\": " << allocation.memoryPeak << "," << std::endl;
            report << "    \"cpu_cores_used\": " << allocation.cpuCoresUsed << "," << std::endl;
            report << "    \"cpu_usage_percentage\": " << allocation.cpuUsagePercentage << "," << std::endl;
            report << "  }," << std::endl;
            
            report << "  \"throughput_metrics\": {" << std::endl;
            auto throughput = getThroughputMetrics();
            report << "    \"spikes_per_second\": " << throughput.spikesPerSecond << "," << std::endl;
            report << "    \"neurons_per_second\": " << throughput.neuronsPerSecond << "," << std::endl;
            report << "    \"real_time_factor\": " << throughput.realTimeFactor << "," << std::endl;
            report << "  }," << std::endl;
        }
        
        report << "  \"metrics\": [" << std::endl;
        std::vector<std::string> metrics = {"memory_usage", "cpu_usage", "step_time", "throughput"};
        for (size_t i = 0; i < metrics.size(); ++i) {
            auto stats = getPerformanceStats(metrics[i]);
            report << "    {\"name\": \"" << metrics[i] << "\",\"mean\": " << stats.mean
                   << ",\"min\": " << stats.min << ",\"max\": " << stats.max
                   << ",\"stddev\": " << stats.stddev << "}";
            if (i < metrics.size() - 1) report << ",";
            report << std::endl;
        }
        report << "  ]" << std::endl;
        
        report << "}";
    }
    
    return report.str();
}

void PerformanceMonitor::saveData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }
    
    // Save performance data in a simple format
    file << "# NLM Performance Data" << std::endl;
    file << "# Generated: " << timestampToString(std::chrono::high_resolution_clock::now()) << std::endl;
    file << std::endl;
    
    // Write metric history
    file << "METRICS:" << std::endl;
    for (const auto& [metricName, metricValues] : metricHistory_) {
        file << "  " << metricName << ":" << std::endl;
        for (const auto& metricValue : metricValues) {
            file << "    " << timestampToString(metricValue.timestamp) << " "
                 << metricValue.value << " " << metricValue.unit << std::endl;
        }
    }
    
    file << std::endl;
    
    // Write alerts
    file << "ALERTS:" << std::endl;
    for (const auto& alert : alertHistory_) {
        file << "  " << timestampToString(alert.timestamp) << " "
             << alertLevelToString(alert.level) << " "
             << alert.name << " - " << alert.description << std::endl;
    }
}

void PerformanceMonitor::loadData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for reading: " + filename);
    }
    
    std::string line;
    std::string currentMetric;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line.find(":") != std::string::npos) {
            // Metric section
            currentMetric = line.substr(0, line.find(":"));
            continue;
        }
        
        if (line.find(" ") == std::string::npos) {
            continue;
        }
        
        // Parse metric data point
        std::istringstream iss(line);
        std::string timestampStr, valueStr, unit;
        iss >> timestampStr >> valueStr >> unit;
        
        MetricValue metricValue;
        metricValue.timestamp = parseTimestamp(timestampStr);
        metricValue.value = std::stod(valueStr);
        metricValue.unit = unit;
        
        storeMetric(currentMetric, metricValue);
    }
}

double PerformanceMonitor::getProgressPercentage() const {
    // This would need access to maxSteps, which isn't stored in this class
    // This is a simplified implementation
    return static_cast<double>(getCurrentStep()) / 1000.0 * 100.0;
}

std::chrono::high_resolution_clock::duration PerformanceMonitor::getElapsedTime() const {
    return std::chrono::high_resolution_clock::now() - simulationStartTime_;
}

void PerformanceMonitor::storeMetric(const std::string& name, const MetricValue& value) {
    auto it = metricHistory_.find(name);
    if (it == metricHistory_.end()) {
        metricHistory_[name] = std::vector<MetricValue>{value};
    } else {
        it->second.push_back(value);
        
        // Maintain history size
        while (it->second.size() > maxHistorySize_) {
            it->second.erase(it->second.begin());
        }
    }
}

void PerformanceMonitor::storeAlert(const PerformanceAlert& alert) {
    alertHistory_.push_back(alert);
    
    // Maintain alert history size
    while (alertHistory_.size() > maxHistorySize_ * 2) {
        alertHistory_.erase(alertHistory_.begin());
    }
}

void PerformanceMonitor::checkThresholds() {
    // Check memory usage
    auto memoryStats = getPerformanceStats("memory_usage");
    if (alertThresholdHigh_ > 0.0 && memoryStats.max > alertThresholdHigh_) {
        PerformanceAlert alert;
        alert.level = PerformanceAlert::AlertLevel::WARNING;
        alert.name = "high_memory_usage";
        alert.description = "Memory usage exceeds threshold";
        alert.threshold = alertThresholdHigh_;
        alert.currentValue = memoryStats.max;
        alert.timestamp = std::chrono::high_resolution_clock::now();
        
        storeAlert(alert);
        
        for (const auto& callback : alertCallbacks_) {
            callback(alert);
        }
    }
    
    // Check CPU usage
    auto cpuStats = getPerformanceStats("cpu_usage");
    if (alertThresholdHigh_ > 0.0 && cpuStats.max > alertThresholdHigh_) {
        PerformanceAlert alert;
        alert.level = PerformanceAlert::AlertLevel::WARNING;
        alert.name = "high_cpu_usage";
        alert.description = "CPU usage exceeds threshold";
        alert.threshold = alertThresholdHigh_;
        alert.currentValue = cpuStats.max;
        alert.timestamp = std::chrono::high_resolution_clock::now();
        
        storeAlert(alert);
        
        for (const auto& callback : alertCallbacks_) {
            callback(alert);
        }
    }
}

PerformanceStats PerformanceMonitor::calculateStats(const std::vector<double>& values) const {
    PerformanceStats stats;
    
    if (values.empty()) {
        return stats;
    }
    
    stats.count = values.size();
    
    // Calculate mean
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    stats.mean = sum / stats.count;
    
    // Calculate variance and stddev
    double sumSquared = 0.0;
    for (double value : values) {
        double diff = value - stats.mean;
        sumSquared += diff * diff;
    }
    
    stats.variance = sumSquared / stats.count;
    stats.stddev = std::sqrt(stats.variance);
    
    // Find min and max
    stats.min = *std::min_element(values.begin(), values.end());
    stats.max = *std::max_element(values.begin(), values.end());
    
    // Calculate median
    std::vector<double> sortedValues = values;
    std::sort(sortedValues.begin(), sortedValues.end());
    size_t mid = sortedValues.size() / 2;
    if (sortedValues.size() % 2 == 0) {
        stats.median = (sortedValues[mid - 1] + sortedValues[mid]) / 2.0;
    } else {
        stats.median = sortedValues[mid];
    }
    
    // Calculate duration (placeholder)
    stats.duration = std::chrono::seconds(60);
    
    return stats;
}

std::string PerformanceMonitor::getUnitForMetric(const std::string& metricName) const {
    static const std::unordered_map<std::string, std::string> units = {
        {"memory_usage", "bytes"},
        {"cpu_usage", "percent"},
        {"step_time", "seconds"},
        {"throughput", "spikes/second"},
        {"progress", "percent"}
    };
    
    auto it = units.find(metricName);
    if (it != units.end()) {
        return it->second;
    }
    return "";
}

std::string PerformanceMonitor::timestampToString(const std::chrono::high_resolution_clock::time_point& timestamp) const {
    auto duration = timestamp.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration % std::chrono::seconds(1));
    
    std::stringstream ss;
    ss << seconds.count() << "." << std::setfill('0') << std::setw(3) << milliseconds.count();
    return ss.str();
}

std::string PerformanceMonitor::alertLevelToString(PerformanceAlert::AlertLevel level) const {
    switch (level) {
        case PerformanceAlert::AlertLevel::INFO: return "INFO";
        case PerformanceAlert::AlertLevel::WARNING: return "WARNING";
        case PerformanceAlert::AlertLevel::CRITICAL: return "CRITICAL";
        case PerformanceAlert::AlertLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::chrono::high_resolution_clock::time_point PerformanceMonitor::parseTimestamp(const std::string& timestampStr) const {
    // Simple timestamp parsing (seconds with milliseconds)
    size_t dotPos = timestampStr.find('.');
    if (dotPos == std::string::npos) {
        return std::chrono::high_resolution_clock::time_point(std::chrono::seconds(std::stoll(timestampStr)));
    }
    
    double time = std::stod(timestampStr);
    auto seconds = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(
        std::chrono::duration<double>(time)
    );
    
    return std::chrono::high_resolution_clock::time_point(seconds);
}