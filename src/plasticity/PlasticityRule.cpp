#include "PlasticityRule.hpp"
#include <cmath>
#include <algorithm>

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
    // Enhanced Hebbian learning with Oja's normalization rule
    // This prevents weight explosion while maintaining learning capability
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Validate timestep
    if (dt <= 0.0) {
        return;
    }
    
    // Get current synapse properties
    float weight = synapse->getWeight();
    float learningRate = pImpl->learningRate;
    
    // Calculate synaptic efficacy (includes short-term plasticity)
    float efficacy = synapse->getEfficacy();
    
    // Find most recent pre- and post-synaptic spikes
    Timestamp lastPreSpike = preSpikes.back();
    Timestamp lastPostSpike = postSpikes.back();
    
    // Calculate spike timing difference for STDP-like influence
    float spikeTimingDiff = static_cast<float>(lastPreSpike - lastPostSpike);
    
    // Standard Hebbian component: weight change proportional to correlated activity
    float hebbianComponent = learningRate * efficacy;
    
    // Oja's normalization: prevents runaway excitation by normalizing learning
    float ojaNormalization = learningRate * efficacy * (efficacy - weight * efficacy * efficacy);
    
    // Combined Hebbian-Oja update
    float delta = hebbianComponent + ojaNormalization;
    
    // Apply STDP modulation based on spike timing
    if (std::abs(spikeTimingDiff) < 50.0f) {  // Within 50ms window
        float stdpFactor = 1.0f - std::abs(spikeTimingDiff) / 50.0f;
        
        if (spikeTimingDiff > 0) {
            // Pre before post: LTP (Long-Term Potentiation)
            delta += learningRate * 0.5f * stdpFactor;
        } else if (spikeTimingDiff < 0) {
            // Post before pre: LTD (Long-Term Depression)
            delta -= learningRate * 0.3f * stdpFactor;
        }
    }
    
    // Apply weight change with bounds checking
    float newWeight = weight + delta;
    newWeight = std::clamp(newWeight, 0.0f, 2.0f);  // Keep weights in reasonable range
    
    float actualDelta = newWeight - weight;
    if (std::abs(actualDelta) > 1e-6f) {
        applyWeightChange(synapse, actualDelta);
    }
    
    // Update eligibility trace for reward-modulated plasticity
    float currentEligibility = synapse->getEligibilityTrace();
    float newEligibility = currentEligibility + efficacy * stdpFactor;
    synapse->setEligibilityTrace(newEligibility);
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

namespace nlm {

void AntiHebbianRule::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    // Anti-Hebbian learning: "neurons that fire together, wire together oppositely"
    // Decreases synaptic weight when neurons fire together
    // Used for inhibitory learning and stability in neural networks
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Validate timestep
    if (dt <= 0.0) {
        return;
    }
    
    // Get current synapse properties
    float weight = synapse->getWeight();
    float learningRate = 0.01f;  // Anti-Hebbian learning rate
    
    // Calculate synaptic efficacy
    float efficacy = synapse->getEfficacy();
    
    // Find most recent pre- and post-synaptic spikes
    Timestamp lastPreSpike = preSpikes.back();
    Timestamp lastPostSpike = postSpikes.back();
    
    // Calculate spike timing difference
    float spikeTimingDiff = static_cast<float>(lastPreSpike - lastPostSpike);
    
    // Anti-Hebbian component: weight decreases with correlated activity
    float antiHebbianComponent = -learningRate * efficacy;
    
    // STDP-like modulation: pre before post typically causes LTP (opposite for anti-Hebbian)
    if (std::abs(spikeTimingDiff) < 50.0f) {
        float stdpFactor = 1.0f - std::abs(spikeTimingDiff) / 50.0f;
        
        if (spikeTimingDiff > 0) {
            // Pre before post: normally LTP, so anti-Hebbian gives LTD
            antiHebbianComponent -= learningRate * 0.5f * stdpFactor;
        } else if (spikeTimingDiff < 0) {
            // Post before pre: normally LTD, so anti-Hebbian gives LTP
            antiHebbianComponent += learningRate * 0.3f * stdpFactor;
        }
    }
    
    // Apply weight change with bounds checking
    float delta = antiHebbianComponent;
    float newWeight = weight + delta;
    newWeight = std::clamp(newWeight, 0.0f, 2.0f);  // Keep weights in reasonable range
    
    float actualDelta = newWeight - weight;
    if (std::abs(actualDelta) > 1e-6f) {
        applyWeightChange(synapse, actualDelta);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (synapse) {
        synapse->addToWeight(delta);
    }
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void BCMRule::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    // Bienenstock-Cooper-Munro (BCM) learning rule
    // Implements synaptic stabilization through a sliding threshold
    // Uses post-synaptic activity to control potentiation vs depression
    
    if (!synapse || postSpikes.empty()) {
        return;
    }
    
    // Validate timestep
    if (dt <= 0.0) {
        return;
    }
    
    // Get current synapse properties
    float weight = synapse->getWeight();
    float learningRate = 0.01f;  // BCM learning rate
    
    // Calculate synaptic efficacy
    float efficacy = synapse->getEfficacy();
    
    // Get most recent post-synaptic spike
    Timestamp lastPostSpike = postSpikes.back();
    
    // Get neuron's current membrane potential (would need to access from neuron)
    // In full implementation, this would require access to the post-synaptic neuron
    // For placeholder, use a simplified model based on spike rate
    float postSynapticActivity = 1.0f;  // Simplified: assume active post-synaptic neuron
    
    // BCM sliding threshold (Theta_M): increases with overall activity
    float thetaM = 0.5f * postSynapticActivity;  // Standard BCM parameter
    thetaM = std::clamp(thetaM, 0.1f, 2.0f);   // Keep within reasonable bounds
    
    // BCM learning rule: delta = eta * x * (y - Theta_M)
    // where x = presynaptic activity, y = postsynaptic activity
    float preSynapticActivity = efficacy;  // Approximate presynaptic activity
    float delta = learningRate * preSynapticActivity * (postSynapticActivity - thetaM);
    
    // Apply weight change with bounds checking
    float newWeight = weight + delta;
    newWeight = std::clamp(newWeight, 0.0f, 2.0f);  // Keep weights in reasonable range
    
    float actualDelta = newWeight - weight;
    if (std::abs(actualDelta) > 1e-6f) {
        applyWeightChange(synapse, actualDelta);
    }
    
    // Update eligibility trace for reward-modulated plasticity
    float currentEligibility = synapse->getEligibilityTrace();
    float newEligibility = currentEligibility + preSynapticActivity * postSynapticActivity;
    synapse->setEligibilityTrace(newEligibility);
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (synapse) {
        synapse->addToWeight(delta);
    }
}

const char* BCMRule::getName() const {
    return "BCM";
}

} // namespace nlm
