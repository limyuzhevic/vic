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
    if (!synapse) {
        NLM_LOG_ERROR("STDP::update: null synapse pointer provided");
        return;
    }
    
    if (preSpikes.empty()) {
        NLM_LOG_ERROR("STDP::update: preSpikes vector is empty");
        return;
    }
    
    if (postSpikes.empty()) {
        NLM_LOG_ERROR("STDP::update: postSpikes vector is empty");
        return;
    }
    
    if (dt <= 0.0) {
        NLM_LOG_ERROR("STDP::update: invalid timestep duration " + std::to_string(dt) + ". Must be positive.");
        return;
    }
    
    // Validate spike timestamps are reasonable
    for (Timestamp time : preSpikes) {
        if (time < 0.0) {
            NLM_LOG_WARNING("STDP::update: negative preSpike timestamp " + std::to_string(time));
        }
    }
    
    for (Timestamp time : postSpikes) {
        if (time < 0.0) {
            NLM_LOG_WARNING("STDP::update: negative postSpike timestamp " + std::to_string(time));
        }
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Validate tau is reasonable
    if (tau <= 0.0f) {
        NLM_LOG_ERROR("STDP::update: invalid time constant tau " + std::to_string(tau) + ". Must be positive.");
        return;
    }
    
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
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Validate STDP parameters are reasonable
        if (pImpl->ltpWeight < 0.0f || pImpl->ltpWeight > 1.0f) {
            NLM_LOG_WARNING("STDP::update: LTP weight " + std::to_string(pImpl->ltpWeight) + 
                           " outside expected range [0.0, 1.0]");
        }
        
        if (pImpl->ltdWeight < 0.0f || pImpl->ltdWeight > 1.0f) {
            NLM_LOG_WARNING("STDP::update: LTD weight " + std::to_string(pImpl->ltdWeight) + 
                           " outside expected range [0.0, 1.0]");
        }
        
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        if (efficacy < 0.0f || efficacy > 2.0f) {
            NLM_LOG_WARNING("STDP::update: synapse efficacy " + std::to_string(efficacy) + 
                           " outside expected range [0.0, 2.0]");
        }
        
        totalDelta *= efficacy;
        
        // Validate resulting weight change
        float currentWeight = synapse->getWeight();
        float newWeight = currentWeight + totalDelta;
        if (newWeight < pImpl->minWeight - 0.1f || newWeight > pImpl->maxWeight + 0.1f) {
            NLM_LOG_WARNING("STDP::update: weight change may result in out-of-bounds weight: " + 
                           std::to_string(newWeight) + ". Current: " + std::to_string(currentWeight));
        }
        
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
