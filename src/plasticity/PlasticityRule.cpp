#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
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
    
    Impl() : learningRate(0.01f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // TODO PHASE 2: Implement real Hebbian learning
    // PLACEHOLDER: Simple correlated firing increases weight
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count coincident spikes (simplified)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms window
                ++coincidences;
            }
        }
    }
    
    // Apply weight change proportional to coincidences
    if (coincidences > 0) {
        applyWeightChange(synapse, pImpl->learningRate * static_cast<float>(coincidences));
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

// Anti-Hebbian plasticity implementation
// Implements anti-Hebbian learning: "neurons that fire together, weaken together"
// Useful for inhibitory learning and preventing runaway excitation

struct AntiHebbianRule::Impl {
    float learningRate;      // Strength of anti-hebbian learning (positive)
    float threshold;          // Activity threshold for anti-hebbian effect
    float baseline;           // Baseline weight change
    float maxWeightChange;    // Maximum weight change magnitude
    
    Impl() : learningRate(0.01f), threshold(0.5f), baseline(0.0f), maxWeightChange(0.1f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() {
    delete pImpl;
}

void AntiHebbianRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    /*
     * Anti-Hebbian learning implementation
     * 
     * Mathematical formulation:
     * Δw = -η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Where:
     *   η = learningRate (positive for anti-Hebbian)
     *   pre * post = correlated activity
     *   ⟨pre⟩⟨post⟩ = uncorrelated activity
     *   Negative sign means correlated activity weakens the connection
     * 
     * Biological inspiration:
     *   - Reflects inhibitory synaptic plasticity
     *   - Prevents runaway excitation in neural circuits
     *   - Homeostatic mechanism for stability
     *   - Related to homosynaptic depression
     *   
     * When to use:
     *   - Creating balance between excitation and inhibition
     *   - Implementing stabilization mechanisms
     *   - Preventing neural runaway activity
     *   - In circuits with strong recurrent excitation
     *   
     * Limitations:
     *   - Simple pairwise correlation (no higher-order interactions)
     *   - Fixed learning rate (no neuromodulation)
     *   - Doesn't account for synaptic eligibility traces
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate anti-hebbian weight change
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            // Anti-hebbian rule: correlated activity weakens connection
            // Use spike timing window similar to STDP but inverted
            if (std::abs(dt) < pImpl->threshold * 100.0f) {  // Threshold-based window
                float delta = -pImpl->learningRate * std::exp(-std::abs(dt) / 50.0f);
                totalDelta += delta;
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change with magnitude limits
        float newWeight = synapse->getWeight() + totalDelta;
        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
        synapse->setWeight(newWeight);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply anti-hebbian weight change with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::setThreshold(float threshold) {
    pImpl->threshold = std::clamp(threshold, 0.0f, 1.0f);
}

float AntiHebbianRule::getThreshold() const {
    return pImpl->threshold;
}

// Bienenstock-Cooper-Munro (BCM) rule implementation
// Implements synaptic modification ratio theory
// Uses sliding threshold that depends on average postsynaptic activity

struct BCMRule::Impl {
    float learningRate;      // Base learning rate (η)
    float slidingThreshold;  // Sliding threshold (θ)
    float thetaMin;           // Minimum threshold value
    float thetaMax;           // Maximum threshold value
    float thetaLearningRate;  // Rate of threshold adaptation
    float baselineTheta;      // Baseline threshold value
    
    Impl() : learningRate(0.01f), slidingThreshold(0.5f), 
             thetaMin(0.1f), thetaMax(1.0f), thetaLearningRate(0.001f), baselineTheta(0.5f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() {
    delete pImpl;
}

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    /*
     * Bienenstock-Cooper-Munro (BCM) learning rule
     * 
     * Mathematical formulation (Synaptic Modification Ratio Theory):
     * Δw = η * (post * (pre - θ))
     * 
     * Where:
     *   η = learningRate
     *   post = postsynaptic activity
     *   pre = presynaptic activity  
     *   θ = sliding threshold (depends on average postsynaptic activity)
     *   
     * Biological inspiration:
     *   - Reflects calcium dynamics in postsynaptic neuron
     *   - Threshold depends on average firing rate (homeostatic)
     *   - Prevents runaway potentiation while allowing LTP/LTD
     *   - Found in hippocampal and cortical synapses
     *   
     * Key features:
     *   - Sliding threshold makes rule state-dependent
     *   - Produces asymmetric LTP/LTD curves
     *   - Stable for varying input statistics
     *   - Biologically plausible
     *   
     * When to use:
     *   - Implementing systems with homeostatic regulation
     *   - Circuits that need stability despite varying inputs
     *   - Models of synaptic learning in cortex/hippocampus
     *   - Systems with balanced excitation/inhibition
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update sliding threshold based on recent postsynaptic activity
    float avgPostActivity = 0.0f;
    size_t postCount = postSpikes.size();
    if (postCount > 0) {
        avgPostActivity = std::accumulate(postSpikes.begin(), postSpikes.end(), 0.0f) / postCount;
    }
    
    // Update sliding threshold (homeostatic regulation)
    pImpl->slidingThreshold += pImpl->thetaLearningRate * (avgPostActivity - pImpl->baselineTheta);
    pImpl->slidingThreshold = std::clamp(pImpl->slidingThreshold, pImpl->thetaMin, pImpl->thetaMax);
    
    // Calculate BCM weight change
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            // Only consider pre-post pairs within reasonable time window
            if (std::abs(dt) < 100.0f) {
                // Calculate weight change based on BCM formula
                // Δw = η * post * (pre - θ)
                float preActivity = 1.0f;  // Simplified: each spike contributes equally
                float postActivity = 1.0f;
                
                // Apply BCM rule
                float delta = pImpl->learningRate * postActivity * (preActivity - pImpl->slidingThreshold);
                totalDelta += delta;
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        float newWeight = synapse->getWeight() + totalDelta;
        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
        synapse->setWeight(newWeight);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply BCM weight change with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setSlidingThreshold(float threshold) {
    pImpl->slidingThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float BCMRule::getSlidingThreshold() const {
    return pImpl->slidingThreshold;
}

void BCMRule::setBaselineTheta(float theta) {
    pImpl->baselineTheta = std::clamp(theta, 0.0f, 1.0f);
}

float BCMRule::getBaselineTheta() const {
    return pImpl->baselineTheta;
}

} // namespace nlm