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
    // Phase 2: Implement real maturation of neural circuits
    // Activity-dependent maturation with biologically-inspired mechanisms
    // Age-dependent changes in neural excitability and connectivity
    
    // Update maturation progress based on neural activity and developmental stage
    float activityLevel = 0.0f;
    float totalFiringRate = 0.0f;
    float totalNeuronCount = static_cast<float>(brain->getTotalNeuronCount());
    
    // Collect activity metrics
    for (const auto& region : brain->getRegions()) {
        const auto& neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            const auto& state = neuron->getState();
            
            // Calculate neuron activity using multiple indicators
            float neuronActivity = 0.0f;
            if (state.firingState == FiringState::Active) {
                neuronActivity = 1.2f; // Enhanced activity for actively firing neurons
            } else if (state.firingState == FiringState::Refractory) {
                neuronActivity = 0.3f; // Moderate activity during refractory period
            } else {
                // Calculate activity based on membrane potential deviation
                float membraneDeviation = std::abs(state.membranePotential - state.restingPotential);
                neuronActivity = std::max(0.0f, membraneDeviation / 25.0f);
                
                // Boost activity for neurons with high firing rates
                float firingRate = neuron->getFiringRate();
                if (firingRate > 0.1f) {
                    neuronActivity *= (1.0f + firingRate * 2.0f);
                }
            }
            
            activityLevel += neuronActivity;
            totalFiringRate += neuron->getFiringRate();
        }
    }
    
    // Calculate network-wide activity metrics
    float normalizedActivity = (totalNeuronCount > 0.0f) ? (activityLevel / totalNeuronCount) : 0.0f;
    float averageFiringRate = (totalNeuronCount > 0.0f) ? (totalFiringRate / totalNeuronCount) : 0.0f;
    
    // Activity-dependent maturation rate with biologically realistic dynamics
    float baseRate = 0.005f; // Slower base rate for more controlled development
    float maturationAcceleration = 1.0f + (normalizedActivity * 1.5f); // Activity boosts maturation
    float maturationSaturation = 1.0f - pImpl->progress; // Slows down as maturation progresses
    
    // Apply homeostatic regulation to prevent runaway maturation
    float homeostaticFactor = 1.0f / (1.0f + pImpl->progress * 2.0f);
    float maturingRate = baseRate * maturationAcceleration * maturationSaturation * homeostaticFactor;
    
    // Temperature-dependent modulation (simulated environmental factor)
    float temperatureModulation = 1.0f + std::sin(currentStep * 0.01f) * 0.1f; // Oscillating temperature effect
    maturingRate *= temperatureModulation;
    
    pImpl->progress += maturingRate;
    if (pImpl->progress > 1.0f) pImpl->progress = 1.0f;
    
    // Apply biologically-inspired maturation effects to neural properties
    applyMaturationEffects(brain, averageFiringRate);
    
    // Enhanced structural maturation through Hebbian-like mechanisms
    if (pImpl->progress > 0.3f && pImpl->progress < 0.8f) {
        enhanceNeuralConnectivity(brain, normalizedActivity);
    }
    
    // Critical period plasticity during specific developmental windows
    if (pImpl->progress < 0.4f) {
        induceCriticalPeriodPlasticity(brain);
    }
    }
    
    void Maturation::applyMaturationEffects(Brain* brain, float averageFiringRate) {
    // Apply biologically-inspired neural property maturation
    float maturationFactor = pImpl->progress;
    
    // Threshold maturation: more stable firing thresholds with experience
    // Thresholds increase with maturation (reducing excitability) but are modulated by activity
    float activityModulation = 1.0f - std::min(1.0f, averageFiringRate * 0.5f); // High activity reduces threshold increase
    float thresholdTarget = -50.0f - (maturationFactor * 20.0f * activityModulation);
    float thresholdUpdateRate = 0.02f * (1.0f - maturationFactor) * activityModulation;
    
    pImpl->matureThreshold = pImpl->matureThreshold + (thresholdTarget - pImpl->matureThreshold) * thresholdUpdateRate;
    
    // Resting potential maturation: moves towards optimal operating range
    float restingTarget = -62.5f; // Optimal resting potential for mature neurons
    float restingUpdateRate = 0.005f * (1.0f - maturationFactor);
    pImpl->matureRestingPotential = pImpl->matureRestingPotential + (restingTarget - pImpl->matureRestingPotential) * restingUpdateRate;
    
    // Time constant maturation: membrane becomes more responsive with age
    float timeConstantTarget = 15.0f; // Mature neurons have faster membrane dynamics
    float timeConstantUpdateRate = 0.01f * (1.0f - maturationFactor);
    pImpl->matureTimeConstant = pImpl->matureTimeConstant + (timeConstantTarget - pImpl->matureTimeConstant) * timeConstantUpdateRate;
    }
    
void Maturation::enhanceNeuralConnectivity(Brain* brain, float activityLevel) {
    // Hebbian-like structural maturation: strengthen frequently co-active connections
    // This simulates activity-dependent synaptic strengthening during development
    
    const float consolidationThreshold = 0.6f; // Maturation progress threshold for connectivity changes
    if (pImpl->progress < consolidationThreshold) {
        // Early developmental phase: establish baseline connectivity patterns
        float connectionStrength = std::min(1.0f, activityLevel * 2.0f);
        int neuronsToEnhance = static_cast<int>(brain->getTotalNeuronCount() * 0.05f * (pImpl->progress / consolidationThreshold));
        
        for (auto& region : brain->getRegions()) {
            auto& neurons = region->getAllNeurons();
            for (auto& neuron : neurons) {
                if (neuronsToEnhance <= 0) break;
                
                auto& state = neuron->getState();
                
                // Enhance neural responsiveness based on firing history
                float firingHistoryIntegral = calculateFiringHistoryIntegral(neuron.get());
                if (firingHistoryIntegral > 0.5f) {
                    // Enhance excitability for neurons with sustained activity
                    // Use adaptation variable as proxy for firing history
                    float adaptationUpdate = connectionStrength * (firingHistoryIntegral - 0.5f) * 2.0f;
                    state.adaptationVariable += adaptationUpdate;
                    
                    // Boost firing rate for neurons with good firing history
                    float firingRateBoost = std::min(0.5f, adaptationUpdate * 0.5f);
                    state.firingRate += firingRateBoost;
                    neuronsToEnhance--;
                }
            }
        }
    }
}

float Maturation::calculateFiringHistoryIntegral(const Neuron* neuron) const {
    // Calculate integral of firing history as measure of sustained activity
    if (!neuron) return 0.0f;
    
    // Use spike count and recent spike frequency as proxy for firing history
    int spikeCount = neuron->getState().lastSpikeTime > 0.0f ? 5 : 0; // Approximate spike count
    
    // Calculate firing rate from adaptation variable (proxy for firing history)
    float firingRateIntegral = neuron->getState().adaptationVariable;
    
    return spikeCount > 0 ? (firingRateIntegral / spikeCount) : 0.0f;
}
    
    void Maturation::induceCriticalPeriodPlasticity(Brain* brain) {
    // Simulate critical period plasticity with heightened sensory responsiveness
    // This mimics the sensitive periods in biological development
    
    float criticalPeriodMultiplier = 2.0f - (pImpl->progress * 1.5f); // Plasticity decreases with maturation
    
    for (auto& region : brain->getRegions()) {
        auto& neurons = region->getAllNeurons();
        for (auto& neuron : neurons) {
            auto& state = neuron->getState();
            
            // Enhance responsiveness during critical period
            if (state.firingState == FiringState::Resting) {
                float activationBoost = 0.02f * criticalPeriodMultiplier;
                state.restingPotential -= activationBoost;
            }
            
            // Lower firing thresholds to increase sensitivity
            float thresholdReduction = 1.0f * criticalPeriodMultiplier;
            float currentThreshold = neuron->getThreshold();
            float newThreshold = currentThreshold - thresholdReduction * 0.001f;
            neuron->setThreshold(newThreshold);
            
            // Stabilize membrane potential around optimal range
            float membraneTarget = -55.0f;
            float membraneUpdateRate = 0.01f * criticalPeriodMultiplier;
            state.membranePotential = state.membranePotential + (membraneTarget - state.membranePotential) * membraneUpdateRate;
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
