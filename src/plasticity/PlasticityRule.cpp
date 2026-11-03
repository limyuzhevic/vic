#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float traceDecay;
    float traceIncrement;
    float eligibilityTrace;
    
    Impl() : learningRate(0.01f), traceDecay(0.99f), traceIncrement(0.1f), eligibilityTrace(0.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // PHASE 2: Real Hebbian learning implementation
    // Implements spike-timing dependent Hebbian learning with eligibility traces
    
    if (preSpikes.empty() || postSpikes.empty()) {
        // Decay eligibility trace
        pImpl->eligibilityTrace *= pImpl->traceDecay;
        return;
    }
    
    float dtMs = static_cast<float>(dt * 1000.0);  // Convert to milliseconds
    
    // Calculate spike timing correlations
    float correlation = calculateHebbianCorrelation(preSpikes, postSpikes, dtMs);
    
    if (correlation > 0.0f) {
        // Positive correlation: potentiate synapse
        float delta = pImpl->learningRate * correlation * pImpl->traceIncrement;
        
        // Update eligibility trace
        pImpl->eligibilityTrace += delta;
        
        // Apply weight change with bounds and sign preservation
        applyWeightChange(synapse, delta);
    }
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= pImpl->traceDecay;
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // PHASE 2: Real Hebbian weight change with biological constraints
    // Implements biologically realistic weight updates with proper bounds
    
    if (!synapse) return;
    
    // Get current weight and synapse type
    float currentWeight = synapse->getWeight();
    SynapseType type = synapse->getType();
    
    // Calculate weight-dependent learning rate
    float weightMagnitude = std::abs(currentWeight);
    float maxChange = 0.1f;  // Maximum weight change per step
    
    // Soft bounds: learning rate decreases as weight approaches saturation
    float learningModulator = 1.0f - weightMagnitude * 2.0f;  // 0.0 at |w|=0.5
    learningModulator = std::max(0.1f, learningModulator);  // Minimum 10%
    
    float scaledDelta = delta * learningModulator;
    
    // Apply sign preservation: excitatory synapses stay positive,
    // inhibitory synapses stay negative
    if (type == SynapseType::Excitatory) {
        // Excitatory synapses: maintain minimum positive weight
        if (currentWeight + scaledDelta < 0.01f) {
            scaledDelta = 0.01f - currentWeight;
        }
    } else if (type == SynapseType::Inhibitory) {
        // Inhibitory synapses: maintain minimum negative weight
        if (currentWeight + scaledDelta > -0.01f) {
            scaledDelta = -0.01f - currentWeight;
        }
    }
    
    // Apply weight change
    synapse->addToWeight(scaledDelta);
    
    // Update eligibility trace for reward-modulated learning
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace + scaledDelta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, std::min(rate, 0.1f));  // Clamp to reasonable range
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// Calculate Hebbian correlation based on spike timing
float HebbianRule::calculateHebbianCorrelation(
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes,
    float dtMs) const {
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    // Use most recent spikes
    Timestamp recentPre = preSpikes.back();
    Timestamp recentPost = postSpikes.back();
    
    // Calculate time difference (post - pre) in milliseconds
    float timeDiff = (recentPost - recentPre) * 1000.0f;
    
    // Hebbian correlation function:
    // Peak at zero time difference (synchronous firing)
    // Decays exponentially with time difference
    // Can be both positive (excitatory) or negative (inhibitory) depending on timing
    
    if (std::abs(timeDiff) < dtMs) {
        // Synchronous firing: strongest positive correlation
        return 1.0f;
    } else {
        // Time-dependent correlation: decays exponentially
        float decayFactor = std::exp(-std::abs(timeDiff) / 50.0f);  // 50ms time constant
        return -decayFactor;  // Negative correlation for asynchronous firing
    }
}

// Get eligibility trace
float HebbianRule::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

// Reset eligibility trace
void HebbianRule::resetEligibilityTrace() {
    pImpl->eligibilityTrace = 0.0f;
}

void AntiHebbianRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    // Implement anti-Hebbian rule: decrease weight when neurons fire together
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Simple implementation: reduce weight based on spike coincidences
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms window
                ++coincidences;
            }
        }
    }
    
    if (coincidences > 0) {
        float delta = -pImpl->learningRate * static_cast<float>(coincidences);
        applyWeightChange(synapse, delta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Simple weight clamping
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
    
    // Update eligibility trace
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace + delta);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, std::min(rate, 0.01f));
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void AntiHebbianRule::resetEligibilityTrace() {
    pImpl->eligibilityTrace = 0.0f;
}

void AntiHebbianRule::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    PlasticityRule::enablePlasticity(hebbian, stdp, rewardModulated);
    if (!hebbian) {
        // Only apply anti-Hebbian when Hebbian is disabled
        pImpl->learningRate = pImpl->defaultLearningRate;
    }
}

struct BCMRule::Impl {
    float learningRate;
    float slidingThreshold;
    float activityHistory[10];
    size_t historyIndex;
    
    Impl() : learningRate(0.01f), slidingThreshold(0.5f), historyIndex(0) {
        for (size_t i = 0; i < 10; ++i) {
            activityHistory[i] = 0.0f;
        }
    }
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    // Implement BCM (Bienenstock-Cooper-Munro) rule: homeostatic plasticity
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate post-synaptic activity
    float postActivity = calculatePostSynapticActivity(postSpikes);
    
    // Update activity history
    pImpl->activityHistory[pImpl->historyIndex] = postActivity;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
    
    // Calculate average activity
    float avgActivity = 0.0f;
    for (size_t i = 0; i < 10; ++i) {
        avgActivity += pImpl->activityHistory[i];
    }
    avgActivity /= 10.0f;
    
    // Sliding threshold update
    float targetThreshold = avgActivity * 0.5f;  // BCM typical: threshold = 0.5 * average activity
    float learningRate = 0.01f / (dt + 1.0f);
    pImpl->slidingThreshold += learningRate * (targetThreshold - pImpl->slidingThreshold);
    pImpl->slidingThreshold = std::clamp(pImpl->slidingThreshold, 0.0f, 1.0f);
    
    // BCM weight change rule: Δw = η * (postActivity - θ) * w * (w - θ)
    float currentWeight = synapse->getWeight();
    float activityError = postActivity - pImpl->slidingThreshold;
    float weightError = currentWeight - pImpl->slidingThreshold;
    
    float delta = pImpl->learningRate * activityError * currentWeight * weightError;
    applyWeightChange(synapse, delta);
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply with sign preservation and soft bounds
    float currentWeight = synapse->getWeight();
    SynapseType type = synapse->getType();
    
    // Scale by efficacy
    float efficacy = synapse->getEfficacy();
    delta *= efficacy;
    
    if (type == SynapseType::Excitatory) {
        if (currentWeight + delta < 0.01f) {
            delta = 0.01f - currentWeight;
        }
    } else if (type == SynapseType::Inhibitory) {
        if (currentWeight + delta > -0.01f) {
            delta = -0.01f - currentWeight;
        }
    }
    
    // Apply weight change
    synapse->addToWeight(delta);
    
    // Update eligibility trace
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace + delta);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, std::min(rate, 0.01f));
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::updateSlidingThreshold(float postActivity, TimestepDuration dt) {
    // Already implemented in update method
}

float BCMRule::getSlidingThreshold() const {
    return pImpl->slidingThreshold;
}

float BCMRule::calculatePostSynapticActivity(const std::vector<Timestamp>& postSpikes) const {
    if (postSpikes.empty()) {
        return 0.0f;
    }
    
    Timestamp recentSpike = postSpikes.back();
    float currentTime = static_cast<float>(recentSpike);
    float timeSinceLastSpike = currentTime - recentSpike;
    
    if (timeSinceLastSpike > 1000.0f) {
        return 0.0f;
    }
    
    return std::exp(-timeSinceLastSpike / 100.0f);  // 100ms time constant
}

void BCMRule::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    PlasticityRule::enablePlasticity(hebbian, stdp, rewardModulated);
}

} // namespace nlm

