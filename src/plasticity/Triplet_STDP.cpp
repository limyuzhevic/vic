#include "Triplet_STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Triplet interaction parameters
    float traceTau;       // Trace decay time constant
    float preTraceWeight; // Presynaptic trace weight
    float postTraceWeight; // Postsynaptic trace weight
    
    // Synaptic traces
    float preTrace;       // Presynaptic activity trace
    float postTrace;      // Postsynaptic activity trace
    
    // Previous spike counts for triplet interactions
    int preSpikeCount;    // Number of recent presynaptic spikes
    int postSpikeCount;   // Number of recent postsynaptic spikes
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), traceTau(50.0f),
             preTraceWeight(0.5f), postTraceWeight(0.5f),
             preTrace(0.0f), postTrace(0.0f), preSpikeCount(0), postSpikeCount(0) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    // Update traces based on spike activity
    updateTraces(dt);
    
    // Count spikes
    preSpikeCount = preSpikes.size();
    postSpikeCount = postSpikes.size();
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Standard STDP
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
                
                // Triplet potentiation
                if (postSpikeCount > 1) {
                    float tripletFactor = pImpl->postTraceWeight * pImpl->postTrace;
                    delta = pImpl->ltpWeight * 0.5f * std::exp(-dt / tau) * tripletFactor;
                    totalDelta += delta;
                }
            } else if (dt < 0) {
                // Standard depression
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
                
                // Triplet depression
                if (preSpikeCount > 1) {
                    float tripletFactor = pImpl->preTraceWeight * pImpl->preTrace;
                    delta = -pImpl->ltdWeight * 0.5f * std::exp(dt / tau) * tripletFactor;
                    totalDelta += delta;
                }
            }
        }
    }
    
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        synapse->addToWeight(totalDelta);
        
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void TripletSTDP::setTraceDecayTime(float traceTau) {
    pImpl->traceTau = std::clamp(traceTau, 1.0f, 200.0f);
}

float TripletSTDP::getTraceDecayTime() const {
    return pImpl->traceTau;
}

void TripletSTDP::setPreTraceWeight(float weight) {
    pImpl->preTraceWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getPreTraceWeight() const {
    return pImpl->preTraceWeight;
}

void TripletSTDP::setPostTraceWeight(float weight) {
    pImpl->postTraceWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getPostTraceWeight() const {
    return pImpl->postTraceWeight;
}

void TripletSTDP::updateTraces(float dt) {
    // Update trace decays
    pImpl->preTrace *= std::exp(-dt / pImpl->traceTau);
    pImpl->postTrace *= std::exp(-dt / pImpl->traceTau);
    
    // Add spike contributions
    pImpl->preTrace += preSpikeCount;
    pImpl->postTrace += postSpikeCount;
    
    // Clamp traces
    pImpl->preTrace = std::min(pImpl->preTrace, 100.0f);
    pImpl->postTrace = std::min(pImpl->postTrace, 100.0f);
}

float TripletSTDP::getPreTrace() const {
    return pImpl->preTrace;
}

float TripletSTDP::getPostTrace() const {
    return pImpl->postTrace;
}

} // namespace nlm
