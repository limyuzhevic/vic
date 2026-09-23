#include "Curiosity.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Curiosity implementation
struct Curiosity::Impl {
    float currentLevel;
    float noveltyLevel;
    float predictionErrorLevel;
    float explorationDrive;
    
    // Integration weights
    float noveltyWeight;
    float predictionErrorWeight;
    float adaptationRate;
    
    // Temporal dynamics
    float timeConstant;
    float decayRate;
    float saturationLimit;
    
    // Learning and adaptation
    float learningRate;
    float explorationTemperature;
    float riskAversion;
    
    // Statistics for analysis
    float maxCuriosityLevel;
    float minCuriosityLevel;
    size_t curiosityEvents;
    float totalNoveltySum;
    float totalPredictionErrorSum;
    float maxNoveltyObserved;
    float maxPredictionErrorObserved;
    
    Impl() : currentLevel(0.0f), noveltyLevel(0.0f), predictionErrorLevel(0.0f),
             explorationDrive(0.0f), noveltyWeight(0.6f), predictionErrorWeight(0.4f),
             adaptationRate(0.1f), timeConstant(20.0f), decayRate(0.05f),
             saturationLimit(1.0f), learningRate(0.01f), explorationTemperature(1.0f),
             riskAversion(0.5f), maxCuriosityLevel(0.0f), minCuriosityLevel(1.0f),
             curiosityEvents(0), totalNoveltySum(0.0f), totalPredictionErrorSum(0.0f),
             maxNoveltyObserved(0.0f), maxPredictionErrorObserved(0.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "CUR";
}

float Curiosity::getLevel() const {
    return pImpl->currentLevel;
}

float Curiosity::getExplorationDrive() const {
    return pImpl->explorationDrive;
}

void Curiosity::initialize(class Brain* brain) {
    // Brain reference is stored for potential future integration
    // For now, curiosity operates as a standalone module
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Update novelty and prediction error levels with temporal dynamics
    float noveltyDecayFactor = std::exp(-static_cast<float>(dt) / pImpl->timeConstant);
    float errorDecayFactor = std::exp(-static_cast<float>(dt) * pImpl->decayRate);
    
    pImpl->noveltyLevel *= noveltyDecayFactor;
    pImpl->predictionErrorLevel *= errorDecayFactor;
    
    // Update novelty and prediction error with new inputs
    pImpl->noveltyLevel = std::min(pImpl->noveltyLevel + novelty * 0.5f, pImpl->saturationLimit);
    pImpl->predictionErrorLevel = std::min(pImpl->predictionErrorLevel + 
                                           predictionError * pImpl->noveltyWeight, 
                                           pImpl->saturationLimit);
    
    // Compute curiosity level as weighted combination
    float weightedNovelty = pImpl->noveltyLevel * pImpl->noveltyWeight;
    float weightedPredictionError = pImpl->predictionErrorLevel * pImpl->predictionErrorWeight;
    
    pImpl->currentLevel = (weightedNovelty + weightedPredictionError) / 
                          (pImpl->noveltyWeight + pImpl->predictionErrorWeight);
    
    // Apply saturation and adaptation
    pImpl->currentLevel = std::clamp(pImpl->currentLevel, 0.0f, pImpl->saturationLimit);
    
    // Apply exploration temperature and risk modulation
    pImpl->explorationDrive = pImpl->currentLevel * 
                             pImpl->explorationTemperature * 
                             (1.0f - pImpl->riskAversion);
    
    // Update statistics
    pImpl->curiosityEvents++;
    pImpl->totalNoveltySum += novelty;
    pImpl->totalPredictionErrorSum += predictionError;
    
    if (novelty > pImpl->maxNoveltyObserved) {
        pImpl->maxNoveltyObserved = novelty;
    }
    if (predictionError > pImpl->maxPredictionErrorObserved) {
        pImpl->maxPredictionErrorObserved = predictionError;
    }
    
    if (pImpl->currentLevel > pImpl->maxCuriosityLevel) {
        pImpl->maxCuriosityLevel = pImpl->currentLevel;
    }
    if (pImpl->currentLevel < pImpl->minCuriosityLevel) {
        pImpl->minCuriosityLevel = pImpl->currentLevel;
    }
}

void Curiosity::reset() {
    pImpl->currentLevel = 0.0f;
    pImpl->noveltyLevel = 0.0f;
    pImpl->predictionErrorLevel = 0.0f;
    pImpl->explorationDrive = 0.0f;
    
    // Reset statistics
    pImpl->maxCuriosityLevel = 0.0f;
    pImpl->minCuriosityLevel = 1.0f;
    pImpl->curiosityEvents = 0;
    pImpl->totalNoveltySum = 0.0f;
    pImpl->totalPredictionErrorSum = 0.0f;
    pImpl->maxNoveltyObserved = 0.0f;
    pImpl->maxPredictionErrorObserved = 0.0f;
}

float Curiosity::getNoveltyLevel() const {
    return pImpl->noveltyLevel;
}

float Curiosity::getPredictionErrorLevel() const {
    return pImpl->predictionErrorLevel;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = std::clamp(weight, 0.0f, 1.0f);
}

float Curiosity::getNoveltyWeight() const {
    return pImpl->noveltyWeight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 1.0f);
}

float Curiosity::getPredictionErrorWeight() const {
    return pImpl->predictionErrorWeight;
}

void Curiosity::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.001f, 0.5f);
}

float Curiosity::getAdaptationRate() const {
    return pImpl->adaptationRate;
}

void Curiosity::setExplorationTemperature(float temp) {
    pImpl->explorationTemperature = std::clamp(temp, 0.1f, 5.0f);
}

float Curiosity::getExplorationTemperature() const {
    return pImpl->explorationTemperature;
}

void Curiosity::setRiskAversion(float risk) {
    pImpl->riskAversion = std::clamp(risk, 0.0f, 1.0f);
}

float Curiosity::getRiskAversion() const {
    return pImpl->riskAversion;
}

float Curiosity::getMaxCuriosityLevel() const {
    return pImpl->maxCuriosityLevel;
}

float Curiosity::getMinCuriosityLevel() const {
    return pImpl->minCuriosityLevel;
}

size_t Curiosity::getCuriosityEvents() const {
    return pImpl->curiosityEvents;
}

float Curiosity::getMeanNovelty() const {
    return pImpl->curiosityEvents > 0 ? 
           pImpl->totalNoveltySum / static_cast<float>(pImpl->curiosityEvents) : 0.0f;
}

float Curiosity::getMeanPredictionError() const {
    return pImpl->curiosityEvents > 0 ? 
           pImpl->totalPredictionErrorSum / static_cast<float>(pImpl->curiosityEvents) : 0.0f;
}

float Curiosity::getMaxNoveltyObserved() const {
    return pImpl->maxNoveltyObserved;
}

float Curiosity::getMaxPredictionErrorObserved() const {
    return pImpl->maxPredictionErrorObserved;
}

} // namespace nlm
