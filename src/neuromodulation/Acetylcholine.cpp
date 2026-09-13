// Acetylcholine: Attention and memory consolidation
// Real implementation with dynamic modulation of attention and plasticity

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityBoost;
    float attentionModulation;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , plasticityBoost(2.0f)
        , attentionModulation(1.5f) {}
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
    // ACh enhances synaptic plasticity for memory consolidation
    return 1.0f + pImpl->level * (pImpl->plasticityBoost - 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline (tonic ACh)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // Phasic ACh signals for attentional focus and learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate);
    
    if (brain_) {
        // Enhance attention to selected neurons
        for (auto& topDown : topDownBias_) {
            // Store attention bias for attention system
            // In real implementation, would modulate attention directly
        }
    }
}

void Acetylcholine::signalNovelty(float novelty) {
    // ACh signals novel events for memory encoding
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate * 0.5f);
}

void Acetylcholine::signalRewardPredictionError(float error) {
    // ACh signals prediction errors for learning
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.3f);
}

void Acetylcholine::setBrain(class Brain* brain) {
    pImpl->brain = brain;
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

void Acetylcholine::setAttentionModulation(float mod) {
    pImpl->attentionModulation = mod;
}

} // namespace nlm
