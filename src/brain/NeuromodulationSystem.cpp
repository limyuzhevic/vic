#include "NeuromodulationSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct NeuromodulationSystem::Impl {
    std::shared_ptr<Config> config;
    Brain* brain = nullptr;
    
    // Neuromodulation system components
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<Novelty> novelty;
    std::unique_ptr<PredictionError> predictionError;
    
    // Neuromodulation levels
    float dopamineLevel = 0.0f;
    float curiosityLevel = 0.0f;
    float noveltyLevel = 0.0f;
    float predictionErrorValue = 0.0f;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg) {
        dopamineLevel = 0.0f;
        curiosityLevel = 0.0f;
        noveltyLevel = 0.0f;
        predictionErrorValue = 0.0f;
    }
};

NeuromodulationSystem::NeuromodulationSystem(std::shared_ptr<Config> config) : pImpl(std::make_unique<Impl>(config)), brain_(nullptr) {
}

NeuromodulationSystem::~NeuromodulationSystem() = default;

NeuromodulationSystem::NeuromodulationSystem(NeuromodulationSystem&& other) noexcept : pImpl(std::move(other.pImpl)), brain_(other.brain_) {
    other.brain_ = nullptr;
}

NeuromodulationSystem& NeuromodulationSystem::operator=(NeuromodulationSystem&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        brain_ = other.brain_;
        other.brain_ = nullptr;
    }
    return *this;
}

bool NeuromodulationSystem::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("NeuromodulationSystem: Cannot initialize with null brain");
        return false;
    }
    
    brain_ = brain;
    pImpl->brain = brain;
    
    // Initialize neuromodulation components
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->novelty = std::make_unique<Novelty>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    
    // Initialize neuromodulation components
    pImpl->dopamine->initialize(brain);
    pImpl->curiosity->initialize(brain);
    pImpl->novelty->initialize(brain);
    pImpl->predictionError->initialize(brain);
    
    NLM_LOG_INFO("NeuromodulationSystem initialized successfully");
    return true;
}

void NeuromodulationSystem::update(TimestepDuration dt) {
    // Update neuromodulation components
    if (pImpl->dopamine) {
        pImpl->dopamine->update(dt);
        pImpl->dopamineLevel = pImpl->dopamine->getLevel();
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->update(dt);
        pImpl->curiosityLevel = pImpl->curiosity->getLevel();
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->update(dt);
        pImpl->noveltyLevel = pImpl->novelty->getLevel();
    }
    
    if (pImpl->predictionError) {
        pImpl->predictionError->update(dt);
        pImpl->predictionErrorValue = pImpl->predictionError->getError();
    }
}

void NeuromodulationSystem::applyToBrain() {
    if (!brain_ || !pImpl->dopamine) {
        return;
    }
    
    // Apply dopamine effects on neural excitability
    float dopamineLevel = pImpl->dopamineLevel;
    if (dopamineLevel > 0.0f) {
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float excitabilityMod = dopamineLevel * 0.5f;
        
        for (auto& region : brain_->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
    
    // Apply neuromodulation factor to plasticity
    if (auto* neuralEngine = brain_->getSpikeSystem()) {
        // This would be handled by NeuralEngine
        pImpl->dopamine->setPlasticityFactor(dopamineLevel);
    }
}

void NeuromodulationSystem::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects based on signal
    float modulation = signal.getLevel();
    
    if (pImpl->dopamine) {
        pImpl->dopamine->setLevel(modulation);
    }
    
    if (pImpl->novelty) {
        pImpl->novelty->setLevel(modulation);
    }
    
    if (pImpl->predictionError) {
        pImpl->predictionError->setError(modulation);
    }
}

float NeuromodulationSystem::getDopamineLevel() const {
    return pImpl->dopamineLevel;
}

float NeuromodulationSystem::getCuriosityLevel() const {
    return pImpl->curiosityLevel;
}

float NeuromodulationSystem::getNoveltyLevel() const {
    return pImpl->noveltyLevel;
}

float NeuromodulationSystem::getPredictionError() const {
    return pImpl->predictionErrorValue;
}

void NeuromodulationSystem::setDopamineLevel(float level) {
    pImpl->dopamineLevel = level;
    if (pImpl->dopamine) {
        pImpl->dopamine->setLevel(level);
    }
}

void NeuromodulationSystem::setCuriosityLevel(float level) {
    pImpl->curiosityLevel = level;
    if (pImpl->curiosity) {
        pImpl->curiosity->setLevel(level);
    }
}

void NeuromodulationSystem::setNoveltyLevel(float level) {
    pImpl->noveltyLevel = level;
    if (pImpl->novelty) {
        pImpl->novelty->setLevel(level);
    }
}

void NeuromodulationSystem::setPredictionError(float error) {
    pImpl->predictionErrorValue = error;
    if (pImpl->predictionError) {
        pImpl->predictionError->setError(error);
    }
}

void NeuromodulationSystem::reset() {
    NLM_LOG_INFO("NeuromodulationSystem reset");
    pImpl->dopamineLevel = 0.0f;
    pImpl->curiosityLevel = 0.0f;
    pImpl->noveltyLevel = 0.0f;
    pImpl->predictionErrorValue = 0.0f;
}

} // namespace nlm