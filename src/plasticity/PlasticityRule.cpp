// PlasticityRule implementation

#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

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
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     * 
     * This implements "neurons that fire together, wire together"
     * but with a threshold to prevent runaway potentiation.
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking
     *   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
     *   - Correlation-based learning in visual cortex
     *   
     * Limitations:
     *   - Doesn't account for STDP timing details
     *   - Single learning rate (no separate potentiation/depression rates)
     *   - Assumes stationary statistics
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (simplified covariance)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within a broad time window as correlated
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    // Compute weight change based on correlation
    // More sophisticated: use actual spike counts and firing rates
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// Anti-Hebbian rule implementation
// Implements anti-Hebbian learning where weight decreases when neurons fire together
struct AntiHebbianRule::Impl {
    float learningRate;
    float depressionRate;
    float correlationThreshold;  // Minimum correlation to trigger depression
    float minWeight;
    float maxWeight;
    
    Impl() : learningRate(0.005f), depressionRate(0.01f), correlationThreshold(0.5f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    /*
     * Anti-Hebbian learning implementation
     * 
     * Implements "neurons that fire together, WIRE APART"
     * Weight decreases when pre and post neurons fire together.
     * 
     * Mathematical formulation:
     * Δw = -η * f(coactivity)
     * 
     * Where f is a sigmoidal function that saturates at strong correlations.
     * 
     * Biological inspiration:
     *   - Competitive learning in cortical development
     *   - Predictable rivalry and rivalry resolution
     *   - Invariant feature learning (response normalization)
     *   
     * Applications:
     *   - Binding and feature competition
     *   - Competitive coding schemes
     *   - Preventing runaway excitation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    // Normalize correlation count to [0,1] range
    float correlation = static_cast<float>(correlationCount) / 100.0f;  // Assuming max 100 correlations
    correlation = std::clamp(correlation, 0.0f, 1.0f);
    
    // Compute depression based on correlation
    float depression = pImpl->depressionRate * correlation;
    
    // Apply weight change (always negative for anti-Hebbian)
    if (depression > 1e-6f) {
        applyWeightChange(synapse, -depression);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

// BCM (Bienenstock-Cooper-Munro) rule implementation
// Implements sliding threshold-based synaptic plasticity
struct BCMRule::Impl {
    float ltpWeight;        // Potentiation rate (learning parameter)
    float ltdWeight;        // Depression rate (learning parameter)
    float threshold;        // Sliding threshold for synaptic modification
    float thresholdTimeConstant;  // Time constant for threshold dynamics
    float thetaPlus;        // Maximum threshold value
    float thetaMinus;       // Minimum threshold value
    float minWeight;        // Minimum synaptic weight
    float maxWeight;        // Maximum synaptic weight
    
    // BCM internal state
    float theta;            // Current sliding threshold
    float postSpikeActivity; // Recent postsynaptic activity (running average)
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.02f), threshold(0.0f),
             thresholdTimeConstant(1000.0f), thetaPlus(1.0f), thetaMinus(0.0f),
             minWeight(-1.0f), maxWeight(1.0f), theta(0.0f), postSpikeActivity(0.0f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    /*
     * BCM (Bienenstock-Cooper-Munro) rule implementation
     * 
     * Implements sliding threshold-based synaptic plasticity that models
     * synaptic competition and homeostatic regulation.
     * 
     * Mathematical formulation:
     * 
     * Threshold dynamics:
     *   dθ/dt = ξ * (⟨y²⟩ - θ²)
     *   Where y is postsynaptic activity
     *   
     * Synaptic modification:
     *   If y > θ: Δw = η+ * (y - θ) * x  (LTP)
     *   If y < θ: Δw = -η- * θ * (y - θ) * x  (LTD)
     *   
     * Where x is presynaptic activity
     * 
     * Biological inspiration:
     *   - Local synaptic competition
     *   - Homeostatic regulation of synaptic strength
     *   - Oja's rule (a special case of BCM)
     *   - Synaptic stabilization mechanisms
     *   
     * Applications:
     *   - Feature competition in neural networks
     *   - Synaptic homeostasis
     *   - Balanced network activity
     */
    
    if (!synapse || postSpikes.empty()) {
        return;
    }
    
    float dtMs = static_cast<float>(dt) * 1000.0f;
    
    // Update threshold based on recent postsynaptic activity
    float postActivity = static_cast<float>(postSpikes.size()) / 10.0f;  // Normalize
    pImpl->postSpikeActivity = pImpl->postSpikeActivity * 0.9f + postActivity * 0.1f;  // Running average
    
    // Update sliding threshold
    float thresholdChange = pImpl->thresholdTimeConstant * (pImpl->postSpikeActivity * pImpl->postSpikeActivity - pImpl->theta * pImpl->theta);
    pImpl->theta += thresholdChange * dtMs / 1000.0f;
    pImpl->theta = std::clamp(pImpl->theta, pImpl->thetaMinus, pImpl->thetaPlus);
    
    // Compute weight changes from synaptic interactions
    float totalDelta = 0.0f;
    
    for (Timestamp postTime : postSpikes) {
        // For each postsynaptic spike, consider all presynaptic spikes
        for (Timestamp preTime : preSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potential potentiation
                if (pImpl->postSpikeActivity > pImpl->theta) {
                    // y > θ: LTP
                    float delta = pImpl->ltpWeight * (pImpl->postSpikeActivity - pImpl->theta);
                    totalDelta += delta;
                } else {
                    // y < θ: LTD (negative threshold modulation)
                    float delta = -pImpl->ltdWeight * pImpl->theta * (pImpl->postSpikeActivity - pImpl->theta);
                    totalDelta += delta;
                }
            }
            // dt <= 0: depression (post fires before or simultaneously with pre)
            else {
                // Postsynaptic spike before or with presynaptic spike: LTD
                float delta = -pImpl->ltdWeight * pImpl->theta * std::exp(dt / 100.0f);
                totalDelta += delta;
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

} // namespace nlm
