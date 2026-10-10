#pragma once

#include "PerformanceProfiler.hpp"
#include <chrono>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <functional>

namespace nlm {

class SimulationProfiler : public PerformanceProfiler {
private:
    struct SimulationStepMetrics {
        std::chrono::high_resolution_clock::time_point timestamp;
        size_t stepNumber;
        double stepTime;
        double cpuTime;
        size_t memoryUsage;
        size_t activeNeurons;
        size_t activeSynapses;
        size_t spikesGenerated;
        double synapticActivity;
        double connectivityUsage;
        std::map<std::string, double> componentMetrics;
    };
    
    std::vector<SimulationStepMetrics> stepMetrics;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point stopTime;
    bool profilingActive;
    double samplingInterval;
    std::mutex dataMutex;
    std::atomic<bool> stopFlag;
    std::thread samplingThread;
    
    // Simulation state tracking
    size_t currentStep;
    size_t totalSteps;
    size_t neuronCount;
    size_t synapseCount;
    double totalSimulationTime;
    double peakStepTime;
    
    // Performance analysis
    struct SimulationPhaseMetrics {
        std::string phaseName;
        double startTime;
        double endTime;
        double duration;
        double cpuUsage;
        size_t memoryUsage;
        double performanceMetric;
        std::vector<SimulationStepMetrics> steps;
    };
    
    std::vector<SimulationPhaseMetrics> phaseMetrics;
    
    // Performance analysis functions
    void collectSimulationData();
    void samplingFunction();
    double calculateStepsPerSecond() const;
    double calculateMemoryPerStep() const;
    double calculateSpikeRate() const;
    double calculateSynapticEfficiency() const;
    double calculateNetworkStability() const;
    
public:
    SimulationProfiler();
    ~SimulationProfiler() override;
    
    // Profiling control
    void startProfiling() override;
    void stopProfiling() override;
    bool isProfiling() const override;
    
    // Performance metrics collection
    double getCpuUsage() const override;
    size_t getMemoryUsage() const override;
    double getThreadUsage() const override;
    double getIoUsage() const override;
    
    // Profiling data
    PerformanceMetrics getCurrentMetrics() const override;
    std::vector<PerformanceMetrics> getHistoricalData() const override;
    
    // Configuration
    void setSamplingInterval(double interval) override;
    double getSamplingInterval() const override;
    
    // Simulation-specific profiling
    void startCpuProfiling() override;
    void stopCpuProfiling() override;
    void startMemoryProfiling() override;
    void stopMemoryProfiling() override;
    void startThreadProfiling() override;
    void stopThreadProfiling() override;
    void startIoProfiling() override;
    void stopIoProfiling() override;
    
    // Simulation-specific methods
    void recordStep(size_t step, double stepTime, size_t neurons, size_t synapses, size_t spikes);
    void recordPhase(const std::string& phaseName, double duration);
    double getAverageStepTime() const;
    double getPeakStepTime() const;
    double getStepsPerSecond() const;
    double getMemoryPerStep() const;
    double getSpikeRate() const;
    double getSimulationEfficiency() const;
    std::vector<SimulationStepMetrics> getStepMetrics() const;
    std::vector<SimulationPhaseMetrics> getPhaseMetrics() const;
    
    // Performance analysis
    void analyzePerformanceTrends() const;
    void identifyBottlenecks() const;
    void analyzeScaleFactors() const;
    void predictPerformance(size_t targetNeurons) const;
    double calculateScaleEfficiency() const;
    double calculateSimulationStability() const;
    std::map<std::string, double> comparePhases() const;
    
    // Network-specific metrics
    double calculateNetworkActivity() const;
    double calculateConnectivityUtilization() const;
    size_t calculateActiveNeurons() const;
    size_t calculateActiveSynapses() const;
    double calculateSynapticActivity() const;
    std::map<std::string, double> getNetworkMetrics() const;
};

} // namespace nlm