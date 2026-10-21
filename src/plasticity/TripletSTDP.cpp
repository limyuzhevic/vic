#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    // Basic STDP parameters
    float ltpWeight;         // Potentiation weight (A+)
    float ltdWeight;         // Depression weight (A-)
    
    // Time constants
    float timeConstant;      // Primary time constant (tau) for standard STDP (ms)
    float secondaryTimeConstant;  // Secondary time constant (tau2) for triplet traces (ms)
    
    // Trace management
    float maxTrace;          // Maximum trace value to prevent runaway potentiation
    float preTrace;          // Eligibility trace for pre-synaptic spikes
    float postTrace;         // Eligibility trace for post-synaptic spikes
    
    // Decay factors
    float preTraceDecay;     // Decay factor for pre-trace
    float postTraceDecay;    // Decay factor for post-trace
    
    // Previous spike tracking
    Timestamp lastPreSpike;  // Time of last pre-synaptic spike
    Timestamp lastPostSpike; // Time of last post-synaptic spike
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             secondaryTimeConstant(100.0f), maxTrace(10.0f), preTrace(0.0f),
             postTrace(0.0f), preTraceDecay(1.0f), postTraceDecay(1.0f),
             lastPreSpike(0.0), lastPostSpike(0.0) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // Update traces for each pre-synaptic spike
    for (Timestamp preTime : preSpikes) {
        // Update pre-trace with new spike
        pImpl->preTrace = pImpl->ltpWeight * std::exp(-(preTime - pImpl->lastPreSpike) / pImpl->timeConstant);
        pImpl->lastPreSpike = preTime;
        
        // Integrate post-trace contribution
        for (Timestamp postTime : postSpikes) {
            float delta_t = static_cast<float>(postTime - preTime);
            
            if (delta_t > 0) {
                // Pre before post - potentiation
                // Include triplet contribution from post-synaptic trace
                float traceContribution = pImpl->postTrace * std::exp(-delta_t / pImpl->secondaryTimeConstant);
                float delta = pImpl->ltpWeight * std::exp(-delta_t / pImpl->timeConstant) + traceContribution;
                totalDelta += delta;
            } else {
                // Post before pre - depression
                // Include triplet contribution from pre-synaptic trace
                float traceContribution = pImpl->preTrace * std::exp(delta_t / pImpl->secondaryTimeConstant);
                float delta = -pImpl->ltdWeight * std::exp(delta_t / pImpl->timeConstant) + traceContribution;
                totalDelta += delta;
            }
        }
    }
    
    // Update post-traces for post-synaptic spikes
    for (Timestamp postTime : postSpikes) {
        // Update post-trace with new spike
        pImpl->postTrace = pImpl->ltpWeight * std::exp(-(postTime - pImpl->lastPostSpike) / pImpl->timeConstant);
        pImpl->lastPostSpike = postTime;
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Clamp delta and apply weight change
        totalDelta = std::clamp(totalDelta, -pImpl->maxTrace, pImpl->maxTrace);
        applyWeightChange(synapse, totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current weight and calculate new weight with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);  // Standard weight bounds
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void TripletSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void TripletSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void TripletSTDP::setSecondaryTimeConstant(float tau2) {
    pImpl->secondaryTimeConstant = std::clamp(tau2, 10.0f, 500.0f);
}

float TripletSTDP::getSecondaryTimeConstant() const {
    return pImpl->secondaryTimeConstant;
}

void TripletSTDP::setMaxTrace(float max) {
    pImpl->maxTrace = std::clamp(max, 1.0f, 100.0f);
}

float TripletSTDP::getMaxTrace() const {
    return pImpl->maxTrace;
}

void TripletSTDP::updatePreTrace(float trace, Timestamp spikeTime, Timestamp currentTime) {
    // Update pre-trace with external spike information
    pImpl->preTrace = trace * std::exp(-(currentTime - spikeTime) / pImpl->timeConstant);
}

void TripletSTDP::updatePostTrace(float trace, Timestamp spikeTime, Timestamp currentTime) {
    // Update post-trace with external spike information
    pImpl->postTrace = trace * std::exp(-(currentTime - spikeTime) / pImpl->timeConstant);
}

float TripletSTDP::getPreTrace() const {
    return pImpl->preTrace;
}

float TripletSTDP::getPostTrace() const {
    return pImpl->postTrace;
}

} // namespace nlm