#include "Maturation.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Maturation::Impl {
    float progress;                   // Maturation progress (0-1)
    MembranePotential matureThreshold; // Target mature threshold
    MembranePotential matureRestingPotential; // Target mature resting potential
    float matureTimeConstant;         // Time constant for maturation
    float maturationRate;             // Rate of maturation progression
    float activityBasedMaturation;    // Activity-dependent maturation factor
    
    // Neural property targets
    float targetMembranePotential;    // Target resting potential
    float targetThreshold;            // Target firing threshold
    float targetRefractoryPeriod;     // Target refractory period
    
    // Development stage modulation
    float criticalPeriodModulator;    // Multiplier during critical period
    float activityMaturationModulator; // Activity influence on maturation
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f)
        , maturationRate(0.001f)
        , activityBasedMaturation(1.0f)
        , targetMembranePotential(-70.0f)
        , targetThreshold(-55.0f)
        , targetRefractoryPeriod(10.0f)
        , criticalPeriodModulator(2.0f)
        , activityMaturationModulator(1.5f) {}
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
    
    const auto& developmentSystem = brain->getDevelopmentSystem();
    
    // Calculate maturation rate based on developmental stage
    float maturationRate = calculateMaturationRate(developmentSystem, currentStep);
    
    // Update maturation progress
    pImpl->progress += maturationRate;
    pImpl->progress = std::clamp(pImpl->progress, 0.0f, 1.0f);
    
    // Apply matured properties to neurons based on progress
    applyMaturatedProperties(brain, currentStep);
    
    // Activity-based maturation enhancement
    if (developmentSystem && !developmentSystem->isCriticalPeriod()) {
        float activityFactor = calculateActivityBasedMaturationFactor(brain);
        pImpl->progress += maturationRate * activityFactor;
        pImpl->progress = std::clamp(pImpl->progress, 0.0f, 1.0f);
    }
}

float Maturation::calculateMaturationRate(const DevelopmentSystem* developmentSystem, 
                                         SimulationStep currentStep) {
    float rate = pImpl->maturationRate;
    
    if (developmentSystem) {
        // Development stage modulation
        if (developmentSystem->isCriticalPeriod()) {
            rate *= pImpl->criticalPeriodModulator;
        } else if (developmentSystem->getStage() == DevelopmentalStage::Maturation) {
            rate *= 1.5f;
        } else if (developmentSystem->getStage() == DevelopmentalStage::Adult) {
            rate *= 0.8f;
        } else if (developmentSystem->getStage() == DevelopmentalStage::Aging) {
            rate *= 0.2f;
        }
        
        // Age-based modulation
        float stageAge = developmentSystem->getStageAge();
        if (stageAge > 300.0f) {
            rate *= 0.5f;  // Slow maturation in later stages
        }
    }
    
    return rate;
}

void Maturation::applyMaturatedProperties(Brain* brain, SimulationStep currentStep) {
    if (!brain) return;
    
    auto neurons = brain->getAllNeurons();
    
    // Calculate maturation factor based on progress
    float maturationFactor = getMaturationFactor();
    
    // Gradually adjust neural properties toward mature targets
    for (const auto& neuron : neurons) {
        auto& state = neuron->getState();
        
        // Interpolate between current and mature properties
        float interpolationFactor = maturationFactor;
        
        // Resting potential maturation
        state.restingPotential = interpolate(state.restingPotential, 
                                             pImpl->matureRestingPotential, 
                                             interpolationFactor);
        
        // Threshold maturation
        state.threshold = interpolate(state.threshold, 
                                      pImpl->matureThreshold, 
                                      interpolationFactor);
        
        // Refractory period maturation
        state.refractoryPeriod = interpolate(state.refractoryPeriod, 
                                             pImpl->targetRefractoryPeriod, 
                                             interpolationFactor);
        
        // Leak conductance maturation
        state.leakConductance = interpolate(state.leakConductance, 
                                           20.0f,  // Mature value
                                           interpolationFactor);
    }
}

float Maturation::calculateActivityBasedMaturationFactor(Brain* brain) {
    if (!brain) return 0.0f;
    
    auto neurons = brain->getAllNeurons();
    float totalActivity = 0.0f;
    
    for (const auto& neuron : neurons) {
        const auto& state = neuron->getState();
        
        // Calculate activity metrics
        float firingRate = state.firingRate;
        float spikeCount = static_cast<float>(neuron->getSpikeHistory().size());
        
        totalActivity += firingRate + spikeCount * 0.1f;
    }
    
    // Normalize activity to maturation factor (0-1)
    float activityFactor = std::min(totalActivity / 100.0f, 1.0f);
    
    // Apply activity maturation modulator
    return activityFactor * pImpl->activityMaturationModulator;
}

float Maturation::getMaturationFactor() const {
    // Smooth sigmoidal maturation curve
    float x = pImpl->progress * 10.0f - 5.0f;
    return 1.0f / (1.0f + std::exp(-x));
}

float Maturation::interpolate(float current, float target, float factor) {
    return current + (target - current) * factor;
}

} // namespace nlm
