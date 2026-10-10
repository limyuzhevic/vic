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
    // Early exit for optimization
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Cache frequently used values
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Pre-compute exponential terms for better performance
    std::vector<float> preExp(preSpikes.size());
    std::vector<float> postExp(postSpikes.size());
    
    // Batch compute exponential terms
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        preExp[i] = std::exp(-static_cast<float>(preSpikes[i]) / tau);
    }
    
    for (size_t i = 0; i < postSpikes.size(); ++i) {
        postExp[i] = std::exp(static_cast<float>(postSpikes[i]) / tau);
    }
    
    // Optimized pairwise computation
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        Timestamp preTime = preSpikes[i];
        float preFactor = preExp[i];
        
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            Timestamp postTime = postSpikes[j];
            float dt_val = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt_val > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->ltpWeight * preFactor;
                totalDelta += delta;
            } else if (dt_val < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->ltdWeight * postExp[j];  // dt is negative, so this subtracts
                totalDelta += delta;
            }
            // dt_val == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
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
