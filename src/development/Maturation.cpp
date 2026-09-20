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
    // Phase 2: Real maturation implementation
    // Progress neural development towards adult properties
    if (!brain) return;
    
    // Get current progress
    float progress = getProgress();
    
    // Calculate maturation rate (slower with age, faster early on)
    float maturationRate = 0.001f * (1.0f + progress) * (1.0f / (1.0f + static_cast<float>(currentStep) / 1000.0f));
    
    // Update progress
    setProgress(progress + maturationRate);
    
    // Update brain properties based on maturation progress
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    // Update all neurons in the region
    auto neurons = region->getAllNeurons();
    for (auto* neuron : neurons) {
        auto& state = neuron->getState();
        
        // Gradually adjust parameters towards mature values
        float matureProgress = std::min(1.0f, progress * 2.0f);  // Mature by progress of 0.5
        
        // Adjust threshold (becomes less excitable)
        float matureThreshold = -55.0f;
        state.threshold = matureThreshold + (state.threshold - matureThreshold) * (1.0f - matureProgress);
        
        // Adjust resting potential
        float matureRestingPotential = -70.0f;
        state.restingPotential = matureRestingPotential + (state.restingPotential - matureRestingPotential) * (1.0f - matureProgress);
        
        // Adjust time constant
        float matureTimeConstant = 20.0f;
        state.leakConductance = matureTimeConstant + (state.leakConductance - matureTimeConstant) * (1.0f - matureProgress);
        
        // Adjust refractory period
        uint32_t matureRefractory = 5;
        state.refractoryPeriod = static_cast<uint32_t>(matureRefractory * (1.0f - matureProgress) + 1.0f);
    }
    
    NLM_LOG_INFO("Maturation progress: " + std::to_string(progress));
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
