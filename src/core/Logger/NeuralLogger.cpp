#include "NeuralLogger.hpp"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <stdexcept>
#include <chrono>
#include <cmath>

namespace nlm {

struct NeuralLogger::Impl {
    NeuralLogConfig config;
    std::mutex mutex;
    
    // Neural event statistics
    struct EventStats {
        size_t spikeCount = 0;
        size_t synapseChangeCount = 0;
        size_t plasticityCount = 0;
        size_t memoryUpdateCount = 0;
        size_t developmentCount = 0;
        double totalSpikeVoltage = 0.0;
        double totalWeightChange = 0.0;
        double totalPlasticityMagnitude = 0.0;
    };
    
    EventStats stats;
    
    // Performance tracking
    struct PerformanceData {
        std::chrono::system_clock::time_point startTime;
        double totalTimeMs = 0.0;
        size_t operationCount = 0;
    };
    
    std::unordered_map<std::string, PerformanceData> performanceData;
};

NeuralLogger::NeuralLogger() : pImpl(std::make_unique<Impl>()) {
    // Initialize default config
    pImpl->config = NeuralLogConfig();
}

NeuralLogger::~NeuralLogger() = default;

void NeuralLogger::configure(const NeuralLogConfig& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->config = config;
}

NeuralLogConfig NeuralLogger::getConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->config;
}

void NeuralLogger::logSpike(const std::string& neuronId, float voltage, float threshold,
                           double timeStamp, const std::string& regionId,
                           const std::string& populationId,
                           const std::unordered_map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableSpikeLogging) return;
    
    std::ostringstream oss;
    oss << "Spike in neuron " << neuronId << " (V=" << voltage << "V, threshold=" << threshold << "V)";
    
    // Log using enhanced logger
    EnhancedLogger::getGlobal().log(LogLevel::Spike, oss.str(), "", 0, "", metadata);
    
    // Update statistics
    pImpl->stats.spikeCount++;
    pImpl->stats.totalSpikeVoltage += voltage;
    
    // Log subthreshold activity if threshold exceeded
    if (pImpl->config.logSpikeThresholds && voltage >= threshold) {
        EnhancedLogger::getGlobal().log(LogLevel::Warning, "Spike threshold exceeded: " + neuronId, "", 0, "", 
                                      {{ "neuron_id", neuronId }, { "voltage", std::to_string(voltage) }, 
                                       { "threshold", std::to_string(threshold) }});
    }
}

void NeuralLogger::logSubthresholdActivity(const std::string& neuronId, float voltage,
                                          double timeStamp, const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.logSpikeThresholds) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Info, "Subthreshold activity in neuron " + neuronId, "", 0, "",
                                   {{ "neuron_id", neuronId }, { "voltage", std::to_string(voltage) },
                                    { "region_id", regionId }, { "time", std::to_string(timeStamp) }});
}

void NeuralLogger::logBurstActivity(const std::vector<std::string>& neuronIds,
                                    const std::vector<float>& voltages,
                                    double startTime, double duration,
                                    const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.logSpikeFrequency) return;
    
    double avgVoltage = 0.0;
    for (float v : voltages) avgVoltage += v;
    avgVoltage /= voltages.size();
    
    EnhancedLogger::getGlobal().log(LogLevel::Info, "Burst activity detected", "", 0, "",
                                   {{ "neuron_count", std::to_string(neuronIds.size()) },
                                    { "avg_voltage", std::to_string(avgVoltage) },
                                    { "duration", std::to_string(duration) },
                                    { "region_id", regionId }});
}

void NeuralLogger::logSynapseCreation(const std::string& synapseId,
                                     const std::string& preNeuronId,
                                     const std::string& postNeuronId,
                                     float initialWeight, float maxWeight,
                                     double timeStamp, const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableSynapseChangeLogging) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::SynapseChange, "New synapse created", "", 0, "",
                                   {{ "synapse_id", synapseId },
                                    { "pre_neuron", preNeuronId },
                                    { "post_neuron", postNeuronId },
                                    { "initial_weight", std::to_string(initialWeight) },
                                    { "max_weight", std::to_string(maxWeight) },
                                    { "region_id", regionId },
                                    { "time", std::to_string(timeStamp) }});
}

void NeuralLogger::logSynapseElimination(const std::string& synapseId,
                                        const std::string& preNeuronId,
                                        const std::string& postNeuronId,
                                        double timeStamp, const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.enableSynapseChangeLogging) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::SynapseChange, "Synapse eliminated", "", 0, "",
                                   {{ "synapse_id", synapseId },
                                    { "pre_neuron", preNeuronId },
                                    { "post_neuron", postNeuronId },
                                    { "region_id", regionId },
                                    { "time", std::to_string(timeStamp) }});
}

void NeuralLogger::logWeightChange(const std::string& synapseId,
                                   const std::string& preNeuronId,
                                   const std::string& postNeuronId,
                                   float oldWeight, float newWeight,
                                   float delta, const std::string& plasticityType,
                                   double timeStamp, double duration,
                                   const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.logWeightChanges) return;
    
    std::ostringstream oss;
    oss << "Synaptic weight change: " << synapseId << " (" << preNeuronId << " -> " << postNeuronId
        << ") weight: " << oldWeight << " -> " << newWeight << " (Δ=" << delta << ")";
    
    EnhancedLogger::getGlobal().log(LogLevel::SynapseChange, oss.str(), "", 0, "",
                                   {{ "synapse_id", synapseId },
                                    { "pre_neuron", preNeuronId },
                                    { "post_neuron", postNeuronId },
                                    { "old_weight", std::to_string(oldWeight) },
                                    { "new_weight", std::to_string(newWeight) },
                                    { "weight_change", std::to_string(delta) },
                                    { "plasticity_type", plasticityType },
                                    { "duration_ms", std::to_string(duration) },
                                    { "region_id", regionId }});
    
    pImpl->stats.synapseChangeCount++;
    pImpl->stats.totalWeightChange += std::abs(delta);
}

void NeuralLogger::logSTDPUpdate(const std::string& preNeuronId,
                                const std::string& postNeuronId,
                                float preSpikeTime, float postSpikeTime,
                                float weightChange, double timeStamp,
                                const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.logSTDP) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Plasticity, "STDP update", "", 0, "",
                                   {{ "pre_neuron", preNeuronId },
                                    { "post_neuron", postNeuronId },
                                    { "pre_spike_time", std::to_string(preSpikeTime) },
                                    { "post_spike_time", std::to_string(postSpikeTime) },
                                    { "weight_change", std::to_string(weightChange) },
                                    { "time", std::to_string(timeStamp) },
                                    { "region_id", regionId }});
}

void NeuralLogger::logHebbianUpdate(const std::string& neuronId,
                                   const std::vector<std::string>& preSynapticNeurons,
                                   float learningRate, double timeStamp,
                                   const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.logHebbian) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Plasticity, "Hebbian update", "", 0, "",
                                   {{ "neuron_id", neuronId },
                                    { "pre_synaptic_count", std::to_string(preSynapticNeurons.size()) },
                                    { "learning_rate", std::to_string(learningRate) },
                                    { "time", std::to_string(timeStamp) },
                                    { "region_id", regionId }});
}

void NeuralLogger::logStructuralPlasticity(const std::string& neuronId,
                                          const std::string& targetNeuronId,
                                          const std::string& connectionType,
                                          double growthRate, double timeStamp,
                                          const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.logStructural) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Plasticity, "Structural plasticity", "", 0, "",
                                   {{ "neuron_id", neuronId },
                                    { "target_neuron", targetNeuronId },
                                    { "connection_type", connectionType },
                                    { "growth_rate", std::to_string(growthRate) },
                                    { "time", std::to_string(timeStamp) },
                                    { "region_id", regionId }});
}

void NeuralLogger::logMemoryAllocation(const std::string& neuronId,
                                       size_t size, const std::string& memoryType,
                                       double timeStamp, const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.logMemoryAllocation) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::MemoryUpdate, "Memory allocated", "", 0, "",
                                   {{ "neuron_id", neuronId },
                                    { "size", std::to_string(size) },
                                    { "memory_type", memoryType },
                                    { "time", std::to_string(timeStamp) },
                                    { "region_id", regionId }});
}

void NeuralLogger::logMemoryDeallocation(const std::string& neuronId,
                                        size_t size, const std::string& memoryType,
                                        double timeStamp, const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.logMemoryDeallocation) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::MemoryUpdate, "Memory deallocated", "", 0, "",
                                   {{ "neuron_id", neuronId },
                                    { "size", std::to_string(size) },
                                    { "memory_type", memoryType },
                                    { "time", std::to_string(timeStamp) },
                                    { "region_id", regionId }});
}

void NeuralLogger::logMemoryUsage(const std::string& neuronId,
                                 size_t usedBytes, size_t totalBytes,
                                 double timeStamp, const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.logMemoryUsage) return;
    
    double usagePercent = totalBytes > 0 ? (static_cast<double>(usedBytes) / totalBytes) * 100.0 : 0.0;
    
    EnhancedLogger::getGlobal().log(LogLevel::MemoryUpdate, "Memory usage update", "", 0, "",
                                   {{ "neuron_id", neuronId },
                                    { "used_bytes", std::to_string(usedBytes) },
                                    { "total_bytes", std::to_string(totalBytes) },
                                    { "usage_percent", std::to_string(usagePercent) },
                                    { "time", std::to_string(timeStamp) },
                                    { "region_id", regionId }});
    
    pImpl->stats.memoryUpdateCount++;
}

void NeuralLogger::logMemoryLeak(const std::string& neuronId,
                                size_t leakedBytes, double timeStamp,
                                const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.detectMemoryLeaks) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Critical, "Memory leak detected", "", 0, "",
                                   {{ "neuron_id", neuronId },
                                    { "leaked_bytes", std::to_string(leakedBytes) },
                                    { "time", std::to_string(timeStamp) },
                                    { "region_id", regionId }});
}

void NeuralLogger::logDevelopmentalStage(const std::string& neuronId,
                                         const std::string& regionId,
                                         const std::string& stage,
                                         double timeStamp,
                                         const std::unordered_map<std::string, std::string>& metadata) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.logDevelopmentalStages) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Development, "Developmental stage", "", 0, "",
                                   {{ "neuron_id", neuronId },
                                    { "region_id", regionId },
                                    { "stage", stage },
                                    { "time", std::to_string(timeStamp) }});
    
    pImpl->stats.developmentCount++;
}

void NeuralLogger::logDifferentiation(const std::string& progenitorNeuronId,
                                     const std::vector<std::string>& daughterNeuronIds,
                                     const std::string& regionId,
                                     double timeStamp) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.logDifferentiation) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Development, "Differentiation", "", 0, "",
                                   {{ "progenitor_neuron", progenitorNeuronId },
                                    { "daughter_count", std::to_string(daughterNeuronIds.size()) },
                                    { "region_id", regionId },
                                    { "time", std::to_string(timeStamp) }});
}

void NeuralLogger::logMaturation(const std::string& neuronId,
                                const std::string& regionId,
                                float maturityLevel,
                                double timeStamp) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->config.logMaturation) return;
    
    EnhancedLogger::getGlobal().log(LogLevel::Development, "Maturation", "", 0, "",
                                   {{ "neuron_id", neuronId },
                                    { "region_id", regionId },
                                    { "maturity_level", std::to_string(maturityLevel) },
                                    { "time", std::to_string(timeStamp) }});
}

void NeuralLogger::logNetworkEvent(const std::string& eventType,
                                  const std::vector<std::string>& neuronIds,
                                  const std::unordered_map<std::string, std::string>& properties,
                                  double timeStamp, const std::string& regionId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogger::getGlobal().log(LogLevel::NeuralEvent, "Network event", "", 0, "",
                                   {{ "event_type", eventType },
                                    { "neuron_count", std::to_string(neuronIds.size()) },
                                    { "region_id", regionId },
                                    { "time", std::to_string(timeStamp) }});
}

void NeuralLogger::logConnectivityChange(const std::string& regionId,
                                        const std::vector<std::pair<std::string, std::string>>& connections,
                                        double timeStamp) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    EnhancedLogger::getGlobal().log(LogLevel::NeuralEvent, "Connectivity change", "", 0, "",
                                   {{ "region_id", regionId },
                                    { "connection_count", std::to_string(connections.size()) },
                                    { "time", std::to_string(timeStamp) }});
}

NeuralLogger& NeuralLogger::getGlobal() {
    static NeuralLogger globalLogger;
    return globalLogger;
}

void NeuralLogger::setGlobal(std::shared_ptr<NeuralLogger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

} // namespace nlm
