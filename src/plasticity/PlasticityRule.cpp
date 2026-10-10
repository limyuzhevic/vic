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
    // Enhanced Hebbian learning with better validation and biological plausibility
    if (!synapse) {
        return;  // Defensive programming
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;  // No correlation without both pre and post spikes
    }
    
    if (dt <= 0.0f) {
        return;  // Invalid timestep duration
    }
    
    // Count coincident spikes with adaptive time window based on dt
    size_t coincidences = 0;
    float timeWindow = 10.0f + dt * 100.0f;  // Adaptive window: 10ms + dt-dependent
    
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(static_cast<float>(pre - post)) < timeWindow) {
                ++coincidences;
            }
        }
    }
    
    // Apply Hebbian learning with biologically realistic bounds
    if (coincidences > 0) {
        // Calculate delta with multiple factors for realism
        float baseDelta = pImpl->learningRate * static_cast<float>(coincidences);
        
        // Apply synaptic efficacy modulation
        float efficacy = synapse->getEfficacy();
        float modulatedDelta = baseDelta * efficacy;
        
        // Apply eligibility trace if available
        float eligibility = synapse->getEligibilityTrace();
        modulatedDelta *= (1.0f + eligibility * 0.5f);  // Boost by eligibility
        
        // Clamp delta to reasonable range
        modulatedDelta = std::clamp(modulatedDelta, -1.0f, 1.0f);
        
        applyWeightChange(synapse, modulatedDelta);
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
