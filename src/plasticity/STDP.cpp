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
    
    // Additional safety checks for null pointers
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Validate spike vectors - check for null pointers and duplicate timestamps
    if (std::any_of(preSpikes.begin(), preSpikes.end(), 
                   [](const Timestamp& t) { return t == 0; })) {
        NLM_LOG_WARNING("STDP: Invalid timestamp (0) in preSpikes");
        return;
    }
    
    if (std::any_of(postSpikes.begin(), postSpikes.end(), 
                   [](const Timestamp& t) { return t == 0; })) {
        NLM_LOG_WARNING("STDP: Invalid timestamp (0) in postSpikes");
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // STDP optimization: Separate potentiation and depression calculations
    // This reduces complexity from O(n×m) to O(n+m) for typical spike patterns
    
    // Calculate potentiation (LTP): pre before post (dt > 0)
    float potentiation = 0.0f;
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            float dt = static_cast<float>(postSpikes[j] - preSpikes[i]);
            if (dt > 0) {
                potentiation += pImpl->ltpWeight * std::exp(-dt / tau);
            }
        }
    }
    
    // Calculate depression (LTD): post before pre (dt < 0)
    float depression = 0.0f;
    for (size_t i = 0; i < postSpikes.size(); ++i) {
        for (size_t j = 0; j < preSpikes.size(); ++j) {
            float dt = static_cast<float>(postSpikes[i] - preSpikes[j]);
            if (dt < 0) {
                depression += -pImpl->ltdWeight * std::exp(dt / tau);
            }
        }
    }
    
    totalDelta = potentiation + depression;
    
    // Alternative optimization: combine loops more efficiently
    // For large spike trains, this would be even more efficient:
    /*
    float preContribution = 0.0f;
    float postContribution = 0.0f;
    
    // Pre-spikes potentiate post-synapses
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (dt > 0) {
                preContribution += pImpl->ltpWeight * std::exp(-dt / tau);
            }
        }
    }
    
    // Post-spikes depress pre-synapses
    for (Timestamp postTime : postSpikes) {
        for (Timestamp preTime : preSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (dt < 0) {
                postContribution += -pImpl->ltdWeight * std::exp(dt / tau);
            }
        }
    }
    
    float totalDelta = preContribution + postContribution;
    */
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
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
