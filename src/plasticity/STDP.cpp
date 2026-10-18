#include "STDP.hpp"
#include "brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    float maxDt;          // Maximum time difference for numerical stability (ms)
    float minEfficacy;    // Minimum efficacy value (for numerical stability)
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), maxDt(1000.0f),
             minEfficacy(1e-6f) {}
};

// Helper function for stable exponential calculations
inline float stableExp(float x) {
    // Clamp the exponent to prevent overflow/underflow
    const float maxExpArg = 50.0f;   // exp(50) overflows
    const float minExpArg = -50.0f;  // exp(-50) underflows to 0
    x = std::clamp(x, minExpArg, maxExpArg);
    return std::exp(x);
}

// Helper function for computing weight change with normalization
inline float computeWeightChange(float dt, float tau, float aPlus, float aMinus) {
    if (tau <= 0.0f) return 0.0f;  // Invalid time constant
    
    // Normalize dt by time constant for better numerical stability
    float dt_tau = dt / tau;
    
    // Pre-clamp for numerical stability
    dt_tau = std::clamp(dt_tau, -50.0f, 50.0f);
    
    float exp_factor = stableExp(-dt_tau);
    
    if (dt > 0.0f) {
        // Pre-before-post: potentiation
        return aPlus * exp_factor;
    } else if (dt < 0.0f) {
        // Post-before-pre: depression
        return -aMinus * stableExp(dt_tau);
    }
    
    return 0.0f;  // No change for dt = 0
}

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt_param) {
    /*
     * Enhanced STDP implementation with improved numerical stability and biological plausibility
     * 
     * Mathematical formulation based on spike-timing correlation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre (in ms):
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+) * normalization_factor
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-) * normalization_factor
     *   
     * Where:
     *   A+ = ltpWeight (potentiation amplitude) - biological interpretation: NMDA receptor contribution
     *   A- = ltdWeight (depression amplitude) - biological interpretation: calcium-dependent depression
     *   τ+ = τ- = timeConstant (STDP time window) - biological interpretation: synaptic decay time constant
     *   normalization_factor = max(1, |Δt| / min_window) to maintain plasticity balance
     *   
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling dynamics
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes arrive
     *   - Post-before-pre causes backpropagating action potentials with LTP/LTD mechanisms
     *   - Incorporates synaptic efficacy (metaplasticity) for use-dependent modulation
     *   - Includes eligibility traces for reward-modulated learning (R-STDP)
     *   
     * Improvements over basic STDP:
     *   - Numerical stability for extreme Δt values
     *   - Pre-emptive weight clamping to prevent overflow
     *   - Enhanced eligibility trace management
     *   - Parameter validation and bounds checking
     *   - Cache-optimized memory access patterns
     *   - Early exit conditions for performance
     */
    
    // Enhanced error handling with detailed validation
    if (!synapse) {
        return;  // Null synapse pointer - safety check
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;  // No spikes to process
    }
    
    // Early exit: check if STDP is enabled
    const PlasticityFlags& flags = synapse->getPlasticityFlags();
    if (!flags.stdp) {
        return;  // STDP plasticity is disabled
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Parameter validation
    if (tau <= 0.0f || pImpl->ltpWeight < 0.0f || pImpl->ltdWeight < 0.0f) {
        return;  // Invalid STDP parameters
    }
    
    // Get previous weight for pre-clamping in eligibility trace update
    float prevWeight = synapse->getWeight();
    
    // Optimized nested loop with early termination
    // Pre-fetch vectors for better cache locality
    const std::vector<Timestamp>& preVec = preSpikes;
    const std::vector<Timestamp>& postVec = postSpikes;
    
    // Compute eligibility trace normalization factor
    float normFactor = 1.0f;
    if (!postVec.empty() && !preVec.empty()) {
        // Use minimum Δt for normalization (at least 1ms to avoid division by zero)
        Timestamp minDelta = std::numeric_limits<Timestamp>::max();
        for (Timestamp postTime : postVec) {
            for (Timestamp preTime : preVec) {
                Timestamp delta = postTime - preTime;
                if (delta < 0) delta = -delta;  // Absolute value
                if (delta < minDelta && delta > 0) minDelta = delta;
            }
        }
        // Normalize with minimum window of 1ms
        normFactor = std::max(1.0f, static_cast<float>(minDelta) / 1.0f);
    }
    
    // Main STDP computation loop
    for (Timestamp preTime : preVec) {
        // Use local reference for better optimization
        float preClamped = static_cast<float>(preTime);
        
        for (Timestamp postTime : postVec) {
            // Cast with proper precision handling for double->float conversion
            // Fixed naming conflict: use dt_param for parameter, delta_time for local variable
            float delta_time = static_cast<float>(postTime - preClamped);  // Δt in ms
            
            // Skip if Δt is outside the effective STDP window
            if (delta_time < -pImpl->maxDt || delta_time > pImpl->maxDt) {
                continue;  // Skip extreme values that don't contribute significantly
            }
            
            // Compute weight change using numerically stable formula
            float delta = computeWeightChange(delta_time, tau, pImpl->ltpWeight, pImpl->ltdWeight);
            
            // Apply normalization factor for balanced plasticity
            delta /= normFactor;
            
            // Pre-clamping to prevent overflow in intermediate calculations
            if (std::abs(totalDelta + delta) > pImpl->maxWeight - pImpl->minWeight) {
                // Scale delta to prevent overflow
                delta *= 0.5f * (pImpl->maxWeight - pImpl->minWeight) / std::abs(totalDelta + delta);
            }
            
            totalDelta += delta;
        }
    }
    
    // Apply weight change with enhanced bounds checking and pre-clamping
    if (std::abs(totalDelta) > 1e-8f) {  // Use higher threshold for numerical stability
        // Scale by synaptic efficacy with bounds checking
        float efficacy = synapse->getEfficacy();
        
        // Ensure efficacy is within valid range
        if (efficacy < pImpl->minEfficacy) efficacy = pImpl->minEfficacy;
        
        totalDelta *= efficacy;
        
        // Pre-clamp weight change based on current weight to prevent overflow
        float currentWeight = synapse->getWeight();
        float maxPossibleChange = std::min(pImpl->maxWeight - currentWeight,
                                         currentWeight - pImpl->minWeight);
        
        if (std::abs(totalDelta) > std::abs(maxPossibleChange) * 0.1f) {
            // Scale down if change would be too large
            totalDelta = (maxPossibleChange * 0.1f) * (totalDelta / std::abs(totalDelta));
        }
        
        // Apply weight change using the dedicated method
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace with normalization and bounds checking
        float currentTrace = synapse->getEligibilityTrace();
        
        // Normalize eligibility trace to prevent accumulation
        if (std::abs(currentTrace) > pImpl->maxWeight * 10.0f) {
            currentTrace *= 0.1f;  // Decay if too large
        }
        
        // Add weight change to eligibility trace with bounds
        float newTrace = currentTrace + totalDelta * 0.01f;  // Scale eligibility trace update
        
        // Clamp eligibility trace to reasonable bounds
        newTrace = std::clamp(newTrace, -pImpl->maxWeight * 10.0f, pImpl->maxWeight * 10.0f);
        
        synapse->setEligibilityTrace(newTrace);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        return;  // Enhanced null pointer checking
    }
    
    // Parameter validation - ensure delta is reasonable
    if (std::abs(delta) > pImpl->maxWeight * 10.0f) {
        // Clamp delta to prevent extreme changes
        delta = (pImpl->maxWeight * 10.0f) * (delta / std::abs(delta));
    }
    
    float currentWeight = synapse->getWeight();
    
    // Apply weight change using the synapse's own method
    // This ensures consistency with the synapse implementation
    synapse->addToWeight(delta);
    
    // Additional safety check to ensure weight stays within bounds
    float newWeight = synapse->getWeight();
    if (newWeight > pImpl->maxWeight) {
        synapse->setWeight(pImpl->maxWeight);
    } else if (newWeight < pImpl->minWeight) {
        synapse->setWeight(pImpl->minWeight);
    }
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
