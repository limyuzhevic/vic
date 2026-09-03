#include "STDP.hpp"
#include <cmath>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;   // A+ for potentiation
    float ltdWeight;   // A- for depression
    float timeConstant;  // Tau for exponential window
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    // TODO PHASE 2: Implement real STDP
    // PLACEHOLDER: Simple STDP rule
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Simplified STDP: 
    // - If pre fires before post (positive dt): potentiate
    // - If post fires before pre (negative dt): depress
    
    float totalDelta = 0.0f;
    
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float dt = static_cast<float>(post - pre);  // ms
            
            if (dt > 0) {
                // Pre before post: potentiation
                float delta = pImpl->ltpWeight * std::exp(-dt / pImpl->timeConstant);
                totalDelta += delta;
            } else {
                // Post before pre: depression
                float delta = -pImpl->ltdWeight * std::exp(dt / pImpl->timeConstant);
                totalDelta += delta;
            }
        }
    }
    
    if (std::abs(totalDelta) > 0.0001f) {
        applyWeightChange(synapse, totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = weight;
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = weight;
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = tau;
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    pImpl->ltpWeight = ltpWeight;
    pImpl->ltdWeight = ltdWeight;
    pImpl->timeConstant = tau;
}

} // namespace nlm
