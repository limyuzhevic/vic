// Brain Initialization Implementation
// Refactored from Brain.cpp constructor and initialization methods

#include "BrainInitialization.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Region.hpp"
#include "../brain/Population.hpp"
#include "../brain/Synapse.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cstring>
#include <sstream>

namespace nlm {

// Initialize neural regions and neurons
void BrainInitialization::initializeRegions(Brain::Impl& pImpl, 
                                            size_t neuronCount, 
                                            size_t regionCount, 
                                            float connectionProbability) {
    pImpl->regions.clear();
    pImpl->sensoryNeurons.clear();
    pImpl->motorNeurons.clear();
    pImpl->neuronById.clear();
    pImpl->nextRegionId = RegionId(1);
    pImpl->totalSpikesTotal = 0;
    pImpl->totalSpikesThisStep = 0;
    
    // Get configuration values
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;  // globally unique IDs across regions
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        pImpl->regions.push_back(std::make_unique<NeuralRegion>("Region_" + std::to_string(i + 1)));
        auto& region = pImpl->regions.back();
        region->setNextNeuronIndex(nextNeuronIndex);
        
        // Add populations with 80/20 excitatory/inhibitory split in internal population
        auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
        size_t internalCount = neuronsPerRegion / 2;
        size_t internalExcitatory = internalCount * 4 / 5;
        auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
        auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
        (void)internalPopId; (void)inhibitoryPopId;
        auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        
        // Collect sensory and motor neurons for I/O
        auto* sensoryPop = region->getPopulation(sensoryPopId);
        auto* motorPop = region->getPopulation(motorPopId);
        if (sensoryPop) {
            for (auto* neuron : sensoryPop->getNeurons()) {
                pImpl->sensoryNeurons.push_back(neuron);
            }
        }
        if (motorPop) {
            for (auto* neuron : motorPop->getNeurons()) {
                pImpl->motorNeurons.push_back(neuron);
            }
        }
        
        NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) +
                    ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons");
        nextNeuronIndex += region->getTotalNeuronCount();
    }
    
    // Build the neuron lookup used by synaptic delivery and injection.
    for (auto& region : pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            pImpl->neuronById[neuron->getId().index()] = neuron;
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = pImpl->regions[i].get();
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

// Initialize all integrated systems
void BrainInitialization::initializeIntegratedSystems(Brain::Impl& pImpl, 
                                                       size_t neuronCount) {
    // ========== INITIALIZE INTEGRATED SYSTEMS ==========
    
    // Initialize memory systems
    pImpl->workingMemory->initialize(nullptr);  // Will be set by Brain class
    pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    pImpl->episodicMemory->initialize(nullptr);  // Will be set by Brain class
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    pImpl->associativeMemory->initialize(nullptr);  // Will be set by Brain class
    
    // Initialize prediction system
    pImpl->predictionSystem->initialize(nullptr);  // Will be set by Brain class
    
    // Initialize cognition systems
    pImpl->planner->initialize(nullptr);  // Will be set by Brain class
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(nullptr);  // Will be set by Brain class
    
    pImpl->attention->initialize(nullptr);  // Will be set by Brain class
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(nullptr);  // Will be set by Brain class
    pImpl->curiosity->initialize(nullptr);  // Will be set by Brain class
    
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
    
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void BrainInitialization::logInitializationStats(Brain::Impl& pImpl) {
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(pImpl->regions.size() ? pImpl->regions[0]->getTotalNeuronCount() : 0));
    NLM_LOG_INFO("Total synapses: " + std::to_string(pImpl->regions.size() ? pImpl->regions[0]->getSynapseCount() : 0));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
}
