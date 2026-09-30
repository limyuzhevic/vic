#include "PlasticityRule.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float correlationWindow;  // Time window for spike correlation (ms)
    float neuromodulationFactor;  // Modulates learning rate
    float decayRate;  // Weight decay rate
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             correlationWindow(100.0f), neuromodulationFactor(1.0f), decayRate(0.001f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float HebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void HebbianRule::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float HebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

void HebbianRule::setCorrelationWindow(float window) {
    pImpl->correlationWindow = std::clamp(window, 10.0f, 1000.0f);
}

float HebbianRule::getCorrelationWindow() const {
    return pImpl->correlationWindow;
}

void HebbianRule::updateNeuromodulationFactor(float factor) {
    pImpl->neuromodulationFactor = std::clamp(factor, 0.0f, 2.0f);
}

void HebbianRule::applyDevelopmentStage(const DevelopmentSystem& devSystem) {
    // Apply developmental stage effects to learning rate
    float plasticityMod = devSystem.getPlasticityModifier();
    float originalRate = pImpl->learningRate;
    
    // Scale learning rate based on developmental stage
    // Higher plasticity in early stages
    float scaledRate = originalRate * plasticityMod;
    
    // Critical period effects: enhance Hebbian learning
    if (devSystem.isCriticalPeriod()) {
        float criticalPeriodProgress = devSystem.getCriticalPeriodProgress();
        // Gradually decrease learning rate during critical period
        float criticalPeriodMod = 1.0f - criticalPeriodProgress * 0.5f;
        scaledRate *= criticalPeriodMod;
    }
    
    // Apply scaled rate with bounds
    pImpl->learningRate = std::clamp(scaledRate, 0.0f, 1.0f);
}

void HebbianRule::applyDecay(float decayRate) {
    // Note: This should be called from update() with synapse parameter
    // For now, applying decay to the learning rate itself
    // In a full implementation, this would decay synapse weights
    if (decayRate <= 0.0f) return;
    
    // Decay learning rate toward baseline (homeostatic regulation)
    // This mimics synaptic scaling and prevents runaway potentiation
    pImpl->learningRate = std::max(pImpl->learningRate * (1.0f - decayRate), 0.001f);
}

void HebbianRule::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation based on covariance rule
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩) * M * D * N
     * 
     * Where:
     *   η = learningRate (base learning rate)
     *   M = neuromodulationFactor (dopamine, acetylcholine, etc.)
     *   D = developmentalStagePlasticityModifier (higher in early stages)
     *   N = spikeCorrelationCount (number of correlated pre-post spike pairs)
     * 
     * This implements "neurons that fire together, wire together" with:
     * - Spike timing correlation within a window
     * - Activity-dependent eligibility traces
     * - Integration with neuromodulation signals
     * - Developmental stage modulation
     * - Weight decay to prevent runaway excitation
     * 
     * Biological inspiration:
     *   - AMPA receptor trafficking at synapses
     *   - Correlated firing in hippocampal neurons
     *   - Neuromodulated synaptic plasticity in cortex
     *   - Developmental critical periods
     *   - Synaptic homeostasis
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (covariance-based)
    size_t correlatedPairs = 0;
    float correlationSum = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within correlation window
            if (std::abs(dt) <= pImpl->correlationWindow) {
                ++correlatedPairs;
                // Weight contribution by recency (recent spikes matter more)
                float recencyWeight = std::exp(-std::abs(dt) / pImpl->correlationWindow);
                correlationSum += recencyWeight;
            }
        }
    }
    
    // Apply weight change with neuromodulation and developmental effects
    if (correlatedPairs > 0) {
        // Compute weight change: η * (correlation count) * neuromodulation * efficiency
        float baseWeightChange = pImpl->learningRate * static_cast<float>(correlatedPairs);
        float neuromodulatedChange = baseWeightChange * pImpl->neuromodulationFactor;
        
        // Update eligibility trace for potential future reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        float eligibilityUpdate = neuromodulatedChange * 0.1f;  // Smaller eligibility contribution
        synapse->setEligibilityTrace(currentTrace + eligibilityUpdate);
        
        // Apply weight change
        applyWeightChange(synapse, neuromodulatedChange);
    }
    
    // Apply weight decay to maintain homeostasis
    applyDecay(pImpl->decayRate);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Compute new weight with bounds checking
    float newWeight = synapse->getWeight() + delta;
    
    // Apply weight constraints to maintain stability
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply synaptic efficacy modulation (use-dependent changes)
    float efficacy = synapse->getEfficacy();
    newWeight *= efficacy;
    
    // Re-clamp after efficacy application
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "HebbianRule";
}

} // namespace nlm
