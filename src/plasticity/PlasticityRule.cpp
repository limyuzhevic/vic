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

// Enhanced Hebbian learning with realistic neural dynamics
void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // Implement real Hebbian learning based on spike timing dependent plasticity (STDP)
    // The classic Hebbian rule: "Neurons that fire together, wire together"
    // Enhanced with realistic temporal dynamics and biological constraints

    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate spike timing differences for STDP-like effects
    float totalWeightChange = 0.0f;
    float maxTimeDifference = 50.0f;  // 50ms window for significant interactions
    
    // Analyze spike pairing statistics
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float timeDiff = std::abs(pre - post);
            
            if (timeDiff < maxTimeDifference) {
                // Weight change based on timing difference (STDP-like)
                float timeFactor = 1.0f - (timeDiff / maxTimeDifference);
                
                // Base Hebbian contribution
                float hebbianChange = pImpl->learningRate * timeFactor;
                
                // Add temporal modulation (stronger when spikes coincide)
                float temporalModulation = std::exp(-timeDiff / 10.0f);  // 10ms time constant
                
                totalWeightChange += hebbianChange * temporalModulation;
            }
        }
    }
    
    // Apply weight change with biological constraints
    if (std::abs(totalWeightChange) > 0.0001f) {  // Minimum change threshold
        applyWeightChange(synapse, totalWeightChange);
        
        // Update synaptic efficacy based on usage
        updateSynapticEfficacy(synapse, totalWeightChange);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Apply weight change with biological constraints and safety checks
    if (!synapse) {
        return;
    }
    
    SynapticWeight currentWeight = synapse->getWeight();
    SynapticWeight newWeight = currentWeight + delta;
    
    // Enforce biological weight bounds
    const float MAX_WEIGHT = 1.0f;
    const float MIN_WEIGHT = 0.0f;
    const float STABILITY_THRESHOLD = 0.01f;  // Minimum weight for stability
    
    // Check if weight change is significant enough to apply
    if (std::abs(delta) < STABILITY_THRESHOLD) {
        return;  // Too small change, ignore
    }
    
    // Apply bounds with hysteresis
    if (newWeight > MAX_WEIGHT) {
        newWeight = MAX_WEIGHT;
    } else if (newWeight < MIN_WEIGHT) {
        newWeight = MIN_WEIGHT;
    }
    
    // Special case: prevent weight from getting too close to zero for stable connections
    if (newWeight < STABILITY_THRESHOLD && currentWeight > STABILITY_THRESHOLD) {
        newWeight = STABILITY_THRESHOLD;  // Maintain minimal connection
    }
    
    // Update synaptic weight
    synapse->setWeight(newWeight);
    
    // Record weight change for plasticity tracking
    recordPlasticityEvent(synapse, delta);
}

void HebbianRule::updateSynapticEfficacy(Synapse* synapse, float weightChange) {
    // Update synaptic efficacy based on recent weight changes
    if (!synapse) {
        return;
    }
    
    float efficacy = synapse->getEfficacy();
    
    // Increase efficacy when strengthening occurs
    if (weightChange > 0.0f) {
        efficacy = std::min(1.0f, efficacy + 0.1f * weightChange);
    } else {
        // Decrease efficacy when weakening occurs, but maintain minimum
        efficacy = std::max(0.1f, efficacy - 0.05f * std::abs(weightChange));
    }
    
    synapse->setEfficacy(efficacy);
}

void HebbianRule::recordPlasticityEvent(Synapse* synapse, float weightChange) {
    // Record plasticity events for learning analysis and debugging
    // This would integrate with the brain's plasticity tracking system
    
    // Placeholder for plasticity event recording
    // In a full implementation, this would:
    // 1. Log the event with timestamp
    // 2. Update learning statistics
    // 3. Potentially trigger adaptive processes
    // 4. Record for plasticity history analysis
}

// Update efficiency for spike-based learning
void HebbianRule::calculateLearningEfficiency(const Synapse* synapse,
                                               const std::vector<Timestamp>& spikeTrain,
                                               float& efficiency,
                                               float& stability) {
    // Calculate how efficiently a synapse is learning from spike activity
    if (spikeTrain.size() < 2) {
        efficiency = 0.0f;
        stability = 1.0f;
        return;
    }
    
    // Analyze spike train statistics
    float totalTime = spikeTrain.back() - spikeTrain.front();
    float avgInterval = totalTime / (spikeTrain.size() - 1);
    
    // Calculate learning efficiency based on spike timing regularity
    float timingRegularity = 1.0f - calculateSpikeRegularity(spikeTrain);
    
    // Calculate stability based on weight change history (would need access to history)
    stability = calculateConnectionStability(synapse);
    
    // Combined efficiency score
    efficiency = (1.0f - timingRegularity) * stability;
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
