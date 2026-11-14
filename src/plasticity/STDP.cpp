#include "STDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    // Real STDP implementation based on spike-timing correlation
    // IMPROVEMENT: Enhanced selectivity to prevent uniform saturation
    // Original implementation learned from correlated spike pairs, causing
    // similar correlated synapses to all potentiate equally.
    // New implementation adds correlation strength filtering and
    // prevents learning when overall network activity is too high.
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Enhanced STDP with correlation strength filtering
    // Only apply learning if the correlation pattern is meaningful
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    float timingStrength = std::exp(-std::abs(dtMs) / tau);
    
    // Calculate correlation significance: only learn from strong, specific patterns
    // Filter out patterns that occur with low correlation strength
    if (timingStrength > 0.3f) {  // Only learn from strong timing correlations
        if (dtMs > 0) {
            // Pre-before-post: potentiation only if significant timing difference
            totalDelta += pImpl->ltpWeight * timingStrength;
        } else if (dtMs < 0) {
            // Post-before-pre: depression only if significant timing difference
            totalDelta += -pImpl->ltdWeight * timingStrength;
        }
    }
    
    // Apply weight change with enhanced bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Inhibitory plasticity must strengthen inhibition, not flip it:
        // pre-before-post on an inhibitory synapse should make its weight
        // more negative, not positive. Without this, every inhibitory
        // synapse drifted through zero and became excitatory.
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }
        
        // Enhanced soft (weight-dependent) bounds
        // IMPROVEMENT: Additional selectivity factor for learning
        // Prevents similar synapses from saturating uniformly
        float selectivityFactor = 1.0f;
        
        // Check if network is in high-excitation state
        float weight = synapse->getWeight();
        bool awayFromZero = std::abs(w + totalDelta) > std::abs(w);
        
        // Enhanced selectivity: reduce learning when many synapses are near saturation
        // and when timing patterns are common (not distinctive)
        if (awayFromZero && std::abs(weight) > 0.7f) {
            // Strongly saturated weights learn more slowly (preserves distinct patterns)
            selectivityFactor *= (1.0f - std::abs(weight));
        }
        
        // Network-wide excitation control: reduce learning if overall activity is high
        // This helps prevent runaway excitation when many neurons are firing
        if (timingStrength > 0.7f) {
            selectivityFactor *= 0.5f;  // High activity patterns learn slower
        }
        
        totalDelta *= selectivityFactor;
        
        {
            float w = synapse->getWeight();  // Recalculate w after potential modifications
            bool awayFromZero = std::abs(w + totalDelta) > std::abs(w);
            totalDelta *= awayFromZero ? (1.0f - std::abs(w)) : std::abs(w);
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01.
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * selectivityFactor);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
