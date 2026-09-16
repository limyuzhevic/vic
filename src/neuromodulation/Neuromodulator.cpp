#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// Neuromodulator: Central controller for all neuromodulatory systems
// Coordinates dopamine, curiosity, novelty, reward prediction error
// Provides integrated signal to brain systems

struct Neuromodulator::Impl {
    class Dopamine* dopamine;
    class Curiosity* curiosity;
    class Novelty* novelty;
    class RewardPredictionError* predictionError;
    
    Impl() : dopamine(nullptr), curiosity(nullptr), novelty(nullptr), predictionError(nullptr) {}
};

Neuromodulator::Neuromodulator() : pImpl(new Impl) {}

Neuromodulator::~Neuromodulator() = default;

void Neuromodulator::initialize(Brain* brain) {
    pImpl->dopamine = new Dopamine();
    pImpl->curiosity = new Curiosity();
    pImpl->novelty = new Novelty();
    pImpl->predictionError = new PredictionError();
    
    pImpl->dopamine->initialize(brain);
    pImpl->curiosity->initialize(brain);
    pImpl->novelty->initialize(brain);
    pImpl->predictionError->initialize(brain);
    
    NLM_LOG_INFO("Neuromodulator system initialized");
}

void Neuromodulator::update(Brain* brain, TimestepDuration dt, 
                           float novelty, float predictionError,
                           float reward, float predictedReward) {
    if (!brain) return;
    
    // Update all neuromodulators
    if (pImpl->dopamine) {
        pImpl->dopamine->update(dt);
        if (reward > 0.0f || predictedReward > 0.0f) {
            pImpl->dopamine->signalReward(reward);
        }
        pImpl->dopamine->signalRewardPredictionError(predictionError);
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->update(novelty, predictionError, dt);
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->update(dt);
    }
    
    if (pImpl->predictionError) {
        pImpl->predictionError->update(dt);
    }
}

// Neuromodulator accessor methods
Dopamine* Neuromodulator::getDopamine() const { return pImpl->dopamine; }
Curiosity* Neuromodulator::getCuriosity() const { return pImpl->curiosity; }
Novelty* Neuromodulator::getNovelty() const { return pImpl->novelty; }
RewardPredictionError* Neuromodulator::getPredictionError() const { return pImpl->predictionError; }

void Neuromodulator::reset() {
    if (pImpl->dopamine) pImpl->dopamine->reset();
    if (pImpl->curiosity) pImpl->curiosity->reset();
    if (pImpl->novelty) pImpl->novelty->reset();
    if (pImpl->predictionError) pImpl->predictionError->reset();
}

} // namespace nlm