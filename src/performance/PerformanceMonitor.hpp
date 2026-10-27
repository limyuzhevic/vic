#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>

namespace nlm {

/**
 * @class PerformanceSample
 * @brief A single performance measurement sample
 * 
 * Represents a point-in-time performance measurement including timing,
 * memory usage, and system metrics collected during neural simulation.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class PerformanceSample {
public:
    /**
     * @brief Construct a new PerformanceSample object
     * 
     * @param timestamp Simulation time when sample was collected
     * @param stepNumber Current simulation step
     */
    PerformanceSample(Timestamp timestamp, SimulationStep stepNumber)
        : timestamp_(timestamp), stepNumber_(stepNumber)
    {
        collectBaselineMetrics();
    }
    
    /**
     * @brief Get the sample timestamp
     */
    Timestamp getTimestamp() const { return timestamp_; }
    
    /**
     * @brief Get the simulation step when sample was collected
     */
    SimulationStep getStepNumber() const { return stepNumber_; }
    
    /**
     * @brief Get the total spike count at sampling time
     */
    size_t getTotalSpikeCount() const { return totalSpikeCount_; }
    
    /**
     * @brief Get the firing neuron count at sampling time
     */
    size_t getFiringNeuronCount() const { return firingNeuronCount_; }
    
    /**
     * @brief Get the average firing rate at sampling time
     */
    float getAverageFiringRate() const { return averageFiringRate_; }
    
    /**
     * @brief Get the excitation/inhibition ratio at sampling time
     */
     float getExcitationInhibitionRatio() const { return excitationInhibitionRatio_; }
    
    /**
     * @brief Get the working memory trace count at sampling time
     */
    size_t getWorkingMemoryTraceCount() const { return workingMemoryTraceCount_; }
    
    /**
     * @brief Get the episodic memory episode count at sampling time
     */
    size_t getEpisodicMemoryEpisodeCount() const { return episodicMemoryEpisodeCount_; }
    
    /**
     * @brief Get the dopamine level at sampling time
     */
    float getDopamineLevel() const { return dopamineLevel_; }
    
    /**
     * @brief Get the curiosity level at sampling time
     */
    float getCuriosityLevel() const { return curiosityLevel_; }
    
    /**
     * @brief Get the developmental stage at sampling time
     */
    DevelopmentalStage getDevelopmentalStage() const { return developmentalStage_; }
    
    /**
     * @brief Get the pending spike event count at sampling time
     */
    size_t getPendingSpikeEventCount() const { return pendingSpikeEventCount_; }
    
    /**
     * @brief Get the time taken to process the last step (microseconds)
     */
    uint64_t getStepProcessingTime() const { return stepProcessingTime_; }
    
    /**
     * @brief Get the memory usage in bytes at sampling time
     */
    uint64_t getMemoryUsage() const { return memoryUsage_; }
    
    /**
     * @brief Update all metrics from brain state
     * 
     * @param brain Pointer to brain to read metrics from
     */
    void updateMetrics(const Brain* brain);
    
    /**
     * @brief Set the step processing time
     */
    void setStepProcessingTime(uint64_t microseconds) { stepProcessingTime_ = microseconds; }
    
    /**
     * @brief Set the memory usage
     */
    void setMemoryUsage(uint64_t bytes) { memoryUsage_ = bytes; }
    
    /**
     * @brief Convert sample to string representation for logging
     */
    std::string toString() const;
    
    /**
     * @brief Calculate the time elapsed since previous sample
     */
    double getTimeSince(const PerformanceSample& previous) const {
        return timestamp_ - previous.timestamp_;
    }
    
    /**
     * @brief Calculate the step difference since previous sample
     */
    SimulationStep getStepsSince(const PerformanceSample& previous) const {
        return stepNumber_ - previous.stepNumber_;
    }

private:
    void collectBaselineMetrics();
    
    Timestamp timestamp_;                    // Simulation time when collected
    SimulationStep stepNumber_;               // Current simulation step
    
    // Neural system metrics
    size_t totalSpikeCount_;                  // Total spikes generated
    size_t firingNeuronCount_;                // Neurons currently firing
    float averageFiringRate_;                 // Average firing rate (Hz)
    float excitationInhibitionRatio_;        // E/I balance ratio
    size_t workingMemoryTraceCount_;          // Active working memory traces
    size_t episodicMemoryEpisodeCount_;      // Number of stored episodes
    float dopamineLevel_;                    // Neuromodulation level
    float curiosityLevel_;                   // Curiosity level
    DevelopmentalStage developmentalStage_;  // Current developmental stage
    size_t pendingSpikeEventCount_;           // Number of pending spike events
    
    // Performance metrics
    uint64_t stepProcessingTime_;             // Step processing time (μs)
    uint64_t memoryUsage_;                    // Memory usage (bytes)
};

/**
 * @class PerformanceMonitor
 * @brief Comprehensive performance monitoring and profiling system
 * 
 * Provides detailed performance tracking for neural simulations including:
 * - Real-time performance sampling and aggregation
 * - Historical performance data management
 * - Bottleneck detection and performance analysis
 * - Progress tracking and reporting
 * - Custom metric collection and analysis
 * 
 * The PerformanceMonitor integrates with the brain simulation loop to
 * collect timing, memory, and neural activity metrics at configurable
 * sampling intervals.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class PerformanceMonitor {
public:
    /**
     * @brief Construct a new PerformanceMonitor object
     */
    PerformanceMonitor()
        : samplingInterval_(100)  // Sample every 100 steps by default
        , isMonitoring_(false)
        , startTime_(std::chrono::high_resolution_clock::now())
        , totalStepsProcessed_(0)
        , totalMemoryAllocated_(0)
        , maxMemoryUsage_(0)
    {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    /**
     * @brief Destroy the PerformanceMonitor object
     */
    ~PerformanceMonitor() = default;
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    /**
     * @brief Start performance monitoring
     */
    void startMonitoring() { isMonitoring_ = true; }
    
    /**
     * @brief Stop performance monitoring
     */
    void stopMonitoring() { isMonitoring_ = false; }
    
    /**
     * @brief Check if monitoring is active
     */
    bool isMonitoring() const { return isMonitoring_; }
    
    /**
     * @brief Set the sampling interval in simulation steps
     */
    void setSamplingInterval(SimulationStep interval) { samplingInterval_ = interval; }
    
    /**
     * @brief Get the sampling interval
     */
    SimulationStep getSamplingInterval() const { return samplingInterval_; }
    
    /**
     * @brief Record a performance sample from the brain state
     * 
     * @param brain Pointer to brain to collect metrics from
     * @param currentTime Current simulation time
     * @param currentStep Current simulation step
     */
    void recordSample(const Brain* brain, Timestamp currentTime, SimulationStep currentStep);
    
    /**
     * @brief Get the current sample count
     */
    size_t getSampleCount() const { return samples_.size(); }
    
    /**
     * @brief Get the most recent sample
     */
    const PerformanceSample& getLatestSample() const {
        if (samples_.empty()) {
            throw std::runtime_error("No samples available");
        }
        return samples_.back();
    }
    
    /**
     * @brief Get all collected samples
     */
    const std::vector<PerformanceSample>& getAllSamples() const { return samples_; }
    
    /**
     * @brief Get the total simulation time since monitoring started
     */
    double getTotalSimulationTime() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_);
        return duration.count() / 1000.0;  // Convert to seconds
    }
    
    /**
     * @brief Get the total number of steps processed
     */
    SimulationStep getTotalStepsProcessed() const { return totalStepsProcessed_; }
    
    /**
     * @brief Get the average steps per second
     */
    double getAverageStepsPerSecond() const {
        double totalTime = getTotalSimulationTime();
        return totalTime > 0.0 ? totalStepsProcessed_ / totalTime : 0.0;
    }
    
    /**
     * @brief Get the average processing time per step (microseconds)
     */
    double getAverageStepProcessingTime() const {
        if (totalStepsProcessed_ == 0) return 0.0;
        uint64_t totalProcessingTime = 0;
        for (const auto& sample : samples_) {
            totalProcessingTime += sample.getStepProcessingTime();
        }
        return static_cast<double>(totalProcessingTime) / samples_.size();
    }
    
    /**
     * @brief Get the maximum processing time per step (microseconds)
     */
    uint64_t getMaxStepProcessingTime() const {
        if (samples_.empty()) return 0;
        return std::max_element(samples_.begin(), samples_.end(),
            [](const PerformanceSample& a, const PerformanceSample& b) {
                return a.getStepProcessingTime() < b.getStepProcessingTime();
            })->getStepProcessingTime();
    }
    
    /**
     * @brief Get the memory usage statistics
     */
    std::string getMemoryStatistics() const {
        std::stringstream ss;
        ss << "Memory Usage: " << (maxMemoryUsage_ / 1024) << " KB peak, "
           << (maxMemoryUsage_ / (1024 * 1024)) << " MB peak\n";
        ss << "Total Allocated: " << (totalMemoryAllocated_ / 1024) << " KB\n";
        return ss.str();
    }
    
    /**
     * @brief Get the performance summary report
     */
    std::string getPerformanceSummary() const;
    
    /**
     * @brief Get the neural performance metrics
     */
    std::string getNeuralPerformanceReport() const;
    
    /**
     * @brief Get the bottleneck analysis report
     */
    std::string getBottleneckAnalysis() const;
    
    /**
     * @brief Clear all collected samples
     */
    void clearSamples() {
        samples_.clear();
        totalStepsProcessed_ = 0;
    }
    
    /**
     * @brief Check for performance bottlenecks
     * 
     * Analyzes collected performance data to identify potential bottlenecks
     * such as slow step processing, memory leaks, or neural system issues.
     * 
     * @return true if bottlenecks are detected, false otherwise
     */
    bool analyzeForBottlenecks() const;
    
    /**
     * @brief Get the performance trend analysis
     * 
     * Analyzes performance trends over time to identify degradation,
     * improvements, or stability patterns.
     * 
     * @return std::string Performance trend analysis report
     */
    std::string getPerformanceTrendAnalysis() const;
    
    /**
     * @brief Export performance data for external analysis
     * 
     * @param format Output format ("csv", "json", "txt")
     * @param filename Output filename (without extension)
     * @return true if export succeeds, false otherwise
     */
    bool exportPerformanceData(const std::string& format, const std::string& filename) const;
    
    /**
     * @brief Update memory usage statistics
     */
    void updateMemoryUsage(uint64_t bytes) {
        totalMemoryAllocated_ += bytes;
        maxMemoryUsage_ = std::max(maxMemoryUsage_, totalMemoryAllocated_);
    }
    
    /**
     * @brief Update step processing time statistics
     */
    void updateStepProcessingTime(uint64_t microseconds) {
        if (currentStepTimeStart_ > 0) {
            uint64_t elapsed = microseconds - currentStepTimeStart_;
            // Add to running statistics (simplified - would normally store in current sample)
        }
        currentStepTimeStart_ = microseconds;
    }
    
    /**
     * @brief Start timing for the next step
     */
    void startStepTiming() {
        currentStepTimeStart_ = getCurrentTimestamp();
    }
    
    /**
     * @brief Get current timestamp in microseconds
     */
    uint64_t getCurrentTimestamp() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - startTime_);
        return duration.count();
    }

private:
    std::vector<PerformanceSample> samples_;           // Collected performance samples
    SimulationStep samplingInterval_;                 // Steps between samples
    bool isMonitoring_;                               // Whether monitoring is active
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;  // Start time
    SimulationStep totalStepsProcessed_;              // Total steps processed
    uint64_t totalMemoryAllocated_;                   // Total memory allocated
    uint64_t maxMemoryUsage_;                         // Peak memory usage
    uint64_t currentStepTimeStart_;                   // Step timing start point
};

/**
 * @class PerformanceProfiler
 * @brief High-resolution performance profiler for detailed timing analysis
 * 
 * Provides detailed timing information for specific code sections and functions.
 * Useful for identifying performance bottlenecks and optimization opportunities.
 * 
 * @author NLM Research Team
 * @version 0.1.0
 */
class PerformanceProfiler {
public:
    /**
     * @brief Construct a new PerformanceProfiler object
     */
    PerformanceProfiler()
    {
        startTime_ = std::chrono::high_resolution_clock::now();
        accumulatedTime_ = 0;
        callCount_ = 0;
    }
    
    /**
     * @brief Destroy the PerformanceProfiler object
     */
    ~PerformanceProfiler() = default;
    
    /**
     * @brief Start timing the current operation
     */
    void start() { startTime_ = std::chrono::high_resolution_clock::now(); }
    
    /**
     * @brief Stop timing and record the elapsed time
     */
    void stop() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        accumulatedTime_ += duration.count();
        callCount_++;
    }
    
    /**
     * @brief Get the accumulated time in microseconds
     */
    uint64_t getAccumulatedTime() const { return accumulatedTime_; }
    
    /**
     * @brief Get the average time per call in microseconds
     */
    double getAverageTime() const {
        return callCount_ > 0 ? static_cast<double>(accumulatedTime_) / callCount_ : 0.0;
    }
    
    /**
     * @brief Get the call count
     */
    size_t getCallCount() const { return callCount_; }
    
    /**
     * @brief Reset the profiler statistics
     */
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
        accumulatedTime_ = 0;
        callCount_ = 0;
    }
    
    /**
     * @brief Get the current timing rate (calls per second)
     */
    double getRate() const {
        if (accumulatedTime_ == 0) return 0.0;
        return (callCount_ * 1000000.0) / accumulatedTime_;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
    uint64_t accumulatedTime_;   // Total accumulated time in microseconds
    size_t callCount_;           // Number of calls
};

} // namespace nlm
