#pragma once

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct StructuralPlasticity::Impl {
    float synaptogenesisRate;
    float pruningRate;
    float maxSynapsesPerNeuron;
    float minSynapsesPerNeuron;
    float growthProbability;
    float retractionProbability;
    
    Impl() : synaptogenesisRate(0.0001f), pruningRate(0.00001f), 
             maxSynapsesPerNeuron(100), minSynapsesPerNeuron(1),
             growthProbability(0.01f), retractionProbability(0.001f) {}
};

StructuralPlasticity::StructuralPlasticity() : pImpl(new Impl) {}

StructuralPlasticity::~StructuralPlasticity() = default;

void StructuralPlasticity::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse) return;
    
    // Structural plasticity: modify the physical connectivity of the neural network
    // This is activity-dependent synaptic formation and elimination
    
    float totalGrowth = 0.0f;
    float totalPruning = 0.0f;
    
    // Synaptogenesis (formation of new synapses)
    float growthRate = pImpl->synaptogenesisRate;
    
    // If there's sufficient activity, promote synaptogenesis
    float activity = calculateSynapseActivity(preSpikes, postSpikes);
    if (activity > 0.5f) {
        growthRate *= activity * pImpl->growthProbability;
    }
    
    totalGrowth += growthRate;
    
    // Pruning (elimination of existing synapses)
    float pruneRate = pImpl->pruningRate;
    
    // Weak synapses are preferentially pruned
    float weight = synapse->getWeight();
    if (std::abs(weight) < 0.1f) {
        pruneRate *= (0.1f - std::abs(weight)) / 0.1f;  // Weaker -> more pruning
    }
    
    totalPruning += pruneRate;
    
    // Apply structural changes
    if (totalGrowth > totalPruning) {
        // Growth dominates: add new synapse connections
        float netGrowth = totalGrowth - totalPruning;
        
        // Create new synapse objects
        for (int i = 0; i < static_cast<int>(netGrowth * 10); ++i) {  // Scale for simulation
            // In a real implementation, this would create actual new synapses
            // For now, we just log the structural change
            NLM_LOG_DEBUG(std::string("Structural growth: ") + std::to_string(i) + 
                         " new synapses formed");
        }
    } else if (totalPruning > totalGrowth) {
        // Pruning dominates: eliminate weak synapses
        float netPruning = totalPruning - totalGrowth;
        
        // Remove weak synapses
        for (int i = 0; i < static_cast<int>(netPruning * 5); ++i) {
            // In a real implementation, this would remove synapses
            NLM_LOG_DEBUG(std::string("Structural pruning: ") + std::to_string(i) + 
                         " synapses eliminated");
        }
    }
}

void StructuralPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Structural plasticity affects both weights and connectivity
    // For weight changes, we apply the delta normally
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* StructuralPlasticity::getName() const {
    return "StructuralPlasticity";
}

void StructuralPlasticity::setSynaptogenesisRate(float rate) {
    pImpl->synaptogenesisRate = std::clamp(rate, 0.0f, 0.01f);
}

float StructuralPlasticity::getSynaptogenesisRate() const {
    return pImpl->synaptogenesisRate;
}

void StructuralPlasticity::setPruningRate(float rate) {
    pImpl->pruningRate = std::clamp(rate, 0.0f, 0.001f);
}

float StructuralPlasticity::getPruningRate() const {
    return pImpl->pruningRate;
}

void StructuralPlasticity::setMaxSynapsesPerNeuron(float max) {
    pImpl->maxSynapsesPerNeuron = std::clamp(max, 1.0f, 1000.0f);
}

float StructuralPlasticity::getMaxSynapsesPerNeuron() const {
    return pImpl->maxSynapsesPerNeuron;
}

void StructuralPlasticity::setMinSynapsesPerNeuron(float min) {
    pImpl->minSynapsesPerNeuron = std::clamp(min, 0.0f, 10.0f);
}

float StructuralPlasticity::getMinSynapsesPerNeuron() const {
    return pImpl->minSynapsesPerNeuron;
}

float StructuralPlasticity::calculateSynapseActivity(const std::vector<Timestamp>& preSpikes,
                                                    const std::vector<Timestamp>& postSpikes) const {
    if (preSpikes.empty() && postSpikes.empty()) return 0.0f;
    
    // Simple activity metric: number of spike pairs within 50ms
    float activity = 0.0f;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < 50.0f) {
                activity += 1.0f;
            }
        }
    }
    
    // Normalize by maximum possible pairs
    float maxPairs = static_cast<float>(preSpikes.size() * postSpikes.size());
    return maxPairs > 0.0f ? activity / maxPairs : 0.0f;
}

} // namespace nlm