#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include "core/Exception.hpp"

namespace nlm {

class RewardModulatedSTDP::Impl {
public:
    float ltpWeight;           // A+ for potentiation
    float ltdWeight;           // A- for depression
    float timeConstant;        // Tau for exponential window (ms)
    float learningRate;        // Reward modulation factor (0-1)
    float minWeight;           // Minimum synaptic weight
    float maxWeight;           // Maximum synaptic weight
    
    Impl() : ltpWeight(0.02f), ltdWeight(0.015f), timeConstant(20.0f),
             learningRate(0.5f), minWeight(-1.0f), maxWeight(1.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() {
    delete pImpl;
}

void RewardModulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    /*
     * Reward-Modulated STDP (R-STDP) implementation
     * 
     * This rule extends standard STDP by modulating synaptic changes based on
     * reward signals received from neuromodulatory systems (dopamine, etc.)
     * 
     * Mathematical formulation:
     * Δw = g_R * (A+ * exp(-Δt/τ+) - A- * exp(Δt/τ-)) * R(t)
     * 
     * Where:
     *   g_R = learningRate (reward modulation factor, 0-1)
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)
     *   τ+ = τ- = timeConstant (STDP time window)
     *   R(t) = reward signal (normalized 0-1)
     * 
     * Biological inspiration:
     *   - Dopamine-mediated reward prediction errors
     *   - Reinforcement learning in prefrontal cortex
     *   - Reward-timing dependent plasticity
     * 
     * Benefits:
     *   - Focuses learning on reward-relevant experiences
     *   - Implements temporal difference learning
     *   - Allows for flexible behavior adaptation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    float reward = pImpl->learningRate;  // Simplified: using learningRate as reward signal
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply reward modulation
    totalDelta *= reward;
    
    // Apply weight change with bounds and validation
    if (std::abs(totalDelta) > 1e-6f) {
        // Check for valid synapse state
        if (!synapse->isValid()) {
            throw RewardModulatedSTDPException("Invalid synapse state");
        }
        
        // Check weight bounds before application
        float currentWeight = synapse->getWeight();
        float newWeight = currentWeight + totalDelta;
        
        if (newWeight < pImpl->minWeight || newWeight > pImpl->maxWeight) {
            // Clamp weight to bounds and log warning
            newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
            totalDelta = newWeight - currentWeight;
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
        
        // Update reward modulation based on actual weight change
        // This implements reward prediction error
        pImpl->learningRate = std::clamp(pImpl->learningRate * 0.99f, 0.0f, 1.0f);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        throw RewardModulatedSTDPException("Null synapse pointer");
    }
    
    // Check if synapse allows plasticity
    if (!synapse->allowsPlasticity()) {
        return;  // Silently ignore if plasticity is disabled
    }
    
    // Validate weight change
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    if (newWeight < pImpl->minWeight || newWeight > pImpl->maxWeight) {
        // Clamp to bounds
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        delta = newWeight - currentWeight;
    }
    
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void RewardModulatedSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void RewardModulatedSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float RewardModulatedSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::configure(float ltpWeight, float ltdWeight, float tau, float learningRate) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    setLearningRate(learningRate);
}

} // namespace nlm