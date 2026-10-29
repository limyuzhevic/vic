#include "BrainHeaders.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointManager.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

bool BrainInitialization::initializeBrain(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize brain: null pointer");
        return false;
    }
    
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = brain->getConfig()->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = brain->getConfig()->getOr<size_t>("region_count", 1);
    float connectionProbability = brain->getConfig()->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
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
                    brain->pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    brain->pImpl->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*brain->pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    auto* workingMem = brain->getWorkingMemory();
    if (workingMem) {
        workingMem->initialize(brain);
        workingMem->setCapacity(neuronCount / 10);
    }
    
    // Initialize episodic memory
    auto* episodicMem = brain->getEpisodicMemory();
    if (episodicMem) {
        episodicMem->initialize(brain);
        episodicMem->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    auto* associativeMem = brain->getAssociativeMemory();
    if (associativeMem) {
        associativeMem->initialize(brain);
    }
    
    // Initialize cognition systems
    auto* planner = brain->getPlanner();
    if (planner) {
        planner->initialize(brain);
        planner->setPlanningDepth(5);
    }
    
    auto* conceptFormation = brain->getConceptFormation();
    if (conceptFormation) {
        conceptFormation->initialize(brain);
    }
    
    auto* attention = brain->getAttention();
    if (attention) {
        attention->initialize(brain);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
    
    // Initialize neuromodulation
    auto* novelty = brain->getNovelty();
    auto* curiosity = brain->getCuriosity();
    if (novelty) {
        novelty->initialize(brain);
    }
    if (curiosity) {
        curiosity->initialize(brain);
    }
    
    // Register spike handlers for event-driven processing
    auto* spikeSystem = brain->getSpikeSystem();
    if (spikeSystem) {
        spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
            // Count spikes
            ++brain->pImpl->totalSpikesThisStep;
            ++brain->pImpl->totalSpikesTotal;
        });
        
        // Register delayed spike handler to deliver synaptic input
        spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver synaptic input
            for (auto& region : brain->pImpl->regions) {
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
    
    // Configure checkpoint manager
    auto* checkpointManager = brain->pImpl->checkpointManager.get();
    if (checkpointManager) {
        std::string checkpointDir = brain->pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(brain->pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(brain->pImpl->motorNeurons.size()));
    
    return true;
}

void BrainInitialization::setupRegions(Brain* brain, size_t neuronCount, size_t regionCount, float connectionProbability) {
    if (!brain) return;
    
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion("Region_" + std::to_string(i + 1));
    }
    
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
        if (region) {
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    brain->pImpl->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    brain->pImpl->motorNeurons.push_back(neuron);
                }
            }
        }
    }
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(static_cast<RegionId>(i + 1));
        if (region) {
            region->initializeRandomConnectivity(*brain->pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void BrainInitialization::setupIntegratedSystems(Brain* brain, size_t neuronCount) {
    if (!brain) return;
    
    // Initialize working memory
    brain->pImpl->workingMemory->initialize(brain);
    brain->pImpl->workingMemory->setCapacity(neuronCount / 10);
    
    // Initialize episodic memory
    brain->pImpl->episodicMemory->initialize(brain);
    brain->pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    brain->pImpl->associativeMemory->initialize(brain);
    
    // Initialize cognition systems
    brain->pImpl->planner->initialize(brain);
    brain->pImpl->planner->setPlanningDepth(5);
    
    brain->pImpl->conceptFormation->initialize(brain);
    
    brain->pImpl->attention->initialize(brain);
    brain->pImpl->attention->setInhibitionStrength(0.5f);
    brain->pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    brain->pImpl->novelty->initialize(brain);
    brain->pImpl->curiosity->initialize(brain);
    
    // Register spike handlers
    brain->pImpl->spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
        ++brain->pImpl->totalSpikesThisStep;
        ++brain->pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler
    brain->pImpl->spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
        for (auto& region : brain->pImpl->regions) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == event.destination_neuron) {
                    MembranePotential synapticCurrent = event.weight * 10.0f;
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

} // namespace nlm