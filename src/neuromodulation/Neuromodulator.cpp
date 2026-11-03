#include "Neuromodulator.hpp"
#include "src/neuromodulation/Dopamine.cpp"
#include "src/neuromodulation/Curiosity.cpp"
#include "src/neuromodulation/Novelty.cpp"
#include "src/neuromodulation/PredictionError.cpp"
#include "src/neuromodulation/Reward.cpp"
#include "src/brain/Brain.hpp"
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct NeuromodulatorManager::Impl {
    class Brain* brain;
    std::unordered_map<std::string, Neuromodulator*> neuromodulators;
    
    Impl() : brain(nullptr) {}
};

NeuromodulatorManager::NeuromodulatorManager() : pImpl(std::make_unique<Impl>()) {}

NeuromodulatorManager::~NeuromodulatorManager() {
    for (auto& pair : pImpl->neuromodulators) {
        delete pair.second;
    }
}

void NeuromodulatorManager::initialize(class Brain* brain) {
    pImpl->brain = brain;
    
    // Create all neuromodulators
    pImpl->neuromodulators["DA"] = new Dopamine();
    pImpl->neuromodulators["ACh"] = new Acetylcholine();
    pImpl->neuromodulators["NE"] = new Norepinephrine();
    pImpl->neuromodulators["5-HT"] = new Serotonin();
    pImpl->neuromodulators["Curiosity"] = new Curiosity();
    pImpl->neuromodulators["Novelty"] = new Novelty();
    pImpl->neuromodulators["PredictionError"] = new PredictionError();
    pImpl->neuromodulators["Reward"] = new Reward();
    
    // Initialize all neuromodulators with brain reference
    for (auto& pair : pImpl->neuromodulators) {
        pair.second->initialize(brain);
    }
    
    NLM_LOG_INFO("Neuromodulator Manager initialized");
}

void NeuromodulatorManager::updateAll(TimestepDuration dt) {
    for (auto& pair : pImpl->neuromodulators) {
        pair.second->update(dt);
    }
}

void NeuromodulatorManager::applyToBrain(class Brain* brain) {
    if (!brain) return;
    
    // Update all neuromodulators
    updateAll(0.1f); // Default timestep
    
    // Apply neuromodulatory effects to neural activity
    float totalDrive = getTotalDrive();
    
    // Apply dopamine effects
    if (auto* dopamine = dynamic_cast<Dopamine*>(pImpl->neuromodulators["DA"])) {
        float level = dopamine->getLevel();
        float plasticityFactor = dopamine->getPlasticityFactor();
        
        // Modulate neural excitability
        for (auto* neuron : brain->getAllNeurons()) {
            float excitabilityMod = level * 0.5f * plasticityFactor;
            if (excitabilityMod > 0.0f) {
                neuron->injectCurrent(excitabilityMod);
            }
        }
    }
    
    // Apply curiosity effects
    if (auto* curiosity = dynamic_cast<Curiosity*>(pImpl->neuromodulators["Curiosity"])) {
        float level = curiosity->getLevel();
        
        // Modulate action selection based on curiosity
        if (level > 0.5f) {
            brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        } else {
            brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
        }
    }
    
    // Apply novelty effects
    if (auto* novelty = dynamic_cast<Novelty*>(pImpl->neuromodulators["Novelty"])) {
        float level = novelty->getLevel();
        
        // Novelty increases plasticity rates
        if (level > 0.0f) {
            NLM_LOG_INFO("Novelty detected: " + std::to_string(level));
        }
    }
    
    NLM_LOG_INFO("Neuromodulatory effects applied to brain");
}

float NeuromodulatorManager::getTotalDrive() const {
    float drive = 0.0f;
    
    for (const auto& pair : pImpl->neuromodulators) {
        drive += pair.second->getLevel();
    }
    
    return std::min(1.0f, drive / pImpl->neuromodulators.size());
}

void NeuromodulatorManager::signalEnvironmentalChange() {
    if (auto* novelty = dynamic_cast<Novelty*>(pImpl->neuromodulators["Novelty"])) {
        novelty->setLevel(1.0f);
    }
    
    if (auto* predictionError = dynamic_cast<PredictionError*>(pImpl->neuromodulators["PredictionError"])) {
        predictionError->setError(1.0f);
    }
    
    NLM_LOG_INFO("Environmental change signaled to neuromodulation systems");
}

void NeuromodulatorManager::signalReward(float reward) {
    if (auto* dopamine = dynamic_cast<Dopamine*>(pImpl->neuromodulators["DA"])) {
        dopamine->signalReward(reward);
    }
    
    if (auto* curiosity = dynamic_cast<Curiosity*>(pImpl->neuromodulators["Curiosity"])) {
        float currentCuriosity = curiosity->getLevel();
        curiosity->setLevel(currentCuriosity + reward * 0.1f);
    }
}

void NeuromodulatorManager::signalPredictionError(float error) {
    if (auto* predictionError = dynamic_cast<PredictionError*>(pImpl->neuromodulators["PredictionError"])) {
        predictionError->setError(error);
    }
    
    if (auto* curiosity = dynamic_cast<Curiosity*>(pImpl->neuromodulators["Curiosity"])) {
        float currentCuriosity = curiosity->getLevel();
        curiosity->setLevel(currentCuriosity + error * 0.1f);
    }
}

void NeuromodulatorManager::resetAll() {
    for (auto& pair : pImpl->neuromodulators) {
        pair.second->reset();
    }
    
    NLM_LOG_INFO("All neuromodulation systems reset");
}

Neuromodulator* NeuromodulatorManager::getNeuromodulator(const std::string& name) {
    auto it = pImpl->neuromodulators.find(name);
    return it != pImpl->neuromodulators.end() ? it->second : nullptr;
}

} // namespace nlm