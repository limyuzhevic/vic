// NeuromodulationSystem.cpp - Implementation of neuromodulation system
#include "NeuromodulationSystem.h"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct NeuromodulationSystem::Impl {
    Brain* brain_ = nullptr;
    float currentDopamine = 0.0f;
    float currentCuriosity = 0.0f;
    float currentNovelty = 0.0f;
    float currentPredictionError = 0.0f;
    float plasticityFactor = 1.0f;
    size_t updateCount = 0;
};

NeuromodulationSystem::NeuromodulationSystem() : pImpl(std::make_unique<Impl>()) {}

NeuromodulationSystem::~NeuromodulationSystem() = default;

bool NeuromodulationSystem::initialize(Brain* brain) {
    if (!brain) return false;
    
    pImpl->brain_ = brain;
    pImpl->updateCount = 0;
    
    // Initialize neuromodulation components
    dopamine_ = std::make_unique<Dopamine>();
    curiosity_ = std::make_unique<Curiosity>();
    novelty_ = std::make_unique<Novelty>();
    predictionError_ = std::make_unique<PredictionError>();
    
    initialized_ = true;
    NLM_LOG_INFO("NeuromodulationSystem initialized");
    return true;
}

void NeuromodulationSystem::update(const TimestepDuration& dt) {
    if (!initialized_ || !pImpl->brain_) return;
    
    pImpl->updateCount++;
    
    // Update each neuromodulator
    if (dopamine_) {
        dopamine_->update(dt);
        pImpl->currentDopamine = dopamine_->getLevel();
    }
    
    if (curiosity_) {
        curiosity_->update(dt);
        pImpl->currentCuriosity = curiosity_->getLevel();
    }
    
    if (novelty_) {
        novelty_->update(dt);
        pImpl->currentNovelty = novelty_->getLevel();
    }
    
    if (predictionError_) {
        predictionError_->update(dt);
        pImpl->currentPredictionError = predictionError_->getLevel();
    }
    
    // Update plasticity factor based on neuromodulators
    pImpl->plasticityFactor = 1.0f;
    if (dopamine_) {
        pImpl->plasticityFactor *= (1.0f + pImpl->currentDopamine * 0.5f);
    }
    
    NLM_LOG_TRACE("NeuromodulationSystem updated");
}

void NeuromodulationSystem::reset() {
    if (dopamine_) dopamine_->reset();
    if (curiosity_) curiosity_->reset();
    if (novelty_) novelty_->reset();
    if (predictionError_) predictionError_->reset();
    
    pImpl->currentDopamine = 0.0f;
    pImpl->currentCuriosity = 0.0f;
    pImpl->currentNovelty = 0.0f;
    pImpl->currentPredictionError = 0.0f;
    pImpl->plasticityFactor = 1.0f;
    pImpl->updateCount = 0;
    
    initialized_ = false;
    NLM_LOG_INFO("NeuromodulationSystem reset");
}

void NeuromodulationSystem::logStatus() const {
    NLM_LOG_INFO("=== Neuromodulation System Status ===");
    NLM_LOG_INFO("Dopamine: " + std::to_string(getDopamineLevel()));
    NLM_LOG_INFO("Curiosity: " + std::to_string(getCuriosityLevel()));
    NLM_LOG_INFO("Novelty: " + std::to_string(getNoveltyLevel()));
    NLM_LOG_INFO("Prediction Error: " + std::to_string(getPredictionErrorLevel()));
    NLM_LOG_INFO("Plasticity Factor: " + std::to_string(getPlasticityFactor()));
}

void NeuromodulationSystem::signalReward(float reward) {
    if (dopamine_) {
        dopamine_->signalReward(reward);
    }
}

void NeuromodulationSystem::signalPredictionError(float error) {
    if (predictionError_) {
        predictionError_->signalPredictionError(error);
    }
}

void NeuromodulationSystem::signalNovelty(float novelty) {
    if (novelty_) {
        novelty_->signalNovelty(novelty);
    }
}

float NeuromodulationSystem::getDopamineLevel() const {
    return dopamine_ ? dopamine_->getLevel() : 0.0f;
}

float NeuromodulationSystem::getCuriosityLevel() const {
    return curiosity_ ? curiosity_->getLevel() : 0.0f;
}

float NeuromodulationSystem::getNoveltyLevel() const {
    return novelty_ ? novelty_->getLevel() : 0.0f;
}

float NeuromodulationSystem::getPredictionErrorLevel() const {
    return predictionError_ ? predictionError_->getLevel() : 0.0f;
}

float NeuromodulationSystem::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

} // namespace nlm
