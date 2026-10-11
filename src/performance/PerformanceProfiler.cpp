#include "PerformanceProfiler.hpp"
#include "../brain/Brain.hpp"
#include "../debugging/DebugConsole.hpp"
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

struct PerformanceProfiler::Impl {
    PerformanceProfilerConfig config;
    const Brain* brain;
    DebugConsole* debugConsole;
    
    // State
    bool active;
    bool collecting;
    std::thread profileThread;
    std::mutex dataMutex;
    std::atomic<bool> running;
    
    // Metrics collection
    std::vector<PerformanceMetrics> metricsHistory;
    PerformanceMetrics currentMetrics;
    
    // Performance alerts
    std::vector<PerformanceAlert> alerts;
    
    // Timing
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::duration<double> totalProfileTime;
    
    // Progress reporting
    std::function<void(double, const std::string&)> progressCallback;
    
    // System monitoring
    std::chrono::steady_clock::time_point lastSystemCheckTime;
    double lastSystemCpuUsage;
    size_t lastSystemMemoryUsage;
    
    // Performance analysis cache
    mutable PerformanceAnalysis cachedAnalysis;
    mutable bool analysisDirty;
    
    Impl(const PerformanceProfilerConfig& cfg) : config(cfg), brain(nullptr), debugConsole(nullptr),
                                                 active(false), collecting(false), running(false),
                                                 lastSystemCpuUsage(0.0), lastSystemMemoryUsage(0),
                                                 analysisDirty(true) {
        // Create report directory
        if (!config.reportDirectory.empty()) {
            std::filesystem::create_directories(config.reportDirectory);
        }
    }
    
    ~Impl() {
        shutdown();
    }
    
    void shutdown() {
        running = false;
        collecting = false;
        
        if (profileThread.joinable()) {
            profileThread.join();
        }
        
        progressCallback = nullptr;
    }
    
    void startCollecting() {
        if (collecting) return;
        
        collecting = true;
        running = true;
        startTime = std::chrono::steady_clock::now();
        lastSystemCheckTime = startTime;
        lastSystemCpuUsage = 0.0;
        lastSystemMemoryUsage = 0;
        
        profileThread = std::thread(&PerformanceProfiler::Impl::profileLoop, this);
        
        NLM_LOG_INFO("PerformanceProfiler: Started data collection");
    }
    
    void stopCollecting() {
        if (!collecting) return;
        
        collecting = false;
        running = false;
        
        if (profileThread.joinable()) {
            profileThread.join();
        }
        
        totalProfileTime = std::chrono::steady_clock::now() - startTime;
        
        NLM_LOG_INFO("PerformanceProfiler: Stopped data collection");
        
        // Generate analysis
        analyzeAndCache();
    }
    
    void profileLoop() {
        double lastTime = 0.0;
        
        while (running) {
            double currentTime = std::chrono::duration<double>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            
            double deltaTime = currentTime - lastTime;
            
            if (deltaTime >= config.profileInterval) {
                updateMetrics(deltaTime);
                lastTime = currentTime;
            }
            
            // Check system resources
            checkSystemResources();
            
            // Sleep briefly
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void updateMetrics(double deltaTime) {
        if (!brain) return;
        
        std::lock_guard<std::mutex> lock(dataMutex);
        
        auto now = std::chrono::steady_clock::now();
        currentMetrics.timestamp = now;
        
        // Update timing metrics
        currentMetrics.timing.lastFrameTime = deltaTime;
        
        // Update simulation metrics
        currentMetrics.simulation.currentStep = brain->getCurrentStep();
        currentMetrics.simulation.currentTime = brain->getCurrentTime();
        currentMetrics.simulation.totalSteps = currentMetrics.simulation.currentStep;
        currentMetrics.simulation.simulationDuration = totalProfileTime.count();
        currentMetrics.simulation.stepsPerSecond = config.profileInterval > 0 ? 
                                                  1.0 / config.profileInterval : 0.0;
        currentMetrics.simulation.totalNeurons = brain->getTotalNeuronCount();
        currentMetrics.simulation.totalSynapses = brain->getTotalSynapseCount();
        currentMetrics.simulation.activeNeurons = brain->getActiveNeuronCount();
        currentMetrics.simulation.firingNeurons = brain->getFiringNeuronCount();
        currentMetrics.simulation.averageFiringRate = brain->getAverageFiringRate();
        currentMetrics.simulation.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        
        // Update neural computation metrics
        currentMetrics.neural.neuronsProcessed = currentMetrics.simulation.totalNeurons;
        currentMetrics.neural.synapsesProcessed = currentMetrics.simulation.totalSynapses;
        currentMetrics.neural.spikesDetected = brain->getTotalSpikeCount();
        currentMetrics.neural.averageNeuronFiringRate = currentMetrics.simulation.averageFiringRate;
        
        // Calculate statistics
        calculateTimingStats(deltaTime);
        calculateMemoryStats();
        calculateCPUMetrics(deltaTime);
        calculateNeuralStats();
        calculateSystemStats();
        
        // Add to history
        metricsHistory.push_back(currentMetrics);
        
        // Limit history size
        if (metricsHistory.size() > config.profileSampleSize) {
            metricsHistory.erase(metricsHistory.begin());
        }
        
        // Check for anomalies
        checkAnomalies();
        
        // Report progress
        if (config.enableProgressReporting && progressCallback) {
            double progress = static_cast<double>(metricsHistory.size()) / config.profileSampleSize;
            progressCallback(progress, "Profiling metrics updated");
        }
    }
    
    void calculateTimingStats(double deltaTime) {
        auto& timing = currentMetrics.timing;
        
        if (metricsHistory.size() > 1) {
            // Calculate average frame time
            double sumFrameTimes = 0.0;
            double minFrameTime = 1e9;
            double maxFrameTime = 0.0;
            
            for (const auto& metrics : metricsHistory) {
                sumFrameTimes += metrics.timing.lastFrameTime;
                minFrameTime = std::min(minFrameTime, metrics.timing.lastFrameTime);
                maxFrameTime = std::max(maxFrameTime, metrics.timing.lastFrameTime);
            }
            
            timing.averageFrameTime = sumFrameTimes / metricsHistory.size();
            timing.minFrameTime = minFrameTime;
            timing.maxFrameTime = maxFrameTime;
            
            timing.fps = (timing.averageFrameTime > 0.0) ? 1.0 / timing.averageFrameTime : 0.0;
            timing.simulationHz = (timing.averageFrameTime > 0.0) ? 1.0 / timing.averageFrameTime : 0.0;
        }
        
        // Update total simulation time
        timing.totalSimulationTime += deltaTime;
    }
    
    void calculateMemoryStats() {
        auto& memory = currentMetrics.memory;
        
        // TODO: Get actual memory usage from system
        // For now, estimate based on brain size
        memory.currentMemoryUsage = currentMetrics.simulation.totalNeurons * 1024 +  // ~1KB per neuron
                                   currentMetrics.simulation.totalSynapses * 512 +   // ~512 bytes per synapse
                                   10 * 1024 * 1024;                              // 10MB overhead
        
        memory.peakMemoryUsage = std::max(memory.peakMemoryUsage, memory.currentMemoryUsage);
        memory.totalMemoryUsage = memory.currentMemoryUsage;
        
        // Estimate memory growth
        memory.memoryGrowthRate = (memory.currentMemoryUsage - lastSystemMemoryUsage) / 
                                 (config.profileInterval > 0 ? config.profileInterval : 0.1);
        
        lastSystemMemoryUsage = memory.currentMemoryUsage;
    }
    
    void calculateCPUMetrics(double deltaTime) {
        auto& cpu = currentMetrics.cpu;
        
        // TODO: Get actual CPU usage from system
        // For now, use a simple model
        double baseCpuUsage = 5.0;  // Base CPU usage (5%)
        double neuralLoad = std::min(95.0, currentMetrics.simulation.totalNeurons / 1000.0 * 10.0);
        
        double currentCpuUsage = baseCpuUsage + neuralLoad;
        cpu.cpuUsageHistory.push_back(currentCpuUsage);
        
        if (cpu.cpuUsageHistory.size() > config.profileSampleSize) {
            cpu.cpuUsageHistory.erase(cpu.cpuUsageHistory.begin());
        }
        
        // Calculate statistics
        double sumCpuUsage = 0.0;
        double peakCpuUsage = 0.0;
        
        for (double usage : cpu.cpuUsageHistory) {
            sumCpuUsage += usage;
            peakCpuUsage = std::max(peakCpuUsage, usage);
        }
        
        cpu.averageCpuUsage = (cpu.cpuUsageHistory.empty()) ? 0.0 : sumCpuUsage / cpu.cpuUsageHistory.size();
        cpu.peakCpuUsage = peakCpuUsage;
        
        // TODO: Get actual thread count
        cpu.activeThreads = 4; // Placeholder
    }
    
    void calculateNeuralStats() {
        auto& neural = currentMetrics.neural;
        
        // Calculate standard deviation of firing rates
        if (metricsHistory.size() > 1) {
            double sumRates = 0.0;
            double sumSquares = 0.0;
            
            for (const auto& metrics : metricsHistory) {
                sumRates += metrics.neural.averageNeuronFiringRate;
                sumSquares += metrics.neural.averageNeuronFiringRate * metrics.neural.averageNeuronFiringRate;
            }
            
            neural.averageNeuronFiringRate = sumRates / metricsHistory.size();
            neural.stddevNeuronFiringRate = std::sqrt((sumSquares / metricsHistory.size()) - 
                                                     neural.averageNeuronFiringRate * neural.averageNeuronFiringRate);
        }
        
        // TODO: Get actual timing from brain
        neural.connectionUpdateTime = 0.001;  // Placeholder
        neural.plasticityUpdateTime = 0.002;  // Placeholder
        neural.memoryAccessTime = 0.0005;     // Placeholder
    }
    
    void calculateSystemStats() {
        auto& system = currentMetrics.system;
        
        // TODO: Get actual system stats
        system.cpuTemperature = 45.0 + std::rand() % 20;  // Random between 45-65
        system.gpuTemperature = 50.0 + std::rand() % 20;  // Random between 50-70
        system.systemTemperature = (system.cpuTemperature + system.gpuTemperature) / 2.0;
        
        // Estimate disk usage
        system.diskUsage = 1024 * 1024 * 1024;  // 1GB
        system.diskUsagePercent = 45.0;       // 45%
        
        // Estimate swap usage
        system.swapUsage = 512 * 1024 * 1024;  // 512MB
        system.swapUsagePercent = 10.0;       // 10%
        
        system.uptime = std::chrono::system_clock::now() - startTime;
    }
    
    void checkSystemResources() {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - lastSystemCheckTime).count();
        
        if (elapsed >= 1.0) {  // Check every second
            // TODO: Get actual system resources
            lastSystemCpuUsage = currentMetrics.cpu.averageCpuUsage;
            lastSystemMemoryUsage = currentMetrics.memory.currentMemoryUsage;
            lastSystemCheckTime = now;
        }
    }
    
    void checkAnomalies() {
        if (!config.enableAnomalyDetection) return;
        
        // Check memory usage
        if (currentMetrics.memory.currentMemoryUsage > config.maxMemoryUsage * 0.9) {
            addAlert(PerformanceAlert("memory", "warning",
                                     "Memory usage is high: " + std::to_string(currentMetrics.memory.currentMemoryUsage / (1024*1024)) + " MB",
                                     currentMetrics.memory.currentMemoryUsage, config.maxMemoryUsage * 0.9));
        }
        
        // Check CPU usage
        if (currentMetrics.cpu.averageCpuUsage > config.maxCpuUsage * 0.9) {
            addAlert(PerformanceAlert("cpu", "warning",
                                     "CPU usage is high: " + std::to_string(currentMetrics.cpu.averageCpuUsage) + "%",
                                     currentMetrics.cpu.averageCpuUsage, config.maxCpuUsage * 0.9));
        }
        
        // Check frame time (too slow)
        if (currentMetrics.timing.lastFrameTime > 0.1) {  // More than 100ms
            addAlert(PerformanceAlert("neural", "error",
                                     "Frame time is too slow: " + std::to_string(currentMetrics.timing.lastFrameTime * 1000) + " ms",
                                     currentMetrics.timing.lastFrameTime, 0.1));
        }
    }
    
    void addAlert(const PerformanceAlert& alert) {
        alerts.push_back(alert);
        
        // Log alert
        std::string level = alert.severity == "critical" ? "ERROR" : "WARNING";
        NLM_LOG_INFO(std::string("[PerformanceAlert] ") + alert.message);
        
        // Notify debug console if available
        if (debugConsole) {
            // TODO: Notify debug console
        }
    }
    
    void analyzeAndCache() const {
        if (metricsHistory.size() < 2) {
            cachedAnalysis.valid = false;
            return;
        }
        
        cachedAnalysis.valid = true;
        cachedAnalysis.summary = "Performance analysis based on " + std::to_string(metricsHistory.size()) + " samples";
        
        // Calculate performance score
        double score = 100.0;
        
        // Deduct for slow frames
        double avgFrameTime = 0.0;
        for (const auto& metrics : metricsHistory) {
            avgFrameTime += metrics.timing.averageFrameTime;
        }
        avgFrameTime /= metricsHistory.size();
        
        if (avgFrameTime > 0.1) {  // More than 100ms
            score -= 30.0 * (avgFrameTime - 0.1);
        }
        
        // Deduct for high memory usage
        double avgMemoryUsage = 0.0;
        for (const auto& metrics : metricsHistory) {
            avgMemoryUsage += metrics.memory.currentMemoryUsage;
        }
        avgMemoryUsage /= metricsHistory.size();
        
        if (avgMemoryUsage > config.maxMemoryUsage * 0.8) {
            score -= 20.0 * (avgMemoryUsage / config.maxMemoryUsage - 0.8);
        }
        
        // Deduct for high CPU usage
        double avgCpuUsage = 0.0;
        for (const auto& metrics : metricsHistory) {
            avgCpuUsage += metrics.cpu.averageCpuUsage;
        }
        avgCpuUsage /= metricsHistory.size();
        
        if (avgCpuUsage > config.maxCpuUsage * 0.8) {
            score -= 20.0 * (avgCpuUsage / config.maxCpuUsage - 0.8);
        }
        
        cachedAnalysis.performanceScore = std::max(0.0, score);
        
        // Detect bottlenecks
        double connectionTime = 0.0, plasticityTime = 0.0, memoryTime = 0.0;
        for (const auto& metrics : metricsHistory) {
            connectionTime += metrics.neural.connectionUpdateTime;
            plasticityTime += metrics.neural.plasticityUpdateTime;
            memoryTime += metrics.neural.memoryAccessTime;
        }
        
        connectionTime /= metricsHistory.size();
        plasticityTime /= metricsHistory.size();
        memoryTime /= metricsHistory.size();
        
        // Identify bottlenecks
        if (connectionTime > plasticityTime && connectionTime > memoryTime) {
            cachedAnalysis.bottlenecks.push_back(std::make_pair("connections", connectionTime));
            cachedAnalysis.suggestions.push_back("Optimize connection updates for better performance");
        } else if (plasticityTime > connectionTime && plasticityTime > memoryTime) {
            cachedAnalysis.bottlenecks.push_back(std::make_pair("plasticity", plasticityTime));
            cachedAnalysis.suggestions.push_back("Optimize plasticity updates for better performance");
        } else if (memoryTime > connectionTime && memoryTime > plasticityTime) {
            cachedAnalysis.bottlenecks.push_back(std::make_pair("memory", memoryTime));
            cachedAnalysis.suggestions.push_back("Optimize memory access for better performance");
        }
        
        // Calculate performance trend
        if (metricsHistory.size() >= 10) {
            double firstHalf = 0.0, secondHalf = 0.0;
            size_t halfSize = metricsHistory.size() / 2;
            
            for (size_t i = 0; i < halfSize; ++i) {
                firstHalf += metricsHistory[i].timing.averageFrameTime;
            }
            firstHalf /= halfSize;
            
            for (size_t i = halfSize; i < metricsHistory.size(); ++i) {
                secondHalf += metricsHistory[i].timing.averageFrameTime;
            }
            secondHalf /= (metricsHistory.size() - halfSize);
            
            cachedAnalysis.trendOverTime = (secondHalf - firstHalf) / firstHalf;
        }
        
        // Generate warnings
        if (cachedAnalysis.performanceScore < 50.0) {
            cachedAnalysis.warnings.push_back("Performance score is below 50. Consider optimization.");
        }
        
        if (cachedAnalysis.trendOverTime > 0.1) {
            cachedAnalysis.warnings.push_back("Performance is degrading over time.");
        } else if (cachedAnalysis.trendOverTime < -0.1) {
            cachedAnalysis.warnings.push_back("Performance is improving over time.");
        }
        
        analysisDirty = false;
    }
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>(PerformanceProfilerConfig())) {}

PerformanceProfiler::PerformanceProfiler(const PerformanceProfilerConfig& config) : pImpl(std::make_unique<Impl>(config)) {}

PerformanceProfiler::~PerformanceProfiler() = default;

bool PerformanceProfiler::initialize() {
    if (pImpl->active) return true;
    
    pImpl->active = true;
    
    NLM_LOG_INFO("PerformanceProfiler: Initialized successfully");
    return true;
}

void PerformanceProfiler::start() {
    pImpl->startCollecting();
}

void PerformanceProfiler::stop() {
    pImpl->stopCollecting();
}

bool PerformanceProfiler::isActive() const {
    return pImpl->active;
}

void PerformanceProfiler::update(const Brain* brain, double deltaTime) {
    if (!pImpl->collecting) return;
    
    pImpl->brain = brain;
    pImpl->updateMetrics(deltaTime);
}

PerformanceMetrics PerformanceProfiler::getCurrentMetrics() const {
    return pImpl->currentMetrics;
}

std::vector<PerformanceMetrics> PerformanceProfiler::getHistoricalMetrics(size_t count) const {
    std::vector<PerformanceMetrics> result;
    
    if (count > pImpl->metricsHistory.size()) {
        count = pImpl->metricsHistory.size();
    }
    
    if (count > 0) {
        result.insert(result.end(), pImpl->metricsHistory.end() - count, pImpl->metricsHistory.end());
    }
    
    return result;
}

std::vector<PerformanceMetrics> PerformanceProfiler::getRecentMetrics(double seconds) const {
    std::vector<PerformanceMetrics> result;
    
    double cutoffTime = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count() - seconds;
    
    for (const auto& metrics : pImpl->metricsHistory) {
        double metricsTime = std::chrono::duration<double>(
            metrics.timestamp.time_since_epoch()).count();
        
        if (metricsTime >= cutoffTime) {
            result.push_back(metrics);
        }
    }
    
    return result;
}

PerformanceAnalysis PerformanceProfiler::analyzePerformance(const std::vector<PerformanceMetrics>& metrics) const {
    // TODO: Implement performance analysis
    return PerformanceAnalysis();
}

PerformanceAnalysis PerformanceProfiler::getCurrentAnalysis() const {
    if (pImpl->analysisDirty) {
        pImpl->analyzeAndCache();
    }
    
    return pImpl->cachedAnalysis;
}

void PerformanceProfiler::addAlert(const PerformanceAlert& alert) {
    pImpl->addAlert(alert);
}

void PerformanceProfiler::clearAlerts() {
    pImpl->alerts.clear();
}

const std::vector<PerformanceAlert>& PerformanceProfiler::getAlerts() const {
    return pImpl->alerts;
}

void PerformanceProfiler::updateConfig(const PerformanceProfilerConfig& config) {
    pImpl->config = config;
}

const PerformanceProfilerConfig& PerformanceProfiler::getConfig() const {
    return pImpl->config;
}

void PerformanceProfiler::setProgressCallback(std::function<void(double, const std::string&)> callback) {
    pImpl->progressCallback = callback;
}

void PerformanceProfiler::clearProgressCallback() {
    pImpl->progressCallback = nullptr;
}

bool PerformanceProfiler::saveProfile(const std::string& filepath, const std::string& format) {
    if (filepath.empty()) {
        return false;
    }
    
    try {
        if (format == "json") {
            // TODO: Implement JSON export
            return false;
        } else if (format == "csv") {
            // TODO: Implement CSV export
            return false;
        }
        
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving profile: ") + e.what());
        return false;
    }
}

bool PerformanceProfiler::loadProfile(const std::string& filepath) {
    // TODO: Implement profile loading
    return false;
}

void PerformanceProfiler::clear() {
    pImpl->metricsHistory.clear();
    pImpl->currentMetrics = PerformanceMetrics();
    pImpl->alerts.clear();
    pImpl->analysisDirty = true;
}

size_t PerformanceProfiler::getSampleCount() const {
    return pImpl->metricsHistory.size();
}

double PerformanceProfiler::getProfileDuration() const {
    return pImpl->totalProfileTime.count();
}

std::vector<std::string> PerformanceProfiler::getOptimizationRecommendations() const {
    return pImpl->cachedAnalysis.suggestions;
}

std::vector<std::string> PerformanceProfiler::getBottleneckAnalysis() const {
    std::vector<std::string> result;
    for (const auto& bottleneck : pImpl->cachedAnalysis.bottlenecks) {
        result.push_back(std::string("Bottleneck: ") + bottleneck.first + 
                        std::string(" (contributes ") + std::to_string(bottleneck.second) + "s to computation)");
    }
    return result;
}

void PerformanceProfiler::setDebugConsole(DebugConsole* console) {
    pImpl->debugConsole = console;
}

bool PerformanceProfiler::isCollecting() const {
    return pImpl->collecting;
}

} // namespace nlm
