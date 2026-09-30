#pragma once

#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <map>
#include <string>
#include <sstream>

namespace nlm {

/**
 * Performance Monitoring System
 * 
 * Real-time performance monitoring and metrics collection for neural simulation:
 * - Step timing performance (min, max, average)
 * - Activity statistics (spike generation, firing rates)
 * - Memory usage tracking
 * - Plasticity operation counting
 * - Neuromodulation activity tracking
 * - Stability and learning efficiency metrics
 * - Thread-safe concurrent access
 */

class PerformanceMonitor {
public:
    /**
     * Performance metrics for each simulation step
     */
    struct StepMetrics {
        std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
        double stepTimeMs;             // Time for this step in milliseconds
        size_t spikesGenerated;        // Spikes generated in this step
        size_t activeNeurons;         // Active neurons in this step
        size_t memoryUsageBytes;       // Memory usage during this step
        size_t stdpUpdates;            // STDP updates in this step
        size_t hebbianUpdates;         // Hebbian updates in this step
        
        StepMetrics()
            : stepTimeMs(0.0), spikesGenerated(0), activeNeurons(0), memoryUsageBytes(0)
            , stdpUpdates(0), hebbianUpdates(0) {}
    };
    
    /**
     * System-wide performance statistics
     */
    struct SystemStats {
        // Timing statistics
        double totalSimTime;           // Total simulation time (ms)
        double avgStepTime;           // Average step time (ms)
        double stdStepTime;           // Standard deviation of step times (ms)
        double minStepTime;           // Minimum step time (ms)
        double maxStepTime;           // Maximum step time (ms)
        
        // Performance metrics
        size_t totalSteps;            // Total simulation steps
        double stepsPerSecond;        // Steps per second
        double simulationSpeed;       // Simulation speed relative to real time
        
        // Activity statistics
        size_t totalSpikesGenerated;  // Total spikes generated
        double avgSpikesPerStep;      // Average spikes per step
        size_t avgActiveNeurons;      // Average active neurons per step
        double avgFiringRate;         // Average firing rate (Hz)
        
        // Plasticity statistics
        size_t totalSTDPUpdates;      // Total STDP updates
        size_t totalHebbianUpdates;   // Total Hebbian updates
        size_t totalStructuralPlasticityEvents; // Total structural plasticity events
        
        // Neuromodulation statistics
        size_t totalDopamineUpdates;  // Total dopamine updates
        size_t totalCuriosityUpdates; // Total curiosity updates
        size_t totalNoveltyUpdates;   // Total novelty updates
        
        // Memory statistics
        size_t maxMemoryUsageBytes;   // Maximum memory usage
        double avgMemoryUsageBytes;   // Average memory usage
        
        // System health
        bool isStable;                // Network stability indicator
        double stabilityScore;        // Stability metric (0.0 to 1.0)
        double learningEfficiency;    // Learning efficiency metric (0.0 to 1.0)
        
        SystemStats()
            : totalSimTime(0.0), avgStepTime(0.0), stdStepTime(0.0), minStepTime(0.0), maxStepTime(0.0)
            , totalSteps(0), stepsPerSecond(0.0), simulationSpeed(1.0)
            , totalSpikesGenerated(0), avgSpikesPerStep(0.0), avgActiveNeurons(0), avgFiringRate(0.0)
            , totalSTDPUpdates(0), totalHebbianUpdates(0), totalStructuralPlasticityEvents(0)
            , totalDopamineUpdates(0), totalCuriosityUpdates(0), totalNoveltyUpdates(0)
            , maxMemoryUsageBytes(0), avgMemoryUsageBytes(0)
            , isStable(true), stabilityScore(0.0), learningEfficiency(0.0) {}
    };
    
    PerformanceMonitor()
        : totalSteps(0), isRunning(false), stopRequested(false)
        , threadPool(std::max(1u, std::thread::hardware_concurrency()))
    {
        startTime = std::chrono::high_resolution_clock::now();
        lastLogTime = startTime;
        memoryUsagePeak = 0;
    }
    
    ~PerformanceMonitor() {
        stop();
    }
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    /**
     * Start performance monitoring
     */
    void start() {
        isRunning = true;
        stopRequested = false;
        startTime = std::chrono::high_resolution_clock::now();
        
        // Start monitoring thread
        monitorThread = std::thread(&PerformanceMonitor::monitorLoop, this);
    }
    
    /**
     * Stop performance monitoring
     */
    void stop() {
        if (isRunning) {
            stopRequested = true;
            monitorThread.join();
            isRunning = false;
        }
    }
    
    /**
     * Record metrics for a simulation step
     */
    void recordStep(size_t spikesGenerated, size_t activeNeurons,
                   size_t memoryUsage, size_t stdpUpdates, size_t hebbianUpdates) {
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        auto now = std::chrono::high_resolution_clock::now();
        StepMetrics metrics;
        metrics.timestamp = now;
        metrics.spikesGenerated = spikesGenerated;
        metrics.activeNeurons = activeNeurons;
        metrics.memoryUsageBytes = memoryUsage;
        metrics.stdpUpdates = stdpUpdates;
        metrics.hebbianUpdates = hebbianUpdates;
        
        // Calculate step time
        if (!stepTimes.empty()) {
            auto prevTime = stepTimes.back();
            metrics.stepTimeMs = std::chrono::duration<double, std::milli>(now - prevTime).count();
        } else {
            metrics.stepTimeMs = 0.0;
        }
        
        stepTimes.push_back(now);
        recentMetrics.push_back(metrics);
        
        // Keep only recent metrics (last 10 minutes)
        auto cutoffTime = now - std::chrono::minutes(10);
        recentMetrics.erase(std::remove_if(recentMetrics.begin(), recentMetrics.end(),
                                          [cutoffTime](const StepMetrics& m) { return m.timestamp < cutoffTime; }),
                           recentMetrics.end());
        
        // Update statistics
        totalSteps++;
        totalSpikesGenerated += spikesGenerated;
        totalActiveNeurons += activeNeurons;
        totalSTDPUpdates += stdpUpdates;
        totalHebbianUpdates += hebbianUpdates;
        
        // Update memory peak
        if (memoryUsage > memoryUsagePeak) {
            memoryUsagePeak = memoryUsage;
        }
        
        // Signal for metrics processing
        metricsCondition.notify_one();
    }
    
    /**
     * Record neuromodulation updates
     */
    void recordNeuromodulation(size_t dopamineUpdates, size_t curiosityUpdates, size_t noveltyUpdates) {
        std::lock_guard<std::mutex> lock(metricsMutex);
        totalDopamineUpdates += dopamineUpdates;
        totalCuriosityUpdates += curiosityUpdates;
        totalNoveltyUpdates += noveltyUpdates;
    }
    
    /**
     * Record structural plasticity events
     */
    void recordStructuralPlasticity(size_t events) {
        std::lock_guard<std::mutex> lock(metricsMutex);
        totalStructuralPlasticityEvents += events;
    }
    
    /**
     * Get current system statistics
     */
    SystemStats getSystemStats() const {
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        SystemStats stats;
        
        if (!recentMetrics.empty()) {
            // Calculate timing statistics
            std::vector<double> stepTimesMs;
            for (size_t i = 1; i < recentMetrics.size(); ++i) {
                double stepTime = std::chrono::duration<double, std::milli>(
                    recentMetrics[i].timestamp - recentMetrics[i-1].timestamp).count();
                stepTimesMs.push_back(stepTime);
            }
            
            if (!stepTimesMs.empty()) {
                stats.minStepTime = *std::min_element(stepTimesMs.begin(), stepTimesMs.end());
                stats.maxStepTime = *std::max_element(stepTimesMs.begin(), stepTimesMs.end());
                double sum = std::accumulate(stepTimesMs.begin(), stepTimesMs.end(), 0.0);
                stats.avgStepTime = sum / stepTimesMs.size();
                
                // Calculate standard deviation
                double mean = stats.avgStepTime;
                double sqSum = 0.0;
                for (double time : stepTimesMs) {
                    sqSum += (time - mean) * (time - mean);
                }
                stats.stdStepTime = std::sqrt(sqSum / stepTimesMs.size());
            }
            
            // Calculate timing statistics
            stats.totalSimTime = std::chrono::duration<double, std::milli>(
                recentMetrics.back().timestamp - startTime).count();
            
            if (stats.totalSimTime > 0.0) {
                stats.stepsPerSecond = (totalSteps * 1000.0) / stats.totalSimTime;
                stats.simulationSpeed = stats.stepsPerSecond / 1000.0; // Assuming 1ms target
            }
            
            // Calculate activity statistics
            stats.avgSpikesPerStep = totalSteps > 0 ? 
                static_cast<double>(totalSpikesGenerated) / totalSteps : 0.0;
            stats.avgActiveNeurons = totalSteps > 0 ? 
                static_cast<double>(totalActiveNeurons) / totalSteps : 0.0;
            stats.avgFiringRate = stats.totalSimTime > 0.0 ? 
                (stats.avgSpikesPerStep * 1000.0 / 1000.0) : 0.0; // Hz
            
            // Calculate plasticity statistics
            stats.totalSTDPUpdates = totalSTDPUpdates;
            stats.totalHebbianUpdates = totalHebbianUpdates;
            stats.totalStructuralPlasticityEvents = totalStructuralPlasticityEvents;
            
            // Calculate neuromodulation statistics
            stats.totalDopamineUpdates = totalDopamineUpdates;
            stats.totalCuriosityUpdates = totalCuriosityUpdates;
            stats.totalNoveltyUpdates = totalNoveltyUpdates;
            
            // Calculate memory statistics
            stats.maxMemoryUsageBytes = memoryUsagePeak;
            if (!recentMetrics.empty()) {
                double sumMemory = 0.0;
                for (const auto& metrics : recentMetrics) {
                    sumMemory += metrics.memoryUsageBytes;
                }
                stats.avgMemoryUsageBytes = sumMemory / recentMetrics.size();
            }
            
            // Calculate stability score
            stats.stabilityScore = calculateStabilityScore();
            
            // Calculate learning efficiency
            stats.learningEfficiency = calculateLearningEfficiency();
            
            // Determine if stable
            stats.isStable = isNetworkStable();
        }
        
        return stats;
    }
    
    /**
     * Get performance summary as string
     */
    std::string getPerformanceSummary() const {
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        SystemStats stats = getSystemStats();
        std::stringstream ss;
        
        ss << "=== Performance Monitor Summary ===\n";
        ss << "Simulation Time: " << stats.totalSimTime << " ms\n";
        ss << "Steps per Second: " << stats.stepsPerSecond << "\n";
        ss << "Average Step Time: " << stats.avgStepTime << " ms\n";
        ss << "Min/Max Step Time: " << stats.minStepTime << " / " << stats.maxStepTime << " ms\n";
        ss << "\nActivity:\n";
        ss << "  Total Spikes Generated: " << stats.totalSpikesGenerated << "\n";
        ss << "  Average Spikes per Step: " << stats.avgSpikesPerStep << "\n";
        ss << "  Average Active Neurons: " << stats.avgActiveNeurons << "\n";
        ss << "  Average Firing Rate: " << stats.avgFiringRate << " Hz\n";
        ss << "\nPlasticity:\n";
        ss << "  STDP Updates: " << stats.totalSTDPUpdates << "\n";
        ss << "  Hebbian Updates: " << stats.totalHebbianUpdates << "\n";
        ss << "  Structural Plasticity Events: " << stats.totalStructuralPlasticityEvents << "\n";
        ss << "\nNeuromodulation:\n";
        ss << "  Dopamine Updates: " << stats.totalDopamineUpdates << "\n";
        ss << "  Curiosity Updates: " << stats.totalCuriosityUpdates << "\n";
        ss << "  Novelty Updates: " << stats.totalNoveltyUpdates << "\n";
        ss << "\nMemory:\n";
        ss << "  Peak Usage: " << stats.maxMemoryUsageBytes << " bytes\n";
        ss << "  Average Usage: " << stats.avgMemoryUsageBytes << " bytes\n";
        ss << "\nSystem Health:\n";
        ss << "  Stable: " << (stats.isStable ? "Yes" : "No") << "\n";
        ss << "  Stability Score: " << stats.stabilityScore << "\n";
        ss << "  Learning Efficiency: " << stats.learningEfficiency << "\n";
        
        return ss.str();
    }
    
    /**
     * Export performance data to file
     */
    bool exportPerformanceData(const std::string& filename) const {
        std::lock_guard<std::mutex> lock(metricsMutex);
        
        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            file << getPerformanceSummary() << std::endl;
            
            // Export detailed step metrics
            file << "\n=== Detailed Step Metrics ===\n";
            for (size_t i = 0; i < recentMetrics.size(); ++i) {
                const auto& metrics = recentMetrics[i];
                file << "Step " << i << ":\n";
                file << "  Timestamp: " << std::chrono::duration_cast<std::chrono::milliseconds>(
                    metrics.timestamp - startTime).count() << " ms\n";
                file << "  Step Time: " << metrics.stepTimeMs << " ms\n";
                file << "  Spikes Generated: " << metrics.spikesGenerated << "\n";
                file << "  Active Neurons: " << metrics.activeNeurons << "\n";
                file << "  Memory Usage: " << metrics.memoryUsageBytes << " bytes\n";
                file << "  STDP Updates: " << metrics.stdpUpdates << "\n";
                file << "  Hebbian Updates: " << metrics.hebbianUpdates << "\n\n";
            }
            
            file.close();
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    /**
     * Start monitoring thread loop
     */
    void monitorLoop() {
        while (!stopRequested) {
            std::unique_lock<std::mutex> lock(metricsMutex);
            metricsCondition.wait_for(lock, std::chrono::seconds(1), 
                                     [this] { return stopRequested; });
            
            if (stopRequested) break;
            
            // Log performance summary every 5 minutes
            auto now = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::minutes>(now - lastLogTime).count() >= 5) {
                NLM_LOG_INFO(getPerformanceSummary());
                lastLogTime = now;
            }
        }
    }
    
private:
    /**
     * Calculate network stability score
     */
    double calculateStabilityScore() const {
        if (recentMetrics.size() < 10) return 0.5; // Not enough data
        
        // Calculate variance in spike generation
        std::vector<size_t> spikes;
        for (const auto& metrics : recentMetrics) {
            spikes.push_back(metrics.spikesGenerated);
        }
        
        double mean = std::accumulate(spikes.begin(), spikes.end(), 0.0) / spikes.size();
        double variance = 0.0;
        for (size_t spike : spikes) {
            variance += (spike - mean) * (spike - mean);
        }
        variance /= spikes.size();
        
        double spikeStdDev = std::sqrt(variance);
        
        // Calculate stability score (inversely proportional to spike variance)
        double stability = std::exp(-spikeStdDev / 10.0);
        return std::max(0.0, std::min(1.0, stability));
    }
    
    /**
     * Calculate learning efficiency
     */
    double calculateLearningEfficiency() const {
        if (recentMetrics.size() < 10) return 0.0;
        
        // Calculate plasticity activity growth over time
        size_t earlyUpdates = 0, lateUpdates = 0;
        size_t earlyCount = recentMetrics.size() / 2;
        
        for (size_t i = 0; i < recentMetrics.size(); ++i) {
            if (i < earlyCount) {
                earlyUpdates += recentMetrics[i].stdpUpdates + recentMetrics[i].hebbianUpdates;
            } else {
                lateUpdates += recentMetrics[i].stdpUpdates + recentMetrics[i].hebbianUpdates;
            }
        }
        
        // Learning efficiency is improvement in later steps
        if (earlyUpdates > 0) {
            return static_cast<double>(lateUpdates) / earlyUpdates;
        }
        return 0.0;
    }
    
    /**
     * Check if network is stable
     */
    bool isNetworkStable() const {
        if (recentMetrics.size() < 10) return true;
        
        // Check for extreme spike generation (indicates instability)
        size_t maxSpikes = 0;
        for (const auto& metrics : recentMetrics) {
            if (metrics.spikesGenerated > maxSpikes) {
                maxSpikes = metrics.spikesGenerated;
            }
        }
        
        // Check for very high spike rates (instability)
        double spikeRate = static_cast<double>(maxSpikes) / recentMetrics.size();
        return spikeRate < 100.0; // Threshold for stability
    }
    
    // Threading support
    std::vector<std::thread> threadPool;
    std::mutex metricsMutex;
    std::condition_variable metricsCondition;
    
    // Performance data
    std::vector<StepMetrics> recentMetrics;
    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> stepTimes;
    
    // Statistics
    size_t totalSteps;
    size_t totalSpikesGenerated;
    size_t totalActiveNeurons;
    size_t totalSTDPUpdates;
    size_t totalHebbianUpdates;
    size_t totalStructuralPlasticityEvents;
    size_t totalDopamineUpdates;
    size_t totalCuriosityUpdates;
    size_t totalNoveltyUpdates;
    size_t memoryUsagePeak;
    
    // Monitoring control
    bool isRunning;
    bool stopRequested;
    
    // Timing
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastLogTime;
};