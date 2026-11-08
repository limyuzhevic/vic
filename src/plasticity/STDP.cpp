#include "STDP.hpp"
#include "../brain/Synapse.hpp"
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

// ============= OPTIMIZED STDP UPDATE =============
    void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Use only the most relevant pre/post pair for efficiency
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    
    if (dtMs > 0) {
        // Pre before post - potentiation
        totalDelta = pImpl->ltpWeight * std::exp(-dtMs / tau);
    } else if (dtMs < 0) {
        // Post before pre - depression
        totalDelta = -pImpl->ltdWeight * std::exp(dtMs / tau);
    }
    
    // Apply optimized weight change with early exit
    if (std::abs(totalDelta) > 1e-8f) {
        float w = synapse->getWeight();
        float absW = std::abs(w);
        
        // Optimized weight-dependent scaling
        if (absW > 0.99f) {
            totalDelta *= (1.0f - absW);  // Near saturation - minimal change
        } else if (absW < 0.01f) {
            totalDelta *= absW;          // Near zero - small change  
        } else {
            totalDelta *= (1.0f - absW * 0.5f);  // Mid-range - moderate scaling
        }
        
        // Apply synaptic efficacy scaling (optimized branching)
        float efficacy = synapse->getEfficacy();
        if (efficacy != 1.0f) {
            totalDelta *= efficacy;
        }
        
        // Handle inhibitory synapse sign (optimized)
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Optimized sign enforcement with single check
        float newWeight = synapse->getWeight();
        if (synapse->getType() == SynapseType::Inhibitory) {
            if (newWeight > -0.01f) {
                synapse->setWeight(-0.01f);
            }
        } else {
            if (newWeight < 0.01f) {
                synapse->setWeight(0.01f);
            }
        }
        
        // Update eligibility trace
        synapse->setEligibilityTrace(synapse->getEligibilityTrace() + totalDelta);
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
