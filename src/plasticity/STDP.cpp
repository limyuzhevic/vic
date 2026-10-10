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
    
    // Advanced STDP parameters
    float calciumThreshold;   // Calcium threshold for LTP/LTD transition
    float learningRateMod;    // Activity-dependent learning rate modulation
    float homeostaticWeight; // Target weight for homeostasis
    float eligibilityTraceDecay; // Decay rate for eligibility traces
    bool enableTriplet;       // Enable triplet interactions
    bool enableNeuromodulation; // Enable reward-modulated learning
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             calciumThreshold(-40.0f), learningRateMod(1.0f),
             homeostaticWeight(0.0f), eligibilityTraceDecay(0.1f),
             enableTriplet(false), enableNeuromodulation(false) {}
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
    
    // Advanced STDP implementation with multiple biological mechanisms
    
    // 1. Basic pairwise STDP
    float basicDelta = calculatePairwiseSTDP(preSpikes, postSpikes);
    
    // 2. Triplet interactions (if enabled)
    float tripletDelta = 0.0f;
    if (pImpl->enableTriplet) {
        tripletDelta = calculateTripletSTDP(preSpikes, postSpikes);
    }
    
    // 3. Neuromodulation (if enabled)
    float neuromodulatedDelta = 0.0f;
    if (pImpl->enableNeuromodulation) {
        neuromodulatedDelta = calculateNeuromodulatedSTDP(synapse, preSpikes, postSpikes);
    }
    
    // 4. Homeostatic plasticity
    float homeostaticDelta = calculateHomeostaticPlasticity(synapse);
    
    // 5. Activity-dependent learning rate
    float activityModulation = calculateActivityModulation(preSpikes, postSpikes);
    
    // Combine all contributions
    float totalDelta = (basicDelta + tripletDelta + neuromodulatedDelta + homeostaticDelta) * activityModulation;
    
    // Apply weight change with advanced constraints
    applyAdvancedWeightChange(synapse, totalDelta);
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

// Advanced STDP calculations

float STDP::calculatePairwiseSTDP(const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes) {
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
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
    
    return totalDelta;
}

float STDP::calculateTripletSTDP(const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes) {
    // Simplified triplet STDP considering pre-post-pre and post-pre-post interactions
    float tripletDelta = 0.0f;
    
    if (preSpikes.size() < 2 || postSpikes.size() < 1) {
        return tripletDelta;
    }
    
    // Consider pre-post-pre triplets (for potentiation)
    for (size_t i = 0; i < preSpikes.size() - 1; ++i) {
        for (Timestamp postTime : postSpikes) {
            float pre1ToPost = static_cast<float>(postTime - preSpikes[i]);
            float postToPre2 = static_cast<float>(preSpikes[i + 1] - postTime);
            
            if (pre1ToPost > 0 && postToPre2 > 0) {
                float contribution = pImpl->ltpWeight * std::exp(-pre1ToPost / pImpl->timeConstant) *
                                    std::exp(-postToPre2 / pImpl->timeConstant) * 0.5f;
                tripletDelta += contribution;
            }
        }
    }
    
    // Consider post-pre-post triplets (for depression)
    if (postSpikes.size() < 2 || preSpikes.size() < 1) {
        return tripletDelta;
    }
    
    for (size_t i = 0; i < postSpikes.size() - 1; ++i) {
        for (Timestamp preTime : preSpikes) {
            float post1ToPre = static_cast<float>(preTime - postSpikes[i]);
            float preToPost2 = static_cast<float>(postSpikes[i + 1] - preTime);
            
            if (post1ToPre > 0 && preToPost2 > 0) {
                float contribution = -pImpl->ltdWeight * std::exp(-post1ToPre / pImpl->timeConstant) *
                                    std::exp(-preToPost2 / pImpl->timeConstant) * 0.5f;
                tripletDelta += contribution;
            }
        }
    }
    
    return tripletDelta;
}

float STDP::calculateNeuromodulatedSTDP(Synapse* synapse,
                                        const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes) {
    if (!synapse) return 0.0f;
    
    // Neuromodulated STDP uses eligibility traces
    float trace = synapse->getEligibilityTrace();
    float decayFactor = pImpl->eligibilityTraceDecay;
    
    // Update eligibility trace with current spike activity
    float activityFactor = 0.0f;
    if (!preSpikes.empty() && !postSpikes.empty()) {
        // Simple activity factor based on recent spike counts
        activityFactor = std::min(1.0f, static_cast<float>(preSpikes.size() + postSpikes.size()) / 10.0f);
        trace = trace * (1.0f - decayFactor) + activityFactor;
        synapse->setEligibilityTrace(trace);
    }
    
    // Neuromodulation factor (simplified - would use actual dopamine signals)
    float neuromodulationFactor = 1.0f;  // Placeholder
    
    return calculatePairwiseSTDP(preSpikes, postSpikes) * neuromodulationFactor * trace;
}

float STDP::calculateHomeostaticPlasticity(Synapse* synapse) {
    if (!synapse) return 0.0f;
    
    float currentWeight = synapse->getWeight();
    float homeostaticDelta = 0.0f;
    
    // Homeostatic plasticity drives synapse toward target weight
    if (std::abs(currentWeight - pImpl->homeostaticWeight) > 0.01f) {
        float deviation = currentWeight - pImpl->homeostaticWeight;
        // Homeostatic learning rate (slower than Hebbian)
        float homeostaticLearningRate = 0.001f;
        homeostaticDelta = -homeostaticLearningRate * deviation;
    }
    
    return homeostaticDelta;
}

float STDP::calculateActivityModulation(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return 1.0f;
    }
    
    // Calculate firing rates from spike times
    float preRate = static_cast<float>(preSpikes.size() - 1) / 
                   static_cast<float>(postSpikes.back() - preSpikes.front());
    float postRate = static_cast<float>(postSpikes.size() - 1) / 
                    static_cast<float>(postSpikes.back() - postSpikes.front());
    
    // Activity-dependent modulation (saturating function)
    float totalRate = preRate + postRate;
    float modulation = 1.0f / (1.0f + std::exp(-5.0f * (totalRate - 10.0f)));  // Sigmoid
    
    return pImpl->learningRateMod * modulation;
}

void STDP::applyAdvancedWeightChange(Synapse* synapse, float delta) {
    if (!synapse || std::abs(delta) < 1e-6f) return;
    
    // Scale by synaptic efficacy
    float efficacy = synapse->getEfficacy();
    delta *= efficacy;
    
    // Apply weight change with advanced constraints
    float newWeight = synapse->getWeight() + delta;
    
    // Apply bounds checking
    if (newWeight < pImpl->minWeight) {
        // Soft bound - allow slight violation but with penalty
        float penalty = 0.1f * (pImpl->minWeight - newWeight);
        newWeight = pImpl->minWeight + penalty;
    } else if (newWeight > pImpl->maxWeight) {
        float penalty = 0.1f * (newWeight - pImpl->maxWeight);
        newWeight = pImpl->maxWeight - penalty;
    }
    
    synapse->setWeight(newWeight);
    
    // Update eligibility trace
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace + delta * 0.1f);
    
    // Apply synaptic efficacy modulation
    float newEfficacy = synapse->getEfficacy();
    if (delta > 0.01f) {  // Potentiation
        newEfficacy = std::min(2.0f, newEfficacy * 1.01f);
    } else if (delta < -0.01f) {  // Depression
        newEfficacy = std::max(0.5f, newEfficacy * 0.99f);
    }
    synapse->setEfficacy(newEfficacy);
}

// Enhanced STDP configuration methods
void STDP::enableTripletInteractions(bool enabled) {
    pImpl->enableTriplet = enabled;
    NLM_LOG_INFO("Triplet STDP " + std::string(enabled ? "enabled" : "disabled"));
}

void STDP::enableNeuromodulation(bool enabled) {
    pImpl->enableNeuromodulation = enabled;
    NLM_LOG_INFO("Neuromodulated STDP " + std::string(enabled ? "enabled" : "disabled"));
}

void STDP::setHomeostaticWeight(float weight) {
    pImpl->homeostaticWeight = weight;
}

void STDP::setEligibilityTraceDecay(float decay) {
    pImpl->eligibilityTraceDecay = std::clamp(decay, 0.0f, 1.0f);
}

void STDP::setCalciumThreshold(float threshold) {
    pImpl->calciumThreshold = threshold;
}

void STDP::setLearningRateModulation(float mod) {
    pImpl->learningRateMod = std::max(0.0f, mod);
}

// Get advanced STDP statistics
float STDP::getEligibilityTrace(Synapse* synapse) const {
    if (!synapse) return 0.0f;
    return synapse->getEligibilityTrace();
}

float STDP::getCalcium(Synapse* synapse) const {
    // Placeholder for calcium dynamics
    return 0.0f;
}

bool STDP::isTripletEnabled() const {
    return pImpl->enableTriplet;
}

bool STDP::isNeuromodulationEnabled() const {
    return pImpl->enableNeuromodulation;
}

float STDP::getHomeostaticWeight() const {
    return pImpl->homeostaticWeight;
}

float STDP::getEligibilityTraceDecay() const {
    return pImpl->eligibilityTraceDecay;
}

} // namespace nlm

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
