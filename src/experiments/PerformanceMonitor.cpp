// PerformanceMonitor.cpp - Implementation of Performance Monitoring System

#include "PerformanceMonitor.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <yaml-cpp/yaml.h>
#include <pugixml.hpp>

namespace nlm {

// Helper function to get CPU temperature (Linux-specific)
double getCPUTemperature() {
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__APPLE__)
    std::ifstream tempFile("/sys/class/thermal/thermal_zone0/temp");
    if (tempFile.is_open()) {
        int temp;
        tempFile >> temp;
        return temp / 1000.0;  // Convert to Celsius
    }
#endif
    return 0.0;
}

// Helper function to get CPU frequency
double getCPUFrequencyMHz() {
#if defined(__x86_64__) || defined(_M_X64)
    // Try to read from /proc/cpuinfo or cpupower
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (cpuinfo.is_open()) {
        std::string line;
        while (std::getline(cpuinfo, line)) {
            if (line.find("cpu MHz") != std::string::npos) {
                try {
                    return std::stod(line.substr(line.find(":") + 1));
                } catch (...) {
                    return 0.0;
                }
            }
        }
    }
#endif
    return 0.0;
}

// Helper function to get memory usage
size_t getCurrentMemoryUsage() {
#if defined(__linux__) || defined(__APPLE__)
    std::ifstream statusFile("/proc/self/status");
    if (statusFile.is_open()) {
        std::string line;
        while (std::getline(statusFile, line)) {
            if (line.find("VmRSS:") != std::string::npos) {
                try {
                    return static_cast<size_t>(std::stoll(line.substr(line.find(":") + 1)) * 1024);  // KB to bytes
                } catch (...) {
                    return 0;
                }
            }
        }
    }
#endif
    return 0;
}

PerformanceMonitor::PerformanceMonitor(const std::string& name, const PerformanceMonitorConfig& config)
    : name_(name), config_(config) {
    if (config_.enabled) {
        start();
    }
}

PerformanceMonitor::~PerformanceMonitor() {
    stop();
}

void PerformanceMonitor::start() {
    if (running_.load()) return;
    
    running_.store(true);
    startTime_ = std::chrono::high_resolution_clock::now();
    lastSampleTime_ = startTime_;
    
    cpuState_.lastCpuTimeStart_ = std::chrono::high_resolution_clock::now();
    memoryState_.lastMemoryCheck_ = startTime_;
    threadState_.lastThreadUpdate_ = startTime_;
    ioState_.lastIOUpdate_ = startTime_;
    
    currentTimestep_.store(0);
    totalSpikesGenerated_.store(0);
    totalSpikesPropagated_.store(0);
    cumulativeWallTimeNs_.store(0);
    cumulativeCPUTimeNs_.store(0);
}

void PerformanceMonitor::stop() {
    if (!running_.load()) return;
    
    running_.store(false);
    paused_.store(false);
    
    // Take final samples
    sampleCPUMetrics();
    sampleMemoryMetrics();
    sampleThreadMetrics();
    sampleIOMetrics();
}

void PerformanceMonitor::pause() {
    if (!running_.load() || paused_.load()) return;
    
    paused_.store(true);
    auto now = std::chrono::high_resolution_clock::now();
    auto pauseDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - pauseTime_);
    cpuState_.lastCpuTimeDurationNs += pauseDuration.count();
}

void PerformanceMonitor::resume() {
    if (!paused_.load()) return;
    
    paused_.store(false);
    pauseTime_ = std::chrono::high_resolution_clock::now();
}

void PerformanceMonitor::update(double timestepTimeNs, size_t spikesGenerated, 
                               size_t spikesPropagated, size_t neuronUpdates,
                               size_t synapseUpdates, double memoryDeltaBytes) {
    if (!running_.load() || paused_.load()) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    // Record timestep performance
    TimestepPerformance ts(currentTimestep_.load());
    ts.wallTimeNs = timestepTimeNs;
    ts.cpuTimeNs = cpuState_.lastCpuTimeDurationNs;
    ts.memoryDeltaBytes = memoryDeltaBytes;
    ts.spikesGenerated = spikesGenerated;
    ts.spikesPropagated = spikesPropagated;
    ts.activeNeurons = activeNeurons_.load();
    ts.neuronUpdates = neuronUpdates;
    ts.synapseUpdates = synapseUpdates;
    
    // Calculate bottleneck ratio
    ts.bottleneckRatio = calculateBottleneckRatio(ts);
    
    // Store timestep data
    timestepHistory_.push_back(ts);
    
    // Update cumulative counters
    cumulativeWallTimeNs_.fetch_add(timestepTimeNs);
    cumulativeCPUTimeNs_.fetch_add(cpuState_.lastCpuTimeDurationNs);
    totalSpikesGenerated_.fetch_add(spikesGenerated);
    totalSpikesPropagated_.fetch_add(spikesPropagated);
    currentTimestep_.fetch_add(1);
    
    // Take samples for next timestep
    sampleCPUMetrics();
    sampleMemoryMetrics();
    sampleThreadMetrics();
    sampleIOMetrics();
    
    // Detect bottlenecks
    if (config_.enableBottleneckDetection) {
        detectBottlenecks();
    }
    
    // Call callbacks
    for (const auto& callback : metricCallbacks_) {
        callback(calculateCurrentMetrics());
    }
}

void PerformanceMonitor::recordMemoryAllocation(size_t size, const char* file, 
                                                size_t line, const char* function,
                                                bool isPool, uint32_t poolId) {
    if (!config_.trackMemoryAllocations) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto now = std::chrono::high_resolution_clock::now();
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    
    memoryAllocations_.emplace_back(timestamp, size, file, line, function, 
                                    getCurrentThreadId(), isPool, poolId);
    
    memoryState_.totalAllocations.fetch_add(1);
    memoryState_.currentMemoryBytes.fetch_add(size);
    
    // Update peak memory
    size_t currentMem = memoryState_.currentMemoryBytes.load();
    size_t peakMem = memoryState_.peakMemoryBytes.load();
    if (currentMem > peakMem) {
        memoryState_.peakMemoryBytes.store(currentMem);
    }
}

void PerformanceMonitor::recordMemoryDeallocation(size_t size, const char* file,
                                                  size_t line, const char* function,
                                                  bool isPool, uint32_t poolId) {
    if (!config_.trackMemoryAllocations) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto now = std::chrono::high_resolution_clock::now();
    uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    
    memoryAllocations_.emplace_back(timestamp, -static_cast<long long>(size), file, line, 
                                    function, getCurrentThreadId(), isPool, poolId);
    
    memoryState_.totalDeallocations.fetch_add(1);
    memoryState_.currentMemoryBytes.fetch_sub(size);
}

void PerformanceMonitor::updateThreadMetrics(const ThreadPerformanceMetrics& metrics) {
    if (!config_.monitorThreads) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    threadState_.currentThreadMetrics_[metrics.threadId] = metrics;
    
    // Calculate averages for history
    if (threadMetricsHistory_.size() < config_.historySize) {
        threadMetricsHistory_.push_back(metrics);
    }
}

void PerformanceMonitor::updateIOMetrics(const IOMetrics& metrics) {
    if (!config_.monitorIO) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    ioMetricsHistory_.push_back(metrics);
}

std::vector<TimestepPerformance> PerformanceMonitor::getTimestepHistory(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (maxCount == 0 || maxCount >= timestepHistory_.size()) {
        return timestepHistory_;
    }
    
    size_t startIndex = timestepHistory_.size() - maxCount;
    return std::vector<TimestepPerformance>(timestepHistory_.begin() + startIndex, timestepHistory_.end());
}

std::vector<MemoryAllocationInfo> PerformanceMonitor::getMemoryAllocations(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (maxCount == 0 || maxCount >= memoryAllocations_.size()) {
        return memoryAllocations_;
    }
    
    size_t startIndex = memoryAllocations_.size() - maxCount;
    return std::vector<MemoryAllocationInfo>(memoryAllocations_.begin() + startIndex, memoryAllocations_.end());
}

std::vector<ThreadPerformanceMetrics> PerformanceMonitor::getThreadMetricsHistory(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (maxCount == 0 || maxCount >= threadMetricsHistory_.size()) {
        return threadMetricsHistory_;
    }
    
    size_t startIndex = threadMetricsHistory_.size() - maxCount;
    return std::vector<ThreadPerformanceMetrics>(threadMetricsHistory_.begin() + startIndex, threadMetricsHistory_.end());
}

std::vector<IOMetrics> PerformanceMonitor::getIOMetricsHistory(size_t maxCount) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (maxCount == 0 || maxCount >= ioMetricsHistory_.size()) {
        return ioMetricsHistory_;
    }
    
    size_t startIndex = ioMetricsHistory_.size() - maxCount;
    return std::vector<IOMetrics>(ioMetricsHistory_.begin() + startIndex, ioMetricsHistory_.end());
}

PerformanceMetrics PerformanceMonitor::calculateCurrentMetrics() const {
    PerformanceMetrics metrics;
    
    // Calculate time elapsed
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - startTime_);
    double elapsedSeconds = elapsed.count() / 1e9;
    
    if (elapsedSeconds > 0) {
        metrics.cpuUsagePercent = (cumulativeCPUTimeNs_.load() / elapsed.count()) * 100.0;
    }
    
    // Memory metrics
    metrics.totalMemoryBytes = memoryState_.peakMemoryBytes.load();
    metrics.peakMemoryBytes = metrics.totalMemoryBytes; // Same for peak
    metrics.availableMemoryBytes = getCurrentMemoryUsage();
    metrics.memoryUsagePercent = elapsedSeconds > 0 ? 
        (static_cast<double>(metrics.totalMemoryBytes) / metrics.availableMemoryBytes) * 100.0 : 0.0;
    metrics.allocations = memoryState_.totalAllocations.load();
    metrics.deallocations = memoryState_.totalDeallocations.load();
    
    // Thread metrics
    metrics.activeThreads = threadState_.activeThreads.load();
    metrics.totalThreads = threadState_.totalThreads.load();
    metrics.threadUtilizationPercent = elapsedSeconds > 0 ? 
        (static_cast<double>(metrics.activeThreads) / metrics.totalThreads) * 100.0 : 0.0;
    
    // Simulation metrics
    if (elapsedSeconds > 0) {
        metrics.timestepsPerSecond = currentTimestep_.load() / elapsedSeconds;
        metrics.avgTimestepTimeMs = getAverageTimestepTimeMs() / 1e6;
        metrics.minTimestepTimeMs = getMinTimestepTimeMs() / 1e6;
        metrics.maxTimestepTimeMs = getMaxTimestepTimeMs() / 1e6;
        metrics.timestepTimeStdDev = getTimestepTimeStdDev() / 1e6;
        metrics.totalSpikesGenerated = totalSpikesGenerated_.load();
        metrics.totalSpikesPropagated = totalSpikesPropagated_.load();
        metrics.avgFiringRateHz = (metrics.totalSpikesGenerated / elapsedSeconds) / activeNeurons_.load();
    }
    
    // Hardware info
    metrics.cpuCores = std::thread::hardware_concurrency();
    metrics.cpuTemperatureCelsius = getCPUTemperature();
    metrics.memoryBandwidthMBps = elapsedSeconds > 0 ? 
        (static_cast<double>(metrics.totalMemoryBytes) / elapsedSeconds) / (1024 * 1024) : 0.0;
    
    return metrics;
}

std::string PerformanceMonitor::analyzeBottlenecks() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (timestepHistory_.empty()) return "No data available";
    
    std::stringstream ss;
    ss << "Bottleneck Analysis:\n";
    
    // Find maximum timestep
    size_t maxTimestepIndex = 0;
    double maxTimestepTime = timestepHistory_[0].wallTimeNs;
    for (size_t i = 1; i < timestepHistory_.size(); ++i) {
        if (timestepHistory_[i].wallTimeNs > maxTimestepTime) {
            maxTimestepIndex = i;
            maxTimestepTime = timestepHistory_[i].wallTimeNs;
        }
    }
    
    const auto& bottleneckTS = timestepHistory_[maxTimestepIndex];
    ss << "Most expensive timestep: #" << bottleneckTS.timestep << " (" 
       << formatTimeNs(bottleneckTS.wallTimeNs) << ")\n";
    
    if (bottleneckTS.bottleneckComponent.empty()) {
        ss << "No bottleneck detected (threshold: " << config_.bottleneckThreshold << ")\n";
    } else {
        ss << "Bottleneck component: " << bottleneckTS.bottleneckComponent << "\n";
        ss << "Bottleneck severity: " << bottleneckTS.bottleneckSeverity << "\n";
        
        // Identify potential causes
        if (bottleneckTS.componentTiming.neuronUpdateNs > bottleneckTS.componentTiming.synapseUpdateNs &&
            bottleneckTS.componentTiming.neuronUpdateNs > bottleneckTS.componentTiming.spikePropagationNs) {
            ss << "Suspected cause: Neuron updates are the bottleneck\n";
        } else if (bottleneckTS.componentTiming.synapseUpdateNs > bottleneckTS.componentTiming.neuronUpdateNs &&
                  bottleneckTS.componentTiming.synapseUpdateNs > bottleneckTS.componentTiming.spikePropagationNs) {
            ss << "Suspected cause: Synapse updates are the bottleneck\n";
        } else if (bottleneckTS.componentTiming.spikePropagationNs > bottleneckTS.componentTiming.neuronUpdateNs &&
                  bottleneckTS.componentTiming.spikePropagationNs > bottleneckTS.componentTiming.synapseUpdateNs) {
            ss << "Suspected cause: Spike propagation is the bottleneck\n";
        }
    }
    
    // Overall performance summary
    double avgStepTime = getAverageTimestepTimeMs();
    double minStepTime = getMinTimestepTimeMs();
    double maxStepTime = getMaxTimestepTimeMs();
    double stdDev = getTimestepTimeStdDev();
    
    ss << "\nPerformance Summary:\n";
    ss << "Average timestep time: " << formatTimeNs(avgStepTime) << "\n";
    ss << "Minimum timestep time: " << formatTimeNs(minStepTime) << "\n";
    ss << "Maximum timestep time: " << formatTimeNs(maxStepTime) << "\n";
    ss << "Standard deviation: " << formatTimeNs(stdDev) << "\n";
    ss << "Coefficient of variation: " << (stdDev / avgStepTime) * 100.0 << "%\n";
    
    return ss.str();
}

double PerformanceMonitor::getAverageTimestepTimeMs() const {
    if (timestepHistory_.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto& ts : timestepHistory_) {
        sum += ts.wallTimeNs;
    }
    return sum / timestepHistory_.size();
}

double PerformanceMonitor::getMinTimestepTimeMs() const {
    if (timestepHistory_.empty()) return 0.0;
    
    double min = timestepHistory_[0].wallTimeNs;
    for (const auto& ts : timestepHistory_) {
        if (ts.wallTimeNs < min) min = ts.wallTimeNs;
    }
    return min;
}

double PerformanceMonitor::getMaxTimestepTimeMs() const {
    if (timestepHistory_.empty()) return 0.0;
    
    double max = timestepHistory_[0].wallTimeNs;
    for (const auto& ts : timestepHistory_) {
        if (ts.wallTimeNs > max) max = ts.wallTimeNs;
    }
    return max;
}

double PerformanceMonitor::getTimestepTimeStdDev() const {
    if (timestepHistory_.size() < 2) return 0.0;
    
    double mean = getAverageTimestepTimeMs();
    double sumSquaredDiff = 0.0;
    for (const auto& ts : timestepHistory_) {
        double diff = ts.wallTimeNs - mean;
        sumSquaredDiff += diff * diff;
    }
    return std::sqrt(sumSquaredDiff / timestepHistory_.size());
}

void PerformanceMonitor::addMetricCallback(std::function<void(const PerformanceMetrics&)> callback) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    metricCallbacks_.push_back(callback);
}

std::string PerformanceMonitor::exportToJSON() const {
    std::stringstream ss;
    ss << "{\n";
    
    auto metrics = calculateCurrentMetrics();
    
    ss << "  \"name\": \"" << name_ << "\",\n";
    ss << "  \"timestepCount\": " << timestepHistory_.size() << ",\n";
    ss << "  \"averageTimestepTimeMs\": " << std::fixed << std::setprecision(6) << metrics.avgTimestepTimeMs << ",\n";
    ss << "  \"minTimestepTimeMs\": " << std::fixed << std::setprecision(6) << metrics.minTimestepTimeMs << ",\n";
    ss << "  \"maxTimestepTimeMs\": " << std::fixed << std::setprecision(6) << metrics.maxTimestepTimeMs << ",\n";
    ss << "  \"cpuUsagePercent\": " << std::fixed << std::setprecision(2) << metrics.cpuUsagePercent << ",\n";
    ss << "  \"memoryUsagePercent\": " << std::fixed << std::setprecision(2) << metrics.memoryUsagePercent << ",\n";
    ss << "  \"totalMemoryBytes\": " << metrics.totalMemoryBytes << ",\n";
    ss << "  \"activeNeurons\": " << activeNeurons_.load() << ",\n";
    ss << "  \"totalSpikesGenerated\": " << totalSpikesGenerated_.load() << ",\n";
    ss << "  \"totalSpikesPropagated\": " << totalSpikesPropagated_.load() << ",\n";
    
    ss << "}\n";
    return ss.str();
}

std::string PerformanceMonitor::exportToCSV() const {
    std::stringstream ss;
    
    // Header
    ss << "timestep,wallTimeNs,neuronUpdates,synapseUpdates,spikesGenerated,spikesPropagated,activeNeurons,bottleneckRatio\n";
    
    // Data
    for (const auto& ts : timestepHistory_) {
        ss << ts.timestep << ","
           << ts.wallTimeNs << ","
           << ts.neuronUpdates << ","
           << ts.synapseUpdates << ","
           << ts.spikesGenerated << ","
           << ts.spikesPropagated << ","
           << ts.activeNeurons << ","
           << ts.bottleneckRatio << "\n";
    }
    
    return ss.str();
}

std::string PerformanceMonitor::exportToYAML() const {
    YAML::Emitter out;
    out << YAML::BeginMap;
    
    out << YAML::Key << "name" << YAML::Value << name_;
    out << YAML::Key << "timestepCount" << YAML::Value << timestepHistory_.size();
    
    auto metrics = calculateCurrentMetrics();
    out << YAML::Key << "metrics" << YAML::Value;
    out << YAML::BeginMap;
    out << YAML::Key << "averageTimestepTimeMs" << YAML::Value << metrics.avgTimestepTimeMs;
    out << YAML::Key << "minTimestepTimeMs" << YAML::Value << metrics.minTimestepTimeMs;
    out << YAML::Key << "maxTimestepTimeMs" << YAML::Value << metrics.maxTimestepTimeMs;
    out << YAML::Key << "cpuUsagePercent" << YAML::Value << metrics.cpuUsagePercent;
    out << YAML::Key << "memoryUsagePercent" << YAML::Value << metrics.memoryUsagePercent;
    out << YAML::Key << "totalMemoryBytes" << YAML::Value << metrics.totalMemoryBytes;
    out << YAML::Key << "activeNeurons" << YAML::Value << activeNeurons_.load();
    out << YAML::EndMap;
    
    out << YAML::EndMap;
    
    return out.c_str();
}

std::string PerformanceMonitor::exportToXML() const {
    pugi::xml_document doc;
    auto root = doc.append_child("PerformanceMonitor");
    
    root.append_attribute("name") = name_.c_str();
    root.append_attribute("timestepCount") = static_cast<unsigned int>(timestepHistory_.size());
    
    auto metrics = calculateCurrentMetrics();
    auto metricsElem = root.append_child("Metrics");
    
    metricsElem.append_child("AverageTimestepTimeMs").append_attribute("value") = metrics.avgTimestepTimeMs;
    metricsElem.append_child("MinTimestepTimeMs").append_attribute("value") = metrics.minTimestepTimeMs;
    metricsElem.append_child("MaxTimestepTimeMs").append_attribute("value") = metrics.maxTimestepTimeMs;
    metricsElem.append_child("CpuUsagePercent").append_attribute("value") = metrics.cpuUsagePercent;
    metricsElem.append_child("MemoryUsagePercent").append_attribute("value") = metrics.memoryUsagePercent;
    metricsElem.append_child("TotalMemoryBytes").append_attribute("value") = metrics.totalMemoryBytes;
    metricsElem.append_child("ActiveNeurons").append_attribute("value") = activeNeurons_.load();
    metricsElem.append_child("TotalSpikesGenerated").append_attribute("value") = totalSpikesGenerated_.load();
    metricsElem.append_child("TotalSpikesPropagated").append_attribute("value") = totalSpikesPropagated_.load();
    
    // Export timestep data
    auto timestepsElem = root.append_child("Timesteps");
    for (const auto& ts : timestepHistory_) {
        auto timestepElem = timestepsElem.append_child("Timestep");
        timestepElem.append_attribute("number") = static_cast<unsigned int>(ts.timestep);
        timestepElem.append_attribute("wallTimeNs") = ts.wallTimeNs;
        timestepElem.append_attribute("neuronUpdates") = static_cast<unsigned int>(ts.neuronUpdates);
        timestepElem.append_attribute("synapseUpdates") = static_cast<unsigned int>(ts.synapseUpdates);
        timestepElem.append_attribute("spikesGenerated") = static_cast<unsigned int>(ts.spikesGenerated);
        timestepElem.append_attribute("spikesPropagated") = static_cast<unsigned int>(ts.spikesPropagated);
        timestepElem.append_attribute("activeNeurons") = static_cast<unsigned int>(ts.activeNeurons);
    }
    
    std::stringstream ss;
    doc.save(ss);
    return ss.str();
}

void PerformanceMonitor::integrateWithProfiler(NLPerformanceProfiler& profiler) {
    // Add callbacks to collect metrics periodically
    addMetricCallback([&profiler](const PerformanceMetrics& metrics) {
        // Update profiler with metrics
        // This would typically record events in the profiler
    });
}

void PerformanceMonitor::sampleCPUMetrics() {
    if (!config_.monitorCPU) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - cpuState_.lastCpuTimeStart_);
    
    // Update CPU time tracking
    double cpuTimeNs = elapsed.count();
    cpuState_.lastCpuTimeDurationNs += cpuTimeNs;
    cpuState_.lastCpuTimeStart_ = now;
    
    // Update thread-local CPU time
    auto threadLocal = NLPerformanceProfiler::getCurrentThreadLocalProfiler();
    if (threadLocal) {
        threadLocal->addCPUTime(cpuTimeNs);
    }
}

void PerformanceMonitor::sampleMemoryMetrics() {
    if (!config_.monitorMemory) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - memoryState_.lastMemoryCheck_);
    
    // Update memory usage
    size_t currentMem = getCurrentMemoryUsage();
    memoryState_.currentMemoryBytes.store(currentMem);
    
    // Update peak memory
    size_t peakMem = memoryState_.peakMemoryBytes.load();
    if (currentMem > peakMem) {
        memoryState_.peakMemoryBytes.store(currentMem);
    }
    
    memoryState_.lastMemoryCheck_ = now;
}

void PerformanceMonitor::sampleThreadMetrics() {
    if (!config_.monitorThreads) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - threadState_.lastThreadUpdate_);
    
    // Update thread metrics
    size_t activeThreads = 0;
    size_t totalThreads = 0;
    
    for (const auto& pair : threadState_.currentThreadMetrics_) {
        const auto& metrics = pair.second;
        activeThreads += (metrics.activeTimePercent > 0.0) ? 1 : 0;
        totalThreads++;
    }
    
    threadState_.activeThreads.store(activeThreads);
    threadState_.totalThreads.store(totalThreads);
    
    threadState_.lastThreadUpdate_ = now;
}

void PerformanceMonitor::sampleIOMetrics() {
    if (!config_.monitorIO) return;
    
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - ioState_.lastIOUpdate_);
    
    // Update I/O metrics (simplified - would typically read from OS-specific sources)
    ioState_.currentIOMetrics_.readThroughputMBps = 0.0;
    ioState_.currentIOMetrics_.writeThroughputMBps = 0.0;
    
    ioState_.lastIOUpdate_ = now;
}

double PerformanceMonitor::calculateBottleneckRatio(const TimestepPerformance& ts) const {
    if (ts.wallTimeNs <= 0) return 0.0;
    
    double maxComponentTime = std::max({
        ts.componentTiming.neuronUpdateNs,
        ts.componentTiming.synapseUpdateNs,
        ts.componentTiming.spikePropagationNs,
        ts.componentTiming.memoryManagementNs,
        ts.componentTiming.lockContentionNs,
        ts.componentTiming.otherOverheadNs
    });
    
    double ratio = maxComponentTime / ts.wallTimeNs;
    return std::min(1.0, ratio);
}

std::string PerformanceMonitor::getCurrentThreadId() {
    std::hash<std::thread::id> hasher;
    return std::to_string(hasher(std::this_thread::get_id()));
}

size_t PerformanceMonitor::getCurrentThreadStackSize() {
    pthread_t thread = pthread_self();
    struct pthread_attr_t attr;
    int result = pthread_getattr_np(thread, &attr);
    if (result == 0) {
        size_t stackSize;
        pthread_attr_getstacksize(&attr, &stackSize);
        pthread_attr_destroy(&attr);
        return stackSize;
    }
    return 0;
}

uint64_t PerformanceMonitor::getThreadNumericId() {
    pthread_t thread = pthread_self();
    return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(thread));
}

PerformanceMonitorFactory::PerformanceMonitor createMonitor(const std::string& name, 
                                                             const PerformanceMonitorConfig& config) {
    return PerformanceMonitor(name, config);
}

std::vector<PerformanceMonitorConfig> PerformanceMonitorFactory::getAvailableMonitorConfigs() {
    return std::vector<PerformanceMonitorConfig>(); // Would return predefined configs
}

PerformanceStatistics::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

PerformanceStatistics::calculateStdDev(const std::vector<double>& values, double mean) {
    if (values.empty()) return 0.0;
    double sumSquaredDiff = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sumSquaredDiff += diff * diff;
    }
    return std::sqrt(sumSquaredDiff / values.size());
}

PerformanceStatistics::calculateMedian(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    size_t n = sorted.size();
    if (n % 2 == 0) {
        return (sorted[n/2 - 1] + sorted[n/2]) / 2.0;
    } else {
        return sorted[n/2];
    }
}

PerformanceStatistics::calculatePercentile(const std::vector<double>& values, double percentile) {
    if (values.empty()) return 0.0;
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    double position = (percentile / 100.0) * (sorted.size() - 1);
    size_t lowerIndex = static_cast<size_t>(std::floor(position));
    size_t upperIndex = static_cast<size_t>(std::ceil(position));
    
    if (lowerIndex == upperIndex) {
        return sorted[lowerIndex];
    }
    
    double weight = position - lowerIndex;
    return sorted[lowerIndex] * (1.0 - weight) + sorted[upperIndex] * weight;
}

PerformanceStatistics::calculateMode(const std::vector<size_t>& values) {
    if (values.empty()) return 0;
    std::unordered_map<size_t, size_t> frequency;
    for (size_t value : values) {
        frequency[value]++;
    }
    
    size_t mode = 0;
    size_t maxCount = 0;
    for (const auto& pair : frequency) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            mode = pair.first;
        }
    }
    
    return mode;
}

PerformanceStatistics::analyzeDistribution(const std::vector<double>& values) {
    if (values.empty()) return "Empty distribution";
    
    double mean = calculateMean(values);
    double stdDev = calculateStdDev(values, mean);
    double min = *std::min_element(values.begin(), values.end());
    double max = *std::max_element(values.begin(), values.end());
    
    std::stringstream ss;
    ss << "Mean: " << mean << ", StdDev: " << stdDev << ", Min: " << min << ", Max: " << max << ", Range: " << (max - min);
    
    if (stdDev < mean * 0.1) {
        ss << ", Distribution: Tight (low variance)";
    } else if (stdDev < mean * 0.5) {
        ss << ", Distribution: Moderate (medium variance)";
    } else {
        ss << ", Distribution: Wide (high variance)";
    }
    
    return ss.str();
}

PerformanceStatistics::identifyTrends(const std::vector<double>& values) {
    if (values.size() < 3) return "Insufficient data for trend analysis";
    
    std::vector<double> x(values.size());
    std::iota(x.begin(), x.end(), 0);
    
    double correlation = calculateCorrelation(x, values);
    
    if (correlation > 0.7) {
        return "Strong upward trend";
    } else if (correlation > 0.3) {
        return "Moderate upward trend";
    } else if (correlation > -0.3) {
        return "No clear trend";
    } else if (correlation > -0.7) {
        return "Moderate downward trend";
    } else {
        return "Strong downward trend";
    }
}

PerformanceStatistics::linearRegression(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) {
        return {0.0, 0.0};
    }
    
    double sumX = std::accumulate(x.begin(), x.end(), 0.0);
    double sumY = std::accumulate(y.begin(), y.end(), 0.0);
    double sumXY = 0.0;
    double sumXX = 0.0;
    double sumYY = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sumXY += x[i] * y[i];
        sumXX += x[i] * x[i];
        sumYY += y[i] * y[i];
    }
    
    double n = static_cast<double>(x.size());
    double denominator = n * sumXX - sumX * sumX;
    
    if (denominator == 0) {
        return {0.0, 0.0};
    }
    
    double slope = (n * sumXY - sumX * sumY) / denominator;
    double intercept = (sumY - slope * sumX) / n;
    
    return {slope, intercept};
}

PerformanceStatistics::calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double sumX = std::accumulate(x.begin(), x.end(), 0.0);
    double sumY = std::accumulate(y.begin(), y.end(), 0.0);
    double sumXY = 0.0;
    double sumXX = 0.0;
    double sumYY = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sumXY += x[i] * y[i];
        sumXX += x[i] * x[i];
        sumYY += y[i] * y[i];
    }
    
    double numerator = (x.size() * sumXY - sumX * sumY);
    double denominator = std::sqrt((x.size() * sumXX - sumX * sumX) * (x.size() * sumYY - sumY * sumY));
    
    if (denominator == 0) return 0.0;
    
    return numerator / denominator;
}

PerformanceStatistics::formatTimeNs(double timeNs) {
    std::stringstream ss;
    if (timeNs >= 1e9) {
        ss << std::fixed << std::setprecision(3) << (timeNs / 1e9) << "s";
    } else if (timeNs >= 1e6) {
        ss << std::fixed << std::setprecision(3) << (timeNs / 1e6) << "ms";
    } else if (timeNs >= 1e3) {
        ss << std::fixed << std::setprecision(3) << (timeNs / 1e3) << "us";
    } else {
        ss << std::fixed << std::setprecision(3) << timeNs << "ns";
    }
    return ss.str();
}

PerformanceStatistics::formatMemoryBytes(size_t bytes) {
    std::stringstream ss;
    if (bytes >= 1024 * 1024 * 1024) {
        ss << std::fixed << std::setprecision(2) << (bytes / (1024 * 1024 * 1024)) << "GB";
    } else if (bytes >= 1024 * 1024) {
        ss << std::fixed << std::setprecision(2) << (bytes / (1024 * 1024)) << "MB";
    } else if (bytes >= 1024) {
        ss << std::fixed << std::setprecision(2) << (bytes / 1024) << "KB";
    } else {
        ss << bytes << "B";
    }
    return ss.str();
}

PerformanceStatistics::formatThroughput(double bytes, double timeNs) {
    if (timeNs <= 0) return "N/A";
    double mbps = (bytes * 8) / (timeNs / 1e9) / (1024 * 1024);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << mbps << "Mbps";
    return ss.str();
}

} // namespace nlm