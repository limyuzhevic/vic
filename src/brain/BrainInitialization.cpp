#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Brain initialization implementation

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    try {
        // Validate configuration
        if (!pImpl->config) {
            NLM_LOG_ERROR("Configuration is null");
            return false;
        }
        
        // Get configuration values with validation
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Validate configuration values
        if (neuronCount == 0) {
            NLM_LOG_ERROR("Neuron count must be positive");
            return false;
        }
        if (regionCount == 0) {
            NLM_LOG_ERROR("Region count must be positive");
            return false;
        }
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            NLM_LOG_ERROR("Connection probability must be between 0.0 and 1.0");
            return false;
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Create regions
        for (size_t i = 0; i < regionCount; ++i) {
            addRegion("Region_" + std::to_string(i + 1));
        }
        
        // Create neurons across regions
        size_t neuronsPerRegion = neuronCount / regionCount;
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1));
                return false;
            }
            
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
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                         ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Failed to get region " + std::to_string(i + 1) + " for connectivity");
                return false;
            }
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        
        // Initialize working memory
        if (!pImpl->workingMemory) {
            NLM_LOG_ERROR("Working memory not initialized");
            return false;
        }
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
        
        // Initialize episodic memory
        if (!pImpl->episodicMemory) {
            NLM_LOG_ERROR("Episodic memory not initialized");
            return false;
        }
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
        
        // Initialize associative memory
        if (!pImpl->associativeMemory) {
            NLM_LOG_ERROR("Associative memory not initialized");
            return false;
        }
        pImpl->associativeMemory->initialize(this);
        
        // Initialize prediction system
        // (PredictionSystem doesn't have initialize method currently)
        
        // Initialize cognition systems
        if (!pImpl->planner) {
            NLM_LOG_ERROR("Planner not initialized");
            return false;
        }
        pImpl->planner->initialize(this);
        pImpl->planner->setPlanningDepth(5);
        
        if (!pImpl->conceptFormation) {
            NLM_LOG_ERROR("Concept formation not initialized");
            return false;
        }
        pImpl->conceptFormation->initialize(this);
        
        if (!pImpl->attention) {
            NLM_LOG_ERROR("Attention system not initialized");
            return false;
        }
        pImpl->attention->initialize(this);
        pImpl->attention->setInhibitionStrength(0.5f);
        pImpl->attention->setExcitationStrength(1.5f);
        
        // Initialize neuromodulation
        if (!pImpl->novelty) {
            NLM_LOG_ERROR("Novelty system not initialized");
            return false;
        }
        pImpl->novelty->initialize(this);
        
        if (!pImpl->curiosity) {
            NLM_LOG_ERROR("Curiosity system not initialized");
            return false;
        }
        pImpl->curiosity->initialize(this);
        
        // Register spike handlers for event-driven processing
        if (!pImpl->spikeSystem) {
            NLM_LOG_ERROR("Spike system not initialized");
            return false;
        }
        pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
            // Count spikes
            ++pImpl->totalSpikesThisStep;
            ++pImpl->totalSpikesTotal;
        });
        
        // Register delayed spike handler to deliver synaptic input
        if (!pImpl->spikeSystem) {
            NLM_LOG_ERROR("Spike system not initialized for delayed handler");
            return false;
        }
        pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
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
        
        // Configure checkpoint manager
        if (!pImpl->checkpointManager) {
            NLM_LOG_ERROR("Checkpoint manager not initialized");
            return false;
        }
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during brain initialization: ") + e.what());
        return false;
    }
}

} // namespace nlm