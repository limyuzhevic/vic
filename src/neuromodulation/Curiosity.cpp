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
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
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

// Include the full Curiosity implementation with advanced features

// Initialize with brain reference
void Curiosity::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
    
    // Set up adaptive weights based on brain configuration
    pImpl->noveltyWeight = 0.6f;
    pImpl->predictionErrorWeight = 0.4f;
    pImpl->decayRate = 0.05f;
}

// Update curiosity based on novelty and prediction error
void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Real curiosity dynamics based on neuroscience
    // Combines intrinsic motivation with predictive coding
    
    // Calculate target curiosity level
    float noveltyComponent = pImpl->noveltyWeight * novelty;
    float predictionErrorComponent = pImpl->predictionErrorWeight * std::abs(predictionError);
    float target = noveltyComponent + predictionErrorComponent;
    
    // Smooth update with time constant
    float timeConstant = 1.0f / (0.05f + 0.1f * dt);
    pImpl->level += (target - pImpl->level) * (1.0f - std::exp(-dt * timeConstant));
    
    // Apply adaptive decay based on environmental stability
    float stability = pImpl->brain ? pImpl->brain->getEnvironmentalStability() : 0.5f;
    pImpl->decayRate = 0.02f + stability * 0.08f; // Faster decay in stable environments
    
    // Natural decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Log significant changes
    if (std::abs(target - pImpl->level) > 0.1f) {
        NLM_LOG_DEBUG("Curiosity updated: novelty=" + std::to_string(novelty) +
                     ", prediction_error=" + std::to_string(predictionError) +
                     ", level=" + std::to_string(pImpl->level));
    }
}

// Compute exploration bonus based on curiosity
float Curiosity::computeExplorationBonus() const {
    // Real exploration bonus function based on entropy and surprise
    float entropy = computeEnvironmentalEntropy();
    float surprise = computePerceivedSurprise();
    
    // Exploration bonus increases with curiosity
    float bonus = pImpl->level * (entropy * 0.5f + surprise * 0.5f);
    
    // Apply learning rate modulation
    float learningRate = pImpl->brain ? pImpl->brain->getLearningRate() : 0.01f;
    bonus *= learningRate * 10.0f;
    
    return std::clamp(bonus, 0.0f, 1.0f);
}

// Get current exploration drive with temporal dynamics
float Curiosity::getExplorationDrive() const {
    // Real exploration drive with temporal dynamics
    // Incorporates both current curiosity level and historical patterns
    
    // Apply temporal smoothing
    static float smoothedLevel = 0.0f;
    float alpha = 0.9f; // Smoothing factor
    smoothedLevel = alpha * pImpl->level + (1.0f - alpha) * smoothedLevel;
    
    // Add temporal persistence for sustained exploration
    float persistence = 0.8f; // Persistence factor
    float temporalFactor = persistence + (1.0f - persistence) * std::abs(pImpl->level - smoothedLevel);
    
    return pImpl->level * temporalFactor;
}

// Set curiosity parameters with validation
void Curiosity::setNoveltyWeight(float weight) {
    if (weight >= 0.0f && weight <= 1.0f) {
        pImpl->noveltyWeight = weight;
        // Renormalize prediction error weight
        pImpl->predictionErrorWeight = 1.0f - weight;
        NLM_LOG_INFO("Curiosity novelty weight set to: " + std::to_string(weight));
    }
}

// Set prediction error weight with validation
void Curiosity::setPredictionErrorWeight(float weight) {
    if (weight >= 0.0f && weight <= 1.0f) {
        pImpl->predictionErrorWeight = weight;
        // Renormalize novelty weight
        pImpl->noveltyWeight = 1.0f - weight;
        NLM_LOG_INFO("Curiosity prediction error weight set to: " + std::to_string(weight));
    }
}

// Get comprehensive curiosity metrics
CuriosityMetrics Curiosity::getMetrics() const {
    CuriosityMetrics metrics;
    
    metrics.level = pImpl->level;
    metrics.explorationDrive = getExplorationDrive();
    metrics.noveltyWeight = pImpl->noveltyWeight;
    metrics.predictionErrorWeight = pImpl->predictionErrorWeight;
    metrics.decayRate = pImpl->decayRate;
    metrics.explorationBonus = computeExplorationBonus();
    metrics.temporalStability = computeTemporalStability();
    
    return metrics;
}

// Reset curiosity to initial state
void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->history.clear();
    NLM_LOG_INFO("Curiosity system reset");
}

// Compute environmental entropy
float Curiosity::computeEnvironmentalEntropy() const {
    if (!pImpl->brain) return 0.5f; // Default moderate entropy
    
    // Get environmental state statistics
    auto environment = pImpl->brain->getEnvironment();
    if (!environment) return 0.5f;
    
    // Compute entropy from environmental features
    // Simple implementation: entropy based on feature variance
    std::vector<float> features = environment->getStateFeatures();
    if (features.empty()) return 0.5f;
    
    float mean = 0.0f;
    for (float f : features) mean += f;
    mean /= features.size();
    
    float variance = 0.0f;
    for (float f : features) {
        float diff = f - mean;
        variance += diff * diff;
    }
    variance /= features.size();
    
    // Normalize to [0, 1] range
    float entropy = 1.0f - std::exp(-variance * 10.0f);
    return std::clamp(entropy, 0.0f, 1.0f);
}

// Compute perceived surprise
float Curiosity::computePerceivedSurprise() const {
    if (pImpl->history.empty()) return 0.0f;
    
    // Calculate surprise as deviation from recent history
    float recentMean = 0.0f;
    size_t recentCount = std::min(size_t(10), pImpl->history.size());
    
    for (size_t i = 0; i < recentCount; ++i) {
        recentMean += pImpl->history[i];
    }
    recentMean /= recentCount;
    
    float currentDev = std::abs(pImpl->level - recentMean);
    return std::min(1.0f, currentDev * 5.0f); // Scale to [0, 1]
}

// Compute temporal stability
float Curiosity::computeTemporalStability() const {
    if (pImpl->history.size() < 5) return 1.0f; // High stability if insufficient data
    
    // Compute coefficient of variation of recent history
    float recentMean = 0.0f;
    size_t recentCount = std::min(size_t(10), pImpl->history.size());
    
    for (size_t i = 0; i < recentCount; ++i) {
        recentMean += pImpl->history[pImpl->history.size() - 1 - i];
    }
    recentMean /= recentCount;
    
    float variance = 0.0f;
    for (size_t i = 0; i < recentCount; ++i) {
        float diff = pImpl->history[pImpl->history.size() - 1 - i] - recentMean;
        variance += diff * diff;
    }
    variance /= recentCount;
    
    float stdDev = std::sqrt(variance);
    return std::max(0.0f, 1.0f - (stdDev / (stdDev + 0.1f))); // Stability in [0, 1]
}

}} // namespace nlm
