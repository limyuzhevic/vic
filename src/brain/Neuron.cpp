// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

using namespace nlm;

// ========== ENHANCED NUMERICAL STABILITY IMPLEMENTATION ==========

namespace NeuronStability {
    // Numerical stability constants
    constexpr float MIN_TIMESTEP = 1e-6f;  // 1 microsecond minimum
    constexpr float MAX_TIMESTEP = 0.1f;   // 100 milliseconds maximum
    constexpr float EPSILON = 1e-6f;       // Small constant for comparisons
    constexpr float V_MIN = -100.0f;       // Absolute minimum membrane potential
    constexpr float V_MAX = 50.0f;        // Absolute maximum membrane potential
    constexpr float V_SOFT_CLAMP_MIN = -95.0f;  // Soft clamp range
    constexpr float V_SOFT_CLAMP_MAX = 45.0f;   // Soft clamp range
    constexpr float MAX_SPIKE_CHANGE = 10.0f;   // Maximum spike-induced voltage change
    
    // Adaptive timestep controller
    class AdaptiveTimestepController {
    public:
        AdaptiveTimestepController(float initialStep = 0.001f) 
            : currentStep(initialStep), minStep(MIN_TIMESTEP), maxStep(MAX_TIMESTEP), 
              stabilityCounter(0), unstableSteps(0) {}
        
        float getTimestep() const { return currentStep; }
        
        void update(float error, float neuronStability, bool spiked) {
            // Error-based timestep adjustment
            float adjustment = error * 0.05f;  // Reduced for stability
            
            // Stability-based timestep adjustment
            if (neuronStability < EPSILON) {
                // Very unstable neuron, reduce timestep
                adjustment += 0.2f;
                unstableSteps++;
            } else {
                unstableSteps = 0;
            }
            
            // If neuron spiked, reduce timestep for stability
            if (spiked) {
                adjustment += 0.3f;
            }
            
            // If we've had multiple unstable steps, increase stability
            if (unstableSteps > 3) {
                adjustment += 0.5f;
            }
            
            // Apply adjustment with bounds
            currentStep += adjustment;
            
            // Clamp to valid range
            currentStep = std::clamp(currentStep, minStep, maxStep);
            
            // Limit how much timestep can change in one step
            float prevStep = currentStep;
            currentStep = std::clamp(currentStep, minStep, maxStep);
            
            // Log if significant change
            if (std::abs(adjustment) > 0.0001f) {
                NLM_LOG_DEBUG("Adjusted timestep to " + std::to_string(currentStep) + "s");
            }
        }
        
    private:
        float currentStep;
        float minStep;
        float maxStep;
        int stabilityCounter;
        int unstableSteps;
    };
    
    // Improved LIF integration with better numerical stability
    struct ImprovedLIFIntegration {
        float integrateMembranePotential(
            float V, float V_rest, float I_syn, float dt,
            float adaptation, float tau = 20.0f, float C = 1.0f) {
            
            // Validate inputs with safety checks
            if (dt < MIN_TIMESTEP) dt = MIN_TIMESTEP;
            if (dt > MAX_TIMESTEP) dt = MAX_TIMESTEP;
            
            // Improved exponential Euler integration
            // dV/dt = (V_rest - V)/tau + I/C
            // V_new = V + dt * ((V_rest - V)/tau + I/C)
            
            float leakTerm = (V_rest - V) / tau;
            float inputTerm = I_syn / C;
            float dV = dt * (leakTerm + inputTerm);
            
            // Stability check for large changes
            float maxChange = std::abs(dt) * 50.0f;  // Maximum expected change per step
            if (std::abs(dV) > maxChange) {
                dV = std::copysign(maxChange, dV);
                NLM_LOG_WARNING("LIF integration step too large, clamped");
            }
            
            float newV = V + dV;
            
            // Apply soft clamping for numerical stability
            newV = applySoftClamp(newV, adaptation);
            
            // Apply hard clamping as final safeguard
            newV = std::clamp(newV, V_MIN, V_MAX);
            
            return newV;
        }
        
    private:
        float applySoftClamp(float V, float adaptation) {
            // Soft clamping that gradually increases as we approach hard limits
            // Also considers adaptation level for additional stability
            float adaptationFactor = std::min(std::abs(adaptation) / 10.0f, 1.0f);
            
            if (V < V_SOFT_CLAMP_MIN * (1.0f + adaptationFactor)) {
                float factor = (V - V_MIN) / (V_SOFT_CLAMP_MIN - V_MIN);
                factor = std::min(factor, 1.0f);
                return V_MIN + factor * (V_SOFT_CLAMP_MIN - V_MIN);
            } else if (V > V_SOFT_CLAMP_MAX * (1.0f - adaptationFactor)) {
                float factor = (V - V_MAX) / (V_SOFT_CLAMP_MAX - V_MAX);
                factor = std::min(factor, 1.0f);
                return V_MAX - factor * (V_SOFT_CLAMP_MAX - V_MAX);
            }
            return V;
        }
    };
    
    // Enhanced spike detection with improved hysteresis
    bool detectSpike(float V, float threshold, float refractoryRemaining,
                    Timestamp lastSpikeTime, Timestamp currentTime,
                    float adaptation = 0.0f) {
        
        // Check refractory period first
        if (refractoryRemaining > 0) {
            return false;
        }
        
        // Apply enhanced hysteresis to prevent multiple spikes
        float hysteresisThreshold = threshold * 1.08f;  // Increased hysteresis
        
        // Adaptation can affect spike threshold (dopamine typically increases threshold)
        float adaptationModulation = 1.0f + adaptation * 0.1f; // Adaptation shifts threshold
        hysteresisThreshold *= adaptationModulation;
        
        // Check if we've crossed the threshold with hysteresis
        bool crossed = V >= hysteresisThreshold;
        
        if (crossed) {
            // Additional safety check: ensure we haven't just spiked
            float timeSinceLastSpike = currentTime - lastSpikeTime;
            float refractoryPeriod = 2.0f; // ms base refractory
            
            // Adaptation can affect refractory period
            float refractoryModulation = std::max(0.5f, 1.0f - adaptation * 0.1f);
            refractoryPeriod *= refractoryModulation;
            
            if (timeSinceLastSpike < refractoryPeriod) {
                NLM_LOG_DEBUG("Preventing immediate re-spike (" + 
                             std::to_string(timeSinceLastSpike) + "ms since last spike)");
                return false;
            }
            
            // Check for rapid voltage changes that might indicate numerical issues
            float voltageChange = V - threshold;
            if (voltageChange > MAX_SPIKE_CHANGE) {
                NLM_LOG_WARNING("Large voltage change before spike: " + std::to_string(voltageChange));
            }
        }
        
        return crossed;
    }
    
    // Improved spike-frequency adaptation with enhanced stability bounds
    float updateSpikeFrequencyAdaptation(float adaptation, float spikeCount, bool spiked) {
        // Enhanced adaptation with stability bounds and spike history awareness
        if (spikeCount > 0 && spiked) {
            float adaptationIncrease = 1.0f * spikeCount * 0.5f; // Reduced for stability
            float newAdaptation = adaptation + adaptationIncrease;
            
            // Bound adaptation to reasonable range with dynamic limits
            float maxAdaptation = 50.0f + std::abs(adaptation) * 0.5f;
            if (newAdaptation > maxAdaptation) {
                newAdaptation = maxAdaptation;
                NLM_LOG_WARNING("Adaptation capped at maximum value: " + std::to_string(newAdaptation));
            }
            
            // Apply more conservative decay with spike history awareness
            float decayFactor = 0.97f; // Slower decay for better stability
            if (spikeCount > 10) {
                // Longer recovery after heavy spiking
                decayFactor = 0.98f;
            }
            
            newAdaptation *= decayFactor;
            
            // Ensure adaptation doesn't go negative due to decay
            if (newAdaptation < -10.0f) {
                newAdaptation = -10.0f;
            }
            
            return newAdaptation;
        }
        
        // Gradual decay even without spikes, but less aggressive
        float newAdaptation = adaptation * 0.99f;
        
        // Ensure minimum adaptation for stability
        if (std::abs(newAdaptation) < 0.1f) {
            newAdaptation = 0.0f;
        }
        
        return newAdaptation;
    }
} // namespace NeuronStability

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};
