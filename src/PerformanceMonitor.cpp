#include "PerformanceMonitor.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>

namespace nlm {

PerformanceMetricsCollector::PerformanceMetricsCollector()
    : totalOperations_(0) {
    lastFrameTime_ = std::chrono::high_resolution_clock::now();
    currentMetrics_.timestamp = getCurrentTime();
}

void PerformanceMetricsCollector::recordMetrics(const PerformanceMetrics& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    metricsHistory_.push_back(metrics);
    
    // Keep only recent metrics (last 10,000 samples)
    if (metricsHistory_.size() > 10000) {
        metricsHistory_.erase(metricsHistory_.begin());
    }
    
    currentMetrics_ = metrics;
    totalOperations_ += metrics.operationsCount;
}

PerformanceMetrics PerformanceMetricsCollector::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentMetrics_;
}

std::vector<PerformanceMetrics> PerformanceMetricsCollector::getMetricsHistory() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return metricsHistory_;
}

double PerformanceMetricsCollector::calculateAverageFrameTime() const {
    if (metricsHistory_.empty()) return 0.0;
    
    double totalFrameTime = 0.0;
    for (const auto& metrics : metricsHistory_) {
        totalFrameTime += metrics.frameTime;
    }
    
    return totalFrameTime / metricsHistory_.size();
}

double PerformanceMetricsCollector::calculateFrameTimeVariance() const {
    if (metricsHistory_.size() < 2) return 0.0;
    
    double mean = calculateAverageFrameTime();
    double variance = 0.0;
    
    for (const auto& metrics : metricsHistory_) {
        double diff = metrics.frameTime - mean;
        variance += diff * diff;
    }
    
    return variance / metricsHistory_.size();
}

double PerformanceMetricsCollector::calculateThroughput() const {
    if (metricsHistory_.empty()) return 0.0;
    
    double totalTime = 0.0;
    double totalOperations = 0.0;
    
    for (const auto& metrics : metricsHistory_) {
        totalTime += metrics.frameTime;
        totalOperations += metrics.operationsCount;
    }
    
    return totalTime > 0.0 ? totalOperations / totalTime : 0.0;
}

uint64_t PerformanceMetricsCollector::getTotalOperations() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalOperations_;
}

void PerformanceMetricsCollector::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    metricsHistory_.clear();
    currentMetrics_ = PerformanceMetrics();
    currentMetrics_.timestamp = getCurrentTime();
    totalOperations_ = 0;
}

double PerformanceMetricsCollector::getCurrentTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - lastFrameTime_);
    return duration.count();
}

PerformanceAnalyzer::PerformanceAnalyzer() {
    // Initialize analyzer with default thresholds
}

std::vector<std::string> PerformanceAnalyzer::analyzeTrends(const std::vector<PerformanceMetrics>& metrics) const {
    std::vector<std::string> analysisResults;
    
    if (metrics.size() < 10) {
        return analysisResults; // Not enough data for meaningful analysis
    }
    
    // Analyze frame time trends
    double currentMeanFrameTime = calculateAverageFrameTime(metrics);
    double currentVarianceFrameTime = calculateFrameTimeVariance(metrics);
    
    if (currentMeanFrameTime > TARGET_FRAME_TIME * 1.5) {
        analysisResults.push_back("WARNING: Frame time is significantly above target (current: " + 
                               std::to_string(currentMeanFrameTime) + "ms, target: " + 
                               std::to_string(TARGET_FRAME_TIME) + "ms)");
    } else if (currentMeanFrameTime > TARGET_FRAME_TIME * 1.2) {
        analysisResults.push_back("NOTICE: Frame time is above target (current: " + 
                               std::to_string(currentMeanFrameTime) + "ms, target: " + 
                               std::to_string(TARGET_FRAME_TIME) + "ms)");
    }
    
    // Analyze variance
    double cvFrameTime = std::sqrt(currentVarianceFrameTime) / currentMeanFrameTime;
    if (cvFrameTime > 0.2) {
        analysisResults.push_back("WARNING: High variance in frame time (coefficient of variation: " + 
                               std::to_string(cvFrameTime * 100) + "%)");
    }
    
    // Analyze throughput trends
    double currentThroughput = calculateThroughput(metrics);
    if (currentThroughput < TARGET_THROUGHPUT * 0.5) {
        analysisResults.push_back("WARNING: Throughput is significantly below target (current: " + 
                               std::to_string(currentThroughput) + " ops/sec, target: " + 
                               std::to_string(TARGET_THROUGHPUT) + " ops/sec)");
    } else if (currentThroughput < TARGET_THROUGHPUT * 0.8) {
        analysisResults.push_back("NOTICE: Throughput is below target (current: " + 
                               std::to_string(currentThroughput) + " ops/sec, target: " + 
                               std::to_string(TARGET_THROUGHPUT) + " ops/sec)");
    }
    
    // Analyze memory usage trends
    double currentMemoryUsage = calculateAverageMemoryUsage(metrics);
    if (currentMemoryUsage > 2000.0) { // 2GB threshold
        analysisResults.push_back("WARNING: Memory usage is high (current: " + 
                               std::to_string(currentMemoryUsage) + "MB)");
    }
    
    // Analyze CPU usage trends
    double currentCPUUsage = calculateAverageCPUUsage(metrics);
    if (currentCPUUsage > MAX_CPU_USAGE * 0.8) {
        analysisResults.push_back("WARNING: CPU usage is high (current: " + 
                               std::to_string(currentCPUUsage) + "%)");
    }
    
    return analysisResults;
}

std::vector<std::string> PerformanceAnalyzer::identifyBottlenecks(const PerformanceMetrics& metrics) const {
    std::vector<std::string> bottlenecks;
    
    // Identify timestep processing bottlenecks
    double timestepTime = metrics.frameTime * 0.8; // Assume 80% of frame time is timestep
    double timestepThroughput = metrics.operationsCount / timestepTime;
    
    if (timestepTime > TARGET_FRAME_TIME * 0.6) {
        bottlenecks.push_back("TIMESTEP PROCESSING: Timestep processing takes too long (" + 
                            std::to_string(timestepTime) + "ms per timestep)");
    }
    
    if (timestepThroughput < 1000.0) { // 1k operations per ms threshold
        bottlenecks.push_back("COMPUTATIONAL EFFICIENCY: Low operations per millisecond (" + 
                            std::to_string(timestepThroughput) + " ops/ms)");
    }
    
    // Identify memory bandwidth bottlenecks
    double memoryBandwidth = metrics.memoryUsage * 8.0 * 1000.0; // MB/s to MB/s (assuming 8 bytes per operation)
    double memoryOperationsPerSecond = metrics.memoryAllocations * 1000.0 / timestepTime;
    
    if (memoryOperationsPerSecond > memoryBandwidth * 0.8) {
        bottlenecks.push_back("MEMORY BANDWIDTH: Memory operations approaching bandwidth limit (" + 
                            std::to_string(memoryOperationsPerSecond) + " allocs/ms vs " + 
                            std::to_string(memoryBandwidth / 1000.0) + " MB/s)");
    }
    
    // Identify spike processing bottlenecks
    if (metrics.spikesProcessed > 0) {
        double spikeProcessingTime = (metrics.frameTime * 0.1).load(); // Assume 10% for spike processing
        double spikesPerMillisecond = metrics.spikesProcessed / timestepTime;
        
        if (spikeProcessingTime > 1.0) { // 1ms threshold
            bottlenecks.push_back("SPIKE PROCESSING: Spike processing takes too long (" + 
                                std::to_string(spikeProcessingTime) + "ms per spike batch)");
        }
    }
    
    // Identify thread synchronization bottlenecks
    double spikeProcessingTime = (metrics.frameTime * 0.1).load(); // Assume 10% for thread sync
    if (spikeProcessingTime > 2.0) { // 2ms threshold
        bottlenecks.push_back("THREAD SYNCHRONIZATION: Thread synchronization takes too long (" + 
                            std::to_string(spikeProcessingTime) + "ms)");
    }
    
    return bottlenecks;
}

std::vector<std::string> PerformanceAnalyzer::detectRegressions(const std::vector<PerformanceMetrics>& metrics) const {
    std::vector<std::string> regressions;
    
    if (metrics.size() < 20) {
        return regressions; // Not enough data for regression detection
    }
    
    // Split data into two halves for comparison
    size_t midpoint = metrics.size() / 2;
    std::vector<PerformanceMetrics> firstHalf(metrics.begin(), metrics.begin() + midpoint);
    std::vector<PerformanceMetrics> secondHalf(metrics.begin() + midpoint, metrics.end());
    
    // Compare frame time performance
    double firstHalfFrameTime = calculateAverageFrameTime(firstHalf);
    double secondHalfFrameTime = calculateAverageFrameTime(secondHalf);
    
    if (secondHalfFrameTime > firstHalfFrameTime * 1.2) {
        double regressionPercent = ((secondHalfFrameTime - firstHalfFrameTime) / firstHalfFrameTime) * 100.0;
        regressions.push_back("REGRESSION DETECTED: Frame time degraded by " + std::to_string(regressionPercent) + "% (before: " + 
                           std::to_string(firstHalfFrameTime) + "ms, after: " + std::to_string(secondHalfFrameTime) + "ms)");
    }
    
    // Compare throughput performance
    double firstHalfThroughput = calculateThroughput(firstHalf);
    double secondHalfThroughput = calculateThroughput(secondHalf);
    
    if (secondHalfThroughput < firstHalfThroughput * 0.8) {
        double regressionPercent = ((firstHalfThroughput - secondHalfThroughput) / firstHalfThroughput) * 100.0;
        regressions.push_back("REGRESSION DETECTED: Throughput degraded by " + std::to_string(regressionPercent) + "% (before: " + 
                           std::to_string(firstHalfThroughput) + " ops/sec, after: " + std::to_string(secondHalfThroughput) + " ops/sec)");
    }
    
    // Compare memory usage performance
    double firstHalfMemoryUsage = calculateAverageMemoryUsage(firstHalf);
    double secondHalfMemoryUsage = calculateAverageMemoryUsage(secondHalf);
    
    if (secondHalfMemoryUsage > firstHalfMemoryUsage * 1.5) {
        double regressionPercent = ((secondHalfMemoryUsage - firstHalfMemoryUsage) / firstHalfMemoryUsage) * 100.0;
        regressions.push_back("REGRESSION DETECTED: Memory usage increased by " + std::to_string(regressionPercent) + "% (before: " + 
                           std::to_string(firstHalfMemoryUsage) + "MB, after: " + std::to_string(secondHalfMemoryUsage) + "MB)");
    }
    
    return regressions;
}

std::string PerformanceAnalyzer::generateReport(const std::vector<PerformanceMetrics>& metrics) const {
    std::ostringstream report;
    
    report << "Performance Analysis Report\n";
    report << "==========================\n\n";
    
    if (metrics.empty()) {
        report << "No metrics available for analysis.\n";
        return report.str();
    }
    
    // Summary statistics
    report << "Summary Statistics:\n";
    report << "------------------\n";
    report << "Total samples: " << metrics.size() << "\n";
    report << "Average frame time: " << std::fixed << std::setprecision(2) << calculateAverageFrameTime() << " ms\n";
    report << "Average throughput: " << std::fixed << std::setprecision(2) << calculateThroughput() << " ops/sec\n";
    report << "Average memory usage: " << std::fixed << std::setprecision(2) << calculateAverageMemoryUsage() << " MB\n";
    report << "Average CPU usage: " << std::fixed << std::setprecision(2) << calculateAverageCPUUsage() << " %\n\n";
    
    // Trend analysis
    auto trendAnalysis = analyzeTrends(metrics);
    if (!trendAnalysis.empty()) {
        report << "Trend Analysis:\n";
        report << "----------------\n";
        for (const auto& trend : trendAnalysis) {
            report << "• " << trend << "\n";
        }
        report << "\n";
    }
    
    // Performance score
    double performanceScore = calculatePerformanceScore();
    report << "Performance Score: " << std::fixed << std::setprecision(2) << performanceScore << "/100\n\n";
    
    // Recommendations
    auto bottlenecks = identifyBottlenecks(getCurrentMetrics());
    if (!bottlenecks.empty()) {
        report << "Recommendations:\n";
        report << "----------------\n";
        for (const auto& bottleneck : bottlenecks) {
            report << "• " << bottleneck << "\n";
        }
        report << "\n";
    }
    
    return report.str();
}

double PerformanceAnalyzer::calculatePerformanceScore(const PerformanceMetrics& metrics) const {
    double score = 100.0;
    
    // Frame time score (weight: 40%)
    double frameTimeScore = std::max(0.0, 100.0 - (metrics.frameTime / TARGET_FRAME_TIME) * 50.0);
    score += frameTimeScore * 0.4;
    
    // Throughput score (weight: 30%)
    double throughputScore = std::max(0.0, 100.0 - (TARGET_THROUGHPUT / metrics.frameTime) * 50.0);
    score += throughputScore * 0.3;
    
    // Memory usage score (weight: 20%)
    double memoryScore = std::max(0.0, 100.0 - (metrics.memoryUsage / 2000.0) * 50.0);
    score += memoryScore * 0.2;
    
    // CPU usage score (weight: 10%)
    double cpuScore = std::max(0.0, 100.0 - (metrics.cpuUsage / MAX_CPU_USAGE) * 100.0);
    score += cpuScore * 0.1;
    
    return std::max(0.0, std::min(score, 100.0));
}

bool PerformanceAnalyzer::isPerformanceAcceptable(const PerformanceMetrics& metrics) const {
    double performanceScore = calculatePerformanceScore(metrics);
    return performanceScore >= 70.0; // Minimum acceptable performance score
}

double PerformanceAnalyzer::calculateAverageFrameTime(const std::vector<PerformanceMetrics>& metrics) const {
    if (metrics.empty()) return 0.0;
    
    double totalFrameTime = 0.0;
    for (const auto& m : metrics) {
        totalFrameTime += m.frameTime;
    }
    
    return totalFrameTime / metrics.size();
}

double PerformanceAnalyzer::calculateFrameTimeVariance(const std::vector<PerformanceMetrics>& metrics) const {
    if (metrics.size() < 2) return 0.0;
    
    double mean = calculateAverageFrameTime(metrics);
    double variance = 0.0;
    
    for (const auto& m : metrics) {
        double diff = m.frameTime - mean;
        variance += diff * diff;
    }
    
    return variance / metrics.size();
}

double PerformanceAnalyzer::calculateThroughput(const std::vector<PerformanceMetrics>& metrics) const {
    if (metrics.empty()) return 0.0;
    
    double totalTime = 0.0;
    double totalOperations = 0.0;
    
    for (const auto& m : metrics) {
        totalTime += m.frameTime;
        totalOperations += m.operationsCount;
    }
    
    return totalTime > 0.0 ? totalOperations / totalTime : 0.0;
}

double PerformanceAnalyzer::calculateAverageMemoryUsage(const std::vector<PerformanceMetrics>& metrics) const {
    if (metrics.empty()) return 0.0;
    
    double totalMemoryUsage = 0.0;
    for (const auto& m : metrics) {
        totalMemoryUsage += m.memoryUsage;
    }
    
    return totalMemoryUsage / metrics.size();
}

double PerformanceAnalyzer::calculateAverageCPUUsage(const std::vector<PerformanceMetrics>& metrics) const {
    if (metrics.empty()) return 0.0;
    
    double totalCPUUsage = 0.0;
    for (const auto& m : metrics) {
        totalCPUUsage += m.cpuUsage;
    }
    
    return totalCPUUsage / metrics.size();
}

void PerformanceProfiler::startProfile(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    profileTimes_[name] -= std::chrono::high_resolution_clock::now();
    isProfiling_ = true;
}

void PerformanceProfiler::stopProfile(const std::string& name) {
    if (!isProfiling_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    profileTimes_[name] += std::chrono::high_resolution_clock::now();
    isProfiling_ = false;
}

double PerformanceProfiler::getProfileTime(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = profileTimes_.find(name);
    if (it == profileTimes_.end()) {
        return 0.0;
    }
    
    return std::chrono::duration<double>(it->second).count();
}

void PerformanceProfiler::startMemoryProfile(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    memoryUsages_[name] = getCurrentMemoryUsage();
}

size_t PerformanceProfiler::getMemoryUsage(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = memoryUsages_.find(name);
    if (it == memoryUsages_.end()) {
        return 0;
    }
    
    return getCurrentMemoryUsage() - it->second;
}

void PerformanceProfiler::startThreadProfile(size_t threadId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    threadTimes_[threadId] -= std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::stopThreadProfile(size_t threadId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    threadTimes_[threadId] += std::chrono::high_resolution_clock::now();
}

double PerformanceProfiler::getThreadTime(size_t threadId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = threadTimes_.find(threadId);
    if (it == threadTimes_.end()) {
        return 0.0;
    }
    
    return std::chrono::duration<double>(it->second).count();
}

std::map<std::string, double> PerformanceProfiler::getAllProfileTimes() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return profileTimes_;
}

std::map<std::string, size_t> PerformanceProfiler::getAllMemoryUsages() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return memoryUsages_;
}

std::map<size_t, double> PerformanceProfiler::getAllThreadTimes() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return threadTimes_;
}

void PerformanceProfiler::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    profileTimes_.clear();
    memoryUsages_.clear();
    threadTimes_.clear();
    isProfiling_ = false;
}

size_t PerformanceProfiler::getCurrentMemoryUsage() const {
    // Implementation would depend on the platform
    // For now, return a placeholder value
    return 0;
}

PerformanceMonitor::PerformanceMonitor() {
    metricsCollector_ = std::make_unique<PerformanceMetricsCollector>();
    analyzer_ = std::make_unique<PerformanceAnalyzer>();
    profiler_ = std::make_unique<PerformanceProfiler>();
}

PerformanceMonitor::~PerformanceMonitor() = default;

void PerformanceMonitor::initialize(const PerformanceMonitorConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    config_ = config;
    samplingTimer_ = 0.0;
    
    // Start background monitoring thread if configured
    if (config_.realTimeMonitoring) {
        monitoringActive_ = true;
        monitoringThread_ = std::thread(&PerformanceMonitor::monitoringLoop, this);
    }
}

void PerformanceMonitor::shutdown() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        monitoringActive_ = false;
    }
    
    if (monitoringThread_.joinable()) {
        monitoringThread_.join();
    }
}

void PerformanceMonitor::startProfile(const std::string& name) {
    profiler_->startProfile(name);
}

void PerformanceMonitor::stopProfile(const std::string& name) {
    profiler_->stopProfile(name);
}

void PerformanceMonitor::recordMetrics(const PerformanceMetrics& metrics) {
    metricsCollector_->recordMetrics(metrics);
}

PerformanceMetrics PerformanceMonitor::getCurrentMetrics() const {
    return metricsCollector_->getCurrentMetrics();
}

std::vector<PerformanceMetrics> PerformanceMonitor::getMetricsHistory() const {
    return metricsCollector_->getMetricsHistory();
}

void PerformanceMonitor::generateReport(const PerformanceMetrics& metrics, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << analyzer_->generateReport(std::vector<PerformanceMetrics>{metrics});
    file.close();
}

void PerformanceMonitor::generateFinalReport() const {
    auto metrics = getMetricsHistory();
    if (metrics.empty()) {
        return;
    }
    
    generateReport(metrics.back(), "final_performance_report.html");
}

void PerformanceMonitor::monitoringLoop() {
    while (monitoringActive_) {
        std::this_thread::sleep_for(std::chrono::duration<double>(config_.samplingInterval));
        
        auto metrics = getCurrentMetrics();
        metricsCollector_->recordMetrics(metrics);
        
        if (config_.generateReports) {
            generateFinalReport();
        }
    }
}

std::unique_ptr<PerformanceMonitor> PerformanceMonitor::getInstance() {
    if (!instance_) {
        instance_ = std::make_unique<PerformanceMonitor>();
    }
    return std::move(instance_);
}

void PerformanceMonitor::destroyInstance() {
    instance_.reset();
}

PerformanceMonitorConfig PerformanceMonitor::getDefaultConfig() {
    PerformanceMonitorConfig config;
    config.enableTimingProfiling = true;
    config.enableMemoryProfiling = true;
    config.enableSystemProfiling = true;
    config.enableThreadProfiling = true;
    config.samplingInterval = 0.1;
    config.targetFrameTime = 16.67;
    config.targetThroughput = 1000.0;
    config.generateReports = true;
    config.realTimeMonitoring = true;
    config.detailedProfiling = false;
    return config;
}

PerformanceMonitorConfig PerformanceMonitor::getProductionConfig() {
    PerformanceMonitorConfig config;
    config.enableTimingProfiling = true;
    config.enableMemoryProfiling = true;
    config.enableSystemProfiling = true;
    config.enableThreadProfiling = false;
    config.samplingInterval = 1.0;
    config.targetFrameTime = 33.33;
    config.targetThroughput = 50000.0;
    config.generateReports = true;
    config.realTimeMonitoring = true;
    config.detailedProfiling = false;
    return config;
}

PerformanceMonitorConfig PerformanceMonitor::getDevelopmentConfig() {
    PerformanceMonitorConfig config;
    config.enableTimingProfiling = true;
    config.enableMemoryProfiling = true;
    config.enableSystemProfiling = true;
    config.enableThreadProfiling = true;
    config.samplingInterval = 0.05;
    config.targetFrameTime = 16.67;
    config.targetThroughput = 10000.0;
    config.generateReports = true;
    config.realTimeMonitoring = true;
    config.detailedProfiling = true;
    return config;
}

// Singleton instance
std::unique_ptr<PerformanceMonitor> PerformanceMonitor::instance_ = nullptr;

// PerformanceMonitorConfig member variable definition
PerformanceMonitorConfig PerformanceMonitor::config_;

// PerformanceMetrics::frameTime member variable definition
std::atomic<double> PerformanceMetrics::frameTime = 0.0;

// PerformanceMetrics::memoryUsage member variable definition  
std::atomic<double> PerformanceMetrics::memoryUsage = 0.0;

// PerformanceMetrics::cpuUsage member variable definition
std::atomic<double> PerformanceMetrics::cpuUsage = 0.0;

// PerformanceMetrics::operationsCount member variable definition
std::atomic<uint64_t> PerformanceMetrics::operationsCount = 0;

// PerformanceMetrics::memoryAllocations member variable definition
std::atomic<uint64_t> PerformanceMetrics::memoryAllocations = 0;

// PerformanceMetrics::memoryDeallocations member variable definition
std::atomic<uint64_t> PerformanceMetrics::memoryDeallocations = 0;

// PerformanceMetrics::floatOperations member variable definition
std::atomic<uint64_t> PerformanceMetrics::floatOperations = 0;

// PerformanceMetrics::neuronUpdates member variable definition
std::atomic<uint64_t> PerformanceMetrics::neuronUpdates = 0;

// PerformanceMetrics::synapseUpdates member variable definition
std::atomic<uint64_t> PerformanceMetrics::synapseUpdates = 0;

// PerformanceMetrics::spikesProcessed member variable definition
std::atomic<uint64_t> PerformanceMetrics::spikesProcessed = 0;

// PerformanceMetrics::throughput member variable definition
std::atomic<double> PerformanceMetrics::throughput = 0.0;

// PerformanceMetrics::timestamp member variable definition
std::atomic<double> PerformanceMetrics::timestamp = 0.0;

// PerformanceMetrics::predictionError member variable definition
std::atomic<float> PerformanceMetrics::predictionError = 0.0f;

// PerformanceMetrics::confidence member variable definition
std::atomic<float> PerformanceMetrics::confidence = 0.0f;

// PerformanceMetrics::baselineReward member variable definition
std::atomic<float> PerformanceMetrics::baselineReward = 0.0f;

} // namespace nlm
