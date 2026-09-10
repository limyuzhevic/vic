#include "PlasticityRule.hpp"
#include "../../core/Logger/Logger.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    // Add logging for rule status
    NLM_LOG_DEBUG("PlasticityRule::isEnabled(): Checking if rule is enabled");
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    NLM_LOG_DEBUG("PlasticityRule::setEnabled(): Setting rule enabled state to " + std::to_string(enabled));
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.01f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {
    NLM_LOG_DEBUG("HebbianRule::HebbianRule(): Created Hebbian rule with learning rate " + std::to_string(pImpl->learningRate));
}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    NLM_LOG_DEBUG("HebbianRule::update(): Updating synapse with " + std::to_string(preSpikes.size()) + 
                 " pre-spikes and " + std::to_string(postSpikes.size()) + " post-spikes");
    
    if (!synapse) {
        NLM_LOG_ERROR("HebbianRule::update(): Synapse pointer is null");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        NLM_LOG_WARNING("HebbianRule::update(): Empty spike history provided (preSpikes: " + 
                       std::to_string(preSpikes.size()) + ", postSpikes: " + std::to_string(postSpikes.size()) + ")");
        return;
    }
    
    // Validate timestep
    if (dt <= 0.0) {
        NLM_LOG_ERROR("HebbianRule::update(): Invalid timestep " + std::to_string(dt));
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
    
    NLM_LOG_DEBUG("HebbianRule::update(): Found " + std::to_string(coincidences) + " coincident spike pairs");
    
    // Apply weight change proportional to coincidences
    if (coincidences > 0) {
        float weightChange = pImpl->learningRate * static_cast<float>(coincidences);
        NLM_LOG_DEBUG("HebbianRule::update(): Applying weight change of " + std::to_string(weightChange));
        applyWeightChange(synapse, weightChange);
    } else {
        NLM_LOG_DEBUG("HebbianRule::update(): No coincident spikes, skipping weight update");
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        NLM_LOG_ERROR("HebbianRule::applyWeightChange(): Synapse pointer is null");
        return;
    }
    
    if (std::abs(delta) > 1.0f) {
        NLM_LOG_WARNING("HebbianRule::applyWeightChange(): Large weight change " + std::to_string(delta) + 
                       ", may cause instability");
    }
    
    NLM_LOG_DEBUG("HebbianRule::applyWeightChange(): Applying delta " + std::to_string(delta));
    synapse->addToWeight(delta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    // Validate learning rate
    if (rate < 0.0f || rate > 1.0f) {
        NLM_LOG_ERROR("HebbianRule::setLearningRate(): Invalid learning rate " + std::to_string(rate) + 
                     ", must be in range [0.0, 1.0]");
        return;
    }
    
    NLM_LOG_DEBUG("HebbianRule::setLearningRate(): Setting learning rate to " + std::to_string(rate));
    pImpl->learningRate = rate;
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
