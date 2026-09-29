#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

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
    // Input validation
    if (synapse == nullptr) {
        NLM_LOG_WARN("STDP::update: synapse is null");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        // No spikes to process - this is normal, not an error
        return;
    }
    
    if (dt <= 0.0f) {
        NLM_LOG_WARN("STDP::update: invalid timestep duration: " + std::to_string(dt));
        return;
    }
    
    // Validate parameters
    if (pImpl->timeConstant <= 0.0f) {
        NLM_LOG_WARN("STDP::update: invalid time constant: " + std::to_string(pImpl->timeConstant));
        return;
    }
    
    if (pImpl->minWeight >= pImpl->maxWeight) {
        NLM_LOG_WARN("STDP::update: invalid weight bounds: min=" + std::to_string(pImpl->minWeight) + 
                    ", max=" + std::to_string(pImpl->maxWeight));
        return;
    }
    
    try {
        float totalDelta = 0.0f;
        float tau = pImpl->timeConstant;
        
        // Process all spike pairs with error handling
        for (size_t i = 0; i < preSpikes.size(); ++i) {
            Timestamp preTime = preSpikes[i];
            
            for (size_t j = 0; j < postSpikes.size(); ++j) {
                Timestamp postTime = postSpikes[j];
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
        
        // Apply weight change with bounds and error handling
        if (std::abs(totalDelta) > 1e-6f) {
            try {
                // Scale by synaptic efficacy if available
                float efficacy = synapse->getEfficacy();
                if (efficacy < 0.0f || efficacy > 2.0f) {
                    NLM_LOG_WARN("STDP::update: invalid efficacy value: " + std::to_string(efficacy));
                    efficacy = std::clamp(efficacy, 0.0f, 2.0f);
                }
                
                totalDelta *= efficacy;
                
                // Apply weight change
                synapse->addToWeight(totalDelta);
                
                // Update eligibility trace for reward-modulated learning
                float currentTrace = synapse->getEligibilityTrace();
                synapse->setEligibilityTrace(currentTrace + totalDelta);
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("STDP::update: error applying weight change: " + std::string(e.what()));
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("STDP::update: error processing spike pairs: " + std::string(e.what()));
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (synapse == nullptr) {
        NLM_LOG_WARN("STDP::applyWeightChange: synapse is null");
        return;
    }
    
    if (std::abs(delta) < 1e-6f) {
        // Very small weight change - skip to avoid numerical issues
        return;
    }
    
    try {
        // Clamp weight to bounds
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        
        // Check for weight change
        if (std::abs(newWeight - synapse->getWeight()) > 1e-6f) {
            synapse->setWeight(newWeight);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("STDP::applyWeightChange: error applying weight change: " + std::string(e.what()));
    }
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    if (weight < 0.0f) {
        NLM_LOG_WARN("STDP::setLTPWeight: negative weight value will be clamped: " + std::to_string(weight));
    }
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    if (weight < 0.0f) {
        NLM_LOG_WARN("STDP::setLTDWeight: negative weight value will be clamped: " + std::to_string(weight));
    }
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    if (tau <= 0.0f) {
        NLM_LOG_WARN("STDP::setTimeConstant: invalid time constant: " + std::to_string(tau));
        return;
    }
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    if (std::abs(ltpWeight - pImpl->ltpWeight) < 1e-6f && 
        std::abs(ltdWeight - pImpl->ltdWeight) < 1e-6f &&
        std::abs(tau - pImpl->timeConstant) < 1e-6f) {
        return; // No change needed
    }
    
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    
    NLM_LOG_INFO("STDP configured: ltpWeight=" + std::to_string(pImpl->ltpWeight) +
                ", ltdWeight=" + std::to_string(pImpl->ltdWeight) +
                ", timeConstant=" + std::to_string(pImpl->timeConstant));
}

} // namespace nlm
