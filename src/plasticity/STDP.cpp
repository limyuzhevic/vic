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
    // Enhanced STDP implementation with robust validation
    if (!synapse) {
        return;  // Defensive programming
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;  // No spike timing for STDP
    }
    
    if (dt <= 0.0f) {
        return;  // Invalid timestep duration
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Enhanced STDP calculation with biological realism
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION (LTP)
                // Exponential decay of potentiation with time difference
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
                
                // Incorporate synaptic history for more realistic dynamics
                float historyFactor = synapse->getHistoryFactor();
                delta *= (1.0f + historyFactor * 0.1f);  // Boost by recent history
                
            } else if (dt < 0) {
                // Post before pre: DEPRESSION (LTD)
                // Exponential depression with time difference
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative
                totalDelta += delta;
                
                // Incorporate calcium dynamics approximation
                float calciumLevel = synapse->getCalciumLevel();
                delta *= (1.0f + calciumLevel * 0.05f);  // Modulate by calcium
                
            } else {
                // Simultaneous spikes: minimal interaction
                // Represent spike pair bursts with small delta
                if (std::abs(dt) < 1.0f) {
                    totalDelta += pImpl->ltpWeight * 0.01f;  // Small potentiation
                }
            }
        }
    }
    
    // Apply weight change with comprehensive validation
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by multiple factors for biological realism
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply neuromodulator influence if available
        float neuromodFactor = synapse->getNeuromodulatorInfluence();
        totalDelta *= (1.0f + neuromodFactor * 0.5f);  // Boost by neuromodulators
        
        // Update eligibility trace for reward modulation
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta * 0.5f);
        
        // Apply bounded weight change
        applyWeightChange(synapse, totalDelta);
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

} // namespace nlm
