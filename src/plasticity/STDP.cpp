#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <numeric>

namespace nlm {

namespace nlm {

// Implementation of separate RewardModulatedSTDP class
struct RewardModulatedSTDP::Impl {
    float learningRate;           // Base learning rate
    float rewardModulationFactor; // Reward modulation factor (0-1)
    float eligibilityTrace;       // Eligibility trace for reward modulation
    float traceDecay;              // Decay rate of eligibility trace
    float minWeight;               // Minimum synaptic weight
    float maxWeight;               // Maximum synaptic weight
    
    // Advanced reward modulation parameters
    float rewardSensitivity;       // Sensitivity to reward signals
    float predictionErrorGain;    // Gain for prediction error signals
    float neuromodulatorBaseLevel; // Base neuromodulator level
    
    Impl() : learningRate(0.01f), rewardModulationFactor(1.0f),
             eligibilityTrace(0.0f), traceDecay(0.01f),
             minWeight(-1.0f), maxWeight(1.0f),
             rewardSensitivity(1.0f), predictionErrorGain(0.5f),
             neuromodulatorBaseLevel(0.1f) {}
};

// Implementation of separate TripletSTDP class
struct TripletSTDP::Impl {
    float prePostWeight;    // Weight for pre-post interactions
    float postPreWeight;    // Weight for post-pre interactions  
    float postPostWeight;   // Weight for post-post interactions
    float timeConstant;     // Time constant for triplet interactions
    float minWeight;        // Minimum synaptic weight
    float maxWeight;        // Maximum synaptic weight
    
    // Triplet interaction parameters
    float tripletOrder;     // Order of triplet interactions
    float spikeHistoryWindow; // Window for spike history
    float interactionStrength; // Strength of triplet interactions
    
    // Neural basis parameters
    float nmdaGain;         // NMDA receptor gain
    float ampaGain;         // AMPA receptor gain
    float calciumDynamics;  // Calcium dynamics time constant
    
    Impl() : prePostWeight(0.002f), postPreWeight(-0.001f),
             postPostWeight(0.001f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), tripletOrder(3.0f),
             spikeHistoryWindow(100.0f), interactionStrength(1.0f),
             nmdaGain(1.0f), ampaGain(1.0f), calciumDynamics(10.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Update activity history
    size_t currentTime = static_cast<size_t>(dt.count());
    for (Timestamp preTime : preSpikes) {
        pImpl->preSpikeTimes[currentTime] += 1.0f;
    }
    for (Timestamp postTime : postSpikes) {
        pImpl->postSpikeTimes[currentTime] += 1.0f;
    }
    
    // Clean old spike times
    for (auto it = pImpl->preSpikeTimes.begin(); it != pImpl->preSpikeTimes.end();) {
        if (currentTime - it->first > tau * 3.0f) {
            it = pImpl->preSpikeTimes.erase(it);
        } else {
            ++it;
        }
    }
    
    for (auto it = pImpl->postSpikeTimes.begin(); it != pImpl->postSpikeTimes.end();) {
        if (currentTime - it->first > tau * 3.0f) {
            it = pImpl->postSpikeTimes.erase(it);
        } else {
            ++it;
        }
    }
    
    // Calculate total pre and post activities
    pImpl->currentPreActivity = 0.0f;
    pImpl->currentPostActivity = 0.0f;
    
    for (const auto& entry : pImpl->preSpikeTimes) {
        pImpl->currentPreActivity += entry.second;
    }
    
    for (const auto& entry : pImpl->postSpikeTimes) {
        pImpl->currentPostActivity += entry.second;
    }
    
    // Apply different STDP variants based on configuration
    // For now, use classic STDP
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Apply advanced STDP rules
    totalDelta += applyTripletSTDP(preSpikes, postSpikes, dt);
    totalDelta += applyRewardModulatedSTDP(synapse, preSpikes, postSpikes, dt);
    totalDelta += applySpikeDependentSTDP(synapse, preSpikes, postSpikes, dt);
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        pImpl->eligibilityTrace += totalDelta * pImpl->rewardModulationFactor;
        pImpl->eligibilityTrace *= (1.0f - pImpl->traceDecay);
    }
}

float STDP::applyTripletSTDP(const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    // Implement triplet STDP: pre-post-post, post-post-pre, post-pre-post interactions
    float tripletDelta = 0.0f;
    
    if (preSpikes.size() < 2 || postSpikes.size() < 2) {
        return tripletDelta;
    }
    
    // Calculate triplet contributions
    // Simplified triplet implementation
    float tau = pImpl->tripletTimeConstant;
    
    // Count spike pairs for triplet interactions
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            float t_pre = preSpikes[i];
            float t_post1 = postSpikes[j];
            
            // Find subsequent post spike
            for (size_t k = j + 1; k < postSpikes.size(); ++k) {
                float t_post2 = postSpikes[k];
                
                // pre-post-post triplet
                float dt1 = t_post1 - t_pre;
                float dt2 = t_post2 - t_post1;
                
                if (dt1 > 0 && dt2 > 0) {
                    tripletDelta += 0.002f * std::exp(-dt1 / tau) * std::exp(-dt2 / tau);
                }
            }
            
            // Find preceding pre spike
            for (size_t k = i > 0 ? i - 1 : 0; k < i; ++k) {
                float t_pre2 = preSpikes[k];
                
                // post-pre-post triplet (reverse time)
                float dt1 = t_pre - t_post1;
                float dt2 = t_pre1 - t_pre2;
                
                if (dt1 > 0 && dt2 > 0) {
                    tripletDelta += -0.001f * std::exp(-dt1 / tau) * std::exp(-dt2 / tau);
                }
            }
        }
    }
    
    return tripletDelta;
}

float STDP::applyRewardModulatedSTDP(Synapse* synapse,
                                    const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration dt) {
    // Implement reward-modulated STDP (R-STDP) with eligibility traces
    float rstdpDelta = 0.0f;
    
    if (!synapse) {
        return rstdpDelta;
    }
    
    // Classic STDP contribution
    float classicDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potentiation
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                classicDelta += delta;
            } else if (dt < 0) {
                // Post before pre: depression
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                classicDelta += delta;
            }
        }
    }
    
    // Apply reward modulation
    float rewardFactor = pImpl->rewardModulationFactor;
    if (synapse->getEligibilityTrace() > 0.0f) {
        rewardFactor *= std::max(0.0f, synapse->getEligibilityTrace());
    }
    
    rstdpDelta = classicDelta * rewardFactor;
    
    // Update eligibility trace
    pImpl->eligibilityTrace += classicDelta;
    pImpl->eligibilityTrace *= (1.0f - pImpl->traceDecay);
    
    return rstdpDelta;
}

float STDP::applySpikeDependentSTDP(Synapse* synapse,
                                   const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   TimestepDuration dt) {
    // Implement spike-dependent STDP: activity-dependent time windows
    float spikeDependentDelta = 0.0f;
    
    if (!synapse) {
        return spikeDependentDelta;
    }
    
    // Adjust time constants based on activity
    float activityLevel = pImpl->currentPreActivity + pImpl->currentPostActivity;
    float adaptiveTau = pImpl->timeConstant * (1.0f + activityLevel * pImpl->activityDepletionRate);
    
    // Apply STDP with adaptive time window
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potentiation with adaptive time window
                float delta = pImpl->ltpWeight * std::exp(-dt / adaptiveTau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: depression with adaptive time window
                float delta = -pImpl->ltdWeight * std::exp(dt / adaptiveTau);
                totalDelta += delta;
            }
        }
    }
    
    // Apply neuromodulator gating
    float neuromodulatorFactor = 1.0f + pImpl->neuromodulatorGain * activityLevel;
    spikeDependentDelta = totalDelta * neuromodulatorFactor;
    
    return spikeDependentDelta;
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

void STDP::configureAdvanced(float ltpWeight, float ltdWeight, float tau,
                             float tripletTau, float rewardMod, float activityDepletion,
                             float neuromodulatorGain) {
    configure(ltpWeight, ltdWeight, tau);
    pImpl->tripletTimeConstant = tripletTau;
    pImpl->rewardModulationFactor = rewardMod;
    pImpl->activityDepletionRate = activityDepletion;
    pImpl->neuromodulatorGain = neuromodulatorGain;
}

void STDP::enableRewardModulation(bool enable, float modulationFactor) {
    pImpl->rewardModulationFactor = enable ? modulationFactor : 1.0f;
}

void STDP::enableTripletInteractions(bool enable, float tripletTau) {
    pImpl->tripletTimeConstant = enable ? tripletTau : 1000.0f;
}

void STDP::enableSpikeDependentLearning(bool enable, float depletionRate) {
    pImpl->activityDepletionRate = enable ? depletionRate : 0.0f;
}

void STDP::enableNeuromodulatorGating(bool enable, float gain) {
    pImpl->neuromodulatorGain = enable ? gain : 0.0f;
}

float STDP::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void STDP::updateEligibilityTrace(float delta) {
    pImpl->eligibilityTrace += delta;
    pImpl->eligibilityTrace *= (1.0f - pImpl->traceDecay);
}

void STDP::resetEligibilityTrace() {
    pImpl->eligibilityTrace = 0.0f;
}

} // namespace nlm

// Implementation of RewardModulatedSTDP methods
namespace nlm {

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new RewardModulatedSTDP::Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate classic STDP contribution
    float classicDelta = calculateClassicSTDP(preSpikes, postSpikes, dt);
    
    // Apply reward modulation
    float rewardFactor = pImpl->rewardModulationFactor;
    
    // Update eligibility trace
    pImpl->eligibilityTrace += classicDelta;
    pImpl->eligibilityTrace *= (1.0f - pImpl->traceDecay);
    
    // Apply neuromodulator-gated learning
    float neuromodulatorLevel = pImpl->neuromodulatorBaseLevel + 
                              calculateNeuromodulatorSignal(synapse, dt);
    
    rewardFactor *= (1.0f + pImpl->predictionErrorGain * neuromodulatorLevel);
    
    // Apply modulated learning
    float delta = classicDelta * rewardFactor * pImpl->learningRate;
    
    // Apply weight change
    applyWeightChange(synapse, delta);
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "RewardModulatedSTDP";
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setRewardModulationFactor(float factor) {
    pImpl->rewardModulationFactor = std::clamp(factor, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getRewardModulationFactor() const {
    return pImpl->rewardModulationFactor;
}

void RewardModulatedSTDP::setEligibilityTraceDecay(float decay) {
    pImpl->traceDecay = std::clamp(decay, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getEligibilityTraceDecay() const {
    return pImpl->traceDecay;
}

void RewardModulatedSTDP::enableEligibilityTrace(bool enable) {
    // Currently always enabled as part of the implementation
}

void RewardModulatedSTDP::setTraceDecay(float decay) {
    setEligibilityTraceDecay(decay);
}

float RewardModulatedSTDP::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

float RewardModulatedSTDP::calculateClassicSTDP(const std::vector<Timestamp>& preSpikes,
                                              const std::vector<Timestamp>& postSpikes,
                                              TimestepDuration dt) {
    // Classic STDP implementation for R-STDP
    float delta = 0.0f;
    float timeConstant = 20.0f; // ms
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: potentiation
                delta += 0.01f * std::exp(-dt / timeConstant);
            } else if (dt < 0) {
                // Post before pre: depression
                delta -= 0.012f * std::exp(dt / timeConstant);
            }
        }
    }
    
    return delta;
}

float RewardModulatedSTDP::calculateNeuromodulatorSignal(Synapse* synapse, TimestepDuration dt) {
    // Calculate neuromodulator signal based on synaptic activity
    if (!synapse) return 0.0f;
    
    // Simple neuromodulator model based on synaptic efficacy
    float efficacy = synapse->getEfficacy();
    float trace = synapse->getEligibilityTrace();
    
    return (efficacy + std::abs(trace)) * 0.5f;
}

} // namespace nlm
