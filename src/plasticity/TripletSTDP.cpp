#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct TripletSTDP::Impl {
    // Triplet time constants
    float fastTimeConstant;  // τ1 for fast component (ms)
    float slowTimeConstant;  // τ2 for slow component (ms)
    
    // Triplet weights
    float prePostWeight;     // w1: potentiation weight (pre-post)
    float postPreWeight;     // w2: depression weight (post-pre)
    float crossWeight;       // w3: cross-term weight (pre-post-post etc.)
    
    // Baseline STDP parameters
    float baselineLTP;       // A+ for standard STDP
    float baselineLTD;       // A- for standard STDP
    float baselineTau;       // Standard STDP time constant
    
    // Triplet contributions cache
    std::unordered_map<uint64_t, float> prePostCache;
    std::unordered_map<uint64_t, float> postPreCache;
    std::unordered_map<uint64_t, float> crossCache;
    
    Impl() : fastTimeConstant(20.0f), slowTimeConstant(200.0f),
             prePostWeight(0.005f), postPreWeight(0.005f), crossWeight(0.002f),
             baselineLTP(0.01f), baselineLTD(0.012f), baselineTau(20.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {
    tripletEnabled = true;
}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    if (tripletEnabled) {
        // Calculate triplet contributions
        totalDelta += calculateTripletContributions(synapse, preSpikes, postSpikes, dt);
    }
    
    // Add standard STDP contribution
    totalDelta += calculateStandardSTDP(synapse, preSpikes, postSpikes, dt);
    
    // Apply weight change
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void TripletSTDP::setTripletTimeConstant(float tau1, float tau2) {
    pImpl->fastTimeConstant = std::clamp(tau1, 5.0f, 50.0f);
    pImpl->slowTimeConstant = std::clamp(tau2, 50.0f, 500.0f);
}

void TripletSTDP::setTripletWeights(float w1, float w2, float w3) {
    pImpl->prePostWeight = std::max(0.0f, w1);
    pImpl->postPreWeight = std::max(0.0f, w2);
    pImpl->crossWeight = std::max(0.0f, w3);
}

float TripletSTDP::getTripletTimeConstant1() const {
    return pImpl->fastTimeConstant;
}

float TripletSTDP::getTripletTimeConstant2() const {
    return pImpl->slowTimeConstant;
}

float TripletSTDP::getTripletWeight1() const {
    return pImpl->prePostWeight;
}

float TripletSTDP::getTripletWeight2() const {
    return pImpl->postPreWeight;
}

float TripletSTDP::getTripletWeight3() const {
    return pImpl->crossWeight;
}

void TripletSTDP::enableTripletInteractions(bool enable) {
    tripletEnabled = enable;
}

bool TripletSTDP::areTripletInteractionsEnabled() const {
    return tripletEnabled;
}

float TripletSTDP::getTripletContribution() const {
    // Calculate current triplet contribution (simplified)
    return pImpl->prePostWeight + pImpl->postPreWeight + pImpl->crossWeight;
}

// Helper methods
float TripletSTDP::calculateTripletContributions(Synapse* synapse,
                                                 const std::vector<Timestamp>& preSpikes,
                                                 const std::vector<Timestamp>& postSpikes,
                                                 TimestepDuration dt) {
    float totalTripletDelta = 0.0f;
    
    // Pre-post-post contributions
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            for (size_t k = j; k < postSpikes.size(); ++k) {
                float dt1 = static_cast<float>(postSpikes[j] - preSpikes[i]);  // pre-post
                float dt2 = static_cast<float>(postSpikes[k] - postSpikes[j]); // post-post
                
                if (dt1 > 0 && dt2 > 0) {
                    // Pre-post-post potentiation
                    float contribution = pImpl->prePostWeight * 
                                         std::exp(-dt1 / pImpl->fastTimeConstant) *
                                         std::exp(-dt2 / pImpl->slowTimeConstant);
                    totalTripletDelta += contribution;
                }
            }
        }
    }
    
    // Post-post-pre contributions
    for (size_t i = 0; i < postSpikes.size(); ++i) {
        for (size_t j = i; j < postSpikes.size(); ++j) {
            for (size_t k = 0; k < preSpikes.size(); ++k) {
                float dt1 = static_cast<float>(postSpikes[i] - preSpikes[k]);  // post-pre
                float dt2 = static_cast<float>(postSpikes[j] - postSpikes[i]); // post-post
                
                if (dt1 < 0 && dt2 > 0) {
                    // Post-post-pre depression
                    float contribution = pImpl->postPreWeight * 
                                         std::exp(dt1 / pImpl->fastTimeConstant) *
                                         std::exp(-dt2 / pImpl->slowTimeConstant);
                    totalTripletDelta += contribution;
                }
            }
        }
    }
    
    // Cross-term (pre-post-post and post-post-pre)
    totalTripletDelta += calculateCrossTerms(preSpikes, postSpikes);
    
    return totalTripletDelta;
}

float TripletSTDP::calculateStandardSTDP(Synapse* synapse,
                                        const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        TimestepDuration dt) {
    float totalDelta = 0.0f;
    float tau = pImpl->baselineTau;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dtMs = static_cast<float>(postTime - preTime);
            
            if (dtMs > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->baselineLTP * std::exp(-dtMs / tau);
                totalDelta += delta;
            } else if (dtMs < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->baselineLTD * std::exp(dtMs / tau);
                totalDelta += delta;
            }
        }
    }
    
    return totalDelta;
}

float TripletSTDP::calculateCrossTerms(const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes) {
    float crossDelta = 0.0f;
    
    // Simple cross-term approximation
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < 50.0f) {  // Within 50ms
                crossDelta += pImpl->crossWeight * std::exp(-std::abs(dt) / 30.0f);
            }
        }
    }
    
    return crossDelta;
}

} // namespace nlm
