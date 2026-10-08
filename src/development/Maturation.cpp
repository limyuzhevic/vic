#include "Maturation.hpp"
#include "Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    float maturityStartAge;
    float maturityEndAge;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f)
        , maturityStartAge(60.0)
        , maturityEndAge(600.0) {}
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
    
    // Calculate maturation progress based on developmental age
    double developmentalAge = brain->getConfig()->getOr<double>("simulation_time", 0.0);
    
    // Normalize progress between start and end age
    if (developmentalAge < pImpl->maturityStartAge) {
        pImpl->progress = 0.0f;
    } else if (developmentalAge > pImpl->maturityEndAge) {
        pImpl->progress = 1.0f;
    } else {
        pImpl->progress = static_cast<float>((developmentalAge - pImpl->maturityStartAge) /
                                            (pImpl->maturityEndAge - pImpl->maturityStartAge));
    }
    
    // Apply maturation effects to neurons if progress is significant
    if (pImpl->progress > 0.0f && brain) {
        applyMaturationEffects(brain);
    }
    
    NLM_LOG_INFO(std::to_string(pImpl->progress * 100.0f) + "% maturation progress");
}

void Maturation::applyMaturationEffects(Brain* brain) {
    if (!brain) return;
    
    // Gradually adjust neuron parameters toward mature values
    float maturationFactor = pImpl->progress;
    
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Gradually adjust threshold toward mature value
                float currentThreshold = neuron->getThreshold();
                float matureThreshold = pImpl->matureThreshold;
                float newThreshold = currentThreshold + 
                                   (matureThreshold - currentThreshold) * maturationFactor * 0.1f; // 10% per update
                neuron->setThreshold(newThreshold);
                
                // Gradually adjust resting potential toward mature value
                float currentResting = neuron->getRestingPotential();
                float matureResting = pImpl->matureRestingPotential;
                float newResting = currentResting + 
                                 (matureResting - currentResting) * maturationFactor * 0.1f;
                neuron->setRestingPotential(newResting);
                
                // Gradually adjust time constant toward mature value
                float currentTimeConstant = neuron->getRefractoryPeriod();
                float matureTimeConstant = static_cast<float>(pImpl->matureTimeConstant);
                float newTimeConstant = currentTimeConstant + 
                                       (matureTimeConstant - currentTimeConstant) * maturationFactor * 0.1f;
                neuron->setRefractoryPeriod(static_cast<uint32_t>(newTimeConstant));
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
