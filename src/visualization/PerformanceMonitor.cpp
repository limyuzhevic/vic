#include "PerformanceMonitor.hpp"
#include "VisualizationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <cmath>

using json = nlohmann::json;

namespace nlm {

struct PerformanceMonitor::Impl {
    // Core configuration
    std::shared_ptr<VisualizationManager> visualizationManager;
    std::atomic<bool> resourceMonitoringActive;
    std::atomic<bool> processMetricsActive;
    std::atomic<bool> alertingEnabled;
    std::atomic<bool> monitoringPaused;
    double monitoringInterval;
    size_t historySize;
    std::chrono::steady_clock::time_point lastUpdate;
    
    // Resource usage tracking
    ResourceUsage currentResourceUsage;
    std::vector<ResourceUsage> resourceUsageHistory;
    std::mutex resourceMutex;
    
    // Process metrics tracking
    ProcessMetrics currentProcessMetrics;
    std::vector<ProcessMetrics> processMetricsHistory;
    std::mutex processMutex;
    
    // Benchmarking
    std::unordered_map<std::string, std::vector<BenchmarkResult>> benchmarkResults;
    std::mutex benchmarkMutex;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> activeBenchmarks;
    
    // Performance alerting
    std::unordered_map<std::string, double> performanceThresholds;
    std::vector<std::pair<std::string, double>> performanceAlerts;
    std::mutex alertMutex;
    
    // Performance threads
    std::thread resourceThread;
    std::thread processThread;
    std::mutex threadsMutex;
    
    Impl(std::shared_ptr<VisualizationManager> manager)
        : visualizationManager(manager)
        , resourceMonitoringActive(false)
        , processMetricsActive(false)
        , alertingEnabled(false)
        , monitoringPaused(false)
        , monitoringInterval(1.0) // 1 second
        , historySize(1000)
        , lastUpdate(std::chrono::steady_clock::now()) {
        
        // Initialize resource usage
        currentResourceUsage = getCurrentResourceUsage();
        
        // Initialize performance thresholds
        performanceThresholds["cpu_usage"] = 80.0;
        performanceThresholds["memory_usage"] = 90.0;
        performanceThresholds["simulation_fps"] = 30.0;
        performanceThresholds["visualization_fps"] = 30.0;
    }
};

PerformanceMonitor::PerformanceMonitor(std::shared_ptr<VisualizationManager> manager)
    : pImpl(std::make_unique<Impl>(manager)) {
    LOG_INFO("PerformanceMonitor initialized");
}

PerformanceMonitor::~PerformanceMonitor() {
    stopResourceMonitoring();
    stopProcessMetrics();
    LOG_INFO("PerformanceMonitor destroyed");
}

PerformanceMonitor::PerformanceMonitor(PerformanceMonitor&& other) noexcept
    : pImpl(std::move(other.pImpl)) {
}

PerformanceMonitor& PerformanceMonitor::operator=(PerformanceMonitor&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool PerformanceMonitor::startResourceMonitoring() {
    if (pImpl->resourceMonitoringActive) {
        return true;
    }
    
    LOG_INFO("Starting resource monitoring");
    
    pImpl->resourceMonitoringActive = true;
    
    // Start resource monitoring thread
    pImpl->resourceThread = std::thread([this]() {
        monitorResources();
    });
    
    return true;
}

void PerformanceMonitor::stopResourceMonitoring() {
    if (!pImpl->resourceMonitoringActive) {
        return;
    }
    
    LOG_INFO("Stopping resource monitoring");
    
    pImpl->resourceMonitoringActive = false;
    
    if (pImpl->resourceThread.joinable()) {
        pImpl->resourceThread.join();
    }
}

bool PerformanceMonitor::isResourceMonitoringActive() const {
    return pImpl->resourceMonitoringActive;
}

PerformanceMonitor::ResourceUsage PerformanceMonitor::getCurrentResourceUsage() const {
    ResourceUsage usage;
    
    // Get process resource usage
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    
    // Convert to percentages
    usage.cpuUser = (ru.ru_utime.tv_sec * 1000.0 + ru.ru_utime.tv_usec / 1000.0) / 1000.0;
    usage.cpuSystem = (ru.ru_stime.tv_sec * 1000.0 + ru.ru_stime.tv_usec / 1000.0) / 1000.0;
    
    // Get memory information
    // This is a simplified implementation - in production, would use system-specific APIs
    usage.memoryTotal = 8ULL * 1024 * 1024 * 1024; // 8GB
    usage.memoryUsed = ru.ru_maxrss * 1024; // Convert from KB to bytes
    usage.memoryAvailable = usage.memoryTotal - usage.memoryUsed;
    
    // Mock I/O speeds
    usage.diskReadSpeed = 100.0; // MB/s
    usage.diskWriteSpeed = 80.0; // MB/s
    usage.networkUploadSpeed = 50.0; // Mbps
    usage.networkDownloadSpeed = 100.0; // Mbps
    
    return usage;
}

std::vector<PerformanceMonitor::ResourceUsage> PerformanceMonitor::getResourceUsageHistory(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->resourceMutex);
    
    if (limit == 0 || limit > pImpl->resourceUsageHistory.size()) {
        return pImpl->resourceUsageHistory;
    }
    
    std::vector<ResourceUsage> result;
    result.reserve(limit);
    
    size_t startIndex = pImpl->resourceUsageHistory.size() - limit;
    for (size_t i = startIndex; i < pImpl->resourceUsageHistory.size(); ++i) {
        result.push_back(pImpl->resourceUsageHistory[i]);
    }
    
    return result;
}

void PerformanceMonitor::monitorResources() {
    const auto updateInterval = std::chrono::milliseconds(static_cast<int>(pImpl->monitoringInterval * 1000));
    
    while (pImpl->resourceMonitoringActive && !pImpl->monitoringPaused) {
        auto startTime = std::chrono::steady_clock::now();
        
        // Collect resource metrics
        ResourceUsage usage = getCurrentResourceUsage();
        
        // Update current metrics
        {
            std::lock_guard<std::mutex> lock(pImpl->resourceMutex);
            pImpl->currentResourceUsage = usage;
            
            if (pImpl->resourceUsageHistory.size() > pImpl->historySize) {
                pImpl->resourceUsageHistory.erase(pImpl->resourceUsageHistory.begin());
            }
            pImpl->resourceUsageHistory.push_back(usage);
        }
        
        // Check alerts
        checkResourceAlerts(usage);
        
        // Sleep for the remainder of the interval
        auto endTime = std::chrono::steady_clock::now();
        auto sleepTime = updateInterval - (endTime - startTime);
        if (sleepTime > std::chrono::milliseconds(0)) {
            std::this_thread::sleep_for(sleepTime);
        }
    }
}

void PerformanceMonitor::checkResourceAlerts(const ResourceUsage& usage) {
    if (!pImpl->alertingEnabled) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->alertMutex);
    
    for (const auto& alert : pImpl->performanceAlerts) {
        const std::string& metricName = alert.first;
        double threshold = alert.second;
        
        double currentValue = 0.0;
        
        if (metricName == "cpu_usage") {
            currentValue = (usage.cpuUser + usage.cpuSystem) / 100.0; // Convert to percentage
        } else if (metricName == "memory_usage") {
            currentValue = static_cast<double>(usage.memoryUsed) / usage.memoryTotal * 100.0;
        }
        
        if (currentValue > threshold) {
            LOG_WARN("Performance alert: " + metricName + " is " + std::to_string(currentValue) +
                    "% (threshold: " + std::to_string(threshold) + "%)");
        }
    }
}

bool PerformanceMonitor::startProcessMetrics() {
    if (pImpl->processMetricsActive) {
        return true;
    }
    
    LOG_INFO("Starting process metrics collection");
    
    pImpl->processMetricsActive = true;
    
    // Start process metrics thread
    pImpl->processThread = std::thread([this]() {
        monitorProcessMetrics();
    });
    
    return true;
}

void PerformanceMonitor::stopProcessMetrics() {
    if (!pImpl->processMetricsActive) {
        return;
    }
    
    LOG_INFO("Stopping process metrics collection");
    
    pImpl->processMetricsActive = false;
    
    if (pImpl->processThread.joinable()) {
        pImpl->processThread.join();
    }
}

bool PerformanceMonitor::isProcessMetricsActive() const {
    return pImpl->processMetricsActive;
}

PerformanceMonitor::ProcessMetrics PerformanceMonitor::getCurrentProcessMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->processMutex);
    return pImpl->currentProcessMetrics;
}

std::vector<PerformanceMonitor::ProcessMetrics> PerformanceMonitor::getProcessMetricsHistory(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->processMutex);
    
    if (limit == 0 || limit > pImpl->processMetricsHistory.size()) {
        return pImpl->processMetricsHistory;
    }
    
    std::vector<ProcessMetrics> result;
    result.reserve(limit);
    
    size_t startIndex = pImpl->processMetricsHistory.size() - limit;
    for (size_t i = startIndex; i < pImpl->processMetricsHistory.size(); ++i) {
        result.push_back(pImpl->processMetricsHistory[i]);
    }
    
    return result;
}

void PerformanceMonitor::monitorProcessMetrics() {
    const auto updateInterval = std::chrono::milliseconds(static_cast<int>(pImpl->monitoringInterval * 1000));
    
    while (pImpl->processMetricsActive && !pImpl->monitoringPaused) {
        auto startTime = std::chrono::steady_clock::now();
        
        // Collect process metrics
        ProcessMetrics metrics;
        metrics.timestamp = std::chrono::system_clock::now();
        
        // Mock metrics (in production, would use actual system calls)
        metrics.cpuUsage = 45.0 + (rand() % 30); // 45-75%
        metrics.memoryUsage = 60.0 + (rand() % 20); // 60-80%
        metrics.gpuUsage = 30.0 + (rand() % 40); // 30-70%
        metrics.memoryAllocated = 1024 * 1024 * 1024; // 1GB
        metrics.memoryPeak = 2048 * 1024 * 1024; // 2GB
        metrics.networkThroughput = 10.0 + (rand() % 20); // MB/s
        metrics.simulationFPS = 60.0 + (rand() % 20); // 60-80 fps
        metrics.visualizationFPS = 30.0 + (rand() % 15); // 30-45 fps
        
        // Update current metrics
        {
            std::lock_guard<std::mutex> lock(pImpl->processMutex);
            pImpl->currentProcessMetrics = metrics;
            
            if (pImpl->processMetricsHistory.size() > pImpl->historySize) {
                pImpl->processMetricsHistory.erase(pImpl->processMetricsHistory.begin());
            }
            pImpl->processMetricsHistory.push_back(metrics);
        }
        
        // Sleep for the remainder of the interval
        auto endTime = std::chrono::steady_clock::now();
        auto sleepTime = updateInterval - (endTime - startTime);
        if (sleepTime > std::chrono::milliseconds(0)) {
            std::this_thread::sleep_for(sleepTime);
        }
    }
}

bool PerformanceMonitor::startBenchmarking(const std::string& benchmarkName) {
    LOG_INFO("Starting benchmarking: " + benchmarkName);
    
    std::lock_guard<std::mutex> lock(pImpl->benchmarkMutex);
    
    pImpl->activeBenchmarks[benchmarkName] = std::chrono::steady_clock::now();
    
    return true;
}

void PerformanceMonitor::stopBenchmarking(const std::string& benchmarkName) {
    LOG_INFO("Stopping benchmarking: " + benchmarkName);
    
    std::lock_guard<std::mutex> lock(pImpl->benchmarkMutex);
    
    auto it = pImpl->activeBenchmarks.find(benchmarkName);
    if (it != pImpl->activeBenchmarks.end()) {
        pImpl->activeBenchmarks.erase(it);
    }
}

bool PerformanceMonitor::isBenchmarking(const std::string& benchmarkName) const {
    std::lock_guard<std::mutex> lock(pImpl->benchmarkMutex);
    return pImpl->activeBenchmarks.find(benchmarkName) != pImpl->activeBenchmarks.end();
}

PerformanceMonitor::BenchmarkResult PerformanceMonitor::runBenchmark(const std::string& benchmarkName, std::function<void()> benchmarkFunction) {
    LOG_INFO("Running benchmark: " + benchmarkName);
    
    BenchmarkResult result;
    result.benchmarkName = benchmarkName;
    result.timestamp = std::chrono::system_clock::now();
    
    // Get current resource usage before benchmark
    ResourceUsage before = getCurrentResourceUsage();
    
    // Run benchmark function
    auto startTime = std::chrono::steady_clock::now();
    benchmarkFunction();
    auto endTime = std::chrono::steady_clock::now();
    
    // Get current resource usage after benchmark
    ResourceUsage after = getCurrentResourceUsage();
    
    // Calculate metrics
    result.executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    result.memoryUsage = static_cast<double>(after.memoryUsed - before.memoryUsed) / 1024.0 / 1024.0; // MB
    result.cpuUsage = (after.cpuUser + after.cpuSystem - before.cpuUser - before.cpuSystem) / result.executionTime;
    
    // Store result
    {
        std::lock_guard<std::mutex> lock(pImpl->benchmarkMutex);
        pImpl->benchmarkResults[benchmarkName].push_back(result);
        
        if (pImpl->benchmarkResults[benchmarkName].size() > 100) {
            pImpl->benchmarkResults[benchmarkName].erase(pImpl->benchmarkResults[benchmarkName].begin());
        }
    }
    
    LOG_INFO("Benchmark completed: " + benchmarkName + " took " + std::to_string(result.executionTime) + " seconds");
    
    return result;
}

std::vector<PerformanceMonitor::BenchmarkResult> PerformanceMonitor::getBenchmarkResults(const std::string& benchmarkName) const {
    std::lock_guard<std::mutex> lock(pImpl->benchmarkMutex);
    
    if (benchmarkName.empty()) {
        // Return all results
        std::vector<BenchmarkResult> allResults;
        for (const auto& pair : pImpl->benchmarkResults) {
            allResults.insert(allResults.end(), pair.second.begin(), pair.second.end());
        }
        return allResults;
    }
    
    auto it = pImpl->benchmarkResults.find(benchmarkName);
    if (it != pImpl->benchmarkResults.end()) {
        return it->second;
    }
    
    return std::vector<BenchmarkResult>();
}

void PerformanceMonitor::analyzePerformanceTrends() {
    LOG_INFO("Analyzing performance trends");
    
    // Analyze trends in resource usage and process metrics
    {
        std::lock_guard<std::mutex> lock(pImpl->resourceMutex);
        if (pImpl->resourceUsageHistory.size() >= 10) {
            analyzeResourceTrends();
        }
    }
    
    {
        std::lock_guard<std::mutex> lock(pImpl->processMutex);
        if (pImpl->processMetricsHistory.size() >= 10) {
            analyzeProcessTrends();
        }
    }
}

void PerformanceMonitor::generatePerformanceReport(const std::string& reportPath) {
    LOG_INFO("Generating performance report: " + reportPath);
    
    json report;
    
    // Add resource usage statistics
    {
        std::lock_guard<std::mutex> lock(pImpl->resourceMutex);
        report["resource_usage"] = getResourceStatistics();
    }
    
    // Add process metrics statistics
    {
        std::lock_guard<std::mutex> lock(pImpl->processMutex);
        report["process_metrics"] = getProcessStatistics();
    }
    
    // Add benchmark results
    {
        std::lock_guard<std::mutex> lock(pImpl->benchmarkMutex);
        json benchmarks = json::array();
        for (const auto& pair : pImpl->benchmarkResults) {
            json benchmarkArray = json::array();
            for (const auto& result : pair.second) {
                json resultJson;
                resultJson["name"] = result.benchmarkName;
                resultJson["execution_time"] = result.executionTime;
                resultJson["memory_usage"] = result.memoryUsage;
                resultJson["cpu_usage"] = result.cpuUsage;
                resultJson["timestamp"] = std::chrono::system_clock::to_time_t(result.timestamp);
                benchmarkArray.push_back(resultJson);
            }
            report["benchmarks"][pair.first] = benchmarkArray;
        }
    }
    
    // Write report to file
    std::ofstream file(reportPath);
    if (file.is_open()) {
        file << report.dump(2);
        file.close();
        LOG_INFO("Performance report generated: " + reportPath);
    } else {
        LOG_ERROR("Failed to generate performance report");
    }
}

json PerformanceMonitor::getPerformanceStatistics() const {
    json stats;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->resourceMutex);
        stats["resource_usage"] = calculateResourceStatistics(pImpl->resourceUsageHistory);
    }
    
    {
        std::lock_guard<std::mutex> lock(pImpl->processMutex);
        stats["process_metrics"] = calculateProcessStatistics(pImpl->processMetricsHistory);
    }
    
    return stats;
}

json PerformanceMonitor::getResourceStatistics() const {
    return calculateResourceStatistics(pImpl->resourceUsageHistory);
}

bool PerformanceMonitor::enablePerformanceAlerting() {
    LOG_INFO("Enabling performance alerting");
    pImpl->alertingEnabled = true;
    return true;
}

void PerformanceMonitor::disablePerformanceAlerting() {
    LOG_INFO("Disabling performance alerting");
    pImpl->alertingEnabled = false;
}

bool PerformanceMonitor::isAlertingEnabled() const {
    return pImpl->alertingEnabled;
}

void PerformanceMonitor::setPerformanceThresholds(const std::unordered_map<std::string, double>& thresholds) {
    std::lock_guard<std::mutex> lock(pImpl->alertMutex);
    pImpl->performanceThresholds = thresholds;
    
    // Update alerts based on new thresholds
    pImpl->performanceAlerts.clear();
    for (const auto& pair : thresholds) {
        pImpl->performanceAlerts.push_back({pair.first, pair.second});
    }
}

void PerformanceMonitor::addPerformanceAlert(const std::string& metricName, double threshold, const std::string& message) {
    std::lock_guard<std::mutex> lock(pImpl->alertMutex);
    pImpl->performanceAlerts.push_back({metricName, threshold});
    LOG_INFO("Performance alert added: " + metricName + " > " + std::to_string(threshold) + " - " + message);
}

void PerformanceMonitor::integrateWithVisualization(std::shared_ptr<VisualizationManager> manager) {
    pImpl->visualizationManager = manager;
    LOG_INFO("PerformanceMonitor integrated with VisualizationManager");
}

void PerformanceMonitor::setMonitoringInterval(double interval) {
    if (interval <= 0.0) {
        LOG_WARN("Invalid monitoring interval, using default 1.0");
        interval = 1.0;
    }
    pImpl->monitoringInterval = interval;
}

void PerformanceMonitor::setHistorySize(size_t size) {
    if (size == 0) {
        LOG_WARN("Invalid history size, using default 1000");
        size = 1000;
    }
    pImpl->historySize = size;
}

void PerformanceMonitor::setAlertThresholds(const std::unordered_map<std::string, double>& thresholds) {
    setPerformanceThresholds(thresholds);
}

void PerformanceMonitor::loadConfiguration(const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open configuration file: " + configPath);
        return;
    }
    
    try {
        json config = json::parse(file);
        
        if (config.contains("monitoring_interval")) {
            setMonitoringInterval(config["monitoring_interval"]);
        }
        if (config.contains("history_size")) {
            setHistorySize(config["history_size"]);
        }
        if (config.contains("alerting_enabled")) {
            enablePerformanceAlerting();
        }
        
        file.close();
        LOG_INFO("Performance monitor configuration loaded: " + configPath);
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load performance monitor configuration: " + std::string(e.what()));
    }
}

void PerformanceMonitor::saveConfiguration() const {
    json config;
    
    config["monitoring_interval"] = pImpl->monitoringInterval;
    config["history_size"] = pImpl->historySize;
    config["alerting_enabled"] = pImpl->alertingEnabled;
    
    std::string configPath = "performance_monitor_config.json";
    std::ofstream file(configPath);
    if (file.is_open()) {
        file << config.dump(2);
        file.close();
        LOG_INFO("Performance monitor configuration saved: " + configPath);
    } else {
        LOG_ERROR("Failed to save performance monitor configuration");
    }
}

std::string PerformanceMonitor::getConfigurationJSON() const {
    json config;
    
    config["monitoring_interval"] = pImpl->monitoringInterval;
    config["history_size"] = pImpl->historySize;
    config["alerting_enabled"] = pImpl->alertingEnabled;
    
    return config.dump(2);
}

void PerformanceMonitor::pauseMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->threadsMutex);
    pImpl->monitoringPaused = true;
    LOG_INFO("Performance monitoring paused");
}

void PerformanceMonitor::resumeMonitoring() {
    std::lock_guard<std::mutex> lock(pImpl->threadsMutex);
    pImpl->monitoringPaused = false;
    LOG_INFO("Performance monitoring resumed");
}

bool PerformanceMonitor::isMonitoringPaused() const {
    return pImpl->monitoringPaused;
}

void PerformanceMonitor::resetMetrics() {
    std::lock_guard<std::mutex> resourceLock(pImpl->resourceMutex);
    std::lock_guard<std::mutex> processLock(pImpl->processMutex);
    std::lock_guard<std::mutex> benchmarkLock(pImpl->benchmarkMutex);
    
    pImpl->resourceUsageHistory.clear();
    pImpl->processMetricsHistory.clear();
    pImpl->benchmarkResults.clear();
    
    LOG_INFO("Performance metrics reset");
}

bool PerformanceMonitor::exportMetrics(const std::string& filepath) {
    json data;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->resourceMutex);
        data["resource_usage"] = pImpl->resourceUsageHistory;
    }
    
    {
        std::lock_guard<std::mutex> lock(pImpl->processMutex);
        data["process_metrics"] = pImpl->processMetricsHistory;
    }
    
    {
        std::lock_guard<std::mutex> lock(pImpl->benchmarkMutex);
        json benchmarks = json::array();
        for (const auto& pair : pImpl->benchmarkResults) {
            for (const auto& result : pair.second) {
                json resultJson;
                resultJson["benchmark_name"] = result.benchmarkName;
                resultJson["execution_time"] = result.executionTime;
                resultJson["memory_usage"] = result.memoryUsage;
                resultJson["cpu_usage"] = result.cpuUsage;
                resultJson["timestamp"] = std::chrono::system_clock::to_time_t(result.timestamp);
                benchmarks.push_back(resultJson);
            }
        }
        data["benchmark_results"] = benchmarks;
    }
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << data.dump(2);
        file.close();
        LOG_INFO("Metrics exported to: " + filepath);
        return true;
    }
    
    LOG_ERROR("Failed to export metrics");
    return false;
}

bool PerformanceMonitor::importMetrics(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open file for import: " + filepath);
        return false;
    }
    
    try {
        json data = json::parse(file);
        
        // Import resource usage
        if (data.contains("resource_usage")) {
            std::lock_guard<std::mutex> lock(pImpl->resourceMutex);
            pImpl->resourceUsageHistory = data["resource_usage"].get<std::vector<ResourceUsage>>();
        }
        
        // Import process metrics
        if (data.contains("process_metrics")) {
            std::lock_guard<std::mutex> lock(pImpl->processMutex);
            pImpl->processMetricsHistory = data["process_metrics"].get<std::vector<ProcessMetrics>>();
        }
        
        // Import benchmark results
        if (data.contains("benchmark_results")) {
            std::lock_guard<std::mutex> lock(pImpl->benchmarkMutex);
            for (const auto& resultJson : data["benchmark_results"]) {
                BenchmarkResult result;
                result.benchmarkName = resultJson["benchmark_name"];
                result.executionTime = resultJson["execution_time"];
                result.memoryUsage = resultJson["memory_usage"];
                result.cpuUsage = resultJson["cpu_usage"];
                result.timestamp = std::chrono::system_clock::from_time_t(resultJson["timestamp"].get<time_t>());
                
                std::string benchmarkName = result.benchmarkName;
                pImpl->benchmarkResults[benchmarkName].push_back(result);
                
                if (pImpl->benchmarkResults[benchmarkName].size() > 100) {
                    pImpl->benchmarkResults[benchmarkName].erase(pImpl->benchmarkResults[benchmarkName].begin());
                }
            }
        }
        
        file.close();
        LOG_INFO("Metrics imported from: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to import metrics: " + std::string(e.what()));
        return false;
    }
}

json PerformanceMonitor::getAllMetrics() const {
    json data;
    
    {
        std::lock_guard<std::mutex> lock(pImpl->resourceMutex);
        data["current_resource_usage"] = pImpl->currentResourceUsage;
        data["resource_usage_history"] = pImpl->resourceUsageHistory;
    }
    
    {
        std::lock_guard<std::mutex> lock(pImpl->processMutex);
        data["current_process_metrics"] = pImpl->currentProcessMetrics;
        data["process_metrics_history"] = pImpl->processMetricsHistory;
    }
    
    {
        std::lock_guard<std::mutex> lock(pImpl->benchmarkMutex);
        data["benchmark_results"] = pImpl->benchmarkResults;
    }
    
    return data;
}

// Helper functions

json PerformanceMonitor::calculateResourceStatistics(const std::vector<ResourceUsage>& usageHistory) {
    json stats;
    
    if (usageHistory.empty()) return stats;
    
    double totalCpuUser = 0.0, totalCpuSystem = 0.0;
    uint64_t totalMemoryUsed = 0;
    size_t count = usageHistory.size();
    
    for (const auto& usage : usageHistory) {
        totalCpuUser += usage.cpuUser;
        totalCpuSystem += usage.cpuSystem;
        totalMemoryUsed += usage.memoryUsed;
    }
    
    stats["cpu_user_avg"] = totalCpuUser / count;
    stats["cpu_system_avg"] = totalCpuSystem / count;
    stats["cpu_total_avg"] = (totalCpuUser + totalCpuSystem) / count;
    stats["memory_used_avg"] = static_cast<double>(totalMemoryUsed) / count / 1024.0 / 1024.0; // MB
    
    return stats;
}

json PerformanceMonitor::calculateProcessStatistics(const std::vector<ProcessMetrics>& metricsHistory) {
    json stats;
    
    if (metricsHistory.empty()) return stats;
    
    double totalCpu = 0.0, totalMemory = 0.0, totalGpu = 0.0, totalNetwork = 0.0;
    double totalSimFPS = 0.0, totalVizFPS = 0.0;
    size_t count = metricsHistory.size();
    
    for (const auto& metrics : metricsHistory) {
        totalCpu += metrics.cpuUsage;
        totalMemory += metrics.memoryUsage;
        totalGpu += metrics.gpuUsage;
        totalNetwork += metrics.networkThroughput;
        totalSimFPS += metrics.simulationFPS;
        totalVizFPS += metrics.visualizationFPS;
    }
    
    stats["cpu_usage_avg"] = totalCpu / count;
    stats["memory_usage_avg"] = totalMemory / count;
    stats["gpu_usage_avg"] = totalGpu / count;
    stats["network_throughput_avg"] = totalNetwork / count;
    stats["simulation_fps_avg"] = totalSimFPS / count;
    stats["visualization_fps_avg"] = totalVizFPS / count;
    
    return stats;
}

void PerformanceMonitor::analyzeResourceTrends() {
    LOG_DEBUG("Analyzing resource usage trends");
    
    // Simple trend analysis
    if (pImpl->resourceUsageHistory.size() < 10) return;
    
    double initialCpu = pImpl->resourceUsageHistory[0].cpuUser + pImpl->resourceUsageHistory[0].cpuSystem;
    double finalCpu = pImpl->resourceUsageHistory.back().cpuUser + pImpl->resourceUsageHistory.back().cpuSystem;
    
    double cpuTrend = (finalCpu - initialCpu) / 10.0; // Per sample
    
    if (std::abs(cpuTrend) > 5.0) {
        LOG_WARN("High CPU trend detected: " + std::to_string(cpuTrend) + "% per sample");
    }
}

void PerformanceMonitor::analyzeProcessTrends() {
    LOG_DEBUG("Analyzing process metrics trends");
    
    // Simple trend analysis
    if (pImpl->processMetricsHistory.size() < 10) return;
    
    double initialFps = pImpl->processMetricsHistory[0].simulationFPS;
    double finalFps = pImpl->processMetricsHistory.back().simulationFPS;
    
    double fpsTrend = (finalFps - initialFps) / 10.0; // Per sample
    
    if (fpsTrend < -5.0) {
        LOG_WARN("High simulation FPS trend detected: " + std::to_string(fpsTrend) + " FPS per sample");
    }
}

} // namespace nlm
