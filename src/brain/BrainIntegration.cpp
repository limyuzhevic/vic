// Brain Integration Systems Implementation
// Refactored integration systems (memory, prediction, cognition, neuromodulation)

#include "BrainIntegration.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Integration systems initialization and management
void BrainIntegration::initializeAllSystems(Brain::Impl& pImpl, size_t neuronCount) {
    // Initialize memory systems
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    
    // Initialize prediction system
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
    
    // Initialize cognition systems
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->attention = std::make_unique<AttentionalSelection>();
    
    // Initialize neuromodulation systems
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->novelty = std::make_unique<Novelty>();
    
    // Initialize development system
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
    
    // Initialize neural computation components (Phase 2)
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
}

void BrainIntegration::configurePlasticitySystems(Brain::Impl& pImpl) {
    // Configure STDP parameters
    float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = pImpl->config->getOr<float>("stdp_tau", 20.0f);
    pImpl->stdp->configure(ltpWeight, ltdWeight, tau);
    
    // Configure structural plasticity
    float synaptogenesisRate = pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = pImpl->config->getOr<float>("pruning_rate", 0.00001f);
    pImpl->structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    pImpl->structuralPlasticity->setPruningRate(pruningRate);
}

void BrainIntegration::configureSystemIntervals(Brain::Impl& pImpl) {
    // Get integration intervals from config
    pImpl->replayInterval = static_cast<size_t>(pImpl->config->getOr<int64_t>("replay_interval", 100));
    pImpl->consolidationInterval = static_cast<size_t>(pImpl->config->getOr<int64_t>("consolidation_interval", 1000));
}

void BrainIntegration::configureCheckpointManager(Brain::Impl& pImpl) {
    // Initialize checkpoint manager
    pImpl->checkpointManager = std::make_unique<CheckpointManager>();
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void BrainIntegration::initializeRandomGenerator(Brain::Impl& pImpl) {
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = pImpl->config->get<int64_t>("random_seed")) {
        seed = *seedOpt;
    }
    pImpl->rng = std::make_unique<RandomGenerator>(seed);
}

void BrainIntegration::registerSystemEventHandlers(Brain::Impl& pImpl) {
    // Register spike handlers once; re-initialising used to register them
    // again and double-count every spike.
    if (!pImpl->spikeHandlersRegistered) {
        pImpl->spikeHandlersRegistered = true;
        
        // Register spike handlers for event-driven processing
        pImpl->spikeSystem->registerHandler([](const DetailedSpikeEvent& event) {
            // Count spikes (simple implementation)
        });
        
        // Register delayed spike handler to deliver synaptic input
        pImpl->spikeSystem->registerDelayedHandler([](const DelayedSpikeEvent& event) {
            // Deliver synaptic input (implementation would need access to neuron lookup)
        });
    }
}

void BrainIntegration::logSystemInitializationStats(Brain::Impl& pImpl) {
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(pImpl->regions.size() ? pImpl->regions[0]->getTotalNeuronCount() : 0));
    NLM_LOG_INFO("Total synapses: " + std::to_string(pImpl->regions.size() ? pImpl->regions[0]->getSynapseCount() : 0));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
}

void BrainIntegration::applyNeuromodulationEffects(Brain::Impl& pImpl) {
    // Apply neuromodulation effects on neural excitability
    if (pImpl->dopamine) {
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
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

void BrainIntegration::updateAllIntegrationSystems(Brain::Impl& pImpl, 
                                                   TimestepDuration timestep,
                                                   SimulationStep currentStep,
                                                   Timestamp currentTime) {
    // Update working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(timestep);
    }
    
    // Update neuromodulation systems
    if (pImpl->novelty) {
        pImpl->novelty->update(timestep);
    }
    
    if (pImpl->curiosity) {
        pImpl->curiosity->update(
            pImpl->novelty ? pImpl->novelty->getLevel() : 0.0f,
            pImpl->predictionError ? pImpl->predictionError->getError() : 0.0f,
            timestep
        );
    }
    
    // Update cognition systems
    if (pImpl->planner) {
        pImpl->planner->update(timestep);
    }
    
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->update(timestep);
    }
    
    if (pImpl->attention) {
        pImpl->attention->update(timestep);
    }
    
    // Update development system
    if (pImpl->developmentSystem) {
        pImpl->developmentSystem->update(nullptr, *pImpl->rng, timestep * 1000);
    }
}

} // namespace nlm
