#include "PerformanceProfiler.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <thread>
#include <chrono>
#include <cmath>
#include <cstring>
#include <csignal>
#include <sys/resource.h>
#include <unistd.h>
#include <fstream>

namespace nlm {

struct PerformanceProfiler::Impl {
    bool initialized;
    bool profiling;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    double samplingInterval;
    size_t maxHistorySize;
    bool enableResourceMonitoring;
    
    PerformanceMetrics currentMetrics;
    std::deque<PerformanceMetrics> metricsHistory;
    std::deque<TimeSeriesPoint> timeSeries;
    std::vector<NeuralActivityHeatmap> activityHeatmaps;
    ResourceMetrics lastResourceMetrics;
    
    ProgressReport currentProgress;
    ProgressCallback progressCallback;
    
    std::mutex metricsMutex;
    std::thread profilingThread;
    std::atomic<bool> running;
    
    // CPU time tracking
    struct timespec lastCpuTimes[3];  // [0]user, [1]system, [2]total
    
    // Memory tracking
    uint64_t peakMemoryUsed;
    
    // Spike tracking
    uint64_t spikesSinceLastUpdate;
    uint64_t activeNeuronsSinceLastUpdate;
    std::unordered_map<NeuronId, bool> neuronActiveState;
    
    // Performance scoring
    std::deque<double> recentTimePerStep;
    static constexpr size_t MAX_TIME_PER_STEP_HISTORY = 100;
    
    Impl() : initialized(false), profiling(false), samplingInterval(1.0),
             maxHistorySize(10000), enableResourceMonitoring(true),
             running(false), peakMemoryUsed(0), spikesSinceLastUpdate(0),
             activeNeuronsSinceLastUpdate(0) {
        std::memset(lastCpuTimes, 0, sizeof(lastCpuTimes));
    }
    
    ~Impl() {
        if (running) {
            stopProfiling();
        }
    }
    
    void initializeThread() {
        profilingThread = std::thread(&Impl::profilingLoop, this);
    }
    
    void shutdownThread() {
        running = false;
        if (profilingThread.joinable()) {
            profilingThread.join();
        }
    }
    
    void profilingLoop() {
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(
                static_cast<int>(samplingInterval * 1000)));
            
            update();
        }
    }
    
    void update() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - lastUpdateTime).count();
        lastUpdateTime = now;
        
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        // Update time-based metrics
        currentMetrics.systemUptime = static_cast<int64_t>(elapsed);
        
        // Update CPU usage
        updateCpuUsage();
        
        // Update memory metrics
        updateMemoryMetrics();
        
        // Update simulation metrics
        updateSimulationMetrics(elapsed);
        
        // Update neural activity metrics
        updateNeuralActivityMetrics();
        
        // Update progress
        currentMetrics.simulationStep = currentProgress.currentStep;
        currentMetrics.simulationTime = currentProgress.currentStep * currentMetrics.timePerStep;
        
        // Add to history
        if (metricsHistory.size() < maxHistorySize) {
            metricsHistory.push_back(currentMetrics);
        } else {
            metricsHistory.pop_front();
            metricsHistory.push_back(currentMetrics);
        }
        
        // Add to time series
        TimeSeriesPoint point;
        point.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
            now.time_since_epoch()).count();
        point.metrics = currentMetrics;
        
        if (timeSeries.size() < maxHistorySize) {
            timeSeries.push_back(point);
        } else {
            timeSeries.pop_front();
            timeSeries.push_back(point);
        }
        
        // Call progress callback if set
        if (progressCallback) {
            progressCallback(currentProgress);
        }
    }
    
    void updateCpuUsage() {
        // Get CPU times
        struct timespec currentTimes[3];
        if (getrusage(RUSAGE_SELF, nullptr) != 0) {
            return;
        }
        
        // This is a simplified implementation - real systems would use /proc/stat
        // or platform-specific APIs
        double cpuUser = currentTimes[0].tv_sec * 1e9 + currentTimes[0].tv_nsec;
        double cpuSystem = currentTimes[1].tv_sec * 1e9 + currentTimes[1].tv_nsec;
        
        if (lastCpuTimes[0].tv_sec > 0) {
            double deltaUser = cpuUser - (lastCpuTimes[0].tv_sec * 1e9 + lastCpuTimes[0].tv_nsec);
            double deltaSystem = cpuSystem - (lastCpuTimes[1].tv_sec * 1e9 + lastCpuTimes[1].tv_nsec);
            double deltaTotal = deltaUser + deltaSystem;
            
            if (deltaTotal > 0) {
                currentMetrics.cpuUsage = std::min(100.0, (deltaUser / deltaTotal) * 100.0);
            }
        }
        
        std::memcpy(lastCpuTimes, currentTimes, sizeof(lastCpuTimes));
    }
    
    void updateMemoryMetrics() {
        // Get memory usage
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            currentMetrics.memoryUsed = usage.ru_maxrss * 1024;  // Convert KB to bytes
            if (currentMetrics.memoryUsed > peakMemoryUsed) {
                peakMemoryUsed = currentMetrics.memoryUsed;
            }
            currentMetrics.memoryPeak = peakMemoryUsed;
        }
        
        // Estimate total memory (simplified)
        currentMetrics.memoryUsagePercent = std::min(100.0, 
            (static_cast<double>(currentMetrics.memoryUsed) / 4.0e9) * 100.0);
    }
    
    void updateSimulationMetrics(double elapsed) {
        // Update time per step
        if (elapsed > 0) {
            currentMetrics.timePerStep = 1.0 / elapsed;
            
            // Track recent time per step for performance scoring
            recentTimePerStep.push_back(currentMetrics.timePerStep);
            if (recentTimePerStep.size() > MAX_TIME_PER_STEP_HISTORY) {
                recentTimePerStep.pop_front();
            }
        }
        
        currentMetrics.totalCpuTime += elapsed * currentMetrics.cpuUsage / 100.0;
    }
    
    void updateNeuralActivityMetrics() {
        // Reset counters
        spikesSinceLastUpdate = 0;
        activeNeuronsSinceLastUpdate = 0;
    }
    
    void recordSpike() {
        std::lock_guard<std::mutex> lock(metricsMutex);
        spikesSinceLastUpdate++;
        currentMetrics.totalSpikes++;
    }
    
    void recordNeuronActivation(NeuronId neuronId) {
        std::lock_guard<std::mutex> lock(metricsMutex);
        activeNeuronsSinceLastUpdate++;
        neuronActiveState[neuronId] = true;
    }
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {}

PerformanceProfiler::~PerformanceProfiler() = default;

bool PerformanceProfiler::initialize() {
    if (pImpl->initialized) {
        return true;
    }
    
    pImpl->initialized = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastUpdateTime = pImpl->startTime;
    
    pImpl->running = true;
    pImpl->initializeThread();
    
    NLM_LOG_INFO("PerformanceProfiler initialized");
    return true;
}

void PerformanceProfiler::shutdown() {
    if (!pImpl->initialized) {
        return;
    }
    
    pImpl->shutdownThread();
    pImpl->initialized = false;
    
    NLM_LOG_INFO("PerformanceProfiler shutdown");
}

bool PerformanceProfiler::isInitialized() const {
    return pImpl->initialized;
}

void PerformanceProfiler::startProfiling() {
    if (pImpl->profiling) {
        return;
    }
    
    pImpl->profiling = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastUpdateTime = pImpl->startTime;
    
    NLM_LOG_INFO("Performance profiling started");
}

void PerformanceProfiler::stopProfiling() {
    if (!pImpl->profiling) {
        return;
    }
    
    pImpl->profiling = false;
    
    NLM_LOG_INFO("Performance profiling stopped");
}

bool PerformanceProfiler::isProfiling() const {
    return pImpl->profiling;
}

void PerformanceProfiler::update() {
    pImpl->update();
}

PerformanceMetrics PerformanceProfiler::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->currentMetrics;
}

std::vector<PerformanceMetrics> PerformanceProfiler::getRecentMetrics(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::vector<PerformanceMetrics> result;
    result.reserve(std::min(count, pImpl->metricsHistory.size()));
    
    size_t startPos = pImpl->metricsHistory.size() > count ? 
        pImpl->metricsHistory.size() - count : 0;
    
    for (size_t i = startPos; i < pImpl->metricsHistory.size(); ++i) {
        result.push_back(pImpl->metricsHistory[i]);
    }
    
    return result;
}

std::vector<TimeSeriesPoint> PerformanceProfiler::getTimeSeries(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::vector<TimeSeriesPoint> result;
    result.reserve(std::min(count, pImpl->timeSeries.size()));
    
    size_t startPos = pImpl->timeSeries.size() > count ? 
        pImpl->timeSeries.size() - count : 0;
    
    for (size_t i = startPos; i < pImpl->timeSeries.size(); ++i) {
        result.push_back(pImpl->timeSeries[i]);
    }
    
    return result;
}

void PerformanceProfiler::captureActivityHeatmap() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    NeuralActivityHeatmap heatmap;
    heatmap.step = pImpl->currentMetrics.simulationStep;
    heatmap.timestamp = pImpl->currentMetrics.simulationTime;
    
    if (pImpl->metricsHistory.empty()) {
        return;
    }
    
    const auto& lastMetrics = pImpl->metricsHistory.back();
    
    NLM_LOG_INFO("Activity heatmap captured at step " + 
                 std::to_string(heatmap.step) + " with " +
                 std::to_string(lastMetrics.firingNeurons) + " firing neurons");
}

std::vector<NeuralActivityHeatmap> PerformanceProfiler::getActivityHeatmaps(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::vector<NeuralActivityHeatmap> result;
    result.reserve(std::min(count, pImpl->activityHeatmaps.size()));
    
    size_t startPos = pImpl->activityHeatmaps.size() > count ? 
        pImpl->activityHeatmaps.size() - count : 0;
    
    for (size_t i = startPos; i < pImpl->activityHeatmaps.size(); ++i) {
        result.push_back(pImpl->activityHeatmaps[i]);
    }
    
    return result;
}

ResourceMetrics PerformanceProfiler::getCurrentResourceMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->lastResourceMetrics;
}

void PerformanceProfiler::setProgress(double progress, const std::string& message,
                                      int64_t currentStep, int64_t totalSteps) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    auto now = std::chrono::steady_clock::now();
    double eta = 0.0;
    double rate = 0.0;
    
    if (progress > 0.0 && progress < 1.0 && totalSteps > 0 && currentStep > 0) {
        rate = static_cast<double>(currentStep) / (now - pImpl->startTime).count();
        if (rate > 0.0) {
            eta = (totalSteps - currentStep) / rate;
        }
    }
    
    pImpl->currentProgress = ProgressReport(progress, message, currentStep, totalSteps, eta, rate);
}

ProgressReport PerformanceProfiler::getCurrentProgress() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    return pImpl->currentProgress;
}

void PerformanceProfiler::onSpikeGenerated() {
    pImpl->recordSpike();
}

void PerformanceProfiler::onNeuronActivated(NeuronId neuronId) {
    pImpl->recordNeuronActivation(neuronId);
}

void PerformanceProfiler::onCheckpointSaved(const std::string& filepath, uint64_t bytesWritten) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->currentMetrics.checkpointSaves++;
    NLM_LOG_INFO("Checkpoint saved: " + filepath + " (" + std::to_string(bytesWritten) + " bytes)");
}

void PerformanceProfiler::onCheckpointLoaded(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->currentMetrics.checkpointLoads++;
    NLM_LOG_INFO("Checkpoint loaded: " + filepath);
}

BottleneckAnalysis PerformanceProfiler::analyzeBottlenecks() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    BottleneckAnalysis analysis;
    
    if (pImpl->metricsHistory.size() >= 10) {
        // Check for CPU bottleneck
        double avgCpuUsage = 0.0;
        for (const auto& metrics : pImpl->metricsHistory) {
            avgCpuUsage += metrics.cpuUsage;
        }
        avgCpuUsage /= pImpl->metricsHistory.size();
        
        if (avgCpuUsage > 90.0) {
            analysis.type = "CPU bottleneck";
            analysis.severity = std::min(1.0, (avgCpuUsage - 90.0) / 10.0);
            analysis.location = "System CPU usage exceeded threshold";
            analysis.recommendation = "Optimize CPU-intensive operations or scale horizontally";
        }
        
        // Check for memory bottleneck
        double avgMemoryUsage = 0.0;
        for (const auto& metrics : pImpl->metricsHistory) {
            avgMemoryUsage += metrics.memoryUsagePercent;
        }
        avgMemoryUsage /= pImpl->metricsHistory.size();
        
        if (avgMemoryUsage > 80.0) {
            analysis.type = "Memory bottleneck";
            analysis.severity = std::min(1.0, (avgMemoryUsage - 80.0) / 20.0);
            analysis.location = "Memory usage exceeds optimal range";
            analysis.recommendation = "Optimize memory allocation or increase memory capacity";
        }
    }
    
    return analysis;
}

double PerformanceProfiler::calculatePerformanceScore() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    if (pImpl->recentTimePerStep.empty()) {
        return 100.0;
    }
    
    double avgTimePerStep = std::accumulate(pImpl->recentTimePerStep.begin(),
                                           pImpl->recentTimePerStep.end(), 0.0) /
                                           pImpl->recentTimePerStep.size();
    
    // Score based on time per step (lower is better)
    // Target: 0.001 seconds per step (1 kHz simulation)
    double targetTimePerStep = 0.001;
    double score = std::max(0.0, 100.0 * (targetTimePerStep / avgTimePerStep));
    
    return score;
}

void PerformanceProfiler::setSamplingInterval(double intervalSeconds) {
    pImpl->samplingInterval = std::max(0.1, intervalSeconds);
}

void PerformanceProfiler::setMaxHistorySize(size_t size) {
    pImpl->maxHistorySize = std::max(size_t(100), size);
}

void PerformanceProfiler::setEnableResourceMonitoring(bool enable) {
    pImpl->enableResourceMonitoring = enable;
}

double PerformanceProfiler::getUptime() const {
    if (!pImpl->initialized) {
        return 0.0;
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration<double>(now - pImpl->startTime).count();
    return elapsed;
}

void PerformanceProfiler::reset() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->metricsHistory.clear();
    pImpl->timeSeries.clear();
    pImpl->activityHeatmaps.clear();
    pImpl->spikesSinceLastUpdate = 0;
    pImpl->activeNeuronsSinceLastUpdate = 0;
    pImpl->neuronActiveState.clear();
    pImpl->recentTimePerStep.clear();
    
    NLM_LOG_INFO("PerformanceProfiler reset");
}

void PerformanceProfiler::setProgressCallback(ProgressCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->progressCallback = callback;
}

} // namespace nlm