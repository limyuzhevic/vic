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
    // Real maturation process with biological plausibility
    // Maturation represents the developmental process where neurons and connections
    // mature from initial states to adult functionality
    
    // Maturity progresses over time, influenced by neural activity and developmental signals
    float maturationRate = 1.0f / (pImpl->matureTimeConstant * 0.001f);  // Convert to per second
    pImpl->progress = std::min(pImpl->progress + maturationRate * 0.001f, 1.0f);  // dt = 1ms
    
    if (pImpl->progress >= 1.0f) {
        // Neuron has reached maturity - apply mature properties
        brain->matureAllNeurons(pImpl->matureThreshold, pImpl->matureRestingPotential);
    } else {
        // During maturation, properties gradually approach mature values
        float maturityFactor = pImpl->progress;
        
        // Mature neurons have:
        // 1. Lower threshold (more excitable)
        float matureThreshold = -55.0f;
        float currentThreshold = brain->getAverageNeuronThreshold();
        float targetThreshold = matureThreshold + (currentThreshold - matureThreshold) * (1.0f - maturityFactor);
        brain->setTargetThreshold(targetThreshold);
        
        // 2. Higher membrane resistance (better signal integration)
        float matureResistance = 15.0f;
        float currentResistance = brain->getAverageMembraneResistance();
        float targetResistance = matureResistance + (currentResistance - matureResistance) * (1.0f - maturityFactor);
        brain->setTargetMembraneResistance(targetResistance);
        
        // 3. Optimized synaptic conductance
        float matureConductance = 20.0f;
        float currentConductance = brain->getAverageSynapseConductance();
        float targetConductance = matureConductance + (currentConductance - matureConductance) * (1.0f - maturityFactor);
        brain->setTargetSynapseConductance(targetConductance);
        
        // 4. Increased synaptic density
        float targetDensity = 0.15f;
        float currentDensity = brain->getCurrentSynapseDensity();
        float densityGrowth = (targetDensity - currentDensity) * maturityFactor * 0.01f;
        brain->adjustSynapseDensity(densityGrowth);
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
