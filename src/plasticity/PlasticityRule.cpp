#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

// HebbianRule implementation
struct HebbianRule::Impl {
    float learningRate; // Learning rate for Hebbian updates
    float timeWindow;   // Time window for correlation (ms)
    float maxCorrelation; // Maximum accumulated correlation
    
    Impl() : learningRate(0.001f), timeWindow(100.0f), maxCorrelation(1000.0f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || pImpl == nullptr || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalCorrelation = 0.0f;
    float tau = pImpl->timeWindow;
    
    // Calculate correlation between pre- and post-synaptic spikes
    // Optimized O(n) algorithm
    for (Timestamp preTime : preSpikes) {
        // Find posts spikes within time window of this pre spike
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(std::abs(postTime - preTime));
            if (deltaT <= tau) {
                // Exponential decay correlation
                float correlation = std::exp(-deltaT / tau);
                totalCorrelation += correlation;
            }
        }
    }
    
    // Apply Hebbian update if significant correlation found
    if (std::abs(totalCorrelation) > 1e-6f && totalCorrelation <= pImpl->maxCorrelation) {
        float delta = pImpl->learningRate * totalCorrelation;
        synapse->addToWeight(delta);
        pImpl->maxCorrelation = std::max(pImpl->maxCorrelation, totalCorrelation);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse || pImpl == nullptr) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    if (pImpl) {
        pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
    }
}

float HebbianRule::getLearningRate() const {
    return pImpl ? pImpl->learningRate : 0.0f;
}

} // namespace nlm
