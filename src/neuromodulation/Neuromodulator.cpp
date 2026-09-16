#include "Neuromodulator.hpp"
#include <algorithm>

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

const char* Neuromodulator::getName() const {
    return "NM";
}

void Neuromodulator::update(TimestepDuration dt) {
    // Default implementation: update all neuromodulators
    // This is a placeholder - real implementation would coordinate interactions
    dopamine->update(dt);
    curiosity->update(dt);
    predictionError->update(dt);
    novelty->update(dt);
}

float Neuromodulator::getTotalModulation() const {
    float total = 0.0f;
    total += dopamine ? dopamine->getLevel() : 0.0f;
    total += curiosity ? curiosity->getLevel() : 0.0f;
    total += predictionError ? predictionError->getLevel() : 0.0f;
    total += novelty ? novelty->getLevel() : 0.0f;
    return total / 4.0f;  // Average
}

Neuromodulator::Neuromodulator()
    : dopamine(new Dopamine())
    , curiosity(new Curiosity())
    , predictionError(new PredictionError())
    , novelty(new Novelty())
    , brain_(nullptr)
{
}

Neuromodulator::~Neuromodulator() = default;

void Neuromodulator::initialize(Brain* brain) {
    brain_ = brain;
    dopamine->setLevel(0.0f);
    curiosity->setLevel(0.0f);
    predictionError->setLevel(0.0f);
    novelty->setLevel(0.0f);
}

float Neuromodulator::getNeuromodulationLevel() const {
    return getTotalModulation();
}

Dopamine* Neuromodulator::getDopamine() const {
    return dopamine.get();
}

Curiosity* Neuromodulator::getCuriosity() const {
    return curiosity.get();
}

PredictionError* Neuromodulator::getPredictionError() const {
    return predictionError.get();
}

Novelty* Neuromodulator::getNovelty() const {
    return novelty.get();
}

void Neuromodulator::reset() {
    if (dopamine) dopamine->setLevel(0.0f);
    if (curiosity) curiosity->setLevel(0.0f);
    if (predictionError) predictionError->setLevel(0.0f);
    if (novelty) novelty->setLevel(0.0f);
}
