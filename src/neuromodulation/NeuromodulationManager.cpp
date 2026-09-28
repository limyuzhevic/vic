#include "NeuromodulationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct NeuromodulationManager::Impl {
    Brain* brain = nullptr;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Configuration
    float baselineDopamine = 0.5f;
    float noveltySensitivity = 1.0f;
    float curiosityWeight = 0.5f;
    float predictionErrorThreshold = 0.1f;
    
    float stdpLearningRate = 1.0f;
    float hebbianLearningRate = 1.0f;
    
    // State
    float currentDopamineLevel = 0.5f;
    float currentCuriosityLevel = 0.0f;
    float currentNoveltyLevel = 0.0f;
    float currentPredictionErrorLevel = 0.0f;
    
    std::vector<float> dopamineHistory;
    std::vector<float> curiosityHistory;
    std::vector<float> noveltyHistory;
    std::vector<float> predictionErrorHistory;
    
    double lastUpdateTime = 0.0;
    double lastNeuromodulationTime = 0.0;
    
    // Integration state
    bool initialized = false;
};

NeuromodulationManager::NeuromodulationManager() : pImpl(std::make_unique<Impl>()) {}

NeuromodulationManager::~NeuromodulationManager() = default;

void NeuromodulationManager::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize neuromodulators
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->novelty = std::make_unique<Novelty>();
    
    // Initialize with brain reference
    if (pImpl->dopamine) pImpl->dopamine->initialize(pImpl->brain);
    if (pImpl->curiosity) pImpl->curiosity->initialize(pImpl->brain);
    if (pImpl->predictionError) pImpl->predictionError->initialize(pImpl->brain);
    if (pImpl->novelty) pImpl->novelty->initialize(pImpl->brain);
    
    // Set baseline levels
    if (pImpl->dopamine) pImpl->dopamine->setLevel(pImpl->baselineDopamine);
    
    pImpl->initialized = true;
    NLM_LOG_INFO("NeuromodulationManager initialized");
}

void NeuromodulationManager::update(float timestep) {
    if (!pImpl->initialized) return;
    
    // Update each neuromodulator
    if (pImpl->dopamine) {
        pImpl->dopamine->update(timestep);
        pImpl->currentDopamineLevel = pImpl->dopamine->getLevel();
        pImpl->dopamineHistory.push_back(pImpl->currentDopamineLevel);
        // Keep history limited
        if (pImpl->dopamineHistory.size() > 1000) {
            pImpl->dopamineHistory.erase(pImpl->dopamineHistory.begin());
        }
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->update(timestep);
        pImpl->currentNoveltyLevel = pImpl->novelty->getLevel();
        pImpl->noveltyHistory.push_back(pImpl->currentNoveltyLevel);
        if (pImpl->noveltyHistory.size() > 1000) {
            pImpl->noveltyHistory.erase(pImpl->noveltyHistory.begin());
        }
        
        // Update curiosity with novelty input
        if (pImpl->curiosity) {
            float noveltyFactor = std::min(pImpl->currentNoveltyLevel * pImpl->noveltySensitivity, 1.0f);
            pImpl->curiosity->update(noveltyFactor, pImpl->currentPredictionErrorLevel, timestep);
            pImpl->currentCuriosityLevel = pImpl->curiosity->getLevel();
            pImpl->curiosityHistory.push_back(pImpl->currentCuriosityLevel);
            if (pImpl->curiosityHistory.size() > 1000) {
                pImpl->curiosityHistory.erase(pImpl->curiosityHistory.begin());
            }
        }
    }
    
    if (pImpl->predictionError) {
        // Prediction error is updated externally via computeError()
        pImpl->currentPredictionErrorLevel = pImpl->predictionError->getError();
        pImpl->predictionErrorHistory.push_back(pImpl->currentPredictionErrorLevel);
        if (pImpl->predictionErrorHistory.size() > 1000) {
            pImpl->predictionErrorHistory.erase(pImpl->predictionErrorHistory.begin());
        }
    }
    
    pImpl->lastUpdateTime = pImpl->brain->getCurrentStep();
    NLM_LOG_DEBUG("NeuromodulationManager updated");
}

void NeuromodulationManager::configure(float baselineDopamine, float noveltySensitivity,
                                       float curiosityWeight, float predictionErrorThreshold) {
    pImpl->baselineDopamine = baselineDopamine;
    pImpl->noveltySensitivity = noveltySensitivity;
    pImpl->curiosityWeight = curiosityWeight;
    pImpl->predictionErrorThreshold = predictionErrorThreshold;
    
    if (pImpl->dopamine) {
        pImpl->dopamine->setLevel(baselineDopamine);
    }
    
    NLM_LOG_INFO("NeuromodulationManager configured");
}

void NeuromodulationManager::applyToNeurons(std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return;
    
    // Apply dopamine effects on excitability
    if (pImpl->dopamine && pImpl->currentDopamineLevel > 0.0f) {
        float excitabilityMod = pImpl->currentDopamineLevel * 0.5f;
        for (Neuron* neuron : neurons) {
            neuron->injectCurrent(excitabilityMod);
        }
    }
    
    // Apply novelty effects (higher novelty increases overall excitability)
    if (pImpl->currentNoveltyLevel > 0.0f) {
        float noveltyExcitability = pImpl->currentNoveltyLevel * 0.3f;
        for (Neuron* neuron : neurons) {
            neuron->injectCurrent(noveltyExcitability);
        }
    }
    
    // Apply curiosity effects (exploration drive)
    if (pImpl->currentCuriosityLevel > 0.0f) {
        float curiosityExcitability = pImpl->currentCuriosityLevel * 0.2f;
        for (Neuron* neuron : neurons) {
            neuron->injectCurrent(curiosityExcitability);
        }
    }
}

void NeuromodulationManager::processRewardPredictionError(float reward, float prediction) {
    if (pImpl->predictionError) {
        pImpl->predictionError->computeError(prediction, reward);
    }
    if (pImpl->dopamine) {
        pImpl->dopamine->signalRewardPredictionError(reward - prediction);
    }
    
    NLM_LOG_DEBUG("NeuromodulationManager processed reward prediction error");
}

void NeuromodulationManager::processNovelty(float noveltyLevel, 
                                           const class Observation& observation,
                                           const class Observation& previousObservation) {
    if (pImpl->novelty) {
        pImpl->novelty->detectNovelty(observation, previousObservation);
    }
    if (pImpl->currentNoveltyLevel != noveltyLevel) {
        pImpl->currentNoveltyLevel = noveltyLevel;
    }
    
    NLM_LOG_DEBUG("NeuromodulationManager processed novelty");
}

void NeuromodulationManager::updatePlasticity(STDP* stdp, Hebbian* hebbian,
                                             std::vector<Synapse*>& synapses) {
    if (!stdpLearningRate || !hebbianLearningRate) return;
    
    float totalModulation = getPlasticityFactor();
    
    for (Synapse* syn : synapses) {
        // Scale STDP learning rates with neuromodulation
        if (syn->getPlasticityFlags().stdp) {
            float originalLTP = syn->getWeight() > 0 ? 1.0f : -1.0f;
            float modulatedLTP = originalLTP * totalModulation;
            stdp->setLTPWeight(modulatedLTP * pImpl->stdpLearningRate);
            
            float modulatedLTD = -originalLTP * totalModulation;
            stdp->setLTDWeight(modulatedLTD * pImpl->stdpLearningRate);
        }
        
        // Scale Hebbian learning with neuromodulation
        if (syn->getPlasticityFlags().hebbian) {
            float originalHebbian = 1.0f;
            float modulatedHebbian = originalHebbian * totalModulation;
            hebbian->setLearningRate(modulatedHebbian * pImpl->hebbianLearningRate);
        }
    }
}

void NeuromodulationManager::setLearningRates(float stdpLearningRate, float hebbianLearningRate) {
    pImpl->stdpLearningRate = stdpLearningRate;
    pImpl->hebbianLearningRate = hebbianLearningRate;
    NLM_LOG_INFO("NeuromodulationManager learning rates updated");
}

float NeuromodulationManager::getTotalNeuromodulationLevel() const {
    float total = pImpl->currentDopamineLevel;
    total += pImpl->currentCuriosityLevel * pImpl->curiosityWeight;
    total += pImpl->currentNoveltyLevel * pImpl->noveltySensitivity;
    total += pImpl->currentPredictionErrorLevel * 0.5f;
    return total;
}

float NeuromodulationManager::getPlasticityFactor() const {
    // Base factor is 1.0, modulated by dopamine
    return 1.0f + pImpl->currentDopamineLevel * 0.5f;
}

float NeuromodulationManager::getDopamineLevel() const {
    return pImpl->currentDopamineLevel;
}

float NeuromodulationManager::getCuriosityLevel() const {
    return pImpl->currentCuriosityLevel;
}

float NeuromodulationManager::getNoveltyLevel() const {
    return pImpl->currentNoveltyLevel;
}

float NeuromodulationManager::getPredictionErrorMagnitude() const {
    return pImpl->currentPredictionErrorLevel;
}

void NeuromodulationManager::reset() {
    if (pImpl->dopamine) pImpl->dopamine->reset();
    if (pImpl->curiosity) pImpl->curiosity->reset();
    if (pImpl->predictionError) pImpl->predictionError->reset();
    if (pImpl->novelty) pImpl->novelty->reset();
    
    // Reset history
    pImpl->dopamineHistory.clear();
    pImpl->curiosityHistory.clear();
    pImpl->noveltyHistory.clear();
    pImpl->predictionErrorHistory.clear();
    
    // Reset levels
    pImpl->currentDopamineLevel = pImpl->baselineDopamine;
    pImpl->currentCuriosityLevel = 0.0f;
    pImpl->currentNoveltyLevel = 0.0f;
    pImpl->currentPredictionErrorLevel = 0.0f;
    
    if (pImpl->dopamine) {
        pImpl->dopamine->setLevel(pImpl->baselineDopamine);
    }
    
    NLM_LOG_INFO("NeuromodulationManager reset");
}

NeuromodulationManager::NeuromodulationState NeuromodulationManager::getState() const {
    NeuromodulationState state;
    state.dopamineLevel = pImpl->currentDopamineLevel;
    state.curiosityLevel = pImpl->currentCuriosityLevel;
    state.noveltyLevel = pImpl->currentNoveltyLevel;
    state.predictionErrorLevel = pImpl->currentPredictionErrorLevel;
    state.dopamineHistory = pImpl->dopamineHistory;
    state.curiosityHistory = pImpl->curiosityHistory;
    state.noveltyHistory = pImpl->noveltyHistory;
    state.predictionErrorHistory = pImpl->predictionErrorHistory;
    state.lastUpdateTime = pImpl->lastUpdateTime;
    return state;
}

void NeuromodulationManager::restoreState(const NeuromodulationState& state) {
    pImpl->currentDopamineLevel = state.dopamineLevel;
    pImpl->currentCuriosityLevel = state.curiosityLevel;
    pImpl->currentNoveltyLevel = state.noveltyLevel;
    pImpl->currentPredictionErrorLevel = state.predictionErrorLevel;
    pImpl->dopamineHistory = state.dopamineHistory;
    pImpl->curiosityHistory = state.curiosityHistory;
    pImpl->noveltyHistory = state.noveltyHistory;
    pImpl->predictionErrorHistory = state.predictionErrorHistory;
    pImpl->lastUpdateTime = state.lastUpdateTime;
    
    if (pImpl->dopamine) {
        pImpl->dopamine->setLevel(state.dopamineLevel);
    }
    
    NLM_LOG_INFO("NeuromodulationManager state restored");
}

} // namespace nlm
