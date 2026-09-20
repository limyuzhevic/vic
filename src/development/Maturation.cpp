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
    if (!brain) {
        return;
    }
    
    // Simple maturation: gradually approach mature properties
    float progress = getProgress();
    progress = std::min(progress + 0.01f, 1.0f); // Increase by 1% per update
    setProgress(progress);
    
    // Gradually adjust neuron properties toward mature values
    // This provides a simple approximation of maturation
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
