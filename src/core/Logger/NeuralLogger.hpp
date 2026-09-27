#pragma once

#include "Logger.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>

namespace nlm {

// Neural-specific logging configuration
struct NeuralLogConfig {
    bool enableSpikeLogging = true;
    bool enableSynapseChangeLogging = true;
    bool enablePlasticityLogging = true;
    bool enableMemoryUpdateLogging = true;
    bool enableDevelopmentLogging = true;
    
    // Spike logging options
    bool logSpikeThresholds = true;
    bool logSpikeTiming = true;
    bool logSpikeFrequency = true;
    
    // Synapse options
    bool logWeightChanges = true;
    bool logConnectionDetails = true;
    bool logPlasticityMechanisms = true;
    
    // Plasticity options
    bool logSTDP = true;
    bool logHebbian = true;
    bool logStructural = true;
    
    // Memory options
    bool logMemoryAllocation = true;
    bool logMemoryDeallocation = true;
    bool logMemoryUsage = true;
    bool logMemoryThresholds = true;
    
    // Development options
    bool logDevelopmentalStages = true;
    bool logDifferentiation = true;
    bool logMaturation = true;
};

// Neural event logger with enhanced neural-specific functionality
class NeuralLogger {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    NeuralLogger();
    ~NeuralLogger();
    
    // Disable copying
    NeuralLogger(const NeuralLogger&) = delete;
    NeuralLogger& operator=(const NeuralLogger&) = delete;
    
    // Configuration
    void configure(const NeuralLogConfig& config);
    NeuralLogConfig getConfig() const;
    
    // Spike logging
    void logSpike(const std::string& neuronId, float voltage, float threshold,
                 double timeStamp, const std::string& regionId = "",
                 const std::string& populationId = "",
                 const std::unordered_map<std::string, std::string>& metadata = {});
    
    void logSubthresholdActivity(const std::string& neuronId, float voltage,
                                double timeStamp, const std::string& regionId = "");
    
    void logBurstActivity(const std::vector<std::string>& neuronIds,
                         const std::vector<float>& voltages,
                         double startTime, double duration,
                         const std::string& regionId = "");
    
    // Synapse logging
    void logSynapseCreation(const std::string& synapseId,
                           const std::string& preNeuronId,
                           const std::string& postNeuronId,
                           float initialWeight, float maxWeight,
                           double timeStamp, const std::string& regionId = "");
    
    void logSynapseElimination(const std::string& synapseId,
                              const std::string& preNeuronId,
                              const std::string& postNeuronId,
                              double timeStamp, const std::string& regionId = "");
    
    void logWeightChange(const std::string& synapseId,
                        const std::string& preNeuronId,
                        const std::string& postNeuronId,
                        float oldWeight, float newWeight,
                        float delta, const std::string& plasticityType,
                        double timeStamp, double duration,
                        const std::string& regionId = "");
    
    // Plasticity logging
    void logSTDPUpdate(const std::string& preNeuronId,
                      const std::string& postNeuronId,
                      float preSpikeTime, float postSpikeTime,
                      float weightChange, double timeStamp,
                      const std::string& regionId = "");
    
    void logHebbianUpdate(const std::string& neuronId,
                         const std::vector<std::string>& preSynapticNeurons,
                         float learningRate, double timeStamp,
                         const std::string& regionId = "");
    
    void logStructuralPlasticity(const std::string& neuronId,
                                const std::string& targetNeuronId,
                                const std::string& connectionType,
                                double growthRate, double timeStamp,
                                const std::string& regionId = "");
    
    // Memory logging
    void logMemoryAllocation(const std::string& neuronId,
                            size_t size, const std::string& memoryType,
                            double timeStamp, const std::string& regionId = "");
    
    void logMemoryDeallocation(const std::string& neuronId,
                              size_t size, const std::string& memoryType,
                              double timeStamp, const std::string& regionId = "");
    
    void logMemoryUsage(const std::string& neuronId,
                       size_t usedBytes, size_t totalBytes,
                       double timeStamp, const std::string& regionId = "");
    
    void logMemoryLeak(const std::string& neuronId,
                     size_t leakedBytes, double timeStamp,
                     const std::string& regionId = "");
    
    // Development logging
    void logDevelopmentalStage(const std::string& neuronId,
                               const std::string& regionId,
                               const std::string& stage,
                               double timeStamp,
                               const std::unordered_map<std::string, std::string>& metadata = {});
    
    void logDifferentiation(const std::string& progenitorNeuronId,
                           const std::vector<std::string>& daughterNeuronIds,
                           const std::string& regionId,
                           double timeStamp);
    
    void logMaturation(const std::string& neuronId,
                      const std::string& regionId,
                      float maturityLevel,
                      double timeStamp);
    
    // Neural network event logging
    void logNetworkEvent(const std::string& eventType,
                        const std::vector<std::string>& neuronIds,
                        const std::unordered_map<std::string, std::string>& properties,
                        double timeStamp, const std::string& regionId = "");
    
    void logConnectivityChange(const std::string& regionId,
                              const std::vector<std::pair<std::string, std::string>>& connections,
                              double timeStamp);
    
    // Get global neural logger instance
    static NeuralLogger& getGlobal();
    
    // Set global neural logger
    static void setGlobal(std::shared_ptr<NeuralLogger> logger);
};

// Convenience macros for neural logging
#define NLM_LOG_SPIKE(neuronId, voltage, threshold) \
    nlm::NeuralLogger::getGlobal().logSpike(neuronId, voltage, threshold, \
                                            std::chrono::duration<double>::zero(), "")

#define NLM_LOG_SYNAPSE_CHANGE(synapseId, pre, post, oldWeight, newWeight, delta, type) \
    nlm::NeuralLogger::getGlobal().logWeightChange(synapseId, pre, post, oldWeight, newWeight, delta, type, \
                                                   std::chrono::duration<double>::zero(), 0.0, "")

#define NLM_LOG_PLASTICITY(neuronId, type, magnitude) \
    nlm::NeuralLogger::getGlobal().logPlasticity(neuronId, "", type, magnitude, std::chrono::duration<double>::zero(), 0.0)

#define NLM_LOG_MEMORY(neuronId, delta, memoryType) \
    nlm::NeuralLogger::getGlobal().logMemoryUpdate(neuronId, delta, memoryType, std::chrono::duration<double>::zero(), 0.0)

#define NLM_LOG_DEVELOPMENT(neuronId, stage) \
    nlm::NeuralLogger::getGlobal().logDevelopmentalStage(neuronId, "", stage, std::chrono::duration<double>::zero())

} // namespace nlm

#endif // NEURAL_LOGGER_HPP