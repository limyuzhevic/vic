#include "Maturation.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../dynamics/NeuralDynamics.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    float maturationRate;
    float activityRequirement;
    float plasticityMaturation;
    std::unordered_map<NeuronId, float> neuronMaturationLevels;
    std::unordered_map<NeuronId, float> neuronDevelopmentTiming;
    float globalMaturationStage;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f)
        , maturationRate(0.01f)
        , activityRequirement(0.3f)
        , plasticityMaturation(0.8f)
        , globalMaturationStage(0.0f) {}
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
    
    // Real maturation implementation
    // Integrates with neural dynamics and other development systems
    
    // Calculate maturation based on multiple factors
    float timeFactor = static_cast<float>(currentStep) * 0.001f; // Convert to realistic time units
    float activityFactor = calculateActivityMaturation(brain);
    float plasticityFactor = calculatePlasticityMaturation(brain);
    float systemIntegrationFactor = calculateSystemIntegration(brain);
    
    // Weighted combination of maturation factors
    float totalFactor = timeFactor * 0.3f + 
                       activityFactor * 0.4f + 
                       plasticityFactor * 0.2f + 
                       systemIntegrationFactor * 0.1f;
    
    // Update maturation progress
    pImpl->progress += totalFactor * pImpl->maturationRate;
    pImpl->progress = std::clamp(pImpl->progress, 0.0f, 1.0f);
    
    // Update global maturation stage
    pImpl->globalMaturationStage = calculateMaturationStage(pImpl->progress);
    
    // Apply maturation effects to brain components
    applyMaturationEffects(brain);
}

MembranePotential Maturation::getMatureThreshold() const {
    // Calculate mature threshold based on maturation progress
    float progressionFactor = pImpl->progress * 0.4f; // 40% of progression
    return matureThreshold + progressionFactor * (matureThreshold - (-60.0f));
}

MembranePotential Maturation::getMatureRestingPotential() const {
    // Calculate mature resting potential based on maturation progress
    float progressionFactor = pImpl->progress * 0.3f; // 30% of progression
    return matureRestingPotential + progressionFactor * (matureRestingPotential - (-75.0f));
}

float Maturation::getMatureTimeConstant() const {
    // Calculate mature time constant based on maturation progress
    float progressionFactor = pImpl->progress * 0.5f; // 50% of progression
    return matureTimeConstant * (1.0f + progressionFactor * 0.5f); // Can increase by up to 50%
}

float Maturation::calculateActivityMaturation(Brain* brain) {
    if (!brain) return 0.0f;
    
    auto neurons = brain->getAllNeurons();
    if (neurons.empty()) return 0.0f;
    
    float totalActivity = 0.0f;
    float totalRequirement = 0.0f;
    
    for (const Neuron* neuron : neurons) {
        float activity = neuron->getActivity();
        totalActivity += activity;
        
        // Get neuron-specific maturation requirement
        float requirement = pImpl->neuronMaturationLevels.count(neuron->getId()) ? 
                           pImpl->neuronMaturationLevels[neuron->getId()] : pImpl->activityRequirement;
        totalRequirement += requirement;
    }
    
    float avgActivity = totalActivity / neurons.size();
    float avgRequirement = totalRequirement / neurons.size();
    
    // Activity maturation factor
    float activityMaturation = std::min(avgActivity / avgRequirement, 1.0f);
    
    return activityMaturation;
}

float Maturation::calculatePlasticityMaturation(Brain* brain) {
    if (!brain) return 0.0f;
    
    float baseMaturation = 0.5f; // 50% base maturation
    
    // Check dopamine system for plasticity modulation
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        float dopamineLevel = dopamine->getLevel();
        // Dopamine enhances plasticity maturation
        baseMaturation += dopamineLevel * pImpl->plasticityMaturation * 0.3f;
    }
    
    // Check prediction system for adaptive maturation
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        float confidence = predictionSystem->getConfidence();
        // High confidence prediction supports maturation
        baseMaturation += confidence * 0.2f;
    }
    
    return std::clamp(baseMaturation, 0.0f, 1.0f);
}

float Maturation::calculateSystemIntegration(Brain* brain) {
    if (!brain) return 0.0f;
    
    float integrationScore = 0.0f;
    
    // Check different brain regions
    auto regions = brain->getAllRegions();
    float regionCount = static_cast<float>(regions.size());
    
    if (regionCount > 0) {
        float regionIntegration = std::min(regionCount / 5.0f, 1.0f); // Target 5 regions
        integrationScore += regionIntegration * 0.3f;
    }
    
    // Check neuromodulation systems
    float neuromodulationScore = 0.0f;
    if (brain->getDopamine()) neuromodulationScore += 0.3f;
    if (brain->getNovelty()) neuromodulationScore += 0.3f;
    if (brain->getCuriosity()) neuromodulationScore += 0.3f;
    if (brain->getPredictionErrorSignal()) neuromodulationScore += 0.3f;
    
    neuromodulationScore = std::min(neuromodulationScore, 1.0f);
    integrationScore += neuromodulationScore * 0.4f;
    
    // Check memory systems
    float memoryScore = 0.0f;
    if (brain->getWorkingMemory()) memoryScore += 0.25f;
    if (brain->getEpisodicMemory()) memoryScore += 0.25f;
    if (brain->getSemanticMemory()) memoryScore += 0.25f;
    if (brain->getAssociativeMemory()) memoryScore += 0.25f;
    
    memoryScore = std::min(memoryScore, 1.0f);
    integrationScore += memoryScore * 0.3f;
    
    return std::min(integrationScore, 1.0f);
}

float Maturation::calculateMaturationStage(float progress) {
    // Convert progress to developmental stage
    if (progress < 0.2f) {
        return 0.0f; // Embryonic stage
    } else if (progress < 0.4f) {
        return 0.25f; // Early development
    } else if (progress < 0.6f) {
        return 0.5f; // Mid development
    } else if (progress < 0.8f) {
        return 0.75f; // Late development
    } else {
        return 1.0f; // Adult stage
    }
}

void Maturation::applyMaturationEffects(Brain* brain) {
    if (!brain) return;
    
    auto neurons = brain->getAllNeurons();
    
    // Apply maturation effects to neurons based on progress
    for (Neuron* neuron : neurons) {
        // Calculate neuron-specific maturation level
        NeuronId neuronId = neuron->getId();
        float maturationLevel = getNeuronMaturationLevel(neuronId);
        
        // Apply properties based on maturation
        applyNeuronMaturationProperties(neuron, maturationLevel);
        
        // Update neuron development timing
        updateNeuronDevelopmentTiming(neuron, maturationLevel);
    }
    
    // Apply global maturation effects
    applyGlobalMaturationEffects(brain);
}

float Maturation::getNeuronMaturationLevel(NeuronId neuronId) {
    // Get or calculate neuron maturation level
    if (pImpl->neuronMaturationLevels.count(neuronId)) {
        return pImpl->neuronMaturationLevels[neuronId];
    }
    
    // Default maturation based on global progress with some variability
    float baseLevel = pImpl->progress * (0.8f + static_cast<float>(rand()) / RAND_MAX * 0.4f);
    pImpl->neuronMaturationLevels[neuronId] = std::clamp(baseLevel, 0.0f, 1.0f);
    
    return pImpl->neuronMaturationLevels[neuronId];
}

void Maturation::applyNeuronMaturationProperties(Neuron* neuron, float maturationLevel) {
    if (!neuron) return;
    
    // Adjust neuron properties based on maturation level
    if (maturationLevel > 0.5f) {
        // Apply adult-like properties
        
        // Adjust threshold
        float thresholdAdjustment = (maturationLevel - 0.5f) * 15.0f; // -60 to -75mV range
        float currentThreshold = neuron->getThreshold() + thresholdAdjustment;
        neuron->setThreshold(currentThreshold);
        
        // Adjust resting potential
        float restingAdjustment = (maturationLevel - 0.5f) * 10.0f; // -70 to -85mV range
        float currentResting = neuron->getRestingPotential() + restingAdjustment;
        neuron->setRestingPotential(currentResting);
        
        // Adjust membrane time constant
        float timeConstantAdjustment = (maturationLevel - 0.5f) * 10.0f;
        float currentTimeConstant = neuron->getTimeConstant() + timeConstantAdjustment;
        neuron->setTimeConstant(currentTimeConstant);
        
        // Adjust synaptic properties
        float synapticStrength = 1.0f + (maturationLevel - 0.5f) * 0.5f;
        neuron->setSynapticStrength(synapticStrength);
        
        // Increase refractory period maturity
        float refractoryMaturation = (maturationLevel - 0.5f) * 2.0f;
        neuron->setRefractoryPeriod(neuron->getRefractoryPeriod() * (1.0f + refractoryMaturation));
    }
}

void Maturation::updateNeuronDevelopmentTiming(Neuron* neuron, float maturationLevel) {
    if (!neuron) return;
    
    NeuronId neuronId = neuron->getId();
    
    // Calculate development timing based on activity and maturation level
    float activity = neuron->getActivity();
    float developmentSpeed = 1.0f + activity * 2.0f; // Higher activity = faster development
    
    float& timing = pImpl->neuronDevelopmentTiming[neuronId];
    timing += developmentSpeed * 0.01f * (1.0f - maturationLevel); // Slower as approaches maturity
    
    // Mark neuron as mature if timing exceeds threshold
    if (timing > 100.0f && maturationLevel > 0.8f) {
        neuron->setMature(true);
    }
}

void Maturation::applyGlobalMaturationEffects(Brain* brain) {
    if (!brain) return;
    
    // Apply global maturation effects to brain systems
    
    // 1. Neuromodulation maturation
    auto* dopamine = brain->getDopamine();
    if (dopamine) {
        // Mature dopamine system has better dynamics
        float maturityEffect = pImpl->progress * 0.5f;
        // Adjust dopamine properties (in real implementation would modify constants)
    }
    
    // 2. Memory system maturation
    if (brain->getWorkingMemory()) {
        float memoryMaturation = pImpl->progress * 0.8f; // Working memory matures faster
        // In real implementation would adjust memory parameters
    }
    
    // 3. Prediction system maturation
    auto* predictionSystem = brain->getPredictionSystem();
    if (predictionSystem) {
        float confidence = predictionSystem->getConfidence();
        float confidenceBoost = (pImpl->progress - confidence) * 0.1f;
        // In real implementation would adjust confidence calculation
    }
    
    // 4. Motor system maturation
    if (brain->getMotorSystem()) {
        float motorMaturation = pImpl->progress * 0.7f;
        // In real implementation would adjust action selection strategies
    }
    
    // 5. Sensory system maturation
    if (brain->getVision()) {
        float sensoryMaturation = pImpl->progress * 0.6f;
        // In real implementation would adjust sensory processing
    }
}

} // namespace nlm
