#include "NeuromodulatorManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct NeuromodulatorManager::Impl {
    class Brain* brain;
    std::unordered_map<std::string, Neuromodulator*> modulators;
    std::unordered_map<std::string, bool> modulatorEnabled;
    
    // Integration with brain systems
    float compositeCuriosity;
    float compositeNovelty;
    float compositePredictionError;
    float totalNeuromodulation;
    
    // Configuration for advanced integration
    float curiosityThreshold;
    float noveltyThreshold;
    float predictionErrorThreshold;
    float neuromodulationGain;
    
    // Temporal integration
    float temporalDecay;
    float learningRate;
    
    // Random number generator for exploration
    std::mt19937 rng;
    
    Impl() : brain(nullptr), compositeCuriosity(0.0f), compositeNovelty(0.0f),
             compositePredictionError(0.0f), totalNeuromodulation(0.0f),
             curiosityThreshold(0.5f), noveltyThreshold(0.5f),
             predictionErrorThreshold(0.5f), neuromodulationGain(1.0f),
             temporalDecay(0.9f), learningRate(0.1f)
    {
        std::random_device rd;
        rng.seed(rd());
    }
};

NeuromodulatorManager::NeuromodulatorManager() : pImpl(new Impl) {}

NeuromodulatorManager::~NeuromodulatorManager() {
    for (auto& pair : pImpl->modulators) {
        delete pair.second;
    }
    pImpl->modulators.clear();
}

void NeuromodulatorManager::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Enable all neuromodulators by default
    for (auto& pair : pImpl->modulators) {
        pImpl->modulatorEnabled[pair.first] = true;
    }
    
    NLM_LOG_INFO("NeuromodulatorManager initialized with brain integration");
}

void NeuromodulatorManager::updateAll(TimestepDuration dt) {
    // Update individual neuromodulators
    for (auto& pair : pImpl->modulators) {
        if (pImpl->modulatorEnabled[pair.first]) {
            pair.second->update(dt);
        }
    }
    
    // Compute composite signals
    computeCompositeSignals();
    
    // Apply neuromodulation to brain
    if (pImpl->brain) {
        applyToBrain(pImpl->brain);
    }
}

void NeuromodulatorManager::addModulator(Neuromodulator* modulator) {
    if (!modulator) return;
    
    const char* name = modulator->getName();
    std::string nameStr(name ? name : "");
    
    if (!nameStr.empty()) {
        // Remove existing modulator with same name
        auto it = pImpl->modulators.find(nameStr);
        if (it != pImpl->modulators.end()) {
            delete it->second;
            pImpl->modulators.erase(it);
        }
        
        pImpl->modulators[nameStr] = modulator;
        pImpl->modulatorEnabled[nameStr] = true;
        
        NLM_LOG_DEBUG("Added neuromodulator: %s", nameStr.c_str());
    }
}

Neuromodulator* NeuromodulatorManager::getModulator(const std::string& name) {
    auto it = pImpl->modulators.find(name);
    return it != pImpl->modulators.end() ? it->second : nullptr;
}

const Neuromodulator* NeuromodulatorManager::getModulator(const std::string& name) const {
    auto it = pImpl->modulators.find(name);
    return it != pImpl->modulators.end() ? it->second : nullptr;
}

void NeuromodulatorManager::computeCompositeSignals() {
    // Compute composite curiosity level
    float totalCuriosity = 0.0f;
    float curiosityWeight = 0.0f;
    
    auto curiosityMod = getModulator("DA"); // Dopamine also contributes to curiosity
    if (curiosityMod && pImpl->modulatorEnabled["DA"]) {
        totalCuriosity += curiosityMod->getLevel() * 0.3f;
        curiosityWeight += 0.3f;
    }
    
    // Get curiosity from dedicated system
    if (pImpl->brain) {
        auto curiositySystem = pImpl->brain->getCuriosity();
        if (curiositySystem) {
            totalCuriosity += curiositySystem->getLevel() * 0.7f;
            curiosityWeight += 0.7f;
        }
    }
    
    pImpl->compositeCuriosity = curiosityWeight > 0.0f ? totalCuriosity / curiosityWeight : 0.0f;
    
    // Compute composite novelty
    float totalNovelty = 0.0f;
    float noveltyWeight = 0.0f;
    
    if (pImpl->brain) {
        auto noveltySystem = pImpl->brain->getNovelty();
        if (noveltySystem) {
            totalNovelty += noveltySystem->getLevel();
            noveltyWeight += 1.0f;
        }
    }
    
    pImpl->compositeNovelty = noveltyWeight > 0.0f ? totalNovelty / noveltyWeight : 0.0f;
    
    // Compute composite prediction error
    float totalPredictionError = 0.0f;
    float predictionErrorWeight = 0.0f;
    
    if (pImpl->brain) {
        auto predErrorSystem = pImpl->brain->getPredictionErrorSignal();
        if (predErrorSystem) {
            totalPredictionError += predErrorSystem->getError();
            predictionErrorWeight += 1.0f;
        }
    }
    
    pImpl->compositePredictionError = predictionErrorWeight > 0.0f ? totalPredictionError / predictionErrorWeight : 0.0f;
    
    // Compute total neuromodulation
    pImpl->totalNeuromodulation = 0.0f;
    float totalWeight = 0.0f;
    
    for (const auto& pair : pImpl->modulators) {
        if (pImpl->modulatorEnabled[pair.first]) {
            pImpl->totalNeuromodulation += pair.second->getLevel() * 0.5f;
            totalWeight += 0.5f;
        }
    }
    
    if (totalWeight > 0.0f) {
        pImpl->totalNeuromodulation /= totalWeight;
    }
}

void NeuromodulatorManager::applyToBrain(Brain* brain) {
    if (!brain) return;
    
    // Apply curiosity to action selection
    if (pImpl->compositeCuriosity > pImpl->curiosityThreshold &&
        brain->getCuriosity()) {
        // Modify action selection probabilities based on curiosity
        brain->getCuriosity()->setNoveltyWeight(
            std::min(1.0f, pImpl->compositeCuriosity * 2.0f));
    }
    
    // Apply novelty to attention modulation
    if (pImpl->compositeNovelty > pImpl->noveltyThreshold) {
        // Enhance sensitivity to novel stimuli
        if (brain->getNovelty()) {
            brain->getNovelty()->setLevel(
                std::min(1.0f, pImpl->compositeNovelty * 1.5f));
        }
    }
    
    // Apply prediction error to learning
    if (std::abs(pImpl->compositePredictionError) > pImpl->predictionErrorThreshold) {
        // Modulate learning rate based on prediction error
        if (brain->getPredictionErrorSignal()) {
            brain->getPredictionErrorSignal()->setLearningRate(
                std::min(1.0f, std::abs(pImpl->compositePredictionError) * 2.0f));
        }
    }
    
    // Apply overall neuromodulation to plasticity
    if (pImpl->totalNeuromodulation > 0.0f) {
        // Get dopamine system for plasticity modulation
        auto dopamine = brain->getDopamine();
        if (dopamine) {
            dopamine->setLevel(pImpl->totalNeuromodulation);
        }
    }
}

float NeuromodulatorManager::getCompositeNeuromodulationLevel() const {
    return pImpl->totalNeuromodulation;
}

float NeuromodulatorManager::getCuriosityLevel() const {
    return pImpl->compositeCuriosity;
}

float NeuromodulatorManager::getNoveltyLevel() const {
    return pImpl->compositeNovelty;
}

float NeuromodulatorManager::getPredictionError() const {
    return pImpl->compositePredictionError;
}

float NeuromodulatorManager::getNeuromodulationLevel() const {
    return pImpl->totalNeuromodulation;
}

float NeuromodulatorManager::getTotalNeuromodulation() const {
    return pImpl->totalNeuromodulation;
}

void NeuromodulatorManager::enableModulator(const std::string& name, bool enable) {
    auto it = pImpl->modulators.find(name);
    if (it != pImpl->modulators.end()) {
        pImpl->modulatorEnabled[name] = enable;
        if (enable) {
            NLM_LOG_DEBUG("Enabled neuromodulator: %s", name.c_str());
        } else {
            NLM_LOG_DEBUG("Disabled neuromodulator: %s", name.c_str());
        }
    }
}

bool NeuromodulatorManager::isModulatorEnabled(const std::string& name) const {
    auto it = pImpl->modulators.find(name);
    if (it == pImpl->modulators.end()) return false;
    
    auto enabledIt = pImpl->modulatorEnabled.find(name);
    return enabledIt != pImpl->modulatorEnabled.end() ? enabledIt->second : false;
}

void NeuromodulatorManager::resetAll() {
    for (auto& pair : pImpl->modulators) {
        if (pImpl->modulatorEnabled[pair.first]) {
            pair.second->reset();
        }
    }
    
    // Reset composite values
    pImpl->compositeCuriosity = 0.0f;
    pImpl->compositeNovelty = 0.0f;
    pImpl->compositePredictionError = 0.0f;
    pImpl->totalNeuromodulation = 0.0f;
    
    NLM_LOG_INFO("NeuromodulatorManager reset all neuromodulators");
}

std::vector<std::string> NeuromodulatorManager::getActiveModulatorNames() const {
    std::vector<std::string> activeNames;
    for (const auto& pair : pImpl->modulators) {
        if (pImpl->modulatorEnabled[pair.first]) {
            activeNames.push_back(pair.first);
        }
    }
    return activeNames;
}

std::vector<NeuromodulatorManager::NeuromodulatorStats> NeuromodulatorManager::getAllStats() const {
    std::vector<NeuromodulatorStats> stats;
    
    for (const auto& pair : pImpl->modulators) {
        NeuromodulatorStats stat;
        stat.name = pair.first;
        stat.level = pair.second->getLevel();
        stat.plasticityFactor = pair.second->getPlasticityFactor();
        stat.enabled = pImpl->modulatorEnabled.count(pair.first) ? pImpl->modulatorEnabled.at(pair.first) : false;
        stats.push_back(stat);
    }
    
    return stats;
}

} // namespace nlm