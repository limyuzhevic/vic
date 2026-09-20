// Novelty implementation - novelty detection system
struct Novelty::Impl {
    // Current novelty level
    float level;
    
    // Novelty detection thresholds
    float noveltyThreshold;
    float decayRate;
    
    // Comparison buffers for pattern change detection
    std::vector<float> currentPattern;
    std::vector<float> previousPattern;
    size_t patternSize;
    
    // History of recent novelties for smoothing
    std::vector<float> noveltyHistory;
    
    // Brain reference for integration
    class Brain* brain;
    
    // Adaptation parameters
    float learningRate;
    float adaptationRate;
    
    // Integration state
    float integratedNovelty;
    
    Impl() : 
        level(0.0f),
        noveltyThreshold(0.1f),
        decayRate(0.05f),
        patternSize(256),  // Default vision pattern size
        learningRate(0.1f),
        adaptationRate(0.01f),
        integratedNovelty(0.0f),
        brain(nullptr) {}
};

Novelty::Novelty() : pImpl(new Impl) {
    NLM_LOG_INFO("Novelty detection system initialized");
}

Novelty::~Novelty() = default;

void Novelty::initialize(class Brain* brain) {
    pImpl->brain = brain;
    
    // Configure based on brain parameters
    if (brain) {
        auto config = brain->getConfig();
        if (config) {
            pImpl->noveltyThreshold = config->getOr<float>("novelty_threshold", 0.1f);
            pImpl->decayRate = config->getOr<float>("novelty_decay_rate", 0.05f);
            pImpl->learningRate = config->getOr<float>("novelty_learning_rate", 0.1f);
            pImpl->adaptationRate = config->getOr<float>("novelty_adaptation_rate", 0.01f);
        }
    }
    
    // Initialize patterns
    pImpl->currentPattern.resize(pImpl->patternSize, 0.0f);
    pImpl->previousPattern.resize(pImpl->patternSize, 0.0f);
    
    NLM_LOG_INFO("Novelty system initialized with threshold: " + std::to_string(pImpl->noveltyThreshold));
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty level over time (prevents runaway excitation)
    pImpl->level *= (1.0f - pImpl->decayRate * dt);
    
    // Update integrated novelty with slower decay
    pImpl->integratedNovelty *= (1.0f - 0.005f * dt);
    
    // Adaptation to environment
    pImpl->noveltyThreshold = pImpl->noveltyThreshold * (1.0f - pImpl->adaptationRate * dt);
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->noveltyHistory;
}

void Novelty::clearHistory() {
    pImpl->noveltyHistory.clear();
    NLM_LOG_DEBUG("Novelty history cleared");
}

} // namespace nlm