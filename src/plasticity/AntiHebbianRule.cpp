#include "../../plasticity/PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct AntiHebbianRule::Impl {
    float learningRate;
    float threshold;  // Activity threshold for anti-Hebbian effect
    
    Impl() : learningRate(0.01f), threshold(0.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    // Anti-Hebbian learning: "neurons that fire together, weaken together"
    // Implemented as negative correlation learning
    //
    // Mathematical formulation:
    // Δw = -η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)  // For covariance-based anti-Hebbian
    //
    // This implements "neurons that fire together, wire together" but in reverse
    // i.e., they fire together, they wire together but with decreased strength
    //
    // Biological inspiration:
    //   - Reflects LTD mechanisms at synapses
    //   - Competitive learning in neural circuits
    //   - Prevents runaway excitation in networks
    //
    // Limitations:
    //   - Doesn't account for spike timing details
    //   - Simple learning rule may not capture complex dynamics
    //
    
    if (preSpikes.empty() || postSpikes.empty() || !synapse) {
        return;
    }
    
    // Calculate spike correlation metrics
    float preMean = 0.0f, postMean = 0.0f;
    for (Timestamp t : preSpikes) {
        preMean += static_cast<float>(t);
    }
    for (Timestamp t : postSpikes) {
        postMean += static_cast<float>(t);
    }
    preMean /= static_cast<float>(preSpikes.size());
    postMean /= static_cast<float>(postSpikes.size());
    
    float prePostCorrelation = 0.0f;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            prePostCorrelation += (static_cast<float>(preTime) - preMean) * 
                                  (static_cast<float>(postTime) - postMean);
        }
    }
    prePostCorrelation /= static_cast<float>(preSpikes.size() * postSpikes.size());
    
    // Apply anti-Hebbian weight change
    // Strong negative correlation leads to weight depression
    float delta = -pImpl->learningRate * prePostCorrelation;
    
    // Apply weight change with bounds
    applyWeightChange(synapse, delta);
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to biologically plausible bounds
    float newWeight = synapse->getWeight() + delta;
    // Anti-Hebbian rules typically work within narrower bounds than standard Hebbian
    newWeight = std::clamp(newWeight, -0.5f, 0.5f);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float AntiHebbianRule::getThreshold() const {
    return pImpl->threshold;
}

} // namespace nlm
