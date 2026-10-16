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
    // TODO PHASE 2: Implement real Hebbian learning
    // PLACEHOLDER: Simple correlated firing increases weight
    
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
    
    // Apply weight change proportional to coincidences
    if (coincidences > 0) {
        applyWeightChange(synapse, pImpl->learningRate * static_cast<float>(coincidences));
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

// Bienenstock-Cooper-Munro (BCM) Rule Implementation
// BCM introduces a sliding threshold that evolves based on postsynaptic activity
struct BCMRule::Impl {
    float learningRate;
    float thetaPlus;      // Sliding threshold (maximum potentiation)
    float thetaMinus;     // Sliding threshold (maximum depression)
    float beta;           // Threshold adaptation rate
    float theta;          // Current sliding threshold
    
    Impl() : learningRate(0.02f), thetaPlus(25.0f), thetaMinus(-25.0f), 
             beta(0.01f), theta(15.0f) {}  // Initial theta
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    // TODO PHASE 2: Implement real BCM rule
    // PLACEHOLDER: BCM rule with sliding threshold
    
    if (preSpikes.empty() || postSpikes.empty() || !synapse) {
        return;
    }
    
    // Calculate average postsynaptic activity (firing rate)
    float postActivity = 0.0f;
    for (Timestamp post : postSpikes) {
        postActivity += static_cast<float>(post);
    }
    postActivity /= postSpikes.size();
    
    // Update sliding threshold theta based on postsynaptic activity
    pImpl->theta += pImpl->beta * (postActivity - pImpl->theta);
    
    // Apply BCM rule: weight change depends on pre-post coincidence AND exceeds threshold
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            // Pre-before-post timing difference
            float dt = static_cast<float>(post - pre);
            
            if (dt > 0) {  // Pre before post
                // Potentiation when postsynaptic activity > theta
                if (postActivity > pImpl->theta) {
                    float weightChange = pImpl->learningRate * dt * (postActivity - pImpl->theta);
                    applyWeightChange(synapse, weightChange);
                }
            } else {  // Post before pre (or simultaneous)
                // Depression when postsynaptic activity < theta
                if (postActivity < pImpl->theta) {
                    float weightChange = -pImpl->learningRate * std::abs(dt) * (pImpl->theta - postActivity);
                    applyWeightChange(synapse, weightChange);
                }
            }
        }
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* BCMRule::getName() const {
    return "BCM";
}

// Anti-Hebbian Rule Implementation
// Anti-Hebbian decreases weight when neurons fire together
void AntiHebbianRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    // TODO PHASE 2: Implement real Anti-Hebbian rule
    // PLACEHOLDER: Anti-Hebbian reduces weight when neurons fire together
    
    if (preSpikes.empty() || postSpikes.empty() || !synapse) {
        return;
    }
    
    // Count coincident spikes (neurons firing at similar times)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 5.0) {  // 5ms window
                ++coincidences;
            }
        }
    }
    
    // Apply negative weight change (depression)
    if (coincidences > 0) {
        float depression = -0.01f * static_cast<float>(coincidences);  // Anti-Hebbian depression
        applyWeightChange(synapse, depression);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

} // namespace nlm
