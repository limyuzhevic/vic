#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <memory>

namespace nlm {

// Performance metrics structure
struct PerformanceMetrics {
    // Time measurements
    double simulationTime;        // Total simulation time
    double stepTime;              // Average step time
    double cpuTime;               // CPU time used
    double wallClockTime;         // Wall clock time
    
    // Memory measurements
    size_t memoryUsed;            // Memory used in bytes
    size_t peakMemoryUsed;        // Peak memory used
    size_t totalMemoryAllocated;  // Total memory allocated
    size_t totalMemoryDeallocated; // Total memory deallocated
    
    // Network metrics
    size_t totalNeuronCount;      // Total neurons
    size_t totalSynapseCount;      // Total synapses
    size_t activeNeuronCount;     // Active neurons
    size_t firingNeuronCount;      // Firing neurons
    size_t spikeCount;            // Total spikes
    
    // Performance metrics
    double simulationSteps;      // Number of simulation steps
    double stepsPerSecond;        // Simulation speed
    double neuronsPerSecond;      // Neuron processing speed
    double synapsesPerSecond;     // Synapse processing speed
    
    // Plasticity metrics
    double stdpUpdates;          // STDP updates
    double hebbianUpdates;       // Hebbian updates
    double structuralPlasticityEvents; // Structural plasticity events
    
    PerformanceMetrics() 
        : simulationTime(0.0), stepTime(0.0), cpuTime(0.0), wallClockTime(0.0)
        , memoryUsed(0), peakMemoryUsed(0), totalMemoryAllocated(0), totalMemoryDeallocated(0)
        , totalNeuronCount(0), totalSynapseCount(0), activeNeuronCount(0)
        , firingNeuronCount(0), spikeCount(0), simulationSteps(0.0)
        , stepsPerSecond(0.0), neuronsPerSecond(0.0), synapsesPerSecond(0.0)
        , stdpUpdates(0.0), hebbianUpdates(0.0), structuralPlasticityEvents(0.0) {}
    
    std::string toString() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);
        ss << "=== Performance Metrics ===" << std::endl;
        ss << "Time:" << std::endl;
        ss << "  Simulation Time: " << simulationTime << "s" << std::endl;
        ss << "  Step Time: " << stepTime << "s" << std::endl;
        ss << "  CPU Time: " << cpuTime << "s" << std::endl;
        ss << "  Wall Clock: " << wallClockTime << "s" << std::endl;
        ss << "  Steps/sec: " << stepsPerSecond << std::endl;
        ss << std::endl;
        ss << "Memory:" << std::endl;
        ss << "  Used: " << (memoryUsed / 1024 / 1024) << "MB" << std::endl;
        ss << "  Peak: " << (peakMemoryUsed / 1024 / 1024) << "MB" << std::endl;
        ss << "  Allocated: " << (totalMemoryAllocated / 1024 / 1024) << "MB" << std::endl;
        ss << "  Deallocated: " << (totalMemoryDeallocated / 1024 / 1024) << "MB" << std::endl;
        ss << std::endl;
        ss << "Network:" << std::endl;
        ss << "  Neurons: " << totalNeuronCount << std::endl;
        ss << "  Synapses: " << totalSynapseCount << std::endl;
        ss << "  Active: " << activeNeuronCount << std::endl;
        ss << "  Firing: " << firingNeuronCount << std::endl;
        ss << "  Spikes: " << spikeCount << std::endl;
        ss << std::endl;
        ss << "Performance:" << std::endl;
        ss << "  Steps/sec: " << stepsPerSecond << std::endl;
        ss << "  Neurons/sec: " << neuronsPerSecond << std::endl;
        ss << "  Synapses/sec: " << synapsesPerSecond << std::endl;
        ss << std::endl;
        ss << "Plasticity:" << std::endl;
        ss << "  STDP Updates: " << stdpUpdates << std::endl;
        ss << "  Hebbian Updates: " << hebbianUpdates << std::endl;
        ss << "  Structural Plasticity Events: " << structuralPlasticityEvents << std::endl;
        
        return ss.str();
    }
};

// Performance monitoring system
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start/stop monitoring
    void start();
    void stop();
    bool isMonitoring() const;
    
    // Configure monitoring
    void setUpdateInterval(double interval); // seconds
    void setMonitoringCPU(bool enable);
    void setMonitoringMemory(bool enable);
    void setMonitoringNetwork(bool enable);
    void setMonitoringPlasticity(bool enable);
    
    // Get current metrics
    PerformanceMetrics getCurrentMetrics() const;
    
    // Get metrics history
    std::vector<PerformanceMetrics> getMetricsHistory(size_t count) const;
    
    // Benchmark operations
    void benchmarkMemoryAllocation(size_t size);
    void benchmarkSpikeProcessing(size_t spikeCount);
    void benchmarkSynapseProcessing(size_t synapseCount);
    
    // Reset metrics
    void reset();
    
    // Check if monitoring is active
    bool isActive() const;
    
    // Get performance summary
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    void updateMetrics();
    void recordStepTime(double time);
    void recordMemoryUsage();
    void recordNetworkMetrics();
    void recordPlasticityMetrics();
};

} // namespace nlm