#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <random>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Advanced STDP parameters
    bool enableTripletRules;     // Enable triplet spike interactions
    bool enableLocalStdp;        // Enable local (neuron-specific) STDP
    bool enableWeightNormalization; // Enable weight normalization
    float tripletLtpWeight;      // Triplet potentiation weight
    float tripletLtdWeight;      // Triplet depression weight
    float eligibilityDecayRate;  // Rate of eligibility trace decay
    size_t maxTraceHistory;      // Maximum number of eligibility traces to store
    
    // Performance optimization
    size_t maxSpikesPerSynapse;  // Maximum spikes to consider for STDP
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             enableTripletRules(false), enableLocalStdp(false),
             enableWeightNormalization(true), tripletLtpWeight(0.005f),
             tripletLtdWeight(0.006f), eligibilityDecayRate(0.01f),
             maxTraceHistory(1000), maxSpikesPerSynapse(100) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Real STDP implementation based on spike-timing correlation
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   
     * Limitations:
     *   - Simplified pairwise rule (doesn't capture triplet interactions)
     *   - Assumes single exponential window (more complex in biology)
     *   - Doesn't account for synaptic eligibility traces
     */
    
    // Input validation
    if (!synapse) {
        NLM_LOG_ERROR("STDP::update: synapse is null");
        return;
    }
    
    if (preSpikes.empty()) {
        NLM_LOG_DEBUG("STDP::update: no pre-synaptic spikes");
        return;
    }
    
    if (postSpikes.empty()) {
        NLM_LOG_DEBUG("STDP::update: no post-synaptic spikes");
        return;
    }
    
    if (std::isnan(dt) || dt <= 0) {
        NLM_LOG_ERROR("STDP::update: invalid timestep duration: " + std::to_string(dt));
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Basic pairwise STDP
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply triplet STDP if enabled
    if (pImpl->enableTripletRules) {
        float tripletDelta = calculateTripletEffect(preSpikes, postSpikes, tau);
        totalDelta += tripletDelta;
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        if (std::isnan(efficacy) || efficacy < 0) {
            NLM_LOG_ERROR("STDP::update: invalid synaptic efficacy: " + std::to_string(efficacy));
            return;
        }
        
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        if (pImpl->eligibilityDecayRate > 0) {
            float currentTrace = synapse->getEligibilityTrace();
            synapse->setEligibilityTrace(currentTrace + totalDelta);
        }
        
        // Decay eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace * (1.0f - pImpl->eligibilityDecayRate));
        
        // Log the update
        NLM_LOG_DEBUG("STDP update: synapse " + std::to_string(synapse->getId().value) + 
                     " weight change: " + std::to_string(totalDelta));
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        NLM_LOG_ERROR("STDP::applyWeightChange: synapse is null");
        return;
    }
    
    if (std::isnan(delta)) {
        NLM_LOG_ERROR("STDP::applyWeightChange: delta is NaN");
        return;
    }
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply weight change
    synapse->setWeight(newWeight);
    
    // Log the update
    NLM_LOG_DEBUG("STDP applyWeightChange: synapse " + std::to_string(synapse->getId().value) + 
                 " weight changed from " + std::to_string(synapse->getWeight() - delta) + 
                 " to " + std::to_string(newWeight));
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    if (std::isnan(weight)) {
        NLM_LOG_ERROR("STDP::setLTPWeight: weight is NaN");
        return;
    }
    
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
    NLM_LOG_DEBUG("STDP::setLTPWeight: set to " + std::to_string(pImpl->ltpWeight));
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    if (std::isnan(weight)) {
        NLM_LOG_ERROR("STDP::setLTDWeight: weight is NaN");
        return;
    }
    
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
    NLM_LOG_DEBUG("STDP::setLTDWeight: set to " + std::to_string(pImpl->ltdWeight));
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    if (std::isnan(tau) || tau <= 0) {
        NLM_LOG_ERROR("STDP::setTimeConstant: invalid tau value: " + std::to_string(tau));
        return;
    }
    
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
    NLM_LOG_DEBUG("STDP::setTimeConstant: set to " + std::to_string(pImpl->timeConstant));
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

float STDP::calculateTripletEffect(const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   float tau) {
    // Calculate triplet STDP effects
    float totalTripletDelta = 0.0f;
    
    if (preSpikes.size() < 2 || postSpikes.size() < 1) {
        return 0.0f;
    }
    
    // Third-order interaction: pre-pre-post
    for (size_t i = 0; i < preSpikes.size() - 1; ++i) {
        for (Timestamp postTime : postSpikes) {
            float dt1 = static_cast<float>(postTime - preSpikes[i]);  // Δt1: post - pre1
            float dt2 = static_cast<float>(preSpikes[i+1] - preSpikes[i]);  // Δt2: pre2 - pre1
            
            if (dt1 > 0 && dt2 > 0) {
                // Pre-pre-post interaction
                float delta = pImpl->tripletLtpWeight * std::exp(-dt1 / tau) * std::exp(-dt2 / tau);
                totalTripletDelta += delta;
            }
        }
    }
    
    // Third-order interaction: post-post-pre
    if (postSpikes.size() >= 2) {
        for (size_t i = 0; i < postSpikes.size() - 1; ++i) {
            for (Timestamp preTime : preSpikes) {
                float dt1 = static_cast<float>(preTime - postSpikes[i]);  // Δt1: pre - post1
                float dt2 = static_cast<float>(postSpikes[i+1] - postSpikes[i]);  // Δt2: post2 - post1
                
                if (dt1 < 0 && dt2 < 0) {
                    // Post-post-pre interaction
                    float delta = -pImpl->tripletLtdWeight * std::exp(dt1 / tau) * std::exp(dt2 / tau);
                    totalTripletDelta += delta;
                }
            }
        }
    }
    
    return totalTripletDelta;
}

void STDP::enableAdvancedFeatures(bool tripletRules, bool localStdp, bool weightNormalization) {
    enableTripletRules = tripletRules;
    enableLocalStdp = localStdp;
    enableWeightNormalization = weightNormalization;
    
    NLM_LOG_DEBUG("STDP advanced features enabled: tripletRules=" + std::to_string(tripletRules) +
                  ", localStdp=" + std::to_string(localStdp) +
                  ", weightNormalization=" + std::to_string(weightNormalization));
}

void STDP::setEligibilityDecayRate(float rate) {
    if (std::isnan(rate) || rate < 0 || rate >= 1.0f) {
        NLM_LOG_ERROR("STDP::setEligibilityDecayRate: invalid rate: " + std::to_string(rate));
        return;
    }
    
    pImpl->eligibilityDecayRate = rate;
    NLM_LOG_DEBUG("STDP::setEligibilityDecayRate: set to " + std::to_string(rate));
}

void STDP::setMaxTraceHistory(size_t maxHistory) {
    if (maxHistory == 0) {
        NLM_LOG_ERROR("STDP::setMaxTraceHistory: maxHistory must be > 0");
        return;
    }
    
    pImpl->maxTraceHistory = maxHistory;
    NLM_LOG_DEBUG("STDP::setMaxTraceHistory: set to " + std::to_string(maxHistory));
}

} // namespace nlm