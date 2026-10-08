#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float adaptationRate;
    float noveltyThreshold;
    float learningProgress;
    float attentionModulation;
    std::vector<float> history;
    std::vector<float> lastPattern;
    std::map<std::string, float> crossModulatorInfluence;
    
    // Advanced novelty detection parameters
    float sensitivity;
    float patternMemorySize;
    float crossPatternDecay;
    float predictionErrorContribution;
    
    // Interaction effects
    float dopamineInfluence;    // From Dopamine system
    float serotoninInfluence;  // From Serotonin system
    float endorphinsInfluence;  // From Endorphins system
    float curiosityInfluence;   // From Curiosity system
    float predictionErrorInfluence; // From PredictionError system
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), adaptationRate(0.1f),
             noveltyThreshold(0.3f), learningProgress(0.0f), attentionModulation(1.0f),
             sensitivity(1.0f), patternMemorySize(50.0f), crossPatternDecay(0.95f),
             predictionErrorContribution(0.5f), dopamineInfluence(0.0f),
             serotoninInfluence(0.0f), endorphinsInfluence(0.0f),
             curiosityInfluence(0.0f), predictionErrorInfluence(0.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Advanced Novelty detection system initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Advanced novelty detection using observation comparison
    float noveltyScore = 0.0f;
    
    // Pattern difference analysis
    float patternDiff = 0.0f;
    size_t numFeatures = observation.size();
    if (numFeatures > 0 && previousObservation.size() > 0) {
        size_t compareLen = std::min(numFeatures, previousObservation.size());
        for (size_t i = 0; i < compareLen; ++i) {
            float diff = std::abs(observation[i] - previousObservation[i]);
            patternDiff += diff;
        }
        patternDiff /= static_cast<float>(compareLen);
    }
    
    // Neuromodulator-modulated novelty sensitivity
    float effectiveSensitivity = pImpl->sensitivity * (1.0f + pImpl->dopamineInfluence * 0.2f);
    effectiveSensitivity *= (1.0f - pImpl->serotoninInfluence * 0.1f); // Serotonin reduces sensitivity
    effectiveSensitivity *= (1.0f + pImpl->endorphinsInfluence * 0.1f); // Endorphins increase sensitivity
    
    // Learning progress adaptation
    if (pImpl->learningProgress > 0.7f) {
        // When learning well, reduce novelty sensitivity
        effectiveSensitivity *= (1.0f - (pImpl->learningProgress - 0.7f) * 0.3f);
    }
    
    // Attention modulation
    effectiveSensitivity *= pImpl->attentionModulation;
    
    // Cross-pattern memory influence
    float patternMemoryInfluence = 1.0f;
    if (!pImpl->lastPattern.empty()) {
        float patternSimilarity = 0.0f;
        size_t memLen = std::min(pImpl->lastPattern.size(), observation.size());
        for (size_t i = 0; i < memLen; ++i) {
            patternSimilarity += std::abs(pImpl->lastPattern[i] - observation[i]);
        }
        patternSimilarity /= static_cast<float>(memLen);
        patternMemoryInfluence = std::lerp(1.0f, 0.5f, patternSimilarity * 2.0f);
    }
    
    // Prediction error contribution
    float predictionErrorMod = 1.0f + pImpl->predictionErrorInfluence * pImpl->predictionErrorContribution;
    
    // Calculate final novelty score
    noveltyScore = patternDiff * effectiveSensitivity * patternMemoryInfluence * predictionErrorMod;
    noveltyScore = std::clamp(noveltyScore, 0.0f, 1.0f);
    
    // Update level with adaptive rate
    pImpl->level = std::lerp(pImpl->level, noveltyScore, pImpl->adaptationRate);
    
    // Update history
    pImpl->history.push_back(pImpl->level);
    if (pImpl->history.size() > 100) {
        pImpl->history.erase(pImpl->history.begin());
    }
    
    // Update pattern memory
    pImpl->lastPattern = observation;
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Multi-strategy novelty detection
    
    // Strategy 1: Euclidean distance
    float euclideanDist = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    for (size_t i = 0; i < compareLen; ++i) {
        euclideanDist += (currentPattern[i] - previousPattern[i]) * (currentPattern[i] - previousPattern[i]);
    }
    euclideanDist = std::sqrt(euclideanDist);
    
    // Strategy 2: Cosine similarity
    float dotProduct = 0.0f, currentNorm = 0.0f, previousNorm = 0.0f;
    for (size_t i = 0; i < compareLen; ++i) {
        dotProduct += currentPattern[i] * previousPattern[i];
        currentNorm += currentPattern[i] * currentPattern[i];
        previousNorm += previousPattern[i] * previousPattern[i];
    }
    currentNorm = std::sqrt(currentNorm);
    previousNorm = std::sqrt(previousNorm);
    
    float cosineSim = 0.0f;
    if (currentNorm > 0.0f && previousNorm > 0.0f) {
        cosineSim = dotProduct / (currentNorm * previousNorm);
    }
    
    // Strategy 3: Dynamic time warping approximation
    float dtwDist = 0.0f;
    if (compareLen > 0) {
        float cumulative = 0.0f;
        for (size_t i = 0; i < compareLen; ++i) {
            cumulative += std::abs(currentPattern[i] - previousPattern[i]);
        }
        dtwDist = cumulative / static_cast<float>(compareLen);
    }
    
    // Neuromodulator-weighted novelty score
    float noveltyScore = 0.0f;
    
    // Weighted combination of strategies
    float euclideanWeight = 0.4f + pImpl->dopamineInfluence * 0.2f;
    float cosineWeight = 0.3f + pImpl->curiosityInfluence * 0.2f;
    float dtwWeight = 0.3f + pImpl->predictionErrorInfluence * 0.2f;
    
    euclideanWeight = std::clamp(euclideanWeight, 0.0f, 1.0f);
    cosineWeight = std::clamp(cosineWeight, 0.0f, 1.0f);
    dtwWeight = std::clamp(dtwWeight, 0.0f, 1.0f);
    
    float totalWeight = euclideanWeight + cosineWeight + dtwWeight;
    if (totalWeight > 0.0f) {
        euclideanWeight /= totalWeight;
        cosineWeight /= totalWeight;
        dtwWeight /= totalWeight;
    }
    
    noveltyScore = euclideanDist * euclideanWeight + 
                  (1.0f - cosineSim) * cosineWeight + 
                  dtwDist * dtwWeight;
    
    // Adaptive novelty threshold based on learning progress
    float effectiveThreshold = pImpl->noveltyThreshold;
    if (pImpl->learningProgress > 0.8f) {
        effectiveThreshold *= 1.2f; // Higher threshold when learning well
    } else if (pImpl->learningProgress < 0.3f) {
        effectiveThreshold *= 0.8f; // Lower threshold when learning poorly
    }
    
    // Neuromodulator threshold modulation
    effectiveThreshold *= (1.0f + pImpl->serotoninInfluence * 0.15f);
    effectiveThreshold *= (1.0f - pImpl->endorphinsInfluence * 0.1f);
    
    // Update novelty level
    pImpl->level = std::min(1.0f, noveltyScore / effectiveThreshold);
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
    
    // Pattern memory decay
    for (auto& val : pImpl->lastPattern) {
        val *= pImpl->crossPatternDecay;
    }
}

void Novelty::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Adaptive decay with neuromodulator influences
    float effectiveDecay = pImpl->decayRate;
    if (pImpl->level > 0.5f) {
        // Lower decay when novelty is high
        effectiveDecay *= 0.7f;
    }
    
    // Learning progress affects decay
    if (pImpl->learningProgress > 0.6f) {
        effectiveDecay *= (1.0f - pImpl->learningProgress * 0.2f);
    }
    
    pImpl->level = std::max(0.0f, pImpl->level - effectiveDecay * dtFloat);
    
    // Pattern memory decay
    for (auto& val : pImpl->lastPattern) {
        val *= pImpl->crossPatternDecay;
    }
}

float Novelty::getHistorySize() const {
    return static_cast<float>(pImpl->history.size());
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

void Novelty::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.0f, 1.0f);
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = std::clamp(threshold, 0.01f, 2.0f);
}

void Novelty::setSensitivity(float sensitivity) {
    pImpl->sensitivity = std::clamp(sensitivity, 0.1f, 2.0f);
}

void Novelty::setLearningProgress(float progress) {
    pImpl->learningProgress = std::clamp(progress, 0.0f, 1.0f);
}

void Novelty::setAttentionModulation(float modulation) {
    pImpl->attentionModulation = std::clamp(modulation, 0.5f, 2.0f);
}

void Novelty::addCrossModulatorInfluence(const std::string& modulatorName, float influence) {
    if (modulatorName == "dopamine") {
        pImpl->dopamineInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "serotonin") {
        pImpl->serotoninInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "endorphins") {
        pImpl->endorphinsInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "curiosity") {
        pImpl->curiosityInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "prediction_error") {
        pImpl->predictionErrorInfluence = std::clamp(influence, 0.0f, 1.0f);
    }
}

void Novelty::reset() {
    pImpl->level = 0.0f;
    pImpl->history.clear();
    pImpl->lastPattern.clear();
    pImpl->learningProgress = 0.0f;
    pImpl->attentionModulation = 1.0f;
}

} // namespace nlm
