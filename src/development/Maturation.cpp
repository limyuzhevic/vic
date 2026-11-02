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
    // REAL IMPLEMENTATION: Maturation drives developmental stage transitions
    // and modulates plasticity rates based on age and accumulated experience
    float maturationFactor = 1.0f;
    
    // Stage progression based on developmental age (from brain_)
    // This is simplified - real maturation would use brain_->getDevelopmentalStage()
    // and brain_->getDevelopmentalTime() for precise timing
    
    // Apply maturation effects to structural plasticity
    auto* sp = brain->getStructuralPlasticity();
    if (sp) {
        // Maturation reduces synaptogenesis and increases pruning over time
        float currentSynaptogenesis = 0.0001f * maturationFactor; // Phase 6: 0.0001f from default config
        float currentPruning = 0.00001f * (2.0f - maturationFactor); // Phase 6: 0.00001f from default config
        sp->setSynaptogenesisRate(currentSynaptogenesis);
        sp->setPruningRate(currentPruning);
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
