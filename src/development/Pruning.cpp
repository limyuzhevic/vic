#include "Pruning.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../development/DevelopmentSystem.hpp"

namespace nlm;

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    
    // Experience-dependent pruning parameters
    float experiencePruningFactor;      // Learning experience scaling
    float environmentalPruningBoost;    // Environmental interaction boost
    float neuromodulationPruning;        // Neuromodulatory influence
    float criticalPeriodPruning;         // Critical period effects
    float attentionPruningModulator;     // Attention-based modulation
    
    // Pruning state
    float totalExperience;                // Cumulative learning experience
    float synapticStrengthSum;             // Sum of all synapse weights
    float recentActivityPattern;           // Recent activity pattern strength
    float memoryDrivenPruning;             // Memory-driven pruning
    
    // Hebbian history for anti-hebbian pruning
    float hebbianHistorySum;               // Sum of hebbian plasticity events
    float antiHebbianHistorySum;           // Sum of anti-hebbian events
    
    Impl() : threshold(0.01f), pruningRate(0.0001f),
             experiencePruningFactor(1.0f),
             environmentalPruningBoost(1.0f),
             neuromodulationPruning(0.0f),
             criticalPeriodPruning(1.0f),
             attentionPruningModulator(1.0f),
             totalExperience(0.0f), synapticStrengthSum(0.0f),
             recentActivityPattern(0.0f), memoryDrivenPruning(0.0f),
             hebbianHistorySum(0.0f), antiHebbianHistorySum(0.0f) {}
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
    
    // Get current synapse count and weight statistics
    size_t totalSynapses = brain->getTotalSynapseCount();
    float averageSynapseWeight = calculateAverageSynapseWeight(brain);
    
    // Calculate pruning rate based on multiple factors
    float effectivePruningRate = calculateEffectivePruningRate(brain, averageSynapseWeight);
    
    // Determine which synapses to prune
    pruneWeakSynapses(brain, rng, effectivePruningRate, averageSynapseWeight);
    
    // Update pruning state
    updatePruningState(brain);
    
    // Integrate neuromodulatory signals
    integrateNeuromodulation(brain);
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) return false;
    
    // Check if synapse meets pruning criteria
    if (shouldPruneSynapse(synapse)) {
        // Mark synapse for pruning
        // In a real implementation, this would remove the synapse from the brain's structures
        NLM_LOG_INFO("Synapse " + std::to_string(synapse->getId().index()) + 
                     " pruned during experience-dependent pruning");
        return true;
    }
    return false;
}

void Pruning::pruneWeakSynapses(Brain* brain, RandomGenerator& rng, float pruningRate, float avgWeight) {
    if (!brain) return;
    
    // Get all synapses
    const auto& regions = brain->getRegions();
    for (const auto& region : regions) {
        // This is a simplified implementation
        // In a real implementation, we would iterate through all synapses
        // and decide which ones to prune based on weight and activity
        
        // For now, we'll simulate the pruning process
        float totalSynapsesInRegion = region->getSynapseCount();
        if (totalSynapsesInRegion == 0) continue;
        
        // Calculate how many synapses to prune in this region
        int synapsesToPrune = static_cast<int>(totalSynapsesInRegion * pruningRate);
        
        for (int i = 0; i < synapsesToPrune; ++i) {
            // Randomly select a synapse to prune (simplified)
            // In a real implementation, we would target weak synapses
            // For demonstration, we'll skip the actual pruning since
            // the brain API doesn't expose synapse removal
            if (rng.uniformReal(0.0f, 1.0f) < 0.5f) {
                // Would prune here in a real implementation
                // pruneSynapse(selectedSynapse);
            }
        }
    }
}

void Pruning::updatePruningState(Brain* brain) {
    if (!brain) return;
    
    // Update total experience based on learning
    pImpl->totalExperience += brain->getTotalSpikeCount() * 0.001f;
    
    // Update recent activity pattern
    pImpl->recentActivityPattern = std::min(1.0f, pImpl->recentActivityPattern + 
                                            brain->getTotalSpikeCount() * 0.0001f);
    
    // Update memory-driven pruning
    if (auto episodicMemory = brain->getEpisodicMemory()) {
        float memoryRetention = episodicMemory->getRetentionStrength();
        pImpl->memoryDrivenPruning = memoryRetention;
    }
    
    // Update Hebbian history
    if (auto stdp = brain->getSTDP()) {
        float hebbianActivity = stdp->getHebbianActivity();
        float antiHebbianActivity = stdp->getAntiHebbianActivity();
        
        pImpl->hebbianHistorySum += hebbianActivity * 0.01f;
        pImpl->antiHebbianHistorySum += antiHebbianActivity * 0.01f;
    }
}

void Pruning::integrateNeuromodulation(Brain* brain) {
    if (!brain) return;
    
    // Integrate dopamine for reward-modulated pruning
    if (auto dopamine = brain->getDopamine()) {
        float dopamineLevel = dopamine->getLevel();
        pImpl->neuromodulationPruning = dopamineLevel;
        
        // Dopamine modulates pruning
        // High dopamine promotes strengthening rather than pruning
        if (dopamineLevel > 0.7f) {
            pImpl->experiencePruningFactor *= 0.8f;  // Less pruning with high dopamine
        } else {
            pImpl->experiencePruningFactor *= 1.2f;  // More pruning with low dopamine
        }
    }
    
    // Integrate novelty for experience-driven pruning
    if (auto novelty = brain->getNovelty()) {
        float noveltyLevel = novelty->getLevel();
        pImpl->criticalPeriodPruning *= (1.0f + noveltyLevel * 0.3f);
        
        // High novelty encourages exploration (less pruning)
        if (noveltyLevel > 0.5f) {
            pImpl->environmentalPruningBoost *= 1.5f;
        }
    }
}

bool Pruning::shouldPruneSynapse(Synapse* synapse) {
    if (!synapse) return false;
    
    // Check if synapse meets pruning criteria
    float weight = synapse->getWeight();
    
    // Primary criterion: weak synapses
    if (std::abs(weight) < pImpl->threshold) {
        return true;
    }
    
    // Secondary criteria based on experience
    float experienceMultiplier = 1.0f + pImpl->experiencePruningFactor * 0.5f;
    float environmentMultiplier = 1.0f + pImpl->environmentalPruningBoost * 0.3f;
    float neuromodulationMultiplier = 1.0f + pImpl->neuromodulationPruning * 0.4f;
    
    // Effective threshold adjusted by experience and environment
    float effectiveThreshold = pImpl->threshold * experienceMultiplier * environmentMultiplier * neuromodulationMultiplier;
    
    // Prune if weight is below effective threshold
    if (std::abs(weight) < effectiveThreshold) {
        return true;
    }
    
    // Special pruning for anti-hebbian synapses (unlearning)
    if (synapse->getPlasticityFlags().reward_modulated) {
        // Reward-modulated synapses may be pruned if they don't contribute to learning
        if (pImpl->antiHebbianHistorySum > pImpl->hebbianHistorySum * 0.5f) {
            return true;
        }
    }
    
    return false;
}

float Pruning::calculateAverageSynapseWeight(Brain* brain) {
    if (!brain) return 0.0f;
    
    // Simplified calculation of average synapse weight
    // In a real implementation, we would iterate through all synapses
    float totalWeight = 0.0f;
    size_t totalSynapses = 0;
    
    const auto& regions = brain->getRegions();
    for (const auto& region : regions) {
        // This is a placeholder - in reality we would access synapse weights
        // For now, return a reasonable default based on total synapses
        float regionWeight = 0.0f;  // Would calculate actual average weight
        totalWeight += regionWeight;
        totalSynapses += region->getSynapseCount();
    }
    
    if (totalSynapses == 0) return 0.0f;
    return totalWeight / totalSynapses;
}

float Pruning::calculateEffectivePruningRate(Brain* brain, float avgWeight) {
    // Base pruning rate
    float baseRate = pImpl->pruningRate;
    
    // Scale by experience
    float experienceMultiplier = 1.0f + pImpl->experiencePruningFactor * 0.3f;
    
    // Scale by environmental interaction
    float environmentMultiplier = 1.0f + pImpl->environmentalPruningBoost * 0.2f;
    
    // Scale by neuromodulation
    float neuromodulationMultiplier = 1.0f + pImpl->neuromodulationPruning * 0.3f;
    
    // Scale by critical period
    float criticalPeriodMultiplier = pImpl->criticalPeriodPruning;
    
    // Scale by attention modulation
    float attentionMultiplier = pImpl->attentionPruningModulator;
    
    // Adjust based on average synapse weight
    float weightFactor = 1.0f;
    if (std::abs(avgWeight) < 0.1f) {
        weightFactor *= 1.5f;  // More pruning when average weight is low
    } else if (std::abs(avgWeight) > 0.5f) {
        weightFactor *= 0.5f;  // Less pruning when average weight is high
    }
    
    float rate = baseRate * experienceMultiplier * environmentMultiplier * 
                 neuromodulationMultiplier * criticalPeriodMultiplier * attentionMultiplier * weightFactor;
    
    return rate;
}

} // namespace nlm
