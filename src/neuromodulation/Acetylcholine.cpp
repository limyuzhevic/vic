// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attention;
    
    Impl() : level(0.0f), baseline(0.02f), peak(1.0f), decayRate(0.02f), releaseRate(1.0f), attention(0.3f) {}
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
    // Acetylcholine modulates attention and working memory
    // High ACh improves signal-to-noise ratio and attention focus
    return 0.8f + 0.4f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Acetylcholine tracks attention and memory availability
    // High ACh promotes working memory and attention
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->attention = std::max(0.0f, pImpl->attention - 0.005f * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // Attention signals from salient stimuli
    pImpl->attention = std::min(1.0f, pImpl->attention + attention * 0.3f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate);
}

void Acetylcholine::signalNovelty(float novelty) {
    // Novelty detection drives acetylcholine release
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate * 1.5f);
}