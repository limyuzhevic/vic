#include "Maturation.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/STDP.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    float maturationRate;
    float criticalPeriodEnd;
    float peakMaturationTime;
    float activityDependentMaturation;
    std::unordered_map<NeuronId, float> neuronMaturationHistory;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f)
        , maturationRate(0.001f)
        , criticalPeriodEnd(100.0f)
        , peakMaturationTime(300.0f)
        , activityDependentMaturation(0.0f) {}
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
    
    // Calculate activity-dependent maturation
    pImpl->activityDependentMaturation = calculateActivityDependentMaturation(brain);
    
    // Update maturation progress based on time and activity
    float timeBasedProgress = maturationRate * 0.1f; // Base rate
    float activityModulation = pImpl->activityDependentMaturation;
    
    // Developmental stage affects maturation rate
    DevelopmentSystem* devSys = brain->getDevelopmentSystem();
    if (devSys) {
        DevelopmentalStage stage = devSys->getStage();
        switch (stage) {
            case DevelopmentalStage::Initial:
                // Rapid maturation in initial stage
                timeBasedProgress *= 1.5f;
                break;
            case DevelopmentalStage::CriticalPeriod:
                // Moderate maturation in critical period
                timeBasedProgress *= 1.0f;
                break;
            case DevelopmentalStage::Maturation:
                // Peak maturation during maturation stage
                timeBasedProgress *= 1.2f;
                break;
            case DevelopmentalStage::Adult:
                // Slow maturation in adult stage
                timeBasedProgress *= 0.3f;
                break;
            case DevelopmentalStage::Aging:
                // Minimal maturation in aging
                timeBasedProgress *= 0.1f;
                break;
        }
    }
    
    // Apply maturation update
    float maturationIncrement = timeBasedProgress + activityModulation * 0.01f;
    pImpl->progress = std::min(1.0f, pImpl->progress + maturationIncrement);
    
    // Update neuron properties based on maturation progress
    updateNeuronMaturation(brain, currentStep);
    
    // Log progress periodically
    static int logCounter = 0;
    if (++logCounter >= 1000) {
        NLM_LOG_INFO("Maturation progress: " + std::to_string(pImpl->progress * 100.0f) + "%, "
                    "activity factor: " + std::to_string(pImpl->activityDependentMaturation));
        logCounter = 0;
    }
}

void Maturation::Impl::updateNeuronMaturation(Brain* brain, SimulationStep currentStep) {
    if (!brain) return;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                NeuronState& state = const_cast<NeuronState&>(neuron->getState());
                NeuronId neuronId = neuron->getId();
                
                // Calculate neuron-specific maturation based on activity and type
                float neuronActivity = neuron->getState().firingRate;
                NeuronType neuronType = neuron->getType();
                
                // Stage-dependent maturation factors
                float stageFactor = 1.0f;
                DevelopmentSystem* devSys = brain->getDevelopmentSystem();
                if (devSys) {
                    DevelopmentalStage stage = devSys->getStage();
                    switch (stage) {
                        case DevelopmentalStage::CriticalPeriod:
                            stageFactor = 1.2f + (progress * 0.5f); // Stronger maturation during critical period
                            break;
                        case DevelopmentalStage::Maturation:
                            stageFactor = 0.8f + (progress * 1.2f); // Peak maturation
                            break;
                        default:
                            stageFactor = 1.0f;
                            break;
                    }
                }
                
                // Activity-dependent maturation velocity
                float maturationVelocity = maturationRate * stageFactor * 
                                         (1.0f + neuronActivity * 2.0f) * // Higher activity accelerates maturation
                                         (0.5f + progress * 0.5f); // Maturation accelerates over time
                
                // Type-specific maturation parameters
                float targetThreshold = matureThreshold;
                float targetResting = matureRestingPotential;
                
                if (neuronType == NeuronType::Excitatory) {
                    targetThreshold *= 1.1f; // Excitatory neurons have slightly higher threshold
                    targetResting *= 0.95f; // Slightly depolarized
                } else if (neuronType == NeuronType::Inhibitory) {
                    targetThreshold *= 0.9f; // Inhibitory neurons have lower threshold
                    targetResting *= 1.05f; // Slightly hyperpolarized
                }
                
                // Apply maturation to neuron properties
                // Threshold maturation (neurons become more selective)
                float thresholdDelta = (targetThreshold - state.threshold) * maturationVelocity * 0.1f;
                state.threshold = std::max(-60.0f, std::min(-40.0f, state.threshold + thresholdDelta));
                
                // Resting potential maturation (neurons stabilize at adult resting potential)
                float restingDelta = (targetResting - state.restingPotential) * maturationVelocity * 0.05f;
                state.restingPotential = std::max(-80.0f, std::min(-60.0f, state.restingPotential + restingDelta));
                
                // Membrane potential tracking towards target
                float potentialDelta = (targetResting - state.membranePotential) * maturationVelocity * 0.01f;
                state.membranePotential = std::max(-85.0f, std::min(-65.0f, state.membranePotential + potentialDelta));
                
                // Refractory period maturation (develop more robust refractory periods)
                if (progress > 0.3f) { // After initial development phase
                    float targetRefractory = static_cast<float>(matureTimeConstant);
                    float refractoryDelta = (targetRefractory - state.refractoryPeriod) * maturationVelocity * 0.02f;
                    state.refractoryPeriod = std::max(1, std::min(20, static_cast<int>(state.refractoryPeriod + refractoryDelta)));
                }
                
                // Adaptation variable maturation (development of spike-frequency adaptation)
                if (progress > 0.5f) {
                    state.adaptationVariable = std::min(0.5f, state.adaptationVariable + maturationVelocity * 0.001f);
                }
                
                // Store neuron maturation history for potential Hebbian-like consolidation
                neuronMaturationHistory[neuronId] = progress;
                
                // Log significant maturation events
                if (progress > 0.5f && neuronActivity > 1.0f) {
                    NLM_LOG_INFO("Neuron " + std::to_string(neuronId.value) + 
                                " (type: " + std::to_string(static_cast<int>(neuronType)) + ") matured: " +
                                "threshold=" + std::to_string(state.threshold) +
                                ", resting=" + std::to_string(state.restingPotential) +
                                ", adaptation=" + std::to_string(state.adaptationVariable));
                }
            }
        }
    }
}

float Maturation::Impl::calculateActivityDependentMaturation(Brain* brain) {
    if (!brain) return 0.0f;
    
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                totalActivity += neuron->getState().firingRate;
                neuronCount++;
            }
        }
    }
    
    float avgActivity = totalActivity / std::max(static_cast<size_t>(1), neuronCount);
    
    // Activity-dependent maturation: higher network activity accelerates maturation
    // This simulates activity-dependent developmental processes in real brains
    float activityFactor = std::min(1.0f, avgActivity * 3.0f); // Scale factor 3x for reasonable range
    
    // Apply Hebbian-like consolidation: co-active neurons mature faster
    // This simulates spike-timing-dependent maturation
    DevelopmentSystem* devSys = brain->getDevelopmentSystem();
    if (devSys && devSys->isCriticalPeriod()) {
        // During critical period, activity has stronger impact on maturation
        activityFactor *= 1.5f;
    }
    
    return activityFactor;
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
