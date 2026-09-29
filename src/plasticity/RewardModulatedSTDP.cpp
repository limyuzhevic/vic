#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float dopamineScale;  // Dopamine modulation factor (0.0 to 1.0)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             dopamineScale(1.0f), minWeight(-1.0f), maxWeight(1.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    // Input validation
    if (synapse == nullptr) {
        NLM_LOG_WARN("RewardModulatedSTDP::update: synapse is null");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        // No spikes to process - this is normal, not an error
        return;
    }
    
    if (dt <= 0.0f) {
        NLM_LOG_WARN("RewardModulatedSTDP::update: invalid timestep duration: " + std::to_string(dt));
        return;
    }
    
    try {
        // Validate parameters
        if (pImpl->timeConstant <= 0.0f) {
            NLM_LOG_WARN("RewardModulatedSTDP::update: invalid time constant: " + std::to_string(pImpl->timeConstant));
            return;
        }
        
        if (pImpl->dopamineScale < 0.0f) {
            NLM_LOG_WARN("RewardModulatedSTDP::update: negative dopamine scale: " + std::to_string(pImpl->dopamineScale));
            return;
        }
        
        if (pImpl->minWeight >= pImpl->maxWeight) {
            NLM_LOG_WARN("RewardModulatedSTDP::update: invalid weight bounds: min=" + std::to_string(pImpl->minWeight) + 
                        ", max=" + std::to_string(pImpl->maxWeight));
            return;
        }
        
        // Get predicted reward from synapse eligibility trace
        // In a real implementation, this would come from neuromodulation system
        float predictedReward = synapse->getPredictedReward();
        float dopamineSignal = pImpl->dopamineScale * std::max(0.0f, predictedReward);
        
        float totalDelta = 0.0f;
        float tau = pImpl->timeConstant;
        
        // Process all spike pairs with error handling
        for (size_t i = 0; i < preSpikes.size(); ++i) {
            Timestamp preTime = preSpikes[i];
            
            for (size_t j = 0; j < postSpikes.size(); ++j) {
                Timestamp postTime = postSpikes[j];
                float dt = static_cast<float>(postTime - preTime);  // Δt in ms
                
                if (dt > 0) {
                    // Pre before post: POTENTIATION
                    float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                    totalDelta += delta;
                } else if (dt < 0) {
                    // Post before pre: DEPRESSION
                    float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative
                    totalDelta += delta;
                }
                // dt == 0: no change (simultaneous spikes)
            }
        }
        
        // Apply dopamine modulation
        totalDelta *= dopamineSignal;
        
        // Apply weight change with bounds and eligibility trace update
        if (std::abs(totalDelta) > 1e-6f) {
            try {
                // Scale by synaptic efficacy if available
                float efficacy = synapse->getEfficacy();
                if (efficacy < 0.0f || efficacy > 2.0f) {
                    NLM_LOG_WARN("RewardModulatedSTDP::update: invalid efficacy value: " + std::to_string(efficacy));
                    efficacy = std::clamp(efficacy, 0.0f, 2.0f);
                }
                
                totalDelta *= efficacy;
                
                // Apply weight change
                synapse->addToWeight(totalDelta);
                
                // Update eligibility trace for future reward modulation
                float currentTrace = synapse->getEligibilityTrace();
                synapse->setEligibilityTrace(currentTrace + totalDelta * dopamineSignal);
                
                // Update predicted reward based on weight change
                synapse->updatePredictedReward(totalDelta);
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("RewardModulatedSTDP::update: error applying weight change: " + std::string(e.what()));
            }
        }
        
        // Log activity for debugging
        if (std::abs(totalDelta) > 1e-6f) {
            NLM_LOG_DEBUG("RewardModulatedSTDP::update: predicted_reward=" + std::to_string(predictedReward) +
                         ", dopamine_signal=" + std::to_string(dopamineSignal) +
                         ", total_delta=" + std::to_string(totalDelta) +
                         ", final_weight=" + std::to_string(synapse->getWeight()));
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("RewardModulatedSTDP::update: error processing spike pairs: " + std::string(e.what()));
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (synapse == nullptr) {
        NLM_LOG_WARN("RewardModulatedSTDP::applyWeightChange: synapse is null");
        return;
    }
    
    if (std::abs(delta) < 1e-6f) {
        // Very small weight change - skip to avoid numerical issues
        return;
    }
    
    try {
        // Clamp weight to bounds
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        
        // Check for weight change
        if (std::abs(newWeight - synapse->getWeight()) > 1e-6f) {
            synapse->setWeight(newWeight);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("RewardModulatedSTDP::applyWeightChange: error applying weight change: " + std::string(e.what()));
    }
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setLTPWeight(float weight) {
    if (weight < 0.0f) {
        NLM_LOG_WARN("RewardModulatedSTDP::setLTPWeight: negative weight value will be clamped: " + std::to_string(weight));
    }
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void RewardModulatedSTDP::setLTDWeight(float weight) {
    if (weight < 0.0f) {
        NLM_LOG_WARN("RewardModulatedSTDP::setLTDWeight: negative weight value will be clamped: " + std::to_string(weight));
    }
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void RewardModulatedSTDP::setTimeConstant(float tau) {
    if (tau <= 0.0f) {
        NLM_LOG_WARN("RewardModulatedSTDP::setTimeConstant: invalid time constant: " + std::to_string(tau));
        return;
    }
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float RewardModulatedSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void RewardModulatedSTDP::setDopamineScale(float scale) {
    if (scale < 0.0f) {
        NLM_LOG_WARN("RewardModulatedSTDP::setDopamineScale: negative scale will be clamped: " + std::to_string(scale));
    }
    pImpl->dopamineScale = std::clamp(scale, 0.0f, 2.0f);
}

float RewardModulatedSTDP::getDopamineScale() const {
    return pImpl->dopamineScale;
}

void RewardModulatedSTDP::configure(float ltpWeight, float ltdWeight, float tau, float dopamineScale) {
    if (std::abs(ltpWeight - pImpl->ltpWeight) < 1e-6f && 
        std::abs(ltdWeight - pImpl->ltdWeight) < 1e-6f &&
        std::abs(tau - pImpl->timeConstant) < 1e-6f &&
        std::abs(dopamineScale - pImpl->dopamineScale) < 1e-6f) {
        return; // No change needed
    }
    
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    setDopamineScale(dopamineScale);
    
    NLM_LOG_INFO("RewardModulatedSTDP configured: ltpWeight=" + std::to_string(pImpl->ltpWeight) +
                ", ltdWeight=" + std::to_string(pImpl->ltdWeight) +
                ", timeConstant=" + std::to_string(pImpl->timeConstant) +
                ", dopamineScale=" + std::to_string(pImpl->dopamineScale));
}

} // namespace nlm