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

// Implement real Hebbian learning based on spike timing
    // Hebbian learning: synapses that fire together strengthen
    // Based on STDP principles but simpler for demonstration
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate average pre and post spike times
    float avgPreTime = 0.0f;
    for (Timestamp pre : preSpikes) {
        avgPreTime += static_cast<float>(pre);
    }
    avgPreTime /= static_cast<float>(preSpikes.size());
    
    float avgPostTime = 0.0f;
    for (Timestamp post : postSpikes) {
        avgPostTime += static_cast<float>(post);
    }
    avgPostTime /= static_cast<float>(postSpikes.size());
    
    // Determine learning direction based on timing
    // Pre-before-post (causal) -> LTP (strengthening)
    // Post-before-pre (anti-causal) -> LTD (weakening)
    float timeDifference = avgPreTime - avgPostTime;
    
    float weightChange = 0.0f;
    const float learningRate = pImpl->learningRate;
    
    if (timeDifference > 0) {
        // Causal activity: pre fires before post
        // This indicates the synapse likely contributed to post firing
        // Strengthen the connection (LTP)
        float proximity = std::min(1.0f, timeDifference / 10.0f);  // Normalize to 10ms window
        weightChange = learningRate * proximity;
    } else if (timeDifference < 0) {
        // Anti-causal activity: post fires before pre
        // This suggests the synapse may be inhibitory or incorrectly timed
        // Weaken the connection (LTD)
        float proximity = std::min(1.0f, std::abs(timeDifference) / 10.0f);
        weightChange = -learningRate * proximity * 0.8f;  // Slightly weaker LTD
    }
    // If timeDifference == 0, no change (simultaneous firing)
    
    // Apply the weight change with biological constraints
    if (std::abs(weightChange) > 1e-6f) {  // Minimum change threshold
        // Constrain weight change based on current weight
        float currentWeight = synapse->getWeight();
        float maxChange = 0.1f * std::abs(currentWeight);  // Max 10% change per step
        
        if (std::abs(weightChange) > maxChange) {
            weightChange = (weightChange > 0 ? 1.0f : -1.0f) * maxChange;
        }
        
        // Apply biological constraints: weights must stay within reasonable bounds
        float newWeight = currentWeight + weightChange;
        
        // Constrain weights to reasonable ranges
        const float minWeight = -0.5f;  // Minimum (inhibitory)
        const float maxWeight = 1.0f;   // Maximum (excitatory)
        
        newWeight = std::clamp(newWeight, minWeight, maxWeight);
        
        // Only apply if change is significant
        if (std::abs(newWeight - currentWeight) > 1e-6f) {
            synapse->setWeight(newWeight);
        }
        
        // Record the learning event
        syn->addToEligibilityTrace(weightChange);
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
