// NeuromodulationSystem implementation
// Comprehensive system for coordinating multiple neuromodulators

#include "NeuromodulationSystem.hpp"
#include "Dopamine.hpp"
#include "Acetylcholine.hpp"
#include "Norepinephrine.hpp"
#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <map>
#include <chrono>

namespace nlm {

struct NeuromodulationSystem::Impl {
    // Neuromodulator instances
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Acetylcholine> acetylcholine;
    std::unique_ptr<Norepinephrine> norepinephrine;
    std::unique_ptr<Serotonin> serotonin;
    
    // Configuration
    NeuromodulationConfig config;
    
    // Integration components
    float predictionErrorLevel;
    float rewardLevel;
    
    // Overall neuromodulation state
    float totalModulation;
    float neuromodulatorActivity;
    
    // Signal history
    std::map<std::string, std::vector<float>> signalHistory;
    std::chrono::steady_clock::time_point lastUpdate;
    
    Impl() : 
        predictionErrorLevel(0.0f),
        rewardLevel(0.0f),
        totalModulation(0.0f),
        neuromodulatorActivity(0.0f) {
        lastUpdate = std::chrono::steady_clock::now();
    }
};

NeuromodulationSystem::NeuromodulationSystem() : pImpl(new Impl) {}

NeuromodulationSystem::~NeuromodulationSystem() = default;

void NeuromodulationSystem::initialize(const NeuromodulationConfig& config) {
    pImpl->config = config;
    
    // Initialize neuromodulators with configuration
    if (config.dopamine.baseline > 0.0f) {
        pImpl->dopamine = std::make_unique<Dopamine>();
        pImpl->dopamine->setBaseline(config.dopamine.baseline);
        pImpl->dopamine->setPhasicStrength(config.dopamine.phasicStrength);
        pImpl->dopamine->setAdaptationRate(config.dopamine.adaptationRate);
        pImpl->dopamine->setLevel(config.dopamine.baseline);
    }
    
    if (config.acetylcholine.baseline > 0.0f) {
        pImpl->acetylcholine = std::make_unique<Acetylcholine>();
        pImpl->acetylcholine->setBaseline(config.acetylcholine.baseline);
        pImpl->acetylcholine->setCholinergicTone(config.acetylcholine.cholinergicTone);
        pImpl->acetylcholine->setAttentionGain(config.acetylcholine.attentionGain);
        pImpl->acetylcholine->setLevel(config.acetylcholine.baseline);
    }
    
    if (config.norepinephrine.baseline > 0.0f) {
        pImpl->norepinephrine = std::make_unique<Norepinephrine>();
        pImpl->norepinephrine->setBaseline(config.norepinephrine.baseline);
        pImpl->norepinephrine->setArousalLevel(config.norepinephrine.arousalLevel);
        pImpl->norepinephrine->setVigilanceLevel(config.norepinephrine.vigilanceLevel);
        pImpl->norepinephrine->setLocusCoeruleusGain(config.norepinephrine.locusCoeruleusGain);
        pImpl->norepinephrine->setLevel(config.norepinephrine.baseline);
    }
    
    if (config.serotonin.baseline > 0.0f) {
        pImpl->serotonin = std::make_unique<Serotonin>();
        pImpl->serotonin->setBaseline(config.serotonin.baseline);
        pImpl->serotonin->setMoodModulation(config.serotonin.moodModulation);
        pImpl->serotonin->setBehavioralInhibition(config.serotonin.behavioralInhibition);
        pImpl->serotonin->setSocialBehaviorModulation(config.serotonin.socialBehaviorModulation);
        pImpl->serotonin->setLevel(config.serotonin.baseline);
    }
    
    NLM_LOG_INFO("Neuromodulation system initialized with " + std::to_string(getActiveNeuromodulators().size()) + " neuromodulators");
}

void NeuromodulationSystem::update(TimestepDuration dt, float predictionError, float reward) {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - pImpl->lastUpdate).count() / 1000.0f;
    pImpl->lastUpdate = currentTime;
    
    if (elapsed <= 0.0f) return;
    
    // Store history
    pImpl->predictionErrorLevel = predictionError;
    pImpl->rewardLevel = reward;
    
    if (predictionError > 0.0f) {
        pImpl->signalHistory["prediction_error"].push_back(predictionError);
        if (pImpl->signalHistory["prediction_error"].size() > 100) {
            pImpl->signalHistory["prediction_error"].erase(pImpl->signalHistory["prediction_error"].begin());
        }
    }
    
    if (reward > 0.0f) {
        pImpl->signalHistory["reward"].push_back(reward);
        if (pImpl->signalHistory["reward"].size() > 100) {
            pImpl->signalHistory["reward"].erase(pImpl->signalHistory["reward"].begin());
        }
    }
    
    // Update neuromodulators
    if (pImpl->dopamine) {
        pImpl->dopamine->signalRewardPredictionError(predictionError);
        pImpl->dopamine->update(elapsed);
        storeSignalHistory("dopamine", pImpl->dopamine->getLevel());
    }
    
    if (pImpl->acetylcholine) {
        pImpl->acetylcholine->update(elapsed);
        storeSignalHistory("acetylcholine", pImpl->acetylcholine->getLevel());
    }
    
    if (pImpl->norepinephrine) {
        pImpl->norepinephrine->update(elapsed);
        storeSignalHistory("norepinephrine", pImpl->norepinephrine->getLevel());
    }
    
    if (pImpl->serotonin) {
        pImpl->serotonin->update(elapsed);
        storeSignalHistory("serotonin", pImpl->serotonin->getLevel());
    }
    
    // Calculate overall modulation
    calculateModulationLevels();
}

void NeuromodulationSystem::signalReward(float reward) {
    if (pImpl->dopamine) {
        pImpl->dopamine->signalReward(reward);
    }
}

void NeuromodulationSystem::signalPredictionError(float error) {
    pImpl->predictionErrorLevel = error;
    if (pImpl->dopamine) {
        pImpl->dopamine->signalRewardPredictionError(error);
    }
}

void NeuromodulationSystem::storeSignalHistory(const std::string& name, float value) {
    pImpl->signalHistory[name].push_back(value);
    if (pImpl->signalHistory[name].size() > 100) {
        pImpl->signalHistory[name].erase(pImpl->signalHistory[name].begin());
    }
}

void NeuromodulationSystem::calculateModulationLevels() {
    pImpl->totalModulation = 0.0f;
    pImpl->neuromodulatorActivity = 0.0f;
    
    // Calculate total modulation from all neuromodulators
    if (pImpl->dopamine) {
        pImpl->totalModulation += pImpl->dopamine->getTotalModulationStrength();
        pImpl->neuromodulatorActivity += 1.0f;
    }
    
    if (pImpl->acetylcholine) {
        pImpl->totalModulation += pImpl->acetylcholine->getTotalModulationStrength();
        pImpl->neuromodulatorActivity += 1.0f;
    }
    
    if (pImpl->norepinephrine) {
        pImpl->totalModulation += pImpl->norepinephrine->getTotalModulationStrength();
        pImpl->neuromodulatorActivity += 1.0f;
    }
    
    if (pImpl->serotonin) {
        pImpl->totalModulation += pImpl->serotonin->getTotalModulationStrength();
        pImpl->neuromodulatorActivity += 1.0f;
    }
}

float NeuromodulationSystem::getDopamineLevel() const {
    return pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
}

float NeuromodulationSystem::getAcetylcholineLevel() const {
    return pImpl->acetylcholine ? pImpl->acetylcholine->getLevel() : 0.0f;
}

float NeuromodulationSystem::getNorepinephrineLevel() const {
    return pImpl->norepinephrine ? pImpl->norepinephrine->getLevel() : 0.0f;
}

float NeuromodulationSystem::getSerotoninLevel() const {
    return pImpl->serotonin ? pImpl->serotonin->getLevel() : 0.0f;
}

float NeuromodulationSystem::getPredictionErrorLevel() const {
    return pImpl->predictionErrorLevel;
}

float NeuromodulationSystem::getRewardLevel() const {
    return pImpl->rewardLevel;
}

float NeuromodulationSystem::getTotalModulation() const {
    return pImpl->totalModulation;
}

float NeuromodulationSystem::getNeuromodulatorActivity() const {
    return pImpl->neuromodulatorActivity;
}

std::vector<std::string> NeuromodulationSystem::getActiveNeuromodulators() const {
    std::vector<std::string> active;
    
    if (pImpl->dopamine && pImpl->dopamine->isActive()) {
        active.push_back("dopamine");
    }
    
    if (pImpl->acetylcholine && pImpl->acetylcholine->isActive()) {
        active.push_back("acetylcholine");
    }
    
    if (pImpl->norepinephrine && pImpl->norepinephrine->isActive()) {
        active.push_back("norepinephrine");
    }
    
    if (pImpl->serotonin && pImpl->serotonin->isActive()) {
        active.push_back("serotonin");
    }
    
    return active;
}

NeuromodulationConfig& NeuromodulationSystem::getConfig() {
    return pImpl->config;
}

const NeuromodulationConfig& NeuromodulationSystem::getConfig() const {
    return pImpl->config;
}

void NeuromodulationSystem::enableNeuromodulation(bool enabled) {
    pImpl->config.enabled = enabled;
}

bool NeuromodulationSystem::isNeuromodulationEnabled() const {
    return pImpl->config.enabled;
}

NeuromodulationSummary NeuromodulationSystem::getSummary() const {
    NeuromodulationSummary summary;
    
    summary.dopamineLevel = getDopamineLevel();
    summary.acetylcholineLevel = getAcetylcholineLevel();
    summary.norepinephrineLevel = getNorepinephrineLevel();
    summary.serotoninLevel = getSerotoninLevel();
    summary.predictionErrorLevel = getPredictionErrorLevel();
    summary.rewardLevel = getRewardLevel();
    
    summary.totalModulation = getTotalModulation();
    summary.neuromodulatorActivity = getNeuromodulatorActivity();
    summary.activeNeuromodulators = getActiveNeuromodulators();
    
    return summary;
}

} // namespace nlm