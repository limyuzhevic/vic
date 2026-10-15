// TripletSTDP.cpp implementation

#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float prePostPostWeight;      // Weight for pre-post-post triplet interaction
    float postPrePreWeight;       // Weight for post-pre-pre triplet interaction
    float facilitationStrength;   // Strength of facilitation trace
    float depressionStrength;     // Strength of depression trace
    bool enableSynapticResources; // Enable synaptic resource management
    float resourceLimit;          // Maximum synaptic resource
    float resourceRecovery;       // Resource recovery rate
    float traceDecay;             // General trace decay rate
    bool enableFacilitationTrace; // Enable facilitation trace
    bool enableDepressionTrace;   // Enable depression trace
    bool enableEligibilityTrace;  // Enable eligibility trace for triplet
    
    // Synaptic state
    float synapticResources;      // Current synaptic resource availability
    float facilitationTrace;      // Facilitation trace variable
    float depressionTrace;        // Depression trace variable
    float eligibilityTrace;        // Eligibility trace for triplet learning
    
    Impl() : prePostPostWeight(0.01f), postPrePreWeight(0.012f),
             facilitationStrength(1.0f), depressionStrength(1.0f),
             enableSynapticResources(true), resourceLimit(1.0f),
             resourceRecovery(0.1f), traceDecay(0.95f),
             enableFacilitationTrace(true), enableDepressionTrace(true),
             enableEligibilityTrace(true),
             synapticResources(1.0f), facilitationTrace(0.0f),
             depressionTrace(0.0f), eligibilityTrace(0.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    /*
     * Triplet STDP implementation with pre-post-post and post-pre-pre interactions
     * 
     * Mathematical formulation:
     * 
     * 1. Facilitation trace (pre-post-post):
     *    f(t) = f(t-1) * exp(-t/τ_f) + Σ δ(t - t_post) * g_pre(t_post)
     *    
     * 2. Depression trace (post-pre-pre):
     *    d(t) = d(t-1) * exp(-t/τ_d) + Σ δ(t - t_pre) * h_post(t_pre)
     *    
     * 3. Synaptic resource availability:
     *    R(t) = R(t-1) * (1 - recovery_rate) + min(R(t), resource_limit)
     *    
     * 4. Triplet interactions:
     *    Δw = R(t) * [f(t) * A_pre_post_post + d(t) * A_post_pre_pre]
     *    
     *    Where:
     *      f(t) = facilitation trace
     *      d(t) = depression trace
     *      R(t) = synaptic resource availability
     *      A_pre_post_post = prePostPostWeight
     *      A_post_pre_pre = postPrePreWeight
     *    
     * Biological inspiration:
     *   - Pre-post-post facilitation: increases synaptic strength
     *   - Post-pre-pre depression: decreases synaptic strength
     *   - Synaptic resources: models vesicle availability
     *   - Uses NMDA-dependent triplet detection
     *   - Mimics mammalian hippocampal triplet STDP
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update synaptic resources
    updateSynapticResources(dt);
    
    // Update facilitation and depression traces
    updateTraceVariables(preSpikes, postSpikes, dt);
    
    // Calculate triplet interactions
    float totalDelta = calculateTripletInteractions(preSpikes, postSpikes, dt);
    
    // Apply eligibility trace modulation if enabled
    if (pImpl->enableEligibilityTrace) {
        totalDelta *= pImpl->eligibilityTrace;
    }
    
    // Apply weight change with synaptic resource constraint
    float maxDelta = pImpl->synapticResources * 0.1f;  // Limit based on resources
    float clampedDelta = std::clamp(totalDelta, -maxDelta, maxDelta);
    
    // Apply weight change
    applyWeightChange(synapse, clampedDelta);
    
    // Update synaptic resources based on weight change
    updateSynapticResourcesForWeightChange(clampedDelta);
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f); // Standard bounds
    synapse->setWeight(newWeight);
}

void TripletSTDP::updateSynapticResources(float dt) {
    // Update synaptic resource availability with recovery
    pImpl->synapticResources = pImpl->synapticResources * (1.0f - pImpl->resourceRecovery * dt / 1000.0f);
    pImpl->synapticResources = std::min(pImpl->synapticResources, pImpl->resourceLimit);
}

void TripletSTDP::updateTraceVariables(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
    // Update facilitation and depression traces based on spike timing
    
    // Update facilitation trace (accumulates with post-post-pre patterns)
    if (pImpl->enableFacilitationTrace) {
        for (Timestamp postTime : postSpikes) {
            for (Timestamp preTime : preSpikes) {
                float dt_val = static_cast<float>(postTime - preTime);
                if (dt_val > 0 && dt_val < 100.0f) {  // Pre before post
                    pImpl->facilitationTrace = pImpl->facilitationTrace * std::exp(-dt_val / (pImpl->traceDecay * 50.0f)) + 1.0f;
                }
            }
        }
    }
    
    // Update depression trace (accumulates with pre-post-post patterns)
    if (pImpl->enableDepressionTrace) {
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt_val = static_cast<float>(postTime - preTime);
                if (dt_val < 0 && dt_val > -100.0f) {  // Post before pre
                    pImpl->depressionTrace = pImpl->depressionTrace * std::exp(dt_val / (pImpl->traceDecay * 50.0f)) + 1.0f;
                }
            }
        }
    }
    
    // Decay traces
    pImpl->facilitationTrace *= pImpl->traceDecay;
    pImpl->depressionTrace *= pImpl->traceDecay;
}

float TripletSTDP::calculateTripletInteractions(const std::vector<Timestamp>& preSpikes,
                                                const std::vector<Timestamp>& postSpikes,
                                                TimestepDuration dt) {
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_val = static_cast<float>(postTime - preTime);
            
            // Pre-post-post facilitation (requires additional post spike)
            if (dt_val > 0 && dt_val < 100.0f) {
                // Look for another post spike shortly after
                for (Timestamp laterPost : postSpikes) {
                    if (laterPost > postTime && (laterPost - postTime) < 20.0f) {
                        // Pre-post-post triplet
                        float tripletEffect = pImpl->facilitationTrace * pImpl->prePostPostWeight * pImpl->facilitationStrength;
                        totalDelta += tripletEffect;
                        break;
                    }
                }
            }
            
            // Post-pre-pre depression (requires additional pre spike)
            else if (dt_val < 0 && dt_val > -100.0f) {
                // Look for another pre spike shortly after
                for (Timestamp laterPre : preSpikes) {
                    if (laterPre > preTime && (laterPre - preTime) < 20.0f) {
                        // Post-pre-pre triplet
                        float tripletEffect = pImpl->depressionTrace * pImpl->postPrePreWeight * pImpl->depressionStrength;
                        totalDelta -= tripletEffect;  // Depression subtracts weight
                        break;
                    }
                }
            }
        }
    }
    
    return totalDelta;
}

void TripletSTDP::updateSynapticResourcesForWeightChange(float delta) {
    // Adjust synaptic resources based on weight change
    if (delta > 0) {
        // Potentiation consumes resources
        pImpl->synapticResources -= std::abs(delta) * 0.1f;
    } else {
        // Depression frees resources (up to limit)
        pImpl->synapticResources = std::min(pImpl->synapticResources - std::abs(delta) * 0.05f, pImpl->resourceLimit);
    }
    pImpl->synapticResources = std::clamp(pImpl->synapticResources, 0.0f, pImpl->resourceLimit);
}

void TripletSTDP::setPrePostPostWeight(float weight) {
    pImpl->prePostPostWeight = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getPrePostPostWeight() const {
    return pImpl->prePostPostWeight;
}

void TripletSTDP::setPostPrePreWeight(float weight) {
    pImpl->postPrePreWeight = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getPostPrePreWeight() const {
    return pImpl->postPrePreWeight;
}

void TripletSTDP::setFacilitationStrength(float strength) {
    pImpl->facilitationStrength = std::clamp(strength, 0.0f, 10.0f);
}

float TripletSTDP::getFacilitationStrength() const {
    return pImpl->facilitationStrength;
}

void TripletSTDP::setDepressionStrength(float strength) {
    pImpl->depressionStrength = std::clamp(strength, 0.0f, 10.0f);
}

float TripletSTDP::getDepressionStrength() const {
    return pImpl->depressionStrength;
}

void TripletSTDP::enableSynapticResources(bool enable) {
    pImpl->enableSynapticResources = enable;
}

void TripletSTDP::setResourceLimit(float limit) {
    pImpl->resourceLimit = std::clamp(limit, 0.1f, 10.0f);
}

void TripletSTDP::setResourceRecovery(float recovery) {
    pImpl->resourceRecovery = std::clamp(recovery, 0.0f, 1.0f);
}

bool TripletSTDP::isSynapticResourcesEnabled() const {
    return pImpl->enableSynapticResources;
}

float TripletSTDP::getResourceLimit() const {
    return pImpl->resourceLimit;
}

float TripletSTDP::getResourceRecovery() const {
    return pImpl->resourceRecovery;
}

void TripletSTDP::enableFacilitationTrace(bool enable) {
    pImpl->enableFacilitationTrace = enable;
}

void TripletSTDP::enableDepressionTrace(bool enable) {
    pImpl->enableDepressionTrace = enable;
}

void TripletSTDP::setTraceDecay(float decay) {
    pImpl->traceDecay = std::clamp(decay, 0.5f, 0.999f);
}

bool TripletSTDP::isFacilitationTraceEnabled() const {
    return pImpl->enableFacilitationTrace;
}

bool TripletSTDP::isDepressionTraceEnabled() const {
    return pImpl->enableDepressionTrace;
}

float TripletSTDP::getTraceDecay() const {
    return pImpl->traceDecay;
}

void TripletSTDP::setEligibilityTraceEnabled(bool enable) {
    pImpl->enableEligibilityTrace = enable;
}

bool TripletSTDP::isEligibilityTraceEnabled() const {
    return pImpl->enableEligibilityTrace;
}

} // namespace nlm