#pragma once

#include "NeuralWorkingMemory.hpp"
#include "../brain/Brain.hpp"
#include "../plasticity/STDP.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace nlm {

// Enhanced NeuralWorkingMemory implementation
struct NeuralWorkingMemory::Impl {
    Brain* brain;
    size_t capacity;
    float decayRate;
    float consolidationThreshold;
    float attractorStrength;
    
    // Memory traces
    std::vector<NeuronId> neurons;
    std::vector<float> activations;
    std::vector<SimulationStep> timestamps;
    std::vector<float> stabilities;
    
    // Synaptic connections for memory maintenance
    std::vector<std::pair<NeuronId, NeuronId>> maintenanceConnections;
    std::vector<float> connectionWeights;
    
    // Activity history for learning
    std::vector<std::vector<float>> activityHistory;
    
    // Neural indices and regions
    std::vector<size_t> neuronIndices;
    std::vector<RegionId> neuronRegions;
    
    // Memory strength and encoding
    std::vector<float> memoryStrengths;
    std::vector<SimulationStep> encodingTimes;
    
    // Competition and attention
    std::vector<NeuronId> winners;
    
    // Integration with plasticity systems
    std::shared_ptr<STDP> stdp;
    float plasticityModulation;
    
    Impl() : brain(nullptr), capacity(100), decayRate(0.01f), 
             consolidationThreshold(0.3f), attractorStrength(0.5f),
             plasticityModulation(1.0f) {}
};

NeuralWorkingMemory::NeuralWorkingMemory()
    : pImpl(new Impl)
{
    pImpl->capacity = 100;
    pImpl->decayRate = 0.01f;
    pImpl->consolidationThreshold = 0.3f;
    pImpl->attractorStrength = 0.5f;
    pImpl->plasticityModulation = 1.0f;
}

NeuralWorkingMemory::~NeuralWorkingMemory() = default;

void NeuralWorkingMemory::initialize(Brain* brain, std::shared_ptr<STDP> stdp) {
    pImpl->brain = brain;
    pImpl->stdp = stdp;
    NLM_LOG_INFO("NeuralWorkingMemory initialized with capacity: " + 
                std::to_string(pImpl->capacity));
}

void NeuralWorkingMemory::store(const std::vector<float>& pattern, float strength) {
    if (pattern.empty() || !pImpl->brain) return;
    
    // Find existing memory or create new one
    bool foundMatch = false;
    for (size_t i = 0; i < pImpl->memoryNeurons_.size(); ++i) {
        if (patternMatch(pattern, pImpl->memoryActivations_[i])) {
            // Strengthen existing memory
            pImpl->memoryActivations_[i] = std::min(1.0f, 
                pImpl->memoryActivations_[i] + strength * 0.5f);
            pImpl->memoryTimestamps_[i] = 0;
            pImpl->memoryStrengths[i] = std::min(1.0f, 
                pImpl->memoryStrengths[i] + strength * 0.3f);
            foundMatch = true;
            break;
        }
    }
    
    if (!foundMatch) {
        // Create new memory if capacity allows
        if (pImpl->memoryNeurons_.size() < pImpl->capacity) {
            pImpl->memoryNeurons_.push_back(i);  // Use placeholder, should be actual neuron
            pImpl->memoryActivations_.push_back(strength);
            pImpl->memoryTimestamps_.push_back(0);
            pImpl->memoryStrengths_.push_back(strength);
            pImpl->encodingTimes_.push_back(0);
            
            // Create attractor connections
            createAttractorNetwork(pImpl->memoryNeurons_.size() - 1);
        }
    }
}

void NeuralWorkingMemory::storeToNeuron(NeuronId neuron, float activation) {
    // Find or add this neuron to memory
    auto it = std::find(pImpl->memoryNeurons_.begin(), pImpl->memoryNeurons_.end(), neuron);
    
    if (it != pImpl->memoryNeurons_.end()) {
        size_t idx = std::distance(pImpl->memoryNeurons_.begin(), it);
        pImpl->memoryActivations_[idx] = activation;
        pImpl->memoryTimestamps_[idx] = 0;
    } else if (pImpl->memoryNeurons_.size() < pImpl->capacity) {
        pImpl->memoryNeurons_.push_back(neuron);
        pImpl->memoryActivations_.push_back(activation);
        pImpl->memoryTimestamps_.push_back(0);
        pImpl->memoryStrengths_.push_back(activation);
        pImpl->encodingTimes_.push_back(0);
        
        // Inject current for immediate effect
        if (pImpl->brain_) {
            pImpl->brain_->injectCurrent(neuron, activation * 5.0f);
        }
    }
}

bool NeuralWorkingMemory::patternMatch(const std::vector<float>& pattern1, 
                                       const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size()) return false;
    
    float similarity = 0.0f;
    for (size_t i = 0; i < pattern1.size(); ++i) {
        similarity += std::abs(pattern1[i] - pattern2[i]);
    }
    
    return similarity < 0.1f;  // 10% difference tolerance
}

void NeuralWorkingMemory::createAttractorNetwork(size_t newIndex) {
    if (newIndex == 0) return;
    
    // Create attractor connections
    NeuronId newNeuron = pImpl->memoryNeurons_[newIndex];
    NeuronId prevNeuron = pImpl->memoryNeurons_[newIndex - 1];
    
    pImpl->maintenanceConnections_.push_back({prevNeuron, newNeuron});
    pImpl->connectionWeights_.push_back(0.5f);
}

void NeuralWorkingMemory::update(float dt) {
    if (!pImpl->brain_) return;
    
    // Update all memory traces
    for (size_t i = 0; i < pImpl->memoryNeurons_.size(); ++i) {
        updateMemoryTrace(i, dt);
    }
    
    // Run competitive selection
    runCompetitiveSelection();
    
    // Apply plasticity modulation
    applyPlasticityModulation(dt);
    
    // Consolidate stable memories
    consolidateStableMemories();
}

void NeuralWorkingMemory::updateMemoryTrace(size_t index, float dt) {
    NeuronId neuron = pImpl->memoryNeurons_[index];
    float activation = pImpl->memoryActivations_[index];
    
    // Inject maintenance current
    pImpl->brain_->injectCurrent(neuron, activation * 2.0f);
    
    // Age the trace
    pImpl->memoryTimestamps_[index]++;
    
    // Apply decay based on age
    if (pImpl->memoryTimestamps_[index] > 1000) {
        float ageDecay = 1.0f - (pImpl->memoryTimestamps_[index] - 1000) * 0.001f;
        activation *= ageDecay;
        pImpl->memoryActivations_[index] = activation;
    }
    
    // Check if trace should be strengthened
    if (activation > pImpl->decayRate_) {
        strengthenTrace(index);
    }
    
    // Update activity history
    recordActivity(neuron, activation, dt);
}

void NeuralWorkingMemory::strengthenTrace(size_t index) {
    NeuronId neuron = pImpl->memoryNeurons_[index];
    float activation = pImpl->memoryActivations_[index];
    
    // Apply Hebbian-like strengthening based on neural activity
    if (pImpl->brain_) {
        // Get neighboring neurons from attractor network
        for (size_t i = 0; i < pImpl->maintenanceConnections_.size(); ++i) {
            auto& conn = pImpl->maintenanceConnections_[i];
            if (conn.first == neuron) {
                NeuronId target = conn.second;
                // Strengthen connection
                float weightChange = pImpl->attractorStrength_ * activation * 0.01f;
                
                if (pImpl->stdp_) {
                    pImpl->stdp_->applyWeightChange(pImpl->brain_->getSynapse(conn.first, conn.second), 
                                                  weightChange);
                }
            }
        }
    }
    
    // Increase memory strength
    pImpl->memoryStrengths_[index] = std::min(1.0f, 
        pImpl->memoryStrengths_[index] + 0.01f);
}

void NeuralWorkingMemory::recordActivity(NeuronId neuron, float activation, float dt) {
    // Find neuron index
    auto it = std::find(pImpl->memoryNeurons_.begin(), pImpl->memoryNeurons_.end(), neuron);
    if (it == pImpl->memoryNeurons_.end()) return;
    
    size_t index = std::distance(pImpl->memoryNeurons_.begin(), it);
    
    // Add to activity history
    pImpl->activityHistory_.push_back(std::vector<float>());
    for (float act : pImpl->memoryActivations_) {
        pImpl->activityHistory_.back().push_back(act);
    }
    
    // Keep history manageable
    if (pImpl->activityHistory_.size() > 1000) {
        pImpl->activityHistory_.erase(pImpl->activityHistory_.begin());
    }
}

void NeuralWorkingMemory::runCompetitiveSelection() {
    pImpl->winners_.clear();
    
    if (pImpl->memoryActivations_.empty()) return;
    
    // Find threshold for selection
    float maxActivation = *std::max_element(pImpl->memoryActivations_.begin(), 
                                           pImpl->memoryActivations_.end());
    float minActivation = *std::min_element(pImpl->memoryActivations_.begin(), 
                                           pImpl->memoryActivations_.end());
    float threshold = pImpl->competitionThreshold_ * (maxActivation - minActivation) + minActivation;
    
    // Select winners
    for (size_t i = 0; i < pImpl->memoryNeurons_.size(); ++i) {
        if (pImpl->memoryActivations_[i] >= threshold) {
            pImpl->winners_.push_back(pImpl->memoryNeurons_[i]);
        }
    }
    
    // Apply competition effects
    applyCompetitionEffects();
}

void NeuralWorkingMemory::applyCompetitionEffects() {
    if (pImpl->winners_.empty() || !pImpl->brain_) return;
    
    // Apply excitation to winners
    for (NeuronId winner : pImpl->winners_) {
        pImpl->brain_->injectCurrent(winner, pImpl->competitionExcitation_ * 2.0f);
    }
    
    // Apply inhibition to non-winners
    for (size_t i = 0; i < pImpl->memoryNeurons_.size(); ++i) {
        bool isWinner = std::find(pImpl->winners_.begin(), pImpl->winners_.end(), 
                                 pImpl->memoryNeurons_[i]) != pImpl->winners_.end();
        
        if (!isWinner) {
            pImpl->brain_->injectCurrent(pImpl->memoryNeurons_[i], 
                                        -pImpl->memoryActivations_[i] * pImpl->competitionInhibition_);
        }
    }
}

void NeuralWorkingMemory::consolidateStableMemories() {
    // Move stable memories to episodic memory
    std::vector<size_t> toConsolidate;
    
    for (size_t i = 0; i < pImpl->memoryNeurons_.size(); ++i) {
        if (pImpl->memoryStrengths_[i] > pImpl->consolidationThreshold_ && 
            pImpl->memoryTimestamps_[i] > 500) {
            toConsolidate.push_back(i);
        }
    }
    
    // Consolidate memories (simplified - would integrate with episodic memory)
    for (auto it = toConsolidate.rbegin(); it != toConsolidate.rend(); ++it) {
        size_t index = *it;
        
        // Create memory record for episodic memory
        // In a real implementation, this would call episodic memory
        
        // Remove from working memory
        pImpl->memoryNeurons_.erase(pImpl->memoryNeurons_.begin() + index);
        pImpl->memoryActivations_.erase(pImpl->memoryActivations_.begin() + index);
        pImpl->memoryTimestamps_.erase(pImpl->memoryTimestamps_.begin() + index);
        pImpl->memoryStrengths_.erase(pImpl->memoryStrengths_.begin() + index);
        pImpl->encodingTimes_.erase(pImpl->encodingTimes_.begin() + index);
        
        // Update indices for remaining memories
        if (index > 0) {
            for (auto& conn : pImpl->maintenanceConnections_) {
                if (conn.first > pImpl->memoryNeurons_[index]) {
                    // Adjust indices for remaining connections
                }
            }
        }
    }
}

void NeuralWorkingMemory::applyPlasticityModulation(float dt) {
    if (!pImpl->stdp_) return;
    
    // Modulate STDP based on working memory activity
    float avgActivation = getAverageActivation();
    
    if (avgActivation > 0.5f) {
        // High activation increases learning rate
        pImpl->stdp_->setLearningRate(std::min(2.0f, 
            pImpl->stdp_->getLearningRate() * (1.0f + avgActivation * 0.5f)));
    } else {
        // Low activation decreases learning rate
        pImpl->stdp_->setLearningRate(std::max(0.1f, 
            pImpl->stdp_->getLearningRate() * (1.0f - avgActivation * 0.3f)));
    }
}

float NeuralWorkingMemory::getAverageActivation() const {
    if (pImpl->memoryActivations_.empty()) return 0.0f;
    
    float total = std::accumulate(pImpl->memoryActivations_.begin(), 
                                 pImpl->memoryActivations_.end(), 0.0f);
    return total / pImpl->memoryActivations_.size();
}

void NeuralWorkingMemory::decayWeakTraces() {
    std::vector<size_t> toRemove;
    
    for (size_t i = 0; i < pImpl->memoryActivations_.size(); ++i) {
        pImpl->memoryActivations_[i] *= (1.0f - pImpl->decayRate_);
        
        if (pImpl->memoryActivations_[i] < 0.01f) {
            toRemove.push_back(i);
        }
    }
    
    // Remove weak traces (in reverse order to maintain indices)
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        pImpl->memoryNeurons_.erase(pImpl->memoryNeurons_.begin() + *it);
        pImpl->memoryActivations_.erase(pImpl->memoryActivations_.begin() + *it);
        pImpl->memoryTimestamps_.erase(pImpl->memoryTimestamps_.begin() + *it);
        pImpl->memoryStrengths_.erase(pImpl->memoryStrengths_.begin() + *it);
        pImpl->encodingTimes_.erase(pImpl->encodingTimes_.begin() + *it);
    }
}

void NeuralWorkingMemory::clear() {
    pImpl->memoryNeurons_.clear();
    pImpl->memoryActivations_.clear();
    pImpl->memoryTimestamps_.clear();
    pImpl->memoryStrengths_.clear();
    pImpl->encodingTimes_.clear();
    pImpl->activityHistory_.clear();
    pImpl->winners_.clear();
    pImpl->maintenanceConnections_.clear();
    pImpl->connectionWeights_.clear();
}

} // namespace nlm
