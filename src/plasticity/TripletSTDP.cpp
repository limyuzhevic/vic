#include "../../plasticity/STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float A_plus1;     // First-order LTP coefficient
    float A_plus2;     // Second-order LTP coefficient
    float A_minus1;    // First-order LTD coefficient
    float A_minus2;    // Second-order LTD coefficient
    float tau_plus;    // Potentiation time constant
    float tau_minus;   // Depression time constant
    
    Impl() : A_plus1(0.01f), A_plus2(0.005f), A_minus1(0.012f), A_minus2(0.004f),
             tau_plus(20.0f), tau_minus(20.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    // Triplet STDP with presynaptic and postsynaptic spike history
    // Extends basic STDP by including contributions from spike triplets
    //
    // Mathematical formulation:
    // Δw = ∑_i ∑_j [A1+ * x_i * y_j * exp(-(t_j-t_i)/τ+) 
    //                + A2+ * x_i * y_k * exp(-(t_k-t_i)/τ+)
    //                + A1- * y_i * x_j * exp(-(t_j-t_i)/τ-)
    //                + A2- * y_i * x_k * exp(-(t_k-t_i)/τ-)]
    //
    // Where:
    //   x_i, x_j, x_k = presynaptic spike activity (binary)
    //   y_i, y_j, y_k = postsynaptic spike activity (binary)
    //   τ+, τ- = time constants for potentiation and depression
    //
    // Biological inspiration:
    //   - Presynaptic trace for facilitating effects
    //   - Postsynaptic trace for depression
    //   - Triplet interactions in cortical synapses
    //   - More accurate to experimental data in some systems
    //
    // Limitations:
    //   - Simplified spike history modeling
    //   - Assumes binary spike generation
    //   - Doesn't capture complex spike patterns
    //
    
    if (preSpikes.empty() || postSpikes.empty() || !synapse) {
        return;
    }
    
    // Calculate presynaptic and postsynaptic traces
    // Simplified: assume traces decay exponentially from recent spikes
    float preTrace = 0.0f;
    float postTrace = 0.0f;
    
    float currentTime = 1000.0f;  // Assume current time
    
    // Calculate traces based on recent spike activity
    for (Timestamp t : preSpikes) {
        float age = currentTime - t;
        preTrace += std::exp(-age / pImpl->tau_plus);
    }
    
    for (Timestamp t : postSpikes) {
        float age = currentTime - t;
        postTrace += std::exp(-age / pImpl->tau_minus);
    }
    
    // Calculate triplet contributions
    float totalDelta = 0.0f;
    
    // First-order interactions (basic STDP)
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (dt > 0) {
                // Pre before post: potentiation
                totalDelta += pImpl->A_plus1 * std::exp(-dt / pImpl->tau_plus);
            } else if (dt < 0) {
                // Post before pre: depression
                totalDelta += pImpl->A_minus1 * std::exp(dt / pImpl->tau_minus);
            }
        }
    }
    
    // Second-order interactions (triplet effects)
    // Simplified: assume traces enhance/depress basic STDP
    if (preTrace > 0.01f && postTrace > 0.01f) {
        // Both presynaptic and postsynaptic traces present
        // Enhances basic STDP effects
        totalDelta *= (1.0f + preTrace * postTrace * 0.1f);
    }
    
    // Apply weight change
    applyWeightChange(synapse, totalDelta);
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Triplet STDP typically operates within standard bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setA1Plus(float A1) {
    pImpl->A_plus1 = std::clamp(A1, 0.0f, 0.1f);
}

float TripletSTDP::getA1Plus() const {
    return pImpl->A_plus1;
}

void TripletSTDP::setA2Plus(float A2) {
    pImpl->A_plus2 = std::clamp(A2, 0.0f, 0.1f);
}

float TripletSTDP::getA2Plus() const {
    return pImpl->A_plus2;
}

void TripletSTDP::setA1Minus(float A1) {
    pImpl->A_minus1 = std::clamp(A1, 0.0f, 0.1f);
}

float TripletSTDP::getA1Minus() const {
    return pImpl->A_minus1;
}

void TripletSTDP::setA2Minus(float A2) {
    pImpl->A_minus2 = std::clamp(A2, 0.0f, 0.1f);
}

float TripletSTDP::getA2Minus() const {
    return pImpl->A_minus2;
}

void TripletSTDP::setTimeConstants(float tauPlus, float tauMinus) {
    pImpl->tau_plus = std::clamp(tauPlus, 1.0f, 100.0f);
    pImpl->tau_minus = std::clamp(tauMinus, 1.0f, 100.0f);
}

void TripletSTDP::getTimeConstants(float& tauPlus, float& tauMinus) const {
    tauPlus = pImpl->tau_plus;
    tauMinus = pImpl->tau_minus;
}

} // namespace nlm
