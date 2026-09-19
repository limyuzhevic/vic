#include "PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

// Real HebbianRule implementation based on covariance rule
struct HebbianRule::Impl {
    float learningRate;
    float baselineFactor;  // For baseline activity subtraction
    float weightChangeLimit;  // Prevent runaway potentiation
    float minimumCoincidenceThreshold;  // Minimum coactivity required for learning
    
    Impl() : learningRate(0.01f), baselineFactor(0.001f), 
             weightChangeLimit(0.1f), minimumCoincidenceThreshold(1.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    /*
     * Real Hebbian Learning Implementation
     * 
     * Implements the Bienenstock-Cooper-Munro (BCM) like Hebbian rule:
     * Δw = η * (⟨pre * post⟩ - θ * ⟨pre⟩⟨post⟩)
     * 
     * Where:
     *   η = learning rate (HebbianRule::learningRate)
     *   θ = threshold (HebbianRule::baselineFactor) 
     *   ⟨pre * post⟩ = coactivity (spike coincidence)
     *   ⟨pre⟩⟨post⟩ = baseline activity product
     * 
     * Biological inspiration:
     *   - STDP modulation of Hebbian learning
     *   - Local synaptic tagging and marking
     *   - Threshold-based synaptic modification
     *   - Calcium-dependent plasticity
     *   
     * Advantages over simple correlation:
     *   - Prevents runaway potentiation
     *   - Incorporates synaptic history
     *   - Activity-dependent thresholds
     *   - More biologically realistic
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate actual coactivity (spike coincidence within window)
    size_t coactivity = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Realistic STDP window: potentiation when pre before post
            if (dt > 0.0f && dt < 100.0f) {  // 100ms STDP window
                ++coactivity;
            }
        }
    }
    
    // Get current synaptic weight and activity history
    float currentWeight = synapse->getWeight();
    const auto& preHistory = synapse->getPreSpikeHistory();
    const auto& postHistory = synapse->getPostSpikeHistory();
    
    // Calculate baseline activity (average firing rate)
    float avgPreFiring = static_cast<float>(preHistory.size()) / 1000.0f;  // Simplified
    float avgPostFiring = static_cast<float>(postHistory.size()) / 1000.0f;  // Simplified
    
    // Compute covariance-based weight change
    // Δw = η * (coactivity - θ * avgPreFiring * avgPostFiring * baseline)
    float baselineProduct = pImpl->baselineFactor * avgPreFiring * avgPostFiring;
    float delta = pImpl->learningRate * (static_cast<float>(coactivity) - baselineProduct);
    
    // Apply homeostatic scaling based on current weight
    // Stronger synapses change less, weaker synapses change more
    float weightFactor = 1.0f;
    if (std::abs(currentWeight) > 0.1f) {
        weightFactor = 0.5f / std::abs(currentWeight);
    }
    
    delta *= weightFactor;
    
    // Apply limits to prevent runaway potentiation
    float maxChange = pImpl->weightChangeLimit * weightFactor;
    if (std::abs(delta) > maxChange) {
        delta = std::copysign(maxChange, delta);
    }
    
    // Minimum coincidence threshold
    if (coactivity < pImpl->minimumCoincidenceThreshold) {
        delta = 0.0f;
    }
    
    // Apply weight change
    applyWeightChange(synapse, delta);
    
    // Update eligibility trace for reward-modulated learning
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace + delta);
    
    // Clamp to bounds
    float newWeight = synapse->getWeight();
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    synapse->addToWeight(delta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// Anti-Hebbian rule implementation
struct AntiHebbianRule::Impl {
    float depressionRate;
    float baselineActivity;
    
    Impl() : depressionRate(0.005f), baselineActivity(0.01f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                              const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt) {
    /*
     * Anti-Hebbian Learning Implementation
     * 
     * Δw = -η * (⟨pre * post⟩ - θ * ⟨pre⟩⟨post⟩)
     * 
     * Biological inspiration:
     *   - Competitive learning
     *   - Lateral inhibition
     *   - Resource competition
     *   - Homeostatic scaling
     *   
     * Used for:
     *   - Maintaining balance in neural circuits
     *   - Preventing runaway excitation
     *   - Competitive attention mechanisms
     *   - Activity-dependent pruning
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spikes (like Hebbian but opposite sign)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (dt > 0.0f && dt < 100.0f) {
                ++correlationCount;
            }
        }
    }
    
    // Calculate baseline activity
    float avgPreFiring = static_cast<float>(preSpikes.size()) / 1000.0f;
    float avgPostFiring = static_cast<float>(postSpikes.size()) / 1000.0f;
    
    // Apply anti-Hebbian depression
    // Stronger depression for correlated activity
    float delta = -pImpl->depressionRate * static_cast<float>(correlationCount);
    
    // Scale by current weight (stronger synapses depress more)
    float currentWeight = synapse->getWeight();
    if (currentWeight > 0) {
        delta *= (1.0f + currentWeight);  // Stronger excitation depresses more
    }
    
    // Apply limits
    delta = std::clamp(delta, -0.05f, 0.0f);
    
    // Apply weight change
    applyWeightChange(synapse, delta);
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    synapse->addToWeight(delta);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

// Bienenstock-Cooper-Munro (BCM) rule implementation
struct BCMRule::Impl {
    float learningRate;
    float thetaPlus;     // Sliding threshold for potentiation
    float thetaMinus;    // Sliding threshold for depression
    float sigma;         // Calcium dynamics parameter
    float calcium;       // Internal calcium concentration
    
    Impl() : learningRate(0.02f), thetaPlus(0.5f), thetaMinus(0.2f),
             sigma(0.1f), calcium(0.1f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    /*
     * Bienenstock-Cooper-Munro (BCM) Learning Rule
     * 
     * Δw = η * post * (pre - θ(y))
     * 
     * Where:
     *   post = postsynaptic activity
     *   pre = presynaptic activity
     *   θ(y) = sliding threshold dependent on average postsynaptic activity
     *   y = average postsynaptic activity
     *   θ(y) = θ_minus + (y^2) / (y^2 + σ^2) * (θ_plus - θ_minus)
     * 
     * Biological inspiration:
     *   - Calcium-dependent synaptic plasticity
     *   - BCM theory of synaptic modification
     *   - Activity-dependent thresholds
     *   - Synaptic competition
     *   
     * Key features:
     *   - Bidirectional plasticity (LTP/LTD)
     *   - Activity-dependent threshold
     *   - Stable learning dynamics
     *   - Prevents runaway excitation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate average postsynaptic activity
    float avgPostActivity = static_cast<float>(postSpikes.size()) / 1000.0f;
    
    // Update calcium dynamics
    pImpl->calcium += (avgPostActivity - pImpl->calcium) * pImpl->sigma;
    
    // Calculate sliding threshold
    float theta = pImpl->thetaMinus + 
                  (pImpl->calcium * pImpl->calcium) / 
                  (pImpl->calcium * pImpl->calcium + 0.1f) * 
                  (pImpl->thetaPlus - pImpl->thetaMinus);
    
    // Calculate weight change
    // Δw = η * post * (pre - θ)
    float preActivity = static_cast<float>(preSpikes.size()) / 1000.0f;
    
    float delta = pImpl->learningRate * avgPostActivity * (preActivity - theta);
    
    // Apply limits
    delta = std::clamp(delta, -0.02f, 0.02f);
    
    // Apply weight change
    applyWeightChange(synapse, delta);
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    synapse->addToWeight(delta);
}

const char* BCMRule::getName() const {
    return "BCM";
}

} // namespace nlm
