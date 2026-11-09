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
    // TODO PHASE 2: Implement real maturation
    // PLACEHOLDER: Progress increases over simulation time
    
    if (currentStep > 0) {
        // Progress increases based on time - faster maturation in earlier stages
        float maturationRate = 0.001f;
        
        // Stage-based maturation: faster in early stages
        switch (brain->getDevelopmentalStage()) {
            case DevelopmentalStage::Initial:
                maturationRate *= 2.0f;  // Fast initial maturation
                break;
            case DevelopmentalStage::CriticalPeriod:
                maturationRate *= 1.5f;  // Continued rapid maturation
                break;
            case DevelopmentalStage::Maturation:
                maturationRate *= 1.0f;  // Steady maturation
                break;
            case DevelopmentalStage::Adult:
                maturationRate *= 0.3f;  // Slow maturation in adulthood
                break;
            case DevelopmentalStage::Aging:
                maturationRate *= 0.1f;  // Very slow in aging
                break;
        }
        
        pImpl->progress += maturationRate;
        
        // Affect brain properties as maturation progresses
        if (brain && pImpl->progress > 0.0f) {
            // Mature neuron properties based on progress
            float progressFactor = std::min(pImpl->progress, 1.0f);
            
            // Update neuron thresholds and excitability
            for (auto& region : brain->getRegions()) {
                for (auto& pop : region.getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        // Make neurons more stable as they mature
                        float stabilityFactor = 1.0f - progressFactor * 0.3f;
                        
                        // Adjust neuron properties based on maturation
                        auto& state = neuron->getState();
                        state.threshold = pImpl->matureThreshold * stabilityFactor;
                        
                        // Reduce excitability with maturation
                        if (progressFactor > 0.5f) {
                            neuron->setThreshold(state.threshold * 0.9f);
                        }
                    }
                }
            }
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
