#include "BrainInitialization.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

bool BrainInitialization::initialize(Brain::Impl* pImpl) {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Initialize all components
    setupRegions(pImpl);
    setupConnectivity(pImpl);
    initializeMemorySystems(pImpl);
    initializePredictionSystem(pImpl);
    initializeCognitionSystems(pImpl);
    initializeNeuromodulationSystems(pImpl);
    setupSpikeSystemHandlers(pImpl);
    setupCheckpointManager(pImpl);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(BrainRegionManagement::getTotalNeuronCount(pImpl)));
    NLM_LOG_INFO("Total synapses: " + std::to_string(BrainRegionManagement::getTotalSynapseCount(pImpl)));
    
    return true;
}

void BrainInitialization::setupRegions(Brain::Impl* pImpl) {
    // Create regions
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    
    for (size_t i = 0; i < regionCount; ++i) {
        BrainRegionManagement::addRegion(pImpl, "Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        NeuralRegion* region = BrainRegionManagement::getRegion(pImpl, RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
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
        }
    }
}

void BrainInitialization::setupConnectivity(Brain::Impl* pImpl) {
    // Initialize connectivity with random weights
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    for (size_t i = 0; i < regionCount; ++i) {
        NeuralRegion* region = BrainRegionManagement::getRegion(pImpl, RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void BrainInitialization::initializeMemorySystems(Brain::Impl* pImpl) {
    // Initialize working memory
    pImpl->workingMemory->initialize(nullptr);  // Will be set later
    pImpl->workingMemory->setCapacity(pImpl->config->getOr<size_t>("neuron_count", 1000) / 10);
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(nullptr);  // Will be set later
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(nullptr);  // Will be set later
}

void BrainInitialization::initializePredictionSystem(Brain::Impl* pImpl) {
    // PredictionSystem doesn't have initialize method currently
    // Future enhancement: Add initialization for prediction system
}

void BrainInitialization::initializeCognitionSystems(Brain::Impl* pImpl) {
    // Initialize cognition systems
    pImpl->planner->initialize(nullptr);  // Will be set later
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(nullptr);  // Will be set later
    
    pImpl->attention->initialize(nullptr);  // Will be set later
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
}

void BrainInitialization::initializeNeuromodulationSystems(Brain::Impl* pImpl) {
    pImpl->novelty->initialize(nullptr);  // Will be set later
    pImpl->curiosity->initialize(nullptr);  // Will be set later
    
    // Dopamine initialization happens separately
}

void BrainInitialization::setupSpikeSystemHandlers(Brain::Impl* pImpl) {
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([pImpl](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
    pImpl->spikeSystem->registerDelayedHandler([pImpl](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        for (auto& region : pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    // Apply synaptic weight as current
                    MembranePotential synapticCurrent = event.weight * 10.0f;  // Scale factor
                    if (event.is_excitatory) {
                        neuron->receiveExcitatoryInput(synapticCurrent);
                    } else {
                        neuron->receiveInhibitoryInput(-synapticCurrent);
                    }
                    return;
                }
            }
        }
    });
}

void BrainInitialization::setupCheckpointManager(Brain::Impl* pImpl) {
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}
