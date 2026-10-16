#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace nlm {

// Forward declarations
class Communicator;
class LoadBalancer;
class CheckpointManager;
class FaultTolerance;
class DistributedNeuron;
class DistributedSynapse;
class PartitionManager;

// Distributed brain configuration
struct DistributedConfig {
    bool enableDistributedMode = false;
    int nodeId = 0;
    int totalNodes = 1;
    std::string mpiPort = "12345";
    float communicationInterval = 0.001f;  // 1ms
    float loadBalancingInterval = 0.1f;    // 100ms
    bool enableFaultTolerance = true;
    bool enableCheckpointing = true;
    std::string checkpointDir = "./distributed_checkpoints";
    float networkBandwidth = 1000.0f;      // MB/s
    float latency = 0.001f;                // seconds
    int maxBufferedEvents = 1000;
};

// Distributed brain class
class DistributedBrain {
public:
    // Create distributed brain with configuration
    explicit DistributedBrain(std::shared_ptr<Config> config);
    
    ~DistributedBrain();
    
    // Disable copying, enable moving
    DistributedBrain(const DistributedBrain&) = delete;
    DistributedBrain& operator=(const DistributedBrain&) = delete;
    DistributedBrain(DistributedBrain&&) noexcept;
    DistributedBrain& operator=(DistributedBrain&&) noexcept;
    
    // Initialize distributed brain with MPI support
    bool initialize(bool useMpi = false);
    
    // Main distributed simulation step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment (local node)
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron (local node)
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type (local node)
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Distributed operation: request neural data from other nodes
    std::vector<float> requestNeuralData(int sourceNode, const std::vector<int>& neuronIds);
    
    // Distributed operation: broadcast spike events to other nodes
    void broadcastSpikeEvents(const std::vector<SpikeEvent>& events);
    
    // Distributed operation: synchronize neural state across nodes
    void synchronizeState();
    
    // Get node information
    int getNodeId() const { return pImpl->config.nodeId; }
    int getTotalNodes() const { return pImpl->config.totalNodes; }
    bool isDistributedMode() const { return pImpl->config.enableDistributedMode; }
    
    // Access underlying brain (for backward compatibility)
    Brain* getUnderlyingBrain() { return pImpl->brain.get(); }
    const Brain* getUnderlyingBrain() const { return pImpl->brain.get(); }
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals (local node)
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset distributed brain state
    void reset();
    
    // Save distributed brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load distributed brain state from file
    bool load(const std::string& filepath);
    
    // Configuration
    void setDistributedConfig(const DistributedConfig& config);
    const DistributedConfig& getDistributedConfig() const;
    
    // Node management
    void addNode(int nodeId, const std::string& host, int port);
    void removeNode(int nodeId);
    bool isNodeActive(int nodeId) const;
    std::vector<int> getActiveNodes() const;
    
    // Performance metrics
    float getAverageCommunicationTime() const;
    float getAverageSynchronizationTime() const;
    size_t getBufferedEventsCount() const;
    
    // Monitoring
    void logStatus() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
