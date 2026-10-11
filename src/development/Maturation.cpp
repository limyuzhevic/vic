#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../memory/Memory.hpp"
#include <cmath>

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
    // Implement real maturation: biological neural development and tuning
    if (!brain) return;
    
    // Update maturation progress based on developmental time
    float maturationRate = 0.01f; // 1% per step (adjustable)
    setProgress(std::min(1.0f, getProgress() + maturationRate * static_cast<float>(currentStep)));
    
    if (getProgress() >= 1.0f) {
        return; // Maturation complete
    }
    
    // Apply maturation effects to brain regions
    for (RegionId regionId : brain->getRegionIds()) {
        NeuralRegion* region = brain->getRegion(regionId);
        if (!region) continue;
        
        auto& populations = region->getPopulations();
        
        // Refine neural properties based on maturation stage
        for (auto& population : populations) {
            auto& neurons = population->getAllNeurons();
            
            for (NeuronId neuronId : neurons) {
                Neuron* neuron = population->getNeuron(neuronId);
                if (!neuron) continue;
                
                // Mature neuron properties based on progress
                float maturityFactor = getProgress();
                
                // Refine membrane properties (Phase 1 was simple, Phase 2 is more realistic)
                if (neuron->getState().firingState != FiringState::Resting) {
                    // Adjust threshold toward mature values
                    float targetThreshold = getMatureThreshold();
                    float currentThreshold = neuron->getThreshold();
                    neuron->setThreshold(std::lerp(currentThreshold, targetThreshold, maturityFactor * 0.1f));
                    
                    // Adjust resting potential
                    float targetResting = getMatureRestingPotential();
                    float currentResting = neuron->getRestingPotential();
                    neuron->setRestingPotential(std::lerp(currentResting, targetResting, maturityFactor * 0.05f));
                    
                    // Adjust threshold variance (maturation reduces variability)
                    float thresholdVariance = 4.0f; // Reduced from Phase 1
                    neuron->setThreshold(getMatureThreshold() + rng.uniformReal(-thresholdVariance, thresholdVariance));
                }
            }
        }
    }
    
    // Consolidate synapses based on activity during maturation
    if (brain->getDevelopmentSystem()) {
        float consolidationThreshold = 0.5f * (1.0f - getProgress()); // More consolidation early
        brain->getDevelopmentSystem()->getEpisodicMemory()->consolidate(consolidationThreshold);
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
