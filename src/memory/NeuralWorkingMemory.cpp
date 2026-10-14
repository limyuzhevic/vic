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
}

void NeuralWorkingMemory::storeSensoryPattern(const std::vector<float>& sensoryPattern, float strength, bool forceUpdate) {
    if (sensoryPattern.empty() || !brain_) return;
    
    // Direct sensory input to working memory
    // Convert sensory pattern to neural activity
    store(sensoryPattern, strength);
    
    // Apply immediate boost to memory traces if forced
    if (forceUpdate && !memoryNeurons_.empty()) {
        strengthenMemory(strength);
    }
}

void NeuralWorkingMemory::applyTopDownModulation(const std::vector<NeuronId>& modulatedNeurons, float modulationStrength) {
    if (!brain_) return;
    
    // Apply top-down effects from working memory to influence neural activity
    for (const auto& neuronId : modulatedNeurons) {
        // Find neuron activation level in working memory
        float activation = getNeuronActivation(neuronId);
        
        if (activation > 0.1f) {
            // Inject top-down current proportional to memory activation
            float topDownCurrent = activation * modulationStrength;
            brain_->injectCurrent(neuronId, topDownCurrent);
            
            // Update stored activation with top-down reinforcement
            auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuronId);
            if (it != memoryNeurons_.end()) {
                size_t idx = std::distance(memoryNeurons_.begin(), it);
                memoryActivations_[idx] = std::min(1.0f, memoryActivations_[idx] + topDownCurrent * 0.1f);
                memoryTimestamps_[idx] = 0;  // Reset age for reinforced traces
            }
        }
    }
}

void NeuralWorkingMemory::updateWithTemporalDynamics(TimestepDuration dt) {
    if (!brain_) return;
    
    // Enhanced temporal dynamics
    update(dt);  // Call base update for maintenance
    
    // Apply recurrent maintenance currents more aggressively
    updateRecurrentConnections();
    
    // Update maintenance synapses based on memory age
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryTimestamps_[i] > 100) {  // Trace older than 100 steps
            // Weaken maintenance connections for older traces
            // This implements temporal decay at synaptic level
            float decayFactor = std::pow(decayRate_, static_cast<float>(memoryTimestamps_[i] - 100) / 1000.0f);
            for (auto& conn : pImpl->maintenanceSynapses) {
                if (conn.first == memoryNeurons_[i] || conn.second == memoryNeurons_[i]) {
                    // Reduce connection strength
                    // In a full implementation, this would update actual synaptic weights
                }
            }
        }
    }
}

void NeuralWorkingMemory::feedIntoAttention() {
    if (!brain_ || !brain_->getAttention()) return;
    
    // Working memory traces feed into attentional selection
    std::vector<NeuronId> memoryCandidates = getMemoryNeurons();
    
    if (!memoryCandidates.empty()) {
        // Apply working memory activity as bottom-up salience to attention system
        for (size_t i = 0; i < memoryCandidates.size(); ++i) {
            float salience = memoryActivations_[i];
            if (salience > 0.05f) {
                brain_->getAttention()->applyBottomUpSalience(memoryCandidates[i], salience);
            }
        }
        
        // Also feed top-down bias from working memory to attention
        // This implements working memory's influence on attentional focus
        if (getMemoryActivity() > 0.5f) {
            // Strong memory activity influences attention
            float memoryBias = getMemoryActivity() * 0.5f;
            for (const auto& neuron : memoryCandidates) {
                brain_->getAttention()->applyTopDownBias(neuron, memoryBias);
            }
        }
    }
}

void NeuralWorkingMemory::manageCapacityWithEviction() {
    // Enhanced capacity management with intelligent eviction
    size_t currentCapacity = getMemoryNeurons().size();
    
    if (currentCapacity <= capacity_) {
        return;  // Within capacity, nothing to do
    }
    
    // Evict weakest traces (priority-based eviction)
    std::vector<size_t> traceIndices(currentCapacity);
    std::iota(traceIndices.begin(), traceIndices.end(), 0);
    
    // Sort by activation level (weakest first)
    std::sort(traceIndices.begin(), traceIndices.end(), 
        [this](size_t a, size_t b) { return memoryActivations_[a] < memoryActivations_[b]; });
    
    // Evict weakest traces until within capacity
    size_t toEvict = currentCapacity - capacity_;
    for (size_t i = 0; i < toEvict; ++i) {
        size_t idx = traceIndices[i];
        
        // Remove the neuron from memory
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        
        // Also remove its recurrent connections
        auto it = pImpl->maintenanceSynapses.begin();
        while (it != pImpl->maintenanceSynapses.end()) {
            if (it->first == memoryNeurons_[idx] || it->second == memoryNeurons_[idx]) {
                it = pImpl->maintenanceSynapses.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
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
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
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

void NeuralWorkingMemory::storeSensoryPattern(const std::vector<float>& sensoryPattern, float strength, bool forceUpdate) {
    if (sensoryPattern.empty() || !brain_) return;
    
    // Direct sensory input to working memory
    // Convert sensory pattern to neural activity
    store(sensoryPattern, strength);
    
    // Apply immediate boost to memory traces if forced
    if (forceUpdate && !memoryNeurons_.empty()) {
        strengthenMemory(strength);
    }
}

void NeuralWorkingMemory::applyTopDownModulation(const std::vector<NeuronId>& neurons, float modulationStrength) {
    if (!brain_) return;
    
    // Apply top-down effects from working memory to influence neural activity
    for (const auto& neuronId : neurons) {
        // Find neuron activation level in working memory
        float activation = getNeuronActivation(neuronId);
        
        if (activation > 0.1f) {
            // Inject top-down current proportional to memory activation
            float topDownCurrent = activation * modulationStrength;
            brain_->injectCurrent(neuronId, topDownCurrent);
            
            // Update stored activation with top-down reinforcement
            auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuronId);
            if (it != memoryNeurons_.end()) {
                size_t idx = std::distance(memoryNeurons_.begin(), it);
                memoryActivations_[idx] = std::min(1.0f, memoryActivations_[idx] + topDownCurrent * 0.1f);
                memoryTimestamps_[idx] = 0;  // Reset age for reinforced traces
            }
        }
    }
}

void NeuralWorkingMemory::updateWithTemporalDynamics(TimestepDuration dt) {
    if (!brain_) return;
    
    // Enhanced temporal dynamics
    update(dt);  // Call base update for maintenance
    
    // Apply recurrent maintenance currents more aggressively
    updateRecurrentConnections();
    
    // Update maintenance synapses based on memory age
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryTimestamps_[i] > 100) {  // Trace older than 100 steps
            // Weaken maintenance connections for older traces
            // This implements temporal decay at synaptic level
            float decayFactor = std::pow(decayRate_, static_cast<float>(memoryTimestamps_[i] - 100) / 1000.0f);
            for (auto& conn : pImpl->maintenanceSynapses) {
                if (conn.first == memoryNeurons_[i] || conn.second == memoryNeurons_[i]) {
                    // Reduce connection strength
                    // In a full implementation, this would update actual synaptic weights
                }
            }
        }
    }
}

// ============ WORKING MEMORY FUNCTIONS - EPISODIC MEMORY INTEGRATION ===========

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
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
}

void NeuralWorkingMemory::storeSensoryPattern(const std::vector<float>& sensoryPattern, float strength, bool forceUpdate) {
    if (sensoryPattern.empty() || !brain_) return;
    
    // Direct sensory input to working memory
    // Convert sensory pattern to neural activity
    store(sensoryPattern, strength);
    
    // Apply immediate boost to memory traces if forced
    if (forceUpdate && !memoryNeurons_.empty()) {
        strengthenMemory(strength);
    }
}

void NeuralWorkingMemory::applyTopDownModulation(const std::vector<NeuronId>& modulatedNeurons, float modulationStrength) {
    if (!brain_) return;
    
    // Apply top-down effects from working memory to influence neural activity
    for (const auto& neuronId : modulatedNeurons) {
        // Find neuron activation level in working memory
        float activation = getNeuronActivation(neuronId);
        
        if (activation > 0.1f) {
            // Inject top-down current proportional to memory activation
            float topDownCurrent = activation * modulationStrength;
            brain_->injectCurrent(neuronId, topDownCurrent);
            
            // Update stored activation with top-down reinforcement
            auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuronId);
            if (it != memoryNeurons_.end()) {
                size_t idx = std::distance(memoryNeurons_.begin(), it);
                memoryActivations_[idx] = std::min(1.0f, memoryActivations_[idx] + topDownCurrent * 0.1f);
                memoryTimestamps_[idx] = 0;  // Reset age for reinforced traces
            }
        }
    }
}

void NeuralWorkingMemory::updateWithTemporalDynamics(TimestepDuration dt) {
    if (!brain_) return;
    
    // Enhanced temporal dynamics
    update(dt);  // Call base update for maintenance
    
    // Apply recurrent maintenance currents more aggressively
    updateRecurrentConnections();
    
    // Update maintenance synapses based on memory age
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryTimestamps_[i] > 100) {  // Trace older than 100 steps
            // Weaken maintenance connections for older traces
            // This implements temporal decay at synaptic level
            float decayFactor = std::pow(decayRate_, static_cast<float>(memoryTimestamps_[i] - 100) / 1000.0f);
            for (auto& conn : pImpl->maintenanceSynapses) {
                if (conn.first == memoryNeurons_[i] || conn.second == memoryNeurons_[i]) {
                    // Reduce connection strength
                    // In a full implementation, this would update actual synaptic weights
                }
            }
        }
    }
}

void NeuralWorkingMemory::feedIntoAttention() {
    if (!brain_ || !brain_->getAttention()) return;
    
    // Working memory traces feed into attentional selection
    std::vector<NeuronId> memoryCandidates = getMemoryNeurons();
    
    if (!memoryCandidates.empty()) {
        // Apply working memory activity as bottom-up salience to attention system
        for (size_t i = 0; i < memoryCandidates.size(); ++i) {
            float salience = memoryActivations_[i];
            if (salience > 0.05f) {
                brain_->getAttention()->applyBottomUpSalience(memoryCandidates[i], salience);
            }
        }
        
        // Also feed top-down bias from working memory to attention
        // This implements working memory's influence on attentional focus
        if (getMemoryActivity() > 0.5f) {
            // Strong memory activity influences attention
            float memoryBias = getMemoryActivity() * 0.5f;
            for (const auto& neuron : memoryCandidates) {
                brain_->getAttention()->applyTopDownBias(neuron, memoryBias);
            }
        }
    }
}

void NeuralWorkingMemory::manageCapacityWithEviction() {
    // Enhanced capacity management with intelligent eviction
    size_t currentCapacity = getMemoryNeurons().size();
    
    if (currentCapacity <= capacity_) {
        return;  // Within capacity, nothing to do
    }
    
    // Evict weakest traces (priority-based eviction)
    std::vector<size_t> traceIndices(currentCapacity);
    std::iota(traceIndices.begin(), traceIndices.end(), 0);
    
    // Sort by activation level (weakest first)
    std::sort(traceIndices.begin(), traceIndices.end(), 
        [this](size_t a, size_t b) { return memoryActivations_[a] < memoryActivations_[b]; });
    
    // Evict weakest traces until within capacity
    size_t toEvict = currentCapacity - capacity_;
    for (size_t i = 0; i < toEvict; ++i) {
        size_t idx = traceIndices[i];
        
        // Remove the neuron from memory
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        
        // Also remove its recurrent connections
        auto it = pImpl->maintenanceSynapses.begin();
        while (it != pImpl->maintenanceSynapses.end()) {
            if (it->first == memoryNeurons_[idx] || it->second == memoryNeurons_[idx]) {
                it = pImpl->maintenanceSynapses.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
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
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
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
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
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

void NeuralWorkingMemory::updateWithTemporalDynamics(TimestepDuration dt) {
    if (!brain_) return;
    
    // Enhanced temporal dynamics
    update(dt);  // Call base update for maintenance
    
    // Apply recurrent maintenance currents more aggressively
    updateRecurrentConnections();
    
    // Update maintenance synapses based on memory age
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (memoryTimestamps_[i] > 100) {  // Trace older than 100 steps
            // Weaken maintenance connections for older traces
            // This implements temporal decay at synaptic level
            float decayFactor = std::pow(decayRate_, static_cast<float>(memoryTimestamps_[i] - 100) / 1000.0f);
            for (auto& conn : pImpl->maintenanceSynapses) {
                if (conn.first == memoryNeurons_[i] || conn.second == memoryNeurons_[i]) {
                    // Reduce connection strength
                    // In a full implementation, this would update actual synaptic weights
                }
            }
        }
    }
}

void NeuralWorkingMemory::feedIntoAttention() {
    if (!brain_ || !brain_->getAttention()) return;
    
    // Working memory traces feed into attentional selection
    std::vector<NeuronId> memoryCandidates = getMemoryNeurons();
    
    if (!memoryCandidates.empty()) {
        // Apply working memory activity as bottom-up salience to attention system
        for (size_t i = 0; i < memoryCandidates.size(); ++i) {
            float salience = memoryActivations_[i];
            if (salience > 0.05f) {
                brain_->getAttention()->applyBottomUpSalience(memoryCandidates[i], salience);
            }
        }
        
        // Also feed top-down bias from working memory to attention
        // This implements working memory's influence on attentional focus
        if (getMemoryActivity() > 0.5f) {
            // Strong memory activity influences attention
            float memoryBias = getMemoryActivity() * 0.5f;
            for (const auto& neuron : memoryCandidates) {
                brain_->getAttention()->applyTopDownBias(neuron, memoryBias);
            }
        }
    }
}

void NeuralWorkingMemory::manageCapacityWithEviction() {
    // Enhanced capacity management with intelligent eviction
    size_t currentCapacity = getMemoryNeurons().size();
    
    if (currentCapacity <= capacity_) {
        return;  // Within capacity, nothing to do
    }
    
    // Evict weakest traces (priority-based eviction)
    std::vector<size_t> traceIndices(currentCapacity);
    std::iota(traceIndices.begin(), traceIndices.end(), 0);
    
    // Sort by activation level (weakest first)
    std::sort(traceIndices.begin(), traceIndices.end(), 
        [this](size_t a, size_t b) { return memoryActivations_[a] < memoryActivations_[b]; });
    
    // Evict weakest traces until within capacity
    size_t toEvict = currentCapacity - capacity_;
    for (size_t i = 0; i < toEvict; ++i) {
        size_t idx = traceIndices[i];
        
        // Remove the neuron from memory
        memoryNeurons_.erase(memoryNeurons_.begin() + idx);
        memoryActivations_.erase(memoryActivations_.begin() + idx);
        memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
        
        // Also remove its recurrent connections
        auto it = pImpl->maintenanceSynapses.begin();
        while (it != pImpl->maintenanceSynapses.end()) {
            if (it->first == memoryNeurons_[idx] || it->second == memoryNeurons_[idx]) {
                it = pImpl->maintenanceSynapses.erase(it);
            } else {
                ++it;
            }
        }
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
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
    }
    
    // Inject current to maintain activation
    if (brain_) {
        brain_->injectCurrent(neuron, activation * 5.0f);
    }
}

std::vector<float> NeuralWorkingMemory::retrieve() const {
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
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
}

// ============ WORKING MEMORY EPISODIC MEMORY INTEGRATION ===========

void NeuralWorkingMemory::integrateWithEpisodicMemory(const NeuralEpisodicMemory* episodicMemory) {
    if (!brain_ || !episodicMemory) return;
    
    // Get all working memory content
    std::vector<float> workingMemoryContent = retrieve();
    
    // Retrieve similar episodes based on working memory pattern
    auto similarEpisodes = episodicMemory->retrieveSimilar(workingMemoryContent, 3);
    
    for (const auto* episode : similarEpisodes) {
        // Enhance working memory with episodic memory patterns
        // This implements pattern completion from episodic memory
        if (episode && !episode->activeNeurons.empty()) {
            // Reactivate associated neurons based on episodic memory
            for (size_t i = 0; i < std::min(episode->activeNeurons.size(), episode->neuronActivations.size()); ++i) {
                NeuronId neuron = episode->activeNeurons[i];
                float activation = episode->neuronActivations[i] * 0.5f;  // Reduced contribution
                
                // Add to working memory if not already present
                if (!contains(neuron)) {
                    storeToNeuron(neuron, activation);
                } else {
                    // Boost existing neuron activation
                    float currentActivation = getNeuronActivation(neuron);
                    storeToNeuron(neuron, std::min(1.0f, currentActivation + activation * 0.3f));
                }
            }
        }
    }
}

void NeuralWorkingMemory::updateWithEpisodicInfluence(TimestepDuration dt) {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    // Update working memory with influence from episodic memory
    // This provides episodic context to working memory
    
    // Get current working memory content
    std::vector<float> currentContent = retrieve();
    
    // Find episodic episodes that match working memory content
    auto matchingEpisodes = brain_->getEpisodicMemory()->retrieveSimilar(currentContent, 5);
    
    for (const auto* episode : matchingEpisodes) {
        if (episode && !episode->activeNeurons.empty()) {
            // Enhance working memory with episodic reinforcement
            for (size_t i = 0; i < std::min(episode->activeNeurons.size(), episode->neuronActivations.size()); ++i) {
                NeuronId neuron = episode->activeNeurons[i];
                float episodicActivation = episode->neuronActivations[i];
                
                // Add episodic memory influence to working memory
                float currentActivation = getNeuronActivation(neuron);
                float enhancedActivation = currentActivation + episodicActivation * 0.2f * episode->salience;
                
                storeToNeuron(neuron, std::min(1.0f, enhancedActivation));
            }
        }
    }
    
    // Apply standard working memory update
    update(dt);
}

void NeuralWorkingMemory::feedIntoEpisodicMemory() {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    // Working memory content feeds into episodic memory as new episodes
    // This captures transient states for long-term storage
    
    // Get current working memory state
    std::vector<float> currentWorkingMemory = retrieve();
    
    if (!currentWorkingMemory.empty() && getMemoryActivity() > 0.1f) {
        // Create an episodic memory item from working memory content
        EpisodicMemoryItem episodicItem;
        episodicItem.timestamp = brain_->getCurrentStep();
        episodicItem.workingMemoryPattern = currentWorkingMemory;
        episodicItem.action = ActionType::Wait;  // Working memory state is not an action
        episodicItem.reward = 0.0f;  // Working memory states don't have direct reward
        episodicItem.salience = getMemoryActivity();  // Activity level as salience
        episodicItem.novelty = 0.0f;  // Would need novelty detection
        
        // Store in episodic memory with enhanced context
        brain_->getEpisodicMemory()->storeEpisodeWithContext(episodicItem,
                                                           std::vector<float>(),  // system state
                                                           brain_->getDopamine() ? brain_->getDopamine()->getLevel() : 0.0f,
                                                           1.0f,  // global excitability
                                                           std::vector<float>(),  // body velocity
                                                           currentWorkingMemory,
                                                           std::vector<float>(),  // prediction target
                                                           std::vector<float>(),  // attention focus
                                                           0.0f,  // prediction error
                                                           0.0f,  // curiosity gain
                                                           episodicItem.salience,
                                                           0.0f); // emotional valence
    }
}

void NeuralWorkingMemory::updateWithSleepConsolidation() {
    if (!brain_ || !brain_->getEpisodicMemory()) return;
    
    // Special handling for sleep periods
    // During sleep, working memory traces are consolidated into episodic memory
    
    // Only apply during sleep-like states (every 1000 steps)
    if (brain_->getCurrentStep() % 1000 == 0) {
        // Transfer important working memory traces to episodic memory
        std::vector<NeuronId> highActivityNeurons;
        
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            if (memoryActivations_[i] > 0.5f) {
                highActivityNeurons.push_back(memoryNeurons_[i]);
            }
        }
        
        if (!highActivityNeurons.empty()) {
            // Convert working memory to episodic memory
            feedIntoEpisodicMemory();
            
            // Clear strong working memory traces after consolidation
            clear();
        }
    }
}

void NeuralWorkingMemory::feedIntoEpisodicMemoryAndAttention() {
    if (!brain_ || !brain_->getEpisodicMemory() || !brain_->getAttention()) return;
    
    // Combined feed: working memory influences both episodic memory and attention
    
    // First, feed into episodic memory
    feedIntoEpisodicMemory();
    
    // Then feed into attention
    feedIntoAttention();
    
    // Also feed episodic memory content back to working memory
    if (brain_->getEpisodicMemory()->getEpisodeCount() > 0) {
        // Get recent episodic memory content
        auto recentEpisodes = brain_->getEpisodicMemory()->getRecentEpisodes(3);
        
        for (const auto* episode : recentEpisodes) {
            if (episode && !episode->activeNeurons.empty()) {
                // Integrate episodic memory into working memory
                for (size_t i = 0; i < std::min(episode->activeNeurons.size(), episode->neuronActivations.size()); ++i) {
                    NeuronId neuron = episode->activeNeurons[i];
                    float episodicActivation = episode->neuronActivations[i] * 0.3f;
                    
                    storeToNeuron(neuron, episodicActivation);
                }
            }
        }
    }
}

} // namespace nlm