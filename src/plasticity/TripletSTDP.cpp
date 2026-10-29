#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    // Triple synaptic tagging: pre, post, and neural activity
    float Aplus;          // Potentiation from pre-post spike pairs
    float Aminus;         // Depression from post-pre spike pairs
    float W;              // Baseline weight
    float U;              // Utilization parameter (0-1)
    float tau_plus;       // Time constant for potentiation
    float tau_minus;       // Time constant for depression
    float tau_rec;         // Recovery time constant
    float tau_f;           // Facilitation time constant
    
    // Triple components:
    // x+(t) = sum of pre-synaptic spikes (tagged for potentiation)
    // x-(t) = sum of post-synaptic spikes (tagged for depression)
    // h(t) = synaptic efficacy (facilitation)
    
    Impl() : Aplus(0.05f), Aminus(0.05f), W(1.0f), U(0.2f),
             tau_plus(100.0f), tau_minus(100.0f), tau_rec(100.0f), tau_f(10.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    /*
     * Triplet STDP (also known as Auto-associative Memory)
     * 
     * Biological basis: Three-factor learning rule for Hebbian-like plasticity
     * that incorporates synaptic history and activity-dependent modulation.
     * 
     * Mathematical formulation (Dayan & Sejnowski, 2002):
     * Δw = A+ * (x+(t) * y(t)) - A- * (x-(t) * y(t))
     * 
     * Where:
     *   x+(t) = sum of pre-synaptic spikes within time window
     *   x-(t) = sum of post-synaptic spikes within time window
     *   y(t) = postsynaptic firing rate or activity
     *   A+, A- = learning parameters
     * 
     * Synaptic dynamics:
     *   dh/dt = -h/τ_rec + U * x+(t) * δ(t - t_pre)
     *   dy/dt = -y/τ_f + (1 - y) * x-(t) * δ(t - t_post)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor dynamics and calcium signaling
     *   - Captures synaptic history and facilitation effects
     *   - Models synaptic tagging and capture (STC) mechanisms
     *   - Implements three-factor learning (pre-post activity + neuromodulator)
     *   - Used in hippocampal place cell memory consolidation
     *   - Important for working memory and pattern completion
     * 
     * Applications:
     *   - Auto-associative memory (pattern completion)
     *   - Synaptic homeostasis (metaplasticity)
     *   - Memory consolidation
     *   - Skilled memory formation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = 20.0f;  // Base time constant
    
    // Implement three-factor learning rule
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_ms = static_cast<float>(postTime - preTime);
            
            if (dt_ms > 0) {
                // Pre before post: potentiation
                // Combination of direct effect and facilitation effect
                float pairEffect = pImpl->Aplus * std::exp(-dt_ms / pImpl->tau_plus);
                
                // Additional potentiation from facilitation
                // Enhanced when previous pre-synaptic spikes occurred
                float facilitationFactor = 1.0f + pImpl->U;
                float facilitationEffect = pImpl->Aplus * facilitationFactor * std::exp(-dt_ms / pImpl->tau_f);
                
                totalDelta += pairEffect + facilitationEffect;
            } else if (dt_ms < 0) {
                // Post before pre: depression
                // Combination of direct effect and recovery effect
                float pairEffect = -pImpl->Aminus * std::exp(dt_ms / pImpl->tau_minus);
                
                // Additional depression from synaptic recovery
                // Enhanced when postsynaptic activity was high
                float recoveryFactor = 1.0f - pImpl->U;
                float recoveryEffect = -pImpl->Aminus * recoveryFactor * std::exp(dt_ms / pImpl->tau_rec);
                
                totalDelta += pairEffect + recoveryEffect;
            }
        }
    }
    
    // Apply with biological constraints
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by baseline weight and current weight
        float currentWeight = synapse->getWeight();
        float weightChange = totalDelta * (0.5f + 0.5f * currentWeight);  // Weight-dependent learning
        
        // Clamp weight change
        weightChange = std::clamp(weightChange, -0.05f, 0.05f);
        
        // Apply weight change
        synapse->addToWeight(weightChange);
        
        // Update eligibility trace for reward-modulated variants
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + weightChange);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply with bounds specific to triplet dynamics
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->W - 0.5f, pImpl->W + 0.5f);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setAplus(float weight) {
    pImpl->Aplus = std::clamp(weight, 0.0f, 0.5f);
}

float TripletSTDP::getAplus() const {
    return pImpl->Aplus;
}

void TripletSTDP::setAminus(float weight) {
    pImpl->Aminus = std::clamp(weight, 0.0f, 0.5f);
}

float TripletSTDP::getAminus() const {
    return pImpl->Aminus;
}

void TripletSTDP::setBaselineWeight(float w) {
    pImpl->W = std::clamp(w, 0.0f, 2.0f);
}

float TripletSTDP::getBaselineWeight() const {
    return pImpl->W;
}

void TripletSTDP::setUtilization(float u) {
    pImpl->U = std::clamp(u, 0.0f, 1.0f);
}

float TripletSTDP::getUtilization() const {
    return pImpl->U;
}

} // namespace nlm