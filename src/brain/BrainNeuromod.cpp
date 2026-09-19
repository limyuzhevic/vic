#include "BrainNeuromod.hpp"

namespace nlm {

BrainNeuromod::BrainNeuromod(std::shared_ptr<Config> cfg)
    : config(cfg)
    , dopamine(nullptr)
    , curiosity(nullptr)
    , predictionError(nullptr)
    , novelty(nullptr)
    , dopamineLevel(0.0f)
    , curiosityLevel(0.0f)
    , noveltyLevel(0.0f)
    , dopaminePlasticityFactor(1.0f)
    , curiosityModulationFactor(1.0f)
{
    // Initialize neuromodulation systems
    dopamine = std::make_unique<Dopamine>();
    curiosity = std::make_unique<Curiosity>();
    predictionError = std::make_unique<PredictionError>();
    novelty = std::make_unique<Novelty>();
    
    // Configure from config
    initializeFromConfig();
}

BrainNeuromod::~BrainNeuromod() = default;

void BrainNeuromod::initialize(Brain* brain) {
    // Already initialized in constructor
}

void BrainNeuromod::update(TimestepDuration timestep) {
    // Update all neuromodulation systems
    if (dopamine) dopamine->update(timestep);
    if (curiosity) curiosity->update(timestep);
    if (predictionError) predictionError->update(timestep);
    if (novelty) novelty->update(timestep);
    
    // Update neuromodulation factors
    updateModulationFactors();
}

void BrainNeuromod::applyToNeuralExcitability(Brain* brain) {
    if (!brain) return;
    
    // Apply dopamine effects on neural excitability
    applyDopamineEffects(brain);
    
    // Apply other neuromodulation effects
    applyCuriosityEffects();
    applyNoveltyEffects();
    applyPredictionErrorEffects();
}

void BrainNeuromod::applyDopamineEffects(Brain* brain) {
    if (!brain || !dopamine) return;
    
    // Get current dopamine level
    float dopamineLevel = dopamine->getLevel();
    if (dopamineLevel == 0.0f) return;
    
    // Apply dopamine effects on neural excitability
    // Dopamine modulates neural excitability by adjusting effective current injection
    // Higher dopamine increases excitability (lower effective threshold)
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        // Get all neurons and apply current injection
        for (auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(excitabilityMod);
                }
            }
        }
    }
}

void BrainNeuromod::applyCuriosityEffects() {
    if (!curiosity) return;
    
    // Apply curiosity effects - increases exploration
    // TODO: Implement curiosity effects
}

void BrainNeuromod::applyNoveltyEffects() {
    if (!novelty) return;
    
    // Apply novelty effects - modulates attention and memory
    // TODO: Implement novelty effects
}

void BrainNeuromod::applyPredictionErrorEffects() {
    if (!predictionError) return;
    
    // Apply prediction error effects - learning signal
    // TODO: Implement prediction error effects
}

void BrainNeuromod::updateModulationFactors() {
    // Update neuromodulation factors for plasticity
    dopaminePlasticityFactor = (dopamine ? dopamine->getPlasticityFactor() : 1.0f);
    curiosityModulationFactor = (curiosity ? curiosity->getPlasticityFactor() : 1.0f);
}

void BrainNeuromod::reset() {
    if (dopamine) dopamine->reset();
    if (curiosity) curiosity->reset();
    if (predictionError) predictionError->reset();
    if (novelty) novelty->reset();
    
    setDopamineLevel(0.0f);
    setCuriosityLevel(0.0f);
    setNoveltyLevel(0.0f);
    setDopaminePlasticityFactor(1.0f);
    setCuriosityModulationFactor(1.0f);
}

void BrainNeuromod::logStatus() const {
    // TODO: Add logging implementation
}

void BrainNeuromod::initializeFromConfig() {
    if (!config) return;
    
    // Get neuromodulation parameters from config
    // For now, use defaults
}

} // namespace nlm