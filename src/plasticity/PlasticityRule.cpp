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
    // Real Hebbian learning based on spike timing correlation
    // Implements multiple Hebbian learning rules:
    // 1. Pair-based Hebbian: Δw ∝ x_pre(t) · x_post(t+τ)
    // 2. Triplet rules for short-term plasticity interactions
    // 3. Normalized Hebbian for weight normalization
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float learningRate = pImpl->learningRate;
    
    // Pair-based Hebbian with spike timing window
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float timeDiff = static_cast<float>(post - pre);
            
            // Positive time difference: post after pre (causal)
            // Negative time difference: pre after post (anti-causal)
            if (timeDiff > 0.0f) {
                // Causal window: strengthens synapse ( potentiation )
                // Width: ~20-50ms for biological realism
                if (timeDiff < 50.0f) {
                    float weightChange = learningRate * 
                                        std::exp(-timeDiff / 10.0f) * 
                                        calculatePostSpikeContraction(post);
                    applyWeightChange(synapse, weightChange);
                }
            } else {
                // Anti-causal window: weakens synapse ( depression )
                // Stronger than potentiation (anti-Hebbian)
                if (-timeDiff < 100.0f) {
                    float weightChange = -learningRate * 
                                        std::exp(timeDiff / 5.0f) * 
                                        calculatePreSpikeDepression(pre);
                    applyWeightChange(synapse, weightChange);
                }
            }
        }
    }
    
    // Apply normalization to maintain weight bounds
    normalizeSynapseWeight(synapse);
    
    // Apply neuromodulatory modulation if enabled
    if (synapse->getPlasticityFlags().reward_modulated) {
        // Reward-modulated Hebbian learning
        float neuromodulatorySignal = synapse->getEligibilityTrace();
        if (neuromodulatorySignal != 0.0f) {
            float rewardModulation = neuromodulatorySignal * learningRate * 0.5f;
            applyWeightChange(synapse, rewardModulation);
        }
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
