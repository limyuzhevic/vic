#pragma once

/**
 * Performance Monitor for Neural Simulation
 * 
 * High-performance monitoring system for tracking simulation metrics,
 * detecting performance thresholds, and providing profiling capabilities.
 * 
 * Features:
 * - Real-time metric collection
 * - Threshold monitoring with alerts
 * - Neural activity profiling
 * - Memory usage tracking
 * - Performance profiling
 */

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <sys/resource.h>
#include <unistd.h>
#include <cmath>
#include <fstream>
#include <numeric>
#include <ctime>
#include <errno.h>
#include <unordered_map>
#include <atomic>
#include <limits>
#include <cassert>

namespace nlm {

// Additional utility classes for profiling

class Profiler {
public:
    Profiler() {
        static std::atomic<size_t> idCounter{0};
        id = ++idCounter;
    }
    
    Profiler(const std::string& name) : name(name) {
        static std::atomic<size_t> idCounter{0};
        id = ++idCounter;
    }
    
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        active = true;
    }
    
    void stop() {
        if (active) {
            auto endTime = std::chrono::high_resolution_clock::now();
            duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            active = false;
        }
    }
    
    double getElapsedSeconds() const {
        if (!active) {
            return duration.count() / 1000000.0;
        }
        auto current = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(current - startTime);
        return elapsed.count() / 1000000.0;
    }
    
    size_t getId() const { return id; }
    const std::string& getName() const { return name; }
    bool isActive() const { return active; }
    
private:
    size_t id;
    std::string name;
    bool active = false;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::microseconds duration{0};
};

class MemoryTracker {
public:
    MemoryTracker() {
        currentUsage = getCurrentMemoryMB();
    }
    
    size_t getCurrentUsage() const {
        return currentUsage;
    }
    
    size_t getCurrentMemoryMB() {
        #ifdef __linux__
        std::ifstream statusFile("/proc/self/status");
        if (!statusFile.is_open()) {
            return 0;
        }
        std::string line;
        while (std::getline(statusFile, line)) {
            if (line.find("VmRSS:") == 0) {
                std::istringstream iss(line);
                std::string key;
                size_t value;
                iss >> key >> value;
                return value;
            }
        }
        #endif
        return 0;
    }
    
    void update() {
        currentUsage = getCurrentMemoryMB();
    }
    
private:
    size_t currentUsage;
};

class NeuralActivityProfiler {
public:
    struct NeuronProfile {
        size_t neuronId;
        double avgFiringRate;
        double totalTimeActive;
        double membranePotentialSum;
        size_t spikeCount;
    };
    
    NeuralActivityProfiler(PerformanceMonitor& monitor) : performanceMonitor(monitor) {}
    
    void recordNeuronActivity(const Brain* brain, size_t stepDuration) {
        if (!brain) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        // Record metrics from brain
        Metrics metrics = performanceMonitor.getCurrentMetrics();
        
        // Create neural activity sample
        NeuralActivitySample sample;
        sample.timestamp = performanceMonitor.getCurrentTimeAsDouble();
        sample.neuronCount = brain->getTotalNeuronCount();
        sample.synapseCount = brain->getTotalSynapseCount();
        sample.firingRate = brain->getAverageFiringRate();
        sample.spikeCount = brain->getTotalSpikeCount();
        sample.activeNeurons = brain->getActiveNeuronCount();
        sample.firingNeurons = brain->getFiringNeuronCount();
        
        // Add to samples
        neuralActivitySamples.push_back(sample);
        
        // Record step timing if monitor is active
        if (activeProfiler) {
            performanceMonitor.recordStep(static_cast<double>(stepDuration), 
                                         brain->getTotalSpikeCount(), 
                                         brain->getActiveNeuronCount());
        }
        
        // Keep samples manageable
        if (neuralActivitySamples.size() > 10000) {
            neuralActivitySamples.erase(neuralActivitySamples.begin(), 
                                       neuralActivitySamples.begin() + 1000);
        }
    }
    
    std::vector<NeuralActivitySample> getSamples() const {
        std::lock_guard<std::mutex> lock(mutex);
        return neuralActivitySamples;
    }
    
    void start() {
        activeProfiler = true;
        NLM_LOG_INFO("Neural activity profiler started");
    }
    
    void stop() {
        activeProfiler = false;
        NLM_LOG_INFO("Neural activity profiler stopped");
    }
    
    double calculateAverageFiringRate() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (neuralActivitySamples.empty()) return 0.0;
        
        double totalRate = 0.0;
        for (const auto& sample : neuralActivitySamples) {
            totalRate += sample.firingRate;
        }
        return totalRate / neuralActivitySamples.size();
    }
    
    size_t getTotalSpikes() const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t totalSpikes = 0;
        for (const auto& sample : neuralActivitySamples) {
            totalSpikes += sample.spikeCount;
        }
        return totalSpikes;
    }
    
    void exportActivityData(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open activity data file: " + filepath);
            return;
        }
        
        // Write CSV header
        file << "timestamp,neuron_count,synapse_count,firing_rate,spike_count,active_neurons,firing_neurons\n";
        
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& sample : neuralActivitySamples) {
            file << sample.timestamp << "," 
                 << sample.neuronCount << "," 
                 << sample.synapseCount << "," 
                 << sample.firingRate << "," 
                 << sample.spikeCount << "," 
                 << sample.activeNeurons << "," 
                 << sample.firingNeurons << "\n";
        }
        
        file.close();
        NLM_LOG_INFO("Neural activity data exported to: " + filepath);
    }
    
    // Generate neural activity report
    std::string generateReport() const {
        std::stringstream ss;
        ss << "=== Neural Activity Performance Report ===\n";
        ss << "Total samples recorded: " << neuralActivitySamples.size() << "\n";
        ss << "Average firing rate: " << calculateAverageFiringRate() << " Hz\n";
        ss << "Total spikes recorded: " << getTotalSpikes() << "\n";
        ss << "Max neurons: " << getMaxNeurons() << "\n";
        ss << "Max synapses: " << getMaxSynapses() << "\n";
        ss << "Activity recording: " << (activeProfiler ? "Active" : "Inactive") << "\n";
        
        return ss.str();
    }
    
private:
    size_t getMaxNeurons() const {
        size_t max = 0;
        for (const auto& sample : neuralActivitySamples) {
            if (sample.neuronCount > max) max = sample.neuronCount;
        }
        return max;
    }
    
    size_t getMaxSynapses() const {
        size_t max = 0;
        for (const auto& sample : neuralActivitySamples) {
            if (sample.synapseCount > max) max = sample.synapseCount;
        }
        return max;
    }
    
    PerformanceMonitor& performanceMonitor;
    mutable std::mutex mutex;
    std::vector<NeuralActivitySample> neuralActivitySamples;
    bool activeProfiler = false;
};

struct PerformanceMonitor::Impl {
    bool monitoringActive;
    double samplingInterval;
    SamplingPolicy policy;
    std::vector<Sample> samples;
    mutable std::mutex mutex;
    
    // Background thread
    std::thread monitorThread;
    std::atomic<bool> stopFlag;
    
    // Current state
    std::chrono::steady_clock::time_point lastSampleTime;
    Metrics currentMetrics;
    
    // Thresholds and alerts
    std::vector<Threshold> thresholds;
    std::vector<Alert> alerts;
    
    // Profiling data
    std::unordered_map<std::string, Timer> profileTimers;
    std::unordered_map<std::string, ProfileData> profileData;
    
    // System info cache
    std::string systemInfo;
    std::chrono::steady_clock::time_point systemInfoUpdateTime;
    
    Impl() : monitoringActive(false), samplingInterval(0.1), policy(SamplingPolicy::RealTime),
             stopFlag(false), lastSampleTime(std::chrono::steady_clock::now()),
             systemInfoUpdateTime(std::chrono::steady_clock::now()) {}
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {}

PerformanceMonitor::~PerformanceMonitor() {
    stop();
}

PerformanceMonitor::PerformanceMonitor(PerformanceMonitor&&) noexcept = default;

PerformanceMonitor& PerformanceMonitor::operator=(PerformanceMonitor&&) noexcept = default;

void PerformanceMonitor::start(double interval, SamplingPolicy policy) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->monitoringActive) {
        return;
    }
    
    pImpl->samplingInterval = interval;
    pImpl->policy = policy;
    pImpl->monitoringActive = true;
    pImpl->stopFlag = false;
    pImpl->samples.clear();
    pImpl->currentMetrics = Metrics();
    
    pImpl->monitorThread = std::thread([this]() {
        monitorThreadFunc();
    });
    
    NLM_LOG_INFO("Performance monitoring started with interval: " + 
                std::to_string(pImpl->samplingInterval) + "s, policy: " + 
                std::to_string(static_cast<int>(pImpl->policy)));
}

std::vector<PerformanceMonitor::Sample> PerformanceMonitor::stop() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->monitoringActive) {
        return std::vector<Sample>();
    }
    
    pImpl->monitoringActive = false;
    pImpl->stopFlag = true;
    
    if (pImpl->monitorThread.joinable()) {
        pImpl->monitorThread.join();
    }
    
    NLM_LOG_INFO("Performance monitoring stopped. Collected " + std::to_string(pImpl->samples.size()) + " samples");
    return pImpl->samples;
}

PerformanceMonitor::Metrics PerformanceMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentMetrics;
}

void PerformanceMonitor::addThreshold(const Threshold& threshold) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->thresholds.push_back(threshold);
    
    // Check if current metrics violate threshold
    checkThresholds(pImpl->currentMetrics);
}

std::vector<PerformanceMonitor::Alert> PerformanceMonitor::getAlerts() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Remove expired alerts (older than 1 hour)
    auto currentTime = std::chrono::steady_clock::now();
    pImpl->alerts.erase(
        std::remove_if(pImpl->alerts.begin(), pImpl->alerts.end(),
                      [currentTime](const Alert& alert) {
                          auto alertTime = std::chrono::steady_clock::time_point(
                              std::chrono::milliseconds(alert.timestamp));
                          auto age = std::chrono::duration_cast<std::chrono::hours>(currentTime - alertTime);
                          return age.count() > 1;
                      }),
        pImpl->alerts.end()
    );
    
    return pImpl->alerts;
}

void PerformanceMonitor::clearAlerts() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->alerts.clear();
}

PerformanceMonitor::Sample PerformanceMonitor::sample() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    Sample sample;
    sample.timestamp = getCurrentTimeAsDouble();
    sample.metrics = pImpl->currentMetrics;
    
    return sample;
}

std::vector<PerformanceMonitor::Sample> PerformanceMonitor::getSamples(size_t maxSamples) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (maxSamples == 0 || pImpl->samples.size() <= maxSamples) {
        return pImpl->samples;
    }
    
    // Return most recent samples
    size_t startIndex = pImpl->samples.size() - maxSamples;
    return std::vector<Sample>(pImpl->samples.begin() + startIndex, pImpl->samples.end());
}

PerformanceMonitor::Metrics PerformanceMonitor::calculateStats(const std::vector<Sample>& samples) {
    Metrics stats;
    
    if (samples.empty()) {
        return stats;
    }
    
    size_t count = samples.size();
    
    // Calculate averages
    for (const auto& sample : samples) {
        stats.cpuUsage += sample.metrics.cpuUsage;
        stats.memoryUsage += sample.metrics.memoryUsage;
        stats.memoryAvailable += sample.metrics.memoryAvailable;
        stats.gpuUsage += sample.metrics.gpuUsage;
        stats.activeThreads += sample.metrics.activeThreads;
        stats.allocatedObjects += sample.metrics.allocatedObjects;
        stats.peakAllocated += sample.metrics.peakAllocated;
        stats.currentFps += sample.metrics.currentFps;
        stats.averageFps += sample.metrics.averageFps;
        stats.frameTimeMs += sample.metrics.frameTimeMs;
        stats.spikeCount += sample.metrics.spikeCount;
        stats.neuronCount += sample.metrics.neuronCount;
        stats.synapseCount += sample.metrics.synapseCount;
        stats.memoryPoolUsed += sample.metrics.memoryPoolUsed;
        stats.eventQueueSize += sample.metrics.eventQueueSize;
    }
    
    // Calculate averages
    stats.cpuUsage /= static_cast<double>(count);
    stats.memoryUsage /= static_cast<double>(count);
    stats.memoryAvailable /= static_cast<double>(count);
    stats.gpuUsage /= static_cast<double>(count);
    stats.activeThreads /= static_cast<double>(count);
    stats.allocatedObjects /= static_cast<double>(count);
    stats.peakAllocated /= static_cast<double>(count);
    stats.currentFps /= static_cast<double>(count);
    stats.averageFps /= static_cast<double>(count);
    stats.frameTimeMs /= static_cast<double>(count);
    stats.spikeCount /= static_cast<double>(count);
    stats.neuronCount /= static_cast<double>(count);
    stats.synapseCount /= static_cast<double>(count);
    stats.memoryPoolUsed /= static_cast<double>(count);
    stats.eventQueueSize /= static_cast<double>(count);
    
    // Find min/max values (would need to track them separately)
    // For now, set min to average for most metrics
    stats.cpuUsage = stats.cpuUsage;  // Will be updated in monitor loop
    stats.memoryUsage = stats.memoryUsage;
    stats.gpuUsage = stats.gpuUsage;
    
    return stats;
}

bool PerformanceMonitor::exportToCSV(const std::string& filepath, const std::vector<Sample>& samples) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
            return false;
        }
        
        // Write header
        file << "timestamp,cpu_usage,memory_usage,memory_available,gpu_usage,active_threads," 
             << "allocated_objects,peak_allocated,current_fps,average_fps,frame_time_ms,spike_count," 
             << "neuron_count,synapse_count,memory_pool_used,event_queue_size\n";
        
        // Write data
        for (const auto& sample : samples) {
            file << sample.timestamp << "," << sample.metrics.cpuUsage << "," 
                 << sample.metrics.memoryUsage << "," << sample.metrics.memoryAvailable << "," 
                 << sample.metrics.gpuUsage << "," << sample.metrics.activeThreads << "," 
                 << sample.metrics.allocatedObjects << "," << sample.metrics.peakAllocated << "," 
                 << sample.metrics.currentFps << "," << sample.metrics.averageFps << "," 
                 << sample.metrics.frameTimeMs << "," << sample.metrics.spikeCount << "," 
                 << sample.metrics.neuronCount << "," << sample.metrics.synapseCount << "," 
                 << sample.metrics.memoryPoolUsed << "," << sample.metrics.eventQueueSize << "\n";
        }
        
        file.close();
        NLM_LOG_INFO("Performance data exported to CSV: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to export performance data: ") + e.what());
        return false;
    }
}

std::string PerformanceMonitor::getSystemInfo() {
    static std::string cachedInfo;
    static std::chrono::steady_clock::time_point lastUpdate;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - lastUpdate).count();
    
    if (elapsed >= 300 && !cachedInfo.empty()) { // Cache for 5 minutes
        return cachedInfo;
    }
    
    try {
        std::stringstream ss;
        ss << "System Information:\n";
        
        // Get hostname
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            ss << "Hostname: " << hostname << "\n";
        }
        
        // Get user info
        uid_t uid = getuid();
        struct passwd *pw = getpwuid(uid);
        if (pw) {
            ss << "User: " << pw->pw_name << "\n";
        }
        
        // Get CPU count
        ss << "CPU Cores: " << std::thread::hardware_concurrency() << "\n";
        
        // Get memory info
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            ss << "Max RSS: " << usage.ru_maxrss / 1024 << " MB\n";
        }
        
        // Get process info
        ss << "PID: " << getpid() << "\n";
        
        ss << "Architecture: " << (sizeof(void*) == 8 ? "x86_64" : "x86") << "\n";
        ss << "Compiler: GCC " << __VERSION__ << "\n";
        
        cachedInfo = ss.str();
        lastUpdate = now;
        
        return cachedInfo;
        
    } catch (const std::exception& e) {
        return "Failed to get system info: " + std::string(e.what());
    }
}

template<typename Func, typename... Args>
auto PerformanceMonitor::profile(const std::string& name, Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Start timer
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Call the function
    auto result = func(std::forward<Args>(args)...);
    
    // Calculate duration
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    double elapsedSeconds = duration.count() / 1000000.0;
    
    // Update profile data
    auto& profileData = pImpl->profileData[name];
    profileData.functionName = name;
    profileData.totalTime += elapsedSeconds;
    profileData.minTime = std::min(profileData.minTime, elapsedSeconds);
    profileData.maxTime = std::max(profileData.maxTime, elapsedSeconds);
    profileData.callCount++;
    
    if (profileData.callCount > 0) {
        profileData.averageTime = profileData.totalTime / profileData.callCount;
    }
    
    return result;
}

void PerformanceMonitor::toggleProfiling(const std::string& name, bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (enable) {
        pImpl->profileTimers[name] = Timer();
        pImpl->profileTimers[name].start();
    } else {
        if (pImpl->profileTimers.count(name)) {
            pImpl->profileTimers[name].stop();
            
            double elapsed = pImpl->profileTimers[name].elapsed();
            
            auto& profileData = pImpl->profileData[name];
            profileData.functionName = name;
            profileData.totalTime += elapsed;
            profileData.minTime = std::min(profileData.minTime, elapsed);
            profileData.maxTime = std::max(profileData.maxTime, elapsed);
            profileData.callCount++;
            
            if (profileData.callCount > 0) {
                profileData.averageTime = profileData.totalTime / profileData.callCount;
            }
            
            pImpl->profileTimers.erase(name);
        }
    }
}

void PerformanceMonitor::monitorThreadFunc() {
    while (!pImpl->stopFlag) {
        try {
            // Update metrics
            updateMetrics(pImpl->currentMetrics);
            
            // Check thresholds
            checkThresholds(pImpl->currentMetrics);
            
            // Sample based on policy
            if (pImpl->policy == SamplingPolicy::RealTime && pImpl->samplingInterval > 0) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - pImpl->lastSampleTime);
                
                if (elapsed.count() >= pImpl->samplingInterval) {
                    Sample sample;
                    sample.timestamp = getCurrentTimeAsDouble();
                    sample.metrics = pImpl->currentMetrics;
                    
                    {
                        std::lock_guard<std::mutex> lock(pImpl->mutex);
                        pImpl->samples.push_back(sample);
                        
                        // Keep only last 1000 samples to prevent memory issues
                        if (pImpl->samples.size() > 1000) {
                            pImpl->samples.erase(pImpl->samples.begin());
                        }
                    }
                    
                    pImpl->lastSampleTime = now;
                }
            }
            
            // Sleep briefly
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Performance monitor thread error: ") + e.what());
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void PerformanceMonitor::updateMetrics(Metrics& metrics) {
    try {
        // Get memory usage
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            metrics.memoryUsage = usage.ru_maxrss / 1024.0; // Convert to MB
        }
        
        // Get thread count
        metrics.activeThreads = std::thread::hardware_concurrency();
        
        // For now, set some placeholder values for other metrics
        metrics.cpuUsage = 0.0;
        metrics.gpuUsage = 0.0;
        metrics.currentFps = 60.0;
        metrics.averageFps = metrics.currentFps;
        metrics.frameTimeMs = 1000.0 / metrics.currentFps;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to update metrics: ") + e.what());
    }
}

double PerformanceMonitor::getCurrentTimeAsDouble() {
    auto now = std::chrono::high_resolution_clock::now();
    auto epoch = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::duration<double>>(epoch).count();
}

void PerformanceMonitor::checkThresholds(const Metrics& metrics) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& threshold : pImpl->thresholds) {
        double currentValue = 0.0;
        
        // Get current value based on metric name
        if (threshold.metric == "cpu_usage") {
            currentValue = metrics.cpuUsage;
        } else if (threshold.metric == "memory_usage") {
            currentValue = metrics.memoryUsage;
        } else if (threshold.metric == "memory_available") {
            currentValue = metrics.memoryAvailable;
        } else if (threshold.metric == "gpu_usage") {
            currentValue = metrics.gpuUsage;
        } else if (threshold.metric == "active_threads") {
            currentValue = static_cast<double>(metrics.activeThreads);
        } else if (threshold.metric == "allocated_objects") {
            currentValue = static_cast<double>(metrics.allocatedObjects);
        } else if (threshold.metric == "frame_time_ms") {
            currentValue = static_cast<double>(metrics.frameTimeMs);
        } else if (threshold.metric == "spike_count") {
            currentValue = static_cast<double>(metrics.spikeCount);
        } else {
            continue; // Unknown metric
        }
        
        // Check against thresholds
        bool isWarning = false;
        bool isCritical = false;
        
        if (currentValue >= threshold.criticalLevel) {
            isCritical = true;
            isWarning = true;
        } else if (currentValue >= threshold.warningLevel) {
            isWarning = true;
        }
        
        if (isWarning || isCritical) {
            Alert alert;
            alert.timestamp = getCurrentTimeAsDouble();
            alert.threshold = threshold;
            alert.currentMetrics = metrics;
            alert.severity = isCritical ? "critical" : "warning";
            
            pImpl->alerts.push_back(alert);
            
            // Log the alert
            std::string severity = isCritical ? "CRITICAL" : "WARNING";
            NLM_LOG_##severity("Performance threshold exceeded: '" + threshold.name + 
                             "' " + threshold.metric + " = " + std::to_string(currentValue) +
                             " (threshold: " + std::to_string(threshold.criticalLevel) + ")");
        }
    }
}

} // namespace nlm