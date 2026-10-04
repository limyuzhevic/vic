#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
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
    /*
     * Enhanced STDP implementation with triplet interactions
     * 
     * Mathematical formulation:
     * 
     * PAIR INTERACTIONS (as in original):
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+)
     * 
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = -A- * exp(Δt / τ-)
     * 
     * TRIPLET INTERACTIONS (additional):
     * Pre-before-pre-before-post (post-2 spike): Enhanced potentiation
     * Post-before-post-before-pre (pre-2 spike): Enhanced depression
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor dynamics and calcium signaling
     *   - Captures "climbing fiber" influence from cerebellum
     *   - Accounts for synaptic metaplasticity
     *   
     * Advanced features:
     *   - Triplet interactions for more realistic spike timing
     *   - Synaptic eligibility traces for reward modulation
     *   - Activity-dependent bounding to prevent runaway changes
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // STEP 1: PAIR INTERACTIONS (original STDP)
    // Calculate pairwise spike timing differences
    std::vector<float> pairPotentiation;
    std::vector<float> pairDepression;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
                pairPotentiation.push_back(delta);
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                totalDelta += delta;
                pairDepression.push_back(delta);
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // STEP 2: TRIPLET INTERACTIONS (advanced feature)
    // Calculate triplet spike timing for more realistic dynamics
    
    std::vector<Timestamp> preBeforePreBeforePost;  // pre1, pre2, post (post-2)
    std::vector<Timestamp> postBeforePostBeforePre; // post1, post2, pre (pre-2)
    
    if (preSpikes.size() >= 2 && postSpikes.size() >= 1) {
        // Look for pre-pre-post patterns
        for (size_t i = 0; i < preSpikes.size() - 1; ++i) {
            for (size_t j = 0; j < postSpikes.size(); ++j) {
                float dt_pre1_pre2 = static_cast<float>(preSpikes[i + 1] - preSpikes[i]);
                float dt_pre2_post = static_cast<float>(postSpikes[j] - preSpikes[i + 1]);
                
                if (dt_pre1_pre2 > 0 && dt_pre2_post > 0 && dt_pre1_pre2 < 50.0f && dt_pre2_post < 50.0f) {
                    // pre1 -> pre2 -> post pattern (post-2 spike)
                    float tripletDelta = pImpl->ltpWeight * 0.5f * std::exp(-dt_pre1_pre2 / tau) * std::exp(-dt_pre2_post / tau);
                    totalDelta += tripletDelta;
                }
            }
        }
    }
    
    if (postSpikes.size() >= 2 && preSpikes.size() >= 1) {
        // Look for post-post-pre patterns
        for (size_t i = 0; i < postSpikes.size() - 1; ++i) {
            for (size_t j = 0; j < preSpikes.size(); ++j) {
                float dt_post1_post2 = static_cast<float>(postSpikes[i + 1] - postSpikes[i]);
                float dt_post2_pre = static_cast<float>(preSpikes[j] - postSpikes[i + 1]);
                
                if (dt_post1_post2 > 0 && dt_post2_pre > 0 && dt_post1_post2 < 50.0f && dt_post2_pre < 50.0f) {
                    // post1 -> post2 -> pre pattern (pre-2 spike)
                    float tripletDelta = -pImpl->ltdWeight * 0.5f * std::exp(-dt_post1_post2 / tau) * std::exp(-dt_post2_pre / tau);
                    totalDelta += tripletDelta;
                }
            }
        }
    }
    
    // STEP 3: ACTIVITY-DEPENDENT BOUNDING
    // Prevent runaway synaptic changes based on recent activity
    float activityLevel = std::sqrt(static_cast<float>(preSpikes.size() + postSpikes.size()));
    float activityFactor = std::min(1.0f, activityLevel / 10.0f);  // Normalize activity
    
    // Apply activity-dependent scaling to weight changes
    float activityScale = 1.0f - activityFactor * 0.5f;  // Reduce changes during high activity
    totalDelta *= activityScale;
    
    // STEP 4: APPLY WEIGHT CHANGE WITH ADVANCED MECHANISMS
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Check for metaplasticity: recent weight changes affect learning rate
        float recentWeightChange = synapse->getEligibilityTrace();
        if (std::abs(recentWeightChange) > 0.1f) {
            // Metaplastic reduction: high recent changes reduce future plasticity
            float metaplasticFactor = 0.5f * std::exp(-std::abs(recentWeightChange));
            totalDelta *= metaplasticFactor;
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning with stronger trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * 0.8f);  // Stronger trace for STDP
        
        // Record spike timing for future triplet calculations
        if (!preSpikes.empty()) {
            synapse->recordPreSpike(preSpikes.back());
        }
        if (!postSpikes.empty()) {
            synapse->recordPostSpike(postSpikes.back());
        }
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
