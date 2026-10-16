#pragma once

#include "NeuralProfiler.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace nlm {

// Neural network performance metrics
struct NeuralNetworkPerformance {
    std::string componentName;
    std::string operationName;
    std::chrono::nanoseconds executionTime;
    std::chrono::nanoseconds neuralForwardPassTime;
    std::chrono::nanoseconds neuralBackwardPassTime;
    std::chrono::nanoseconds weightUpdateTime;
    std::chrono::nanoseconds memoryTransferTime;
    std::chrono::nanoseconds activationComputeTime;
    size_t inputSize;
    size_t outputSize;
    size_t neuronsProcessed;
    size_t synapsesProcessed;
    float forwardPassAccuracy;
    float backwardPassAccuracy;
    float gradientNorm;
    float weightNorm;
    std::chrono::system_clock::time_point timestamp;
    
    NeuralNetworkPerformance(const std::string& comp, const std::string& op)
        : componentName(comp), operationName(op), executionTime(0),
          neuralForwardPassTime(0), neuralBackwardPassTime(0), weightUpdateTime(0),
          memoryTransferTime(0), activationComputeTime(0), inputSize(0), outputSize(0),
          neuronsProcessed(0), synapsesProcessed(0), forwardPassAccuracy(0.0f),
          backwardPassAccuracy(0.0f), gradientNorm(0.0f), weightNorm(0.0f) {}
};

// Neural performance profiling configuration
struct NeuralProfilingConfig {
    bool enableNeuralNetworkProfiling;      // Enable neural network performance profiling
    bool enableNeuronProfiling;             // Enable per-neuron performance tracking
    bool enableSynapseProfiling;            // Enable per-synapse performance tracking
    bool enableActivationProfiling;         // Enable activation function profiling
    bool enableGradientProfiling;           // Enable gradient computation profiling
    bool enableMemoryBandwidthProfiling;    // Enable memory bandwidth profiling
    bool enableOptimizationSuggestions;     // Enable automatic optimization suggestions
    bool enableRealTimeNeuralMonitoring;     // Enable real-time neural performance monitoring
    size_t maxNeuralProfiles;               // Maximum number of neural profiles
    size_t maxNeuronProfiles;               // Maximum number of neuron profiles
    size_t maxSynapseProfiles;              // Maximum number of synapse profiles
    double neuronPerformanceThreshold;      // Neuron performance threshold
    double synapsePerformanceThreshold;     // Synapse performance threshold
    size_t profilingIntervalMs;             // Profiling interval in milliseconds
    
    NeuralProfilingConfig()
        : enableNeuralNetworkProfiling(true), enableNeuronProfiling(true),
          enableSynapseProfiling(true), enableActivationProfiling(true),
          enableGradientProfiling(true), enableMemoryBandwidthProfiling(true),
          enableOptimizationSuggestions(true), enableRealTimeNeuralMonitoring(true),
          maxNeuralProfiles(1000), maxNeuronProfiles(10000), maxSynapseProfiles(100000),
          neuronPerformanceThreshold(1.0), synapsePerformanceThreshold(0.01),
          profilingIntervalMs(1000) {}
};

// Neuron performance profile
struct NeuronPerformance {
    std::string neuronId;
    std::string regionName;
    std::string populationName;
    std::chrono::nanoseconds totalExecutionTime;
    std::chrono::nanoseconds spikeProcessingTime;
    std::chrono::nanoseconds synapticInputTime;
    std::chrono::nanoseconds membranePotentialTime;
    std::chrono::nanoseconds refractoryTime;
    size_t spikeCount;
    float averageFiringRate;
    float maxFiringRate;
    float averageMembranePotential;
    float maxMembranePotential;
    float minMembranePotential;
    std::chrono::system_clock::time_point lastUpdateTime;
    
    NeuronPerformance(const std::string& id, const std::string& region, const std::string& population)
        : neuronId(id), regionName(region), populationName(population),
          totalExecutionTime(0), spikeProcessingTime(0), synapticInputTime(0),
          membranePotentialTime(0), refractoryTime(0), spikeCount(0),
          averageFiringRate(0.0f), maxFiringRate(0.0f), averageMembranePotential(0.0f),
          maxMembranePotential(0.0f), minMembranePotential(0.0f) {}
};

// Synapse performance profile
struct SynapsePerformance {
    std::string synapseId;
    std::string sourceNeuronId;
    std::string destinationNeuronId;
    std::chrono::nanoseconds totalExecutionTime;
    std::chrono::nanoseconds transmissionTime;
    std::chrono::nanoseconds plasticityUpdateTime;
    std::chrono::nanoseconds eligibilityTraceTime;
    size_t spikeTransmissionCount;
    float averageWeightChange;
    float maxWeightChange;
    float minWeightChange;
    float averageEfficacy;
    float averageDepression;
    float averageFacilitation;
    std::chrono::system_clock::time_point lastUpdateTime;
    
    SynapsePerformance(const std::string& id, const std::string& source, const std::string& destination)
        : synapseId(id), sourceNeuronId(source), destinationNeuronId(destination),
          totalExecutionTime(0), transmissionTime(0), plasticityUpdateTime(0),
          eligibilityTraceTime(0), spikeTransmissionCount(0), averageWeightChange(0.0f),
          maxWeightChange(0.0f), minWeightChange(0.0f), averageEfficacy(0.0f),
          averageDepression(0.0f), averageFacilitation(0.0f) {}
};

// Neural performance profiler implementation
class NeuralProfiler : public INeuralProfiler {
private:
    NeuralProfilingConfig config_;
    std::atomic<bool> isRunning_{false};
    std::thread monitoringThread_;
    
    // Neural performance tracking
    std::unordered_map<std::string, NeuralNetworkPerformance> neuralNetworkPerformances_;
    std::unordered_map<std::string, NeuronPerformance> neuronPerformances_;
    std::unordered_map<std::string, SynapsePerformance> synapsePerformances_;
    mutable std::mutex neuralMutex_;
    mutable std::mutex neuronMutex_;
    mutable std::mutex synapseMutex_;
    
    // Current neural metrics
    size_t totalNeurons_{0};
    size_t totalSynapses_{0};
    size_t activeNeurons_{0};
    size_t firingNeurons_{0};
    float averageNetworkActivity_{0.0f};
    mutable std::mutex metricsMutex_;
    
    // Monitoring state
    std::chrono::system_clock::time_point lastMonitoringTime_;
    size_t monitoringCount_ = 0;
    
public:
    NeuralProfiler();
    ~NeuralProfiler();
    
    // INeuralProfiler implementation
    void start() override;
    void stop() override;
    bool isRunning() const override;
    
    void recordNeuronPerformance(const std::string& neuronId, const std::string& region,
                                const std::string& population, const std::chrono::nanoseconds& duration,
                                size_t spikeCount, float firingRate, float membranePotential) override;
    void recordSynapsePerformance(const std::string& synapseId, const std::string& sourceNeuron,
                                 const std::string& destinationNeuron, const std::chrono::nanoseconds& duration,
                                 float weightChange, float efficacy) override;
    void recordNeuralNetworkPerformance(const std::string& componentName, const std::string& operationName,
                                       const std::chrono::nanoseconds& totalTime,
                                       const std::chrono::nanoseconds& forwardPassTime,
                                       const std::chrono::nanoseconds& backwardPassTime,
                                       float accuracy, size_t inputSize, size_t outputSize) override;
    
    std::vector<NeuralNetworkPerformance> getNeuralNetworkPerformances() const override;
    std::vector<NeuronPerformance> getNeuronPerformances() const override;
    std::vector<SynapsePerformance> getSynapsePerformances() const override;
    
    std::string generateHtmlReport() const override;
    std::string generateJsonReport() const override;
    std::string generateTextReport() const override;
    
    // Configuration management
    void setConfig(const NeuralProfilingConfig& config);
    NeuralProfilingConfig getConfig() const;
    
    // Neural system monitoring
    void updateNeuralMetrics(size_t totalNeurons, size_t totalSynapses, size_t activeNeurons,
                            size_t firingNeurons, float averageActivity);
    
    // Performance analysis
    void analyzeNeuronPerformance(NeuronPerformance& neuron) const;
    void analyzeSynapsePerformance(SynapsePerformance& synapse) const;
    void analyzeNeuralNetworkPerformance(NeuralNetworkPerformance& performance) const;
    
    // Report generation helpers
    std::string getCurrentTimestamp() const;
    double calculatePerformanceScore(const NeuronPerformance& neuron) const;
    double calculateSynapseEfficiency(const SynapsePerformance& synapse) const;
};

// Neural system metrics aggregation
struct NeuralSystemMetrics {
    size_t totalNeurons;
    size_t totalSynapses;
    size_t activeNeurons;
    size_t firingNeurons;
    float averageNetworkActivity;
    double averageNeuronPerformance;
    double averageSynapseEfficiency;
    std::chrono::system_clock::time_point timestamp;
};