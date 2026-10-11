// ProfilerIntegration.cpp - Performance profiler integration implementation

#include "cli/ProfilerIntegration.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

ProfilerIntegration::ProfilerIntegration()
    : coreProfiler(nullptr), memoryProfiler(nullptr), threadProfiler(nullptr),
      benchmark(nullptr), visualizer(nullptr), monitoringActive(false) {
    initializeProfilers();
}

ProfilerIntegration::ProfilerIntegration(const PerformanceConfig& config)
    : performanceConfig(config), coreProfiler(nullptr), memoryProfiler(nullptr),
      threadProfiler(nullptr), benchmark(nullptr), visualizer(nullptr), monitoringActive(false) {
    initializeProfilers();
    configure(config);
}

ProfilerIntegration::~ProfilerIntegration() {
    stopProfiling();
}

void ProfilerIntegration::initializeProfilers() {
    if (performanceConfig.enableCoreProfiler) {
        coreProfiler = std::make_unique<PerformanceProfiler>(performanceConfig.coreProfilerConfig);
    }
    
    if (performanceConfig.enableMemoryProfiler) {
        memoryProfiler = std::make_unique<MemoryProfiler>(performanceConfig.memoryProfilerConfig);
    }
    
    if (performanceConfig.enableThreadProfiler) {
        threadProfiler = std::make_unique<ThreadProfiler>(performanceConfig.threadProfilerConfig);
    }
    
    if (performanceConfig.enableBenchmarking) {
        benchmark = std::make_unique<Benchmark>(performanceConfig.benchmarkConfig);
    }
    
    if (performanceConfig.enableVisualization) {
        visualizer = std::make_unique<Visualization>(performanceConfig.visualizationConfig);
    }
    
    NLM_LOG_INFO("Profiler integration initialized with " +
                std::to_string(performanceConfig.enableCoreProfiler) + " core, " +
                std::to_string(performanceConfig.enableMemoryProfiler) + " memory, " +
                std::to_string(performanceConfig.enableThreadProfiler) + " thread, " +
                std::to_string(performanceConfig.enableBenchmarking) + " benchmark, " +
                std::to_string(performanceConfig.enableVisualization) + " visualization profilers");
}

void ProfilerIntegration::configure(const PerformanceConfig& config) {
    std::lock_guard<std::mutex> lock(dataMutex);
    performanceConfig = config;
    
    // Re-initialize profilers if needed
    coreProfiler.reset();
    memoryProfiler.reset();
    threadProfiler.reset();
    benchmark.reset();
    visualizer.reset();
    
    initializeProfilers();
    
    NLM_LOG_INFO("Profiler integration reconfigured");
}

void ProfilerIntegration::startProfiling(const std::string& sessionName) {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (isProfilingActive()) {
        NLM_LOG_WARNING("Profiler integration already active");
        return;
    }
    
    NLM_LOG_INFO("Starting profiler integration session: " + sessionName);
    
    // Update session name
    performanceConfig.sessionName = sessionName;
    
    // Start individual profilers
    if (performanceConfig.enableCoreProfiler && coreProfiler) {
        coreProfiler->startProfiling(sessionName);
        status.coreProfilerActive = true;
    }
    
    if (performanceConfig.enableMemoryProfiler && memoryProfiler) {
        memoryProfiler->startProfiling(sessionName);
        status.memoryProfilerActive = true;
    }
    
    if (performanceConfig.enableThreadProfiler && threadProfiler) {
        threadProfiler->startProfiling(sessionName);
        status.threadProfilerActive = true;
    }
    
    if (performanceConfig.enableBenchmarking && benchmark) {
        benchmark->clearResults();
        status.benchmarkProfilerActive = true;
    }
    
    if (performanceConfig.enableVisualization && visualizer) {
        status.visualizationActive = true;
    }
    
    monitoringActive = performanceConfig.enableRealTimeMonitoring;
    startTime = std::chrono::steady_clock::now();
    
    // Update status
    updateStatus();n    
    NLM_LOG_INFO("Profiler integration session started: " + sessionName);
}

void ProfilerIntegration::stopProfiling() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (!isProfilingActive()) {
        NLM_LOG_WARNING("Profiler integration not active");
        return;
    }
    
    NLM_LOG_INFO("Stopping profiler integration session: " + performanceConfig.sessionName);
    
    // Stop individual profilers
    if (performanceConfig.enableCoreProfiler && coreProfiler) {
        coreProfiler->stopProfiling();
        status.coreProfilerActive = false;
    }
    
    if (performanceConfig.enableMemoryProfiler && memoryProfiler) {
        memoryProfiler->stopProfiling();
        status.memoryProfilerActive = false;
    }
    
    if (performanceConfig.enableThreadProfiler && threadProfiler) {
        threadProfiler->stopProfiling();
        status.threadProfilerActive = false;
    }
    
    monitoringActive = false;
    
    // Analyze performance data
    analyzePerformanceData();
    
    // Update status
    updateStatus();
    
    NLM_LOG_INFO("Profiler integration session stopped");
}

bool ProfilerIntegration::isProfilingActive() const {
    return status.coreProfilerActive || status.memoryProfilerActive || 
           status.threadProfilerActive || status.benchmarkProfilerActive ||
           status.visualizationActive;
}

void ProfilerIntegration::startProfiler(const std::string& profilerName) {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (profilerName == "core" && performanceConfig.enableCoreProfiler && coreProfiler) {
        coreProfiler->startProfiling(performanceConfig.sessionName);
        status.coreProfilerActive = true;
        updateStatus();
    }
    else if (profilerName == "memory" && performanceConfig.enableMemoryProfiler && memoryProfiler) {
        memoryProfiler->startProfiling(performanceConfig.sessionName);
        status.memoryProfilerActive = true;
        updateStatus();
    }
    else if (profilerName == "thread" && performanceConfig.enableThreadProfiler && threadProfiler) {
        threadProfiler->startProfiling(performanceConfig.sessionName);
        status.threadProfilerActive = true;
        updateStatus();
    }
    else if (profilerName == "benchmark" && performanceConfig.enableBenchmarking && benchmark) {
        status.benchmarkProfilerActive = true;
        updateStatus();
    }
    else if (profilerName == "visualization" && performanceConfig.enableVisualization && visualizer) {
        status.visualizationActive = true;
        updateStatus();
    }
    else {
        NLM_LOG_WARNING("Unknown or disabled profiler: " + profilerName);
    }
}

void ProfilerIntegration::stopProfiler(const std::string& profilerName) {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (profilerName == "core" && performanceConfig.enableCoreProfiler && coreProfiler) {
        coreProfiler->stopProfiling();
        status.coreProfilerActive = false;
        updateStatus();
    }
    else if (profilerName == "memory" && performanceConfig.enableMemoryProfiler && memoryProfiler) {
        memoryProfiler->stopProfiling();
        status.memoryProfilerActive = false;
        updateStatus();
    }
    else if (profilerName == "thread" && performanceConfig.enableThreadProfiler && threadProfiler) {
        threadProfiler->stopProfiling();
        status.threadProfilerActive = false;
        updateStatus();
    }
    else if (profilerName == "benchmark" && performanceConfig.enableBenchmarking && benchmark) {
        status.benchmarkProfilerActive = false;
        updateStatus();
    }
    else if (profilerName == "visualization" && performanceConfig.enableVisualization && visualizer) {
        status.visualizationActive = false;
        updateStatus();
    }
    else {
        NLM_LOG_WARNING("Unknown or disabled profiler: " + profilerName);
    }
}

bool ProfilerIntegration::isProfilerActive(const std::string& profilerName) const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (profilerName == "core") {
        return status.coreProfilerActive;
    }
    else if (profilerName == "memory") {
        return status.memoryProfilerActive;
    }
    else if (profilerName == "thread") {
        return status.threadProfilerActive;
    }
    else if (profilerName == "benchmark") {
        return status.benchmarkProfilerActive;
    }
    else if (profilerName == "visualization") {
        return status.visualizationActive;
    }
    
    return false;
}

void ProfilerIntegration::recordPerformanceData(const std::string& profilerName,
                                               const PerformanceProfiler::PerformanceData& data) {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (profilerName == "core" && coreProfiler) {
        coreProfiler->recordOperation("core_performance", data.wallClockTime);
    }
    else if (profilerName == "memory" && memoryProfiler) {
        memoryProfiler->recordMemoryUsage(static_cast<size_t>(data.memoryUsage));
    }
    else if (profilerName == "thread" && threadProfiler) {
        threadProfiler->recordThreadExecution(data.threadData.empty() ? 0 : data.threadData.begin()->second);
    }
    
    performanceHistory.push_back(data);
    updatePerformanceHistory();
    
    if (monitoringActive) {
        detectBottlenecks();
    }
}

void ProfilerIntegration::recordMemoryAllocation(const std::string& profilerName,
                                                 void* address, size_t size,
                                                 const std::string& allocationSite,
                                                 const std::string& callStack,
                                                 int threadId) {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (profilerName == "memory" && memoryProfiler) {
        memoryProfiler->recordAllocation(address, size, allocationSite, callStack, threadId);
    }
}

void ProfilerIntegration::recordThreadExecution(const std::string& profilerName,
                                                int64_t durationNs) {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (profilerName == "thread" && threadProfiler) {
        threadProfiler->recordThreadExecution(durationNs);
    }
}

void ProfilerIntegration::recordSynchronizationEvent(const std::string& profilerName,
                                                     const ThreadProfiler::SynchronizationEvent& event) {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (profilerName == "thread" && threadProfiler) {
        threadProfiler->recordSynchronizationEvent(event);
    }
}

void ProfilerIntegration::recordBenchmark(const std::string& profilerName,
                                          const Benchmark::BenchmarkResult& result) {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (profilerName == "benchmark" && benchmark) {
        benchmark->addBenchmark(performanceConfig.benchmarkConfig);
    }
}

PerformanceProfiler::PerformanceData ProfilerIntegration::getCurrentPerformanceData() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    PerformanceProfiler::PerformanceData data;
    
    if (coreProfiler && performanceConfig.enableCoreProfiler) {
        data = coreProfiler->getCurrentPerformanceData();
    }
    
    return data;
}

MemoryProfiler::MemoryStats ProfilerIntegration::getCurrentMemoryStats() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (memoryProfiler && performanceConfig.enableMemoryProfiler) {
        return memoryProfiler->getMemoryStats();
    }
    
    return MemoryProfiler::MemoryStats();
}

ThreadProfiler::ThreadStats ProfilerIntegration::getCurrentThreadStats() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    if (threadProfiler && performanceConfig.enableThreadProfiler) {
        return threadProfiler->getThreadStats();
    }
    
    return ThreadProfiler::ThreadStats();
}

std::string ProfilerIntegration::getPerformanceComparisonReport() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    std::ostringstream oss;
    oss << "Performance Comparison Report\n";
    oss << "============================\n\n";
    
    oss << "Session: " << performanceConfig.sessionName << "\n";
    oss << "Duration: " << (startTime > std::chrono::steady_clock::time_point() ? 
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count() : 0) << "ms\n\n";
    
    // Compare with baseline if available
    if (!baselineData.empty()) {
        oss << "Comparing with baseline data:\n";
        oss << "Baseline samples: " << baselineData.size() << "\n";
        oss << "Current samples: " << performanceHistory.size() << "\n\n";
        
        // Calculate averages
        double baselineWallClock = 0.0, currentWallClock = 0.0;
        double baselineCPU = 0.0, currentCPU = 0.0;
        double baselineMemory = 0.0, currentMemory = 0.0;
        
        for (const auto& data : baselineData) {
            baselineWallClock += data.wallClockTime;
            baselineCPU += data.cpuTime;
            baselineMemory += data.memoryUsage;
        }
        
        for (const auto& data : performanceHistory) {
            currentWallClock += data.wallClockTime;
            currentCPU += data.cpuTime;
            currentMemory += data.memoryUsage;
        }
        
        if (!baselineData.empty()) {
            baselineWallClock /= baselineData.size();
            currentWallClock /= performanceHistory.size();
            baselineCPU /= baselineData.size();
            currentCPU /= performanceHistory.size();
            baselineMemory /= baselineData.size();
            currentMemory /= performanceHistory.size();
        }
        
        oss << "Baseline - Wall Clock: " << baselineWallClock << "ms, CPU: " << baselineCPU << "ms, Memory: " << baselineMemory << " bytes\n";
        oss << "Current  - Wall Clock: " << currentWallClock << "ms, CPU: " << currentCPU << "ms, Memory: " << currentMemory << " bytes\n\n";
        
        if (baselineWallClock > 0) {
            double wallClockRatio = currentWallClock / baselineWallClock;
            oss << "Wall Clock Ratio: " << wallClockRatio << "x\n";
            
            if (wallClockRatio < 0.9) {
                oss << "✓ Improvement detected\n";
            } else if (wallClockRatio > 1.1) {
                oss << "⚠ Regression detected\n";
            }
        }
        
        if (baselineCPU > 0) {
            double cpuRatio = currentCPU / baselineCPU;
            oss << "CPU Time Ratio: " << cpuRatio << "x\n";
            
            if (cpuRatio < 0.9) {
                oss << "✓ Improvement detected\n";
            } else if (cpuRatio > 1.1) {
                oss << "⚠ Regression detected\n";
            }
        }
        
        if (baselineMemory > 0) {
            double memoryRatio = currentMemory / baselineMemory;
            oss << "Memory Usage Ratio: " << memoryRatio << "x\n";
            
            if (memoryRatio < 0.9) {
                oss << "✓ Improvement detected\n";
            } else if (memoryRatio > 1.1) {
                oss << "⚠ Regression detected\n";
            }
        }
    }
    
    return oss.str();
}

std::string ProfilerIntegration::getPerformanceRegressionReport() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    std::ostringstream oss;
    oss << "Performance Regression Report\n";
    oss << "===========================\n\n";
    
    oss << "Session: " << performanceConfig.sessionName << "\n";
    
    if (detectedRegressions.empty()) {
        oss << "No performance regressions detected\n";
    } else {
        oss << "Regressions detected:\n";
        for (const auto& regression : detectedRegressions) {
            oss << "  - " << regression << "\n";
        }
    }
    
    return oss.str();
}

std::string ProfilerIntegration::getScalabilityAnalysisReport() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    std::ostringstream oss;
    oss << "Scalability Analysis Report\n";
    oss << "=========================\n\n";
    
    oss << "Session: " << performanceConfig.sessionName << "\n";
    oss << "Total samples: " << performanceHistory.size() << "\n";
    
    if (performanceHistory.size() < 3) {
        oss << "Insufficient data for scalability analysis (need at least 3 samples)\n";
        return oss.str();
    }
    
    // Simple scalability analysis
    oss << "Scalability Analysis:\n";
    for (size_t i = 1; i < performanceHistory.size(); ++i) {
        double timeRatio = performanceHistory[i].wallClockTime / performanceHistory[i-1].wallClockTime;
        double timeScale = performanceHistory[i].timestamp / performanceHistory[i-1].timestamp;
        
        oss << "  Sample " << i << ": Time scale = " << timeScale << ", Performance ratio = " << timeRatio << "\n";
        
        if (timeRatio > timeScale * 1.5) {
            oss << "    → Sublinear scaling detected\n";
        } else if (timeRatio < timeScale * 0.5) {
            oss << "    → Superlinear scaling detected\n";
        } else {
            oss << "    → Linear scaling detected\n";
        }
    }
    
    return oss.str();
}

std::vector<std::string> ProfilerIntegration::getOptimizationRecommendations() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    std::vector<std::string> recommendations = optimizationRecommendations;
    
    // Add recommendations based on current performance
    if (performanceHistory.size() >= 10) {
        // Check for trends
        size_t halfPoint = performanceHistory.size() / 2;
        double firstHalfTime = 0.0, secondHalfTime = 0.0;
        
        for (size_t i = 0; i < halfPoint; ++i) {
            firstHalfTime += performanceHistory[i].wallClockTime;
        }
        
        for (size_t i = halfPoint; i < performanceHistory.size(); ++i) {
            secondHalfTime += performanceHistory[i].wallClockTime;
        }
        
        if (secondHalfTime > firstHalfTime * 1.5) {
            recommendations.push_back("Performance degradation over time detected. Consider optimization.");
        }
    }
    
    return recommendations;
}

std::vector<std::string> ProfilerIntegration::getPerformanceBottlenecks() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    return detectedBottlenecks;
}

std::string ProfilerIntegration::generateComprehensiveReport(const std::string& format) const {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    std::ostringstream oss;
    
    if (format == "json") {
        oss << generateJsonReport();
    } else if (format == "csv") {
        oss << generateCsvReport();
    } else {
        oss << generateTextReport();
    }
    
    return oss.str();
}

bool ProfilerIntegration::exportPerformanceData(const std::string& filename, const std::string& format) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    std::string data;
    if (format == "json") {
        data = generateJsonReport();
    } else if (format == "csv") {
        data = generateCsvReport();
    } else {
        data = generateTextReport();
    }
    
    file << data;
    file.close();
    
    NLM_LOG_INFO("Performance data exported to: " + filename);
    return true;
}

bool ProfilerIntegration::generatePerformanceDashboard(const std::string& filename) const {
    if (!visualizer) {
        NLM_LOG_ERROR("Visualizer not available");
        return false;
    }
    
    // Generate dashboard data
    std::vector<Visualization::ChartConfig> charts;
    
    // Create performance trend chart
    std::vector<Visualization::ChartDataPoint> performanceData;
    for (size_t i = 0; i < performanceHistory.size(); ++i) {
        Visualization::ChartDataPoint point;
        point.timestamp = performanceHistory[i].timestamp;
        point.value = performanceHistory[i].wallClockTime;
        point.label = "Sample " + std::to_string(i);
        point.series = "Wall Clock Time";
        performanceData.push_back(point);
    }
    
    charts.push_back(visualizer->createLineChart("Performance Trend", performanceData, "Time", "Wall Clock Time (ms)"));
    
    // Generate HTML dashboard
    std::string htmlContent = visualizer->generateHtmlDashboard(charts);
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filename);
        return false;
    }
    
    file << htmlContent;
    file.close();
    
    NLM_LOG_INFO("Performance dashboard generated: " + filename);
    return true;
}

bool ProfilerIntegration::loadBaselineData(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open baseline file: " + filename);
        return false;
    }
    
    // TODO: Implement baseline data loading
    // For now, we'll just create some placeholder baseline data
    baselineData.clear();
    
    // Add some placeholder baseline data
    for (int i = 0; i < 10; ++i) {
        PerformanceProfiler::PerformanceData data;
        data.timestamp = i * 1000;
        data.wallClockTime = 1000.0;
        data.cpuTime = 800.0;
        data.memoryUsage = 10 * 1024 * 1024;
        baselineData.push_back(data);
    }
    
    file.close();
    
    NLM_LOG_INFO("Baseline data loaded: " + filename);
    return true;
}

void ProfilerIntegration::setBaselineData(const std::vector<PerformanceProfiler::PerformanceData>& baseline) {
    std::lock_guard<std::mutex> lock(dataMutex);
    baselineData = baseline;
}

std::string ProfilerIntegration::compareWithBaseline(const std::vector<PerformanceProfiler::PerformanceData>& current) const {
    std::ostringstream oss;
    oss << "Performance Comparison with Baseline\n";
    oss << "=====================================\n\n";
    
    if (baselineData.empty()) {
        oss << "No baseline data available for comparison\n";
        return oss.str();
    }
    
    oss << "Baseline collected " << baselineData.size() << " data points\n";
    oss << "Current session collected " << current.size() << " data points\n\n";
    
    // Compare average metrics
    double baselineWallClock = 0.0, currentWallClock = 0.0;
    double baselineCpu = 0.0, currentCpu = 0.0;
    double baselineMemory = 0.0, currentMemory = 0.0;
    
    for (const auto& data : baselineData) {
        baselineWallClock += data.wallClockTime;
        baselineCpu += data.cpuTime;
        baselineMemory += data.memoryUsage;
    }
    
    for (const auto& data : current) {
        currentWallClock += data.wallClockTime;
        currentCpu += data.cpuTime;
        currentMemory += data.memoryUsage;
    }
    
    size_t baselineCount = baselineData.size();
    size_t currentCount = current.size();
    
    baselineWallClock /= baselineCount;
    currentWallClock /= currentCount;
    baselineCpu /= baselineCount;
    currentCpu /= currentCount;
    baselineMemory /= baselineCount;
    currentMemory /= currentCount;
    
    oss << "Average Metrics Comparison:\n";
    oss << "  Wall Clock Time: Baseline = " << baselineWallClock << "ms, Current = " << currentWallClock << "ms, Ratio = " << (currentWallClock / baselineWallClock) << "x\n";
    oss << "  CPU Time: Baseline = " << baselineCpu << "ms, Current = " << currentCpu << "ms, Ratio = " << (currentCpu / baselineCpu) << "x\n";
    oss << "  Memory Usage: Baseline = " << baselineMemory << " bytes, Current = " << currentMemory << " bytes, Ratio = " << (currentMemory / baselineMemory) << "x\n\n";
    
    // Determine improvement/regression
    if (currentWallClock < baselineWallClock * 0.9) {
        oss << "✓ Current performance is better than baseline (wall clock)\n";
    } else if (currentWallClock > baselineWallClock * 1.1) {
        oss << "⚠ Current performance is worse than baseline (wall clock)\n";
    }
    
    if (currentCpu < baselineCpu * 0.9) {
        oss << "✓ Current CPU performance is better than baseline\n";
    } else if (currentCpu > baselineCpu * 1.1) {
        oss << "⚠ Current CPU performance is worse than baseline\n";
    }
    
    if (currentMemory < baselineMemory * 0.9) {
        oss << "✓ Current memory usage is better than baseline\n";
    } else if (currentMemory > baselineMemory * 1.1) {
        oss << "⚠ Current memory usage is worse than baseline\n";
    }
    
    return oss.str();
}

std::vector<std::string> ProfilerIntegration::detectPerformanceRegressions(
    const std::vector<PerformanceProfiler::PerformanceData>& current) const {
    std::vector<std::string> regressions;
    
    if (baselineData.empty()) {
        return regressions;
    }
    
    // Simple regression detection
    double baselineWallClock = 0.0, currentWallClock = 0.0;
    double baselineCpu = 0.0, currentCpu = 0.0;
    double baselineMemory = 0.0, currentMemory = 0.0;
    
    for (const auto& data : baselineData) {
        baselineWallClock += data.wallClockTime;
        baselineCpu += data.cpuTime;
        baselineMemory += data.memoryUsage;
    }
    
    for (const auto& data : current) {
        currentWallClock += data.wallClockTime;
        currentCpu += data.cpuTime;
        currentMemory += data.memoryUsage;
    }
    
    size_t baselineCount = baselineData.size();
    size_t currentCount = current.size();
    
    baselineWallClock /= baselineCount;
    currentWallClock /= currentCount;
    baselineCpu /= baselineCount;
    currentCpu /= currentCount;
    baselineMemory /= baselineCount;
    currentMemory /= currentCount;
    
    if (baselineWallClock > 0 && currentWallClock / baselineWallClock > 1.1) {
        regressions.push_back("Wall clock time regression (more than 10% slower)");
    }
    
    if (baselineCpu > 0 && currentCpu / baselineCpu > 1.1) {
        regressions.push_back("CPU time regression (more than 10% slower)");
    }
    
    if (baselineMemory > 0 && currentMemory / baselineMemory > 1.1) {
        regressions.push_back("Memory usage regression (more than 10% higher)");
    }
    
    return regressions;
}

std::string ProfilerIntegration::analyzeScalability(const std::vector<PerformanceProfiler::PerformanceData>& data) const {
    std::ostringstream oss;
    oss << "Scalability Analysis\n";
    oss << "===================\n\n";
    
    if (data.size() < 3) {
        oss << "Insufficient data for scalability analysis (need at least 3 data points)\n";
        return oss.str();
    }
    
    // Check for linear vs sublinear scaling
    std::vector<double> metrics;
    std::vector<double> timeScales;
    
    for (const auto& sample : data) {
        metrics.push_back(sample.wallClockTime);
        timeScales.push_back(sample.timestamp);
    }
    
    oss << "Collected " << data.size() << " data points over " << 
        (data.back().timestamp - data.front().timestamp) << " ms\n\n";
    
    // Simple scaling analysis (assuming time scales linearly)
    oss << "Scaling Analysis:\n";
    for (size_t i = 1; i < data.size(); ++i) {
        double timeRatio = data[i].wallClockTime / data[i-1].wallClockTime;
        double timeScale = data[i].timestamp / data[i-1].timestamp;
        
        oss << "  Point " << i << ": Time scale = " << timeScale << ", Performance ratio = " << timeRatio << "\n";
        
        if (timeRatio > timeScale * 1.5) {
            oss << "    → Sublinear scaling detected (performance degradation)\n";
        } else if (timeRatio < timeScale * 0.5) {
            oss << "    → Superlinear scaling detected (performance improvement)\n";
        } else {
            oss << "    → Linear scaling detected\n";
        }
    }
    
    return oss.str();
}

ProfilerIntegration::IntegrationStatus ProfilerIntegration::getStatus() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    return status;
}

void ProfilerIntegration::reset() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    // Stop all profilers
    if (coreProfiler) coreProfiler->stopProfiling();
    if (memoryProfiler) memoryProfiler->stopProfiling();
    if (threadProfiler) threadProfiler->stopProfiling();
    
    // Clear data
    performanceHistory.clear();
    baselineData.clear();
    detectedBottlenecks.clear();
    detectedRegressions.clear();
    optimizationRecommendations.clear();
    
    // Reset status
    status.coreProfilerActive = false;
    status.memoryProfilerActive = false;
    status.threadProfilerActive = false;
    status.benchmarkProfilerActive = false;
    status.visualizationActive = false;
    status.totalProfileDuration = 0;
    status.totalSamplesCollected = 0;
    status.bottlenecksDetected = 0;
    status.regressionsDetected = 0;
    status.analysisComplete = false;
    
    NLM_LOG_INFO("Profiler integration reset");
}

void ProfilerIntegration::cleanupOldData() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    // Remove old performance data
    auto cutoffTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count() - 
        performanceConfig.coreProfilerConfig.samplingIntervalMs * 1000;
    
    performanceHistory.erase(
        std::remove_if(performanceHistory.begin(), performanceHistory.end(),
            [cutoffTime](const PerformanceProfiler::PerformanceData& data) {
                return data.timestamp < cutoffTime;
            }),
        performanceHistory.end());
    
    // Limit the number of samples
    if (performanceHistory.size() > performanceConfig.coreProfilerConfig.maxMemorySamples) {
        size_t excess = performanceHistory.size() - performanceConfig.coreProfilerConfig.maxMemorySamples;
        performanceHistory.erase(performanceHistory.begin(), performanceHistory.begin() + excess);
    }
}

void ProfilerIntegration::updateStatus() {
    status.totalProfileDuration = (startTime > std::chrono::steady_clock::time_point()) ? 
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count() : 0;
    
    status.totalSamplesCollected = performanceHistory.size();
    status.bottlenecksDetected = detectedBottlenecks.size();
    status.regressionsDetected = detectedRegressions.size();
    status.analysisComplete = !detectedBottlenecks.empty() || !detectedRegressions.empty() || !optimizationRecommendations.empty();
}

void ProfilerIntegration::analyzePerformanceData() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    // Detect bottlenecks
    detectBottlenecks();
    
    // Detect regressions if baseline available
    if (!baselineData.empty()) {
        detectedRegressions = detectPerformanceRegressions(performanceHistory);
    }
    
    // Generate optimization recommendations
    generateRecommendations();
}

void ProfilerIntegration::detectBottlenecks() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    detectedBottlenecks.clear();
    
    if (performanceHistory.size() < 2) {
        return;
    }
    
    // Check for memory bottlenecks
    std::vector<double> memoryUsages;
    for (const auto& data : performanceHistory) {
        memoryUsages.push_back(data.memoryUsage);
    }
    
    double avgMemory = std::accumulate(memoryUsages.begin(), memoryUsages.end(), 0.0) / memoryUsages.size();
    if (avgMemory > 1024 * 1024 * 1024) { // > 1GB
        detectedBottlenecks.push_back("High memory usage detected (>1GB average)");
    }
    
    // Check for time bottlenecks
    int64_t duration = status.totalProfileDuration;
    if (duration > 60000) { // > 1 minute
        detectedBottlenecks.push_back("Long execution time detected (>1 minute)");
    }
    
    // Check for performance degradation over time
    if (performanceHistory.size() >= 10) {
        size_t halfPoint = performanceHistory.size() / 2;
        double firstHalfTime = 0.0, secondHalfTime = 0.0;
        
        for (size_t i = 0; i < halfPoint; ++i) {
            firstHalfTime += performanceHistory[i].wallClockTime;
        }
        
        for (size_t i = halfPoint; i < performanceHistory.size(); ++i) {
            secondHalfTime += performanceHistory[i].wallClockTime;
        }
        
        if (secondHalfTime > firstHalfTime * 1.5) {
            detectedBottlenecks.push_back("Performance degradation over time detected");
        }
    }
}

void ProfilerIntegration::generateRecommendations() {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    optimizationRecommendations.clear();
    
    if (performanceHistory.size() < 2) {
        return;
    }
    
    // Calculate performance metrics
    double totalWallClock = 0.0;
    double totalCpu = 0.0;
    double totalMemory = 0.0;
    
    for (const auto& data : performanceHistory) {
        totalWallClock += data.wallClockTime;
        totalCpu += data.cpuTime;
        totalMemory += data.memoryUsage;
    }
    
    size_t count = performanceHistory.size();
    double avgWallClock = totalWallClock / count;
    double avgCpu = totalCpu / count;
    double avgMemory = totalMemory / count;
    
    // Generate recommendations
    if (avgWallClock > 10000) { // > 10 seconds
        optimizationRecommendations.push_back("Optimize wall-clock time - consider parallel processing");
    }
    
    if (avgCpu > avgWallClock * 0.9) { // CPU time is close to wall clock time
        optimizationRecommendations.push_back("CPU time is close to wall clock - consider optimizing CPU-bound operations");
    }
    
    if (avgMemory > 512 * 1024 * 1024) { // > 512MB
        optimizationRecommendations.push_back("Optimize memory usage - consider memory pooling or data structure optimization");
    }
    
    if (performanceHistory.size() > 100) {
        optimizationRecommendations.push_back("Consider reducing sampling frequency to improve performance");
    }
}

void ProfilerIntegration::updatePerformanceHistory() {
    // Performance history is updated in real-time, no need for periodic updates
}

std::string ProfilerIntegration::generateJsonReport() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"sessionName\": \"" << performanceConfig.sessionName << "\",\n";
    oss << "  \"durationMs\": " << status.totalProfileDuration << ",\n";
    oss << "  \"isActive\": " << (isProfilingActive() ? "true" : "false") << ",\n";
    oss << "  \"dataPoints\": " << performanceHistory.size() << ",\n";
    
    if (!performanceHistory.empty()) {
        oss << "  \"samples\": [\n";
        for (size_t i = 0; i < performanceHistory.size(); ++i) {
            const auto& data = performanceHistory[i];
            oss << "    {\n";
            oss << "      \"timestamp\": " << data.timestamp << ",\n";
            oss << "      \"wallClockTime\": " << data.wallClockTime << ",\n";
            oss << "      \"cpuTime\": " << data.cpuTime << ",\n";
            oss << "      \"memoryUsage\": " << data.memoryUsage << ",\n";
            oss << "      \"throughput\": " << (data.wallClockTime > 0 ? 1.0 : 0.0) << ",\n";
            oss << "      \"metadata\": {\"session\": \"" << performanceConfig.sessionName << "\"}\n";
            if (i < performanceHistory.size() - 1) {
                oss << "    },\n";
            } else {
                oss << "    }\n";
            }
        }
        oss << "  ],\n";
    }
    
    oss << "  \"bottlenecks\": [\n";
    for (size_t i = 0; i < detectedBottlenecks.size(); ++i) {
        oss << "    \"" << escapeJsonString(detectedBottlenecks[i]) << "\"";
        if (i < detectedBottlenecks.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "  ],\n";
    
    oss << "  \"recommendations\": [\n";
    for (size_t i = 0; i < optimizationRecommendations.size(); ++i) {
        oss << "    \"" << escapeJsonString(optimizationRecommendations[i]) << "\"";
        if (i < optimizationRecommendations.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "  ],\n";
    
    oss << "  \"regressions\": [\n";
    for (size_t i = 0; i < detectedRegressions.size(); ++i) {
        oss << "    \"" << escapeJsonString(detectedRegressions[i]) << "\"";
        if (i < detectedRegressions.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "  ]\n";
    oss << "}\n";
    
    return oss.str();
}

std::string ProfilerIntegration::generateCsvReport() const {
    std::ostringstream oss;
    oss << "timestamp,wallClockTime,cpTime,memoryUsage,throughput\n";
    
    for (const auto& data : performanceHistory) {
        oss << data.timestamp << ","
            << data.wallClockTime << ","
            << data.cpuTime << ","
            << data.memoryUsage << ","
            << (data.wallClockTime > 0 ? 1.0 : 0.0) << "\n";
    }
    
    return oss.str();
}

std::string ProfilerIntegration::generateTextReport() const {
    std::ostringstream oss;
    oss << "Performance Integration Report\n";
    oss << "=============================\n\n";
    oss << "Session: " << performanceConfig.sessionName << "\n";
    oss << "Duration: " << status.totalProfileDuration << "ms\n";
    oss << "Status: " << (isProfilingActive() ? "Active" : "Stopped") << "\n";
    oss << "Data Points: " << performanceHistory.size() << "\n\n";
    
    if (!performanceHistory.empty()) {
        oss << "Recent Samples:\n";
        size_t recentCount = std::min(size_t(10), performanceHistory.size());
        for (size_t i = 0; i < recentCount; ++i) {
            const auto& data = performanceHistory[performanceHistory.size() - 1 - i];
            oss << "  " << data.timestamp << ": " << data.wallClockTime << "ms, "
                << data.cpuTime << "ms CPU, "
                << data.memoryUsage / (1024 * 1024) << "MB memory\n";
        }
    }
    
    oss << "\nBottlenecks:\n";
    if (detectedBottlenecks.empty()) {
        oss << "  None detected\n";
    } else {
        for (const auto& bottleneck : detectedBottlenecks) {
            oss << "  - " << bottleneck << "\n";
        }
    }
    
    oss << "\nRecommendations:\n";
    if (optimizationRecommendations.empty()) {
        oss << "  None\n";
    } else {
        for (const auto& recommendation : optimizationRecommendations) {
            oss << "  - " << recommendation << "\n";
        }
    }
    
    oss << "\nRegressions:\n";
    if (detectedRegressions.empty()) {
        oss << "  None detected\n";
    } else {
        for (const auto& regression : detectedRegressions) {
            oss << "  - " << regression << "\n";
        }
    }
    
    return oss.str();
}

std::string ProfilerIntegration::escapeJsonString(const std::string& input) const {
    std::string output;
    output.reserve(input.size());
    
    for (char c : input) {
        switch (c) {
            case '"': output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    output += "\\u" + toHexString(static_cast<unsigned int>(c));
                } else {
                    output += c;
                }
                break;
        }
    }
    
    return output;
}

std::string ProfilerIntegration::toHexString(unsigned int value) const {
    std::stringstream stream;
    stream << std::hex << std::setw(4) << std::setfill('0') << value;
    return stream.str();
}

} // namespace nlm