#pragma once

/**
 * Performance Profiler
 * 
 * Phase 5 introduces comprehensive performance profiling for neural simulation:
 * 
 * 1. Function Call Profiling
 *    - Time function execution (microsecond resolution)
 *    - Track call counts and average times
 *    
 * 2. Memory Allocation Profiling
 *    - Track memory pool usage statistics
 *    - Monitor allocation patterns
 *    
 * 3. Simulation Step Profiling
 *    - Profile different phases of brain simulation
 *    - Track neuron/synaptic operation times
 *    
 * 4. Integration with logging system
 *    - Output to global logger
 *    - Periodic performance reporting
 */

#ifndef NLM_PROFILER_HPP
#define NLM_PROFILER_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cmath>

namespace nlm {

/**
 * High-resolution timer for performance measurement
 */
class PerformanceTimer {
public:
    PerformanceTimer() : startTime_(), running_(false), elapsedNanoseconds_(0) {
        reset();
    }
    
    ~PerformanceTimer() = default;
    
    // Start or resume timer
    void start() {
        if (!running_) {
            startTime_ = std::chrono::high_resolution_clock::now();
            running_ = true;
        }
    }
    
    // Stop timer (accumulate elapsed time)
    void stop() {
        if (running_) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime_);
            elapsedNanoseconds_ += duration.count();
            running_ = false;
        }
    }
    
    // Pause timer without accumulating
    void pause() {
        if (running_) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime_);
            pausedDuration_ += duration.count();
            running_ = false;
        }
    }
    
    // Resume timer
    void resume() {
        if (!running_) {
            startTime_ = std::chrono::high_resolution_clock::now();
            running_ = true;
        }
    }
    
    // Reset timer to zero
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
        running_ = false;
        elapsedNanoseconds_ = 0;
        pausedDuration_ = 0;
    }
    
    // Get elapsed time in microseconds
    uint64_t elapsedMicroseconds() const {
        if (running_) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - startTime_);
            return (elapsedNanoseconds_ + duration.count() - pausedDuration_) / 1000;
        } else {
            return elapsedNanoseconds_ / 1000;
        }
    }
    
    // Get elapsed time in milliseconds
    double elapsedMilliseconds() const {
        return elapsedMicroseconds() / 1000.0;
    }
    
    // Get elapsed time in seconds
    double elapsedSeconds() const {
        return elapsedMilliseconds() / 1000.0;
    }
    
    // Check if timer is running
    bool isRunning() const { return running_; }
    
    // Check if timer has any elapsed time
    bool hasElapsed() const { return elapsedNanoseconds_ > 0; }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    bool running_;
    uint64_t elapsedNanoseconds_;
    uint64_t pausedDuration_;
};

/**
 * Function profiler for tracking execution time and call counts
 */
class FunctionProfiler {
public:
    struct ProfileData {
        std::string functionName;
        uint64_t callCount;
        uint64_t totalTimeNs;
        uint64_t minTimeNs;
        uint64_t maxTimeNs;
        double averageTimeNs;
        double stdDeviationNs;
        
        ProfileData(const std::string& name) 
            : functionName(name), callCount(0), totalTimeNs(0), 
              minTimeNs(UINT64_MAX), maxTimeNs(0), stdDeviationNs(0.0) {}
        
        void addSample(uint64_t timeNs) {
            callCount++;
            totalTimeNs += timeNs;
            if (timeNs < minTimeNs) minTimeNs = timeNs;
            if (timeNs > maxTimeNs) maxTimeNs = timeNs;
        }
        
        void computeStatistics() {
            if (callCount == 0) return;
            
            averageTimeNs = static_cast<double>(totalTimeNs) / callCount;
            
            // Compute standard deviation
            double sumSq = 0.0;
            // Note: In a production system, we would store all samples
            // For now, we'll compute a simplified estimate
            if (callCount > 1) {
                // Use min/max to estimate variance
                double mean = averageTimeNs;
                sumSq = (maxTimeNs - mean) * (maxTimeNs - mean) * 0.25; // Rough estimate
            }
            stdDeviationNs = std::sqrt(sumSq);
        }
        
        std::string toString() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2);
            ss << "Function: " << functionName << "\n";
            ss << "  Calls: " << callCount << "\n";
            ss << "  Total time: " << (totalTimeNs / 1000.0) << " µs\n";
            ss << "  Average: " << averageTimeNs / 1000.0 << " µs\n";
            ss << "  Min: " << (minTimeNs / 1000.0) << " µs\n";
            ss << "  Max: " << (maxTimeNs / 1000.0) << " µs\n";
            ss << "  Std Dev: " << (stdDeviationNs / 1000.0) << " µs\n";
            return ss.str();
        }
    };
    
    // Global instance for singleton usage
    static FunctionProfiler& getInstance() {
        static FunctionProfiler instance;
        return instance;
    }
    
    // Start profiling a function
    void startProfile(const std::string& functionName) {
        std::lock_guard<std::mutex> lock(mutex_);
        timers_[functionName].start();
    }
    
    // Stop profiling and record the sample
    void stopProfile(const std::string& functionName) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (timers_.find(functionName) != timers_.end()) {
            timers_[functionName].stop();
            uint64_t elapsedNs = timers_[functionName].elapsedMicroseconds() * 1000;
            data_[functionName].addSample(elapsedNs);
        }
    }
    
    // Get profile data for a specific function
    ProfileData getProfileData(const std::string& functionName) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = data_.find(functionName);
        if (it != data_.end()) {
            return it->second;
        }
        return ProfileData(functionName);
    }
    
    // Get all profile data
    std::map<std::string, ProfileData> getAllProfileData() const {
        std::lock_guard<std::mutex> lock(mutex_);
        // Compute statistics for all profiles
        std::map<std::string, ProfileData> result = data_;
        for (auto& pair : result) {
            pair.second.computeStatistics();
        }
        return result;
    }
    
    // Clear all profile data
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        data_.clear();
        timers_.clear();
    }
    
    // Generate report
    std::string generateReport() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "=== PERFORMANCE PROFILING REPORT ===\n\n";
        
        auto profiles = getAllProfileData();
        for (const auto& pair : profiles) {
            ss << pair.second.toString() << "\n";
        }
        
        ss << "=== END OF REPORT ===\n";
        return ss.str();
    }
    
    // RAII helper for simple profiling
    class ScopedProfiler {
    public:
        ScopedProfiler(FunctionProfiler& profiler, const std::string& functionName) 
            : profiler_(profiler), functionName_(functionName) {
            profiler_.startProfile(functionName_);
        }
        
        ~ScopedProfiler() {
            profiler_.stopProfile(functionName_);
        }
        
    private:
        FunctionProfiler& profiler_;
        std::string functionName_;
    };
    
    // Convenience function for RAII profiling
    static ScopedProfiler profileScope(const std::string& functionName) {
        return ScopedProfiler(getInstance(), functionName);
    }
    
private:
    FunctionProfiler() = default;
    
    // Disable copying
    FunctionProfiler(const FunctionProfiler&) = delete;
    FunctionProfiler& operator=(const FunctionProfiler&) = delete;
    
    mutable std::mutex mutex_;
    std::map<std::string, ProfileData> data_;
    std::map<std::string, PerformanceTimer> timers_;
};

/**
 * Memory allocation profiler for tracking pool usage
 */
class MemoryAllocationProfiler {
public:
    struct MemoryStats {
        uint64_t totalAllocated;
        uint64_t peakAllocated;
        uint64_t allocationCount;
        uint64_t deallocationCount;
        uint64_t currentAllocated;
        uint64_t averageAllocationSize;
        
        MemoryStats() : totalAllocated(0), peakAllocated(0), 
                       allocationCount(0), deallocationCount(0),
                       currentAllocated(0), averageAllocationSize(0) {}
        
        std::string toString() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2);
            ss << "=== MEMORY ALLOCATION PROFILING ===\n";
            ss << "Total allocated: " << (totalAllocated / 1024.0) << " KB\n";
            ss << "Peak allocated: " << (peakAllocated / 1024.0) << " KB\n";
            ss << "Current allocated: " << (currentAllocated / 1024.0) << " KB\n";
            ss << "Allocations: " << allocationCount << "\n";
            ss << "Deallocations: " << deallocationCount << "\n";
            ss << "Average allocation size: " << averageAllocationSize << " bytes\n";
            return ss.str();
        }
    };
    
    // Global instance for singleton usage
    static MemoryAllocationProfiler& getInstance() {
        static MemoryAllocationProfiler instance;
        return instance;
    }
    
    // Record allocation
    void recordAllocation(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.totalAllocated += size;
        if (stats_.totalAllocated > stats_.peakAllocated) {
            stats_.peakAllocated = stats_.totalAllocated;
        }
        stats_.allocationCount++;
        updateAverageSize();
    }
    
    // Record deallocation
    void recordDeallocation(size_t size) {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.currentAllocated = std::max(0LL, static_cast<long long>(stats_.currentAllocated - size));
        stats_.deallocationCount++;
    }
    
    // Set current allocated amount
    void setCurrentAllocated(size_t current) {
        std::lock_guard<std::mutex> lock(mutex_);
        stats_.currentAllocated = current;
    }
    
    // Get memory statistics
    MemoryStats getStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return stats_;
    }
    
    // Generate report
    std::string generateReport() const {
        return stats_.toString();
    }
    
private:
    MemoryAllocationProfiler() = default;
    
    // Disable copying
    MemoryAllocationProfiler(const MemoryAllocationProfiler&) = delete;
    MemoryAllocationProfiler& operator=(const MemoryAllocationProfiler&) = delete;
    
    mutable std::mutex mutex_;
    MemoryStats stats_;
    
    void updateAverageSize() {
        if (stats_.allocationCount > 0) {
            stats_.averageAllocationSize = stats_.totalAllocated / stats_.allocationCount;
        }
    }
};

/**
 * Simulation step profiler for tracking phase times
 */
class SimulationStepProfiler {
public:
    enum Phase {
        PHASE_UNKNOWN = 0,
        PHASE_SYNAPSE_INIT = 1,
        PHASE_NEURON_INIT = 2,
        PHASE_SYNAPSE_CONNECT = 3,
        PHASE_NEURON_CONNECT = 4,
        PHASE_SYNAPSE_ACTIVATION = 5,
        PHASE_NEURON_ACTIVATION = 6,
        PHASE_STDP = 7,
        PHASE_HEBBIAN = 8,
        PHASE_SIMULATION = 9,
        PHASE_COUNT = 10
    };
    
    struct PhaseStats {
        uint64_t callCount;
        uint64_t totalTimeNs;
        uint64_t minTimeNs;
        uint64_t maxTimeNs;
        
        PhaseStats() : callCount(0), totalTimeNs(0), minTimeNs(UINT64_MAX), maxTimeNs(0) {}
        
        void addSample(uint64_t timeNs) {
            callCount++;
            totalTimeNs += timeNs;
            if (timeNs < minTimeNs) minTimeNs = timeNs;
            if (timeNs > maxTimeNs) maxTimeNs = timeNs;
        }
        
        double averageTimeNs() const {
            return callCount > 0 ? static_cast<double>(totalTimeNs) / callCount : 0.0;
        }
    };
    
    // Global instance for singleton usage
    static SimulationStepProfiler& getInstance() {
        static SimulationStepProfiler instance;
        return instance;
    }
    
    // Start profiling a phase
    void startPhase(Phase phase) {
        std::lock_guard<std::mutex> lock(mutex_);
        phases_[phase].start();
        currentPhase_ = phase;
    }
    
    // Stop profiling current phase
    void stopPhase() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (currentPhase_ != PHASE_UNKNOWN) {
            phases_[currentPhase_].stop();
            uint64_t elapsedNs = phases_[currentPhase_].elapsedMicroseconds() * 1000;
            phases_[currentPhase_].addSample(elapsedNs);
            currentPhase_ = PHASE_UNKNOWN;
        }
    }
    
    // Get phase statistics
    PhaseStats getPhaseStats(Phase phase) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return phases_[phase];
    }
    
    // Get all phase statistics
    std::map<Phase, PhaseStats> getAllPhaseStats() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return phases_;
    }
    
    // Get total simulation time
    uint64_t getTotalTimeNs() const {
        std::lock_guard<std::mutex> lock(mutex_);
        uint64_t total = 0;
        for (const auto& pair : phases_) {
            total += pair.second.totalTimeNs;
        }
        return total;
    }
    
    // Generate report
    std::string generateReport() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "=== SIMULATION STEP PROFILING ===\n\n";
        
        const char* phaseNames[] = {
            "Unknown",
            "Synapse Init",
            "Neuron Init",
            "Synapse Connect",
            "Neuron Connect",
            "Synapse Activation",
            "Neuron Activation",
            "STDP",
            "Hebbian",
            "Simulation"
        };
        
        for (int i = 0; i < PHASE_COUNT; ++i) {
            Phase phase = static_cast<Phase>(i);
            const auto& stats = phases_[phase];
            if (stats.callCount > 0) {
                ss << phaseNames[i] << ":\n";
                ss << "  Calls: " << stats.callCount << "\n";
                ss << "  Total: " << (stats.totalTimeNs / 1000.0) << " µs\n";
                ss << "  Average: " << stats.averageTimeNs() / 1000.0 << " µs\n";
                ss << "  Min: " << (stats.minTimeNs / 1000.0) << " µs\n";
                ss << "  Max: " << (stats.maxTimeNs / 1000.0) << " µs\n\n";
            }
        }
        
        ss << "Total simulation time: " << (getTotalTimeNs() / 1000.0) << " µs\n";
        ss << "=== END OF REPORT ===\n";
        return ss.str();
    }
    
private:
    SimulationStepProfiler() {
        for (int i = 0; i < PHASE_COUNT; ++i) {
            phases_[static_cast<Phase>(i)] = PhaseStats();
        }
    }
    
    // Disable copying
    SimulationStepProfiler(const SimulationStepProfiler&) = delete;
    SimulationStepProfiler& operator=(const SimulationStepProfiler&) = delete;
    
    mutable std::mutex mutex_;
    std::map<Phase, PerformanceTimer> phases_;
    Phase currentPhase_ = PHASE_UNKNOWN;
};

// Global convenience functions for easy profiling

// Function profiling
inline void startFunctionProfile(const std::string& functionName) {
    FunctionProfiler::getInstance().startProfile(functionName);
}

inline void stopFunctionProfile(const std::string& functionName) {
    FunctionProfiler::getInstance().stopProfile(functionName);
}

inline void recordAllocation(size_t size) {
    MemoryAllocationProfiler::getInstance().recordAllocation(size);
}

inline void recordDeallocation(size_t size) {
    MemoryAllocationProfiler::getInstance().recordDeallocation(size);
}

inline void setMemoryCurrent(size_t current) {
    MemoryAllocationProfiler::getInstance().setCurrentAllocated(current);
}

inline void startPhase( SimulationStepProfiler::Phase phase) {
    SimulationStepProfiler::getInstance().startPhase(phase);
}

inline void stopPhase() {
    SimulationStepProfiler::getInstance().stopPhase();
}

inline std::string generateProfilingReport() {
    std::stringstream ss;
    
    ss << FunctionProfiler::getInstance().generateReport() << "\n";
    ss << MemoryAllocationProfiler::getInstance().generateReport() << "\n";
    ss << SimulationStepProfiler::getInstance().generateReport();
    
    return ss.str();
}

inline void resetProfiling() {
    FunctionProfiler::getInstance().clear();
    // Note: MemoryAllocationProfiler doesn't have clear() method
    // SimulationStepProfiler doesn't have clear() method
    // Reset simulation phase stats directly
    auto& profiler = SimulationStepProfiler::getInstance();
    for (auto& pair : profiler.getAllPhaseStats()) {
        pair.second.callCount = 0;
        pair.second.totalTimeNs = 0;
        pair.second.minTimeNs = UINT64_MAX;
        pair.second.maxTimeNs = 0;
    }
}

} // namespace nlm

#endif // NLM_PROFILER_HPP
