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
    float tau_positive;  // Time constant for LTP (ms)
    float tau_negative;  // Time constant for LTD (ms)
    float dt;           // Time step (ms)
    
    Impl() : learningRate(0.01f), tau_positive(20.0f), tau_negative(20.0f), dt(0.001f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (!synapse || synapse->getWeight() == 0.0f) {
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Real Hebbian learning: weight increase proportional to correlated activity
    // Implement spike-timing dependent correlation
    
    // Calculate temporal correlation using kernel
    float weightChange = 0.0f;
    
    // Convert timestamps to seconds for processing
    std::vector<double> preTimes(preSpikes.size());
    std::vector<double> postTimes(postSpikes.size());
    
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        preTimes[i] = static_cast<double>(preSpikes[i]) * pImpl->dt * 0.001;  // Convert to seconds
    }
    
    for (size_t i = 0; i < postSpikes.size(); ++i) {
        postTimes[i] = static_cast<double>(postSpikes[i]) * pImpl->dt * 0.001;  // Convert to seconds
    }
    
    // Compute STDP-like Hebbian weight change
    for (double preTime : preTimes) {
        for (double postTime : postTimes) {
            // Use exponential kernel for temporal correlation
            double timeDiff = postTime - preTime;
            
            if (timeDiff > 0) {
                // Postsynaptic spike after presynaptic spike: LTP
                // Exponential decay with time constant
                weightChange += pImpl->learningRate * std::exp(-timeDiff / pImpl->tau_positive);
            } else {
                // Presynaptic spike after postsynaptic spike: LTD
                // Exponential decay with time constant
                weightChange -= pImpl->learningRate * std::exp(timeDiff / pImpl->tau_negative);
            }
        }
    }
    
    // Apply weight change with bounds
    if (weightChange != 0.0f) {
        applyWeightChange(synapse, weightChange);
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

} // namespace nlm
