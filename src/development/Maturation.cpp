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

// Fix the bug in update method - add missing developmentalFactor variable

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    // Real neural maturation based on developmental principles
    // Implements age-dependent and activity-dependent neural maturation
    
    if (!brain) {
        return;
    }
    
    // Get current developmental state
    float currentProgress = getProgress();
    
    // Calculate maturation rate based on multiple factors
    float baseMaturationRate = 0.01f; // Default maturation rate per time step
    
    // Time-dependent maturation
    float timeFactor = std::min(1.0f, currentStep / 1000.0f); // Progress over simulation time
    
    // Activity-dependent maturation
    float activityFactor = brain->getAverageNeuronActivity();
    activityFactor = std::min(1.0f, activityFactor * 2.0f);
    
    // Neuromodulatory influence on maturation
    auto* dopamine = brain->getDopamine();
    auto* acetylcholine = brain->getAcetylcholine();
    
    float daLevel = dopamine ? dopamine->getLevel() : 0.5f;
    float achLevel = acetylcholine ? acetylcholine->getLevel() : 0.5f;
    
    // Dopamine promotes maturation, acetylcholine modulates learning
    float neuromodulatoryFactor = 1.0f + daLevel * 0.5f + achLevel * 0.3f;
    
    // Developmental stage modulation (slower maturation as we approach maturity)
    float developmentalStage = std::min(1.0f, currentProgress * 2.0f); // Progress through development
    float developmentalFactor = std::max(0.5f, 1.0f - developmentalStage * 0.5f); // Slower maturation as we approach maturity
    
    // Calculate final maturation rate
    float maturationRate = baseMaturationRate * 
                          timeFactor * 
                          activityFactor * 
                          neuromodulatoryFactor * 
                          developmentalFactor;
    
    // Random fluctuations for biological realism
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.8f, 1.2f);
    maturationRate *= dis(gen);
    
    // Update progress
    setProgress(currentProgress + maturationRate);
    
    // Apply maturation effects to brain
    applyMaturationEffects(brain);
    
    // Update mature neuron parameters based on progress
    if (currentProgress > 0.3f && currentProgress < 0.7f) {
        // Intermediate maturation phase: adjust thresholds and dynamics
        adjustNeuronParameters(brain, currentProgress);
    } else if (currentProgress > 0.7f) {
        // Late maturation phase: stabilize mature parameters
        stabilizeMatureParameters(brain);
    }
}

// Add the missing helper methods to Maturation.cpp

void Maturation::applyMaturationEffects(Brain* brain) {
    // Apply maturation effects to brain components
    if (!brain) {
        return;
    }
    
    float progress = getProgress();
    
    // Adjust neural excitability based on maturation
    float excitabilityFactor = 1.0f + progress * 0.5f; // More excitable as maturation progresses
    brain->setExcitabilityMultiplier(excitabilityFactor);
    
    // Adjust synaptic strength bounds
    float minWeight = -0.5f + progress * 0.3f; // Less negative as matures
    float maxWeight = 0.8f + progress * 0.2f;   // More positive as matures
    brain->setSynapticWeightBounds(minWeight, maxWeight);
    
    // Adjust learning rates
    float learningRate = 0.01f + progress * 0.05f; // Increased learning during maturation
    brain->setPlasticityLearningRate(learningRate);
    
    // Adjust neuromodulation sensitivity
    float neuromodSensitivity = 1.0f + progress * 0.3f;
    brain->setNeuromodulationSensitivity(neuromodSensitivity);
}

void Maturation::adjustNeuronParameters(Brain* brain, float progress) {
    // Adjust neuron parameters during intermediate maturation
    if (!brain) {
        return;
    }
    
    // Adjust membrane properties
    float membraneTimeConstant = 20.0f - progress * 10.0f; // Faster dynamics
    brain->setMembraneTimeConstant(membraneTimeConstant);
    
    float membraneResistance = 10.0f + progress * 5.0f; // More stable
    brain->setMembraneResistance(membraneResistance);
    
    // Adjust firing thresholds
    float threshold = getMatureThreshold();
    brain->setNeuronThreshold(threshold);
    
    // Adjust resting potential towards mature values
    float restingPotential = getMatureRestingPotential();
    brain->setRestingPotential(restingPotential);
    
    // Adjust refractory period
    float refractoryPeriod = 5.0f - progress * 2.0f; // Shorter refractory period
    brain->setRefractoryPeriod(refractoryPeriod);
}

void Maturation::stabilizeMatureParameters(Brain* brain) {
    // Stabilize neuron parameters in late maturation
    if (!brain) {
        return;
    }
    
    // Set mature neuron parameters
    float matureThreshold = getMatureThreshold();
    float matureRestingPotential = getMatureRestingPotential();
    float matureTimeConstant = getMatureTimeConstant();
    
    brain->setNeuronThreshold(matureThreshold);
    brain->setRestingPotential(matureRestingPotential);
    brain->setMembraneTimeConstant(matureTimeConstant);
    
    // Set stable plasticity parameters
    brain->setPlasticityLearningRate(0.02f);
    brain->setNeuromodulationSensitivity(1.5f);
    
    // Stabilize synaptic strength bounds
    float matureMinWeight = -0.2f;
    float matureMaxWeight = 1.0f;
    brain->setSynapticWeightBounds(matureMinWeight, matureMaxWeight);
}

} // namespace nlm
