#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    float potentiationRate;
    float depressionRate;
    float eligibilityTraceDecay;
    float neuromodulationSensitivity;
    float baselineActivity;
    float calciumThreshold;
    float calciumDynamics;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), potentiationRate(0.015f), depressionRate(0.01f),
             eligibilityTraceDecay(0.95f), neuromodulationSensitivity(1.0f),
             baselineActivity(0.05f), calciumThreshold(0.3f), calciumDynamics(0.5f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {
    NLM_LOG_INFO("Enhanced Hebbian learning initialized - covariance-based plasticity");
}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Enhanced Hebbian Learning Implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Enhanced version with:
     * - Spike timing dependence (STD-like)
     * - Calcium-dependent processes
     * - Neuromodulator integration
     * - Eligibility traces
     * 
     * Biological inspiration:
     *   - AMPA receptor trafficking dynamics
     *   - Calcium signaling cascades
     *   - NMDA receptor voltage dependence
     *   - Spike-timing dependent plasticity
     *   - Neuromodulator-specific plasticity rules
     */
     
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Enhanced spike analysis with timing precision
    float correlationScore = 0.0f;
    float preActivity = static_cast<float>(preSpikes.size()) / 10.0f;
    float postActivity = static_cast<float>(postSpikes.size()) / 10.0f;
    
    // Count correlated spike pairs with precise timing
    size_t correlationCount = 0;
    float totalTimeDiff = 0.0f;
    size_t validPairs = 0;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float timeDiff = static_cast<float>(postTime - preTime);
            
            // Multiple time windows for different plasticity regimes
            if (std::abs(timeDiff) < 50.0f) {  // Strong correlation (early STDP)
                correlationCount += 2;
                totalTimeDiff += std::abs(timeDiff);
                validPairs += 2;
            } else if (std::abs(timeDiff) < 200.0f) {  // Moderate correlation
                ++correlationCount;
                totalTimeDiff += std::abs(timeDiff);
                ++validPairs;
            }
        }
    }
    
    // Calculate covariance-based weight change
    float expectedCorrelation = preActivity * postActivity;
    float actualCorrelation = static_cast<float>(correlationCount) / 10.0f;
    
    float covariance = actualCorrelation - expectedCorrelation;
    
    // Apply calcium-dependent modulation
    float calciumLevel = std::min(1.0f, covariance * pImpl->calciumDynamics);
    float calciumThreshold = pImpl->calciumThreshold;
    
    // Apply spike timing dependence
    float timingModulation = 1.0f;
    if (validPairs > 0) {
        float avgTimeDiff = totalTimeDiff / validPairs;
        timingModulation = std::exp(-avgTimeDiff / 50.0f);  // Exponential decay
    }
    
    // Apply learning rule with calcium threshold
    float delta = 0.0f;
    if (calciumLevel > calciumThreshold) {
        // Potentiation phase (LTP)
        delta = pImpl->potentiationRate * covariance * timingModulation;
    } else {
        // Depression phase (LTD)
        delta = -pImpl->depressionRate * covariance * timingModulation;
    }
    
    // Apply neuromodulation integration
    delta *= pImpl->neuromodulationSensitivity;
    
    // Apply eligibility traces (simplified)
    float traceFactor = 1.0f;  // Placeholder for eligibility trace system
    delta *= traceFactor;
    
    // Apply with bounds and limits
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply enhanced bounds with homeostatic constraints
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply soft bounds to prevent runaway excitation/depression
    float targetWeight = 0.0f;  // Homeostatic set point
    float distanceFromTarget = std::abs(newWeight - targetWeight);
    
    if (distanceFromTarget > 1.0f) {
        // Soft bound: pull weight back to homeostatic target
        float pullBackFactor = std::min(0.2f, (distanceFromTarget - 1.0f) / 2.0f);
        newWeight = currentWeight * (1.0f - pullBackFactor) + targetWeight * pullBackFactor;
    }
    
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
    // Adjust related parameters proportionally
    pImpl->potentiationRate = pImpl->learningRate * 1.5f;
    pImpl->depressionRate = pImpl->learningRate * 1.0f;
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void Hebbian::setCalciumThreshold(float threshold) {
    pImpl->calciumThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float Hebbian::getCalciumThreshold() const {
    return pImpl->calciumThreshold;
}

void Hebbian::setNeuromodulationSensitivity(float sensitivity) {
    pImpl->neuromodulationSensitivity = std::clamp(sensitivity, 0.0f, 2.0f);
}

float Hebbian::getNeuromodulationSensitivity() const {
    return pImpl->neuromodulationSensitivity;
}

} // namespace nlm
