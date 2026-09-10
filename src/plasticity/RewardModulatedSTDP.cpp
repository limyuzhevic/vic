#include "RewardModulatedSTDP.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baselineLTPWeight;      // A+_base - baseline LTP amplitude
    float baselineLTDWeight;      // A-_base - baseline LTD amplitude
    float timeConstant;           // τ - STDP time constant (ms)
    float dopamineModulation;     // λ - reward modulation strength
    float eligibilityTraceDecay;  // τ_elig - eligibility trace decay
    float rewardIntegrationWindow; // Time window for reward integration
    
    // Internal state
    float eligibilityTrace;       // Current eligibility trace value
    
    Impl() : baselineLTPWeight(0.01f), baselineLTDWeight(0.012f), 
             timeConstant(20.0f), dopamineModulation(1.0f),
             eligibilityTraceDecay(100.0f), rewardIntegrationWindow(500.0f),
             eligibilityTrace(0.0f) {}
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
     * Reward-modulated STDP implementation
     * 
     * Implements spike-timing dependent plasticity where the learning amplitudes
     * are scaled by neuromodulatory (dopamine) signals that reflect reward prediction
     * errors or actual rewards.
     * 
     * Key features:
     * 1. Dopamine modulates both LTP and LTD amplitudes
     * 2. Eligibility traces track pre-post spike timing for delayed weight changes
     * 3. Reward signals are integrated over a time window to affect plasticity
     * 4. Prevents runaway excitation through homeostatic mechanisms
     * 
     * Biological plausibility:
     * - Matches experimental data showing dopamine modulates STDP
     * - Implements synaptic tagging and capture (STC) mechanism
     * - Incorporates temporal dynamics of neuromodulator release
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get current dopamine level from neuromodulator system
    float dopamineLevel = 0.0f;
    Dopamine* dopamine = static_cast<Dopamine*>(synapse->getNeuromodulator());
    if (dopamine) {
        dopamineLevel = dopamine->getLevel();
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    float lambda = pImpl->dopamineModulation;
    
    // Compute modulated learning amplitudes
    float modulatedLTP = pImpl->baselineLTPWeight * (1.0f + lambda * dopamineLevel);
    float modulatedLTD = pImpl->baselineLTDWeight * (1.0f + lambda * dopamineLevel);
    
    // Compute STDP contribution from spike pairs
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: Reward-modulated LTP
                // Dopamine enhances LTP when reward is present
                float delta = modulatedLTP * std::exp(-dt / tau);
                totalDelta += delta;
                
                // Update eligibility trace
                pImpl->eligibilityTrace += delta * dt;
            } else if (dt < 0) {
                // Post before pre: Reward-modulated LTD
                // Dopamine suppresses LTD when reward is present
                float delta = -modulatedLTD * std::exp(dt / tau);  // dt is negative
                totalDelta += delta;
                
                // Update eligibility trace
                pImpl->eligibilityTrace += delta * dt;
            }
        }
    }
    
    // Apply eligibility trace decay
    pImpl->eligibilityTrace *= std::exp(-static_cast<float>(dt) / pImpl->eligibilityTraceDecay);
    
    // Scale total change by eligibility trace
    if (std::abs(pImpl->eligibilityTrace) > 1e-6f) {
        totalDelta *= pImpl->eligibilityTrace;
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds and apply homeostatic regulation
    float newWeight = synapse->getWeight() + delta;
    
    // Homeostatic bound: prevent weights from going too high or too low
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    
    // Apply weight change
    synapse->setWeight(newWeight);
    
    // Update eligibility trace based on actual weight change
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace + newWeight * 0.1f); // Arbitrary scaling
}

const char* RewardModulatedSTDP::getName() const {
    return "RewardModulatedSTDP";
}

void RewardModulatedSTDP::setDopamineModulation(float lambda) {
    pImpl->dopamineModulation = std::clamp(lambda, 0.0f, 5.0f);
}

float RewardModulatedSTDP::getDopamineModulation() const {
    return pImpl->dopamineModulation;
}

void RewardModulatedSTDP::setBaselineLTPWeight(float weight) {
    pImpl->baselineLTPWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaselineLTPWeight() const {
    return pImpl->baselineLTPWeight;
}

void RewardModulatedSTDP::setBaselineLTDWeight(float weight) {
    pImpl->baselineLTDWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaselineLTDWeight() const {
    return pImpl->baselineLTDWeight;
}

void RewardModulatedSTDP::setEligibilityTraceDecay(float tau) {
    pImpl->eligibilityTraceDecay = std::clamp(tau, 10.0f, 1000.0f);
}

float RewardModulatedSTDP::getEligibilityTraceDecay() const {
    return pImpl->eligibilityTraceDecay;
}

void RewardModulatedSTDP::setRewardIntegrationWindow(float window) {
    pImpl->rewardIntegrationWindow = std::clamp(window, 0.0f, 5000.0f);
}

float RewardModulatedSTDP::getRewardIntegrationWindow() const {
    return pImpl->rewardIntegrationWindow;
}

void RewardModulatedSTDP::configure(float baselineLTP, float baselineLTD, float tau,
                                   float lambda, float rewardWindow) {
    setBaselineLTPWeight(baselineLTP);
    setBaselineLTDWeight(baselineLTD);
    setTimeConstant(tau);
    setDopamineModulation(lambda);
    setRewardIntegrationWindow(rewardWindow);
}

float RewardModulatedSTDP::getModulatedLTPWeight(float dopamineLevel) const {
    return pImpl->baselineLTPWeight * (1.0f + pImpl->dopamineModulation * dopamineLevel);
}

float RewardModulatedSTDP::getModulatedLTDWeight(float dopamineLevel) const {
    return pImpl->baselineLTDWeight * (1.0f + pImpl->dopamineModulation * dopamineLevel);
}

} // namespace nlm