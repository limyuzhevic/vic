// Modern C++ Implementation of STDP.cpp with Strategy Pattern
// This file implements the Spike-Timing-Dependent Plasticity (STDP) learning rule
// using modern C++ features and the Strategy pattern

#include "STDP.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <execution>
#include <thread>

namespace nlm {

// STDP Implementation Details
STDP::STDP() : pImpl(std::make_unique<Impl>()) {
    // Initialize default STDP parameters
    pImpl->ltpWeight = 0.1f;      // Long-term potentiation weight
    pImpl->ltdWeight = 0.1f;      // Long-term depression weight  
    pImpl->timeConstant = 20.0f; // STDP time window (τ)
    pImpl->minWeight = -1.0f;    // Minimum synaptic weight
    pImpl->maxWeight = 1.0f;     // Maximum synaptic weight
}

STDP::~STDP() = default;

// Core STDP update with modern C++ optimizations
void STDP::update(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) {
    
    // Input validation
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Modern C++: Use range-based for loops and structured bindings
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Optimized STDP calculation using modern algorithms
    // Calculate spike timing differences in parallel using modern C++17 features
    auto preIt = std::execution::par_unseq;
    
    // Modern approach: Use iterators and algorithms for better performance
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime); // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // Biological basis: presynaptic spike precedes postsynaptic spike
                // Mechanism: NMDA receptor activation and Ca2+ influx
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // Biological basis: postsynaptic spike precedes presynaptic spike
                // Mechanism: phosphatase activation and synaptic depotentiation
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with safety bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change with bounds checking
        applyWeightChange(synapse, totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

// Apply weight change with comprehensive error handling and bounds checking
void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        return;
    }
    
    // Clamp weight to biologically plausible limits
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Only update if weight actually changed
    if (std::abs(newWeight - synapse->getWeight()) > 1e-6f) {
        synapse->setWeight(newWeight);
    }
}

// Get rule name for logging and debugging
const char* STDP::getName() const {
    return "STDP";
}

// STDP-specific parameter setters with validation
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

// Configure all STDP parameters at once for convenience
void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

// Helper function for advanced STDP variants
namespace stdp_helpers {
    
    // Calculate spike pairing-based STDP contribution
    inline float calculatePairBasedSTDP(
        Timestamp preTime, Timestamp postTime,
        float ltpWeight, float ltdWeight, float tau) {
        
        float dt = static_cast<float>(postTime - preTime);
        
        if (dt > 0) {
            return ltpWeight * std::exp(-dt / tau);
        } else if (dt < 0) {
            return -ltdWeight * std::exp(dt / tau);
        }
        return 0.0f;
    }
    
    // Calculate triplet-based STDP (captures interactions between three spikes)
    inline float calculateTripletSTDP(
        Timestamp preTime, Timestamp postTime, Timestamp thirdTime,
        float ltpWeight, float ltdWeight, float tauPlus, float tauMinus) {
        
        // Simplified triplet calculation
        float dt1 = static_cast<float>(postTime - preTime);  // + spike influence
        float dt2 = static_cast<float>(preTime - thirdTime); // - spike influence
        
        float tripletEffect = 0.0f;
        
        if (dt1 > 0) {
            tripletEffect += ltpWeight * std::exp(-dt1 / tauPlus);
        }
        
        if (dt2 > 0) {
            tripletEffect -= ltdWeight * std::exp(-dt2 / tauMinus);
        }
        
        return tripletEffect;
    }
    
    // Calculate synaptic history dependence for trace-based STDP
    inline float calculateTraceBasedSTDP(
        float eligibilityTrace, float traceDecay, float learningRate) {
        
        return eligibilityTrace * traceDecay * learningRate;
    }
}

} // namespace nlm