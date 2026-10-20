#include "StatisticsCollector.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <cmath>
#include <unordered_map>
#include <numeric>
#include <nlohmann/json.hpp>

namespace nlm {

using json = nlohmann::json;

struct StatisticsCollector::Impl {
    std::shared_ptr<Logger> logger;
    std::shared_ptr<EnhancedProgressReporter> progressReporter;
    
    // Configuration
    bool enableSystemTracking;
    bool enableNeuralTracking;
    bool enablePerformanceTracking;
    bool enableEventTracking;
    std::chrono::milliseconds samplingInterval;
    size_t historySize;
    size_t maxMemoryThreshold;
    double cpuThreshold;
    
    // Data storage
    std::vector<SystemStatistics> systemHistory;
    std::vector<NeuralStatistics> neuralHistory;
    std::vector<PerformanceMetrics> performanceHistory;
    std::map<std::string, EventStatistics> eventStats;
    std::map<std::string, std::vector<double>> customMetrics;
    
    // Current values
    SystemStatistics currentSystemStats;
    NeuralStatistics currentNeuralStats;
    PerformanceMetrics currentPerformanceMetrics;
    
    // Real-time monitoring
    std::atomic<bool> realTimeMonitoring{false};
    std::thread monitoringThread;
    std::mutex dataMutex;
    
    // Thresholds
    std::map<std::string, double> performanceThresholds;
    uint64_t memoryThreshold;
    double cpuThreshold;
    
    // Monitoring flags
    bool enableSystemTracking;
    bool enableNeuralTracking;
    bool enablePerformanceTracking;
    bool enableEventTracking;
    
    Impl(std::shared_ptr<Logger> logg)
        : logger(logg), enableSystemTracking(true), enableNeuralTracking(true),
          enablePerformanceTracking(true), enableEventTracking(true),
          samplingInterval(100), historySize(1000), maxMemoryThreshold(100 * 1024 * 1024), // 100MB
          memoryThreshold(500 * 1024 * 1024), // 500MB
          cpuThreshold(90.0) { // 90%
        
        // Initialize default performance thresholds
        performanceThresholds["memory_usage"] = 0.9;
        performanceThresholds["cpu_usage"] = 0.9;
        performanceThresholds["response_time"] = 5000.0; // 5 seconds
        performanceThresholds["efficiency_score"] = 0.5;
        
        // Initialize default neural thresholds
        performanceThresholds["firing_rate"] = 100.0;
        performanceThresholds["connection_strength"] = 0.5;
        performanceThresholds["spike_count"] = 1000.0;
    }
};

StatisticsCollector::StatisticsCollector(std::shared_ptr<Logger> logger)
    : pImpl(std::make_unique<Impl>(logger)) {
}

StatisticsCollector::~StatisticsCollector() {
    stopRealTimeMonitoring();
}

void StatisticsCollector::recordSystemStatistics(const SystemStatistics& stats) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->currentSystemStats = stats;
    pImpl->systemHistory.push_back(stats);
    
    // Trim history if too large
    if (pImpl->systemHistory.size() > pImpl->historySize) {
        pImpl->systemHistory.erase(pImpl->systemHistory.begin());
    }
    
    // Check thresholds
    checkAndAlertThresholds();
}

void StatisticsCollector::updateSystemResourceUsage() {
    SystemStatistics stats;
    stats.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // Read system memory info
    std::ifstream meminfo("/proc/meminfo");
    if (meminfo.is_open()) {
        std::string line;
        while (std::getline(meminfo, line)) {
            if (line.find("MemTotal:") == 0) {
                stats.memoryTotal = std::stoull(line.substr(line.find(':') + 1)) * 1024;
            } else if (line.find("MemAvailable:") == 0) {
                stats.memoryAvailable = std::stoull(line.substr(line.find(':') + 1)) * 1024;
            } else if (line.find("MemUsed:") == 0 || line.find("VmRSS:") == 0) {
                stats.memoryUsage = std::stoull(line.substr(line.find(':') + 1)) * 1024;
            }
        }
        meminfo.close();
    }
    
    // Calculate memory available
    if (stats.memoryTotal > 0) {
        stats.memoryAvailable = stats.memoryTotal - stats.memoryUsage;
    }
    
    // Get CPU core count
    stats.cpuCores = std::thread::hardware_concurrency();
    if (stats.cpuCores == 0) {
        stats.cpuCores = 1;
    }
    
    // Get CPU frequency
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (cpuinfo.is_open()) {
        std::string line;
        while (std::getline(cpuinfo, line)) {
            if (line.find("cpu MHz") == 0) {
                stats.cpuFrequency = std::stod(line.substr(line.find(':') + 1));
                break;
            }
        }
        cpuinfo.close();
    }
    
    recordSystemStatistics(stats);
}

SystemStatistics StatisticsCollector::getLatestSystemStats() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->currentSystemStats;
}

std::vector<SystemStatistics> StatisticsCollector::getSystemStatisticsHistory(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (limit == 0 || limit >= pImpl->systemHistory.size()) {
        return pImpl->systemHistory;
    }
    
    std::vector<SystemStatistics> result;
    result.reserve(limit);
    auto start = pImpl->systemHistory.end() - limit;
    for (auto it = start; it != pImpl->systemHistory.end(); ++it) {
        result.push_back(*it);
    }
    
    return result;
}

void StatisticsCollector::recordNeuralStatistics(const NeuralStatistics& stats) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (!stats.testName.empty()) {
        pImpl->currentNeuralStats = stats;
        pImpl->neuralHistory.push_back(stats);
        
        // Trim history if too large
        if (pImpl->neuralHistory.size() > pImpl->historySize) {
            pImpl->neuralHistory.erase(pImpl->neuralHistory.begin());
        }
    }
}

void StatisticsCollector::updateNeuralNetworkMetrics(std::shared_ptr<Brain> brain, const std::string& testName) {
    if (!brain || testName.empty()) return;
    
    NeuralStatistics stats;
    stats.testName = testName;
    
    stats.neuronCount = brain->getTotalNeuronCount();
    stats.synapseCount = brain->getTotalSynapseCount();
    stats.firingNeuronCount = brain->getFiringNeuronCount();
    stats.averageFiringRate = brain->getAverageFiringRate();
    stats.totalSpikeCount = brain->getTotalSpikeCount();
    stats.totalMemoryUsage = brain->getTotalMemoryUsage();
    stats.plasticityWeight = brain->getPlasticityWeight();
    
    // Calculate efficiency score
    if (stats.neuronCount > 0) {
        stats.efficiencyScore = (static_cast<double>(stats.firingNeuronCount) / stats.neuronCount) *
                               (stats.averageFiringRate / 100.0);
    }
    
    recordNeuralStatistics(stats);
}

NeuralStatistics StatisticsCollector::getLatestNeuralStats(const std::string& testName) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (testName.empty()) {
        return pImpl->currentNeuralStats;
    }
    
    for (auto it = pImpl->neuralHistory.rbegin(); it != pImpl->neuralHistory.rend(); ++it) {
        if (it->testName == testName) {
            return *it;
        }
    }
    
    NeuralStatistics empty;
    return empty;
}

std::vector<NeuralStatistics> StatisticsCollector::getNeuralStatisticsHistory(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (limit == 0 || limit >= pImpl->neuralHistory.size()) {
        return pImpl->neuralHistory;
    }
    
    std::vector<NeuralStatistics> result;
    result.reserve(limit);
    auto start = pImpl->neuralHistory.end() - limit;
    for (auto it = start; it != pImpl->neuralHistory.end(); ++it) {
        result.push_back(*it);
    }
    
    return result;
}

void StatisticsCollector::recordPerformanceMetrics(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->currentPerformanceMetrics = metrics;
    pImpl->performanceHistory.push_back(metrics);
    
    // Trim history if too large
    if (pImpl->performanceHistory.size() > pImpl->historySize) {
        pImpl->performanceHistory.erase(pImpl->performanceHistory.begin());
    }
}

void StatisticsCollector::updatePerformanceCounters() {
    PerformanceMetrics metrics;
    
    // Get system performance info
    metrics.cpuUsage = getCurrentCPUUsage();
    metrics.memoryUsage = getCurrentMemoryUsagePercentage();
    
    // Get process-specific metrics
    metrics.executionTime = getCurrentExecutionTime();
    metrics.wallClockTime = getCurrentWallClockTime();
    
    recordPerformanceMetrics(metrics);
}

PerformanceMetrics StatisticsCollector::getLatestPerformanceMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->currentPerformanceMetrics;
}

std::vector<PerformanceMetrics> StatisticsCollector::getPerformanceHistory(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (limit == 0 || limit >= pImpl->performanceHistory.size()) {
        return pImpl->performanceHistory;
    }
    
    std::vector<PerformanceMetrics> result;
    result.reserve(limit);
    auto start = pImpl->performanceHistory.end() - limit;
    for (auto it = start; it != pImpl->performanceHistory.end(); ++it) {
        result.push_back(*it);
    }
    
    return result;
}

void StatisticsCollector::recordEventStatistics(const EventStatistics& stats) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    if (pImpl->eventStats.find(stats.eventName) != pImpl->eventStats.end()) {
        // Update existing event statistics
        EventStatistics& existing = pImpl->eventStats[stats.eventName];
        existing.duration += stats.duration;
        existing.minDuration = std::min(existing.minDuration, stats.duration);
        existing.maxDuration = std::max(existing.maxDuration, stats.duration);
        existing.eventCount += stats.eventCount;
        
        if (existing.eventCount > 0) {
            existing.averageDuration = existing.duration / existing.eventCount;
            existing.successRate = (stats.eventCount * 100.0) / existing.eventCount;
        }
    } else {
        pImpl->eventStats[stats.eventName] = stats;
    }
}

void StatisticsCollector::recordEvent(const std::string& eventName, const std::chrono::milliseconds& duration,
                                    bool success, std::map<std::string, double> metrics) {
    EventStatistics stats;
    stats.eventName = eventName;
    stats.duration = duration;
    stats.minDuration = duration;
    stats.maxDuration = duration;
    stats.eventCount = 1;
    stats.successRate = success ? 100.0 : 0.0;
    stats.eventMetrics = metrics;
    
    recordEventStatistics(stats);
}

EventStatistics StatisticsCollector::getEventStatistics(const std::string& eventName) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    auto it = pImpl->eventStats.find(eventName);
    if (it != pImpl->eventStats.end()) {
        return it->second;
    }
    
    EventStatistics empty;
    return empty;
}

std::map<std::string, EventStatistics> StatisticsCollector::getAllEventStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    return pImpl->eventStats;
}

void StatisticsCollector::recordMetric(const std::string& name, double value,
                                      const std::string& category,
                                      std::map<std::string, std::string> tags) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::string key = category + ":" + name;
    pImpl->customMetrics[key].push_back(value);
    
    // Trim history if too large
    if (pImpl->customMetrics[key].size() > pImpl->historySize) {
        pImpl->customMetrics[key].erase(pImpl->customMetrics[key].begin());
    }
}

double StatisticsCollector::getMetric(const std::string& name, const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::string key = category + ":" + name;
    auto it = pImpl->customMetrics.find(key);
    if (it == pImpl->customMetrics.end() || it->second.empty()) {
        return 0.0;
    }
    
    // Calculate average
    double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
    return sum / it->second.size();
}

std::vector<std::pair<std::string, double>> StatisticsCollector::getMetricsByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::vector<std::pair<std::string, double>> result;
    
    for (const auto& pair : pImpl->customMetrics) {
        if (pair.first.find(category + ":") == 0) {
            std::string name = pair.first.substr(category.length() + 1);
            double value = getMetric(name, category);
            result.push_back(std::make_pair(name, value));
        }
    }
    
    return result;
}

void StatisticsCollector::setEnableSystemTracking(bool enable) {
    pImpl->enableSystemTracking = enable;
}

void StatisticsCollector::setEnableNeuralTracking(bool enable) {
    pImpl->enableNeuralTracking = enable;
}

void StatisticsCollector::setEnablePerformanceTracking(bool enable) {
    pImpl->enablePerformanceTracking = enable;
}

void StatisticsCollector::setEnableEventTracking(bool enable) {
    pImpl->enableEventTracking = enable;
}

void StatisticsCollector::setSamplingInterval(std::chrono::milliseconds interval) {
    pImpl->samplingInterval = interval;
}

void StatisticsCollector::setHistorySize(size_t size) {
    pImpl->historySize = size;
}

std::string StatisticsCollector::generateSystemReport() const {
    std::stringstream ss;
    
    ss << "=== System Statistics Report ===\n";
    
    auto latest = getLatestSystemStats();
    ss << "Memory Usage: " << (latest.memoryUsage / (1024 * 1024)) << " MB / "
       << (latest.memoryTotal / (1024 * 1024)) << " MB\n";
    ss << "CPU Usage: " << latest.cpuUtilization << "%\n";
    ss << "CPU Cores: " << latest.cpuCores << "\n";
    ss << "CPU Frequency: " << latest.cpuFrequency << " MHz\n";
    
    return ss.str();
}

std::string StatisticsCollector::generateNeuralReport(const std::string& testName) const {
    std::stringstream ss;
    
    ss << "=== Neural Statistics Report ===\n";
    
    auto latest = getLatestNeuralStats(testName);
    if (testName.empty()) {
        ss << "(No test name specified, showing latest overall stats)\n";
    }
    
    ss << "Test Name: " << latest.testName << "\n";
    ss << "Neuron Count: " << latest.neuronCount << "\n";
    ss << "Synapse Count: " << latest.synapseCount << "\n";
    ss << "Firing Neuron Count: " << latest.firingNeuronCount << "\n";
    ss << "Average Firing Rate: " << latest.averageFiringRate << " Hz\n";
    ss << "Total Spike Count: " << latest.totalSpikeCount << "\n";
    ss << "Memory Usage: " << (latest.totalMemoryUsage / 1024) << " KB\n";
    ss << "Plasticity Weight: " << latest.plasticityWeight << "\n";
    ss << "Efficiency Score: " << latest.efficiencyScore << "\n";
    
    return ss.str();
}

std::string StatisticsCollector::generatePerformanceReport() const {
    std::stringstream ss;
    
    ss << "=== Performance Report ===\n";
    
    auto latest = getLatestPerformanceMetrics();
    ss << "Execution Time: " << latest.executionTime.count() << " ms\n";
    ss << "Wall Clock Time: " << latest.wallClockTime.count() << " ms\n";
    ss << "CPU Usage: " << latest.cpuUsage << "%\n";
    ss << "Memory Usage: " << latest.memoryUsage << "%\n";
    ss << "Cache Hits: " << latest.cacheHits << "\n";
    ss << "Cache Misses: " << latest.cacheMisses << "\n";
    
    return ss.str();
}

std::string StatisticsCollector::generateEventReport() const {
    std::stringstream ss;
    
    ss << "=== Event Statistics Report ===\n";
    
    auto allEvents = getAllEventStatistics();
    for (const auto& pair : allEvents) {
        const auto& stats = pair.second;
        ss << "Event: " << stats.eventName << "\n";
        ss << "  Count: " << stats.eventCount << "\n";
        ss << "  Average Duration: " << stats.averageDuration.count() << " ms\n";
        ss << "  Min Duration: " << stats.minDuration.count() << " ms\n";
        ss << "  Max Duration: " << stats.maxDuration.count() << " ms\n";
        ss << "  Success Rate: " << stats.successRate << "%\n";
        ss << "\n";
    }
    
    return ss.str();
}

void StatisticsCollector::clearAllStatistics() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->systemHistory.clear();
    pImpl->neuralHistory.clear();
    pImpl->performanceHistory.clear();
    pImpl->eventStats.clear();
    pImpl->customMetrics.clear();
}

bool StatisticsCollector::exportStatistics(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        
        // Export system statistics
        j["system_stats"] = json::array();
        auto systemHistory = getSystemStatisticsHistory();
        for (const auto& stats : systemHistory) {
            json sys;
            sys["timestamp"] = stats.timestamp;
            sys["memory_usage"] = stats.memoryUsage;
            sys["memory_available"] = stats.memoryAvailable;
            sys["memory_total"] = stats.memoryTotal;
            sys["cpu_utilization"] = stats.cpuUtilization;
            sys["cpu_cores"] = stats.cpuCores;
            sys["cpu_frequency"] = stats.cpuFrequency;
            j["system_stats"].push_back(sys);
        }
        
        // Export neural statistics
        j["neural_stats"] = json::array();
        auto neuralHistory = getNeuralStatisticsHistory();
        for (const auto& stats : neuralHistory) {
            json neural;
            neural["test_name"] = stats.testName;
            neural["neuron_count"] = stats.neuronCount;
            neural["synapse_count"] = stats.synapseCount;
            neural["firing_neuron_count"] = stats.firingNeuronCount;
            neural["average_firing_rate"] = stats.averageFiringRate;
            neural["total_spike_count"] = stats.totalSpikeCount;
            neural["total_memory_usage"] = stats.totalMemoryUsage;
            neural["plasticity_weight"] = stats.plasticityWeight;
            neural["computation_time"] = stats.computationTime.count();
            neural["efficiency_score"] = stats.efficiencyScore;
            j["neural_stats"].push_back(neural);
        }
        
        // Export performance metrics
        j["performance_metrics"] = json::array();
        auto performanceHistory = getPerformanceHistory();
        for (const auto& metrics : performanceHistory) {
            json perf;
            perf["execution_time"] = metrics.executionTime.count();
            perf["wall_clock_time"] = metrics.wallClockTime.count();
            perf["cpu_usage"] = metrics.cpuUsage;
            perf["memory_usage"] = metrics.memoryUsage;
            j["performance_metrics"].push_back(perf);
        }
        
        // Export event statistics
        j["event_stats"] = json::object();
        auto allEvents = getAllEventStatistics();
        for (const auto& pair : allEvents) {
            json event;
            event["event_name"] = pair.first;
            event["duration"] = pair.second.duration.count();
            event["min_duration"] = pair.second.minDuration.count();
            event["max_duration"] = pair.second.maxDuration.count();
            event["average_duration"] = pair.second.averageDuration.count();
            event["event_count"] = pair.second.eventCount;
            event["success_rate"] = pair.second.successRate;
            j["event_stats"][pair.first] = event;
        }
        
        file << j.dump(2);
        file.close();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool StatisticsCollector::importStatistics(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        file.close();
        
        // Clear existing data
        clearAllStatistics();
        
        // Import system statistics
        if (j.find("system_stats") != j.end()) {
            for (const auto& item : j["system_stats"]) {
                SystemStatistics stats;
                stats.timestamp = item["timestamp"].get<uint64_t>();
                stats.memoryUsage = item["memory_usage"].get<uint64_t>();
                stats.memoryAvailable = item["memory_available"].get<uint64_t>();
                stats.memoryTotal = item["memory_total"].get<uint64_t>();
                stats.cpuUtilization = item["cpu_utilization"].get<double>();
                stats.cpuCores = item["cpu_cores"].get<int>();
                stats.cpuFrequency = item["cpu_frequency"].get<double>();
                recordSystemStatistics(stats);
            }
        }
        
        // Import neural statistics
        if (j.find("neural_stats") != j.end()) {
            for (const auto& item : j["neural_stats"]) {
                NeuralStatistics stats;
                stats.testName = item["test_name"].get<std::string>();
                stats.neuronCount = item["neuron_count"].get<uint64_t>();
                stats.synapseCount = item["synapse_count"].get<uint64_t>();
                stats.firingNeuronCount = item["firing_neuron_count"].get<uint32_t>();
                stats.averageFiringRate = item["average_firing_rate"].get<double>();
                stats.totalSpikeCount = item["total_spike_count"].get<uint64_t>();
                stats.totalMemoryUsage = item["total_memory_usage"].get<uint64_t>();
                stats.plasticityWeight = item["plasticity_weight"].get<double>();
                stats.computationTime = std::chrono::milliseconds(item["computation_time"].get<long long>());
                stats.efficiencyScore = item["efficiency_score"].get<double>();
                recordNeuralStatistics(stats);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void StatisticsCollector::startRealTimeMonitoring() {
    pImpl->realTimeMonitoring = true;
    pImpl->monitoringThread = std::thread(&StatisticsCollector::monitoringLoop, this);
}

void StatisticsCollector::stopRealTimeMonitoring() {
    pImpl->realTimeMonitoring = false;
    if (pImpl->monitoringThread.joinable()) {
        pImpl->monitoringThread.join();
    }
}

bool StatisticsCollector::isRealTimeMonitoring() const {
    return pImpl->realTimeMonitoring;
}

std::string StatisticsCollector::generateRealTimeStats() const {
    std::stringstream ss;
    
    ss << "=== Real-Time Statistics ===\n";
    ss << "Monitoring: " << (isRealTimeMonitoring() ? "ON" : "OFF") << "\n";
    
    auto latestSystem = getLatestSystemStats();
    ss << "System Memory: " << (latestSystem.memoryUsage / (1024 * 1024)) << " MB\n";
    ss << "System CPU: " << latestSystem.cpuUtilization << "%\n";
    
    auto latestNeural = getLatestNeuralStats();
    ss << "Neural Neurons: " << latestNeural.neuronCount << "\n";
    ss << "Neural Synapses: " << latestNeural.synapseCount << "\n";
    
    auto latestPerf = getLatestPerformanceMetrics();
    ss << "Performance CPU: " << latestPerf.cpuUsage << "%

";
    
    return ss.str();
}

void StatisticsCollector::monitoringLoop() {
    while (pImpl->realTimeMonitoring) {
        updateSystemResourceUsage();
        
        // Sample neural stats if tracking is enabled
        if (pImpl->enableNeuralTracking) {
            // Note: This would need access to the brain object
            // For now, we'll skip neural sampling in the thread
        }
        
        updatePerformanceCounters();
        
        std::this_thread::sleep_for(pImpl->samplingInterval);
    }
}

void StatisticsCollector::checkAndAlertThresholds() const {
    auto latestSystem = getLatestSystemStats();
    auto latestPerf = getLatestPerformanceMetrics();
    
    // Check memory threshold
    if (latestSystem.memoryUsage > pImpl->memoryThreshold) {
        // Log memory threshold violation
    }
    
    // Check CPU threshold
    if (latestSystem.cpuUtilization > pImpl->cpuThreshold) {
        // Log CPU threshold violation
    }
    
    // Check performance thresholds
    for (const auto& pair : pImpl->performanceThresholds) {
        if (pair.first == "memory_usage") {
            if (latestSystem.memoryUsage > pair.second * latestSystem.memoryTotal) {
                // Log performance threshold violation
            }
        } else if (pair.first == "cpu_usage") {
            if (latestSystem.cpuUtilization > pair.second) {
                // Log performance threshold violation
            }
        }
    }
}

void StatisticsCollector::setProgressReporter(std::shared_ptr<EnhancedProgressReporter> reporter) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->progressReporter = reporter;
}

void StatisticsCollector::updateProgressFromMetrics() {
    if (pImpl->progressReporter) {
        // Update progress based on system metrics
        auto latestSystem = getLatestSystemStats();
        
        // Example: Update overall progress based on memory usage
        if (latestSystem.memoryTotal > 0) {
            double memoryProgress = static_cast<double>(latestSystem.memoryUsage) /
                                   static_cast<double>(latestSystem.memoryTotal);
            pImpl->progressReporter->updateProgress("system", 
                                                   static_cast<size_t>(memoryProgress * 100),
                                                   "System resource monitoring");
        }
    }
}

StatisticsCollector::AggregatedStatistics StatisticsCollector::getAggregatedStatistics() const {
    AggregatedStatistics aggregated;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    // Aggregate system statistics
    if (!pImpl->systemHistory.empty()) {
        double totalMemory = 0.0;
        double totalCPU = 0.0;
        
        for (const auto& stats : pImpl->systemHistory) {
            totalMemory += stats.memoryUsage;
            totalCPU += stats.cpuUtilization;
        }
        
        aggregated.totalMemoryUsage = static_cast<uint64_t>(totalMemory);
        aggregated.averageCPUUsage = totalCPU / pImpl->systemHistory.size();
    }
    
    // Aggregate neural statistics
    if (!pImpl->neuralHistory.empty()) {
        double totalEfficiency = 0.0;
        for (const auto& stats : pImpl->neuralHistory) {
            totalEfficiency += stats.efficiencyScore;
        }
        aggregated.efficiencyScore = totalEfficiency / pImpl->neuralHistory.size();
    }
    
    return aggregated;
}

StatisticsCollector::ConnectivityAnalysis StatisticsCollector::analyzeNeuralConnectivity(std::shared_ptr<Brain> brain) const {
    ConnectivityAnalysis analysis;
    
    if (!brain) return analysis;
    
    // Analyze brain connectivity
    analysis.totalConnections = brain->getTotalSynapseCount();
    
    if (analysis.totalConnections > 0) {
        // Calculate average connection strength (simplified)
        analysis.averageConnectionStrength = 0.5; // Placeholder
    }
    
    // Count strongly connected components (simplified)
    analysis.stronglyConnectedComponents = 1; // Placeholder
    
    return analysis;
}

StatisticsCollector::LearningDynamics StatisticsCollector::analyzeLearningDynamics(const std::string& testName) const {
    LearningDynamics dynamics;
    
    auto neuralHistory = getNeuralStatisticsHistory();
    for (const auto& stats : neuralHistory) {
        if (testName.empty() || stats.testName == testName) {
            dynamics.learningCurve.push_back(stats.efficiencyScore);
        }
    }
    
    if (!dynamics.learningCurve.empty()) {
        dynamics.learningRate = (dynamics.learningCurve.back() - dynamics.learningCurve.front()) / 
                               dynamics.learningCurve.size();
        dynamics.convergenceScore = dynamics.learningCurve.back();
    }
    
    return dynamics;
}

// Helper functions (need to be defined elsewhere)
double StatisticsCollector::getCurrentCPUUsage() {
    // Implementation for getting current CPU usage
    return 50.0; // Placeholder
}

uint64_t StatisticsCollector::getCurrentMemoryUsagePercentage() {
    // Implementation for getting current memory usage percentage
    return 50; // Placeholder
}

std::chrono::milliseconds StatisticsCollector::getCurrentExecutionTime() {
    // Implementation for getting current execution time
    return std::chrono::milliseconds(1000); // Placeholder
}

std::chrono::milliseconds StatisticsCollector::getCurrentWallClockTime() {
    // Implementation for getting current wall clock time
    return std::chrono::milliseconds(1000); // Placeholder
}

} // namespace nlm

// Note: The implementation above includes several placeholder functions that
// would need to be implemented with actual system monitoring code.
// This provides a comprehensive skeleton for the enhanced statistics collection system.