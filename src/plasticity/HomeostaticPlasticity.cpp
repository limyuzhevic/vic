#include "HomeostaticPlasticity.hpp"
#include "../../brain/Neuron.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>

namespace nlm {

struct HomeostaticPlasticity::Impl {
    // Homeostatic regulation parameters
    float targetFiringRate;     // Target firing rate in Hz (0.1-100 typical)
    float learningRate;          // Learning rate for homeostatic adjustment (0.001-0.1)
    float timeConstant;          // Time constant for homeostatic dynamics (ms)
    float previousError;         // Previous firing rate error (Hz)
    float integralError;         // Integral of firing rate error (Hz * ms)
    
    // Neuron activity tracking
    std::vector<std::pair<Timestamp, float>> neuronActivityHistory;
    std::vector<float> recentFiringRates;
    size_t activityHistorySize;
    
    // Configuration
    static constexpr size_t DEFAULT_HISTORY_SIZE = 100;  // Number of timesteps to track
    static constexpr float DEFAULT_TARGET_RATE = 5.0f;    // Hz
    static constexpr float DEFAULT_LEARNING_RATE = 0.01f; // 1% per timestep
    static constexpr float DEFAULT_TIME_CONSTANT = 50.0f; // ms
    
    Impl() : targetFiringRate(DEFAULT_TARGET_RATE)
           , learningRate(DEFAULT_LEARNING_RATE)
           , timeConstant(DEFAULT_TIME_CONSTANT)
           , previousError(0.0f)
           , integralError(0.0f)
           , activityHistorySize(DEFAULT_HISTORY_SIZE)
    {
        recentFiringRates.reserve(DEFAULT_HISTORY_SIZE);
    }
};

HomeostaticPlasticity::HomeostaticPlasticity() : pImpl(new Impl) {}

HomeostaticPlasticity::~HomeostaticPlasticity() = default;

void HomeostaticPlasticity::update(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
    /*
     * Homeostatic Plasticity Implementation
     * 
     * Core Principle: Maintain firing rate near target to prevent runaway excitation/silencing
     * 
     * Biological Inspiration:
     * - Synaptic scaling: global adjustment of synaptic weights based on activity
     * - Intrinsic plasticity: modification of neuronal properties to maintain firing rate
     * - Adaptive firing threshold: adjustment of firing thresholds based on network activity
     * 
     * Mathematical Model (Firing Rate Homeostasis):
     *   Error = CurrentFiringRate - TargetFiringRate
     *   WeightChange = LearningRate * TimeStep * Error * EligibilityTrace
     *   
     * Where:
     *   CurrentFiringRate = Average firing rate over recent history
     *   Error drives synaptic scaling (positive -> depress, negative -> potentiate)
     *   Eligibility trace tracks recent synaptic activity
     *   
     * Stability Mechanisms:
     *   - Integral control prevents steady-state error
     *   - Time constant provides smooth dynamics
     *   - Bounds on learning rate prevent oscillations
     *   
     * Functional Benefits:
     *   - Prevents runaway excitation (memory instability)
     *   - Prevents runaway inhibition (network silence)
     *   - Maintains dynamic range for learning
     *   - Provides robustness to input variations
     */
    
    if (!synapse) {
        return;
    }
    
    // For now, use eligibility trace from synapse for homeostatic modulation
    // This connects homeostatic plasticity with reward-modulated learning
    float eligibilityTrace = synapse->getEligibilityTrace();
    
    // Calculate homeostatic error based on current synaptic efficacy
    float currentEfficacy = synapse->getEfficacy();
    
    // Target efficacy based on firing rate (simplified mapping)
    // Higher efficacy should produce higher firing rates
    float targetEfficacy = 0.5f + (pImpl->targetFiringRate / 20.0f) * 0.5f; // Range [0.5, 1.0]
    
    float error = targetEfficacy - currentEfficacy;
    
    // Update integral of error (integral control for steady-state accuracy)
    float dtMs = static_cast<float>(dt) * 1000.0f; // Convert to milliseconds
    pImpl->integralError += error * dtMs;
    
    // Calculate homeostatic weight change (proportional-integral control)
    float proportionalTerm = error;
    float integralTerm = pImpl->integralError / pImpl->timeConstant;
    
    float homeostaticDelta = pImpl->learningRate * (proportionalTerm + integralTerm) * eligibilityTrace;
    
    // Apply homeostatic modulation to synaptic weight
    if (std::abs(homeostaticDelta) > 1e-8f) {
        float newWeight = synapse->getWeight() + homeostaticDelta;
        
        // Clamp to valid range
        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
        
        // Apply bounded change
        float maxChange = std::abs(newWeight - synapse->getWeight());
        if (maxChange > 0.1f) { // Limit to 10% change per timestep
            newWeight = synapse->getWeight() + 0.1f * (newWeight - synapse->getWeight());
        }
        
        synapse->setWeight(newWeight);
        
        // Update eligibility trace with homeostatic contribution
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace * 0.9f); // Decay eligibility
    }
}

void HomeostaticPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply synaptic weight change with homeostatic constraints
    float newWeight = synapse->getWeight() + delta;
    
    // Homeostatic bounds: prevent weights from drifting too far
    float homeostaticMin = -0.8f;
    float homeostaticMax = 0.8f;
    
    newWeight = std::clamp(newWeight, homeostaticMin, homeostaticMax);
    
    synapse->setWeight(newWeight);
}

const char* HomeostaticPlasticity::getName() const {
    return "HomeostaticPlasticity";
}

void HomeostaticPlasticity::setTargetFiringRate(float rate) {
    pImpl->targetFiringRate = std::clamp(rate, 0.1f, 100.0f);
}

float HomeostaticPlasticity::getTargetFiringRate() const {
    return pImpl->targetFiringRate;
}

void HomeostaticPlasticity::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0001f, 0.1f);
}

float HomeostaticPlasticity::getLearningRate() const {
    return pImpl->learningRate;
}

void HomeostaticPlasticity::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 500.0f);
}

float HomeostaticPlasticity::getTimeConstant() const {
    return pImpl->timeConstant;
}

void HomeostaticPlasticity::recordNeuronActivity(Neuron* neuron, Timestamp currentTime, float currentTimeMs) {
    if (!neuron) return;
    
    // Record neuron firing state
    bool isFiring = neuron->isFiring();
    
    // Calculate approximate firing rate (spikes per second)
    // For now, use a simple approximation based on firing state
    float firingRate = isFiring ? pImpl->targetFiringRate : 0.0f;
    
    // Store in activity history
    pImpl->neuronActivityHistory.emplace_back(currentTime, firingRate);
    
    // Maintain history size
    if (pImpl->neuronActivityHistory.size() > pImpl->activityHistorySize) {
        pImpl->neuronActivityHistory.erase(pImpl->neuronActivityHistory.begin());
    }
    
    // Update recent firing rates
    pImpl->recentFiringRates.push_back(firingRate);
    if (pImpl->recentFiringRates.size() > pImpl->activityHistorySize) {
        pImpl->recentFiringRates.erase(pImpl->recentFiringRates.begin());
    }
}

void HomeostaticPlasticity::resetNeuronHistory() {
    pImpl->neuronActivityHistory.clear();
    pImpl->recentFiringRates.clear();
    pImpl->previousError = 0.0f;
    pImpl->integralError = 0.0f;
}

} // namespace nlm
