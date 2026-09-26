#include "PlasticityRule.hpp"

namespace nlm {

// Base class implementations
bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    float normalizationFactor;  // For weight normalization
    float targetCorrelation;    // Target firing correlation
    
    Impl() : learningRate(0.01f), normalizationFactor(0.1f), targetCorrelation(0.5f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    /*
     * Enhanced Hebbian learning implementation
     * 
     * Implements spike-timing dependent Hebbian plasticity with:
     *   - Asymmetric spike timing window (post fires shortly after pre)
     *   - Weight normalization to prevent runaway excitation
     *   - Configuration via parameter map
     *   - Eligibility trace support for reward modulation
     *   
     * Mathematical formulation:
     *   Δw = η * (correlation - target) * f(t_post - t_pre)
     *   Where f(τ) is a spike-timing window function
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate firing correlation with temporal dynamics
    float correlation = calculateFiringCorrelation(preSpikes, postSpikes, dt);
    
    // Calculate weight adjustment based on deviation from target
    float error = correlation - pImpl->targetCorrelation;
    float weightChange = pImpl->learningRate * error * pImpl->normalizationFactor;
    
    // Apply weight change with bounds checking
    if (std::abs(weightChange) > 1e-6f) {
        applyWeightChange(synapse, weightChange);
        
        // Update eligibility trace for reward-modulated learning
        if (synapse->getPlasticityFlags().reward_modulated) {
            float currentTrace = synapse->getEligibilityTrace();
            synapse->setEligibilityTrace(currentTrace + weightChange * 0.1f);
        }
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply basic Hebbian weight change
    synapse->addToWeight(delta);
    
    // Apply weight normalization to prevent runaway excitation
    float currentWeight = synapse->getWeight();
    float targetWeight = 0.0f;  // Hebbian tends to push toward maximum
    
    // Soft normalization to maintain biological plausibility
    if (std::abs(currentWeight) > 0.9f) {
        float normalizedWeight = targetWeight + (currentWeight - targetWeight) * 0.9f;
        synapse->setWeight(normalizedWeight);
    }
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::configure(const std::unordered_map<std::string, float>& params) {
    if (params.find("learning_rate") != params.end()) {
        setLearningRate(params.at("learning_rate"));
    }
    if (params.find("normalization_factor") != params.end()) {
        pImpl->normalizationFactor = std::clamp(params.at("normalization_factor"), 0.0f, 1.0f);
    }
    if (params.find("target_correlation") != params.end()) {
        pImpl->targetCorrelation = std::clamp(params.at("target_correlation"), 0.0f, 1.0f);
    }
}

float HebbianRule::calculateFiringCorrelation(
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes,
    TimestepDuration dt) const {
    
    // Calculate asymmetric correlation (post after pre is more correlated)
    float totalWeight = 0.0f;
    float totalCount = 0.0f;
    
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float timeDiff = static_cast<float>(post - pre);  // Δt in ms
            
            if (timeDiff > 0 && timeDiff < 50.0f) {  // 0-50ms window, post after pre
                // More correlated if post fires soon after pre
                float correlation = std::exp(-timeDiff / 10.0f);  // 10ms decay
                totalWeight += correlation;
                totalCount += 1.0f;
            } else if (timeDiff > -10.0f && timeDiff < 0) {  // -10 to 0ms, pre after post
                // Less correlated or anti-correlated if pre fires after post
                float correlation = std::exp(timeDiff / 5.0f);  // 5ms decay for anti-correlation
                totalWeight -= correlation;
                totalCount += 1.0f;
            }
        }
    }
    
    return totalCount > 0.0f ? totalWeight / totalCount : 0.0f;
}

} // namespace nlm
