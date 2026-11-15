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
    // PHASE 2: Implement real maturation based on configuration
    // During development, neurons mature (matureThreshold < membrane_potential > resting_potential)
    
    if (!brain || currentStep < 100) {
        return;
    }
    
    // Get development settings from config
    float synaptogenesisRate = 0.001f;
    float pruningRate = 0.0f;
    
    // Simulate maturation over time - neurons become more mature/stable
    setProgress(std::min(1.0f, getProgress() + 0.01f * (currentStep / 1000.0f)));
    
    // Mature neurons have more stable membrane potentials
    // For now, just increase progress - real implementation would modify neuron properties
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
