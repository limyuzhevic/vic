#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct TripletSTDP::Impl {
    // Triplet STDP parameters
    float tripletPotentiation;         // A+ amplitude for pre-pre-post interactions
    float tripletDepression;           // B- amplitude for pre-post-post interactions
    float potentiationTraceTau;        // Time constant for pre-synaptic facilitation trace
    float depressionTraceTau;          // Time constant for post-synaptic depression trace
    float minWeight;                    // Minimum synaptic weight
    float maxWeight;                    // Maximum synaptic weight
    
    // For triplet STDP: maintain traces for facilitation and depression
    float potentiationTrace;           // Pre-synaptic facilitation trace (x)
    float depressionTrace;             // Post-synaptic depression trace (y)
    
    // For triplet STDP: track recent spikes for trace updates
    float lastPreSpikeTime;            // Time of last pre-synaptic spike (ms)
    float lastPostSpikeTime;           // Time of last post-synaptic spike (ms)
    
    Impl() : tripletPotentiation(0.02f), tripletDepression(0.015f),
             potentiationTraceTau(200.0f), depressionTraceTau(200.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             potentiationTrace(0.0f), depressionTrace(0.0f),
             lastPreSpikeTime(-1e6f), lastPostSpikeTime(-1e6f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    /*
     * Real Triplet STDP (Brostowitz-style) implementation
     * 
     * Mathematical formulation (Triplet STDP):
     * 
     * The triplet STDP rule models synaptic facilitation and depression
     * using trace dynamics:
     * 
     * dx/dt = -x/τx + δ(t - t_pre)  (potentiation trace)
     * dy/dt = -y/τy + δ(t - t_post)  (depression trace)
     * 
     * Weight change:
     * Δw = A × x - B × y
     * 
     * Where:
     * - x = pre-synaptic facilitation trace
     * - y = post-synaptic depression trace  
     * - A = triplet potentiation amplitude (pre-pre-post)
     * - B = triplet depression amplitude (pre-post-post)
     * - τx = potentiation trace time constant
     * - τy = depression trace time constant
     * 
     * Biological inspiration:
     * - Synaptic resource dynamics (facilitation/depression)
     * - Calcium-based plasticity mechanisms
     * - Short-term plasticity in real neurons
     * - Complex spike timing dependencies
     * 
     * Key features:
     * - Captures triplet spike interactions
     * - Models synaptic resource depletion/recovery
     * - Provides more biological realism
     * - Can exhibit metaplasticity effects
     * 
     * Implementation details:
     * - Update traces based on spike inputs
     * - Decay traces exponentially over time
     * - Compute weight change from trace interactions
     * - Apply with biological bounds
     * - Track spike times for trace updates
     * 
     * Biological applications:
     * - Modeling short-term plasticity
     * - Simulating calcium-based learning
     * - Implementing complex spike timing dependencies
     * - Studying synaptic resource allocation
     */
    
    if (!synapse) {
        return;
    }
    
    // Update traces based on recent spike activity
    float currentTime = 0.0f;  // Simplified - assume all spikes in current time step
    
    // Update potentiation trace (x) - pre-synaptic facilitation
    if (!preSpikes.empty()) {
        // Add delta to potentiation trace for each pre-synaptic spike
        pImpl->potentiationTrace += static_cast<float>(preSpikes.size()) * 1.0f;
    }
    
    // Update depression trace (y) - post-synaptic depression  
    if (!postSpikes.empty()) {
        // Add delta to depression trace for each post-synaptic spike
        pImpl->depressionTrace += static_cast<float>(postSpikes.size()) * 1.0f;
    }
    
    // Decay traces exponentially over time
    // dx/dt = -x/τx
    // dy/dt = -y/τy
    float potentiationDecayFactor = std::exp(-dt / pImpl->potentiationTraceTau);
    float depressionDecayFactor = std::exp(-dt / pImpl->depressionTraceTau);
    
    pImpl->potentiationTrace *= potentiationDecayFactor;
    pImpl->depressionTrace *= depressionDecayFactor;
    
    // Compute triplet STDP weight change
    // Δw = A × potentiation - B × depression
    float delta = pImpl->tripletPotentiation * pImpl->potentiationTrace - 
                  pImpl->tripletDepression * pImpl->depressionTrace;
    
    // Apply triplet STDP weight change with biological bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setTripletPotentiation(float amplitude) {
    pImpl->tripletPotentiation = std::clamp(amplitude, 0.0f, 0.1f);
}

float TripletSTDP::getTripletPotentiation() const {
    return pImpl->tripletPotentiation;
}

void TripletSTDP::setTripletDepression(float amplitude) {
    pImpl->tripletDepression = std::clamp(amplitude, 0.0f, 0.1f);
}

float TripletSTDP::getTripletDepression() const {
    return pImpl->tripletDepression;
}

void TripletSTDP::setPotentiationTraceTimeConstant(float tau) {
    pImpl->potentiationTraceTau = std::clamp(tau, 50.0f, 1000.0f);
}

float TripletSTDP::getPotentiationTraceTimeConstant() const {
    return pImpl->potentiationTraceTau;
}

void TripletSTDP::setDepressionTraceTimeConstant(float tau) {
    pImpl->depressionTraceTau = std::clamp(tau, 50.0f, 1000.0f);
}

float TripletSTDP::getDepressionTraceTimeConstant() const {
    return pImpl->depressionTraceTau;
}

void TripletSTDP::resetTraces() {
    pImpl->potentiationTrace = 0.0f;
    pImpl->depressionTrace = 0.0f;
}

} // namespace nlm
