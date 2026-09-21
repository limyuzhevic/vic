#include "Neuromodulator.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::applyToPlasticity(STDP* stdp, Hebbian* hebbian) {
    // Dopamine modulates plasticity based on reward prediction error
    // Positive dopamine increases plasticity, negative decreases it
    
    if (stdp) {
        // Get current dopamine level (-1 to 1)
        float dopamine = getLevel();
        
        // Scale STDP learning rates based on dopamine
        // Positive dopamine (reward prediction) increases LTP and reduces LTD
        // Negative dopamine (prediction error) decreases LTP and enhances LTD
        
        float baseLTP = 0.01f;
        float baseLTD = 0.012f;
        
        // Dopamine modulates STDP parameters
        if (dopamine > 0) {
            // Reward: Enhance LTP, reduce LTD
            stdp->setLTPWeight(baseLTP * (1.0f + dopamine * 0.5f));
            stdp->setLTDWeight(baseLTD * (1.0f - dopamine * 0.3f));
        } else {
            // Punishment/prediction error: Reduce LTP, enhance LTD
            stdp->setLTPWeight(baseLTP * (1.0f + dopamine * 0.3f));  // dopamine is negative
            stdp->setLTDWeight(baseLTD * (1.0f - dopamine * 0.5f));  // -dopamine makes this larger
        }
    }
    
    if (hebbian) {
        // Hebbian plasticity also modulated by dopamine
        // This allows reward-based strengthening of associations
    }
}

} // namespace nlm
