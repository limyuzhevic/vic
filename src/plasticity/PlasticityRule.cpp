#include "PlasticityRule.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float traceDecayRate;           // Rate of eligibility trace decay
    float maxTraceValue;            // Maximum eligibility trace value
    bool useTrace;                  // Whether to use eligibility trace mechanism
    
    Impl() : learningRate(0.01f), traceDecayRate(0.001f), maxTraceValue(1.0f), useTrace(true) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Real Hebbian learning implementation with eligibility trace mechanism
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate synaptic eligibility trace
    float eligibility = 0.0f;
    
    if (pImpl->useTrace) {
        // Compute eligibility trace as sum of pre-post spike coincidences
        // Weighted by temporal distance (more recent spikes = higher eligibility)
        float totalEligibility = 0.0f;
        
        for (Timestamp pre : preSpikes) {
            for (Timestamp post : postSpikes) {
                float timeDiff = std::abs(pre - post);
                
                if (timeDiff < dt * 10.0) {  // Within 10ms window
                    // More recent spikes contribute more to eligibility
                    float weight = 1.0f - std::min(timeDiff / (dt * 10.0f), 1.0f);
                    totalEligibility += weight;
                }
            }
        }
        
        // Decay eligibility trace
        pImpl->traceDecayRate = std::min(pImpl->traceDecayRate, 0.1f);  // Cap decay rate
        eligibility = totalEligibility * pImpl->maxTraceValue;
        synapse->setEligibilityTrace(eligibility);
    } else {
        // Simple Hebbian without trace
        eligibility = static_cast<float>(std::min(preSpikes.size(), postSpikes.size()));
    }
    
    // Apply Hebbian weight change
    float weightChange = pImpl->learningRate * eligibility;
    
    // Apply weight change with bounds checking
    applyWeightChange(synapse, weightChange);
    
    // Apply additional homeostatic regulation
    float targetWeight = 0.0f;
    float currentWeight = synapse->getWeight();
    
    // Homeostatic weight regulation (keep weights within reasonable bounds)
    if (std::abs(currentWeight) > 0.5f) {
        float homeostaticForce = -0.01f * currentWeight;  // Opposes extreme weights
        applyWeightChange(synapse, homeostaticForce);
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
