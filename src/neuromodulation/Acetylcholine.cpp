class Acetylcholine::Impl {
public:
    float level;
    float attentionLevel;
    float memoryConsolidation;
    float attentionGain;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() 
        : level(0.0f)
        , attentionLevel(0.0f)
        , memoryConsolidation(0.0f)
        , attentionGain(1.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline with realistic dynamics
    if (pImpl->level > pImpl->baseline) {
        pImpl->level = std::max(pImpl->baseline, 
                              pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    } else {
        // Recovery towards baseline
        pImpl->level = std::min(pImpl->baseline, 
                              pImpl->level + pImpl->decayRate * static_cast<float>(dt) * 0.5f);
    }
    
    // Calculate attention modulation
    pImpl->attentionLevel = pImpl->level * pImpl->attentionGain;
    
    // Memory consolidation is stronger with higher ACh
    pImpl->memoryConsolidation = pImpl->level * pImpl->memoryConsolidation * 0.7f;
}

void Acetylcholine::signalAttention(float salience, const std::vector<float>& sensoryPattern) {
    if (salience <= 0.0f) return;
    
    // Compute pattern novelty (simplified)
    float novelty = 0.0f;
    if (!sensoryPattern.empty()) {
        // In a real implementation, compare to recent patterns
        // For now, use salience as a proxy
        novelty = salience * 0.5f;
    }
    
    // Update attention level with saturation
    if (novelty > 0.0f) {
        pImpl->attentionLevel = std::min(pImpl->peak, 
                                        pImpl->attentionLevel + novelty * pImpl->attentionGain);
        pImpl->level = std::min(pImpl->peak, 
                              pImpl->level + novelty * pImpl->releaseRate * pImpl->attentionGain);
    }
    
    // Apply cholinergic modulation to plasticity
    if (pImpl->level > pImpl->arousalThreshold) {
        // Strong attention enhances working memory encoding
        if (brain_) {
            auto* workingMem = brain_->getWorkingMemory();
            if (workingMem) {
                workingMem->decay(0.1f);  // Enhance retention
            }
        }
    }
}
