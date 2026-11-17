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
    if (preSpikes.empty() || postSpikes.empty() || !synapse) {
        return;
    }
    
    // Count coincident spikes with precise timing
    size_t coincidences = 0;
    float totalTemporalDifference = 0.0f;
    size_t validPairs = 0;
    
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float timeDiff = std::abs(static_cast<float>(pre) - static_cast<float>(post));
            // Use shorter time window for more precise Hebbian learning
            if (timeDiff < 5.0f) {  // 5ms window instead of 10ms
                ++coincidences;
                if (timeDiff > 0.0f) {
                    // Stronger learning for precisely timed spikes
                    totalTemporalDifference += 1.0f / timeDiff;
                    ++validPairs;
                }
            }
        }
    }
    
    // Apply Hebbian learning with temporal precision
    if (coincidences > 0) {
        float learningStrength = pImpl->learningRate;
        
        // Temporal factor: more precise timing = stronger learning
        if (validPairs > 0) {
            learningStrength *= std::min(2.0f, totalTemporalDifference / static_cast<float>(validPairs));
        }
        
        // Apply weight change with hard bounds to prevent saturation
        float delta = learningStrength * static_cast<float>(coincidences);
        
        // Hebbian learning is always potentiation (weights increase)
        synapse->addToWeight(std::min(delta, 0.5f));  // Cap increase per update
        
        NLM_LOG_TRACE("Hebbian learning: synapse weight changed by " + std::to_string(delta) +
                     ", total coincidences: " + std::to_string(coincidences));
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
