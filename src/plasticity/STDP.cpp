#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <unordered_map>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float timeConstantMinus;  // Separate tau- (for asymmetric STDP)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Triplet STDP parameters (for more biological accuracy)
    float aPlus;          // Triplet potentiation coefficient
    float aMinus;         // Triplet depression coefficient
    float bPlus;          // Triplet potentiation baseline
    float bMinus;         // Triplet depression baseline
    
    // Learning rate modifiers
    float learningRate;   // Global learning rate modifier
    float dopamineMod;    // Neuromodulation factor (0-1)
    
    // Eligibility traces for reward-modulated learning
    std::unordered_map<SynapseHandle, float> eligibilityTraces;
    float traceDecayRate; // Rate at which eligibility traces decay
    float traceTimeConstant; // Time constant for eligibility traces
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             timeConstantMinus(20.0f), minWeight(-1.0f), maxWeight(1.0f),
             aPlus(0.005f), aMinus(0.005f), bPlus(0.0f), bMinus(0.0f),
             learningRate(1.0f), dopamineMod(1.0f), traceDecayRate(0.01f),
             traceTimeConstant(100.0f) {
        // Initialize eligibility trace map
        eligibilityTraces.clear();
    }
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
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    float tauMinus = pImpl->timeConstantMinus;
    
    // Apply global learning rate and neuromodulation
    float effectiveLTP = pImpl->ltpWeight * pImpl->learningRate * pImpl->dopamineMod;
    float effectiveLTD = pImpl->ltdWeight * pImpl->learningRate * pImpl->dopamineMod;
    
    // Pairwise STDP (classic implementation)
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION (LTP)
                // Exponential decay of potentiation effect with time difference
                float delta = effectiveLTP * std::exp(-dt / tau);
                totalDelta += delta;
                
                // Add triplet contribution for more biological accuracy
                // Triplet STDP: pre-pre-post interactions
                if (dt > 10.0f) {  // Only consider significant time differences
                    float tripletDelta = pImpl->aPlus * std::exp(-dt / (2.0f * tau)) *
                                        (1.0f + pImpl->bPlus * 0.1f);  // Simplified triplet
                    totalDelta += tripletDelta;
                }
            } else if (dt < 0) {
                // Post before pre: DEPRESSION (LTD)
                // Exponential decay of depression effect with time difference
                float delta = -effectiveLTD * std::exp(dt / tauMinus);
                totalDelta += delta;
                
                // Add triplet contribution
                if (dt < -10.0f) {  // Consider negative time differences
                    float tripletDelta = pImpl->aMinus * std::exp(dt / (2.0f * tauMinus)) *
                                        (1.0f + pImpl->bMinus * 0.1f);
                    totalDelta += tripletDelta;
                }
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply eligibility traces for reward-modulated learning
    SynapseHandle synapseHandle = synapse->getHandle();
    float eligibility = pImpl->eligibilityTraces[synapseHandle];
    
    // Decay eligibility trace
    eligibility *= (1.0f - pImpl->traceDecayRate * dt);
    pImpl->eligibilityTraces[synapseHandle] = eligibility;
    
    // Update eligibility trace with STDP changes
    eligibility += totalDelta * 0.1f; // Contribution factor
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f || std::abs(eligibility) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        float totalWeightChange = (totalDelta + eligibility) * efficacy;
        
        // Apply weight change using the clamp function
        applyWeightChange(synapse, totalWeightChange);
        
        // Update eligibility trace in synapse (for consistency)
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + eligibility * 0.5f);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight and apply change
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Clamp weight to biological limits
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply smooth transition to prevent abrupt changes
    float weightChange = newWeight - currentWeight;
    if (std::abs(weightChange) > 0.001f) {
        // Add homeostatic regulation - prevent runaway potentiation/depression
        float homeostaticTarget = 0.0f;  // Resting weight
        float homeostaticStrength = 0.01f;  // Homeostatic learning rate
        
        float homeostaticCorrection = homeostaticStrength * (homeostaticTarget - newWeight);
        newWeight += homeostaticCorrection;
        
        // Final clamp after homeostatic correction
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    }
    
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "Advanced STDP (with Triplet Interactions and Eligibility Traces)";
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
    // Maintain relationship with timeConstantMinus
    pImpl->timeConstantMinus = pImpl->timeConstant * 1.05f; // Slight asymmetry
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::setTripletParameters(float aPlus, float aMinus, float bPlus, float bMinus) {
    pImpl->aPlus = std::clamp(aPlus, 0.0f, 0.1f);
    pImpl->aMinus = std::clamp(aMinus, 0.0f, 0.1f);
    pImpl->bPlus = std::clamp(bPlus, 0.0f, 1.0f);
    pImpl->bMinus = std::clamp(bMinus, 0.0f, 1.0f);
}

void STDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 10.0f);
}

void STDP::setNeuromodulation(float factor) {
    pImpl->dopamineMod = std::clamp(factor, 0.0f, 2.0f);
}

void STDP::setEligibilityTraceParams(float decayRate, float timeConstant) {
    pImpl->traceDecayRate = std::clamp(decayRate, 0.001f, 0.1f);
    pImpl->traceTimeConstant = std::clamp(timeConstant, 10.0f, 1000.0f);
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

// Function to update eligibility trace based on neural activity
void STDP::updateEligibilityTrace(Synapse* synapse, Timestamp currentTime) {
    if (!synapse) return;
    
    SynapseHandle handle = synapse->getHandle();
    float currentTrace = synapse->getEligibilityTrace();
    
    // Decay the eligibility trace
    float decayFactor = std::exp(-static_cast<float>(pImpl->traceTimeConstant));
    currentTrace *= decayFactor;
    
    // Update eligibility trace in synapse and in our map
    synapse->setEligibilityTrace(currentTrace);
    pImpl->eligibilityTraces[handle] = currentTrace;
}

} // namespace nlm
