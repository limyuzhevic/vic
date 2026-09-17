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
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <future>

namespace nlm {

struct PerformanceProfiler::Impl {
    bool initialized;
    bool profiling;
    bool realTimeMonitoring;
    ProfilingMode profilingMode;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    double samplingInterval;
    size_t maxHistorySize;
    bool enableResourceMonitoring;
    bool enableNeuralActivityMonitoring;
    bool enableBottleneckDetection;
    bool enableRegressionDetection;
    std::string baselineFile;
    std::string outputDir;
    
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
    uint64_t lastMemoryUsed;
    
    // Spike tracking
    uint64_t spikesSinceLastUpdate;
    uint64_t activeNeuronsSinceLastUpdate;
    std::unordered_map<NeuronId, bool> neuronActiveState;
    
    // Performance scoring
    std::deque<double> recentTimePerStep;
    static constexpr size_t MAX_TIME_PER_STEP_HISTORY = 100;
    
    // Neural activity tracking
    std::vector<float> currentActivityData;
    int64_t lastHeatmapStep;
    
    // Benchmarking
    bool benchmarking;
    std::string currentBenchmarkName;
    std::chrono::steady_clock::time_point benchmarkStartTime;
    
    // Real-time monitoring
    std::atomic<bool> realTimeRunning;
    std::thread realTimeThread;
    
    // Baseline for regression detection
    std::map<std::string, double> baselineMetrics;
    
    // File I/O
    std::ofstream metricsStream;
    std::ofstream heatmapsStream;
    std::ofstream timeSeriesStream;
    
    Impl() : initialized(false), profiling(false), realTimeMonitoring(false),
             profilingMode(ProfilingMode::Realtime), samplingInterval(1.0),
             maxHistorySize(10000), enableResourceMonitoring(true),
             enableNeuralActivityMonitoring(true), enableBottleneckDetection(true),
             enableRegressionDetection(true), running(false),
             realTimeRunning(false), peakMemoryUsed(0), lastMemoryUsed(0),
             spikesSinceLastUpdate(0), activeNeuronsSinceLastUpdate(0),
             lastHeatmapStep(-1), benchmarking(false), currentBenchmarkName(""),
             realTimeThread() {
        std::memset(lastCpuTimes, 0, sizeof(lastCpuTimes));
    }
    
    ~Impl() {
        if (running) {
            stopProfiling();
        }
        if (realTimeRunning) {
            realTimeMonitoring = false;
            if (realTimeThread.joinable()) {
                realTimeThread.join();
            }
        }
        closeOutputFiles();
    }
    
    void closeOutputFiles() {
        if (metricsStream.is_open()) {
            metricsStream.close();
        }
        if (heatmapsStream.is_open()) {
            heatmapsStream.close();
        }
        if (timeSeriesStream.is_open()) {
            timeSeriesStream.close();
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
    
    void realTimeMonitoringLoop() {
        while (realTimeRunning) {
            updateCurrentMetrics();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
        
        // Write to output files based on profiling mode
        writeOutputFiles();
        
        // Call progress callback if set
        if (progressCallback) {
            progressCallback(currentProgress);
        }
    }
    
    void updateCurrentMetrics() {
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration<double>(now - lastUpdateTime).count();
        lastUpdateTime = now;
        
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
    }
    
    void updateCpuUsage() {
        // Get CPU times using getrusage
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) != 0) {
            return;
        }
        
        // This is a simplified implementation - real systems would use /proc/stat
        // or platform-specific APIs for more accurate CPU usage
        double cpuUser = usage.ru_utime.tv_sec * 1e9 + usage.ru_utime.tv_usec;
        double cpuSystem = usage.ru_stime.tv_sec * 1e9 + usage.ru_stime.tv_usec;
        double cpuTotal = cpuUser + cpuSystem;
        
        if (lastCpuTimes[0].tv_sec > 0) {
            double lastUser = lastCpuTimes[0].tv_sec * 1e9 + lastCpuTimes[0].tv_nsec;
            double lastSystem = lastCpuTimes[1].tv_sec * 1e9 + lastCpuTimes[1].tv_nsec;
            double lastTotal = lastUser + lastSystem;
            
            double deltaUser = cpuUser - lastUser;
            double deltaSystem = cpuSystem - lastSystem;
            double deltaTotal = cpuTotal - lastTotal;
            
            if (deltaTotal > 0) {
                currentMetrics.cpuUsage = std::min(100.0, (deltaUser / deltaTotal) * 100.0);
            }
        }
        
        lastCpuTimes[0].tv_sec = usage.ru_utime.tv_sec;
        lastCpuTimes[0].tv_nsec = usage.ru_utime.tv_usec * 1000;
        lastCpuTimes[1].tv_sec = usage.ru_stime.tv_sec;
        lastCpuTimes[1].tv_nsec = usage.ru_stime.tv_usec * 1000;
        lastCpuTimes[2].tv_sec = usage.ru_maxrss / 1024; // Convert KB to seconds (simplified)
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
        
        // Track idle CPU time
        if (currentMetrics.cpuUsage < 1.0) {
            currentMetrics.idleCpuTime += 1.0; // Approximate
        }
    }
    
    void updateSimulationMetrics(double elapsed) {
        // Update time per step
        if (elapsed > 0) {
            currentMetrics.timePerStep = 1.0 / elapsed;
            
            // Track recent time per step for performance scoring and statistics
            recentTimePerStep.push_back(currentMetrics.timePerStep);
            if (recentTimePerStep.size() > MAX_TIME_PER_STEP_HISTORY) {
                recentTimePerStep.pop_front();
            }
            
            // Calculate standard deviation of time per step
            if (recentTimePerStep.size() >= 2) {
                double sum = std::accumulate(recentTimePerStep.begin(), recentTimePerStep.end(), 0.0);
                double mean = sum / recentTimePerStep.size();
                double sqSum = 0.0;
                for (double val : recentTimePerStep) {
                    sqSum += (val - mean) * (val - mean);
                }
                currentMetrics.timePerStepStdDev = std::sqrt(sqSum / (recentTimePerStep.size() - 1));
            }
        }
        
        currentMetrics.totalCpuTime += elapsed * currentMetrics.cpuUsage / 100.0;
    }
    
    void updateNeuralActivityMetrics() {
        // Reset counters
        spikesSinceLastUpdate = 0;
        activeNeuronsSinceLastUpdate = 0;
        
        // Update average firing rate statistics
        if (recentTimePerStep.size() >= 10) {
            double sum = std::accumulate(recentTimePerStep.begin(), recentTimePerStep.end(), 0.0);
            double avgTimePerStep = sum / recentTimePerStep.size();
            currentMetrics.averageFiringRate = 1.0 / avgTimePerStep; // Hz
            
            // Calculate standard deviation
            double mean = currentMetrics.averageFiringRate;
            double sqSum = 0.0;
            for (double val : recentTimePerStep) {
                double rate = 1.0 / val;
                sqSum += (rate - mean) * (rate - mean);
            }
            if (recentTimePerStep.size() >= 2) {
                currentMetrics.averageFiringRateStdDev = std::sqrt(sqSum / (recentTimePerStep.size() - 1));
            }
        }
        
        // Update active neurons count
        currentMetrics.activeNeurons = activeNeuronsSinceLastUpdate;
        currentMetrics.firingNeurons = spikesSinceLastUpdate;
        
        // Update activity heatmap data
        if (enableNeuralActivityMonitoring) {
            updateActivityData();
        }
    }
    
    void updateActivityData() {
        // Simulate neural activity data update
        // In a real implementation, this would capture actual neural activity data
        if (!currentActivityData.empty()) {
            double sum = std::accumulate(currentActivityData.begin(), currentActivityData.end(), 0.0f);
            double mean = sum / currentActivityData.size();
            
            // Update heatmap data
            if (currentActivityData.size() > 0) {
                currentMetrics.maxActivity = *std::max_element(currentActivityData.begin(), currentActivityData.end());
                currentMetrics.minActivity = *std::min_element(currentActivityData.begin(), currentActivityData.end());
                currentMetrics.averageFiringRate = mean;
            }
        }
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
    
    void captureCurrentHeatmap() {
        if (!enableNeuralActivityMonitoring) return;
        
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        NeuralActivityHeatmap heatmap;
        heatmap.step = currentMetrics.simulationStep;
        heatmap.timestamp = currentMetrics.simulationTime;
        
        // Simulate activity data based on current metrics
        // In a real implementation, this would capture actual neural population activity
        int regionCount = 10; // Default region count
        int neuronsPerRegion = 100; // Default neurons per region
        heatmap.regionCount = regionCount;
        heatmap.neuronsPerRegion = neuronsPerRegion;
        
        heatmap.activityData.resize(regionCount * neuronsPerRegion);
        for (size_t i = 0; i < heatmap.activityData.size(); ++i) {
            // Simulate activity with some variation based on firing rate
            heatmap.activityData[i] = currentMetrics.averageFiringRate / 100.0f; // Normalize
        }
        
        // Calculate statistics
        if (!heatmap.activityData.empty()) {
            heatmap.maxActivity = *std::max_element(heatmap.activityData.begin(), heatmap.activityData.end());
            heatmap.minActivity = *std::min_element(heatmap.activityData.begin(), heatmap.activityData.end());
            
            double sum = std::accumulate(heatmap.activityData.begin(), heatmap.activityData.end(), 0.0f);
            heatmap.averageActivity = sum / heatmap.activityData.size();
            
            // Generate normalized data
            heatmap.normalizedData.resize(heatmap.activityData.size());
            if (heatmap.maxActivity > heatmap.minActivity) {
                for (size_t i = 0; i < heatmap.activityData.size(); ++i) {
                    heatmap.normalizedData[i] = (heatmap.activityData[i] - heatmap.minActivity) /
                                             (heatmap.maxActivity - heatmap.minActivity);
                }
            } else {
                std::fill(heatmap.normalizedData.begin(), heatmap.normalizedData.end(), 0.5f);
            }
        }
        
        activityHeatmaps.push_back(heatmap);
        lastHeatmapStep = currentMetrics.simulationStep;
        
        // Write to heatmaps stream if recording
        if (heatmapsStream.is_open() && profilingMode == ProfilingMode::Recording) {
            heatmapsStream << std::fixed << std::setprecision(6);
            heatmapsStream << "step," << heatmap.step << ",time," << heatmap.timestamp << ",regions," << heatmap.regionCount << ",neurons_per_region," << heatmap.neuronsPerRegion << ",";
            for (size_t i = 0; i < heatmap.activityData.size(); ++i) {
                heatmapsStream << heatmap.activityData[i];
                if (i < heatmap.activityData.size() - 1) heatmapsStream << ",";
            }
            heatmapsStream << std::endl;
        }
    }
    
    void writeOutputFiles() {
        if (outputDir.empty()) return;
        
        // Ensure output directory exists
        std::filesystem::create_directories(outputDir);
        
        // Open files if recording mode
        if (profilingMode == ProfilingMode::Recording || profilingMode == ProfilingMode::Benchmarking) {
            if (!metricsStream.is_open()) {
                metricsStream.open(outputDir + "/metrics.csv", std::ios::app);
                if (metricsStream.is_open()) {
                    metricsStream << "timestamp,step,simulation_time,cpu_usage,memory_used,memory_peak,memory_percent,time_per_step,time_per_step_stddev,total_spikes,active_neurons,firing_neurons,firing_rate,std_dev_firing_rate" << std::endl;
                }
            }
            
            if (!timeSeriesStream.is_open()) {
                timeSeriesStream.open(outputDir + "/time_series.csv", std::ios::app);
                if (timeSeriesStream.is_open()) {
                    timeSeriesStream << "timestamp,step,simulation_time,cpu_usage,memory_used,memory_peak,memory_percent,time_per_step" << std::endl;
                }
            }
        }
        
        // Write metrics if stream is open
        if (metricsStream.is_open()) {
            metricsStream << std::fixed << std::setprecision(6);
            metricsStream << currentMetrics.simulationTime << ","
                         << currentMetrics.simulationStep << ","
                         << currentMetrics.cpuUsage << ","
                         << currentMetrics.memoryUsed << ","
                         << currentMetrics.memoryPeak << ","
                         << currentMetrics.memoryUsagePercent << ","
                         << currentMetrics.timePerStep << ","
                         << currentMetrics.timePerStepStdDev << ","
                         << currentMetrics.totalSpikes << ","
                         << currentMetrics.activeNeurons << ","
                         << currentMetrics.firingNeurons << ","
                         << currentMetrics.averageFiringRate << ","
                         << currentMetrics.averageFiringRateStdDev << std::endl;
        }
        
        // Write time series if stream is open
        if (timeSeriesStream.is_open()) {
            timeSeriesStream << std::fixed << std::setprecision(6);
            timeSeriesStream << currentMetrics.simulationTime << ","
                           << currentMetrics.simulationStep << ","
                           << currentMetrics.cpuUsage << ","
                           << currentMetrics.memoryUsed << ","
                           << currentMetrics.memoryPeak << ","
                           << currentMetrics.memoryUsagePercent << ","
                           << currentMetrics.timePerStep << std::endl;
        }
    }
    
    void loadBaselineFromFile() {
        if (baselineFile.empty()) return;
        
        std::ifstream file(baselineFile);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Baseline file not found: " + baselineFile);
            return;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string metric;
            double value;
            if (std::getline(iss, metric, ',') && iss >> value) {
                baselineMetrics[metric] = value;
            }
        }
        
        file.close();
        NLM_LOG_INFO("Loaded baseline metrics from: " + baselineFile);
    }
    
    std::vector<PerformanceRegression> detectRegressions() {
        std::vector<PerformanceRegression> regressions;
        
        for (const auto& [metric, baseline] : baselineMetrics) {
            double current = 0.0;
            
            // Map metric names to current metrics
            if (metric == "cpu_usage") current = currentMetrics.cpuUsage;
            else if (metric == "memory_usage") current = currentMetrics.memoryUsagePercent;
            else if (metric == "time_per_step") current = currentMetrics.timePerStep;
            else if (metric == "firing_rate") current = currentMetrics.averageFiringRate;
            else continue;
            
            if (baseline > 0) {
                double regressionPercent = ((current - baseline) / baseline) * 100.0;
                
                std::string severity = "info";
                if (std::abs(regressionPercent) > 50.0) severity = "critical";
                else if (std::abs(regressionPercent) > 20.0) severity = "warning";
                
                std::string recommendation = "Investigate " + metric + " performance changes";
                
                PerformanceRegression reg(metric, baseline, current, regressionPercent, severity, recommendation);
                regressions.push_back(reg);
            }
        }
        
        return regressions;
    }
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {}

PerformanceProfiler::~PerformanceProfiler() = default;

bool PerformanceProfiler::initialize() {
    if (pImpl->initialized) {
        return true;
    }
    
    // Create output directory if specified
    if (!pImpl->outputDir.empty()) {
        std::filesystem::create_directories(pImpl->outputDir);
    }
    
    pImpl->initialized = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastUpdateTime = pImpl->startTime;
    
    pImpl->running = true;
    pImpl->initializeThread();
    
    pImpl->loadBaselineFromFile();
    
    NLM_LOG_INFO("Enhanced PerformanceProfiler initialized");
    return true;
}

void PerformanceProfiler::shutdown() {
    if (!pImpl->initialized) {
        return;
    }
    
    pImpl->shutdownThread();
    pImpl->initialized = false;
    
    pImpl->closeOutputFiles();
    
    NLM_LOG_INFO("Enhanced PerformanceProfiler shutdown");
}

bool PerformanceProfiler::isInitialized() const {
    return pImpl->initialized;
}

void PerformanceProfiler::setProfilingMode(ProfilingMode mode) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->profilingMode = mode;
}

void PerformanceProfiler::setSamplingInterval(double intervalSeconds) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->samplingInterval = std::max(0.1, intervalSeconds);
}

void PerformanceProfiler::setMaxHistorySize(size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->maxHistorySize = std::max(size_t(100), size);
}

void PerformanceProfiler::setEnableResourceMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->enableResourceMonitoring = enable;
}

void PerformanceProfiler::setEnableNeuralActivityMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->enableNeuralActivityMonitoring = enable;
}

void PerformanceProfiler::setEnableBottleneckDetection(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->enableBottleneckDetection = enable;
}

void PerformanceProfiler::setEnableRegressionDetection(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->enableRegressionDetection = enable;
}

void PerformanceProfiler::setBaselineMetrics(const std::string& baselineFile) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->baselineFile = baselineFile;
}

void PerformanceProfiler::setOutputDirectory(const std::string& outputDir) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->outputDir = outputDir;
}

void PerformanceProfiler::startProfiling() {
    if (pImpl->profiling) {
        return;
    }
    
    pImpl->profiling = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastUpdateTime = pImpl->startTime;
    
    NLM_LOG_INFO("Enhanced PerformanceProfiler profiling started");
}

void PerformanceProfiler::stopProfiling() {
    if (!pImpl->profiling) {
        return;
    }
    
    pImpl->profiling = false;
    
    NLM_LOG_INFO("Enhanced PerformanceProfiler profiling stopped");
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
    pImpl->captureCurrentHeatmap();
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
    NLM_LOG_INFO("Enhanced PerformanceProfiler: Checkpoint saved: " + filepath + " (" + std::to_string(bytesWritten) + " bytes)");
}

void PerformanceProfiler::onCheckpointLoaded(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->currentMetrics.checkpointLoads++;
    NLM_LOG_INFO("Enhanced PerformanceProfiler: Checkpoint loaded: " + filepath);
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
        
        // Check for time-per-step bottleneck
        double avgTimePerStep = 0.0;
        for (const auto& metrics : pImpl->metricsHistory) {
            avgTimePerStep += metrics.timePerStep;
        }
        avgTimePerStep /= pImpl->metricsHistory.size();
        
        if (avgTimePerStep > 0.01) { // 10 ms per step
            analysis.type = "Computation bottleneck";
            analysis.severity = std::min(1.0, (avgTimePerStep - 0.01) / 0.01);
            analysis.location = "Simulation step processing time";
            analysis.recommendation = "Optimize neural computation algorithms or use SIMD vectorization";
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

std::vector<PerformanceRegression> PerformanceProfiler::analyzePerformanceRegression() const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    return pImpl->detectRegressions();
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
    pImpl->currentActivityData.clear();
    
    NLM_LOG_INFO("Enhanced PerformanceProfiler reset");
}

void PerformanceProfiler::setProgressCallback(ProgressCallback callback) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->progressCallback = callback;
}

bool PerformanceProfiler::exportMetrics(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "timestamp,step,simulation_time,cpu_usage,memory_used,memory_peak,memory_percent,time_per_step,time_per_step_stddev,total_spikes,active_neurons,firing_neurons,firing_rate,std_dev_firing_rate" << std::endl;
        
        auto metrics = getRecentMetrics();
        for (const auto& metrics : metrics) {
            file << metrics.simulationTime << ","
                 << metrics.simulationStep << ","
                 << metrics.cpuUsage << ","
                 << metrics.memoryUsed << ","
                 << metrics.memoryPeak << ","
                 << metrics.memoryUsagePercent << ","
                 << metrics.timePerStep << ","
                 << metrics.timePerStepStdDev << ","
                 << metrics.totalSpikes << ","
                 << metrics.activeNeurons << ","
                 << metrics.firingNeurons << ","
                 << metrics.averageFiringRate << ","
                 << metrics.averageFiringRateStdDev << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Enhanced PerformanceProfiler: Exported metrics to " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Enhanced PerformanceProfiler: Failed to export metrics: " + std::string(e.what()));
        return false;
    }
}

bool PerformanceProfiler::exportHeatmaps(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "step,timestamp,region_count,neurons_per_region,max_activity,min_activity,average_activity" << std::endl;
        
        auto heatmaps = getActivityHeatmaps();
        for (const auto& heatmap : heatmaps) {
            file << heatmap.step << ","
                 << heatmap.timestamp << ","
                 << heatmap.regionCount << ","
                 << heatmap.neuronsPerRegion << ","
                 << heatmap.maxActivity << ","
                 << heatmap.minActivity << ","
                 << heatmap.averageActivity << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Enhanced PerformanceProfiler: Exported heatmaps to " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Enhanced PerformanceProfiler: Failed to export heatmaps: " + std::string(e.what()));
        return false;
    }
}

bool PerformanceProfiler::exportTimeSeries(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "timestamp,step,simulation_time,cpu_usage,memory_used,memory_peak,memory_percent,time_per_step" << std::endl;
        
        auto timeSeries = getTimeSeries();
        for (const auto& point : timeSeries) {
            file << point.timestamp << ","
                 << point.metrics.simulationStep << ","
                 << point.metrics.simulationTime << ","
                 << point.metrics.cpuUsage << ","
                 << point.metrics.memoryUsed << ","
                 << point.metrics.memoryPeak << ","
                 << point.metrics.memoryUsagePercent << ","
                 << point.metrics.timePerStep << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Enhanced PerformanceProfiler: Exported time series to " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Enhanced PerformanceProfiler: Failed to export time series: " + std::string(e.what()));
        return false;
    }
}

bool PerformanceProfiler::exportAnalysisReport(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Enhanced PerformanceProfiler Analysis Report" << std::endl;
        file << "=============================================" << std::endl << std::endl;
        
        // Current metrics
        auto metrics = getCurrentMetrics();
        file << "Current Performance Metrics:" << std::endl;
        file << "  Simulation Step: " << metrics.simulationStep << std::endl;
        file << "  Simulation Time: " << metrics.simulationTime << " seconds" << std::endl;
        file << "  CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
        file << "  Memory Used: " << (metrics.memoryUsed / 1024 / 1024) << " MB" << std::endl;
        file << "  Memory Peak: " << (metrics.memoryPeak / 1024 / 1024) << " MB" << std::endl;
        file << "  Memory Usage: " << metrics.memoryUsagePercent << "%" << std::endl;
        file << "  Time per Step: " << metrics.timePerStep * 1000 << " ms" << std::endl;
        file << "  Total Spikes: " << metrics.totalSpikes << std::endl;
        file << "  Active Neurons: " << metrics.activeNeurons << std::endl;
        file << "  Firing Neurons: " << metrics.firingNeurons << std::endl;
        file << "  Average Firing Rate: " << metrics.averageFiringRate << " Hz" << std::endl;
        file << "  Firing Rate Std Dev: " << metrics.averageFiringRateStdDev << " Hz" << std::endl;
        file << "  Checkpoint Saves: " << metrics.checkpointSaves << std::endl;
        file << "  Checkpoint Loads: " << metrics.checkpointLoads << std::endl;
        file << "  Total CPU Time: " << metrics.totalCpuTime << " seconds" << std::endl;
        file << "  Idle CPU Time: " << metrics.idleCpuTime << " seconds" << std::endl;
        file << std::endl;
        
        // Bottleneck analysis
        auto bottleneck = analyzeBottlenecks();
        file << "Bottleneck Analysis:" << std::endl;
        if (!bottleneck.type.empty()) {
            file << "  Type: " << bottleneck.type << std::endl;
            file << "  Severity: " << bottleneck.severity << std::endl;
            file << "  Location: " << bottleneck.location << std::endl;
            file << "  Recommendation: " << bottleneck.recommendation << std::endl;
        } else {
            file << "  No bottlenecks detected" << std::endl;
        }
        file << std::endl;
        
        // Performance score
        double score = calculatePerformanceScore();
        file << "Performance Score: " << score << "/100" << std::endl;
        file << "  Target: 100/100 (lower time per step = higher score)" << std::endl;
        file << std::endl;
        
        // Regression analysis
        auto regressions = analyzePerformanceRegression();
        if (!regressions.empty()) {
            file << "Performance Regressions:" << std::endl;
            for (const auto& reg : regressions) {
                file << "  Metric: " << reg.metric << std::endl;
                file << "    Baseline: " << reg.baselineValue << std::endl;
                file << "    Current: " << reg.currentValue << std::endl;
                file << "    Regression: " << reg.regressionPercent << "% (" << reg.severity << ")" << std::endl;
                file << "    Recommendation: " << reg.recommendation << std::endl;
            }
        } else {
            file << "Performance Regressions: None detected" << std::endl;
        }
        file << std::endl;
        
        file.close();
        NLM_LOG_INFO("Enhanced PerformanceProfiler: Exported analysis report to " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Enhanced PerformanceProfiler: Failed to export analysis report: " + std::string(e.what()));
        return false;
    }
}

void PerformanceProfiler::enableRealTimeMonitoring(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    if (enable && !pImpl->realTimeRunning) {
        pImpl->realTimeMonitoring = true;
        pImpl->realTimeRunning = true;
        pImpl->realTimeThread = std::thread(&Impl::realTimeMonitoringLoop, pImpl.get());
        NLM_LOG_INFO("Enhanced PerformanceProfiler: Real-time monitoring enabled");
    } else if (!enable && pImpl->realTimeRunning) {
        pImpl->realTimeMonitoring = false;
        pImpl->realTimeRunning = false;
        if (pImpl->realTimeThread.joinable()) {
            pImpl->realTimeThread.join();
        }
        NLM_LOG_INFO("Enhanced PerformanceProfiler: Real-time monitoring disabled");
    }
}

bool PerformanceProfiler::isRealTimeMonitoringEnabled() const {
    return pImpl->realTimeMonitoring;
}

void PerformanceProfiler::startBenchmark(const std::string& benchmarkName) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    pImpl->benchmarking = true;
    pImpl->currentBenchmarkName = benchmarkName;
    pImpl->benchmarkStartTime = std::chrono::steady_clock::now();
    
    NLM_LOG_INFO("Enhanced PerformanceProfiler: Started benchmark " + benchmarkName);
}

void PerformanceProfiler::endBenchmark() {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    if (pImpl->benchmarking) {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(endTime - pImpl->benchmarkStartTime).count();
        
        NLM_LOG_INFO("Enhanced PerformanceProfiler: Ended benchmark " + pImpl->currentBenchmarkName + ". Duration: " + std::to_string(duration) + " seconds");
        
        pImpl->benchmarking = false;
        pImpl->currentBenchmarkName.clear();
    }
}

bool PerformanceProfiler::isBenchmarking() const {
    return pImpl->benchmarking;
}

std::string PerformanceProfiler::getCurrentBenchmarkName() const {
    return pImpl->currentBenchmarkName;
}

std::vector<float> PerformanceProfiler::analyzeHeatmapRegion(int regionId) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::vector<float> regionData;
    
    for (const auto& heatmap : pImpl->activityHeatmaps) {
        if (regionId < heatmap.regionCount) {
            size_t startIdx = regionId * heatmap.neuronsPerRegion;
            size_t endIdx = std::min(startIdx + heatmap.neuronsPerRegion, heatmap.activityData.size());
            
            for (size_t i = startIdx; i < endIdx; ++i) {
                regionData.push_back(heatmap.activityData[i]);
            }
        }
    }
    
    return regionData;
}

double PerformanceProfiler::calculateHeatmapActivityScore(int regionId) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    
    std::vector<float> regionData = analyzeHeatmapRegion(regionId);
    
    if (regionData.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(regionData.begin(), regionData.end(), 0.0f);
    return sum / regionData.size();
}

bool PerformanceProfiler::saveSession(const std::string& sessionName) {
    try {
        std::filesystem::create_directories(outputDir + "/sessions");
        std::string sessionDir = outputDir + "/sessions/" + sessionName;
        std::filesystem::create_directories(sessionDir);
        
        // Export metrics
        std::string metricsFile = sessionDir + "/metrics.csv";
        if (!exportMetrics(metricsFile)) {
            return false;
        }
        
        // Export heatmaps
        std::string heatmapsFile = sessionDir + "/heatmaps.csv";
        if (!exportHeatmaps(heatmapsFile)) {
            return false;
        }
        
        // Export time series
        std::string timeSeriesFile = sessionDir + "/time_series.csv";
        if (!exportTimeSeries(timeSeriesFile)) {
            return false;
        }
        
        // Export analysis report
        std::string reportFile = sessionDir + "/analysis_report.txt";
        if (!exportAnalysisReport(reportFile)) {
            return false;
        }
        
        NLM_LOG_INFO("Enhanced PerformanceProfiler: Saved session " + sessionName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Enhanced PerformanceProfiler: Failed to save session: " + std::string(e.what()));
        return false;
    }
}

bool PerformanceProfiler::loadSession(const std::string& sessionName) {
    // Note: This is a simplified implementation
    // A full implementation would restore from saved session files
    std::string sessionDir = outputDir + "/sessions/" + sessionName;
    
    if (!std::filesystem::exists(sessionDir)) {
        NLM_LOG_ERROR("Enhanced PerformanceProfiler: Session not found: " + sessionName);
        return false;
    }
    
    // In a complete implementation, this would load session data
    // For now, just log that session loading is not fully implemented
    NLM_LOG_INFO("Enhanced PerformanceProfiler: Session loading is partially implemented for " + sessionName);
    return true;
}

std::vector<std::string> PerformanceProfiler::listSessions() const {
    std::vector<std::string> sessions;
    
    if (!std::filesystem::exists(outputDir + "/sessions")) {
        return sessions;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(outputDir + "/sessions")) {
        if (entry.is_directory()) {
            sessions.push_back(entry.path().filename().string());
        }
    }
    
    return sessions;
}

bool PerformanceProfiler::deleteSession(const std::string& sessionName) {
    std::string sessionDir = outputDir + "/sessions/" + sessionName;
    
    try {
        if (std::filesystem::exists(sessionDir)) {
            std::filesystem::remove_all(sessionDir);
            NLM_LOG_INFO("Enhanced PerformanceProfiler: Deleted session " + sessionName);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Enhanced PerformanceProfiler: Failed to delete session: " + std::string(e.what()));
    }
    
    return false;
}

} // namespace nlm
