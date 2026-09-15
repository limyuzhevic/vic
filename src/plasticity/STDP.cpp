#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <map>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation (~10-20ms window)
    float ltdWeight;      // A- for depression (~30-50ms window)
    float tauPlus;        // τ+ for potentiation time constant
    float tauMinus;       // τ- for depression time constant
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    float homeostaticFactor; // Homeostatic scaling factor
    
    // Eligibility traces (spike-dependent)
    std::map<SynapseId, float> eligibilityTraces;  // synapse_id -> trace value
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), 
             tauPlus(20.0f), tauMinus(30.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             homeostaticFactor(1.0f) {}
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
    
    // Process spike pairs for pairwise STDP with biexponential windows
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (deltaT > 0) {
                // Pre before post: POTENTIATION with exponential decay
                // Biological basis: NMDA receptor activation, Ca2+ influx
                totalDelta += pImpl->ltpWeight * std::exp(-deltaT / pImpl->tauPlus);
            } else if (deltaT < 0) {
                // Post before pre: DEPRESSION with exponential decay
                // Biological basis: backpropagating action potentials, Ca2+ influx
                totalDelta -= pImpl->ltdWeight * std::exp(deltaT / pImpl->tauMinus);
            }
        }
    }
    
    // Apply homeostatic plasticity to prevent runaway excitation
    float homeostasisFactor = 1.0f;
    if (synapse->getType() == SynapseType::Excitatory) {
        // Excitatory synaptic scaling based on overall activity
        float targetExcLevel = 0.8f;  // Target excitation level (0-1)
        float currentExc = synapse->getWeight();
        if (std::abs(currentExc) > 1e-6f) {
            homeostasisFactor = 1.0f - pImpl->homeostaticFactor * (currentExc - targetExcLevel);
        }
    } else if (synapse->getType() == SynapseType::Inhibitory) {
        // Inhibitory synaptic scaling based on overall activity
        float targetInhLevel = -0.4f;  // Target inhibition level (negative)
        float currentInh = synapse->getWeight();
        if (std::abs(currentInh) > 1e-6f) {
            homeostasisFactor = 1.0f - pImpl->homeostaticFactor * (currentInh - targetInhLevel);
        }
    }
    
    totalDelta *= homeostasisFactor;
    
    // Apply synaptic normalization to maintain stability
    float totalDeltasq = totalDelta * totalDelta;
    if (totalDeltasq > 0.001f) {
        // Normalize based on existing weight magnitude
        float weightMagnitude = std::abs(synapse->getWeight());
        if (weightMagnitude > 0.1f) {
            totalDelta *= 0.1f / weightMagnitude;  // Limit updates for strong synapses
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
        updateEligibilityTrace(synapse, totalDelta);
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

void STDP::setTimeConstantPlus(float tau) {
    pImpl->tauPlus = std::clamp(tau, 5.0f, 50.0f);
}

float STDP::getTimeConstantPlus() const {
    return pImpl->tauPlus;
}

void STDP::setTimeConstantMinus(float tau) {
    pImpl->tauMinus = std::clamp(tau, 10.0f, 100.0f);
}

float STDP::getTimeConstantMinus() const {
    return pImpl->tauMinus;
}

void STDP::setTauPlus(float tauPlus) {
    setTimeConstantPlus(tauPlus);
}

float STDP::getTauPlus() const {
    return getTimeConstantPlus();
}

void STDP::setTauMinus(float tauMinus) {
    setTimeConstantMinus(tauMinus);
}

float STDP::getTauMinus() const {
    return getTimeConstantMinus();
}

void STDP::setHomeostaticFactor(float factor) {
    pImpl->homeostaticFactor = std::clamp(factor, 0.0f, 0.1f);
}

float STDP::getHomeostaticFactor() const {
    return pImpl->homeostaticFactor;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tauPlus, float tauMinus,
                     float homeostaticFactor) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstantPlus(tauPlus);
    setTimeConstantMinus(tauMinus);
    setHomeostaticFactor(homeostaticFactor);
}

void STDP::updateEligibilityTrace(Synapse* synapse, float delta, float decayRate) {
    if (!synapse) return;
    
    float currentTrace = pImpl->eligibilityTraces[synapse->getId()];
    currentTrace = currentTrace * (1.0f - decayRate) + delta;
    pImpl->eligibilityTraces[synapse->getId()] = currentTrace;
}

float STDP::getTraceValue(const Synapse* synapse) const {
    if (!synapse) return 0.0f;
    auto it = pImpl->eligibilityTraces.find(synapse->getId());
    if (it != pImpl->eligibilityTraces.end()) {
        return it->second;
    }
    return 0.0f;
}

} // namespace nlm
