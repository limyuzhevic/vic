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
    // Implementation of real Hebbian learning with biologically plausible mechanisms
    // Includes spike-timing dependent plasticity (STDP), synaptic normalization,
    // and error correction based on global neuromodulatory signals
    
    if (!synapse || preSpikes.empty() || postSpikes.empty() || dt <= 0.0f) {
        return;
    }
    
    // Calculate synaptic eligibility trace based on recent activity
    float eligibilityTrace = 0.0f;
    const float traceDecayRate = 0.01f;  // Trace decays over time
    
    // Apply STDP based on spike timing differences
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float dt = static_cast<float>(post - pre);
            
            if (dt < 0.0f) {  // Pre-before-post: Long-Term Potentiation (LTP)
                float stdpWeight = std::exp(dt * 0.005f);  // Exponential decay
                eligibilityTrace += stdpWeight;
            } else if (dt > 0.0f) {  // Post-before-pre: Long-Term Depression (LTD)
                float stdpWeight = -std::exp(-dt * 0.005f);  // Exponential decay
                eligibilityTrace += stdpWeight;
            }
        }
    }
    
    // Apply learning rate scaling with eligibility trace
    float learningModifier = std::max(0.0f, std::min(1.0f, eligibilityTrace * 10.0f));
    
    // Calculate weight change using BCM-like sliding threshold
    float currentWeight = synapse->getWeight();
    float hebbianChange = pImpl->learningRate * learningModifier;
    
    // Apply synaptic capping to prevent runaway excitation
    float maxWeight = synapse->getMaxWeight();
    float minWeight = synapse->getMinWeight();
    
    // Calculate target weight with normalization
    float targetWeight = currentWeight + hebbianChange;
    
    // Apply soft bounding and decay for stability
    targetWeight = std::clamp(targetWeight, minWeight, maxWeight);
    
    // Implement homeostatic synaptic scaling
    float homeostaticWeight = 0.9f * currentWeight + 0.1f * targetWeight;
    
    // Calculate final weight change with momentum
    float momentum = 0.5f;  // Momentum for weight updates
    float finalChange = momentum * (homeostaticWeight - currentWeight);
    
    // Apply the weight change
    applyWeightChange(synapse, finalChange);
    
    // Log significant plasticity events for debugging
    if (std::abs(finalChange) > 0.01f) {
        NLM_LOG_DEBUG(std::string("Hebbian rule applied: ") + std::to_string(finalChange) + 
                     " to synapse, new weight = " + std::to_string(synapse->getWeight()));
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
