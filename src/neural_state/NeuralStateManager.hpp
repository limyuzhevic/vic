#include "../core/Types/Types.hpp"
#include <vector>
#include <unordered_map>

namespace nlm {

class NeuralStateManager {
public:
    // Singleton pattern for global state management
    static NeuralStateManager& getInstance() {
        static NeuralStateManager instance;
        return instance;
    }
    
    // Store neuron state by ID
    void storeNeuronState(NeuronId id, const NeuronState& state);
    
    // Retrieve neuron state by ID
    const NeuronState* getNeuronState(NeuronId id) const;
    
    // Update existing neuron state
    void updateNeuronState(NeuronId id, const NeuronState& state);
    
    // Remove neuron state
    void removeNeuronState(NeuronId id);
    
    // Store synapse state by ID
    void storeSynapseState(SynapseId id, const SynapseState& state);
    
    // Retrieve synapse state by ID
    const SynapseState* getSynapseState(SynapseId id) const;
    
    // Update existing synapse state
    void updateSynapseState(SynapseId id, const SynapseState& state);
    
    // Remove synapse state
    void removeSynapseState(SynapseId id);
    
    // Clear all states
    void clear();
    
    // Get number of stored neuron states
    size_t getNeuronStateCount() const { return neuronStates_.size(); }
    
    // Get number of stored synapse states
    size_t getSynapseStateCount() const { return synapseStates_.size(); }
    
    // Get all neuron states
    const std::unordered_map<NeuronId, NeuronState>& getAllNeuronStates() const { return neuronStates_; }
    
    // Get all synapse states
    const std::unordered_map<SynapseId, SynapseState>& getAllSynapseStates() const { return synapseStates_; }
    
    // Clean up expired states based on TTL
    void cleanupExpiredStates(Timestamp currentTime);
    
    // Set TTL for different types of states
    void setNeuronStateTTL(Timestamp ttl) { neuronStateTTL_ = ttl; }
    void setSynapseStateTTL(Timestamp ttl) { synapseStateTTL_ = ttl; }
    
    // Get TTL values
    Timestamp getNeuronStateTTL() const { return neuronStateTTL_; }
    Timestamp getSynapseStateTTL() const { return synapseStateTTL_; }
    
private:
    NeuralStateManager() : neuronStateTTL_(0), synapseStateTTL_(0) {}
    
    // Disallow copying
    NeuralStateManager(const NeuralStateManager&) = delete;
    NeuralStateManager& operator=(const NeuralStateManager&) = delete;
    
    // Neuron state storage
    std::unordered_map<NeuronId, NeuronState> neuronStates_;
    
    // Synapse state storage
    std::unordered_map<SynapseId, SynapseState> synapseStates_;
    
    // Time-to-live for states (0 means no expiration)
    Timestamp neuronStateTTL_;
    Timestamp synapseStateTTL_;
    
    // Last access times for TTL cleanup
    std::unordered_map<NeuronId, Timestamp> neuronLastAccess_;
    std::unordered_map<SynapseId, Timestamp> synapseLastAccess_;
};

} // namespace nlm