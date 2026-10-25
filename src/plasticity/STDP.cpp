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
    // Validate input parameters
    if (!synapse || !isFinite(dt) || preSpikes.empty() || postSpikes.empty()) {
        NLM_LOG_WARNING("STDP::update called with invalid parameters");
        return;
    }
    
    // Ensure dt is positive (simulation timestep)
    if (dt <= 0.0) {
        NLM_LOG_WARNING("STDP::update called with invalid timestep: " + std::to_string(dt));
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Validate time constant to prevent numerical issues
    if (std::abs(tau) < 1e-9f) {
        NLM_LOG_WARNING("STDP::update called with zero or near-zero time constant");
        return;
    }
    
    // Apply STDP rule for each spike pair
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float timeDiff = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (timeDiff > nlm::NUMERICAL_EPSILON) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-timeDiff / tau);
                
                // Apply numerical bounds to prevent overflow
                delta = std::clamp(delta, nlm::NUMERICAL_MIN_EXP, nlm::NUMERICAL_MAX_EXP);
                totalDelta += delta;
                
            } else if (timeDiff < -nlm::NUMERICAL_EPSILON) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(timeDiff / tau);  // timeDiff is negative
                
                // Apply numerical bounds
                delta = std::clamp(delta, nlm::NUMERICAL_MIN_EXP, nlm::NUMERICAL_MAX_EXP);
                totalDelta += delta;
            }
            // timeDiff ≈ 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Check if weight change is significant
    if (std::abs(totalDelta) > nlm::NUMERICAL_EPSILON) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change with bounds checking
        applyWeightChange(synapse, totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
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

struct RewardModulatedSTDP::Impl {
    float learningRate;           // Reward modulation learning rate
    float predictionError;        // Current prediction error signal
    float eligibilityTrace;       // Eligibility trace for reward modulation
    float ltpWeight;               // LTP weight factor
    float ltdWeight;               // LTD weight factor
    float timeConstant;            // STDP time constant
    
    Impl() : learningRate(0.01f), predictionError(0.0f), eligibilityTrace(0.0f),
             ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    // Validate input parameters
    if (!synapse || !isFinite(dt) || preSpikes.empty() || postSpikes.empty()) {
        NLM_LOG_WARNING("RewardModulatedSTDP::update called with invalid parameters");
        return;
    }
    
    if (dt <= 0.0) {
        NLM_LOG_WARNING("RewardModulatedSTDP::update called with invalid timestep: " + std::to_string(dt));
        return;
    }
    
    float timeDiff = 0.0f;
    float totalDelta = 0.0f;
    
    // Calculate STDP contribution for each spike pair
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            timeDiff = static_cast<float>(postTime - preTime);
            
            if (timeDiff > nlm::NUMERICAL_EPSILON) {
                // Pre before post: Potentiation modulated by prediction error
                float delta = pImpl->ltpWeight * std::exp(-timeDiff / pImpl->timeConstant);
                delta *= (1.0f + pImpl->predictionError); // Reward modulation
                totalDelta += delta;
                
            } else if (timeDiff < -nlm::NUMERICAL_EPSILON) {
                // Post before pre: Depression modulated by prediction error
                float delta = -pImpl->ltdWeight * std::exp(timeDiff / pImpl->timeConstant);
                delta *= (1.0f + pImpl->predictionError); // Reward modulation
                totalDelta += delta;
            }
        }
    }
    
    // Apply weight change with reward modulation
    if (std::abs(totalDelta) > nlm::NUMERICAL_EPSILON) {
        // Apply eligibility trace scaling
        float eligibility = pImpl->eligibilityTrace;
        totalDelta *= eligibility;
        
        // Apply STDP weight change
        applyWeightChange(synapse, totalDelta);
        
        // Update eligibility trace
        pImpl->eligibilityTrace *= nlm::STDP_ELIGIBILITY_DECAY;
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        NLM_LOG_WARNING("RewardModulatedSTDP::applyWeightChange called with null synapse");
        return;
    }
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, nlm::SYNAPTIC_WEIGHT_MIN, nlm::SYNAPTIC_WEIGHT_MAX);
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

void RewardModulatedSTDP::setPredictionError(float error) {
    pImpl->predictionError = std::clamp(error, -1.0f, 1.0f);
}

float RewardModulatedSTDP::getPredictionError() const {
    return pImpl->predictionError;
}

void RewardModulatedSTDP::resetEligibilityTrace() {
    pImpl->eligibilityTrace = 0.0f;
}

float RewardModulatedSTDP::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void RewardModulatedSTDP::configure(float ltpWeight, float ltdWeight, float tau,
                                   float learningRate, float predictionError) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    setLearningRate(learningRate);
    setPredictionError(predictionError);
}

struct TripletSTDP::Impl {
    float a1Plus;      // First-order potentiation (A1+)
    float a2Plus;      // Second-order potentiation (A2+)
    float a1Minus;     // First-order depression (A1-)
    float a2Minus;     // Second-order depression (A2-)
    float tauPlus;     // Potentiation time constant (τ+)
    float tauMinus;    // Depression time constant (τ-)
    float minWeight;   // Minimum synaptic weight
    float maxWeight;   // Maximum synaptic weight
    
    Impl() : a1Plus(0.005f), a2Plus(0.0025f), a1Minus(0.005f), a2Minus(0.0025f),
             tauPlus(20.0f), tauMinus(20.0f), minWeight(-1.0f), maxWeight(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    // Validate input parameters
    if (!synapse || !isFinite(dt) || preSpikes.empty() || postSpikes.empty()) {
        NLM_LOG_WARNING("TripletSTDP::update called with invalid parameters");
        return;
    }
    
    if (dt <= 0.0) {
        NLM_LOG_WARNING("TripletSTDP::update called with invalid timestep: " + std::to_string(dt));
        return;
    }
    
    // Maintain spike history for triplet interactions
    static std::vector<std::pair<Timestamp, bool>> spikeHistory;
    spikeHistory.reserve(preSpikes.size() + postSpikes.size());
    
    // Add pre-synaptic spikes
    for (Timestamp preTime : preSpikes) {
        spikeHistory.emplace_back(preTime, true);  // true = pre-synaptic
    }
    
    // Add post-synaptic spikes
    for (Timestamp postTime : postSpikes) {
        spikeHistory.emplace_back(postTime, false);  // false = post-synaptic
    }
    
    // Sort by time
    std::sort(spikeHistory.begin(), spikeHistory.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    float totalDelta = 0.0f;
    
    // Calculate triplet contributions
    for (size_t i = 0; i < spikeHistory.size(); ++i) {
        if (spikeHistory[i].second) {  // Pre-synaptic spike
            // Look for post-synaptic spikes after this pre spike
            for (size_t j = i + 1; j < spikeHistory.size(); ++j) {
                if (!spikeHistory[j].second) {  // Post-synaptic spike
                    float timeDiff = static_cast<float>(spikeHistory[j].first - spikeHistory[i].first);
                    
                    if (timeDiff > 0) {
                        // First-order potentiation (A1+)
                        totalDelta += pImpl->a1Plus * std::exp(-timeDiff / pImpl->tauPlus);
                        
                        // Second-order potentiation (A2+) from previous pre spikes
                        for (size_t k = 0; k < i; ++k) {
                            if (spikeHistory[k].second) {
                                float prevTimeDiff = static_cast<float>(spikeHistory[i].first - spikeHistory[k].first);
                                if (prevTimeDiff > 0 && prevTimeDiff < pImpl->tauPlus) {
                                    totalDelta += pImpl->a2Plus * std::exp(-prevTimeDiff / pImpl->tauPlus);
                                }
                            }
                        }
                    }
                }
            }
        } else {  // Post-synaptic spike
            // Look for pre-synaptic spikes after this post spike
            for (size_t j = i + 1; j < spikeHistory.size(); ++j) {
                if (spikeHistory[j].second) {  // Pre-synaptic spike
                    float timeDiff = static_cast<float>(spikeHistory[j].first - spikeHistory[i].first);
                    
                    if (timeDiff > 0) {
                        // First-order depression (A1-)
                        totalDelta += -pImpl->a1Minus * std::exp(-timeDiff / pImpl->tauMinus);
                        
                        // Second-order depression (A2-) from previous post spikes
                        for (size_t k = 0; k < i; ++k) {
                            if (!spikeHistory[k].second) {
                                float prevTimeDiff = static_cast<float>(spikeHistory[i].first - spikeHistory[k].first);
                                if (prevTimeDiff > 0 && prevTimeDiff < pImpl->tauMinus) {
                                    totalDelta += -pImpl->a2Minus * std::exp(-prevTimeDiff / pImpl->tauMinus);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > nlm::NUMERICAL_EPSILON) {
        applyWeightChange(synapse, totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        NLM_LOG_WARNING("TripletSTDP::applyWeightChange called with null synapse");
        return;
    }
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setA1Plus(float a1Plus) {
    pImpl->a1Plus = std::clamp(a1Plus, 0.0f, 0.1f);
}

float TripletSTDP::getA1Plus() const {
    return pImpl->a1Plus;
}

void TripletSTDP::setA2Plus(float a2Plus) {
    pImpl->a2Plus = std::clamp(a2Plus, 0.0f, 0.1f);
}

float TripletSTDP::getA2Plus() const {
    return pImpl->a2Plus;
}

void TripletSTDP::setA1Minus(float a1Minus) {
    pImpl->a1Minus = std::clamp(a1Minus, 0.0f, 0.1f);
}

float TripletSTDP::getA1Minus() const {
    return pImpl->a1Minus;
}

void TripletSTDP::setA2Minus(float a2Minus) {
    pImpl->a2Minus = std::clamp(a2Minus, 0.0f, 0.1f);
}

float TripletSTDP::getA2Minus() const {
    return pImpl->a2Minus;
}

void TripletSTDP::setTauPlus(float tauPlus) {
    pImpl->tauPlus = std::clamp(tauPlus, 1.0f, 100.0f);
}

float TripletSTDP::getTauPlus() const {
    return pImpl->tauPlus;
}

void TripletSTDP::setTauMinus(float tauMinus) {
    pImpl->tauMinus = std::clamp(tauMinus, 1.0f, 100.0f);
}

float TripletSTDP::getTauMinus() const {
    return pImpl->tauMinus;
}

void TripletSTDP::configure(float a1Plus, float a2Plus, float a1Minus, float a2Minus,
                           float tauPlus, float tauMinus) {
    setA1Plus(a1Plus);
    setA2Plus(a2Plus);
    setA1Minus(a1Minus);
    setA2Minus(a2Minus);
    setTauPlus(tauPlus);
    setTauMinus(tauMinus);
}

} // namespace nlm
