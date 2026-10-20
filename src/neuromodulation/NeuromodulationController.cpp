#include "NeuromodulationController.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralRegion.hpp"

namespace nlm {

struct NeuromodulationController::Impl {
    std::shared_ptr<Config> config;
    Brain* brain;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg), brain(nullptr) {
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
    }
    
    ~Impl() = default;
};

NeuromodulationController::NeuromodulationController(std::shared_ptr<Config> config) : 
    pImpl(new Impl(config)),
    dopamineLevel(1.0f),
    curiosityGain(1.0f),
    noveltyThreshold(0.5f) {}

NeuromodulationController::~NeuromodulationController() {
    delete pImpl;
}

bool NeuromodulationController::initialize(Brain* brain) {
    NLM_LOG_INFO("Initializing NeuromodulationController...");
    
    pImpl->brain = brain;
    
    pImpl->novelty->initialize(brain);
    pImpl->curiosity->initialize(brain);
    pImpl->dopamine->initialize(brain);
    
    NLM_LOG_INFO("NeuromodulationController initialized successfully");
    return true;
}

void NeuromodulationController::update(Timestamp time, TimestepDuration timestep) {
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

float NeuromodulationController::getDopamineLevel() const {
    return pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
}

float NeuromodulationController::getCuriosityLevel() const {
    return pImpl->curiosity ? pImpl->curiosity->getLevel() : 0.0f;
}

float NeuromodulationController::getNoveltyLevel() const {
    return pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f;
}

void NeuromodulationController::applyNeuromodulation(const Neuromodulator& signal) {
    float modulation = signal.getLevel();
    
    // Scale neuromodulation effects
    if (pImpl->dopamine) {
        pImpl->dopamine->setLevel(pImpl->dopamine->getLevel() * modulation);
    }
}

void NeuromodulationController::reset() {
    NLM_LOG_INFO("Resetting NeuromodulationController...");
    NLM_LOG_INFO("NeuromodulationController reset complete");
}

} // namespace nlm