#include "Maturation.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../plasticity/STDP.hpp"
#include <algorithm>

namespace nlm {

struct Maturation::Impl {
    float progress;
    float matureThreshold;      // Membrane potential threshold for maturity
    float matureRestingPotential; // Mature resting potential
    float matureTimeConstant;   // Mature membrane time constant
    float maturationRate;       // Rate of maturation
    float criticalPeriodStart;  // When critical period begins
    float criticalPeriodEnd;    // When critical period ends
    float plasticityBoost;      // Plasticity multiplier during critical period
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f)
        , maturationRate(0.001f)
        , criticalPeriodStart(100.0f)
        , criticalPeriodEnd(500.0f)
        , plasticityBoost(2.0f) {}
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
    
    // Update maturation progress
    pImpl->progress += pImpl->maturationRate;
    pImpl->progress = std::min(pImpl->progress, 1.0f);
    
    // Update brain based on maturation stage
    float maturationFactor = pImpl->progress;
    
    for (auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        
        for (auto* neuron : neurons) {
            // Update neuron based on maturation
            updateNeuron(neuron, maturationFactor);
        }
    }
}

void Maturation::updateNeuron(Neuron* neuron, float maturationFactor) {
    if (!neuron) return;
    
    // Apply maturation effects to neuron properties
    // As maturation progresses, neurons become more mature
    
    // Update membrane potential properties
    float currentVrest = neuron->getRestingPotential();
    float matureVrest = pImpl->matureRestingPotential;
    neuron->setRestingPotential(currentVrest + (matureVrest - currentVrest) * maturationFactor);
    
    // Update membrane time constant
    float currentTc = neuron->getTimeConstant();
    float matureTc = pImpl->matureTimeConstant;
    neuron->setTimeConstant(currentTc + (matureTc - currentTc) * maturationFactor);
    
    // Update firing threshold
    float currentThreshold = neuron->getFiringThreshold();
    float matureThreshold = pImpl->matureThreshold;
    neuron->setFiringThreshold(currentThreshold + (matureThreshold - currentThreshold) * maturationFactor);
    
    // Update plasticity mechanisms based on maturation
    updatePlasticity(neuron, maturationFactor);
}

void Maturation::updatePlasticity(Neuron* neuron, float maturationFactor) {
    // Modulate plasticity based on maturation
    // Critical period enhancement
    if (isCriticalPeriod()) {
        float criticalPeriodFactor = getCriticalPeriodProgress();
        // During critical period, plasticity is enhanced
        float enhancedFactor = pImpl->plasticityBoost * criticalPeriodFactor;
        
        // Update STDP parameters on connected synapses
        // This would require accessing synapses from the neuron
        // For now, we'll log this information
        NLM_LOG_INFO("Maturation: Critical period plasticity enhancement: " + 
                    std::to_string(enhancedFactor));
    }
    
    // General maturation effects on plasticity
    // As neurons mature, plasticity decreases
    float plasticityReduction = 1.0f - maturationFactor * 0.5f;  // Reduce by up to 50%
    
    NLM_LOG_INFO("Maturation: Plasticity factor for mature neuron: " + 
                std::to_string(plasticityReduction));
}

bool Maturation::isCriticalPeriod() const {
    return pImpl->progress >= pImpl->criticalPeriodStart && 
           pImpl->progress <= pImpl->criticalPeriodEnd;
}

float Maturation::getCriticalPeriodProgress() const {
    if (pImpl->progress <= pImpl->criticalPeriodStart) {
        return 0.0f;
    }
    if (pImpl->progress >= pImpl->criticalPeriodEnd) {
        return 1.0f;
    }
    return (pImpl->progress - pImpl->criticalPeriodStart) / 
           (pImpl->criticalPeriodEnd - pImpl->criticalPeriodStart);
}

MaturationPotential Maturation::getMaturePotential() const {
    // Returns the mature neuron potential state
    MaturationPotential mature;
    mature.restingPotential = pImpl->matureRestingPotential;
    mature.firingThreshold = pImpl->matureThreshold;
    mature.timeConstant = pImpl->matureTimeConstant;
    mature.plasticityFactor = 1.0f / pImpl->plasticityBoost;  // Reduced plasticity
    return mature;
}

} // namespace nlm
