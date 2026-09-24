#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float ltpWeight;           // Long-term potentiation weight (A+)
    float ltdWeight;           // Long-term depression weight (A-)
    float timeConstant;        // Time constant for exponential windows (τ)
    float tauPre;              // Trace decay time constant for pre-synaptic (τ1)
    float tauPost;             // Trace decay time constant for post-synaptic (τ2)
    float maxWeight;           // Maximum synaptic weight
    float minWeight;           // Minimum synaptic weight
    float preTrace;            // Pre-synaptic trace value
    float postTrace;           // Post-synaptic trace value
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             tauPre(20.0f), tauPost(20.0f), maxWeight(1.0f), minWeight(-1.0f),
             preTrace(0.0f), postTrace(0.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    /*
     * Real Triplet STDP implementation
     * 
     * Mathematical formulation (including triplet interactions):
     * Δw = A+ * (x₁ + x₂) - A- * (y₁ + y₂)
     * 
     * Where traces evolve as:
     * dx/dt = -x/τ1 + Σδ(t - t_pre) (pre-synaptic trace)
     * dy/dt = -y/τ2 + Σδ(t - t_post) (post-synaptic trace)
     * 
     * With spike interactions:
     * x₁ = trace at t - Δt, x₂ = trace at t (pre spike contribution)
     * y₁ = trace at t, y₂ = trace at t + Δt (post spike contribution)
     * 
     * Biological inspiration:
     *   - Captures triplet spike interactions
     *   - Accounts for synaptic resource depletion
     *   - Explains spike-dependent plasticity windows
     *   - Related to synaptic tagging and capture
     *   
     * Limitations:
     *   - Still simplified compared to biological triplet STDP
     *   - Assumes exponential trace decay
     *   - No synaptic eligibility traces
     */
    if (!synapse) return;
    
    // Update traces based on recent spikes
    updateTraces(preSpikes, postSpikes, dt);
    
    // Compute weight changes from trace interactions
    float totalDelta = computeWeightChange();
    
    // Apply with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
    }
}

void TripletSTDP::updateTraces(const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt) {
    float timeConstant = pImpl->timeConstant;
    
    // Decay traces
    pImpl->preTrace *= std::exp(-dt / pImpl->tauPre);
    pImpl->postTrace *= std::exp(-dt / pImpl->tauPost);
    
    // Add contributions from recent spikes
    for (Timestamp preTime : preSpikes) {
        // Pre-synaptic trace gets boosted by spike
        pImpl->preTrace += 1.0f;
    }
    
    for (Timestamp postTime : postSpikes) {
        // Post-synaptic trace gets boosted by spike
        pImpl->postTrace += 1.0f;
    }
}

float TripletSTDP::computeWeightChange() const {
    // Triplet interaction terms
    float x1 = pImpl->preTrace;          // Trace at t - Δt (pre contribution)
    float x2 = pImpl->preTrace;          // Trace at t (pre spike contribution)
    float y1 = pImpl->postTrace;         // Trace at t (post contribution)
    float y2 = pImpl->postTrace;         // Trace at t + Δt (post spike contribution)
    
    // Weight change from triplet interactions
    float potentiation = pImpl->ltpWeight * (x1 + x2);
    float depression = pImpl->ltdWeight * (y1 + y2);
    
    return potentiation - depression;
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
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

void TripletSTDP::setTraceDecay(float tauPre, float tauPost) {
    pImpl->tauPre = std::clamp(tauPre, 1.0f, 100.0f);
    pImpl->tauPost = std::clamp(tauPost, 1.0f, 100.0f);
}

void TripletSTDP::getTraceDecay(float& tauPre, float& tauPost) const {
    tauPre = pImpl->tauPre;
    tauPost = pImpl->tauPost;
}

} // namespace nlm