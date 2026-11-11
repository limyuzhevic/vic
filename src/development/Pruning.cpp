#include "Pruning.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <unordered_set>
#include <cmath>

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    float minWeight;
    float maxWeight;
    float activityThreshold;
    
    Impl() : threshold(0.01f), pruningRate(0.0001f), minWeight(-0.5f), maxWeight(0.5f), activityThreshold(1.0f) {}
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
    // TODO PHASE 2: Implement real pruning
    // PLACEHOLDER: Weak synapses are removed probabilistically
    // More sophisticated implementation based on biological principles
    
    if (!brain) return;
    
    auto regions = brain->getRegions();
    std::vector<Synapse*> synapsesToRemove;
    
    // Collect all synapses for evaluation
    for (const auto& region : regions) {
        auto synapses = region->getSynapses();
        for (const auto& syn : synapses) {
            synapsesToRemove.push_back(syn.get());
        }
    }
    
    if (synapsesToRemove.empty()) return;
    
    // Evaluate each synapse for potential pruning
    std::vector<float> pruningProbabilities;
    
    for (Synapse* synapse : synapsesToRemove) {
        float probability = calculatePruningProbability(brain, synapse, rng);
        if (probability > 0.0f) {
            pruningProbabilities.push_back(probability);
        }
    }
    
    // Sort by pruning probability (highest first)
    std::vector<size_t> indices(pruningProbabilities.size());
    for (size_t i = 0; i < pruningProbabilities.size(); ++i) {
        indices[i] = i;
    }
    
    std::sort(indices.begin(), indices.end(),
              [&](size_t a, size_t b) {
                  return pruningProbabilities[a] > pruningProbabilities[b];n              });
    
    // Prune synapses based on calculated probabilities
    size_t maxToPrune = static_cast<size_t>(synapsesToRemove.size() * pImpl->pruningRate);
    maxToPrune = std::min(maxToPrune, synapsesToRemove.size());
    
    for (size_t i = 0; i < maxToPrune; ++i) {
        size_t synapseIndex = indices[i];
        if (synapseIndex < synapsesToRemove.size()) {
            pruneSynapse(synapsesToRemove[synapseIndex]);
        }
    }
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    // TODO PHASE 2: Implement real synapse removal
    // More sophisticated implementation with biological constraints
    
    if (!synapse) return false;
    
    // Check if synapse meets pruning criteria
    if (!shouldPruneSynapse(synapse)) {
        return false;
    }
    
    // Get source and target neurons
    Neuron* sourceNeuron = nullptr;
    Neuron* targetNeuron = nullptr;
    
    auto regions = synapse->getBrain()->getRegions();
    for (const auto& region : regions) {
        auto populations = region->getPopulations();
        for (const auto& pop : populations) {
            auto neurons = pop->getNeurons();
            for (const auto* neuron : neurons) {
                if (neuron->getId() == synapse->getSourceNeuron()) {
                    sourceNeuron = const_cast<Neuron*>(neuron);
                }
                if (neuron->getId() == synapse->getDestinationNeuron()) {
                    targetNeuron = const_cast<Neuron*>(neuron);
                }
            }
        }
    }
    
    if (!sourceNeuron || !targetNeuron) {
        return false;
    }
    
    // Remove synapse from neurons' connection lists
    // Note: This requires access to the brain's region management
    // For simplicity, we'll assume the synapse is properly removed from the brain's data structures
    
    return true;
}

bool Pruning::shouldPruneSynapse(Synapse* synapse) const {
    if (!synapse) return false;
    
    // Check multiple criteria for synapse pruning
    
    // Criterion 1: Weak weight
    float weight = synapse->getWeight();
    if (std::abs(weight) < pImpl->minWeight) {
        return true;
    }
    
    // Criterion 2: Unstable weight (large changes)
    if (synapse->getEfficacy() < pImpl->threshold) {
        return true;
    }
    
    // Criterion 3: Low neuronal activity
    // This would require access to the neurons' firing rates
    // For now, we'll skip this check
    
    // Criterion 4: Inappropriate synapse type
    SynapseType type = synapse->getType();
    if (type == SynapseType::None) {
        return true;
    }
    
    // Criterion 5: Old synapse (based on age)
    // This would require tracking synapse creation time
    // For now, we'll skip this check
    
    return false;
}

float Pruning::calculatePruningProbability(Brain* brain, Synapse* synapse, RandomGenerator& rng) const {
    if (!synapse) return 0.0f;
    
    float probability = pImpl->pruningRate;
    
    // Factor 1: Weak weight (more likely to be pruned)
    float weight = synapse->getWeight();
    if (std::abs(weight) < 0.1f) {
        probability *= 2.0f;
    } else if (std::abs(weight) > 0.8f) {
        probability *= 0.5f;
    }
    
    // Factor 2: Low efficacy (unstable synapses)
    float efficacy = synapse->getEfficacy();
    if (efficacy < pImpl->threshold) {
        probability *= 1.5f;
    }
    
    // Factor 3: Low neuronal activity
    // This would require access to neurons' firing rates
    // For now, we'll use a placeholder
    
    // Factor 4: Synapse age (older synapses more likely to be pruned)
    // This would require tracking synapse creation time
    
    // Factor 5: Network state
    // Synapses in highly active networks might be preserved
    // Synapses in inactive networks might be pruned
    
    // Apply developmental stage modulation
    float developmentalFactor = 1.0f;  // Would depend on developmental stage
    probability *= developmentalFactor;
    
    // Add random variation
    float randomFactor = rng.uniformReal(0.8f, 1.2f);
    probability *= randomFactor;
    
    // Clamp probability to reasonable bounds
    return std::min(probability, 1.0f);
}

} // namespace nlm
