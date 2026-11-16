// Enhanced NeuralWorkingMemory with better error handling, capacity management, and performance optimizations

#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Performance monitoring
    size_t totalStoreOperations = 0;
    size_t totalRetrieveOperations = 0;
    size_t totalUpdateOperations = 0;
    
    Impl() : brain(nullptr) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , capacity_(100)
    , decayRate_(0.01f)
{
    NLM_LOG_INFO("NeuralWorkingMemory initialized with default capacity: " + std::to_string(capacity_));
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize NeuralWorkingMemory with null brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralWorkingMemory initialized with " + std::to_string(brain_->getTotalNeuronCount()) + " neurons");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    pImpl->totalStoreOperations++;
    
    if (pattern.empty()) {
        NLM_LOG_WARNING("Attempted to store empty pattern in working memory");
        return;
    }
    
    if (!brain_) {
        NLM_LOG_ERROR("NeuralWorkingMemory not initialized - brain pointer is null");
        return;
    }
    
    // Check capacity before storing
    if (memoryNeurons_.size() >= capacity_) {
        // Evict oldest traces to make room
        memoryNeurons_.erase(memoryNeurons_.begin());
        memoryActivations_.erase(memoryActivations_.begin());
        memoryTimestamps_.erase(memoryTimestamps_.begin());
        // Update active traces
        activeTraces_.clear();
        for (size_t i = 0; i < memoryActivations_.size(); ++i) {
            if (memoryActivations_[i] > 0.1f) {
                activeTraces_.push_back(i);
            }
        }
        NLM_LOG_DEBUG("Evicted oldest memory trace to make room");
    }
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Validate neuron exists
        bool neuronFound = false;
        if (auto* region = brain_->getRegion(RegionId(neuron.index() / 1000))) {
            for (auto* nn : region->getAllNeurons()) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * 5.0f);
                    neuronFound = true;
                    break;
                }
            }
        }
        
        if (!neuronFound) {
            NLM_LOG_WARNING("Could not find neuron " + std::to_string(neuron.index()) + " for memory storage");
        }
        
        // Update stored activation
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
        }
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
    }
    
    NLM_LOG_DEBUG("Stored pattern with " + std::to_string(neuronsNeeded) + " neurons, strength: " + std::to_string(strength));
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    pImpl->totalStoreOperations++;
    
    if (!brain_) {
        NLM_LOG_ERROR("NeuralWorkingMemory not initialized - brain pointer is null");
        return;
    }
    
    // Validate activation range
    activation = std::clamp(activation, 0.0f, 1.0f);
    
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);

    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        it = memoryNeurons_.end() - 1;
    } else {
        NLM_LOG_WARNING("Working memory capacity reached - cannot store new neuron");
        return;
    }
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        bool present = std::find(activeTraces_.begin(), activeTraces_.end(), idx) != activeTraces_.end();
        if (activation > 0.1f && !present) {
            activeTraces_.push_back(idx);
        }
        if (activation <= 0.1f && present) {
            activeTraces_.erase(std::remove(activeTraces_.begin(), activeTraces_.end(), idx), activeTraces_.end());
        }
    }
    
    // Inject current to maintain activation
    brain_->injectCurrent(neuron, activation * 5.0f);
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
    pImpl->totalRetrieveOperations++;
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    for (float activation : memoryActivations_) {
        result.push_back(activation);
    }
    
    return result;
}

bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
}

float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        return memoryActivations_[idx];
    }
    return 0.0f;
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    pImpl->totalUpdateOperations++;
    
    if (!brain_) {
        NLM_LOG_ERROR("NeuralWorkingMemory not initialized - brain pointer is null");
        return;
    }
    
    // Update maintenance - reinforce active memory neurons
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current
            brain_->injectCurrent(neuron, activation * 2.0f);
            
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
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
    NLM_LOG_DEBUG("Working memory cleared");
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    // Validate factor
    factor = std::max(0.0f, std::min(10.0f, factor));
    
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
    
    NLM_LOG_DEBUG("Memory strengthened by factor " + std::to_string(factor));
}

void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) return;
    
    // Find neurons with above-threshold activation
    float threshold = 0.3f;
    
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryActivations_[i] >= threshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // Inhibitory competition - suppress non-winners
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
    if (memoryActivations_.empty()) return 0.0f;
    
    float total = 0.0f;
    for (float act : memoryActivations_) {
        total += act;
    }
    return total / memoryActivations_.size();
}

void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) return;
    }
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
}

void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = 0.0f;
        
        // Find from neuron activation
        auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), conn.first);
        if (it != memoryNeurons_.end()) {
            size_t idx = std::distance(memoryNeurons_.begin(), it);
            fromActivation = memoryActivations_[idx];
        }
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal
            brain_->injectCurrent(conn.second, fromActivation * 2.0f);
        }
    }
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        memoryNeurons_.erase(memoryNeurons_.begin() + *it);
        memoryActivations_.erase(memoryActivations_.begin() + *it);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + *it);
    }
    
    // Rebuild active traces
    activeTraces_.clear();
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] > 0.1f) {
            activeTraces_.push_back(i);
        }
    }
}

// Performance monitoring
void NeuralWorkingMemory::logPerformanceStats() const {
    NLM_LOG_INFO("NeuralWorkingMemory Performance Stats:");
    NLM_LOG_INFO("  Store operations: " + std::to_string(pImpl->totalStoreOperations));
    NLM_LOG_INFO("  Retrieve operations: " + std::to_string(pImpl->totalRetrieveOperations));
    NLM_LOG_INFO("  Update operations: " + std::to_string(pImpl->totalUpdateOperations));
    NLM_LOG_INFO("  Current memory traces: " + std::to_string(memoryNeurons_.size()));
    NLM_LOG_INFO("  Active traces: " + std::to_string(activeTraces_.size()));
    NLM_LOG_INFO("  Memory capacity: " + std::to_string(capacity_));
    NLM_LOG_INFO("  Current memory activity: " + std::to_string(getMemoryActivity()));
}

} // namespace nlm