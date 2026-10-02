#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <numeric>
#include <algorithm>

namespace nlm {

struct Curiosity::Impl {
    // Brain reference for context
    class Brain* brain;
    
    // Current curiosity level
    float level;
    
    // Weighted contributions from different sources
    float predictionErrorContribution;
    float noveltyContribution;
    
    // Parameters for learning dynamics
    float predictionErrorWeight;
    float noveltyWeight;
    float integrationRate;     // How quickly curiosity responds to signals
    float decayRate;            // Natural decay of curiosity
    
    // Statistics and history
    float minCuriosity;
    float maxCuriosity;
    float sumCuriosity;
    size_t countCuriosity;
    std::vector<float> history;
    
    // Adaptive parameters
    float explorationThreshold;   // Level above which exploration is triggered
    float habituationRate;        // Rate at which curiosity decreases with repeated exposure
    float sensitizationRate;      // Rate at which curiosity increases with novel experiences
    
    // Internal state for learning
    float recentPredictionError;
    float recentNovelty;
    size_t explorationCount;
    
    // Neuromodulatory influences
    float dopamineInfluence;     // Dopamine modulates curiosity
    float acetylcholineInfluence; // ACh enhances attention to novel stimuli
    
    Impl() : brain(nullptr), level(0.0f),
             predictionErrorContribution(0.0f), noveltyContribution(0.0f),
             predictionErrorWeight(0.6f), noveltyWeight(0.4f),
             integrationRate(0.1f), decayRate(0.05f),
             explorationThreshold(0.5f), habituationRate(0.01f),
             sensitizationRate(0.02f), recentPredictionError(0.0f),
             recentNovelty(0.0f), explorationCount(0),
             dopamineInfluence(1.0f), acetylcholineInfluence(1.0f),
             minCuriosity(std::numeric_limits<float>::max()),
             maxCuriosity(std::numeric_limits<float>::lowest()),
             sumCuriosity(0.0f), countCuriosity(0) {}
};

Curiosity::Curiosity() : pImpl(new Impl),
    predictionErrorWeight_(0.6f), noveltyWeight_(0.4f),
    integrationRate_(0.1f), decayRate_(0.05f) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float predictionError, float novelty, TimestepDuration dt) {
    // Store recent signals for learning
    pImpl->recentPredictionError = predictionError;
    pImpl->recentNovelty = novelty;
    
    // Compute weighted contributions to curiosity
    // Based on the curiosity hypothesis: prediction error and novelty drive exploration
    
    // Normalized prediction error (0-1 range)
    float normalizedPredictionError = std::min(predictionError, 1.0f);
    
    // Ensure novelty is in [0,1] range
    float normalizedNovelty = std::min(novelty, 1.0f);
    
    // Calculate weighted contributions
    pImpl->predictionErrorContribution = normalizedPredictionError * pImpl->predictionErrorWeight;
    pImpl->noveltyContribution = normalizedNovelty * pImpl->noveltyWeight;
    
    // Combined signal from both sources
    float combinedSignal = pImpl->predictionErrorContribution + pImpl->noveltyContribution;
    
    // Integration with time constant
    float timeStepSeconds = static_cast<float>(dt);
    float integration = pImpl->integrationRate * combinedSignal;
    
    // Apply neuromodulatory influences
    float neuromodulatoryEffect = pImpl->dopamineInfluence * pImpl->acetylcholineInfluence;
    integration *= neuromodulatoryEffect;
    
    // Update curiosity level with integration
    pImpl->level += integration;
    
    // Natural decay and habituation
    float decay = pImpl->decayRate * pImpl->level;
    float habituation = pImpl->habituationRate * pImpl->level;
    
    pImpl->level -= decay;
    pImpl->level -= habituation;
    
    // Sensitization from novel experiences
    if (normalizedNovelty > 0.5f) {  // Significant novelty triggers sensitization
        pImpl->level += pImpl->sensitizationRate * (1.0f - pImpl->level);
    }
    
    // Ensure level stays in reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
    
    // Update statistics
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    if (pImpl->countCuriosity > 0) {
        pImpl->minCuriosity = std::min(pImpl->minCuriosity, pImpl->level);
        pImpl->maxCuriosity = std::max(pImpl->maxCuriosity, pImpl->level);
        pImpl->sumCuriosity += pImpl->level;
    } else {
        pImpl->minCuriosity = pImpl->level;
        pImpl->maxCuriosity = pImpl->level;
    }
    pImpl->countCuriosity++;
    
    // Check if exploration is triggered
    if (pImpl->level > pImpl->explorationThreshold) {
        pImpl->explorationCount++;
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Curiosity::setIntegrationRate(float rate) {
    pImpl->integrationRate = std::clamp(rate, 0.01f, 1.0f);
}

void Curiosity::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.0f, 0.5f);
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->predictionErrorContribution = 0.0f;
    pImpl->noveltyContribution = 0.0f;
    pImpl->history.clear();
    pImpl->recentPredictionError = 0.0f;
    pImpl->recentNovelty = 0.0f;
    pImpl->explorationCount = 0;
    
    // Reset statistics
    pImpl->minCuriosity = std::numeric_limits<float>::max();
    pImpl->maxCuriosity = std::numeric_limits<float>::lowest();
    pImpl->sumCuriosity = 0.0f;
    pImpl->countCuriosity = 0;
}

bool Curiosity::isDrivingExploration() const {
    return pImpl->level > pImpl->explorationThreshold;
}

float Curiosity::getMeanCuriosity() const {
    if (pImpl->countCuriosity == 0) return 0.0f;
    return pImpl->sumCuriosity / pImpl->countCuriosity;
}

float Curiosity::getMaxCuriosity() const {
    return pImpl->maxCuriosity;
}

float Curiosity::getMinCuriosity() const {
    return pImpl->minCuriosity;
}

} // namespace nlm
