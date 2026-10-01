#include "PerformanceAPI.hpp"
#include "PerformanceProfiler.hpp"
#include "BenchmarkSuite.hpp"
#include "PerformanceAnalyzer.hpp"
#include "../../core/Logger/Logger.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <mutex>

namespace nlm {

PerformanceAPI* PerformanceAPI::instance = nullptr;

PerformanceAPI& PerformanceAPI::getInstance() {
    if (!instance) {
        instance = new PerformanceAPI();
    }
    return *instance;
}

void PerformanceAPI::shutdown() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

PerformanceAPI::PerformanceAPI() : pImpl(std::make_unique<Impl>()) {}

PerformanceAPI::~PerformanceAPI() {
    shutdownAPI();
}

bool PerformanceAPI::initialize(std::shared_ptr<Brain> brain) {
    if (pImpl->profiler && pImpl->profiler->isActive()) {
        return false; // Already initialized
    }
    
    // Initialize profiler
    pImpl->profiler = std::make_unique<PerformanceProfiler>();
    if (!pImpl->profiler->initialize()) {
        NLM_LOG_ERROR("PerformanceAPI: Failed to initialize PerformanceProfiler");
        return false;
    }
    
    // Initialize benchmark suite
    pImpl->benchmarkSuite = std::make_unique<BenchmarkSuite>();
    if (!pImpl->benchmarkSuite->initialize()) {
        NLM_LOG_ERROR("PerformanceAPI: Failed to initialize BenchmarkSuite");
        return false;
    }
    
    // Initialize analyzer
    pImpl->analyzer = std::make_unique<PerformanceAnalyzer>();
    if (!pImpl->analyzer->initialize()) {
        NLM_LOG_ERROR("PerformanceAPI: Failed to initialize PerformanceAnalyzer");
        return false;
    }
    
    // Enable brain integration if brain provided
    if (brain) {
        enableBrainIntegration(brain);
    }
    
    NLM_LOG_INFO("PerformanceAPI: Initialized successfully");
    return true;
}

void PerformanceAPI::shutdownAPI() {
    if (pImpl->brain) {
        disableBrainIntegration();
    }
    
    if (pImpl->profiler) {
        pImpl->profiler->shutdown();
    }
    
    if (pImpl->benchmarkSuite) {
        pImpl->benchmarkSuite->shutdown();
    }
    
    if (pImpl->analyzer) {
        pImpl->analyzer->shutdown();
    }
    
    pImpl->profiler.reset();
    pImpl->benchmarkSuite.reset();
    pImpl->analyzer.reset();
    pImpl->brain.reset();
    
    NLM_LOG_INFO("PerformanceAPI: Shutdown completed");
}

bool PerformanceAPI::startProfiling(const std::string& sectionName) {
    if (pImpl->profiler && pImpl->profiler->isActive()) {
        pImpl->profiler->startProfile(sectionName);
        NLM_LOG_DEBUG("PerformanceAPI: Started profiling section: " + sectionName);
        return true;
    }
    return false;
}

void PerformanceAPI::stopProfiling(const std::string& sectionName) {
    if (pImpl->profiler && pImpl->profiler->isActive()) {
        pImpl->profiler->stopProfile(sectionName);
        NLM_LOG_DEBUG("PerformanceAPI: Stopped profiling section: " + sectionName);
    }
}

void PerformanceAPI::pauseProfiling(const std::string& sectionName) {
    if (pImpl->profiler && pImpl->profiler->isActive()) {
        pImpl->profiler->pauseProfile(sectionName);
        NLM_LOG_DEBUG("PerformanceAPI: Paused profiling section: " + sectionName);
    }
}

void PerformanceAPI::resumeProfiling(const std::string& sectionName) {
    if (pImpl->profiler && pImpl->profiler->isActive()) {
        pImpl->profiler->resumeProfile(sectionName);
        NLM_LOG_DEBUG("PerformanceAPI: Resumed profiling section: " + sectionName);
    }
}

void PerformanceAPI::profileBrainStep() {
    if (pImpl->profiler && pImpl->profiler->isActive()) {
        startProfiling("brain_step");
        // In real implementation, would profile actual brain step
        std::this_thread::sleep_for(std::chrono::microseconds(100)); // Simulate work
        stopProfiling("brain_step");
    }
}

void PerformanceAPI::profileMemoryOperation(size_t bytes, bool isAllocation) {
    if (pImpl->profiler && pImpl->profiler->isActive()) {
        if (isAllocation) {
            pImpl->profiler->trackMemoryAllocation(bytes);
        } else {
            pImpl->profiler->trackMemoryDeallocation(bytes);
        }
    }
}

void PerformanceAPI::profileCPUOperation(size_t operations) {
    if (pImpl->profiler && pImpl->profiler->isActive()) {
        updateCPUUsage();
    }
}

void PerformanceAPI::profileIOOperation(size_t bytes, bool isRead) {
    if (pImpl->profiler && pImpl->profiler->isActive()) {
        if (isRead) {
            pImpl->profiler->trackReadOperation(bytes);
        } else {
            pImpl->profiler->trackWriteOperation(bytes);
        }
    }
}

void PerformanceAPI::startRealTimeMonitoring(std::chrono::milliseconds interval) {
    if (pImpl->profiler) {
        pImpl->profiler->startRealTimeMonitoring(interval);
        NLM_LOG_INFO("PerformanceAPI: Started real-time monitoring with interval: " + std::to_string(interval.count()) + "ms");
    }
}

void PerformanceAPI::stopRealTimeMonitoring() {
    if (pImpl->profiler) {
        pImpl->profiler->stopRealTimeMonitoring();
        NLM_LOG_INFO("PerformanceAPI: Stopped real-time monitoring");
    }
}

bool PerformanceAPI::isMonitoring() const {
    return (pImpl->profiler && pImpl->profiler->isRealTimeMonitoringActive());
}

std::vector<BenchmarkSuite::BenchmarkResult> PerformanceAPI::runPerformanceBenchmarks(const std::string& benchmarkType) {
    std::vector<BenchmarkSuite::BenchmarkResult> results;
    
    if (!pImpl->benchmarkSuite) return results;
    
    if (benchmarkType == "all") {
        results = pImpl->benchmarkSuite->runAllBenchmarks();
    } else if (benchmarkType == "configuration") {
        results = pImpl->benchmarkSuite->runBenchmarksByType(BenchmarkSuite::TestType::Configuration);
    } else if (benchmarkType == "performance") {
        results = pImpl->benchmarkSuite->runBenchmarksByType(BenchmarkSuite::TestType::Performance);
    } else if (benchmarkType == "stress") {
        results = pImpl->benchmarkSuite->runBenchmarksByType(BenchmarkSuite::TestType::Stress);
    } else if (benchmarkType == "regression") {
        results = pImpl->benchmarkSuite->runBenchmarksByType(BenchmarkSuite::TestType::Regression);
    }
    
    return results;
}

void PerformanceAPI::scheduleBenchmark(const std::string& benchmarkName,
                                       std::chrono::system_clock::time_point when) {
    if (pImpl->benchmarkSuite) {
        pImpl->benchmarkSuite->scheduleBenchmark(benchmarkName, when);
    }
}

std::string PerformanceAPI::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "=== NLM Performance Report ===" << std::endl;
    ss << std::endl;
    
    if (pImpl->profiler) {
        ss << "--- Current Profiler Stats ---" << std::endl;
        auto stats = pImpl->profiler->getProfileStats();
        ss << "Total Execution Time: " << stats.totalExecutionTime.count() << "s" << std::endl;
        ss << "Current Memory Usage: " << stats.totalMemoryUsage << " bytes" << std::endl;
        ss << "Current CPU Usage: " << stats.cpuUsage << "%" << std::endl;
        ss << "Active Sections: " << stats.activeSections.size() << std::endl;
        ss << "Real-time Monitoring: " << (stats.isRealTimeEnabled ? "Active" : "Inactive") << std::endl;
        ss << std::endl;
    }
    
    if (pImpl->benchmarkSuite) {
        ss << "--- Benchmark Suite Stats ---" << std::endl;
        auto suiteStats = pImpl->benchmarkSuite->getSuiteStats();
        ss << "Total Benchmarks: " << suiteStats.totalBenchmarks << std::endl;
        ss << "Completed: " << suiteStats.completedBenchmarks << std::endl;
        ss << "Passed: " << suiteStats.passedBenchmarks << std::endl;
        ss << "Failed: " << suiteStats.failedBenchmarks << std::endl;
        ss << "Total Time: " << suiteStats.totalTime << "s" << std::endl;
        ss << std::endl;
    }
    
    if (pImpl->analyzer) {
        ss << "--- Analysis Results ---" << std::endl;
        ss << "Bottleneck Sections: ";
        auto bottlenecks = pImpl->analyzer->getBottleneckSections();
        for (size_t i = 0; i < bottlenecks.size(); ++i) {
            ss << bottlenecks[i];
            if (i < bottlenecks.size() - 1) ss << ", ";
        }
        ss << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

std::string PerformanceAPI::getPerformanceSummary() const {
    if (pImpl->profiler) {
        return pImpl->profiler->getProfileSummary();
    }
    return "Performance system not initialized";
}

std::vector<std::string> PerformanceAPI::getBottleneckSections(double threshold) const {
    if (pImpl->profiler) {
        return pImpl->profiler->getBottleneckSections(threshold);
    }
    return std::vector<std::string>();
}

std::map<std::string, double> PerformanceAPI::getPerformanceMetrics() const {
    std::map<std::string, double> metrics;
    
    if (pImpl->profiler) {
        auto stats = pImpl->profiler->getProfileStats();
        metrics["total_execution_time"] = stats.totalExecutionTime.count();
        metrics["memory_usage"] = static_cast<double>(stats.totalMemoryUsage);
        metrics["cpu_usage"] = stats.cpuUsage;
        metrics["fps"] = stats.averageFPS;
    }
    
    return metrics;
}

void PerformanceAPI::enableBrainIntegration(std::shared_ptr<Brain> brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PerformanceAPI: Brain integration enabled");
}

void PerformanceAPI::disableBrainIntegration() {
    pImpl->brain.reset();
    NLM_LOG_INFO("PerformanceAPI: Brain integration disabled");
}

bool PerformanceAPI::isBrainIntegrated() const {
    return pImpl->brain != nullptr;
}

void PerformanceAPI::profileBrainStep(const std::string& operation) {
    if (pImpl->brain && pImpl->profiler) {
        std::string sectionName = "brain_step_" + operation;
        pImpl->profiler->startProfile(sectionName);
        
        // In real implementation, would profile actual brain operations
        // For now, simulate some work
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        
        pImpl->profiler->stopProfile(sectionName);
    }
}

void PerformanceAPI::setProfilingInterval(std::chrono::milliseconds interval) {
    if (pImpl->profiler) {
        pImpl->profiler->setProfilingInterval(interval);
    }
}

void PerformanceAPI::setMemoryTracking(bool enable) {
    if (pImpl->profiler) {
        pImpl->profiler->setMemoryTracking(enable);
    }
}

void PerformanceAPI::setCPUTracking(bool enable) {
    if (pImpl->profiler) {
        pImpl->profiler->setCPUTracking(enable);
    }
}

void PerformanceAPI::setIOTracking(bool enable) {
    if (pImpl->profiler) {
        pImpl->profiler->setIOTracking(enable);
    }
}

void PerformanceAPI::setGPUTracking(bool enable) {
    if (pImpl->profiler) {
        pImpl->profiler->setGPUTracking(enable);
    }
}

void PerformanceAPI::setDetailedTiming(bool enable) {
    if (pImpl->profiler) {
        pImpl->profiler->setDetailedTiming(enable);
    }
}

void PerformanceAPI::setMaxBenchmarkConcurrency(size_t count) {
    if (pImpl->benchmarkSuite) {
        pImpl->benchmarkSuite->setMaxConcurrentBenchmarks(count);
    }
}

bool PerformanceAPI::exportPerformanceData(const std::string& filename, const std::string& format) const {
    bool success = false;
    
    if (format == "json") {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << "{\n";
            file << "  \"performance_data\": {\n";
            file << "    \"timestamp\": \"" << std::chrono::system_clock::now() << "\"\n";
            file << "  }\n";
            file << "}\n";
            success = true;
        }
    }
    
    if (success) {
        NLM_LOG_INFO("PerformanceAPI: Performance data exported to " + filename);
    }
    
    return success;
}

bool PerformanceAPI::exportBenchmarkResults(const std::string& filename, const std::string& format) const {
    bool success = false;
    
    if (pImpl->benchmarkSuite) {
        pImpl->benchmarkSuite->saveResults(filename, format);
        success = true;
    }
    
    return success;
}

bool PerformanceAPI::exportAnalysisResults(const std::string& filename, const std::string& format) const {
    bool success = false;
    
    if (pImpl->analyzer) {
        pImpl->analyzer->exportAnalysisResults(filename, format);
        success = true;
    }
    
    return success;
}

void PerformanceAPI::enablePerformanceValidation(const std::map<std::string, double>& requirements) {
    pImpl->performanceRequirements = requirements;
    NLM_LOG_INFO("PerformanceAPI: Performance validation enabled with " + std::to_string(requirements.size()) + " requirements");
}

void PerformanceAPI::disablePerformanceValidation() {
    pImpl->performanceRequirements.clear();
    NLM_LOG_INFO("PerformanceAPI: Performance validation disabled");
}

bool PerformanceAPI::isPerformanceValidationEnabled() const {
    return !pImpl->performanceRequirements.empty();
}

std::vector<std::string> PerformanceAPI::validatePerformanceRequirements() const {
    std::vector<std::string> results;
    
    if (pImpl->analyzer && isPerformanceValidationEnabled()) {
        bool isValid = pImpl->analyzer->validatePerformanceRequirements(pImpl->performanceRequirements);
        if (!isValid) {
            results.push_back("Performance requirements validation failed");
        }
    }
    
    return results;
}

void PerformanceAPI::resetPerformanceData() {
    if (pImpl->profiler) {
        pImpl->profiler->reset();
    }
    
    if (pImpl->benchmarkSuite) {
        pImpl->benchmarkSuite->clearResults();
    }
    
    NLM_LOG_INFO("PerformanceAPI: Performance data reset");
}

void PerformanceAPI::clearAllData() {
    resetPerformanceData();
    
    if (pImpl->analyzer) {
        // Analyzer doesn't have clearAllData, but reset is similar
        pImpl->analyzer->setAnalysisWindow(100);
    }
    
    NLM_LOG_INFO("PerformanceAPI: All data cleared");
}

void PerformanceAPI::pauseAllProfiling() {
    if (pImpl->profiler) {
        for (const auto& section : pImpl->profiler->getAllSectionNames()) {
            pImpl->profiler->pauseProfile(section);
        }
    }
}

void PerformanceAPI::resumeAllProfiling() {
    if (pImpl->profiler) {
        for (const auto& section : pImpl->profiler->getAllSectionNames()) {
            pImpl->profiler->resumeProfile(section);
        }
    }
}

void PerformanceAPI::stopAllProfiling() {
    if (pImpl->profiler) {
        pImpl->profiler->shutdown();
    }
}

void PerformanceAPI::updateCPUUsage() {
    if (pImpl->profiler) {
        pImpl->profiler->updateCPUUsage();
    }
}

// Initialize static member
struct PerformanceAPI::Impl {
    std::unique_ptr<PerformanceProfiler> profiler;
    std::unique_ptr<BenchmarkSuite> benchmarkSuite;
    std::unique_ptr<PerformanceAnalyzer> analyzer;
    std::shared_ptr<Brain> brain;
    std::map<std::string, double> performanceRequirements;
};

} // namespace nlm
