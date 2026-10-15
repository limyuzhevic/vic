#include "Pruning.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Pruning::Impl {
    float threshold;
    float pruningRate;
    float experienceModifier;
    float developmentalAge;
    float learningScore;
    
    Impl() 
        : threshold(0.2f)
        , pruningRate(0.00005f)
        , experienceModifier(1.0f)
        , developmentalAge(0.0)
        , learningScore(0.0f) {}
};

Pruning::Pruning() : pImpl(new Impl) {}

Pruning::~Pruning() = default;

float Pruning::getThreshold() const {
    return pImpl->threshold;
}

void Pruning::setThreshold(float threshold) {
    pImpl->threshold = std::clamp(threshold, 0.0f, 1.0f);
}

float Pruning::getPruningRate() const {
    return pImpl->pruningRate;
}

void Pruning::setPruningRate(float rate) {
    pImpl->pruningRate = std::max(0.0f, rate);
}

void Pruning::update(class Brain* brain, class RandomGenerator& rng) {
    if (!brain) return;
    
    // Calculate learning score from brain systems
    pImpl->learningScore = 0.0f;
    
    if (brain->getDopamine()) {
        float rewardSignal = brain->getDopamine()->getLevel();
        pImpl->learningScore += rewardSignal * 2.0f;
    }
    
    if (brain->getCuriosity()) {
        float curiosityLevel = brain->getCuriosity()->getLevel();
        pImpl->learningScore += curiosityLevel * 1.5f;
    }
    
    if (brain->getWorkingMemory()) {
        float memoryActivity = brain->getWorkingMemory()->getMemoryActivity();
        pImpl->learningScore += memoryActivity * 3.0f;
    }
    
    // Cap learning score
    pImpl->learningScore = std::min(1.0f, pImpl->learningScore);
    
    // Threshold adjustment based on learning score
    // High learning scores -> lower pruning threshold (keep more synapses)
    // Low learning scores -> higher pruning threshold (prune more synapses)
    float thresholdAdjustment = (1.0f - pImpl->learningScore) * 0.3f;
    
    // Experience-dependent pruning
    float adaptiveThreshold = pImpl->threshold + thresholdAdjustment;
    
    // Age-dependent pruning intensity
    float ageModifier = 1.0f;
    if (pImpl->developmentalAge > 600.0) {
        ageModifier = 1.5f;  // Higher pruning in later maturation stages
    }
    
    // Stochastic element for biological realism
    float stochasticFactor = 1.0f + (rng.getRandomFloat() - 0.5f) * 0.3f;
    
    // Calculate pruning effectiveness
    float pruningEfficiency = adaptiveThreshold * pImpl->pruningRate * ageModifier * stochasticFactor;
    
    // In a full implementation, this would actually prune synapses
    // For now, we'll just log the potential pruning
    NLM_LOG_INFO("Pruning: threshold=" + std::to_string(adaptiveThreshold) + 
                " learningScore=" + std::to_string(pImpl->learningScore) +
                " efficiency=" + std::to_string(pruningEfficiency));
    
    // Update developmental age
    pImpl->developmentalAge += 0.001f;
}

bool Pruning::pruneSynapse(class Synapse* synapse) {
    if (!synapse) return false;
    
    // Simple pruning logic: remove weak synapses
    float weight = synapse->getWeight();
    
    if (weight < pImpl->threshold) {
        // In a full implementation, this would actually remove the synapse
        NLM_LOG_INFO("Pruning: pruning weak synapse with weight " + std::to_string(weight));
        return true;  // Would indicate successful pruning
    }
    
    return false;  // No pruning needed
}

} // namespace nlm