#pragma once

#include "core/Types/Types.hpp"
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <memory>

namespace nlm {

// Neural activity data structure
struct NeuralActivity {
    std::vector<NeuronId> neurons;
    double activityLevel;
    Timestamp timestamp;
    std::string activityType; // "spike", "synapse", "population"
};

// Synaptic activity record
struct SynapticActivity {
    SynapseId synapseId;
    NeuronId preNeuronId;
    NeuronId postNeuronId;
    double weightChange;
    Timestamp timestamp;
    double efficacy;
};

// Network topology node (neuron or region)
struct NetworkNode {
    NeuronId neuronId;
    RegionId regionId;
    PopulationId populationId;
    std::string nodeType; // "neuron", "region", "population"
    double connectivity; // Total connectivity strength
    std::vector<SynapseHandle> outgoingSynapses;
    std::vector<SynapseHandle> incomingSynapses;
};

// NeuralMonitor - Neural activity monitoring class
class NeuralMonitor {
public:
    NeuralMonitor();
    ~NeuralMonitor();
    
    // Start/stop monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Spike monitoring
    void recordSpike(NeuronId neuronId, Timestamp timestamp);
    void recordSpike(NeuronId neuronId, Timestamp timestamp, double amplitude);
    
    // Population monitoring
    void recordPopulationActivity(const std::vector<NeuronId>& neurons, double activity);
    void recordPopulationActivity(const std::vector<NeuronId>& neurons, double activity, const std::string& populationName);
    
    // Synaptic tracking
    void recordSynapticActivity(const SynapticActivity& activity);
    void recordSynapticWeightChange(SynapseId synapseId, NeuronId preNeuronId, 
                                    NeuronId postNeuronId, double weightChange);
    
    // Network topology
    void updateNetworkNode(const NetworkNode& node);
    void addSynapseConnection(SynapseId synapseId, NeuronId preNeuronId, NeuronId postNeuronId);
    void removeSynapseConnection(SynapseId synapseId);
    
    // Statistics
    size_t getTotalSpikes() const;
    size_t getFiringNeurons() const;
    double getAverageFiringRate() const;
    size_t getActiveNeuronCount() const;
    size_t getRefractoryNeuronCount() const;
    
    // Activity history
    std::vector<NeuralActivity> getRecentActivity(size_t limit = 100) const;
    std::vector<NeuralActivity> getActivityByTimeRange(Timestamp startTime, 
                                                         Timestamp endTime) const;
    
    // Network analysis
    std::map<std::string, double> analyzeNetworkTopology() const;
    double calculateNetworkEfficiency() const;
    std::vector<NetworkNode> getHighDegreeNodes(size_t minDegree) const;
    
    // Synaptic analysis
    std::map<std::string, double> analyzeSynapticDynamics() const;
    double getAverageSynapticStrength() const;
    size_t getStrongestSynapse() const;
    
    // Performance metrics
    double getCurrentFiringRate() const;
    size_t getTotalConnections() const;
    double getNetworkActivityLevel() const;
    
    // Pattern detection
    std::vector<std::vector<NeuronId>> detectSynchronizedGroups(double correlationThreshold) const;
    std::vector<NeuronId> detectBurstNeurons(Timestamp windowDuration) const;
    
    // Recording control
    void clearActivityHistory();
    void setActivityRetentionTime(Timestamp retentionTime);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm