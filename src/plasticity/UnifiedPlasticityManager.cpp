#include "UnifiedPlasticityManager.hpp"
#include "../../plasticity/PlasticityRule.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include "../../memory/MemorySystem.hpp"
#include "../../development/DevelopmentSystem.hpp"
#include "../../brain/Brain.hpp"
#include "../../prediction/PredictionSystem.hpp"
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>
#include <iostream>

namespace nlm {

struct UnifiedPlasticityManager::Impl {
    // Brain integration
    Brain* brain;
    
    // Memory integration
    MemorySystem* memorySystem;
    
    // Neuromodulation integration
    Neuromodulator* neuromodulator;
    
    // Development system integration
    DevelopmentSystem* developmentSystem;
    
    // Prediction system integration
    PredictionSystem* predictionSystem;
    
    // Context
    class Context* context;
    
    // Configuration
    bool plasticityEnabled;
    float globalLearningRate;
    
    // Plasticity rules registry
    std::map<std::string, PlasticityRule*> plasticityRules;
    
    // Statistics
    float totalPlasticityWeight;
    size_t activePlasticityRules;
    
    // State tracking
    float homeostaticPressure;
    float adaptationCounter;
    
    Impl() : brain(nullptr), memorySystem(nullptr), neuromodulator(nullptr),
             developmentSystem(nullptr), predictionSystem(nullptr), context(nullptr),
             plasticityEnabled(true), globalLearningRate(0.01f),
             totalPlasticityWeight(0.0f), activePlasticityRules(0),
             homeostaticPressure(0.0f), adaptationCounter(0.0f) {}
};

UnifiedPlasticityManager::UnifiedPlasticityManager() : pImpl(new Impl) {}

UnifiedPlasticityManager::~UnifiedPlasticityManager() {
    // Clean up plasticity rules
    for (auto& pair : pImpl->plasticityRules) {
        delete pair.second;
    }
    pImpl->plasticityRules.clear();
}

void UnifiedPlasticityManager::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void UnifiedPlasticityManager::update(TimestepDuration dt) {
    if (!pImpl->plasticityEnabled || !pImpl->brain) return;
    
    // Update developmental adaptation
    updateDevelopmentalAdaptation();
    
    // Adapt to experience
    adaptToExperience();
    
    // Enforce homeostasis
    enforceHomeostasis();
    
    // Update all plasticity rules
    for (auto& pair : pImpl->plasticityRules) {
        PlasticityRule* rule = pair.second;
        if (rule) {
            // Get synapses from brain
            std::vector<Synapse*> synapses = pImpl->brain->getAllSynapses();
            
            for (Synapse* synapse : synapses) {
                if (synapse) {
                    // Get spike history
                    std::vector<Timestamp> preSpikes = synapse->getPreSpikeHistory();
                    std::vector<Timestamp> postSpikes = synapse->getPostSpikeHistory();
                    
                    // Apply rule
                    rule->update(synapse, preSpikes, postSpikes, dt);
                }
            }
        }
    }
}

void UnifiedPlasticityManager::applyPlasticityRules(Synapse* synapse,
                                                   const std::vector<Timestamp>& preSpikes,
                                                   const std::vector<Timestamp>& postSpikes,
                                                   TimestepDuration dt) {
    if (!synapse || !pImpl->plasticityEnabled) return;
    
    // Apply each registered plasticity rule
    for (auto& pair : pImpl->plasticityRules) {
        PlasticityRule* rule = pair.second;
        if (rule) {
            rule->update(synapse, preSpikes, postSpikes, dt);
        }
    }
}

void UnifiedPlasticityManager::setGlobalLearningRate(float rate) {
    pImpl->globalLearningRate = std::clamp(rate, 0.0f, 1.0f);
}

float UnifiedPlasticityManager::getGlobalLearningRate() const {
    return pImpl->globalLearningRate;
}

void UnifiedPlasticityManager::setPlasticityEnabled(bool enabled) {
    pImpl->plasticityEnabled = enabled;
}

bool UnifiedPlasticityManager::isPlasticityEnabled() const {
    return pImpl->plasticityEnabled;
}

void UnifiedPlasticityManager::setNeuromodulator(Neuromodulator* neuromodulator) {
    pImpl->neuromodulator = neuromodulator;
}

Neuromodulator* UnifiedPlasticityManager::getNeuromodulator() const {
    return pImpl->neuromodulator;
}

void UnifiedPlasticityManager::setMemorySystem(MemorySystem* memory) {
    pImpl->memorySystem = memory;
}

MemorySystem* UnifiedPlasticityManager::getMemorySystem() const {
    return pImpl->memorySystem;
}

void UnifiedPlasticityManager::setDevelopmentSystem(DevelopmentSystem* development) {
    pImpl->developmentSystem = development;
}

DevelopmentSystem* UnifiedPlasticityManager::getDevelopmentSystem() const {
    return pImpl->developmentSystem;
}

void UnifiedPlasticityManager::setPredictionSystem(PredictionSystem* prediction) {
    pImpl->predictionSystem = prediction;
}

PredictionSystem* UnifiedPlasticityManager::getPredictionSystem() const {
    return pImpl->predictionSystem;
}

void UnifiedPlasticityManager::setContext(Context* context) {
    pImpl->context = context;
}

Context* UnifiedPlasticityManager::getContext() const {
    return pImpl->context;
}

void UnifiedPlasticityManager::registerPlasticityRule(PlasticityRule* rule) {
    if (rule) {
        const char* name = rule->getName();
        if (name) {
            pImpl->plasticityRules[name] = rule;
            pImpl->activePlasticityRules++;
        }
    }
}

void UnifiedPlasticityManager::unregisterPlasticityRule(const char* name) {
    if (name && pImpl->plasticityRules.count(name) > 0) {
        delete pImpl->plasticityRules[name];
        pImpl->plasticityRules.erase(name);
        pImpl->activePlasticityRules--;
    }
}

PlasticityRule* UnifiedPlasticityManager::getPlasticityRule(const char* name) const {
    if (name && pImpl->plasticityRules.count(name) > 0) {
        return pImpl->plasticityRules.at(name);
    }
    return nullptr;
}

float UnifiedPlasticityManager::getTotalPlasticityWeight() const {
    return pImpl->totalPlasticityWeight;
}

size_t UnifiedPlasticityManager::getActivePlasticityRules() const {
    return pImpl->activePlasticityRules;
}

void UnifiedPlasticityManager::enforceHomeostasis() {
    if (!pImpl->brain) return;
    
    // Calculate homeostatic pressure
    pImpl->homeostaticPressure = calculateHomeostaticPressure();
    
    // If pressure is too high, apply homeostatic regulation
    if (pImpl->homeostaticPressure > 0.8f) {
        // Scale down plasticity weights
        std::vector<Synapse*> synapses = pImpl->brain->getAllSynapses();
        
        for (Synapse* synapse : synapses) {
            if (synapse) {
                float currentWeight = synapse->getWeight();
                float homeostaticAdjustment = -0.1f * (pImpl->homeostaticPressure - 0.8f);
                float newWeight = currentWeight + homeostaticAdjustment;
                synapse->setWeight(newWeight);
            }
        }
    }
}

float UnifiedPlasticityManager::calculateHomeostaticPressure() const {
    if (!pImpl->brain) return 0.0f;
    
    std::vector<Synapse*> synapses = pImpl->brain->getAllSynapses();
    float maxWeight = 0.0f;
    float minWeight = 1.0f;
    
    for (Synapse* synapse : synapses) {
        if (synapse) {
            float weight = synapse->getWeight();
            maxWeight = std::max(maxWeight, weight);
            minWeight = std::min(minWeight, weight);
        }
    }
    
    float weightSpread = maxWeight - minWeight;
    
    // Normalize to 0-1 range
    return std::min(1.0f, weightSpread * 10.0f);
}

void UnifiedPlasticityManager::adaptToExperience() {
    pImpl->adaptationCounter++;
    
    // Adapt plasticity parameters based on experience
    if (pImpl->adaptationCounter > 100.0f) {
        adjustPlasticityParameters();
        pImpl->adaptationCounter = 0.0f;
    }
}

void UnifiedPlasticityManager::adjustPlasticityParameters() {
    // Gradually reduce global learning rate to stabilize learning
    pImpl->globalLearningRate *= 0.95f;
    
    // If too low, reset to reasonable value
    if (pImpl->globalLearningRate < 0.001f) {
        pImpl->globalLearningRate = 0.01f;
    }
}

void UnifiedPlasticityManager::updateDevelopmentalAdaptation() {
    if (!pImpl->developmentSystem) return;
    
    // Get current developmental stage
    DevelopmentalStage stage = pImpl->developmentSystem->getStage();
    
    // Adapt plasticity based on developmental stage
    switch (stage) {
        case DevelopmentalStage::Initial:
            pImpl->globalLearningRate = 0.05f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->globalLearningRate = 0.08f;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->globalLearningRate = 0.03f;
            break;
        case DevelopmentalStage::Adult:
            pImpl->globalLearningRate = 0.01f;
            break;
        case DevelopmentalStage::Aging:
            pImpl->globalLearningRate = 0.005f;
            break;
    }
}

void UnifiedPlasticityManager::integrateRewardPredictionError() {
    // Integrate reward prediction error signals from prediction system
    if (pImpl->predictionSystem && pImpl->neuromodulator) {
        float predictionError = pImpl->predictionSystem->getPredictionError();
        
        // Modulate plasticity based on prediction error
        if (predictionError > 0.0f) {
            // Positive prediction error (surprise): increase exploration
            pImpl->globalLearningRate *= 1.1f;
        } else {
            // Negative prediction error (expected): decrease exploration
            pImpl->globalLearningRate *= 0.9f;
        }
        
        // Clamp to reasonable bounds
        pImpl->globalLearningRate = std::clamp(pImpl->globalLearningRate, 0.001f, 0.2f);
    }
}

} // namespace nlm
