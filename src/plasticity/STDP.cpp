#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <vector>

namespace nlm {

struct STDP::Impl {
    // STDP learning parameters
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float tauPlus;        // Time constant for potentiation (ms)
    float tauMinus;       // Time constant for depression (ms)
    
    // Trace variables for temporal dynamics
    mutable std::unordered_map<SynapseId, float> preTrace;  // Pre-synaptic spike trace
    mutable std::unordered_map<SynapseId, float> postTrace; // Post-synaptic spike trace
    
    // Biological constraints
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    float maxTrace;       // Maximum trace value (for normalization)
    
    // Neuromodulation
    float learningRate;   // Global learning rate modulation (0-1)
    bool enableNeuromodulation; // Whether to apply neuromodulation
    
    // Normalization parameters
    float weightNormFactor;    // Weight normalization factor
    float traceNormFactor;     // Trace normalization factor
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), 
             tauPlus(20.0f), tauMinus(20.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             maxTrace(10.0f), learningRate(1.0f),
             enableNeuromodulation(true), 
             weightNormFactor(1.0f), traceNormFactor(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        // Clean up trace memory when no activity
        if (pImpl->preTrace.find(synapse->getId()) != pImpl->preTrace.end()) {
            pImpl->preTrace.erase(synapse->getId());
        }
        if (pImpl->postTrace.find(synapse->getId()) != pImpl->postTrace.end()) {
            pImpl->postTrace.erase(synapse->getId());
        }
        return;
    }
    
    // Get current spike times (most recent)
    Timestamp currentPreTime = preSpikes.back();
    Timestamp currentPostTime = postSpikes.back();
    
    // Get or initialize traces for this synapse
    SynapseId synId = synapse->getId();
    float& preTrace = pImpl->preTrace[synId];
    float& postTrace = pImpl->postTrace[synId];
    
    // Update traces with exponential decay
    float decayFactor = std::exp(-dt / pImpl->tauPlus);
    preTrace *= decayFactor;
    postTrace *= decayFactor;
    
    // Add new spikes to traces (biological trace mechanism)
    for (Timestamp time : preSpikes) {
        preTrace += 1.0f;  // Each spike adds 1.0 to pre trace
    }
    for (Timestamp time : postSpikes) {
        postTrace += 1.0f; // Each spike adds 1.0 to post trace
    }
    
    // Normalize traces to prevent runaway values
    if (pImpl->traceNormFactor > 0.0f) {
        preTrace = std::min(preTrace / pImpl->traceNormFactor, pImpl->maxTrace);
        postTrace = std::min(postTrace / pImpl->traceNormFactor, pImpl->maxTrace);
    }
    
    // Calculate synaptic weight change based on spike traces
    float deltaW = 0.0f;
    
    // Pair-based STDP contributions
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: LTP contribution
                // Using asymmetric exponential windows
                deltaW += pImpl->ltpWeight * std::exp(-dt / pImpl->tauPlus);
            } else {
                // Post before pre: LTD contribution  
                deltaW -= pImpl->ltdWeight * std::exp(dt / pImpl->tauMinus);
            }
        }
    }
    
    // Neuromodulation: scale learning rate based on brain state
    float learningRate = pImpl->learningRate;
    if (pImpl->enableNeuromodulation) {
        // Apply global neuromodulation factors
        learningRate *= synapse->getEfficacy();
        
        // Calcium-based learning modulation (simplified)
        float calciumLevel = 1.0f;
        if (postTrace > 0.0f) {
            calciumLevel = std::min(2.0f, 1.0f + postTrace / 5.0f);
        }
        
        // Normalize based on calcium concentration
        if (calciumLevel > 0.0f) {
            deltaW /= calciumLevel;
        }
    }
    
    // Apply neuromodulated learning rate
    deltaW *= learningRate;
    
    // Apply STDP update via eligibility trace (addressing LTD/LTP integration)
    float currentEligibilityTrace = synapse->getEligibilityTrace();
    float newEligibilityTrace = currentEligibilityTrace + deltaW;
    
    // Apply eligibility trace decay
    float eligibilityDecay = 0.995f;  // ~0.5% per ms
    newEligibilityTrace *= eligibilityDecay;
    
    // Constrain eligibility trace
    newEligibilityTrace = std::max(-pImpl->maxTrace, std::min(pImpl->maxTrace, newEligibilityTrace));
    
    // Apply weight change with normalization
    float currentWeight = synapse->getWeight();
    float rawDeltaW = deltaW + newEligibilityTrace;  // Combine direct and eligibility-based updates
    
    // Weight normalization to prevent runaway excitation
    float normalizedDeltaW = rawDeltaW;
    if (pImpl->weightNormFactor > 0.0f) {
        // Normalize based on current network activity
        float absWeight = std::abs(currentWeight);
        if (absWeight > 0.1f) {
            float normalizationFactor = std::min(1.0f, pImpl->weightNormFactor / absWeight);
            normalizedDeltaW *= normalizationFactor;
        }
    }
    
    // Clamp to reasonable bounds
    normalizedDeltaW = std::clamp(normalizedDeltaW, 
                                 -0.1f, 0.1f);  // Limit per-step change
    
    // Apply weight change
    synapse->addToWeight(normalizedDeltaW);
    
    // Update eligibility trace for reward-modulated learning
    synapse->setEligibilityTrace(newEligibilityTrace);
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change with bounds
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
    pImpl->tauPlus = std::clamp(tau, 1.0f, 100.0f);
    pImpl->tauMinus = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->tauPlus;  // Return average of both time constants
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

// Helper method to get trace value (for debugging/monitoring)
float STDP::getPreTrace(SynapseId synId) const {
    auto it = pImpl->preTrace.find(synId);
    return it != pImpl->preTrace.end() ? it->second : 0.0f;
}

float STDP::getPostTrace(SynapseId synId) const {
    auto it = pImpl->postTrace.find(synId);
    return it != pImpl->postTrace.end() ? it->second : 0.0f;
}

// Enable/disable neuromodulation
void STDP::setNeuromodulationEnabled(bool enabled) {
    pImpl->enableNeuromodulation = enabled;
}

bool STDP::isNeuromodulationEnabled() const {
    return pImpl->enableNeuromodulation;
}

// Set learning rate modulation
void STDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 10.0f);
}

float STDP::getLearningRate() const {
    return pImpl->learningRate;
}

// Set normalization factors
void STDP::setWeightNormalization(float factor) {
    pImpl->weightNormFactor = std::clamp(factor, 0.0f, 10.0f);
}

void STDP::setTraceNormalization(float factor) {
    pImpl->traceNormFactor = std::clamp(factor, 0.0f, 10.0f);
}

} // namespace nlm
