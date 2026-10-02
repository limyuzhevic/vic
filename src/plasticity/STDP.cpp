#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

// Forward declaration for helper functions
float calculateTripletEffect(const std::vector<Timestamp>& preSpikes, 
                            const std::vector<Timestamp>& postSpikes,
                            float ltpWeight, float ltdWeight, float timeConstant,
                            float tripletTimeWindow);

struct STDP::Impl {
    float ltpWeight;              // A+ for potentiation
    float ltdWeight;              // A- for depression
    float timeConstant;           // Tau for exponential window (ms)
    float minWeight;              // Minimum synaptic weight
    float maxWeight;              // Maximum synaptic weight
    
    // Advanced STDP features
    float ltpWeightNorm;          // Normalized LTP weight
    float ltdWeightNorm;          // Normalized LTD weight
    float metaplasticity;         // Metaplasticity factor (0.0-1.0)
    float baselineLTP;            // Baseline LTP rate
    float baselineLTD;            // Baseline LTD rate
    float activityHistory;         // Recent activity for adaptation
    float weightHistory;          // Weight history for normalization
    
    // Triplet STDP parameters
    float tripletLTP;             // Triplet LTP coefficient
    float tripletLTD;             // Triplet LTD coefficient
    float tripletTimeWindow;      // Triplet time window (ms)
    float traceDecay;             // Eligibility trace decay rate
    
    // Balance mechanisms
    float balanceFactor;           // LTP/LTD balance factor (0.5 = equal)
    size_t recentEvents;           // Number of recent spike events
    float ltpStabilization;        // LTP stabilization factor
    float ltdStabilization;        // LTD stabilization factor
    
    // Normalization
    float weightNormalization;     // Weight normalization factor
    size_t normalizationWindow;   // Window for weight normalization
    float maxWeightChange;        // Maximum allowed weight change per step
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             ltpWeightNorm(0.01f), ltdWeightNorm(0.012f), 
             metaplasticity(0.0f), baselineLTP(0.01f), baselineLTD(0.012f),
             activityHistory(0.0f), weightHistory(0.0f),
             tripletLTP(0.005f), tripletLTD(0.003f),
             tripletTimeWindow(50.0f), traceDecay(0.95f),
             balanceFactor(0.5f), recentEvents(0), ltpStabilization(1.0f),
             ltdStabilization(1.0f), weightNormalization(1.0f),
             normalizationWindow(1000), maxWeightChange(0.01f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate basic pairwise STDP effect
    float totalDelta = calculatePairwiseSTDP(preSpikes, postSpikes, dt);
    
    // Calculate triplet STDP effect (more realistic biological model)
    float tripletDelta = calculateTripletEffect(preSpikes, postSpikes, 
                                               pImpl->ltpWeight, pImpl->ltdWeight,
                                               pImpl->timeConstant, pImpl->tripletTimeWindow);
    
    // Combine effects
    float combinedDelta = totalDelta + tripletDelta;
    
    // Apply normalization and adaptation
    combinedDelta = normalizeWeightChange(combinedDelta, synapse);
    combinedDelta = adaptLearningRates(combinedDelta, synapse, preSpikes, postSpikes);
    
    // Apply balance mechanisms
    combinedDelta = applyBalanceMechanisms(combinedDelta, synapse, preSpikes.size(), postSpikes.size());
    
    // Update metaplasticity
    updateMetaplasticity(preSpikes, postSpikes);
    
    // Update activity and weight history
    pImpl->activityHistory = static_cast<float>(preSpikes.size() + postSpikes.size()) / 2.0f;
    pImpl->weightHistory = synapse->getWeight();
    
    // Apply final weight change with bounds
    applyWeightChangeWithHistory(synapse, combinedDelta);
}

float STDP::calculatePairwiseSTDP(const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) const {
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Adaptive learning rates based on recent activity
    float adaptiveLTP = pImpl->ltpWeight * (1.0f + pImpl->metaplasticity * 0.5f);
    float adaptiveLTD = pImpl->ltdWeight * (1.0f + pImpl->metaplasticity * 0.3f);
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION with triplet interaction
                float delta = adaptiveLTP * std::exp(-dt / tau);
                // Add triplet contribution for long time windows
                if (dt > pImpl->timeConstant * 2.0f) {
                    delta *= 1.0f + pImpl->tripletLTP * std::exp(-(dt - pImpl->timeConstant * 2.0f) / (pImpl->timeConstant * 2.0f));
                }
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -adaptiveLTD * std::exp(dt / tau);  // dt is negative, so this subtracts
                // Add triplet contribution for depression
                if (dt < -pImpl->timeConstant * 2.0f) {
                    delta *= 1.0f + pImpl->tripletLTD * std::exp((dt + pImpl->timeConstant * 2.0f) / (pImpl->timeConstant * 2.0f));
                }
                totalDelta += delta;
            }
        }
    }
    
    return totalDelta;
}

float STDP::calculateTripletEffect(const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 float ltpWeight, float ltdWeight, float timeConstant,
                                 float tripletTimeWindow) {
    float tripletEffect = 0.0f;
    
    // Simplified triplet STDP: considers interactions between consecutive spikes
    // This models the biological phenomenon where triplets of spikes have different effects
    
    // For each pre-post pair, check for triplet interactions
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            float dt = static_cast<float>(postSpikes[j] - preSpikes[i]);
            
            if (dt > 0) {
                // Look for preceding pre-synaptic spike for LTP triplet
                if (i > 0) {
                    float dt2 = static_cast<float>(preSpikes[i] - preSpikes[i-1]);
                    if (dt2 > 0 && dt2 < tripletTimeWindow) {
                        tripletEffect += ltpWeight * 0.5f * std::exp(-dt / timeConstant) * std::exp(-dt2 / timeConstant);
                    }
                }
                
                // Look for following post-synaptic spike for LTP triplet
                if (j + 1 < postSpikes.size()) {
                    float dt2 = static_cast<float>(postSpikes[j+1] - postSpikes[j]);
                    if (dt2 > 0 && dt2 < tripletTimeWindow) {
                        tripletEffect += ltpWeight * 0.3f * std::exp(-dt / timeConstant) * std::exp(-dt2 / timeConstant);
                    }
                }
            } else if (dt < 0) {
                // Look for preceding post-synaptic spike for LTD triplet
                if (j > 0) {
                    float dt2 = static_cast<float>(postSpikes[j] - postSpikes[j-1]);
                    if (dt2 > 0 && dt2 < tripletTimeWindow) {
                        tripletEffect += -ltdWeight * 0.5f * std::exp(dt / timeConstant) * std::exp(-dt2 / timeConstant);
                    }
                }
                
                // Look for following pre-synaptic spike for LTD triplet
                if (i + 1 < preSpikes.size()) {
                    float dt2 = static_cast<float>(preSpikes[i+1] - preSpikes[i]);
                    if (dt2 > 0 && dt2 < tripletTimeWindow) {
                        tripletEffect += -ltdWeight * 0.3f * std::exp(dt / timeConstant) * std::exp(-dt2 / timeConstant);
                    }
                }
            }
        }
    }
    
    return tripletEffect;
}

float STDP::normalizeWeightChange(float delta, Synapse* synapse) {
    // Normalize weight change based on current weight
    if (!synapse) return delta;
    
    float currentWeight = synapse->getWeight();
    
    // Adaptive normalization based on weight proximity to bounds
    float normFactor = pImpl->weightNormalization;
    if (std::abs(currentWeight - pImpl->maxWeight) < 0.1f) {
        normFactor *= 0.5f;  // Reduce changes near max weight
    } else if (std::abs(currentWeight - pImpl->minWeight) < 0.1f) {
        normFactor *= 0.5f;  // Reduce changes near min weight
    }
    
    // Apply normalization
    float normalizedDelta = delta * normFactor;
    
    // Apply maximum allowed change
    if (std::abs(normalizedDelta) > pImpl->maxWeightChange) {
        normalizedDelta = std::copysign(pImpl->maxWeightChange, normalizedDelta);
    }
    
    return normalizedDelta;
}

float STDP::adaptLearningRates(float delta, Synapse* synapse, 
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes) {
    // Adapt learning rates based on recent activity and metaplasticity
    float adaptationFactor = 1.0f;
    
    // Increase learning rates if there's high recent activity
    float recentActivity = static_cast<float>(preSpikes.size() + postSpikes.size()) / 2.0f;
    if (recentActivity > pImpl->activityHistory * 2.0f) {
        adaptationFactor *= 1.2f;
    } else if (recentActivity < pImpl->activityHistory * 0.5f) {
        adaptationFactor *= 0.8f;
    }
    
    // Apply metaplasticity
    adaptationFactor *= (1.0f + pImpl->metaplasticity);
    
    // Apply balance factors
    if (delta > 0) {  // LTP
        adaptationFactor *= pImpl->ltpStabilization;
    } else {  // LTD
        adaptationFactor *= pImpl->ltdStabilization;
    }
    
    return delta * adaptationFactor;
}

float STDP::applyBalanceMechanisms(float delta, Synapse* synapse, 
                                  size_t preCount, size_t postCount) {
    // Apply LTP/LTD balance mechanisms
    float balanceFactor = pImpl->balanceFactor;
    
    // Calculate LTP/LTD ratio based on spike counts
    float ltpRatio = static_cast<float>(preCount) / static_cast<float>(std::max(size_t(1), preCount + postCount));
    float ltdRatio = static_cast<float>(postCount) / static_cast<float>(std::max(size_t(1), preCount + postCount));
    
    // Apply balance correction
    if (delta > 0 && ltdRatio > ltpRatio * 1.5f) {
        // Too much LTD relative to LTP, reduce LTP contribution
        delta *= balanceFactor * 0.8f;
    } else if (delta < 0 && ltpRatio > ltdRatio * 1.5f) {
        // Too much LTP relative to LTD, reduce LTD contribution
        delta *= balanceFactor * 0.8f;
    }
    
    // Stabilize extreme values
    pImpl->recentEvents++;
    if (pImpl->recentEvents >= pImpl->normalizationWindow) {
        float activityRatio = pImpl->activityHistory / pImpl->weightHistory;
        if (activityRatio > 1.5f) {
            pImpl->ltpStabilization = std::max(0.5f, pImpl->ltpStabilization - 0.01f);
            pImpl->ltdStabilization = std::min(2.0f, pImpl->ltdStabilization + 0.01f);
        } else if (activityRatio < 0.5f) {
            pImpl->ltpStabilization = std::min(2.0f, pImpl->ltpStabilization + 0.01f);
            pImpl->ltdStabilization = std::max(0.5f, pImpl->ltdStabilization - 0.01f);
        }
        pImpl->recentEvents = 0;
    }
    
    return delta;
}

void STDP::updateMetaplasticity(const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes) {
    // Update metaplasticity based on recent spike patterns
    float recentActivity = static_cast<float>(preSpikes.size() + postSpikes.size()) / 2.0f;
    
    // Metaplasticity adapts learning rates based on network stability
    if (recentActivity > pImpl->activityHistory * 2.0f) {
        // High activity: increase metaplasticity to stabilize learning
        pImpl->metaplasticity = std::min(1.0f, pImpl->metaplasticity + 0.01f);
    } else if (recentActivity < pImpl->activityHistory * 0.5f) {
        // Low activity: decrease metaplasticity to allow more learning
        pImpl->metaplasticity = std::max(0.0f, pImpl->metaplasticity - 0.01f);
    }
    
    // Reset metaplasticity if it's too extreme
    if (pImpl->metaplasticity > 0.8f && recentActivity < pImpl->activityHistory) {
        pImpl->metaplasticity = std::max(0.0f, pImpl->metaplasticity - 0.1f);
    }
}

void STDP::applyWeightChangeWithHistory(Synapse* synapse, float delta) {
    if (!synapse) return;
    
    // Apply weight change with eligibility trace and history
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Clamp weight to bounds
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Update synapse weight
    synapse->setWeight(newWeight);
    
    // Update eligibility trace for reward-modulated learning
    float eligibilityTrace = synapse->getEligibilityTrace();
    eligibilityTrace = eligibilityTrace * pImpl->traceDecay + delta;
    synapse->setEligibilityTrace(eligibilityTrace);
    
    // Update baseline weights based on long-term average
    if (std::abs(delta) > 0.001f) {
        pImpl->baselineLTP = pImpl->ltpWeight * 0.9f + pImpl->ltpWeight * 0.1f;
        pImpl->baselineLTD = pImpl->ltdWeight * 0.9f + pImpl->ltdWeight * 0.1f;
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Original simple implementation for compatibility
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "Advanced STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
    pImpl->ltpWeightNorm = std::clamp(weight * 0.8f, 0.0f, 0.8f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
    pImpl->ltdWeightNorm = std::clamp(weight * 0.8f, 0.0f, 0.8f);
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

void STDP::setTripletLTP(float weight) {
    pImpl->tripletLTP = std::clamp(weight, 0.0f, 0.1f);
}

float STDP::getTripletLTP() const {
    return pImpl->tripletLTP;
}

void STDP::setTripletLTD(float weight) {
    pImpl->tripletLTD = std::clamp(weight, 0.0f, 0.1f);
}

float STDP::getTripletLTD() const {
    return pImpl->tripletLTD;
}

void STDP::setBalanceFactor(float factor) {
    pImpl->balanceFactor = std::clamp(factor, 0.1f, 1.0f);
}

float STDP::getBalanceFactor() const {
    return pImpl->balanceFactor;
}

void STDP::setMetaplasticity(float metaplasticity) {
    pImpl->metaplasticity = std::clamp(metaplasticity, 0.0f, 1.0f);
}

float STDP::getMetaplasticity() const {
    return pImpl->metaplasticity;
}

void STDP::setMaxWeightChange(float maxChange) {
    pImpl->maxWeightChange = std::clamp(maxChange, 0.001f, 0.1f);
}

float STDP::getMaxWeightChange() const {
    return pImpl->maxWeightChange;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau,
                     float tripletLTP, float tripletLTD, float balanceFactor) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    setTripletLTP(tripletLTP);
    setTripletLTD(tripletLTD);
    setBalanceFactor(balanceFactor);
}

} // namespace nlm