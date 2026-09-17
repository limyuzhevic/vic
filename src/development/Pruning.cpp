#include "Pruning.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    float developmentalInfluence;
    float homeostaticTarget;
    float strengthThreshold;
    float activityThreshold;
    float minConnectionProbability;
    
    // Statistics
    size_t totalSynapsesPruned;
    size_t synapsesPrunedThisStep;
    float pruningEfficiency;
    
    // Activity history for activity-dependent pruning
    std::unordered_map<NeuronId, float> neuronActivityHistory;
    double activityUpdateTime;
    
    Impl() 
        : threshold(0.01f), pruningRate(0.0001f), developmentalInfluence(1.0f),
          homeostaticTarget(0.1f), strengthThreshold(0.05f), activityThreshold(0.001f),
          minConnectionProbability(0.05f),
          totalSynapsesPruned(0), synapsesPrunedThisStep(0),
          pruningEfficiency(0.0f), activityUpdateTime(0.0),
          neuronActivityHistory() {}
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
    pImpl->pruningRate = std::clamp(rate, 0.0f, 0.01f);
}

float Pruning::getDevelopmentalInfluence() const {
    return pImpl->developmentalInfluence;
}

void Pruning::setDevelopmentalInfluence(float factor) {
    pImpl->developmentalInfluence = std::clamp(factor, 0.0f, 2.0f);
}

float Pruning::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

void Pruning::setHomeostaticTarget(float target) {
    pImpl->homeostaticTarget = std::clamp(target, 0.0f, 0.5f);
}

float Pruning::getStrengthThreshold() const {
    return pImpl->strengthThreshold;
}

void Pruning::setStrengthThreshold(float threshold) {
    pImpl->strengthThreshold = threshold;
}

float Pruning::getActivityThreshold() const {
    return pImpl->activityThreshold;
}

void Pruning::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = threshold;
}

float Pruning::getCurrentDensity(Brain* brain) const {
    if (!brain) return 0.0f;
    
    size_t totalNeurons = 0;
    size_t totalPossibleSynapses = 0;
    
    for (const auto& region : brain->getRegions()) {
        totalNeurons += region->getTotalNeuronCount();
        if (totalNeurons >= 2) {
            totalPossibleSynapses += totalNeurons * (totalNeurons - 1);
        }
    }
    
    if (totalPossibleSynapses == 0) return 0.0f;
    
    size_t actualSynapses = brain->getTotalSynapseCount();
    return static_cast<float>(actualSynapses) / static_cast<float>(totalPossibleSynapses);
}

bool Pruning::needsPruning(Brain* brain) const {
    if (!brain) return false;
    
    float currentDensity = getCurrentDensity(brain);
    float homeostaticLevel = pImpl->homeostaticTarget / pImpl->developmentalInfluence;
    
    // Need pruning if density is above homeostatic level with some tolerance
    return currentDensity > (homeostaticLevel * 1.1f);
}

bool Pruning::removeWeakSynapse(Brain* brain, Synapse* synapse, 
                                float sourceActivity, float targetActivity,
                                RandomGenerator& rng) {
    if (!brain || !synapse) return false;
    
    NeuronId sourceId = synapse->getSourceNeuron();
    NeuronId targetId = synapse->getDestinationNeuron();
    
    // Check pruning conditions
    bool shouldPrune = false;
    
    // 1. Strength-based pruning (weak synapses)
    float weight = std::abs(synapse->getWeight());
    if (weight < pImpl->strengthThreshold) {
        // Weak synapse - prune with probability based on pruning rate
        float pruneProbability = pImpl->pruningRate * pImpl->developmentalInfluence;
        pruneProbability = std::min(pruneProbability, 1.0f);
        
        if (rng.bernoulli(pruneProbability)) {
            shouldPrune = true;
            NLM_LOG_DEBUG("Pruning: Weak synapse (weight=" + std::to_string(weight) + 
                         ") between neurons " + std::to_string(sourceId.index()) + " and " +
                         std::to_string(targetId.index()) + " pruned");
        }
    }
    
    // 2. Activity-based pruning (less active synapses)
    float activityCorrelation = 0.0f;
    if (sourceActivity > 0.0f && targetActivity > 0.0f) {
        activityCorrelation = std::min(1.0f, (sourceActivity + targetActivity) / 2.0f);
    }
    
    if (activityCorrelation < pImpl->activityThreshold) {
        // Low activity correlation - prune with probability
        float pruneProbability = pImpl->pruningRate * 0.5f * pImpl->developmentalInfluence;
        pruneProbability = std::min(pruneProbability, 1.0f);
        
        if (rng.bernoulli(pruneProbability)) {
            shouldPrune = true;
            NLM_LOG_DEBUG("Pruning: Low activity synapse (correlation=" + 
                         std::to_string(activityCorrelation) + ") between neurons " +
                         std::to_string(sourceId.index()) + " and " +
                         std::to_string(targetId.index()) + " pruned");
        }
    }
    
    // 3. Homeostatic pruning (maintain target density)
    float currentDensity = getCurrentDensity(brain);
    float targetDensity = pImpl->homeostaticTarget;
    
    if (currentDensity > (targetDensity * 1.2f)) {
        // High density - aggressive pruning
        if (rng.bernoulli(pImpl->pruningRate * 2.0f * pImpl->developmentalInfluence)) {
            shouldPrune = true;
            NLM_LOG_DEBUG("Pruning: Homeostatic pruning (density=" + 
                         std::to_string(currentDensity) + ") synapse between neurons " +
                         std::to_string(sourceId.index()) + " and " +
                         std::to_string(targetId.index()) + " pruned");
        }
    }
    
    if (shouldPrune) {
        // Remove the synapse from its region
        for (auto& region : brain->getRegions()) {
            Synapse* foundSyn = region->getSynapse(synapse->getId());
            if (foundSyn == synapse) {
                // Remove synapse from region (this is a simplified version)
                // In real implementation, would need to actually remove from region's synapse storage
                syn->setWeight(0.0f);  // Mark as pruned
                return true;
            }
        }
    }
    
    return false;
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) return false;
    
    // For now, mark synapse for removal by zeroing its weight
    // Actual removal would require modifying the region's synapse storage
    synapse->setWeight(0.0f);
    return true;
}

size_t Pruning::getTotalSynapsesPruned() const {
    return pImpl->totalSynapsesPruned;
}

size_t Pruning::getSynapsesPrunedThisStep() const {
    return pImpl->synapsesPrunedThisStep;
}

float Pruning::getPruningEfficiency() const {
    return pImpl->pruningEfficiency;
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    pImpl->synapsesPrunedThisStep = 0;
    
    if (!brain) return;
    
    NLM_LOG_DEBUG("Pruning update: Current density = " + 
                 std::to_string(getCurrentDensity(brain)) + ", Target = " +
                 std::to_string(pImpl->homeostaticTarget));
    
    // Update activity history with recent spikes
    double currentTime = 0.0;  // TODO: Get from brain simulation time
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        
        for (auto* neuron : neurons) {
            const auto& spikes = neuron->getSpikeHistory();
            float recentActivity = 0.0f;
            
            if (!spikes.empty()) {
                // Calculate activity based on recent spikes
                recentActivity = static_cast<float>(spikes.size());
                
                // Update activity history (exponential decay)
                float decayFactor = 0.9f;
                if (pImpl->neuronActivityHistory.find(neuron->getId()) != pImpl->neuronActivityHistory.end()) {
                    pImpl->neuronActivityHistory[neuron->getId()] = 
                        pImpl->neuronActivityHistory[neuron->getId()] * decayFactor + recentActivity * (1.0f - decayFactor);
                } else {
                    pImpl->neuronActivityHistory[neuron->getId()] = recentActivity;
                }
            }
        }
    }
    
    // Check if we need to prune synapses
    if (!needsPruning(brain)) {
        NLM_LOG_DEBUG("Pruning: Network at target density, skipping pruning");
        pImpl->pruningEfficiency = 1.0f;  // Efficient - no need to prune
        return;
    }
    
    // Activity-dependent pruning
    for (auto& region : brain->getRegions()) {
        for (auto& syn : region->getSynapses()) {
            NeuronId sourceId = syn->getSourceNeuron();
            NeuronId targetId = syn->getDestinationNeuron();
            
            // Get current activity of both neurons
            float sourceActivity = 0.0f;
            float targetActivity = 0.0f;
            
            auto itSource = pImpl->neuronActivityHistory.find(sourceId);
            if (itSource != pImpl->neuronActivityHistory.end()) {
                sourceActivity = itSource->second;
            } else {
                // Get activity from synapse spike history
                const auto& preSpikes = syn->getPreSpikeHistory();
                sourceActivity = static_cast<float>(preSpikes.size());
            }
            
            auto itTarget = pImpl->neuronActivityHistory.find(targetId);
            if (itTarget != pImpl->neuronActivityHistory.end()) {
                targetActivity = itTarget->second;
            } else {
                // Get activity from synapse spike history
                const auto& postSpikes = syn->getPostSpikeHistory();
                targetActivity = static_cast<float>(postSpikes.size());
            }
            
            // Attempt to remove weak synapse based on activity and weight
            if (removeWeakSynapse(brain, syn, sourceActivity, targetActivity, rng)) {
                ++pImpl->totalSynapsesPruned;
                ++pImpl->synapsesPrunedThisStep;
                NLM_LOG_DEBUG("Pruning: Removed synapse " + std::to_string(syn->getId().index()) +
                             " between neurons " + std::to_string(sourceId.index()) + " and " +
                             std::to_string(targetId.index()));
            }
        }
    }
    
    // Calculate pruning efficiency
    float excessSynapses = 0.0f;
    float targetSynapses = static_cast<float>(brain->getTotalNeuronCount()) * 
                          static_cast<float>(brain->getTotalNeuronCount() - 1) * 
                          pImpl->minConnectionProbability;
    
    float currentSynapses = static_cast<float>(brain->getTotalSynapseCount());
    
    if (currentSynapses > targetSynapses) {
        excessSynapses = (currentSynapses - targetSynapses) / targetSynapses;
        pImpl->pruningEfficiency = std::min(1.0f, 1.0f - excessSynapses);
    } else {
        pImpl->pruningEfficiency = 1.0f;
    }
    
    NLM_LOG_INFO("Pruning complete: Removed " + std::to_string(pImpl->synapsesPrunedThisStep) +
                 " synapses, efficiency = " + std::to_string(pImpl->pruningEfficiency));
}

} // namespace nlm
