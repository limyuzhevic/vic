#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), attentionGain(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances attention and working memory
    // Higher ACh increases learning rate for attended stimuli
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: decay when attention shifts
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float salience, TimestepDuration dt) {
    // ACh tracks attention - increases when processing salient stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + salience * pImpl->attentionGain * dt);
}

void Acetylcholine::signalLearning(float predictionError, float learningRate) {
    // ACh enhances learning on prediction errors
    if (predictionError != 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + std::abs(predictionError) * learningRate);
    }
}

} // namespace nlm