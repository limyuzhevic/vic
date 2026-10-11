#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float a1;          // First trace amplitude (pre-dependent)
    float a2;          // Second trace amplitude (post-dependent)
    float a3;          // Third trace amplitude (interaction term)
    float tau1;        // First trace time constant
    float tau2;        // Second trace time constant
    float theta;        // Threshold for triplet interactions
    float minWeight;    // Minimum synaptic weight
    float maxWeight;    // Maximum synaptic weight
    
    // Trace variables
    float x1;          // First trace (pre-spike
    float x2;          // Second trace (post-spike)
    float x3;          // Third trace (interaction)
    
    Impl() : a1(0.005f), a2(0.005f), a3(0.0005f), tau1(20.0f), tau2(100.0f),
             theta(0.0f), minWeight(-1.0f), maxWeight(1.0f),
             x1(0.0f), x2(0.0f), x3(0.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    /*
     * Triplet STDP implementation
     * 
     * Extends standard STDP with triplet interactions:
     * Δw = A+ * [x1(t) - θ] * postSpike - A- * post * x2(t)
     * 
     * Where:
     *   x1(t) = trace of pre-spikes (decays exponentially)
     *   x2(t) = trace of post-spikes (decays exponentially)
     *   x3(t) = interaction term (trace of paired interactions)
     *   A+ = a1 + a2 * x2(t) + a3 * x3(t) (potentiation amplitude)
     *   A- = a1 * pre(t) (depression amplitude)
     *   θ = threshold for LTP/LTD switching
     * 
     * Biological inspiration:
     *   - Captures short-term synaptic dynamics
     *   - Accounts for synaptic eligibility traces
     *   - Models NMDA receptor cooperativity
     *   - Reflects calcium dynamics in dendritic compartments
     *   
     * Implementation details:
     *   - Maintains three traces for different timescales
     *   - Updates traces on each spike
     *   - Computes triplet contributions
     *   - Applies weight change with bounds
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update traces based on spike activity
    Timestamp currentTime = postSpikes.back();  // Use most recent spike time
    
    // Decay all traces
    pImpl->x1 *= std::exp(-dt / pImpl->tau1);
    pImpl->x2 *= std::exp(-dt / pImpl->tau2);
    pImpl->x3 *= std::exp(-dt / 20.0f);  // Fixed decay for interaction term
    
    // Add contributions from recent spikes
    for (Timestamp preTime : preSpikes) {
        float delta = static_cast<float>(currentTime - preTime);
        if (delta > 0 && delta < 1000.0f) {
            pImpl->x1 += std::exp(-delta / pImpl->tau1);
            pImpl->x3 += std::exp(-delta / 20.0f);
        }
    }
    
    for (Timestamp postTime : postSpikes) {
        float delta = static_cast<float>(currentTime - postTime);
        if (delta > 0 && delta < 1000.0f) {
            pImpl->x2 += std::exp(-delta / pImpl->tau2);
            pImpl->x3 += std::exp(-delta / 20.0f);
        }
    }
    
    // Calculate LTP contribution from x1 and theta
    float ltpContribution = pImpl->a1 * (pImpl->x1 - pImpl->theta);
    for (Timestamp postTime : postSpikes) {
        float timeSincePost = static_cast<float>(currentTime - postTime);
        if (timeSincePost > 0 && timeSincePost < 1000.0f) {
            ltpContribution += (pImpl->a2 * pImpl->x2 + pImpl->a3 * pImpl->x3) * 
                              std::exp(-timeSincePost / 20.0f);
        }
    }
    
    // Calculate LTD contribution
    float ltdContribution = 0.0f;
    for (Timestamp preTime : preSpikes) {
        float timeSincePre = static_cast<float>(currentTime - preTime);
        if (timeSincePre > 0 && timeSincePre < 1000.0f) {
            ltdContribution += pImpl->a1 * std::exp(-timeSincePre / pImpl->tau1);
        }
    }
    
    // Apply weight change
    float totalDelta = ltpContribution - ltdContribution;
    
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        synapse->addToWeight(totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Standard weight application with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setA1(float weight) {
    pImpl->a1 = std::clamp(weight, 0.0f, 0.01f);
}

float TripletSTDP::getA1() const {
    return pImpl->a1;
}

void TripletSTDP::setA2(float weight) {
    pImpl->a2 = std::clamp(weight, 0.0f, 0.01f);
}

float TripletSTDP::getA2() const {
    return pImpl->a2;
}

void TripletSTDP::setA3(float weight) {
    pImpl->a3 = std::clamp(weight, 0.0f, 0.001f);
}

float TripletSTDP::getA3() const {
    return pImpl->a3;
}

void TripletSTDP::setTau1(float tau) {
    pImpl->tau1 = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTau1() const {
    return pImpl->tau1;
}

void TripletSTDP::setTau2(float tau) {
    pImpl->tau2 = std::clamp(tau, 1.0f, 500.0f);
}

float TripletSTDP::getTau2() const {
    return pImpl->tau2;
}

void TripletSTDP::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, -1.0f, 1.0f);
}

float TripletSTDP::getTheta() const {
    return pImpl->theta;
}

void TripletSTDP::configure(float a1, float a2, float a3, float tau1, float tau2) {
    setA1(a1);
    setA2(a2);
    setA3(a3);
    setTau1(tau1);
    setTau2(tau2);
}

} // namespace nlm
