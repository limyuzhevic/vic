#include "AsynchronousSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct AsynchronousSTDP::Impl {
    float learningRate;     // Global learning rate (A+)
    float decayRate;        // Trace decay rate per timestep
    float timeConstant;     // Characteristic time constant for exponential decay (ms)
    float maxTrace;         // Maximum eligibility trace value to prevent runaway potentiation
    float traceDecay;       // Current decay factor for eligibility trace
    
    // Implementation details
    bool initialized;       // Whether implementation is ready
    
    Impl() : learningRate(0.01f), decayRate(0.001f), timeConstant(50.0f),
             maxTrace(10.0f), traceDecay(1.0f), initialized(false) {}
};

AsynchronousSTDP::AsynchronousSTDP() : pImpl(new Impl) {}

AsynchronousSTDP::~AsynchronousSTDP() = default;

void AsynchronousSTDP::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // For each pre-synaptic spike
    for (Timestamp preTime : preSpikes) {
        float trace = 0.0f;
        
        // Calculate eligibility trace for this pre-synaptic spike
        // Trace represents the "potential" for future potentiation
        trace = pImpl->learningRate * std::exp(-(pImpl->timeConstant));
        
        // Update eligibility trace based on post-synaptic spikes
        for (Timestamp postTime : postSpikes) {
            float delta_t = static_cast<float>(postTime - preTime);
            
            if (delta_t > 0) {
                // Pre before post - positive contribution (potentiation)
                trace *= std::exp(-delta_t / pImpl->timeConstant);
                totalDelta += trace;
            } else {
                // Post before pre - negative contribution (depression)
                // For now, we focus on delayed spikes effect, negative dt is not implemented
                // This could be added for comprehensive delayed spike handling
            }
        }
    }
    
    // Apply weight change with bounds and eligibility trace
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Clamp eligibility trace and apply weight change
        totalDelta = std::clamp(totalDelta, -pImpl->maxTrace, pImpl->maxTrace);
        applyWeightChange(synapse, totalDelta);
    }
}

void AsynchronousSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight and calculate new weight with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);  // Standard weight bounds
    synapse->setWeight(newWeight);
}

const char* AsynchronousSTDP::getName() const {
    return "AsynchronousSTDP";
}

void AsynchronousSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float AsynchronousSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void AsynchronousSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 200.0f);
}

float AsynchronousSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void AsynchronousSTDP::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.0001f, 0.01f);
}

float AsynchronousSTDP::getDecayRate() const {
    return pImpl->decayRate;
}

void AsynchronousSTDP::setMaxTrace(float max) {
    pImpl->maxTrace = std::clamp(max, 1.0f, 100.0f);
}

float AsynchronousSTDP::getMaxTrace() const {
    return pImpl->maxTrace;
}

void AsynchronousSTDP::updateEligibilityTrace(float trace, Timestamp spikeTime, Timestamp currentTime) {
    // Update eligibility trace with spike information
    // This method can be called from other parts of the system to update
    // the trace based on actual spike times
    pImpl->traceDecay = std::exp(-(currentTime - spikeTime) / pImpl->timeConstant);
}

float AsynchronousSTDP::getEligibilityTrace() const {
    // Return current eligibility trace state
    // This is a simplified implementation
    return pImpl->traceDecay;
}

} // namespace nlm