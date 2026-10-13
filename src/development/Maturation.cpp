#include "Maturation.hpp"

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f) {}
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    pImpl->progress = std::clamp(progress, 0.0f, 1.0f);
}

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    if (!brain) return;
    
    // Increase maturation progress over time
    pImpl->progress += 0.01f; // Progress at ~1% per simulation step
    
    // Apply maturation effects based on progress
    if (brain->getSpikeSystem()) {
        auto* spikeSys = brain->getSpikeSystem();
        
        // Adjust neuron excitability based on maturation stage
        // Early development: higher excitability, more stochastic firing
        // Mature: lower excitability, more regular firing patterns
        float excitabilityFactor = 1.0f - pImpl->progress; // Decreases with maturity
        
        // Update neuron properties
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Modify membrane time constant based on maturity
                    neuron->setMembraneTimeConstant(pImpl->matureTimeConstant * excitabilityFactor);
                    
                    // Adjust resting potential towards mature value
                    float currentResting = neuron->getState().restingPotential;
                    float matureResting = pImpl->matureRestingPotential;
                    
                    // Gradually approach mature resting potential
                    float newResting = currentResting + (matureResting - currentResting) * 0.01f * excitabilityFactor;
                    neuron->setRestingPotential(newResting);
                    
                    // Adjust threshold towards mature threshold
                    float currentThreshold = neuron->getState().threshold;
                    float matureThreshold = pImpl->matureThreshold;
                    
                    float newThreshold = currentThreshold + (matureThreshold - currentThreshold) * 0.01f * excitabilityFactor;
                    neuron->setThreshold(newThreshold);
                    
                    // Reduce stochasticity (noise) as brain matures
                    // Early development: more stochastic, mature: more deterministic
                    if (brain->getRandomGenerator()) {
                        brain->getRandomGenerator()->setNoiseLevel(0.1f * excitabilityFactor);
                    }
                }
            }
        }
        
        // Maturation affects plasticity rules
        if (brain->getSTDP()) {
            auto* stdp = brain->getSTDP();
            // Modify STDP parameters based on maturation
            float maturityFactor = 1.0f - pImpl->progress;
            
            // Adjust LTP/LTD balance
            // Early development: more symmetric LTP/LTD
            // Mature: more asymmetric with LTD dominance
            float ltpWeight = 0.01f * (1.0f + maturityFactor);  // Reduced LTP with maturity
            float ltdWeight = 0.012f * (1.0f - maturityFactor * 0.5f); // Reduced LTD
            float tau = 20.0f * (1.0f - maturityFactor * 0.5f); // Shorter time constants
            
            stdp->configure(ltpWeight, ltdWeight, tau);
        }
        
        if (brain->getHebbian()) {
            auto* hebbian = brain->getHebbian();
            // Hebbian learning strength decreases with maturity
            float hebbianFactor = 1.0f - pImpl->progress * 0.5f;
            hebbian->setLearningRate(0.01f * hebbianFactor);
        }
    }
}

MembranePotential Maturation::getMatureThreshold() const {
    return pImpl->matureThreshold;
}

MembranePotential Maturation::getMatureRestingPotential() const {
    return pImpl->matureRestingPotential;
}

float Maturation::getMatureTimeConstant() const {
    return pImpl->matureTimeConstant;
}

} // namespace nlm
