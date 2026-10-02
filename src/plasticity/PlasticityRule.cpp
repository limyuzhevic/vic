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
    // Real Hebbian learning: "fire together, wire together"
    // Based on spike timing dependent plasticity with eligibility traces
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike pairing statistics
    float totalPairingStrength = 0.0f;
    size_t validPairings = 0;
    
    // For each pre-synaptic spike, find nearest post-synaptic spike
    for (Timestamp preTime : preSpikes) {
        Timestamp bestPostTime = -1;
        float minTimeDiff = std::numeric_limits<float>::max();
        
        for (Timestamp postTime : postSpikes) {
            float timeDiff = std::abs(static_cast<float>(preTime - postTime));
            
            // Strong pairing within narrow time window (STDP-like)
            if (timeDiff < 20.0f) {  // 20ms window
                if (timeDiff < minTimeDiff) {
                    minTimeDiff = timeDiff;
                    bestPostTime = postTime;
                }
            }
        }
        
        if (bestPostTime >= 0 && minTimeDiff < std::numeric_limits<float>::max()) {
            // Calculate pairing strength: exponential decay with time difference
            float timeDiff = static_cast<float>(bestPostTime - preTime);
            float timeFactor = std::exp(-std::abs(timeDiff) / 10.0f);  // 10ms time constant
            
            // Stronger when spikes coincide, weaker with larger time difference
            float pairingStrength = pImpl->learningRate * timeFactor;
            totalPairingStrength += pairingStrength;
            ++validPairings;
            
            // Apply immediate Hebbian weight update for coincident spikes
            if (std::abs(timeDiff) < 1.0f) {  // Very close timing
                applyWeightChange(synapse, pairingStrength);
            }
        }
    }
    
    // Add trace-based learning for spikes within learning window
    if (validPairings > 0 && !preSpikes.empty() && !postSpikes.empty()) {
        // Calculate average pairing strength
        float avgPairingStrength = totalPairingStrength / validPairings;
        
        // Update eligibility trace with averaged pairing contribution
        float eligibilityContribution = avgPairingStrength * 0.5f;
        synapse->setEligibilityTrace(synapse->getEligibilityTrace() + eligibilityContribution);
        
        // Apply weight change based on accumulated trace
        float traceWeightChange = synapse->getEligibilityTrace() * pImpl->learningRate * 0.1f;
        applyWeightChange(synapse, traceWeightChange);
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
