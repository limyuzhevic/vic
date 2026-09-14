#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <unordered_set>

namespace nlm {

struct WorkingMemoryTrace {
    float activation;
    SimulationStep timestamp;
    bool isActive;
    
    WorkingMemoryTrace() : activation(0.0f), timestamp(0), isActive(true) {}
};

// NeuralWorkingMemory Implementation

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Efficient storage: neuron -> trace mapping
    std::unordered_map<NeuronId, WorkingMemoryTrace> traces;
    std::priority_queue<std::pair<float, NeuronId>> priorityQueue;  // For winner-take-all
    
    // Legacy storage for backward compatibility
    std::vector<NeuronId> legacyMemoryNeurons;
    std::vector<float> legacyMemoryActivations;
    std::vector<SimulationStep> legacyMemoryTimestamps;
    
    // Maintenance connections (recurrent)
    std::unordered_map<std::pair<NeuronId, NeuronId>, float> maintenanceSynapses;
    
    Impl() : brain(nullptr) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Update trace efficiently
        auto it = pImpl->traces.find(neuron);
        if (it != pImpl->traces.end()) {
            it->second.activation = activation;
            it->second.timestamp = 0;
            it->second.isActive = true;
        } else {
            pImpl->traces[neuron] = WorkingMemoryTrace{activation, 0, true};
        }
        
        // Set neuron activation
        if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
            for (auto* nn : *n) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * 5.0f);
                    break;
                }
            }
        }
        
        // Update old storage for compatibility
        if (i < memoryActivations_.size()) {
            size_t idx = i % memoryNeurons_.size();
            memoryActivations_[idx] = activation;
            memoryTimestamps_[idx] = 0;
        } else {
            memoryNeurons_.push_back(neuron);
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
        }
    }
    
    // Create maintenance connections if needed (efficient)
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        NeuronId from = memoryNeurons_[i-1];
        NeuronId to = memoryNeurons_[i];
        float connStrength = strength * 0.5f;
        
        // Update efficient storage
        pImpl->maintenanceSynapses[{from, to}] = connStrength;
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Update efficient storage
    pImpl->traces[neuron] = WorkingMemoryTrace{activation, 0, true};
    
    // Update legacy storage for compatibility
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    
    // Use efficient storage if available, otherwise fallback to legacy
    if (!pImpl->traces.empty()) {
        result.reserve(pImpl->traces.size());
        for (const auto& trace : pImpl->traces) {
            result.push_back(trace.second.activation);
        }
    } else {
        result = memoryActivations_;
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    // Efficient lookup
    return pImpl->traces.find(neuron) != pImpl->traces.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    // Efficient lookup
    auto it = pImpl->traces.find(neuron);
    if (it != pImpl->traces.end()) {
        return it->second.activation;
    }
    
    // Fallback to legacy linear search
    auto it2 = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it2 != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it2);
        return memoryActivations_[idx];
    }
    
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update efficient storage
    for (auto& pair : pImpl->traces) {
        WorkingMemoryTrace& trace = pair.second;
        
        if (trace.activation > 0.1f) {
            // Inject maintenance current
            brain_->injectCurrent(pair.first, trace.activation * 2.0f);
            
            // Age the trace
            trace.timestamp++;
            
            // Check if trace is too old
            if (trace.timestamp > 1000) {
                trace.activation *= (1.0f - decayRate_);
            }
        }
    }
    
    // Update legacy storage for compatibility
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current
            brain_->injectCurrent(memoryNeurons_[i], activation * 2.0f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Check if trace is too old
            if (memoryTimestamps_[i] > 1000) {
                activation *= (1.0f - decayRate_);
            }
            
            memoryActivations_[i] = activation;
        }
    }
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competition to select winners
    runCompetition();
}

void NeuralWorkingMemory::clear() {
    pImpl->traces.clear();
    pImpl->priorityQueue = std::priority_queue<std::pair<float, NeuronId>>();
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& pair : pImpl->traces) {
        pair.second.activation = std::min(1.0f, pair.second.activation * factor);
    }
    
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (pImpl->traces.empty()) {
        // Fallback to legacy method
        return;
    }
    
    // Clear priority queue
    while (!pImpl->priorityQueue.empty()) {
        pImpl->priorityQueue.pop();
    }
    
    // Fill priority queue with neuron activations
    for (const auto& pair : pImpl->traces) {
        pImpl->priorityQueue.push(std::make_pair(pair.second.activation, pair.first.value));
    }
    
    // Extract top 3 winners (winner-take-all)
    float threshold = 0.3f;
    std::vector<NeuronId> tempWinners;
    
    while (!pImpl->priorityQueue.empty() && tempWinners.size() < 3) {
        auto top = pImpl->priorityQueue.top();
        pImpl->priorityQueue.pop();
        
        if (top.first >= threshold) {
            tempWinners.push_back(NeuronId(top.second));
        }
    }
    
    winners_ = tempWinners;
    
    // Inhibitory competition - suppress non-winners (legacy)
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(winners_.begin(), winners_.end(), memoryNeurons_[i]) != winners_.end();
        
        if (!isWinner && brain_) {
            // Apply strong inhibition
            brain_->injectCurrent(memoryNeurons_[i], -memoryActivations_[i] * 3.0f);
        }
    }
}

bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

float NeuralWorkingMemory::getMemoryActivity() const {
    if (pImpl->traces.empty()) {
        // Legacy fallback
        if (memoryActivations_.empty()) return 0.0f;
        
        float total = 0.0f;
        for (float act : memoryActivations_) {
            total += act;
        }
        return total / memoryActivations_.size();
    }
    
    float total = 0.0f;
    for (const auto& trace : pImpl->traces) {
        total += trace.second.activation;
    }
    return total / pImpl->traces.size();
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Efficient update
    pImpl->maintenanceSynapses[{from, to}] = strength;
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Efficient lookup
    for (const auto& pair : pImpl->maintenanceSynapses) {
        NeuronId from = pair.first.first;
        NeuronId to = pair.first.second;
        float strength = pair.second;
        
        // Find from neuron activation
        auto it = pImpl->traces.find(from);
        float fromActivation = (it != pImpl->traces.end()) ? it->second.activation : 0.0f;
        
        // Also check legacy for compatibility
        if (fromActivation == 0.0f) {
            auto it2 = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), from);
            if (it2 != memoryNeurons_.end()) {
                size_t idx = std::distance(memoryNeurons_.begin(), it2);
                fromActivation = memoryActivations_[idx];
            }
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(to, fromActivation * 2.0f);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    // Clear priority queue
    std::priority_queue<std::pair<float, NeuronId>> newQueue;
    
    // Process efficient storage
    std::vector<NeuronId> toRemove;
    for (auto& pair : pImpl->traces) {
        WorkingMemoryTrace& trace = pair.second;
        trace.activation *= (1.0f - decayRate_);
        
        if (trace.activation < 0.01f) {
            toRemove.push_back(pair.first);
        } else {
            // Keep in priority queue
            newQueue.push(std::make_pair(trace.activation, pair.first.value));
        }
    }
    
    // Remove weak traces from efficient storage
    for (const auto& neuron : toRemove) {
        pImpl->traces.erase(neuron);
    }
    
    pImpl->priorityQueue = newQueue;
    
    // Process legacy storage for compatibility
    std::vector<size_t> toRemoveLegacy;
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemoveLegacy.push_back(i);
        }
    }
    
    // Remove weak traces from legacy (in reverse order to maintain indices)
    for (auto it = toRemoveLegacy.rbegin(); it != toRemoveLegacy.rend(); ++it) {
        memoryNeurons_.erase(memoryNeurons_.begin() + *it);
        memoryActivations_.erase(memoryActivations_.begin() + *it);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
    }
}

// Lazy migration to efficient storage
void NeuralWorkingMemory::migrateToEfficientStorage() {
    if (!memoryNeurons_.empty() && pImpl->traces.empty()) {
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            NeuronId neuron = memoryNeurons_[i];
            float activation = memoryActivations_[i];
            SimulationStep timestamp = memoryTimestamps_[i];
            
            pImpl->traces[neuron] = WorkingMemoryTrace{activation, timestamp, true};
        }
        
        // Clear legacy storage after migration
        memoryNeurons_.clear();
        memoryActivations_.clear();
        memoryTimestamps_.clear();
    }
}

void NeuralWorkingMemory::migrateToLegacyStorage() {
    if (!pImpl->traces.empty() && memoryNeurons_.empty()) {
        for (const auto& pair : pImpl->traces) {
            NeuronId neuron = pair.first;
            float activation = pair.second.activation;
            SimulationStep timestamp = pair.second.timestamp;
            
            memoryNeurons_.push_back(neuron);
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(timestamp);
        }
        
        // Clear efficient storage after migration
        pImpl->traces.clear();
        pImpl->priorityQueue = std::priority_queue<std::pair<float, NeuronId>>();
    }
}

// New utility function for lazy initialization
void NeuralWorkingMemory::ensureEfficientStorage() {
    if (!pImpl->traces.empty() && memoryNeurons_.empty()) {
        migrateToLegacyStorage();
    } else if (memoryNeurons_.empty() && !pImpl->traces.empty()) {
        migrateToEfficientStorage();
    }
}