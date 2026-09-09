#include "NeuralWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent)
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages
    std::vector<SimulationStep> traceAges;
    
    // Context for memory traces
    std::vector<std::string> traceContexts;
    
    // Neural representation patterns
    std::vector<std::vector<float>> tracePatterns;
    
    // Rehearsal counters for strengthening
    std::vector<size_t> rehearsalCounts;
    
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

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength, const std::string& context) {
    if (pattern.empty() || !brain_) return;
    
    // Find neurons to encode this pattern
    size_t neuronsNeeded = std::min(pattern.size(), memoryNeurons_.size());
    
    for (size_t i = 0; i < neuronsNeeded; ++i) {
        NeuronId neuron = memoryNeurons_[i % memoryNeurons_.size()];
        float activation = pattern[i] * strength;
        
        // Set neuron activation
        if (auto* n = brain_->getRegion(neuron.getId() / 1000)->getAllNeurons()) {
            for (auto* nn : *n) {
                if (nn->getId() == neuron) {
                    nn->injectCurrent(activation * 5.0f);
                    break;
                }
            }
        }
        
        // Update stored activation
        if (i < memoryActivations_.size()) {
            memoryActivations_[i] = activation;
            if (i < pImpl->traceContexts.size()) {
                pImpl->traceContexts[i] = context;
            }
        } else {
            memoryActivations_.push_back(activation);
            memoryTimestamps_.push_back(0);
            memoryNeurons_.push_back(neuron);
            pImpl->traceContexts.push_back(context);
            pImpl->tracePatterns.push_back(std::vector<float>());
        }
    }
    
    // Create maintenance connections if needed
    for (size_t i = 1; i < memoryNeurons_.size(); ++i) {
        createRecurrentConnection(memoryNeurons_[i-1], memoryNeurons_[i], strength * 0.5f);
    }
}

void NeuralWorkingMemory::store(NeuronId neuron, float activation, const std::string& context) {
    // Enhanced store method with context support
    if (!brain_) return;
    
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
        if (idx < pImpl->traceContexts.size()) {
            pImpl->traceContexts[idx] = context;
        }
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        pImpl->traceContexts.push_back(context);
        pImpl->tracePatterns.push_back(std::vector<float>());
    }
    
    // Inject current to maintain activation
    brain_->injectCurrent(neuron, activation * 5.0f);
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation, const std::string& context) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = activation;
        memoryTimestamps_[idx] = 0;
        if (idx < pImpl->traceContexts.size()) {
            pImpl->traceContexts[idx] = context;
        }
    } else if (memoryNeurons_.size() < capacity_) {
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(activation);
        memoryTimestamps_.push_back(0);
        pImpl->traceContexts.push_back(context);
        pImpl->tracePatterns.push_back(std::vector<float>());
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation, float strength) {
    storeToNeuron(neuron, activation * strength, "");
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    store(pattern, strength, "");
}

bool NeuralWorkingMemory::contains(const std::string& context) const {
    for (const auto& ctx : pImpl->traceContexts) {
        if (ctx == context) return true;
    }
    return false;
}

std::string NeuralWorkingMemory::getContext(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        if (idx < pImpl->traceContexts.size()) {
            return pImpl->traceContexts[idx];
        }
    }
    return "";
}

void NeuralWorkingMemory::updateContext(NeuronId neuron, const std::string& newContext) {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        if (idx < pImpl->traceContexts.size()) {
            pImpl->traceContexts[idx] = newContext;
        }
    }
}

void NeuralWorkingMemory::storeTimed(const std::vector<float>& pattern, float strength, SimulationStep timestamp, const std::string& context) {
    store(pattern, strength, context);
    // Update timestamps for all stored neurons
    for (auto& t : memoryTimestamps_) {
        t = timestamp;
    }
}

std::vector<float> NeuralWorkingMemory::retrieve(const std::string& context) const {
    std::vector<float> result;
    
    if (context.empty()) {
        // Return all activations
        result.reserve(memoryActivations_.size());
        for (float activation : memoryActivations_) {
            result.push_back(activation);
        }
    } else {
        // Return only activations with matching context
        for (size_t i = 0; i < memoryNeurons_.size() && i < pImpl->traceContexts.size(); ++i) {
            if (pImpl->traceContexts[i] == context) {
                result.push_back(memoryActivations_[i]);
            }
        }
    }
    
    return result;
}

std::vector<float> NeuralWorkingMemory::retrieveWithTimestamps() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    for (float activation : memoryActivations_) {
        result.push_back(activation);
    }
    return result;
}

SimulationStep NeuralWorkingMemory::getAge(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        if (idx < memoryTimestamps_.size()) {
            return memoryTimestamps_[idx];
        }
    }
    return 0;
}

void NeuralWorkingMemory::rehearse(NeuronId neuron) {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        if (idx < memoryActivations_.size()) {
            // Increase activation as if rehearsed
            memoryActivations_[idx] = std::min(1.0f, memoryActivations_[idx] * 1.1f);
            if (idx < pImpl->rehearsalCounts.size()) {
                pImpl->rehearsalCounts[idx]++;
            }
            
            // Inject current to strengthen trace
            if (brain_) {
                brain_->injectCurrent(neuron, memoryActivations_[idx] * 8.0f);
            }
        }
    }
}

size_t NeuralWorkingMemory::getTraceCountOlderThan(SimulationStep threshold) const {
    size_t count = 0;
    for (auto age : memoryTimestamps_) {
        if (age > threshold) count++;
    }
    return count;
}

std::set<std::string> NeuralWorkingMemory::getAllContexts() const {
    std::set<std::string> contexts;
    for (const auto& ctx : pImpl->traceContexts) {
        if (!ctx.empty()) {
            contexts.insert(ctx);
        }
    }
    return contexts;
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    storeToNeuron(neuron, activation, "");
}

void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
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
    pImpl->traceContexts.clear();
    pImpl->tracePatterns.clear();
    pImpl->rehearsalCounts.clear();
    pImpl->maintenanceSynapses.clear();
    activeTraces_.clear();
    winners_.clear();
}

void NeuralWorkingMemory::strengthenMemory(float factor) {
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
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
        if (*it < pImpl->traceContexts.size()) {
            pImpl->traceContexts.erase(pImpl->traceContexts.begin() + *it);
        }
        if (*it < pImpl->tracePatterns.size()) {
            pImpl->tracePatterns.erase(pImpl->tracePatterns.begin() + *it);
        }
        if (*it < pImpl->rehearsalCounts.size()) {
            pImpl->rehearsalCounts.erase(pImpl->rehearsalCounts.begin() + *it);
        }
    }
}

} // namespace nlm
