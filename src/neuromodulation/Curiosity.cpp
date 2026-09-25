#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Constants.hpp"
#include "../core/Utils.hpp"

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float novelty_weight;
    float prediction_error_weight;
    float decay_rate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , novelty_weight(Constants::NOVELTY_THRESHOLD)  // Using as placeholder
        , prediction_error_weight(Constants::NOVELTY_THRESHOLD)  // Using as placeholder
        , decay_rate(Constants::CURIOSITY_DECAY_MS) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float prediction_error, TimestepDuration dt) {
    // Curiosity increases with novelty and prediction error
    float target = pImpl->novelty_weight * novelty + 
                   pImpl->prediction_error_weight * prediction_error;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decay_rate * static_cast<float>(dt));
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->novelty_weight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->prediction_error_weight = weight;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
