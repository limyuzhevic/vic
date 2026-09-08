#include "NeuralStateManager.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <iostream>

namespace nlm {

void NeuralStateManager::storeNeuronState(NeuronId id, const NeuronState& state) {
    // Remove existing state if present
    if (neuronStates_.find(id) != neuronStates_.end()) {
        neuronStates_.erase(id);
        neuronLastAccess_.erase(id);
    }
    
    neuronStates_[id] = state;
    neuronLastAccess_[id] = 0; // 0 means newly created
    
    if (neuronStateTTL_ > 0) {
        std::cout << "Stored neuron state for neuron " << id.index() << std::endl;
    }
}

void NeuralStateManager::updateNeuronState(NeuronId id, const NeuronState& state) {
    auto it = neuronStates_.find(id);
    if (it != neuronStates_.end()) {
        it->second = state;
        neuronLastAccess_[id] = 0; // Mark as updated
        if (neuronStateTTL_ > 0) {
            std::cout << "Updated neuron state for neuron " << id.index() << std::endl;
        }
    } else {
        std::cout << "Warning: Cannot update neuron state for unknown neuron " << id.index() << std::endl;
    }
}

void NeuralStateManager::removeNeuronState(NeuronId id) {
    neuronStates_.erase(id);
    neuronLastAccess_.erase(id);
    if (neuronStateTTL_ > 0) {
        std::cout << "Removed neuron state for neuron " << id.index() << std::endl;
    }
}

void NeuralStateManager::storeSynapseState(SynapseId id, const SynapseState& state) {
    // Remove existing state if present
    if (synapseStates_.find(id) != synapseStates_.end()) {
        synapseStates_.erase(id);
        synapseLastAccess_.erase(id);
    }
    
    synapseStates_[id] = state;
    synapseLastAccess_[id] = 0; // 0 means newly created
    
    if (synapseStateTTL_ > 0) {
        std::cout << "Stored synapse state for synapse " << id.index() << std::endl;
    }
}

void NeuralStateManager::updateSynapseState(SynapseId id, const SynapseState& state) {
    auto it = synapseStates_.find(id);
    if (it != synapseStates_.end()) {
        it->second = state;
        synapseLastAccess_[id] = 0; // Mark as updated
        if (synapseStateTTL_ > 0) {
            std::cout << "Updated synapse state for synapse " << id.index() << std::endl;
        }
    } else {
        std::cout << "Warning: Cannot update synapse state for unknown synapse " << id.index() << std::endl;
    }
}

void NeuralStateManager::removeSynapseState(SynapseId id) {
    synapseStates_.erase(id);
    synapseLastAccess_.erase(id);
    if (synapseStateTTL_ > 0) {
        std::cout << "Removed synapse state for synapse " << id.index() << std::endl;
    }
}

void NeuralStateManager::clear() {
    neuronStates_.clear();
    synapseStates_.clear();
    neuronLastAccess_.clear();
    synapseLastAccess_.clear();
    std::cout << "Cleared all neural states" << std::endl;
}

void NeuralStateManager::cleanupExpiredStates(Timestamp currentTime) {
    if (neuronStateTTL_ <= 0 && synapseStateTTL_ <= 0) {
        return; // No TTL cleanup needed
    }
    
    // Clean up expired neuron states
    if (neuronStateTTL_ > 0) {
        auto it = neuronLastAccess_.begin();
        while (it != neuronLastAccess_.end()) {
            if (currentTime - it->second > neuronStateTTL_) {
                neuronStates_.erase(it->first);
                it = neuronLastAccess_.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    // Clean up expired synapse states
    if (synapseStateTTL_ > 0) {
        auto it = synapseLastAccess_.begin();
        while (it != synapseLastAccess_.end()) {
            if (currentTime - it->second > synapseStateTTL_) {
                synapseStates_.erase(it->first);
                it = synapseLastAccess_.erase(it);
            } else {
                ++it;
            }
        }
    }
}

} // namespace nlm