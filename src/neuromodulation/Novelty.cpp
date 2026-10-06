#include "Novelty.hpp"
#include "brain/Brain.hpp"
#include "Neuromodulator.hpp"
#include "Dopamine.hpp"
#include "Curiosity.hpp"
#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    float adaptationRate;
    float predictionErrorInfluence;
    float dopamineInfluence;
    std::vector<float> featureBuffer;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f),
             adaptationRate(0.15f), predictionErrorInfluence(0.4f), dopamineInfluence(0.3f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    if (!observation.isValid()) {
        pImpl->level = 0.0f;
        return;
    }
    
    const SensoryInput* currentInput = observation.getSensoryInput();
    const SensoryInput* previousInput = previousObservation.getSensoryInput();
    
    if (!currentInput || !previousInput) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Extract features from sensory inputs
    std::vector<float> currentPattern;
    std::vector<float> previousPattern;
    
    // Simple feature extraction - use intensity values
    currentPattern.reserve(currentInput->getIntensityCount());
    previousPattern.reserve(currentInput->getIntensityCount());
    
    for (size_t i = 0; i < currentInput->getIntensityCount(); ++i) {
        currentPattern.push_back(currentInput->getIntensity(i));
        previousPattern.push_back(previousInput->getIntensity(i));
    }
    
    // Add other sensory features
    float currentFrequency = currentInput->getFrequency();
    float previousFrequency = previousInput->getFrequency();
    currentPattern.push_back(currentFrequency);
    previousPattern.push_back(previousFrequency);
    
    float currentPhase = currentInput->getPhase();
    float previousPhase = previousInput->getPhase();
    currentPattern.push_back(currentPhase);
    previousPattern.push_back(previousPhase);
    
    // Detect novelty using pattern comparison
    detectNovelty(currentPattern, previousPattern);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute difference between patterns with enhanced metrics
    float totalDiff = 0.0f;
    float correlation = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    if (compareLen > 0) {
        float sumCurrent = std::accumulate(currentPattern.begin(), currentPattern.begin() + compareLen, 0.0f);
        float sumPrevious = std::accumulate(previousPattern.begin(), previousPattern.begin() + compareLen, 0.0f);
        float meanCurrent = sumCurrent / compareLen;
        float meanPrevious = sumPrevious / compareLen;
        
        float numerator = 0.0f;
        float denomCurrent = 0.0f;
        float denomPrevious = 0.0f;
        
        for (size_t i = 0; i < compareLen; ++i) {
            float diff = currentPattern[i] - previousPattern[i];
            totalDiff += std::abs(diff);
            
            numerator += (currentPattern[i] - meanCurrent) * (previousPattern[i] - meanPrevious);
            denomCurrent += (currentPattern[i] - meanCurrent) * (currentPattern[i] - meanCurrent);
            denomPrevious += (previousPattern[i] - meanPrevious) * (previousPattern[i] - meanPrevious);
        }
        
        if (denomCurrent > 0.0f && denomPrevious > 0.0f) {
            correlation = numerator / std::sqrt(denomCurrent * denomPrevious);
        }
    }
    
    // Normalize by pattern size
    float avgDiff = compareLen > 0 ? totalDiff / compareLen : 0.0f;
    
    // Update novelty level based on difference and inverse correlation
    float noveltyScore = std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
    float correlationComponent = (1.0f - std::abs(correlation)) * 0.5f;
    
    pImpl->level = noveltyScore * 0.7f + correlationComponent * 0.3f;
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
    
    // Enhance with prediction error influence if available
    if (pImpl->brain && pImpl->brain->getPredictionErrorSignal()) {
        float error = pImpl->brain->getPredictionErrorSignal()->getError();
        pImpl->level += std::abs(error) * pImpl->predictionErrorInfluence;
    }
    
    // Dopamine modulation
    if (pImpl->brain && pImpl->brain->getDopamine()) {
        float dopamine = pImpl->brain->getDopamine()->getLevel();
        pImpl->level += dopamine * pImpl->dopamineInfluence;
    }
    
    // Apply adaptation
    pImpl->level = std::min(1.0f, pImpl->level * (1.0f + pImpl->adaptationRate));
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
