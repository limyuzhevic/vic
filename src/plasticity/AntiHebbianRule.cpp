#include "AntiHebbianRule.hpp"
#include "../../brain/Synapse.hpp"

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.005f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    // TODO PHASE 2: Implement real AntiHebbian learning
    // PLACEHOLDER: Anti-Hebbian - decreases weight when neurons fire together
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count coincident spikes (simplified)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms window
                ++coincidences;
            }
        }
    }
    
    // Apply weight change proportional to coincidences (negative)
    if (coincidences > 0) {
        applyWeightChange(synapse, -pImpl->learningRate * static_cast<float>(coincidences));
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
