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
    // Implement real Hebbian learning based on spike timing
    // LTP (Long-Term Potentiation): Pre before Post (pre < post)
    // LTD (Long-Term Depression): Post before Pre (post < pre)
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate symmetric spike time differences
    float totalLTP = 0.0f;  // Long-Term Potentiation
    float totalLTD = 0.0f;  // Long-Term Depression
    
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float timeDiff = post - pre;
            
            if (timeDiff > 0.0f) {
                // Pre before Post: LTP
                // Maximum effect at ~20ms, decays beyond that
                float ltp = std::exp(-timeDiff / 20.0f) * pImpl->learningRate;
                totalLTP += ltp;
            } else if (timeDiff < 0.0f) {
                // Post before Pre: LTD
                // Maximum effect at ~20ms, decays beyond that
                float ltd = std::exp(timeDiff / 20.0f) * pImpl->learningRate;
                totalLTD += ltd;
            }
        }
    }
    
    // Apply net weight change (LTP - LTD)
    float netChange = totalLTP - totalLTD;
    if (netChange != 0.0f) {
        applyWeightChange(synapse, netChange);
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
