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
    
    Impl() : learningRate(0.01f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Real Hebbian learning implementation
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate average pre- and post-synaptic spike times
    double avgPreTime = 0.0, avgPostTime = 0.0;
    for (const auto& time : preSpikes) {
        avgPreTime += time;
    }
    for (const auto& time : postSpikes) {
        avgPostTime += time;
    }
    avgPreTime /= preSpikes.size();
    avgPostTime /= postSpikes.size();
    
    // Calculate time difference (cross-correlation peak)
    double timeDiff = avgPostTime - avgPreTime;
    
    // Apply Hebbian rule: strengthen synapse if pre fires before post (causal relationship)
    // Weight change = learning_rate * firing_correlation * sign(timeDiff)
    double correlation = 0.0;
    for (const auto& preTime : preSpikes) {
        for (const auto& postTime : postSpikes) {
            // Calculate cross-correlation: penalize anti-correlated activity
            double delta = postTime - preTime;
            if (delta > 0.0 && delta < 100.0) { // Within biological window
                correlation += std::exp(-delta / 10.0); // Temporal decay
            }
        }
    }
    
    correlation /= static_cast<double>(preSpikes.size() * postSpikes.size());
    
    // Apply weight change
    double weightChange = pImpl->learningRate * correlation;
    
    // Constrain to biological limits
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + static_cast<float>(weightChange * dt);
    
    // Ensure weight stays within biological bounds [-1.0, 1.0]
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    
    // Apply the change
    synapse->addToWeight(newWeight - currentWeight);
    
    // Record Hebbian update for eligibility trace
    synapse->setEligibilityTrace(synapse->getEligibilityTrace() + static_cast<float>(correlation));
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Biological constraint: apply weight change with saturation
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply hard bounds based on synapse type
    if (synapse->isExcitatory()) {
        newWeight = std::clamp(newWeight, 0.0f, 1.0f);
    } else if (synapse->isInhibitory()) {
        newWeight = std::clamp(newWeight, -1.0f, 0.0f);
    } else {
        newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    }
    
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    // Biological constraint: learning rate should be between 0.001 and 0.1
    pImpl->learningRate = std::clamp(rate, 0.001f, 0.1f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
