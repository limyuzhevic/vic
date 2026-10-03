#include "Pruning.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    float ageThreshold;
    float strengthThreshold;
    float homeostaticTarget;
    float competitionFactor;
    float activityDependency;
    
    // Statistics
    size_t synapsesPruned;
    size_t totalSynapses;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f), ageThreshold(100.0f), 
             strengthThreshold(0.05f), homeostaticTarget(0.1f), 
             competitionFactor(0.5f), activityDependency(0.7f),
             synapsesPruned(0), totalSynapses(0) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = rate;
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Reset statistics
    pImpl->synapsesPruned = 0;
    pImpl->totalSynapses = 0;
    
    // Get all synapses in the brain
    auto allSynapses = brain->getAllSynapses();
    if (allSynapses.empty()) return;
    
    pImpl->totalSynapses = allSynapses.size();
    
    // Calculate current synaptic strength statistics
    float totalStrength = 0.0f;
    float avgStrength = 0.0f;
    
    for (const Synapse* synapse : allSynapses) {
        if (synapse) {
            totalStrength += synapse->getWeight();
        }
    }
    
    avgStrength = totalStrength / allSynapses.size();
    
    // Prune weak synapses
    pruneWeakSynapses(brain, allSynapses, avgStrength, rng);
    
    // Activity-dependent pruning
    activityDependentPruning(brain, allSynapses, rng);
    
    // Homeostatic pruning
    homeostaticPruning(brain, allSynapses, rng);
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) return false;
    
    // Check various criteria for pruning
    
    // 1. Check weight threshold
    if (synapse->getWeight() < pImpl->strengthThreshold) {
        return executePrune(synapse);
    }
    
    // 2. Check age (if available)
    // In real implementation, synapse would have age tracking
    // For now, skip this check
    
    // 3. Check activity correlation (if available)
    // In real implementation, would check pre/post activity correlation
    
    return false;
}

void Pruning::pruneWeakSynapses(Brain* brain, const std::vector<const Synapse*>& synapses, 
                                float avgStrength, RandomGenerator& rng) {
    // Prune synapses that are significantly weaker than average
    for (const Synapse* synapse : synapses) {
        if (!synapse) continue;
        
        float weight = synapse->getWeight();
        
        // Check if synapse should be pruned based on weight
        if (weight < avgStrength * pImpl->threshold) {
            // Additional probabilistic pruning
            float pruneProbability = pImpl->pruningRate * 
                                    std::exp((avgStrength - weight) * 0.5f); // Stronger bias for weaker synapses
            
            if (rng.bernoulli(pruneProbability)) {
                executePrune(const_cast<Synapse*>(synapse));
            }
        }
    }
}

void Pruning::activityDependentPruning(Brain* brain, const std::vector<const Synapse*>& synapses, 
                                       RandomGenerator& rng) {
    // Prune synapses with low activity correlation
    if (!brain) return;
    
    for (const Synapse* synapse : synapses) {
        if (!synapse) continue;
        
        // Get presynaptic neuron
        const Neuron* preNeuron = synapse->getPresynapticNeuron();
        if (!preNeuron) continue;
        
        // Calculate activity correlation
        float activityCorrelation = calculateActivityCorrelation(synapse, preNeuron);
        
        // Prune if activity correlation is too low
        if (activityCorrelation < pImpl->activityDependency * pImpl->threshold) {
            float pruneProbability = pImpl->pruningRate * (1.0f - activityCorrelation);
            
            if (rng.bernoulli(pruneProbability)) {
                executePrune(const_cast<Synapse*>(synapse));
            }
        }
    }
}

void Pruning::homeostaticPruning(Brain* brain, const std::vector<const Synapse*>& synapses, 
                                 RandomGenerator& rng) {
    if (!brain) return;
    
    // Calculate current connectivity level
    float currentConnectivity = static_cast<float>(pImpl->totalSynapses - pImpl->synapsesPruned) /
                                (static_cast<float>(brain->getAllNeurons().size() * brain->getAllNeurons().size()));
    
    // Homeostatic target is typically lower than maximum possible
    float targetConnectivity = pImpl->homeostaticTarget;
    
    if (currentConnectivity > targetConnectivity) {
        // Need to prune more synapses to reach homeostatic balance
        float excess = currentConnectivity - targetConnectivity;
        size_t synapsesToPrune = static_cast<size_t>(excess * pImpl->totalSynapses * 10.0f);
        
        if (synapsesToPrune > 0) {
            // Select synapses for homeostatic pruning
            selectHomeostaticSynapses(brain, synapses, synapsesToPrune, rng);
        }
    }
}

void Pruning::selectHomeostaticSynapses(Brain* brain, const std::vector<const Synapse*>& synapses,
                                        size_t numToPrune, RandomGenerator& rng) {
    if (numToPrune == 0 || synapses.empty()) return;
    
    // Create weighted list for selection (weaker synapses have higher probability)
    std::vector<std::pair<const Synapse*, float>> weightedSynapses;
    
    for (const Synapse* synapse : synapses) {
        if (!synapse) continue;
        
        float weight = synapse->getWeight();
        float selectionWeight = std::max(0.0f, pImpl->homeostaticTarget - weight);
        weightedSynapses.emplace_back(synapse, selectionWeight);
    }
    
    if (weightedSynapses.empty()) return;
    
    // Normalize weights for probability distribution
    float totalWeight = 0.0f;
    for (const auto& pair : weightedSynapses) {
        totalWeight += pair.second;
    }
    
    if (totalWeight <= 0.0f) return;
    
    // Select synapses for pruning
    std::vector<const Synapse*> selectedSynapses;
    selectedSynapses.reserve(numToPrune);
    
    for (size_t i = 0; i < numToPrune; ++i) {
        // Roulette wheel selection
        float selection = rng.uniformReal(0.0f, totalWeight);
        float cumulative = 0.0f;
        
        for (const auto& pair : weightedSynapses) {
            cumulative += pair.second;
            if (selection <= cumulative) {
                selectedSynapses.push_back(pair.first);
                break;
            }
        }
    }
    
    // Execute pruning
    for (const Synapse* synapse : selectedSynapses) {
        executePrune(const_cast<Synapse*>(synapse));
    }
}

float Pruning::calculateActivityCorrelation(const Synapse* synapse, const Neuron* preNeuron) {
    if (!synapse || !preNeuron) return 0.0f;
    
    // Calculate correlation between presynaptic activity and synaptic weight
    float preActivity = preNeuron->getActivity();
    float synapticWeight = synapse->getWeight();
    
    // Simple correlation model
    float correlation = preActivity * synapticWeight;
    
    // Normalize to reasonable range
    correlation = std::clamp(correlation, 0.0f, 1.0f);
    
    return correlation;
}

bool Pruning::executePrune(Synapse* synapse) {
    if (!synapse) return false;
    
    // Get presynaptic and postsynaptic neurons
    Neuron* preNeuron = synapse->getPresynapticNeuron();
    
    // Check if synapse can be safely pruned
    if (preNeuron && preNeuron->canPruneSynapse(synapse)) {
        // Execute pruning
        bool success = synapse->markForPruning();
        if (success) {
            pImpl->synapsesPruned++;
        }
        return success;
    }
    
    return false;
}

} // namespace nlm
