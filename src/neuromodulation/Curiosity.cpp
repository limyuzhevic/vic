#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;                    // Current curiosity level (exploration drive)
    float noveltyWeight;            // Weight of novelty in curiosity computation
    float predictionErrorWeight;    // Weight of prediction error in curiosity computation
    float decayRate;                // Decay rate for curiosity
    float adaptationRate;           // Rate of adaptation to changing environment
    
    // Integration and learning components
    float explorationBias;         // Bias towards exploration vs. exploitation
    float uncertainty;              // Estimated uncertainty about environment
    float learningProgress;         // Track learning progress for curiosity
    
    // History for computation
    std::vector<float> noveltyHistory;      // History of novelty values
    std::vector<float> errorHistory;        // History of prediction errors
    std::vector<float> adaptationHistory;   // For tracking adaptation
    
    // Adaptive parameters for sophisticated exploration
    float explorationTemperature;   // Controls exploration randomness
    float intrinsicMotivationalGain; // Strength of intrinsic motivation
    
    Impl() : brain(nullptr), level(0.0f), noveltyWeight(0.5f), predictionErrorWeight(0.5f)
        , decayRate(0.05f), adaptationRate(0.01f), explorationBias(0.5f), uncertainty(0.5f)
        , learningProgress(0.0f), explorationTemperature(1.0f), intrinsicMotivationalGain(1.0f) {}
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
    // Enhanced curiosity computation using sophisticated exploration models
    // Based on: Schmidhuber (1991), "A theory of active learning and stochastic
    // reinforcement learning", and: Oudeyer & Kaplan (2009), "What is intrinsic
    // motivation? A behavioral drive view"
    
    // Calculate prediction error magnitude
    float errorMagnitude = std::abs(predictionError);
    
    // Update novelty history for adaptive weighting
    pImpl->noveltyHistory.push_back(novelty);
    if (pImpl->noveltyHistory.size() > 100) {
        pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
    }
    
    // Update error history for uncertainty estimation
    pImpl->errorHistory.push_back(errorMagnitude);
    if (pImpl->errorHistory.size() > 100) {
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    // Compute adaptive weights based on recent statistics
    float avgNovelty = calculateAverage(pImpl->noveltyHistory);
    float avgError = calculateAverage(pImpl->errorHistory);
    
    // Adapt weights based on learning progress and uncertainty
    float learningProgress = 1.0f - (avgError / (avgError + 0.1f));
    pImpl->learningProgress = (pImpl->learningProgress * 0.9f) + (learningProgress * 0.1f);
    
    // Higher curiosity when learning progress is low (more to learn)
    float learningComponent = (1.0f - pImpl->learningProgress) * 0.5f;
    
    // Compute uncertainty from prediction error variance
    pImpl->uncertainty = calculateUncertainty(pImpl->errorHistory);
    
    // Adaptation based on environmental uncertainty
    float uncertaintyComponent = pImpl->uncertainty * 0.3f;
    
    // Novelty and prediction error components
    float noveltyComponent = novelty * pImpl->noveltyWeight * (1.0f + pImpl->uncertainty);
    float errorComponent = errorMagnitude * pImpl->predictionErrorWeight;
    
    // Intrinsic motivation component (active learning drive)
    float intrinsicComponent = pImpl->intrinsicMotivationalGain * learningComponent;
    
    // Integration with novelty system for context-sensitive exploration
    float contextGain = 1.0f;
    if (pImpl->brain && pImpl->brain->getNovelty()) {
        float noveltyLevel = pImpl->brain->getNovelty()->getLevel();
        contextGain = 0.7f + 0.3f * noveltyLevel;  // More curiosity when novelty is high
    }
    
    // Smooth update with time constant
    float timeConstant = 1.0f;  // Time constant for adaptation
    float target = (noveltyComponent + errorComponent + intrinsicComponent) * contextGain;
    
    // Apply adaptation rate for stability
    pImpl->level += (target - pImpl->level) * std::min(1.0f, pImpl->adaptationRate * static_cast<float>(dt));
    
    // Decay mechanism for exploration homeostasis
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Store adaptation history for later analysis
    pImpl->adaptationHistory.push_back(pImpl->level);
    if (pImpl->adaptationHistory.size() > 50) {
        pImpl->adaptationHistory.erase(pImpl->adaptationHistory.begin());
    }
}

float Curiosity::getExplorationDrive() const {
    // Enhanced exploration drive computation
    // Combines curiosity level with adaptive exploration parameters
    
    // Base exploration drive from curiosity level
    float drive = pImpl->level;
    
    // Modify based on exploration bias
    float biasComponent = pImpl->explorationBias * 0.2f;
    
    // Incorporate temperature for stochastic exploration
    float stochasticComponent = pImpl->explorationTemperature * std::tanh(drive);
    
    // Final exploration drive with bounds
    float explorationDrive = drive + biasComponent + stochasticComponent;
    return std::clamp(explorationDrive, 0.0f, 1.0f);
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->learningProgress = 0.0f;
    pImpl->uncertainty = 0.0f;
    pImpl->noveltyHistory.clear();
    pImpl->errorHistory.clear();
}

} // namespace nlm
