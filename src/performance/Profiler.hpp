#pragma once

/**
 * Profiler - Detailed performance profiling for NLM
 * 
 * Provides comprehensive profiling capabilities including:
 * - Line-by-line profiling
 * - Function call profiling
 * - Memory allocation profiling
 * - Spike timing analysis
 * - Plasticity event profiling
 * - Neuromodulation profiling
 * - Performance bottleneck detection
 * - Hot path identification
 * - Memory leak detection
 * - Performance regression tracking
 * 
 * Integration:
 * - Integrates with PerformanceMonitor for metrics collection
 * - Provides detailed profiling for Brain.cpp components
 * - Captures performance hotspots for optimization
 * - Tracks resource usage patterns
 */

#ifndef NLM_PROFILER_HPP
#define NLM_PROFILER_HPP

#include <chrono>
#include <deque>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <functional>
#include <numeric>
#include <set>
#include <stack>
#include <memory>
#include <limits>

namespace nlm {

// Line profiling data
struct LineProfileData {
    std::string fileName;           // Source file name
    int lineNumber;                  // Line number
    std::string functionName;        // Function containing the line
    uint64_t executionCount;         // Number of times executed
    double totalTime;                // Total execution time
    double averageTime;              // Average execution time
    double minTime;                  // Minimum execution time
    double maxTime;                  // Maximum execution time
    uint64_t memoryAllocated;        // Memory allocated at this line
    uint64_t functionCallCount;      // Number of function calls
    Timestamp timestamp;             // Timestamp of first execution
};

// Function profiling data
struct FunctionProfileData {
    std::string functionName;        // Function name
    std::string fileName;            // Source file name
    int lineNumber;                  // Line number where function is defined
    uint64_t callCount;              // Number of times called
    double totalTime;                // Total execution time
    double averageTime;              // Average execution time
    double cpuTime;                  // CPU time spent
    double memoryAllocated;          // Memory allocated by this function
    double memoryDeallocated;        // Memory deallocated by this function
    uint64_t nestedCallCount;        // Number of nested calls
    std::vector<std::string> callees; // Functions called by this function
    std::vector<std::string> callers; // Functions that called this function
    Timestamp timestamp;             // Timestamp of first call
};

// Memory allocation tracking
struct MemoryAllocationProfile {
    void* address;                   // Memory address
    size_t size;                     // Size in bytes
    std::string allocationSite;       // File and line where allocated
    Timestamp timestamp;              // When allocated
    std::thread::id threadId;         // Thread that allocated
    std::stack<std::string> callStack; // Call stack at allocation
    size_t allocationSequence;        // Unique allocation sequence number
};

// Spike timing analysis
struct SpikeTimingProfile {
    Timestamp spikeTime;              // When spike occurred
    uint64_t neuronId;                // Neuron that spiked
    double membranePotential;         // Membrane potential at spike time
    std::string neuronType;           // Type of neuron
    std::string region;              // Brain region
    Timestamp previousSpikeTime;      // Previous spike time
    double interSpikeInterval;        // Time since previous spike
    double synapticInput;             // Synaptic input at spike time
    Timestamp timestamp;              // Timestamp of recording
};

// Plasticity event profiling
struct PlasticityEventProfile {
    Timestamp eventTime;              // When event occurred
    std::string plasticityType;       // Type of plasticity (STDP, Hebbian)
    uint64_t synapseId;                // Synapse affected
    uint64_t preNeuronId;             // Pre-synaptic neuron
    uint64_t postNeuronId;            // Post-synaptic neuron
    double weightChange;              // Weight change magnitude
    double learningRate;              // Learning rate used
    double eligibilityTrace;          // Eligibility trace value
    Timestamp timestamp;              // Timestamp of recording
};

// Neuromodulation profiling
struct NeuromodulationProfile {
    Timestamp timestamp;              // Timestamp
    std::string neuromodulatorType;   // Type of neuromodulator
    double level;                     // Neuromodulator level
    double effectMagnitude;           // Effect magnitude on neural activity
    uint64_t targetNeuronId;          // Target neuron ID
    std::string targetRegion;        // Target region
    Timestamp previousReleaseTime;    // Previous release time
    double timeSincePrevious;         // Time since previous release
};

// Hotspot detection result
struct HotspotInfo {
    std::string location;             // File and line
    std::string functionName;         // Function name
    double cumulativeTime;            // Total time spent
    double percentageOfTotal;         // Percentage of total time
    int severity;                     // Severity level (1-5)
    std::string recommendation;       // Optimization recommendation
};

// Memory leak detection result
struct MemoryLeakInfo {
    std::string allocationSite;       // Where memory was allocated
    size_t leakedSize;                // Amount of memory leaked
    uint64_t allocationCount;         // Number of allocations
    double averageAllocationSize;     // Average allocation size
    double leakProbability;           // Probability of leak
    std::string suggestedFix;         // Suggested fix
};

class Profiler {
public:
    Profiler();
    ~Profiler();
    
    // Start/stop profiling
    void startProfiling();
    void stopProfiling();
    bool isProfiling() const { return isProfiling_; }
    
    // Line profiling
    void profileLine(const std::string& fileName, int lineNumber, 
                    const std::string& functionName, double duration,
                    uint64_t memoryAllocated, uint64_t functionCallCount);
    
    std::vector<LineProfileData> getLineProfileData() const;
    void clearLineProfileData();
    
    // Function profiling
    void profileFunctionEntry(const std::string& functionName, 
                            const std::string& fileName, int lineNumber);
    void profileFunctionExit(const std::string& functionName, double duration);
    
    std::vector<FunctionProfileData> getFunctionProfileData() const;
    void clearFunctionProfileData();
    
    // Memory allocation profiling
    void profileMemoryAllocation(void* address, size_t size,
                                const std::string& allocationSite);
    void profileMemoryDeallocation(void* address);
    
    std::vector<MemoryAllocationProfile> getMemoryAllocationProfile() const;
    void clearMemoryAllocationProfile();
    
    // Spike timing analysis
    void profileSpike(Timestamp spikeTime, uint64_t neuronId,
                     double membranePotential, const std::string& neuronType,
                     const std::string& region, Timestamp previousSpikeTime,
                     double synapticInput);
    
    std::vector<SpikeTimingProfile> getSpikeProfile() const;
    void clearSpikeProfile();
    
    // Plasticity event profiling
    void profilePlasticityEvent(Timestamp eventTime,
                               const std::string& plasticityType,
                               uint64_t synapseId, uint64_t preNeuronId,
                               uint64_t postNeuronId, double weightChange,
                               double learningRate, double eligibilityTrace);
    
    std::vector<PlasticityEventProfile> getPlasticityProfile() const;
    void clearPlasticityProfile();
    
    // Neuromodulation profiling
    void profileNeuromodulation(Timestamp timestamp,
                                const std::string& neuromodulatorType,
                                double level, double effectMagnitude,
                                uint64_t targetNeuronId, 
                                const std::string& targetRegion,
                                Timestamp previousReleaseTime);
    
    std::vector<NeuromodulationProfile> getNeuromodulationProfile() const;
    void clearNeuromodulationProfile();
    
    // Analysis methods
    std::vector<HotspotInfo> detectHotspots() const;
    std::vector<MemoryLeakInfo> detectMemoryLeaks() const;
    
    // Export/import profiling data
    void exportProfile(const std::string& filename) const;
    void importProfile(const std::string& filename);
    
    // Statistics
    size_t getLineProfileCount() const;
    size_t getFunctionProfileCount() const;
    size_t getMemoryAllocationCount() const;
    size_t getSpikeProfileCount() const;
    size_t getPlasticityEventCount() const;
    size_t getNeuromodulationEventCount() const;
    double getTotalProfileTime() const;
    
    // Reset all profiling data
    void resetProfile();
    
private:
    bool isProfiling_;
    Timestamp startTime_;
    
    // Profiling data
    std::vector<LineProfileData> lineProfileData_;
    std::vector<FunctionProfileData> functionProfileData_;
    std::vector<MemoryAllocationProfile> memoryAllocationProfile_;
    std::vector<SpikeTimingProfile> spikeProfile_;
    std::vector<PlasticityEventProfile> plasticityProfile_;
    std::vector<NeuromodulationProfile> neuromodulationProfile_;
    
    // Call stack tracking for memory profiling
    std::stack<std::string> callStack_;
    mutable std::mutex mutex_;
    
    // Statistics
    double totalProfileTime_;
    
    // Helper methods
    void updateFunctionCallersCallees(const std::string& caller, const std::string& callee);
    void calculateFunctionStatistics(FunctionProfileData& funcData) const;
    void calculateLineStatistics(LineProfileData& lineData) const;
};

#endif // NLM_PROFILER_HPP