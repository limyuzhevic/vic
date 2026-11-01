#include "MemorySystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"

namespace nlm {

struct NeuromodulationSystem::Impl {
    Brain* brain_;
    std::shared_ptr<Config> config_;
    
    float currentNoveltyLevel_;
    float currentCuriosityLevel_;
    float currentDopamineLevel_;
    float currentPredictionError_;
    float plasticityFactor_;
    
    Impl() : brain_(nullptr), currentNoveltyLevel_(0.0f), currentCuriosityLevel_(0.0f),
             currentDopamineLevel_(0.0f), currentPredictionError_(0.0f), plasticityFactor_(1.0f) {}
};

NeuromodulationSystem::NeuromodulationSystem() : pImpl(std::make_unique<Impl>()) {}
NeuromodulationSystem::~NeuromodulationSystem() = default;

void NeuromodulationSystem::initialize(Brain* brain, std::shared_ptr<Config> config) {
    pImpl->brain_ = brain;
    pImpl->config_ = config;
}

void NeuromodulationSystem::update(TimestepDuration dt, float noveltyLevel, float predictionError) {
    pImpl->currentNoveltyLevel_ = noveltyLevel;
    pImpl->currentPredictionError_ = predictionError;
    
    // Update novelty detection
    if (pImpl->brain_ && pImpl->brain_->getNovelty()) {
        pImpl->brain_->getNovelty()->update(dt);
    }
    
    // Update curiosity
    if (pImpl->brain_ && pImpl->brain_->getCuriosity()) {
        pImpl->brain_->getCuriosity()->update(noveltyLevel, predictionError, dt);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->brain_ && pImpl->brain_->getDopamine()) {
        pImpl->brain_->getDopamine()->update(dt);
    }
    
    // Calculate plasticity factor based on neuromodulators
    pImpl->plasticityFactor_ = 1.0f;
    if (pImpl->brain_ && pImpl->brain_->getDopamine()) {
        pImpl->plasticityFactor_ = pImpl->brain_->getDopamine()->getPlasticityFactor();
    }
}

void NeuromodulationSystem::applyEffects(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                         Timestamp currentTime) {
    if (pImpl->brain_ && pImpl->brain_->getDopamine()) {
        float dopamineLevel = pImpl->brain_->getDopamine()->getLevel();
        float excitabilityMod = dopamineLevel * 0.5f;
        
        if (excitabilityMod > 0.0f) {
            for (auto& region : regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

float NeuromodulationSystem::getNoveltyLevel() const {
    return pImpl->currentNoveltyLevel_;
}

float NeuromodulationSystem::getCuriosityLevel() const {
    return pImpl->currentCuriosityLevel_;
}

float NeuromodulationSystem::getDopamineLevel() const {
    return pImpl->currentDopamineLevel_;
}

float NeuromodulationSystem::getPredictionErrorLevel() const {
    return pImpl->currentPredictionError_;
}

float NeuromodulationSystem::getPlasticityFactor() const {
    return pImpl->plasticityFactor_;
}

void NeuromodulationSystem::reset() {
    pImpl->currentNoveltyLevel_ = 0.0f;
    pImpl->currentCuriosityLevel_ = 0.0f;
    pImpl->currentDopamineLevel_ = 0.0f;
    pImpl->currentPredictionError_ = 0.0f;
    pImpl->plasticityFactor_ = 1.0f;
}

} // namespace nlm
