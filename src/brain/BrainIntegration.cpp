#include "BrainIntegration.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

BrainIntegration::BrainIntegration() = default;

BrainIntegration::~BrainIntegration() = default;

bool BrainIntegration::initialize(BrainCore* core) {
    this->core = core;
    
    // Initialize memory systems
    workingMemory = std::make_unique<NeuralWorkingMemory>();
    episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize prediction system
    predictionSystem = std::make_unique<PredictionSystem>();
    
    // Initialize cognition systems
    planner = std::make_unique<NeuralPlanner>();
    conceptFormation = std::make_unique<ConceptFormation>();
    attention = std::make_unique<AttentionalSelection>();
    
    // Initialize development system
    developmentSystem = std::make_unique<DevelopmentSystem>();
    
    // Initialize neuromodulation systems
    dopamine = std::make_unique<Dopamine>();
    curiosity = std::make_unique<Curiosity>();
    novelty = std::make_unique<Novelty>();
    predictionError = std::make_unique<PredictionError>();
    
    // Set up cross-references
    if (workingMemory) workingMemory->initialize(core);
    if (episodicMemory) episodicMemory->initialize(core);
    if (associativeMemory) associativeMemory->initialize(core);
    if (predictionSystem) predictionSystem->initialize(core);
    if (planner) planner->initialize(core);
    if (conceptFormation) conceptFormation->initialize(core);
    if (attention) attention->initialize(core);
    if (developmentSystem) developmentSystem->initialize(core);
    if (dopamine) dopamine->initialize(core);
    if (curiosity) curiosity->initialize(core);
    if (novelty) novelty->initialize(core);
    if (predictionError) predictionError->initialize(core);
    
    NLM_LOG_INFO("Integrated brain systems initialized (Phase 6)");
    return true;
}

void BrainIntegration::step(BrainCore* core, TimestepDuration timestep) {
    this->core = core;
    
    // Update memory systems
    updateMemorySystems(core, timestep);
    
    // Update prediction system
    updatePredictionSystem(core, timestep);
    
    // Update cognition systems
    updateCognitionSystems(core, timestep);
    
    // Update development
    updateDevelopment(core, timestep);
    
    // Apply neuromodulation
    applyNeuromodulation(core);
    
    // Apply development effects
    applyDevelopmentEffects(core);
}

void BrainIntegration::applyNeuromodulation(BrainCore* core) {
    if (!core || !dopamine) return;
    
    // Update novelty detection
    if (novelty) {
        novelty->update(timestep);
    }
    
    // Update curiosity
    if (curiosity) {
        curiosity->update(
            novelty ? novelty->getLevel() : 0.0f,
            predictionError ? predictionError->getError() : 0.0f,
            timestep
        );
    }
    
    // Update dopamine (reward prediction error)
    if (dopamine) {
        dopamine->update(timestep);
        
        // Apply dopamine effects on neural excitability
        float dopamineLevel = dopamine->getLevel();
        for (auto* neuron : core->getSensoryNeurons()) {
            // Dopamine modulates excitability by injecting additional current
            // Higher dopamine increases excitability (lower effective threshold)
            float excitabilityMod = dopamineLevel * 0.5f;
            if (excitabilityMod > 0.0f) {
                neuron->injectCurrent(excitabilityMod);
            }
        }
    }
}

void BrainIntegration::updateMemorySystems(BrainCore* core, TimestepDuration timestep) {
    if (!workingMemory) return;
    
    // Update working memory
    workingMemory->update(timestep);
    
    // Store active neurons in working memory
    for (auto* neuron : core->getSensoryNeurons()) {
        const auto& state = neuron->getState();
        bool isActive = (state.firingState == FiringState::Refractory &&
                        state.lastSpikeTime >= 0.0f &&
                        std::abs(static_cast<float>(core->currentTime()) - state.lastSpikeTime) < 0.002f * 2.0f);
        
        if (isActive) {
            workingMemory->storeToNeuron(neuron->getId(), 1.0f);
        }
    }
    
    // Update episodic memory periodically
    if (episodicMemory) {
        // Store episode every 10 steps
        if (core->getSpikesThisStep() > 0) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = core->getSpikesThisStep();
            episode.reward = dopamine ? dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto* neuron : core->getSensoryNeurons()) {
                if (neuron->isFiring() || 
                    std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                    episode.activeNeurons.push_back(neuron->getId());
                    episode.neuronActivations.push_back(
                        std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                }
            }
            
            episodicMemory->storeEpisode(episode);
        }
    }
}

void BrainIntegration::updatePredictionSystem(BrainCore* core, TimestepDuration timestep) {
    if (!predictionSystem) return;
    
    // The prediction system would be updated with sensory observations
    // For now, just track prediction error history
    // predictionSystem->update(core->getSensoryNeurons(), timestep);
}

void BrainIntegration::updateCognitionSystems(BrainCore* core, TimestepDuration timestep) {
    if (!planner) return;
    
    // Update attention system
    if (attention) {
        attention->update(timestep);
        
        // Apply attention to working memory winners
        if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
            attention->processCompetition(competitors);
        }
    }
    
    // Update concept formation
    if (conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
        // conceptFormation->update(core->getSensoryNeurons(), timestep);
    }
    
    // Update neural planner
    if (planner) {
        planner->update(timestep);
    }
}

void BrainIntegration::updateDevelopment(BrainCore* core, TimestepDuration timestep) {
    if (!developmentSystem) return;
    
    // Development updates structural plasticity every 1000 steps
    static SimulationStep lastDevelopmentUpdate = 0;
    if (core->getSpikesThisStep() % 1000 == 0) {
        developmentSystem->update(core, *core->getConfig()->getRandomGenerator(), timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = core->getStructuralPlasticity();
        if (sp) {
            DevelopmentalStage stage = core->getDevelopmentalStage();
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void BrainIntegration::applyDevelopmentEffects(BrainCore* core) {
    if (!developmentSystem) return;
    
    // Apply development effects on neural excitability
    float developmentFactor = 1.0f;
    
    // Get current developmental stage
    DevelopmentalStage stage = core->getDevelopmentalStage();
    switch (stage) {
        case DevelopmentalStage::Initial:
            developmentFactor = 1.0f;  // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            developmentFactor = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            developmentFactor = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            developmentFactor = 0.2f;  // Stable
            break;
    }
    
    // Apply developmental effects to neurons
    for (auto* neuron : core->getSensoryNeurons()) {
        // Development affects neural excitability
        float excitabilityMod = developmentFactor * 0.1f;
        if (excitabilityMod > 0.0f) {
            neuron->injectCurrent(excitabilityMod);
        }
    }
}

} // namespace nlm
