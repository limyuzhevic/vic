#include "Hebbian.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt) {
    /*
     * Enhanced Hebbian learning implementation for selective learning
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Enhanced with adaptive learning and selective mechanisms:
     *   - Adaptive learning rate based on network stability
     *   - Competition between synapses for resources
     *   - Activity-dependent learning
     *   - Better temporal filtering for selective pairing
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate average firing rates for adaptive learning
    float preRate = static_cast<float>(preSpikes.size()) / dt;
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    float avgActivity = (preRate + postRate) * 0.5f;
    
    // Adaptive learning rate based on network stability
    // High activity -> lower learning rate to prevent runaway changes
    // Low activity -> higher learning rate to promote exploration
    float activityFactor = std::clamp(1.0f - avgActivity / 100.0f, 0.1f, 1.0f);
    float adaptiveLearningRate = pImpl->learningRate * activityFactor;
    
    // Enhanced spike pair detection with longer, configurable time window
    // This allows for more selective pairing and temporal integration
    float timeWindow = 0.2f + pImpl->learningRate * 0.3f;  // 0.2-0.5s
    float delta = 0.0f;
    
    // Find correlated spike pairs within time window
    for (size_t preIdx = 0; preIdx < preSpikes.size(); ++preIdx) {
        for (size_t postIdx = 0; postIdx < postSpikes.size(); ++postIdx) {
            float timeDiff = static_cast<float>(std::abs(
                preSpikes[preIdx] - postSpikes[postIdx]));
            
            if (timeDiff < timeWindow) {
                // Weighted contribution based on proximity
                float weight = 1.0f - (timeDiff / timeWindow);
                delta += weight * adaptiveLearningRate;
                
                // Early exit if we found a strong pair to avoid overcounting
                if (std::abs(delta) > adaptiveLearningRate * 5.0f) {
                    break;
                }
            }
        }
    }
    
    if (synapse->getType() == SynapseType::Inhibitory) {
        delta = -delta;  // strengthen inhibition, don't flip it to excitatory
    }
    
    // Apply competition: only the most strongly correlated synapses learn
    float coactivity = std::abs(delta);
    static thread_local float lastCoactivity = 0.0f;
    
    if (coactivity > pImpl->covarianceThreshold) {
        // Competitive learning: only highest coactivity wins
        if (coactivity > lastCoactivity * 2.0f) {
            lastCoactivity = coactivity;
            applySelectiveWeightChange(synapse, delta);
        }
    }
    
    // Activity-dependent scaling: stronger correlations get larger updates
    if (std::abs(delta) > 1e-6f) {
        float activityScale = 1.0f + std::log10f(1.0f + coactivity);
        delta *= activityScale;
        
        // Apply with bounds. Enhanced soft bound with activity-dependent limits
        delta *= (1.0f - std::abs(synapse->getWeight()) * 1.5f);
        applyWeightChange(synapse, delta);
        
        // Maintain minimum weight to prevent complete silencing
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
    }
}

void Hebbian::applySelectiveWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Activity-dependent weight change with selective learning
    float newWeight = synapse->getWeight() + delta;
    
    // Apply more sophisticated bounds based on synapse type and activity
    if (synapse->getType() == SynapseType::Excitatory) {
        newWeight = std::clamp(newWeight, 0.0f, pImpl->maxWeight * (1.5f - synapse->getWeight()));
        // Encourage moderate weights for better network dynamics
        if (newWeight > 0.8f) newWeight *= 0.8f;
    } else if (synapse->getType() == SynapseType::Inhibitory) {
        newWeight = std::clamp(newWeight, pImpl->minWeight, 0.0f);
        // Prevent inhibitory weights from becoming too small
        if (newWeight > -0.02f) newWeight = -0.02f;
    } else {
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    }
    
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
