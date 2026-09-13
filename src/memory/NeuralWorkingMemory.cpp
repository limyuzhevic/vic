// Comprehensive implementation of NeuralWorkingMemory class
// This implementation provides a biologically-inspired working memory system
// that maintains neural activity through recurrent connections and competitive dynamics

#pragma once

#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cassert>

namespace nlm {

// Implement the Impl structure for NeuralWorkingMemory
struct NeuralWorkingMemory::Impl {
    Brain* brain;
    
    // Maintenance connections (recurrent) - create reverberating circuits
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceSynapses;
    
    // Memory trace ages - track how long traces have been active
    std::vector<SimulationStep> traceAges;
    
    // Consolidation markers - track which traces should be consolidated
    std::vector<bool> consolidateTrace;
    
    // Competitive strength for each neuron
    std::vector<float> competitionStrength;
    
    // Neural bursts for rapid encoding
    std::vector<bool> isBursting;
    
    Impl() : brain(nullptr) {}
};

// Constructor - initialize memory system with default parameters
NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(std::make_unique<Impl>())
    , brain_(nullptr)
    , capacity_(100)           // Default capacity of 100 neurons
    , decayRate_(0.01f)        // 1% decay per timestep
    , maintenanceInterval_(2)   // Maintenance every 2 timesteps
    , consolidationThreshold_(0.8f) // Consolidate when trace reaches 80% of max
    , lastMaintenanceStep_(0)
{
    NLM_LOG_DEBUG("NeuralWorkingMemory created with default capacity=%zu, decay=%.2f",
                  capacity_, decayRate_);
}

// Destructor - cleanup handled automatically by unique_ptr
NeuralWorkingMemory::~NeuralWorkingMemory() = default;

// Initialize with brain reference - establish connection to neural substrate
void NeuralWorkingMemory::initialize(Brain* brain) {
    assert(brain != nullptr && "Brain pointer cannot be null");
    
    // Clear previous state before re-initialization
    clear();
    
    // Store brain reference for neural interactions
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize active traces
    activeTraces_.clear();
    
    NLM_LOG_INFO("NeuralWorkingMemory initialized with capacity=%zu neurons", capacity_);
}

// Store information in working memory as a neural activity pattern
void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !brain_) {
        return; // Invalid input or not initialized
    }
    
    if (pattern.size() > capacity_) {
        NLM_LOG_WARN("Pattern size %zu exceeds capacity %zu, truncating", 
                     pattern.size(), capacity_);
    }
    
    // Encode each pattern element as a neural activation
    for (size_t i = 0; i < pattern.size() && i < capacity_; ++i) {
        NeuronId neuron(i); // Simplified neuron ID generation
        float activation = pattern[i] * strength;
        
        // Store the activation in memory
        storeToNeuron(neuron, activation);
        
        // Trigger neural burst for rapid encoding
        if (activation > 0.5f) {
            pImpl->isBursting.push_back(true);
        }
    }
    
    // Establish recurrent connections for maintenance
    createRecurrentConnections();
    
    // Record the encoding event
    NLM_LOG_DEBUG("Stored pattern with %zu elements, max activation=%.2f", 
                  pattern.size(), pattern.empty() ? 0.0f : *std::max_element(pattern.begin(), pattern.end()));
}

// Store specific neuron activation in working memory
void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    
    if (it != memoryNeurons_.end()) {
        // Update existing trace
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        memoryActivations_[idx] = std::min(1.0f, activation); // Clamp to max activation
        memoryTimestamps_[idx] = 0; // Reset age
        
        // Set competition strength based on activation
        pImpl->competitionStrength[idx] = activation;
        
        // Reset burst state if activation is high
        if (activation > 0.7f && pImpl->isBursting.size() > idx) {
            pImpl->isBursting[idx] = true;
        }
    } else if (memoryNeurons_.size() < capacity_) {
        // Add new trace within capacity limits
        memoryNeurons_.push_back(neuron);
        memoryActivations_.push_back(std::min(1.0f, activation));
        memoryTimestamps_.push_back(0);
        pImpl->traceAges.push_back(0);
        pImpl->consolidateTrace.push_back(false);
        pImpl->competitionStrength.push_back(activation);
        pImpl->isBursting.push_back(activation > 0.7f);
        
        // Record in active traces
        activeTraces_.push_back(memoryNeurons_.size() - 1);
    } else {
        // Replace weakest trace with new input (competitive replacement)
        size_t weakestIdx = findWeakestTrace();
        if (weakestIdx != size_t(-1)) {
            replaceTrace(weakestIdx, neuron, activation);
            
            // Update maintenance connections
            updateRecurrentConnections();
        }
    }
    
    // Inject current to neural substrate for persistence
    if (brain_) {
        float currentStrength = activation * 5.0f; // Scale factor for neural injection
        brain_->injectCurrent(neuron, currentStrength);
    }
}

// Retrieve current working memory content as activity levels
std::vector<float> NeuralWorkingMemory::retrieve() const {
    std::vector<float> result;
    result.reserve(memoryActivations_.size());
    
    // Copy current activations
    for (float activation : memoryActivations_) {
        result.push_back(activation);
    }
    
    return result;
}

// Check if specific neuron is part of working memory
bool NeuralWorkingMemory::contains(NeuronId neuron) const {
    return std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron) != memoryNeurons_.end();
}

// Get activation level of a specific neuron in working memory
float NeuralWorkingMemory::getNeuronActivation(NeuronId neuron) const {
    auto it = std::find(memoryNeurons_.begin(), memoryNeurons_.end(), neuron);
    if (it != memoryNeurons_.end()) {
        size_t idx = std::distance(memoryNeurons_.begin(), it);
        return memoryActivations_[idx];
    }
    return 0.0f;
}

// Update working memory (maintenance and decay)
void NeuralWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) {
        return; // Not initialized
    }
    
    // Update memory traces for maintenance
    updateMemoryTraces(dt);
    
    // Decay weak traces
    decayWeakTraces();
    
    // Run competitive selection
    runCompetition();
    
    // Apply consolidation for strong traces
    consolidateStrongTraces();
}

// Clear working memory completely
void NeuralWorkingMemory::clear() {
    memoryNeurons_.clear();
    memoryActivations_.clear();
    memoryTimestamps_.clear();
    activeTraces_.clear();
    pImpl->maintenanceSynapses.clear();
    pImpl->traceAges.clear();
    pImpl->consolidateTrace.clear();
    pImpl->competitionStrength.clear();
    pImpl->isBursting.clear();
    winners_.clear();
    
    NLM_LOG_DEBUG("Working memory cleared");
}

// Strengthen working memory representation (for rehearsal)
void NeuralWorkingMemory::strengthenMemory(float factor) {
    assert(factor > 0.0f && "Factor must be positive");
    
    for (auto& activation : memoryActivations_) {
        activation = std::min(1.0f, activation * factor);
    }
    
    // Re-inject current to reinforce
    if (brain_ && !memoryNeurons_.empty()) {
        for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
            brain_->injectCurrent(memoryNeurons_[i], memoryActivations_[i] * 3.0f);
        }
    }
    
    NLM_LOG_DEBUG("Memory strengthened by factor %.2f", factor);
}

// Competition between memory traces for selective retention
void NeuralWorkingMemory::runCompetition() {
    winners_.clear();
    
    if (memoryActivations_.empty()) {
        return;
    }
    
    // Normalize activation levels for fair competition
    float maxActivation = *std::max_element(memoryActivations_.begin(), memoryActivations_.end());
    if (maxActivation < 0.001f) {
        // All traces are weak - maintain all equally
        winners_ = memoryNeurons_;
        return;
    }
    
    // Apply competitive inhibition
    std::vector<float> normalizedActs;
    for (float act : memoryActivations_) {
        normalizedActs.push_back(act / maxActivation);
    }
    
    // Apply lateral inhibition based on relative activation
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        for (size_t j = 0; j < memoryNeurons_.size(); ++j) {
            if (i != j) {
                float relativeAct = normalizedActs[i] / (normalizedActs[j] + 0.01f);
                
                if (relativeAct > 1.5f) { // i is much stronger than j
                    // Apply strong inhibition to j
                    float inhibition = std::min(1.0f, (relativeAct - 1.5f) * 0.2f);
                    if (brain_) {
                        brain_->injectCurrent(memoryNeurons_[j], -inhibition * 3.0f);
                    }
                }
            }
        }
    }
    
    // Select winners based on threshold
    float competitionThreshold = 0.4f; // Normalized threshold
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        if (normalizedActs[i] >= competitionThreshold) {
            winners_.push_back(memoryNeurons_[i]);
        }
    }
    
    // If no winners, select the strongest trace
    if (winners_.empty() && !memoryNeurons_.empty()) {
        size_t strongestIdx = std::distance(memoryActivations_.begin(), 
                                            std::max_element(memoryActivations_.begin(), memoryActivations_.end()));
        winners_.push_back(memoryNeurons_[strongestIdx]);
    }
    
    NLM_LOG_DEBUG("Competition resulted in %zu winners", winners_.size());
}

// Check if neuron is part of the winning population
bool NeuralWorkingMemory::isWinning(NeuronId neuron) const {
    return std::find(winners_.begin(), winners_.end(), neuron) != winners_.end();
}

// Get overall memory activity level (average activation)
float NeuralWorkingMemory::getMemoryActivity() const {
    if (memoryActivations_.empty()) {
        return 0.0f;
    }
    
    float total = 0.0f;
    for (float act : memoryActivations_) {
        total += act;
    }
    
    return total / memoryActivations_.size();
}

// Create recurrent connections for memory maintenance
void NeuralWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first == from && conn.second == to) {
            return; // Connection already exists
        }
    }
    
    pImpl->maintenanceSynapses.emplace_back(from, to);
}

// Create all recurrent connections for current memory
void NeuralWorkingMemory::createRecurrentConnections() {
    pImpl->maintenanceSynapses.clear();
    
    if (memoryNeurons_.size() < 2) {
        return; // Need at least 2 neurons for recurrent connections
    }
    
    // Create a circular chain for persistent activity
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        size_t next = (i + 1) % memoryNeurons_.size();
        createRecurrentConnection(memoryNeurons_[i], memoryNeurons_[next], 0.5f);
    }
}

// Update recurrent connections for maintenance
void NeuralWorkingMemory::updateRecurrentConnections() {
    // Apply maintenance currents through recurrent connections
    for (const auto& conn : pImpl->maintenanceSynapses) {
        float fromActivation = getNeuronActivation(conn.first);
        
        if (fromActivation > 0.1f && brain_) {
            // Send maintenance signal to target neuron
            float maintenanceSignal = fromActivation * 2.0f;
            brain_->injectCurrent(conn.second, maintenanceSignal);
        }
    }
}

// Update all memory traces for maintenance
void NeuralWorkingMemory::updateMemoryTraces(TimestepDuration dt) {
    // Increment trace ages
    for (auto& age : pImpl->traceAges) {
        age += dt;
    }
    
    // Inject maintenance current to persistent neurons
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        NeuronId neuron = memoryNeurons_[i];
        float activation = memoryActivations_[i];
        
        if (activation > 0.1f) {
            // Inject maintenance current to sustain activity
            brain_->injectCurrent(neuron, activation * 2.0f);
            
            // Age the trace
            memoryTimestamps_[i]++;
            
            // Apply decay based on age
            if (memoryTimestamps_[i] > 1000) {
                // Apply age-dependent decay after 1000 steps
                float ageDecay = 1.0f - (pImpl->traceAges[i] * decayRate_);
                memoryActivations_[i] *= std::max(0.0f, ageDecay);
            }
        }
        
        // Apply burst-related enhancement for neural bursts
        if (pImpl->isBursting.size() > i && pImpl->isBursting[i]) {
            memoryActivations_[i] = std::min(1.0f, memoryActivations_[i] * 1.5f);
        }
    }
}

// Decay weak memory traces below threshold
void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < memoryActivations_.size(); ++i) {
        // Apply exponential decay
        memoryActivations_[i] *= (1.0f - decayRate_);
        
        // Check if trace has decayed below threshold
        if (memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        removeTrace(*it);
    }
}

// Find weakest trace for replacement
size_t NeuralWorkingMemory::findWeakestTrace() const {
    if (memoryActivations_.empty()) {
        return size_t(-1);
    }
    
    size_t weakestIdx = 0;
    float weakestActivation = memoryActivations_[0];
    
    for (size_t i = 1; i < memoryActivations_.size(); ++i) {
        if (memoryActivations_[i] < weakestActivation) {
            weakestIdx = i;
            weakestActivation = memoryActivations_[i];
        }
    }
    
    return weakestActivation < 0.01f ? weakestIdx : size_t(-1); // Only replace very weak traces
}

// Remove trace at specified index
void NeuralWorkingMemory::removeTrace(size_t idx) {
    if (idx >= memoryNeurons_.size()) {
        return;
    }
    
    // Remove from memory arrays
    memoryNeurons_.erase(memoryNeurons_.begin() + idx);
    memoryActivations_.erase(memoryActivations_.begin() + idx);
    memoryTimestamps_.erase(memoryTimestamps_.begin() + idx);
    
    // Remove from auxiliary arrays
    if (idx < pImpl->traceAges.size()) {
        pImpl->traceAges.erase(pImpl->traceAges.begin() + idx);
    }
    if (idx < pImpl->consolidateTrace.size()) {
        pImpl->consolidateTrace.erase(pImpl->consolidateTrace.begin() + idx);
    }
    if (idx < pImpl->competitionStrength.size()) {
        pImpl->competitionStrength.erase(pImpl->competitionStrength.begin() + idx);
    }
    if (idx < pImpl->isBursting.size()) {
        pImpl->isBursting.erase(pImpl->isBursting.begin() + idx);
    }
    
    // Update active traces
    std::vector<size_t> newActiveTraces;
    for (size_t activeIdx : activeTraces_) {
        if (activeIdx != idx && activeIdx > idx) {
            newActiveTraces.push_back(activeIdx - 1); // Adjust for removed element
        } else if (activeIdx != idx) {
            newActiveTraces.push_back(activeIdx);
        }
    }
    activeTraces_ = newActiveTraces;
    
    // Update maintenance connections
    std::vector<std::pair<NeuronId, NeuronId>> newConnections;
    for (const auto& conn : pImpl->maintenanceSynapses) {
        if (conn.first != memoryNeurons_[idx] && conn.second != memoryNeurons_[idx]) {
            // Adjust neuron IDs if they're from removed trace
            NeuronId newFrom = conn.first;
            NeuronId newTo = conn.second;
            
            // Map old neuron IDs to new indices if necessary
            for (size_t i = 0; i < memoryNeurons_.size() + 1; ++i) {
                if (i == idx) continue; // Skip removed neuron
                if (conn.first >= i && conn.first < memoryNeurons_.size()) {
                    newFrom = memoryNeurons_[i];
                }
                if (conn.second >= i && conn.second < memoryNeurons_.size()) {
                    newTo = memoryNeurons_[i];
                }
            }
            
            newConnections.emplace_back(newFrom, newTo);
        }
    }
    pImpl->maintenanceSynapses = newConnections;
}

// Replace trace with new neuron and activation
void NeuralWorkingMemory::replaceTrace(size_t idx, NeuronId newNeuron, float newActivation) {
    if (idx >= memoryNeurons_.size()) {
        // Add new trace instead
        storeToNeuron(newNeuron, newActivation);
        return;
    }
    
    // Replace existing trace
    memoryNeurons_[idx] = newNeuron;
    memoryActivations_[idx] = std::min(1.0f, newActivation);
    memoryTimestamps_[idx] = 0;
    
    // Update auxiliary structures
    if (idx < pImpl->traceAges.size()) {
        pImpl->traceAges[idx] = 0;
    }
    if (idx < pImpl->consolidateTrace.size()) {
        pImpl->consolidateTrace[idx] = false;
    }
    if (idx < pImpl->competitionStrength.size()) {
        pImpl->competitionStrength[idx] = newActivation;
    }
    if (idx < pImpl->isBursting.size()) {
        pImpl->isBursting[idx] = newActivation > 0.7f;
    }
    
    // Inject current for new activation
    if (brain_) {
        brain_->injectCurrent(newNeuron, newActivation * 5.0f);
    }
}

// Consolidate strong traces into persistent memory
void NeuralWorkingMemory::consolidateStrongTraces() {
    for (size_t i = 0; i < memoryNeurons_.size(); ++i) {
        float activation = memoryActivations_[i];
        
        if (activation >= consolidationThreshold_ && !pImpl->consolidateTrace[i]) {
            // Mark for consolidation
            pImpl->consolidateTrace[i] = true;
            
            // Store as persistent memory (in a real implementation, this would
            // be transferred to long-term memory storage)
            NLM_LOG_DEBUG("Consolidating trace for neuron %llu with activation %.2f", 
                         memoryNeurons_[i].getId(), activation);
        }
    }
}

// Get neurons currently in working memory (for attention processing)
const std::vector<NeuronId>& NeuralWorkingMemory::getMemoryNeurons() const {
    return memoryNeurons_;
}

void NeuralWorkingMemory::setCapacity(size_t cap) {
    if (cap == capacity_) {
        return; // No change needed
    }
    
    capacity_ = cap;
    
    // Remove excess traces if capacity decreased
    if (memoryNeurons_.size() > capacity_) {
        size_t excess = memoryNeurons_.size() - capacity_;
        for (size_t i = 0; i < excess; ++i) {
            removeTrace(memoryNeurons_.size() - 1 - i);
        }
    }
    
    NLM_LOG_DEBUG("Working memory capacity set to %zu", capacity_);
}

void NeuralWorkingMemory::setDecayRate(float rate) {
    assert(rate >= 0.0f && rate <= 1.0f && "Decay rate must be between 0.0 and 1.0");
    decayRate_ = rate;
}

// Get capacity
size_t NeuralWorkingMemory::getCapacity() const {
    return capacity_;
}

// Get decay rate
float NeuralWorkingMemory::getDecayRate() const {
    return decayRate_;
}

// End of NeuralWorkingMemory implementation

}} // namespace nlm
