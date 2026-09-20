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
    float timeConstant;   // Time window for correlation (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : learningRate(0.01f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Implement real Hebbian learning based on spike timing correlation
    // 
    // Mathematical formulation (Hebbian covariance rule):
    // Δw = η × (〈pre × post〉 - 〈pre〉〈post〉)
    // 
    // Simplified for spike-based systems:
    // Δw = η × correlation(pre, post)
    // 
    // Where correlation considers:
    // - Coincident firing within a time window (LTP)
    // - Anti-correlation (post before pre) for LTD
    // 
    // Biological inspiration:
    // - Reflects NMDA receptor-mediated calcium signaling
    // - Pre-before-post strengthens connections (LTP)
    // - Post-before-pre weakens connections (LTD)
    // 
    // Limitations:
    // - Single learning rate (unlike STDP's A+/A- asymmetry)
    // - Fixed time window (simpler than STDP's exponential decay)
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float timeWindow = pImpl->timeConstant;  // ms
    
    // Calculate Hebbian weight change based on spike timing
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            // Positive correlation: pre fires before post (within time window)
            if (dt > 0 && dt < timeWindow) {
                // Potentiation: neurons fire together in correct order
                float correlation = 1.0f - (dt / timeWindow);  // Linear decay
                totalDelta += pImpl->learningRate * correlation;
            } 
            // Negative correlation: post fires before pre (anti-correlation)
            else if (dt <= 0 && std::abs(dt) < timeWindow) {
                // Depression: anti-Hebbian effect
                float correlation = 1.0f - (std::abs(dt) / timeWindow);
                totalDelta -= pImpl->learningRate * correlation;
            }
            // Outside time window: no effect
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float HebbianRule::getTimeConstant() const {
    return pImpl->timeConstant;
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

void HebbianRule::configure(float learningRate, float timeConstant, float maxWeight) {
    setLearningRate(learningRate);
    setTimeConstant(timeConstant);
    setMaxWeight(maxWeight);
}

} // namespace nlm
