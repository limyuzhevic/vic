// Curiosity implementation - exploration drive based on novelty and prediction error
struct Curiosity::Impl {
    // Core curiosity state
    float level;
    float explorationDrive;
    
    // Computing weights
    float noveltyWeight;
    float predictionErrorWeight;
    
    // Integration components
    float noveltyInput;
    float predictionErrorInput;
    
    // Brain reference for neural integration
    class Brain* brain;
    
    // Exploration history
    std::vector<float> explorationHistory;
    
    // Reset rate for emotional state decay
    float resetRate;
    
    // Exploration bias parameters
    float habituationRate;
    float sensitivizationRate;
    
    Impl() : 
        level(0.0f),
        explorationDrive(0.0f),
        noveltyWeight(0.6f),
        predictionErrorWeight(0.4f),
        noveltyInput(0.0f),
        predictionErrorInput(0.0f),
        brain(nullptr),
        resetRate(0.05f),
        habituationRate(0.02f),
        sensitivizationRate(0.01f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {
    NLM_LOG_INFO("Curiosity system initialized");
}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(class Brain* brain) {
    pImpl->brain = brain;
    
    // Configure based on brain parameters
    if (brain) {
        auto config = brain->getConfig();
        if (config) {
            pImpl->noveltyWeight = config->getOr<float>("curiosity_novelty_weight", 0.6f);
            pImpl->predictionErrorWeight = config->getOr<float>("curiosity_prediction_weight", 0.4f);
        }
    }
    
    NLM_LOG_INFO("Curiosity system initialized with novelty weight: " + std::to_string(pImpl->noveltyWeight) +
                 " and prediction error weight: " + std::to_string(pImpl->predictionErrorWeight));
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Update inputs
    pImpl->noveltyInput = novelty;
    pImpl->predictionErrorInput = std::abs(predictionError);
    
    // Compute curiosity as weighted combination of novelty and prediction error
    float rawCuriosity = pImpl->noveltyInput * pImpl->noveltyWeight + 
                        pImpl->predictionErrorInput * pImpl->predictionErrorWeight;
    
    // Apply emotional dynamics (exponential integration)
    pImpl->level = pImpl->level * (1.0f - pImpl->resetRate * dt) + 
                   rawCuriosity * pImpl->sensitivizationRate;
    
    // Decay exploration drive
    pImpl->explorationDrive = pImpl->explorationDrive * (1.0f - pImpl->habituationRate * dt);
    
    // Update exploration history for monitoring
    pImpl->explorationHistory.push_back(pImpl->level);
    if (pImpl->explorationHistory.size() > 50) {
        pImpl->explorationHistory.erase(pImpl->explorationHistory.begin());
    }
    
    // Modulate with brain state if available
    if (pImpl->brain && pImpl->level > 0.1f) {
        // Scale by available neural resources
        float brainCapacity = pImpl->brain->getAverageFiringRate();
        pImpl->explorationDrive = std::min(pImpl->level, brainCapacity * 0.1f);
    }
    
    // Log for debugging
    NLM_LOG_DEBUG("Curiosity: novelty=" + std::to_string(novelty) +
                  ", prediction_error=" + std::to_string(predictionError) +
                  ", level=" + std::to_string(pImpl->level) +
                  ", exploration=" + std::to_string(pImpl->explorationDrive));
}

float Curiosity::getExplorationDrive() const {
    return pImpl->explorationDrive;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
    NLM_LOG_DEBUG("Curiosity novelty weight set to: " + std::to_string(pImpl->noveltyWeight));
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
    // Normalize weights to sum to 1
    float totalWeight = pImpl->noveltyWeight + pImpl->predictionErrorWeight;
    if (totalWeight > 0.0f) {
        pImpl->noveltyWeight /= totalWeight;
        pImpl->predictionErrorWeight /= totalWeight;
    }
    
    NLM_LOG_DEBUG("Curiosity prediction error weight set to: " + std::to_string(pImpl->predictionErrorWeight));
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->explorationDrive = 0.0f;
    pImpl->noveltyInput = 0.0f;
    pImpl->predictionErrorInput = 0.0f;
    pImpl->explorationHistory.clear();
    
    NLM_LOG_INFO("Curiosity system reset");
}

} // namespace nlm
