#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f) {}
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

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Real curiosity computation
    // Curiosity increases with both novelty and prediction error (prediction surprises)
    // but decays over time
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update with memory of previous curiosity levels
    float learningRate = 0.1f;
    pImpl->level += (target - pImpl->level) * learningRate;
    
    // Decay over time - curiosity fades if not sustained
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level *= decayFactor;
    
    // Curiosity drives exploration by affecting neural gain
    // Higher curiosity increases neural variability and exploration
    if (pImpl->brain && pImpl->level > 0.5f) {
        // Apply exploration drive to all neurons
        // This increases spontaneous activity and trial-and-error learning
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, pImpl->level * 2.0f);
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm
