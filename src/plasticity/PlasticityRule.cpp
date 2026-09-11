#include "PlasticityRule.hpp"
#include "STDP.hpp"
#include "Hebbian.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

// Anti-Hebbian rule implementation
void AntiHebbianRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            if (std::abs(static_cast<float>(postTime - preTime)) < 10.0f) {
                ++correlationCount;
            }
        }
    }
    
    // Anti-Hebbian: weaken connection when neurons fire together
    // Weight change is negative (depression) proportional to correlation
    float delta = -0.005f * static_cast<float>(correlationCount);
    
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    // Constrain to reasonable range
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

// BCM (Bienenstock-Cooper-Munro) rule implementation
void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    if (!synapse || postSpikes.empty()) {
        return;
    }
    
    // Get current postsynaptic firing rate (average firing rate from recent spikes)
    float postFiringRate = static_cast<float>(postSpikes.size()) / 10.0f;  // Normalized over 10ms window
    
    // Get current threshold θ (sliding threshold)
    float theta = theta_;
    
    // Calculate BCM learning rule
    // Δw = x · (y - θ) where x is pre-synaptic activity, y is post-synaptic activity
    // Simplified version for spike-based implementation
    
    // For each pre-synaptic spike, calculate weight change
    float totalDelta = 0.0f;
    for (Timestamp preTime : preSpikes) {
        // Compare pre spike with post spikes
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {  // Pre before post
                // Potentiation phase: pre-synaptic activity contributes to potentiation
                // Weight change depends on post-synaptic activity vs threshold
                if (postFiringRate > theta) {
                    // Sliding threshold is exceeded - potentiation occurs
                    float weightChange = -0.01f * std::exp(-dt / 10.0f);
                    totalDelta += weightChange;
                }
            }
        }
    }
    
    // Update sliding threshold based on recent average post-synaptic activity
    theta_ = theta_ * 0.9f + postFiringRate * 0.1f;  // Exponential moving average
    theta_ = std::clamp(theta_, 0.0f, 1.0f);  // Constrain threshold
    
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

// RewardModulatedSTDP implementation
void RewardModulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Initialize parameters if not set (could happen if constructor sets defaults)
    if (ltpWeight_ == 0.0f && ltdWeight_ == 0.0f) {
        configure(0.01f, 0.012f, 20.0f, 0.5f, 0.1f);
    }
    
    // First compute STDP-based weight change like STDP
    float stdpDelta = 0.0f;
    float tau = timeConstant_;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potentiation
                float delta = ltpWeight_ * std::exp(-dt / tau);
                stdpDelta += delta;
            } else if (dt < 0) {
                // Post before pre: depression
                float delta = -ltdWeight_ * std::exp(dt / tau);
                stdpDelta += delta;
            }
        }
    }
    
    // Apply eligibility trace for reward modulation
    float eligibility = synapse->getEligibilityTrace();
    eligibility += stdpDelta * learningRate_;
    synapse->setEligibilityTrace(eligibility);
    
    // Store the STDP component for potential reward signal
    storedStdpDelta_ = stdpDelta;
    
    // Update sliding threshold for BCM-like behavior
    float postFiringRate = static_cast<float>(postSpikes.size()) / 10.0f;
    float theta = theta_;
    
    if (postFiringRate > theta) {
        // Threshold exceeded - add additional modulation
        float modDelta = rewardWeight_ * (postFiringRate - theta) * eligibility;
        applyWeightChange(synapse, modDelta);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setLTPWeight(float weight) {
    ltpWeight_ = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTPWeight() const {
    return ltpWeight_;
}

void RewardModulatedSTDP::setLTDWeight(float weight) {
    ltdWeight_ = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTDWeight() const {
    return ltdWeight_;
}

void RewardModulatedSTDP::setTimeConstant(float tau) {
    timeConstant_ = std::clamp(tau, 1.0f, 100.0f);
}

float RewardModulatedSTDP::getTimeConstant() const {
    return timeConstant_;
}

void RewardModulatedSTDP::configure(float ltp, float ltd, float tau, float reward, float learning) {
    setLTPWeight(ltp);
    setLTDWeight(ltd);
    setTimeConstant(tau);
    rewardWeight_ = std::clamp(reward, 0.0f, 1.0f);
    learningRate_ = std::clamp(learning, 0.0f, 1.0f);
}

// TripletSTDP implementation
void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    if (!synapse || preSpikes.size() < 2 || postSpikes.size() < 2) {
        return;
    }
    
    // Initialize parameters if not set
    if (weightPlus_ == 0.0f && weightMinus_ == 0.0f) {
        configure(0.01f, 0.012f, 20.0f, 20.0f, -1.0f, 1.0f);
    }
    
    // Implement triplet STDP with contributions from three spike pairs
    // Formula: w(t+1) = w(t) + A+ * (sum_{i} f(t, t_i+pre) + sum_{i} g(t, t_i+post) + h(t, t_i-pre))
    // where f = facilitation, g = depression, h = calcium dynamics
    
    float tau_plus = timeConstantPlus_;
    float tau_minus = timeConstantMinus_;
    float A_plus = weightPlus_;
    float A_minus = weightMinus_;
    
    float totalDelta = 0.0f;
    
    // Post-post triplet contribution (g(t, t_i+post))
    // Posts that follow each other contribute to depression
    for (size_t i = 1; i < postSpikes.size(); ++i) {
        float delta_t = static_cast<float>(postSpikes[i] - postSpikes[i-1]);
        if (delta_t > 0) {
            float depression = A_minus * std::exp(-delta_t / tau_minus);
            totalDelta -= depression;
        }
    }
    
    // Pre-post triplet contribution (sum_{i} f(t, t_i+pre))
    // Pres that are followed by posts contribute to potentiation
    for (size_t i = 1; i < preSpikes.size(); ++i) {
        float delta_t = static_cast<float>(preSpikes[i] - preSpikes[i-1]);
        if (delta_t > 0) {
            float potentiation = A_plus * std::exp(-delta_t / tau_plus);
            totalDelta += potentiation;
        }
    }
    
    // Post-pre triplet contribution (h(t, t_i-pre))
    // Posts that precede pres contribute to depression
    for (size_t i = 1; i < postSpikes.size(); ++i) {
        for (size_t j = 1; j < preSpikes.size(); ++j) {
            float delta_t = static_cast<float>(preSpikes[j] - postSpikes[i]);
            if (delta_t > 0) {
                float depression = A_minus * std::exp(-delta_t / tau_minus);
                totalDelta -= depression;
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, minWeight_, maxWeight_);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::configure(float wtPlus, float wtMinus, float tp, float tm, float minWt, float maxWt) {
    weightPlus_ = std::clamp(wtPlus, 0.0f, 1.0f);
    weightMinus_ = std::clamp(wtMinus, 0.0f, 1.0f);
    timeConstantPlus_ = std::clamp(tp, 1.0f, 100.0f);
    timeConstantMinus_ = std::clamp(tm, 1.0f, 100.0f);
    minWeight_ = std::clamp(minWt, -1.0f, 1.0f);
    maxWeight_ = std::clamp(maxWt, -1.0f, 1.0f);
}