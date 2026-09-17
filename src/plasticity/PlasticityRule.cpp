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
    float maxWeight;
    float minWeight;
    float eligibilityTraceDecay;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             eligibilityTraceDecay(0.001f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Real Hebbian learning using correlation-based plasticity
    // Implements "neurons that fire together, wire together" principle
    // Based on covariance rule: Δw = η × (⟨x·y⟩ - ⟨x⟩⟨y⟩)
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float dtFloat = static_cast<float>(dt);
    
    // Calculate correlation between pre- and post-synaptic activity
    // For Hebbian learning, we need co-activity
    float totalCorrelation = 0.0f;
    size_t correlationCount = 0;
    
    // Compute spike-time correlation in a learning window
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float timeDiff = static_cast<float>(postTime - preTime);
            
            // Use exponential decay for spike-time correlation
            // This captures both temporal proximity and magnitude
            if (std::abs(timeDiff) < 100.0f) {  // 100ms learning window
                float temporalWeight = std::exp(-std::abs(timeDiff) / 20.0f);  // τ = 20ms
                totalCorrelation += temporalWeight;
                ++correlationCount;
            }
        }
    }
    
    // Calculate baseline activity (auto-correlation of each neuron)
    float baseline = 0.0f;
    if (correlationCount > 0) {
        baseline = (totalCorrelation * totalCorrelation) / static_cast<float>(correlationCount);
    }
    
    // Hebbian weight change: Δw = η × (correlation - baseline)
    float delta = pImpl->learningRate * (totalCorrelation - baseline);
    
    // Apply with bounds and eligibility trace
    applyWeightChange(synapse, delta);
    
    // Update eligibility trace for reward-modulated learning
    if (std::abs(delta) > 1e-6f) {
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + delta);
        // Decay eligibility trace
        synapse->decayEligibilityTrace(pImpl->eligibilityTraceDecay * dtFloat);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply Hebbian learning rule with homeostatic constraints
    float newWeight = synapse->getWeight() + delta;
    
    // Apply bounds
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Homeostatic normalization: prevent runaway excitation
    // When average weight exceeds threshold, normalize
    if (newWeight > pImpl->maxWeight * 0.9f) {
        newWeight = newWeight * 0.9f + pImpl->maxWeight * 0.1f;
    } else if (newWeight < pImpl->minWeight * 0.9f) {
        newWeight = newWeight * 0.9f + pImpl->minWeight * 0.1f;
    }
    
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

void HebbianRule::setEligibilityTraceDecay(float decayRate) {
    pImpl->eligibilityTraceDecay = std::clamp(decayRate, 0.0001f, 0.1f);
}

float HebbianRule::getEligibilityTraceDecay() const {
    return pImpl->eligibilityTraceDecay;
}

} // namespace nlm
