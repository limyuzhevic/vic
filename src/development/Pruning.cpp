#include "Pruning.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Pruning::Impl {
    float threshold;           // Threshold for pruning
    float pruningRate;         // Base pruning probability
    float criticalPeriodMultiplier; // Multiplier during critical period
    float activityBasedThreshold; // Threshold based on activity
    
    // Tracking for activity-dependent pruning
    std::unordered_map<SynapseId, float> synapseActivity;
    std::unordered_map<SynapseId, SimulationStep> synapseCreationStep;
    
    Impl() : threshold(0.01f), pruningRate(0.001f), 
             criticalPeriodMultiplier(0.5f), activityBasedThreshold(0.1f) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning::~() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = std::max(0.0f, threshold);
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = std::max(0.0f, rate);
}

void Pruning::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    const auto& developmentSystem = brain->getDevelopmentSystem();
    
    // Calculate pruning rate based on developmental stage
    float pruningRate = calculatePruningRate(developmentSystem);
    
    // Get all synapses in the brain
    auto allSynapses = brain->getAllSynapses();
    
    // Perform activity-based pruning
    for (const auto& synapse : allSynapses) {
        // Calculate synapse activity (decay with time since creation)
        float activity = calculateSynapseActivity(synapse->getId());
        
        // Determine if synapse should be pruned
        if (shouldPruneSynapse(synapse, activity, rng, developmentSystem)) {
            brain->removeSynapse(synapse->getId());
        }
    }
}

float Pruning::calculatePruningRate(const DevelopmentSystem* developmentSystem) {
    float rate = pImpl->pruningRate;
    
    if (developmentSystem) {
        if (developmentSystem->isCriticalPeriod()) {
            rate *= 0.1f;  // Less pruning during critical period
        } else if (developmentSystem->getStage() == DevelopmentalStage::Maturation) {
            rate *= 0.8f;
        } else if (developmentSystem->getStage() == DevelopmentalStage Erosion::getPrunedSynapseActivity(SynapseId synapseId) {
    auto it = pImpl->synapseActivity.find(synapseId);
    return (it != pImpl->synapseActivity.end()) ? it->second : 0.0f;
}

bool Pruning::shouldPruneSynapse(const Synapse* synapse,
                                float activity,
                                RandomGenerator& rng,
                                const DevelopmentSystem* developmentSystem) {
    if (!synapse) return false;
    
    // Base pruning probability
    float baseProbability = pImpl->pruningRate;
    
    // Activity-based pruning (less active synapses more likely to be pruned)
    float activityFactor = 1.0f - activity;
    
    // Developmental stage modulation
    float developmentFactor = 1.0f;
    if (developmentSystem) {
        if (developmentSystem->isCriticalPeriod()) {
            developmentFactor *= 0.5f;
        } else if (developmentSystem->getStage() == DevelopmentalStage::Maturation) {
            developmentFactor *= 0.7f;
        } else if (developmentSystem->getStage() == DevelopmentalStage::Adult) {
            developmentFactor *= 0.9f;
        }
    }
    
    // Combined probability
    float probability = baseProbability * activityFactor * developmentFactor;
    
    // Random check against probability
    return rng.uniformReal(0.0f, 1.0f) < probability;
}

bool Pruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) return false;
    
    // Remove synapse from brain (placeholder - actual removal logic would go here)
    return true;
}

} // namespace nlm