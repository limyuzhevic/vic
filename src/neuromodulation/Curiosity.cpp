#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float adaptationRate;
    float targetLevel;
    float memory;
    float explorationThreshold;
    float learningProgress;
    std::vector<float> history;
    std::map<std::string, float> crossModulatorInfluence;
    
    // Interaction effects
    float dopamineInfluence;    // From Dopamine system
    float serotoninInfluence;  // From Serotonin system
    float endorphinsInfluence;  // From Endorphins system
    float noveltyInfluence;     // From Novelty system
    float predictionErrorInfluence; // From PredictionError system
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , adaptationRate(0.15f)
        , targetLevel(0.5f)
        , memory(0.0f)
        , explorationThreshold(0.3f)
        , learningProgress(0.0f)
        , dopamineInfluence(0.0f)
        , serotoninInfluence(0.0f)
        , endorphinsInfluence(0.0f)
        , noveltyInfluence(0.0f)
        , predictionErrorInfluence(0.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Advanced Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Adaptive target calculation based on cross-modulator influences
    float noveltyContribution = pImpl->noveltyWeight * novelty;
    float predictionErrorContribution = pImpl->predictionErrorWeight * predictionError;
    
    // Neuromodulator cross-talk effects
    float dopamineModulation = pImpl->dopamineInfluence * 0.2f;
    float serotoninModulation = pImpl->serotoninInfluence * 0.3f;
    float endorphinsModulation = pImpl->endorphinsInfluence * 0.1f;
    float noveltyModulation = pImpl->noveltyInfluence * 0.15f;
    float predictionErrorModulation = pImpl->predictionErrorInfluence * 0.25f;
    
    // Learning progress adaptation
    float learningProgressModulation = pImpl->learningProgress * 0.2f;
    
    // Cross-modulator interactions for exploration drive
    float explorationMultiplier = 1.0f + dopamineModulation + serotoninModulation + 
                                 endorphinsModulation + noveltyModulation + 
                                 predictionErrorModulation;
    explorationMultiplier = std::clamp(explorationMultiplier, 0.5f, 3.0f);
    
    // Calculate adaptive target
    float adaptiveTarget = (noveltyContribution + predictionErrorContribution) * explorationMultiplier;
    
    // Adjust target based on learning progress
    if (pImpl->learningProgress > 0.7f) {
        // When learning well, reduce exploration
        adaptiveTarget *= (1.0f - (pImpl->learningProgress - 0.7f) * 0.5f);
    } else if (pImpl->learningProgress < 0.3f) {
        // When learning poorly, increase exploration
        adaptiveTarget *= (1.0f + (0.3f - pImpl->learningProgress) * 0.8f);
    }
    
    // Smooth update with adaptive rate
    float effectiveAdaptationRate = pImpl->adaptationRate;
    if (pImpl->learningProgress > 0.8f) {
        effectiveAdaptationRate *= 0.5f; // Slower adaptation when learning well
    }
    
    pImpl->level += (adaptiveTarget - pImpl->level) * effectiveAdaptationRate;
    
    // Adaptive decay with learning state
    float effectiveDecay = pImpl->decayRate;
    if (pImpl->level > pImpl->explorationThreshold) {
        // Lower decay when exploration is high
        effectiveDecay *= 0.8f;
    }
    
    pImpl->level = std::max(0.0f, pImpl->level - effectiveDecay * dtFloat);
    
    // Memory trace for pattern detection
    pImpl->memory = pImpl->memory * 0.9f + pImpl->level * 0.1f;
    
    // Adaptive exploration threshold based on cross-modulator state
    pImpl->explorationThreshold = std::lerp(pImpl->explorationThreshold, 
                                          0.3f + pImpl->serotoninInfluence * 0.2f, 
                                          pImpl->adaptationRate);
    
    // Update history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Curiosity::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.0f, 1.0f);
}

void Curiosity::setExplorationThreshold(float threshold) {
    pImpl->explorationThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Curiosity::setLearningProgress(float progress) {
    pImpl->learningProgress = std::clamp(progress, 0.0f, 1.0f);
}

float Curiosity::getLearningProgress() const {
    return pImpl->learningProgress;
}

void Curiosity::addCrossModulatorInfluence(const std::string& modulatorName, float influence) {
    if (modulatorName == "dopamine") {
        pImpl->dopamineInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "serotonin") {
        pImpl->serotoninInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "endorphins") {
        pImpl->endorphinsInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "novelty") {
        pImpl->noveltyInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "prediction_error") {
        pImpl->predictionErrorInfluence = std::clamp(influence, 0.0f, 1.0f);
    }
}

const std::vector<float>& Curiosity::getHistory() const {
    return pImpl->history;
}

void Curiosity::clearHistory() {
    pImpl->history.clear();
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
    pImpl->memory = 0.0f;
    pImpl->learningProgress = 0.0f;
    pImpl->history.clear();
}

} // namespace nlm
