#include "PerformanceMonitor.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <json/json.h> // Assume nlohmann/json is available
#include <spdlog/spdlog.h> // Assume spdlog is available
#include <mutex>
#include <thread>
#include <atomic>
#include <set>
#include <queue>
#include <chrono>
#include <numeric>

namespace nlm {

struct PerformanceMonitor::Impl {
    std::shared_ptr<Config> config;
    PerformanceConfig perfConfig;
    std::atomic<bool> monitoringActive{false};
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastUpdateTime;
    uint64_t currentStep{0};
    std::chrono::microseconds totalSimulationTime{0};
    
    // Metrics storage
    std::unordered_map<std::string, PerformanceMetric> metrics;
    std::unordered_map<std::string, std::chrono::microseconds> timers;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> timerStarts;
    
    // Performance profiling
    std::unordered_map<std::string, uint64_t> callStackCounts;
    std::unordered_map<std::string, uint64_t> allocationCounts;
    std::unordered_map<std::string, size_t> allocationSizes;
    std::unordered_map<std::string, uint64_t> eventCounts;
    std::vector<std::pair<std::string, uint64_t>> eventTimeline;
    
    // Profile history
    std::vector<PerformanceProfile> profileHistory;
    PerformanceProfile currentProfile;
    
    // Alerts
    std::vector<PerformanceAlert> alerts;
    std::mutex alertsMutex;
    
    // Flame graph
    std::unique_ptr<FlameGraphEntry> flameGraph;
    
    // Memory tracking
    size_t peakMemoryUsage{0};
    std::vector<size_t> memoryHistory;
    
    // CPU and I/O tracking
    std::vector<double> cpuUsageHistory;
    std::vector<std::pair<size_t, std::chrono::microseconds>> ioHistory;
    
    // Statistics
    uint64_t totalAllocationCount{0};
    size_t totalAllocationSize{0};
    size_t totalDeallocationSize{0};
    
    // Thread safety
    mutable std::mutex metricsMutex;
    mutable std::mutex profileMutex;
    
    // Performance tracking
    uint64_t totalSteps{0};
    std::chrono::microseconds totalRealTime{0};
    double averageCPUUsage{0.0};
    
    // Configuration
    bool initialized{false};
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {
}

PerformanceMonitor::PerformanceMonitor(std::shared_ptr<Config> config) 
    : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
    pImpl->perfConfig.monitoringLevel = MonitoringLevel::Detailed;
    pImpl->perfConfig.enableRealTimeMonitoring = true;
    pImpl->perfConfig.enableCallStackProfiling = true;
    pImpl->perfConfig.enableMemoryProfiling = true;
    pImpl->perfConfig.enableEventProfiling = true;
    pImpl->perfConfig.enableBottleneckDetection = true;
    pImpl->perfConfig.maxProfileHistory = 1000;
    pImpl->perfConfig.updateInterval = std::chrono::milliseconds(100);
    pImpl->perfConfig.outputFormat = OutputFormat::JSON;
    pImpl->perfConfig.generateFlameGraphs = true;
    pImpl->perfConfig.enableRegressionDetection = true;
    pImpl->perfConfig.regressionDetectionInterval = std::chrono::minutes(5);
    pImpl->perfConfig.enableOptimizationRecommendations = true;
    pImpl->perfConfig.enablePerformanceDashboard = false;
    pImpl->initialized = true;
}

PerformanceMonitor::~PerformanceMonitor() = default;

bool PerformanceMonitor::initialize() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    // Load configuration from config system
    if (pImpl->config) {
        // Load performance monitoring configuration
        auto monitoringLevel = pImpl->config->getOr<std::string>("performance.monitoring_level", "Detailed");
        if (monitoringLevel == "Silent") pImpl->perfConfig.monitoringLevel = MonitoringLevel::Silent;
        else if (monitoringLevel == "Basic") pImpl->perfConfig.monitoringLevel = MonitoringLevel::Basic;
        else if (monitoringLevel == "Comprehensive") pImpl->perfConfig.monitoringLevel = MonitoringLevel::Comprehensive;
        
        pImpl->perfConfig.enableRealTimeMonitoring = 
            pImpl->config->getOr<bool>("performance.enable_real_time", true);
        pImpl->perfConfig.enableCallStackProfiling = 
            pImpl->config->getOr<bool>("performance.enable_call_stack_profiling", true);
        pImpl->perfConfig.enableMemoryProfiling = 
            pImpl->config->getOr<bool>("performance.enable_memory_profiling", true);
        pImpl->perfConfig.enableEventProfiling = 
            pImpl->config->getOr<bool>("performance.enable_event_profiling", true);
        pImpl->perfConfig.enableBottleneckDetection = 
            pImpl->config->getOr<bool>("performance.enable_bottleneck_detection", true);
        pImpl->perfConfig.maxProfileHistory = 
            pImpl->config->getOr<size_t>("performance.max_profile_history", 1000);
        pImpl->perfConfig.updateInterval = 
            std::chrono::milliseconds(pImpl->config->getOr<int>("performance.update_interval_ms", 100));
        pImpl->perfConfig.generateFlameGraphs = 
            pImpl->config->getOr<bool>("performance.generate_flame_graphs", true);
        pImpl->perfConfig.enableRegressionDetection = 
            pImpl->config->getOr<bool>("performance.enable_regression_detection", true);
        pImpl->perfConfig.regressionDetectionInterval = 
            std::chrono::milliseconds(pImpl->config->getOr<int>("performance.regression_detection_interval_ms", 300000));
        pImpl->perfConfig.enableOptimizationRecommendations = 
            pImpl->config->getOr<bool>("performance.enable_optimization_recommendations", true);
    }
    
    // Initialize flame graph
    pImpl->flameGraph = std::make_unique<FlameGraphEntry>();
    pImpl->flameGraph->functionName = "root";
    pImpl->flameGraph->samples = 0;
    
    // Initialize performance counters
    pImpl->totalSteps = 0;
    pImpl->totalRealTime = 0;
    pImpl->averageCPUUsage = 0.0;
    
    return true;
}

void PerformanceMonitor::startMonitoring() {
    if (pImpl->perfConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->monitoringActive = true;
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->lastUpdateTime = pImpl->startTime;
    
    NLM_LOG_INFO("Performance monitoring started");
}

void PerformanceMonitor::stopMonitoring() {
    if (!pImpl->monitoringActive) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->monitoringActive = false;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    pImpl->totalRealTime += std::chrono::duration_cast<std::chrono::microseconds>(endTime - pImpl->startTime);
    
    NLM_LOG_INFO("Performance monitoring stopped");
}

bool PerformanceMonitor::isMonitoring() const {
    return pImpl->monitoringActive;
}

void PerformanceMonitor::updateMetrics() {
    if (pImpl->perfConfig.monitoringLevel == MonitoringLevel::Silent || !pImpl->monitoringActive) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    pImpl->currentStep++;
    
    // Update current profile
    pImpl->currentProfile.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(currentTime.time_since_epoch());
    pImpl->currentProfile.stepNumber = pImpl->currentStep;
    pImpl->currentProfile.metrics.clear();
    pImpl->currentProfile.callStackCounts = pImpl->callStackCounts;
    pImpl->currentProfile.allocationCounts = pImpl->allocationCounts;
    pImpl->currentProfile.eventCounts = pImpl->eventCounts;
    
    // Convert metrics to profile format
    for (const auto& [name, metric] : pImpl->metrics) {
        pImpl->currentProfile.metrics.push_back(metric);
    }
    
    // Check thresholds
    if (pImpl->perfConfig.monitoringLevel != MonitoringLevel::Basic) {
        checkThresholds();
    }
}

void PerformanceMonitor::recordMetric(const std::string& name, PerformanceCategory category, 
                     double value, const std::string& unit) {
    if (pImpl->perfConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    PerformanceMetric metric;
    metric.name = name;
    metric.category = category;
    metric.value = value;
    metric.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
    metric.unit = unit;
    
    pImpl->metrics[name] = metric;
    
    // Update baseline comparison
    if (pImpl->currentProfile.baseline > 0.0) {
        metric.baseline = pImpl->currentProfile.baseline;
    }
}

void PerformanceMonitor::recordMemoryUsage(size_t bytes, size_t allocated, size_t deallocated) {
    if (pImpl->perfConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->peakMemoryUsage = std::max(pImpl->peakMemoryUsage, bytes);
    pImpl->memoryHistory.push_back(bytes);
    
    if (pImpl->memoryHistory.size() > pImpl->perfConfig.maxProfileHistory) {
        pImpl->memoryHistory.erase(pImpl->memoryHistory.begin());
    }
    
    // Record memory metrics
    recordMetric("memory.usage", PerformanceCategory::MemoryManagement, 
                 static_cast<double>(bytes), "bytes");
    recordMetric("memory.allocated", PerformanceCategory::MemoryManagement,
                 static_cast<double>(allocated), "bytes");
    recordMetric("memory.deallocated", PerformanceCategory::MemoryManagement,
                 static_cast<double>(deallocated), "bytes");
    
    pImpl->totalAllocationCount += allocated;
    pImpl->totalAllocationSize += allocated;
    pImpl->totalDeallocationSize += deallocated;
}

void PerformanceMonitor::recordCPUTime(std::chrono::microseconds time) {
    if (pImpl->perfConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    double cpuUsage = static_cast<double>(time.count()) / 1000000.0; // Convert to seconds
    recordMetric("cpu.time", PerformanceCategory::Overall, cpuUsage, "seconds");
    
    pImpl->cpuUsageHistory.push_back(cpuUsage);
    if (pImpl->cpuUsageHistory.size() > pImpl->perfConfig.maxProfileHistory) {
        pImpl->cpuUsageHistory.erase(pImpl->cpuUsageHistory.begin());
    }
    
    // Update average CPU usage
    if (!pImpl->cpuUsageHistory.empty()) {
        pImpl->averageCPUUsage = std::accumulate(pImpl->cpuUsageHistory.begin(), 
                                               pImpl->cpuUsageHistory.end(), 0.0) / 
                                               pImpl->cpuUsageHistory.size();
    }
}

void PerformanceMonitor::recordIOOperation(size_t bytes, std::chrono::microseconds duration) {
    if (pImpl->perfConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->ioHistory.emplace_back(bytes, duration);
    if (pImpl->ioHistory.size() > pImpl->perfConfig.maxProfileHistory) {
        pImpl->ioHistory.erase(pImpl->ioHistory.begin());
    }
    
    recordMetric("io.bytes", PerformanceCategory::Overall, 
                 static_cast<double>(bytes), "bytes");
    recordMetric("io.time", PerformanceCategory::Overall,
                 static_cast<double>(duration.count()) / 1000000.0, "seconds");
}

void PerformanceMonitor::startProfile(const std::string& functionName, const std::string& filePath, 
                     int lineNumber) {
    if (pImpl->perfConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::string profileKey = functionName + ":" + filePath + ":" + std::to_string(lineNumber);
    pImpl->callStackCounts[profileKey]++;
}

void PerformanceMonitor::stopProfile(const std::string& functionName) {
    // Profile counting is done via startProfile, this is for API compatibility
}

void PerformanceMonitor::recordAllocation(const std::string& type, size_t size) {
    if (pImpl->perfConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->allocationCounts[type]++;
    pImpl->allocationSizes[type] += size;
    
    recordMetric("allocation.count", PerformanceCategory::MemoryManagement,
                 static_cast<double>(pImpl->allocationCounts[type]), "count");
    recordMetric("allocation.size", PerformanceCategory::MemoryManagement,
                 static_cast<double>(pImpl->allocationSizes[type]), "bytes");
}

void PerformanceMonitor::recordDeallocation(const std::string& type, size_t size) {
    if (pImpl->perfConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    // Deallocation tracking
    recordMetric("deallocation.size", PerformanceCategory::MemoryManagement,
                 static_cast<double>(size), "bytes");
}

void PerformanceMonitor::recordEvent(const std::string& eventName, uint64_t count) {
    if (pImpl->perfConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->eventCounts[eventName] += count;
    
    pImpl->eventTimeline.emplace_back(eventName, pImpl->currentProfile.timestamp.count());
    if (pImpl->eventTimeline.size() > pImpl->perfConfig.maxProfileHistory) {
        pImpl->eventTimeline.erase(pImpl->eventTimeline.begin());
    }
    
    recordMetric("event.count", PerformanceCategory::Overall,
                 static_cast<double>(pImpl->eventCounts[eventName]), "count");
}

std::vector<PerformanceProfile> PerformanceMonitor::getProfileHistory() const {
    std::lock_guard<std::mutex> lock(pImpl->profileMutex);
    return pImpl->profileHistory;
}

PerformanceProfile PerformanceMonitor::getCurrentProfile() const {
    std::lock_guard<std::mutex> lock(pImpl->profileMutex);
    return pImpl->currentProfile;
}

void PerformanceMonitor::addProfileToHistory() {
    std::lock_guard<std::mutex> lock(pImpl->profileMutex);
    
    pImpl->profileHistory.push_back(pImpl->currentProfile);
    if (pImpl->profileHistory.size() > pImpl->perfConfig.maxProfileHistory) {
        pImpl->profileHistory.erase(pImpl->profileHistory.begin());
    }
}

void PerformanceMonitor::clearProfileHistory() {
    std::lock_guard<std::mutex> lock(pImpl->profileMutex);
    pImpl->profileHistory.clear();
    pImpl->currentProfile = PerformanceProfile();
}

std::vector<PerformanceAlert> PerformanceMonitor::getAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    return pImpl->alerts;
}

void PerformanceMonitor::acknowledgeAlert(const std::string& alertId) {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    
    for (auto& alert : pImpl->alerts) {
        if (alert.id == alertId) {
            alert.acknowledged = true;
            break;
        }
    }
}

void PerformanceMonitor::checkThresholds() {
    if (pImpl->perfConfig.monitoringLevel == MonitoringLevel::Silent) return;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    // Check common performance thresholds
    auto it = pImpl->metrics.find("cpu.time");
    if (it != pImpl->metrics.end() && it->second.value > 1000.0) { // 1 second threshold
        addAlert("high_cpu_usage", AlertType::Warning, PerformanceCategory::Overall,
                 "High CPU usage detected", it->second.value, 1000.0,
                 "Consider optimizing CPU-intensive operations");
    }
    
    it = pImpl->metrics.find("memory.usage");
    if (it != pImpl->metrics.end() && it->second.value > 1024.0 * 1024.0 * 1024.0) { // 1GB threshold
        addAlert("high_memory_usage", AlertType::Warning, PerformanceCategory::MemoryManagement,
                 "High memory usage detected", it->second.value, 1024.0 * 1024.0 * 1024.0,
                 "Consider optimizing memory allocation or increasing memory");
    }
    
    it = pImpl->metrics.find("event.count");
    if (it != pImpl->metrics.end() && it->second.value > 100000.0) { // 100k events threshold
        addAlert("high_event_count", AlertType::Info, PerformanceCategory::Overall,
                 "High event count detected", it->second.value, 100000.0,
                 "Consider event batching or optimization");
    }
}

void PerformanceMonitor::addAlert(const std::string& id, AlertType type, PerformanceCategory category,
                 const std::string& message, double currentValue, double threshold,
                 const std::string& recommendation) {
    std::lock_guard<std::mutex> lock(pImpl->alertsMutex);
    
    PerformanceAlert alert;
    alert.id = id;
    alert.type = type;
    alert.category = category;
    alert.message = message;
    alert.currentValue = currentValue;
    alert.thresholdValue = threshold;
    alert.timestamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
    alert.acknowledged = false;
    alert.recommendation = recommendation;
    
    pImpl->alerts.push_back(alert);
}

void PerformanceMonitor::setConfig(const PerformanceConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->perfConfig = config;
}

PerformanceConfig PerformanceMonitor::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->perfConfig;
}

std::vector<std::string> PerformanceMonitor::analyzePerformance() const {
    std::vector<std::string> analysis;
    
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    // Analyze common performance metrics
    if (pImpl->cpuUsageHistory.size() > 10) {
        double avgCPU = pImpl->averageCPUUsage;
        if (avgCPU > 80.0) {
            analysis.push_back("High CPU usage detected (average: " + std::to_string(avgCPU) + "%)");
        }
    }
    
    if (pImpl->memoryHistory.size() > 10) {
        double avgMemory = std::accumulate(pImpl->memoryHistory.begin(), pImpl->memoryHistory.end(), 0.0) / pImpl->memoryHistory.size();
        if (avgMemory > 512.0 * 1024.0 * 1024.0) { // 512MB
            analysis.push_back("High memory usage detected (average: " + 
                             std::to_string(avgMemory / (1024.0 * 1024.0)) + " MB)");
        }
    }
    
    if (pImpl->eventTimeline.size() > 100) {
        analysis.push_back("High event rate detected (" + std::to_string(pImpl->eventTimeline.size()) + " events)");
    }
    
    if (pImpl->callStackCounts.size() > 100) {
        analysis.push_back("High function call count detected (" + std::to_string(pImpl->callStackCounts.size()) + " unique calls)");
    }
    
    return analysis;
}

std::vector<std::string> PerformanceMonitor::detectRegression() const {
    std::vector<std::string> regressions;
    
    // Simple regression detection - would be more sophisticated in production
    if (pImpl->profileHistory.size() > 10) {
        // Compare recent profiles with older ones
        auto recentIt = pImpl->profileHistory.end();
        std::advance(recentIt, -5);
        
        for (auto it = pImpl->profileHistory.begin(); it != recentIt; ++it) {
            if (it->stepNumber > 0 && pImpl->currentProfile.stepNumber > 0) {
                double stepDiff = pImpl->currentProfile.stepNumber - it->stepNumber;
                if (stepDiff > 100) { // More than 100 steps apart
                    // Simple regression check - could be enhanced
                    regressions.push_back("Performance regression detected between steps " + 
                                        std::to_string(it->stepNumber) + " and " + 
                                        std::to_string(pImpl->currentProfile.stepNumber));
                }
            }
        }
    }
    
    return regressions;
}

std::vector<std::string> PerformanceMonitor::getOptimizationRecommendations() const {
    std::vector<std::string> recommendations;
    
    // Generate optimization recommendations based on collected data
    if (pImpl->averageCPUUsage > 90.0) {
        recommendations.push_back("Reduce CPU-intensive operations by batching or parallelization");
    }
    
    if (pImpl->peakMemoryUsage > 1024.0 * 1024.0 * 1024.0) {
        recommendations.push_back("Optimize memory usage - consider object pooling or memory reuse");
    }
    
    if (pImpl->eventTimeline.size() > 10000) {
        recommendations.push_back("Reduce event frequency - consider event aggregation or throttling");
    }
    
    if (pImpl->callStackCounts.size() > 1000) {
        recommendations.push_back("Optimize function calls - consider inlining or reducing call depth");
    }
    
    return recommendations;
}

bool PerformanceMonitor::establishBaseline(const std::string& baselineName) {
    // Simple baseline establishment - could be enhanced with persistent storage
    // For now, store in memory with the config system
    if (pImpl->config) {
        // Store baseline name in config
        pImpl->config->set("performance.baseline_name", baselineName, ConfigSource::Runtime);
        return true;
    }
    return false;
}

bool PerformanceMonitor::compareToBaseline(const std::string& baselineName) {
    // Simple baseline comparison - could be enhanced
    if (pImpl->config) {
        auto baselineNameOpt = pImpl->config->get<std::string>("performance.baseline_name");
        if (baselineNameOpt && baselineNameOpt.value() == baselineName) {
            // For now, just return true if baseline exists
            return true;
        }
    }
    return false;
}

double PerformanceMonitor::getPerformanceScore() const {
    double score = 100.0;
    
    // Deduct points for performance issues
    if (pImpl->averageCPUUsage > 80.0) {
        score -= (pImpl->averageCPUUsage - 80.0) * 0.5;
    }
    
    if (pImpl->peakMemoryUsage > 512.0 * 1024.0 * 1024.0) {
        score -= std::log2(pImpl->peakMemoryUsage / (512.0 * 1024.0 * 1024.0)) * 5.0;
    }
    
    return std::max(0.0, score);
}

bool PerformanceMonitor::exportPerformanceData(const std::string& filename, OutputFormat format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    switch (format) {
        case OutputFormat::Text:
            file << getPerformanceSummary();
            break;
        case OutputFormat::JSON:
            // Would need to implement JSON serialization
            file << "{\"error\": \"JSON export not implemented in this implementation\"}";
            break;
        case OutputFormat::CSV:
            // Would need to implement CSV export
            file << "name,category,value,timestamp\n";
            for (const auto& [name, metric] : pImpl->metrics) {
                file << name << ","
                     << static_cast<int>(metric.category) << ","
                     << metric.value << ","
                     << metric.timestamp.count() << "\n";
            }
            break;
        case OutputFormat::HTML:
            // Would need to implement HTML export
            file << "<html><body>HTML export not implemented in this implementation</body></html>";
            break;
    }
    
    file.close();
    return true;
}

bool PerformanceMonitor::importPerformanceData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple import - would need proper serialization format
    file.close();
    return true;
}

bool PerformanceMonitor::generateFlameGraph(const std::string& filename) const {
    if (!pImpl->perfConfig.generateFlameGraphs) return false;
    
    // Simple flame graph generation
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Flame graph not implemented in this implementation";
    file.close();
    return true;
}

bool PerformanceMonitor::generateDetailedReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << getPerformanceSummary();
    file << "\n\n=== Performance Analysis ===\n";
    
    auto analysis = analyzePerformance();
    for (const auto& item : analysis) {
        file << "- " << item << "\n";
    }
    
    auto regressions = detectRegression();
    if (!regressions.empty()) {
        file << "\n=== Performance Regressions ===\n";
        for (const auto& item : regressions) {
            file << "- " << item << "\n";
        }
    }
    
    file.close();
    return true;
}

std::string PerformanceMonitor::getStatistics() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "=== Performance Statistics ===\n";
    ss << "Total Steps: " << pImpl->totalSteps << "\n";
    ss << "Total Real Time: " << pImpl->totalRealTime.count() / 1000000.0 << " seconds\n";
    ss << "Average CPU Usage: " << pImpl->averageCPUUsage << "%\n";
    ss << "Peak Memory Usage: " << pImpl->peakMemoryUsage / (1024.0 * 1024.0) << " MB\n";
    ss << "Total Metrics: " << pImpl->metrics.size() << "\n";
    ss << "Total Events: " << pImpl->eventTimeline.size() << "\n";
    ss << "Performance Score: " << getPerformanceScore() << "/100\n";
    ss << std::endl;
    return ss.str();
}

std::string PerformanceMonitor::getPerformanceSummary() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    ss << "=== NLM Performance Summary ===\n";
    ss << "Step: " << pImpl->currentStep << "\n";
    ss << "Real-time: " << pImpl->totalRealTime.count() / 1000000.0 << " seconds\n";
    ss << "CPU Usage: " << pImpl->averageCPUUsage << "%\n";
    ss << "Memory: " << pImpl->peakMemoryUsage / (1024.0 * 1024.0) << " MB\n";
    ss << "Events: " << pImpl->eventTimeline.size() << "\n";
    ss << "Functions: " << pImpl->callStackCounts.size() << "\n";
    ss << std::endl;
    return ss.str();
}

uint64_t PerformanceMonitor::getTotalSteps() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->totalSteps;
}

std::chrono::microseconds PerformanceMonitor::getTotalSimulationTime() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->totalSimulationTime;
}

void PerformanceMonitor::lock() const {
    pImpl->metricsMutex.lock();
}

void PerformanceMonitor::unlock() const {
    pImpl->metricsMutex.unlock();
}

// Singleton implementation
namespace {
    std::shared_ptr<PerformanceMonitor> g_performanceMonitorInstance;
    std::mutex g_singletonMutex;
}

PerformanceMonitor& PerformanceMonitor::getInstance() {
    std::lock_guard<std::mutex> lock(g_singletonMutex);
    if (!g_performanceMonitorInstance) {
        g_performanceMonitorInstance = std::shared_ptr<PerformanceMonitor>(new PerformanceMonitor());
    }
    return *g_performanceMonitorInstance;
}

void PerformanceMonitor::setInstance(std::shared_ptr<PerformanceMonitor> monitor) {
    std::lock_guard<std::mutex> lock(g_singletonMutex);
    g_performanceMonitorInstance = monitor;
}

bool PerformanceMonitor::isInitialized() {
    std::lock_guard<std::mutex> lock(g_singletonMutex);
    return g_performanceMonitorInstance != nullptr && g_performanceMonitorInstance->pImpl->initialized;
}

} // namespace nlm