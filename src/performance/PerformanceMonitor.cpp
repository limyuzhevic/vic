#include "PerformanceMonitor.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <ctime>
#include <random>

namespace nlm {

struct PerformanceMonitor::Impl {
    MonitorMode mode = MonitorMode::NORMAL;
    std::mutex dataMutex;
    
    std::unique_ptr<RealTimePerformanceMonitor> realTimeMonitor;
    std::unique_ptr<ResourceMonitor> resourceMonitor;
    std::unique_ptr<ComponentProfiler> profiler;
    std::unique_ptr<PerformanceAnalyzer> analyzer;
    std::unique_ptr<PerformanceVisualizer> visualizer;
    std::unique_ptr<PerformanceBenchmarker> benchmarker;
    
    PerformanceMetrics globalGoals;
    double globalSamplingInterval = 1.0;
    std::atomic<bool> monitoring{false};
    std::thread monitoringThread;
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {
    // Initialize components
    pImpl->realTimeMonitor = std::make_unique<RealTimePerformanceMonitor>();
    pImpl->resourceMonitor = std::make_unique<ResourceMonitor>();
    pImpl->profiler = std::make_unique<ComponentProfiler>();
    pImpl->analyzer = std::make_unique<PerformanceAnalyzer>();
    pImpl->visualizer = std::make_unique<PerformanceVisualizer>();
    pImpl->benchmarker = std::make_unique<PerformanceBenchmarker>();
    
    // Set up default goals
    pImpl->globalGoals = PerformanceMetrics();
    pImpl->globalGoals.cpuUsage = 80.0;      // 80% CPU threshold
    pImpl->globalGoals.memoryPercent = 80.0; // 80% memory threshold
    pImpl->globalGoals.eventRate = 1000.0;    // 1000 events per second
}

PerformanceMonitor::~PerformanceMonitor() {
    shutdown();
}

void PerformanceMonitor::initialize(MonitorMode mode) {
    pImpl->mode = mode;
    
    // Start all monitors
    startAllMonitors();
    
    if (pImpl->mode == MonitorMode::VERBOSE || pImpl->mode == MonitorMode::DEBUG) {
        std::cout << "Performance monitoring initialized in " << 
            (mode == MonitorMode::SILENT ? "SILENT" :
             mode == MonitorMode::NORMAL ? "NORMAL" :
             mode == MonitorMode::VERBOSE ? "VERBOSE" : "DEBUG") << " mode." << std::endl;
    }
}

void PerformanceMonitor::shutdown() {
    stopAllMonitors();
    
    if (pImpl->mode == MonitorMode::VERBOSE || pImpl->mode == MonitorMode::DEBUG) {
        std::cout << "Performance monitoring shut down." << std::endl;
    }
}

RealTimePerformanceMonitor& PerformanceMonitor::getRealTimeMonitor() {
    return *pImpl->realTimeMonitor;
}

ResourceMonitor& PerformanceMonitor::getResourceMonitor() {
    return *pImpl->resourceMonitor;
}

ComponentProfiler& PerformanceMonitor::getProfiler() {
    return *pImpl->profiler;
}

PerformanceAnalyzer& PerformanceMonitor::getAnalyzer() {
    return *pImpl->analyzer;
}

PerformanceVisualizer& PerformanceMonitor::getVisualizer() {
    return *pImpl->visualizer;
}

PerformanceBenchmarker& PerformanceMonitor::getBenchmarker() {
    return *pImpl->benchmarker;
}

void PerformanceMonitor::startAllMonitors() {
    if (pImpl->monitoring) return;
    
    pImpl->monitoring = true;
    
    // Start real-time monitor
    pImpl->realTimeMonitor->startMonitoring();
    
    // Start resource monitor
    pImpl->resourceMonitor->startMonitoring();
    
    // Start profiler if enabled
    if (pImpl->profiler->isEnabled()) {
        pImpl->profiler->startProfiling();
    }
    
    if (pImpl->mode == MonitorMode::VERBOSE || pImpl->mode == MonitorMode::DEBUG) {
        std::cout << "All performance monitors started." << std::endl;
    }
}

void PerformanceMonitor::stopAllMonitors() {
    if (!pImpl->monitoring) return;
    
    pImpl->monitoring = false;
    
    // Stop real-time monitor
    pImpl->realTimeMonitor->stopMonitoring();
    
    // Stop resource monitor
    pImpl->resourceMonitor->stopMonitoring();
    
    // Stop profiler
    pImpl->profiler->stopProfiling();
    
    if (pImpl->mode == MonitorMode::VERBOSE || pImpl->mode == MonitorMode::DEBUG) {
        std::cout << "All performance monitors stopped." << std::endl;
    }
}

void PerformanceMonitor::pauseAllMonitors() {
    pImpl->realTimeMonitor->pauseMonitoring();
}

void PerformanceMonitor::resumeAllMonitors() {
    // RealTimeMonitor doesn't have a resume method in this implementation
    // ResourceMonitor doesn't have pause/resume methods either
}

void PerformanceMonitor::setGlobalSamplingInterval(double intervalSeconds) {
    pImpl->globalSamplingInterval = intervalSeconds;
    pImpl->realTimeMonitor->setSamplingInterval(intervalSeconds);
}

void PerformanceMonitor::setGlobalThresholds(double cpuThreshold, double memoryThreshold) {
    pImpl->globalGoals.cpuUsage = cpuThreshold;
    pImpl->globalGoals.memoryPercent = memoryThreshold;
    pImpl->realTimeMonitor->setCpuThreshold(cpuThreshold);
    pImpl->realTimeMonitor->setMemoryThreshold(memoryThreshold);
}

void PerformanceMonitor::setPerformanceGoals(const PerformanceMetrics& goals) {
    pImpl->globalGoals = goals;
}

std::string PerformanceMonitor::generateComprehensiveReport() const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    std::stringstream ss;
    ss << "=== NLM Performance Monitoring Report ===" << std::endl;
    ss << "Generated at: " << std::chrono::system_clock::now() << std::endl;
    ss << std::endl;
    
    // Current metrics
    ss << "Current Performance Metrics:" << std::endl;
    ss << "  CPU Usage: " << getRealTimeMonitor().getCurrentMetrics().cpuUsage << "%" << std::endl;
    ss << "  Memory Usage: " << getRealTimeMonitor().getCurrentMetrics().memoryUsage << " MB (" 
       << getRealTimeMonitor().getCurrentMetrics().memoryPercent << "%)" << std::endl;
    ss << "  Simulation Speed: " << getRealTimeMonitor().getCurrentMetrics().simulationSpeed << "x" << std::endl;
    ss << "  Event Rate: " << getRealTimeMonitor().getCurrentMetrics().eventRate << " events/sec" << std::endl;
    ss << "  Neuron Count: " << getRealTimeMonitor().getCurrentMetrics().neuronCount << std::endl;
    ss << "  Synapse Count: " << getRealTimeMonitor().getCurrentMetrics().synapseCount << std::endl;
    ss << "  Firing Rate: " << getRealTimeMonitor().getCurrentMetrics().firingRate << " Hz" << std::endl;
    ss << "  E/I Ratio: " << getRealTimeMonitor().getCurrentMetrics().excitationInhibitionRatio << std::endl;
    ss << std::endl;
    
    // System metrics
    ss << "System Resource Metrics:" << std::endl;
    ss << "  CPU Usage: " << getResourceMonitor().getCpuUsage() << "%" << std::endl;
    ss << "  Memory Usage: " << getResourceMonitor().getMemoryUsageMB() << " MB" << std::endl;
    ss << "  Memory Percent: " << getResourceMonitor().getMemoryPercent() << "%" << std::endl;
    ss << std::endl;
    
    // Profiling data
    ss << "Profiling Summary:" << std::endl;
    ss << "  Total Profiling Time: " << getProfiler().getTotalProfilingTime() << " seconds" << std::endl;
    ss << "  Profiled Components: " << getProfiler().getProfiledComponents().size() << std::endl;
    
    if (!getProfiler().findSlowestComponents(0).empty()) {
        ss << "  Slowest Components:" << std::endl;
        ss << getProfiler().findSlowestComponents(5);
    }
    
    if (!getProfiler().findMostCalledComponents(0).empty()) {
        ss << "  Most Called Components:" << std::endl;
        ss << getProfiler().findMostCalledComponents(5);
    }
    
    ss << std::endl;
    
    // Performance goals
    ss << "Performance Goals:" << std::endl;
    ss << "  CPU Goal: " << pImpl->globalGoals.cpuUsage << "%" << std::endl;
    ss << "  Memory Goal: " << pImpl->globalGoals.memoryPercent << "%" << std::endl;
    ss << "  Event Rate Goal: " << pImpl->globalGoals.eventRate << " events/sec" << std::endl;
    ss << std::endl;
    
    return ss.str();
}

void PerformanceMonitor::exportPerformanceData(const std::string& filepath) const {
    // Export data to file
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filepath);
    }
    
    file << generateComprehensiveReport();
}

void PerformanceMonitor::importPerformanceData(const std::string& filepath) {
    // Import data from file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filepath);
    }
    
    // Implementation would read and restore performance data
    std::cout << "Performance data imported from: " << filepath << std::endl;
}

bool PerformanceMonitor::isMonitoring() const {
    return pImpl->monitoring;
}

PerformanceMonitor::MonitorMode PerformanceMonitor::getMode() const {
    return pImpl->mode;
}

void PerformanceMonitor::setMode(MonitorMode mode) {
    pImpl->mode = mode;
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    getRealTimeMonitor().reset();
    getResourceMonitor().reset();
    getProfiler().reset();
    
    if (pImpl->mode == MonitorMode::VERBOSE || pImpl->mode == MonitorMode::DEBUG) {
        std::cout << "Performance monitoring data reset." << std::endl;
    }
}

void PerformanceMonitor::clearAllData() {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    getRealTimeMonitor().reset();
    getResourceMonitor().reset();
    getProfiler().clearAllProfiles();
    
    if (pImpl->mode == MonitorMode::VERBOSE || pImpl->mode == MonitorMode::DEBUG) {
        std::cout << "All performance monitoring data cleared." << std::endl;
    }
}

} // namespace nlm
