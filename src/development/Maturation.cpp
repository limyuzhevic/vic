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
    // Real maturation: synaptic and cellular maturation over developmental time
    // Neotenic neurons show accelerated maturation with higher levels
    if (!brain) return;
    
    // Maturation progresses over simulation time
    float maturityRate = 0.001f;
    pImpl->progress += maturityRate;
    
    if (pImpl->progress > 1.0f) {
        pImpl->progress = 1.0f;
        
        // When mature, stabilize neural properties:
        // - Increase synaptic stability
        // - Reduce overall excitability
        // - Enhance baseline membrane stability
        
        // Update mature targets with decay towards steady state
        pImpl->matureThreshold += (pImpl->matureThreshold - (-50.0f)) * 0.05f;
        pImpl->matureRestingPotential += (pImpl->matureRestingPotential - (-70.0f)) * 0.03f;
        pImpl->matureTimeConstant += (pImpl->matureTimeConstant - 25.0f) * 0.02f;
    }
    
    // Brain-level maturation effects
    if (pImpl->progress > 0.5f) {  // Late maturation phase
        // Increase homeostatic synaptic scaling
        // Reduce runaway excitation in mature networks
        if (auto* sp = brain->getStructuralPlasticity()) {
            float scale = 1.0f - (pImpl->progress - 0.5f) * 0.3f;  // Less plasticity as mature
            sp->setSynaptogenesisRate(0.0001f * scale);
            sp->setPruningRate(0.00005f * (2.0f - scale));
        }
        
        // Enhance neuromodulatory baseline
        if (auto* dopamine = brain->getDopamine()) {
            dopamine->setLevel(std::max(0.1f, dopamine->getLevel() - 0.02f));
        }
    }
}
