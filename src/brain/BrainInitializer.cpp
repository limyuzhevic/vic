// Brain Initializer Implementation
// Phase 6: Brain initialization and setup

#include "BrainInitializer.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <chrono>

namespace nlm {

BrainInitializer::BrainInitializer() {
    stats_.initializationTime = 0.0;
}

BrainInitializer::~BrainInitializer() {
    // Cleanup handled by Brain destructor
}

bool BrainInitializer::initialize(Brain* brain) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize null brain");
        return false;
    }
    
    NLM_LOG_INFO("Starting NLM Brain initialization (Phase 6: Integrated Artificial Brain)...");
    
    // Validate configuration
    if (!validateConfiguration(brain->getConfig().get())) {
        NLM_LOG_ERROR("Invalid configuration for brain initialization");
        return false;
    }
    
    // Clear previous state if any
    brain->reset();
    
    // Get configuration values
    size_t neuronCount = static_cast<size_t>(brain->getConfig()->getOr<int64_t>("neuron_count", 1000));
    size_t regionCount = static_cast<size_t>(brain->getConfig()->getOr<int64_t>("region_count", 1));
    float connectionProbability = brain->getConfig()->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions, " +
                 std::to_string(connectionProbability) + " connection probability");
    
    try {
        // Create regions
        setupRegions(brain, regionCount);
        
        // Create neurons across regions
        setupNeuralPopulations(brain, neuronCount, regionCount);
        
        // Build neuron lookup for O(1) synaptic delivery
        setupNeuronLookup(brain);
        
        // Initialize connectivity
        setupConnectivity(brain, connectionProbability);
        
        // Initialize all integrated systems
        setupAllIntegratedSystems(brain);
        
        // Register spike handlers for event-driven processing
        registerSpikeHandlers(brain);
        
        // Validate brain state after initialization
        if (!validateBrainState(brain)) {
            NLM_LOG_ERROR("Brain state validation failed");
            cleanup(brain);
            return false;
        }
        
        // Log initialization statistics
        logInitializationStats(brain);
        
        stats_.success = true;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stats_.initializationTime = std::chrono::duration<double>(endTime - startTime).count();
        
        NLM_LOG_INFO("NLM Brain initialization completed successfully in " + 
                     std::to_string(stats_.initializationTime) + " seconds");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain initialization failed with exception: " + std::string(e.what()));
        cleanup(brain);
        stats_.success = false;
        return false;
    }
}

void BrainInitializer::setupRegions(Brain* brain, size_t regionCount) {
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion("Region_" + std::to_string(i + 1));
    }
    stats_.regionsCreated = regionCount;
}

void BrainInitializer::setupNeuralPopulations(Brain* brain, size_t neuronCount, size_t regionCount) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    size_t nextNeuronIndex = 1;  // globally unique IDs across regions
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            region->setNextNeuronIndex(nextNeuronIndex);
            
            // Add populations with balanced excitation/inhibition
            setupBalancedPopulations(region, neuronsPerRegion);
            
            // Collect sensory and motor neurons for I/O
            collectIOMeurons(region, brain->pImpl->sensoryNeurons, brain->pImpl->motorNeurons);
            
            stats_.neuronsCreated += region->getTotalNeuronCount();
            stats_.populationsCreated += region->getPopulationCount();
            
            nextNeuronIndex += region->getTotalNeuronCount();
        }
    }
}

void BrainInitializer::setupBalancedPopulations(NeuralRegion* region, size_t neuronsPerRegion) {
    // Split the internal population 80/20 into excitatory and inhibitory neurons.
    auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
    size_t internalCount = neuronsPerRegion / 2;
    size_t internalExcitatory = internalCount * 4 / 5;
    auto internalPopId = region->addPopulation(internalExcitatory, NeuronType::Internal);
    auto inhibitoryPopId = region->addPopulation(internalCount - internalExcitatory, NeuronType::Inhibitory);
    (void)internalPopId; (void)inhibitoryPopId;
    auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
}

void BrainInitializer::collectIOMeurons(NeuralRegion* region, 
                                       std::vector<Neuron*>& sensoryNeurons,
                                       std::vector<Neuron*>& motorNeurons) {
    // Collect sensory neurons for input injection
    auto* sensoryPop = region->getPopulation(1);  // First population is sensory
    if (sensoryPop) {
        for (auto* neuron : sensoryPop->getNeurons()) {
            sensoryNeurons.push_back(neuron);
        }
    }
    
    // Collect motor neurons for output
    auto* motorPop = region->getPopulation(region->getPopulationCount());  // Last population is motor
    if (motorPop) {
        for (auto* neuron : motorPop->getNeurons()) {
            motorNeurons.push_back(neuron);
        }
    }
}

void BrainInitializer::setupNeuronLookup(Brain* brain) {
    // Build the neuron lookup used by synaptic delivery and injection (O(1) access)
    for (auto& region : brain->pImpl->regions) {
        for (auto* neuron : region->getAllNeurons()) {
            brain->pImpl->neuronById[neuron->getId().index()] = neuron;
        }
    }
}

void BrainInitializer::setupConnectivity(Brain* brain, float connectionProbability) {
    // Initialize connectivity with random weights
    for (size_t i = 0; i < brain->getRegionCount(); ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*brain->pImpl->rng, connectionProbability, 0.2f, 0.1f);
            stats_.synapsesCreated += region->getTotalSynapseCount();
        }
    }
}

void BrainInitializer::setupAllIntegratedSystems(Brain* brain) {
    // Initialize working memory
    brain->pImpl->workingMemory->initialize(brain);
    brain->pImpl->workingMemory->setCapacity(static_cast<size_t>(brain->getConfig()->getOr<int64_t>("neuron_count", 1000)) / 10);
    
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
    brain->pImpl->dopamine->initialize(brain);
}

void BrainInitializer::registerSpikeHandlers(Brain* brain) {
    // Register spike handlers for event-driven processing
    if (!brain->pImpl->spikeHandlersRegistered) {
        brain->pImpl->spikeHandlersRegistered = true;
        
        // Count spikes
        brain->pImpl->spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
            ++brain->pImpl->totalSpikesThisStep;
            ++brain->pImpl->totalSpikesTotal;
        });
        
        // Deliver synaptic input for delayed spikes
        brain->pImpl->spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
            auto it = brain->pImpl->neuronById.find(event.destination_neuron.index());
            if (it != brain->pImpl->neuronById.end()) {
                Neuron* neuron = it->second;
                // Deliver synaptic input with delay
                neuron->injectCurrent(event.weight);
            }
        });
    }
}

bool BrainInitializer::validateBrainState(Brain* brain) const {
    if (!brain) {
        return false;
    }
    
    // Check essential components
    if (!brain->pImpl->spikeSystem) {
        NLM_LOG_ERROR("Missing spike system");
        return false;
    }
    
    if (brain->pImpl->regions.empty()) {
        NLM_LOG_ERROR("No regions created");
        return false;
    }
    
    if (brain->getTotalNeuronCount() == 0) {
        NLM_LOG_ERROR("No neurons created");
        return false;
    }
    
    // Validate each region
    for (const auto& region : brain->pImpl->regions) {
        if (region->getTotalNeuronCount() == 0) {
            NLM_LOG_ERROR("Region has no neurons");
            return false;
        }
    }
    
    return true;
}

void BrainInitializer::logInitializationStats(Brain* brain) const {
    NLM_LOG_INFO("=== Brain Initialization Statistics ===");
    NLM_LOG_INFO("  Regions created: " + std::to_string(stats_.regionsCreated));
    NLM_LOG_INFO("  Neurons created: " + std::to_string(stats_.neuronsCreated));
    NLM_LOG_INFO("  Populations created: " + std::to_string(stats_.populationsCreated));
    NLM_LOG_INFO("  Synapses created: " + std::to_string(stats_.synapsesCreated));
    NLM_LOG_INFO("  Initialization time: " + std::to_string(stats_.initializationTime) + " seconds");
    NLM_LOG_INFO("  Average neurons per region: " + 
                 std::to_string(static_cast<double>(stats_.neuronsCreated) / stats_.regionsCreated));
    NLM_LOG_INFO("  Average synapses per neuron: " + 
                 std::to_string(static_cast<double>(stats_.synapsesCreated) / stats_.neuronsCreated));
}

void BrainInitializer::cleanup(Brain* brain) {
    if (!brain) return;
    
    // Clear all dynamic allocations
    brain->pImpl->regions.clear();
    brain->pImpl->sensoryNeurons.clear();
    brain->pImpl->motorNeurons.clear();
    brain->pImpl->neuronById.clear();
    brain->pImpl->spikeHandlersRegistered = false;
    
    // Reset statistics
    stats_ = InitializationStats();
}

bool BrainInitializer::validateConfiguration(const Config* config) const {
    if (!config) return false;
    
    // Validate essential configuration parameters
    auto neuronCount = config->getOr<int64_t>("neuron_count", 1000);
    auto regionCount = config->getOr<int64_t>("region_count", 1);
    auto connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    if (neuronCount <= 0) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount));
        return false;
    }
    
    if (regionCount <= 0) {
        NLM_LOG_ERROR("Invalid region_count: " + std::to_string(regionCount));
        return false;
    }
    
    if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connectionProbability));
        return false;
    }
    
    return true;
}

} // namespace nlm