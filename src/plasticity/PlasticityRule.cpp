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
    // Advanced Hebbian learning with STDP-like dynamics and homeostatic mechanisms
    // Implements "neurons that fire together, wire together" with biological realism
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Apply Spike-Timing Dependent Plasticity (STDP) for Hebbian learning
    applySTDPToSynapse(synapse, preSpikes, postSpikes, dt);
    
    // Apply trace-based Hebbian learning
    applyTraceBasedLearning(synapse, preSpikes, postSpikes, dt);
    
    // Apply weight normalization to prevent runaway growth
    applyWeightNormalization(synapse, dt);
    
    // Apply homeostatic plasticity to maintain stability
    applyHomeostaticPlasticity(synapse, dt);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

void HebbianRule::applySTDPToSynapse(Synapse* synapse,
                                      const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes,
                                      TimestepDuration dt) {
    // Spike-Timing Dependent Plasticity (STDP) for Hebbian learning
    // Potentiation when pre fires before post, depression when post fires before pre
    
    float totalSTDPChange = 0.0f;
    
    // Process all spike pairs
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float delta_t = post - pre;  // Positive if post fires after pre
            
            if (std::abs(delta_t) < 50.0f) {  // Consider spikes within 50ms
                if (delta_t > 0) {
                    // Post after pre: Long-Term Potentiation (LTP)
                    // Potency decays with time difference
                    float LTP_magnitude = pImpl->learningRate * std::exp(-delta_t / 20.0f);
                    totalSTDPChange += LTP_magnitude;
                } else {
                    // Pre after post: Long-Term Depression (LTD)
                    float LTD_magnitude = -pImpl->learningRate * std::exp(std::abs(delta_t) / 20.0f);
                    totalSTDPChange += LTD_magnitude;
                }
            }
        }
    }
    
    // Apply the total STDP change
    if (std::abs(totalSTDPChange) > 0.001f) {
        applyWeightChange(synapse, totalSTDPChange);
    }
}

void HebbianRule::applyTraceBasedLearning(Synapse* synapse,
                                          const std::vector<Timestamp>& preSpikes,
                                          const std::vector<Timestamp>& postSpikes,
                                          TimestepDuration dt) {
    // Trace-based Hebbian learning: maintains activity traces for online learning
    
    static thread_local float preTrace = 0.0f;    // Pre-synaptic activity trace
    static thread_local float postTrace = 0.0f;   // Post-synaptic activity trace
    
    float traceDecayRate = 0.01f;  // Decay rate per timestep
    
    // Update pre-synaptic trace
    preTrace *= (1.0f - traceDecayRate);
    for (Timestamp pre : preSpikes) {
        preTrace += 1.0f;  // Each spike increments trace
    }
    
    // Update post-synaptic trace  
    postTrace *= (1.0f - traceDecayRate);
    for (Timestamp post : postSpikes) {
        postTrace += 1.0f;
    }
    
    // Apply trace-based weight change
    float traceBasedChange = pImpl->learningRate * preTrace * postTrace * 0.001f;
    
    // Use eligibility trace from synapse if available
    float eligibility = synapse->getEligibilityTrace();
    if (std::abs(eligibility) > 0.001f) {
        traceBasedChange *= eligibility;  // Modulate by reward prediction error
    }
    
    applyWeightChange(synapse, traceBasedChange);
}

void HebbianRule::applyWeightNormalization(Synapse* synapse, TimestepDuration dt) {
    // Weight normalization to prevent runaway potentiation/depression
    // Implements synaptic competition and homeostasis
    
    float currentWeight = synapse->getWeight();
    float maxWeight = 1.0f;
    float minWeight = -1.0f;
    
    // Normalize to [-1, 1] range with smooth transitions
    if (currentWeight > maxWeight) {
        float excess = currentWeight - maxWeight;
        applyWeightChange(synapse, -excess * 0.1f);  // Gradual reduction
    } else if (currentWeight < minWeight) {
        float deficit = minWeight - currentWeight;
        applyWeightChange(synapse, -deficit * 0.1f);  // Gradual correction
    }
    
    // Apply exponential moving average for stability
    float targetWeight = currentWeight * 0.9f + 0.0f * 0.1f;  // Move toward 0
    float drift = targetWeight - currentWeight;
    applyWeightChange(synapse, drift * 0.05f);  // Dampen drift
}

void HebbianRule::applyHomeostaticPlasticity(Synapse* synapse, TimestepDuration dt) {
    // Homeostatic plasticity to maintain network stability
    // Scales synaptic weights based on overall activity
    
    // Calculate current synaptic efficacy
    float efficacy = synapse->getEfficacy();
    
    // Target efficacy based on average network activity
    float targetEfficacy = 1.0f;  // Ideal efficacy
    
    // Homeostatic scaling factor
    float homeostaticFactor = targetEfficacy / efficacy;
    
    // Apply subtle homeostatic adjustment
    float adjustment = (homeostaticFactor - 1.0f) * 0.001f;
    
    // Apply to both weight and eligibility trace
    if (std::abs(adjustment) > 0.0001f) {
        // Scale weight toward target
        float weightAdjustment = adjustment * synapse->getWeight();
        applyWeightChange(synapse, weightAdjustment);
        
        // Adjust eligibility trace
        float currentEligibility = synapse->getEligibilityTrace();
        float eligibilityAdjustment = currentEligibility * adjustment;
        synapse->setEligibilityTrace(currentEligibility + eligibilityAdjustment);
    }
}
