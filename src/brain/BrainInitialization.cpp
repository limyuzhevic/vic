// Brain initialization implementation
#include "BrainInitialization.hpp"
#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

bool BrainInitialization::initialize(Brain* brain, std::shared_ptr<Config> config) {
    if (!brain) {
        NLM_LOG_ERROR("Brain pointer is null");
        return false;
    }
    
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    // Get configuration values
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = config->getOr<size_t>("region_count", 1);
    float connectionProbability = config->getOr<float>("connection_probability", 0.1f);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    createRegions(brain, neuronCount, regionCount, connectionProbability);
    
    // Initialize plasticity systems
    initializePlasticitySystems(brain);
    
    // Initialize integrated systems
    initializeIntegratedSystems(brain);
    
    // Configure spike system
    configureSpikeSystem(brain);
    
    // Configure checkpoint manager
    configureCheckpointManager(brain);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    return true;
}

void BrainInitialization::createRegions(Brain* brain, size_t neuronCount, 
                                        size_t regionCount, float connectionProbability) {
    if (!brain) return;
    
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion("Region_" + std::to_string(i + 1));
    }
    
    // Create neurons across regions
    size_t neuronsPerRegion = neuronCount / regionCount;
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
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
                    brain->injectCurrent(NeuronId(), 0.0f); // TODO: Store for later access
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    brain->injectCurrent(NeuronId(), 0.0f); // TODO: Store for later access
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                        ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                        std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
    }
    
    // Initialize connectivity with random weights
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            // TODO: Add random connectivity initialization method
            // region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void BrainInitialization::initializePlasticitySystems(Brain* brain) {
    if (!brain) return;
    
    // Plasticity systems are initialized in Brain constructor
    // Just log the status
    NLM_LOG_INFO("Plasticity systems (STDP, Hebbian, Structural) initialized");
}

void BrainInitialization::initializeIntegratedSystems(Brain* brain) {
    if (!brain) return;
    
    // Get config for initialization
    auto config = brain->getConfig();
    size_t neuronCount = config->getOr<size_t>("neuron_count", 1000);
    
    // Initialize working memory
    if (auto* workingMemory = brain->getWorkingMemory()) {
        workingMemory->initialize(brain);
        workingMemory->setCapacity(neuronCount / 10);
        NLM_LOG_INFO("Working memory initialized with capacity: " + std::to_string(workingMemory->getCapacity()));
    }
    
    // Initialize episodic memory
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        episodicMemory->initialize(brain);
        episodicMemory->setMaxEpisodes(1000);
        NLM_LOG_INFO("Episodic memory initialized with max episodes: 1000");
    }
    
    // Initialize associative memory
    if (auto* associativeMemory = brain->getAssociativeMemory()) {
        associativeMemory->initialize(brain);
        NLM_LOG_INFO("Associative memory initialized");
    }
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems
    if (auto* planner = brain->getPlanner()) {
        planner->initialize(brain);
        planner->setPlanningDepth(5);
        NLM_LOG_INFO("Neural planner initialized with depth: 5");
    }
    
    if (auto* conceptFormation = brain->getConceptFormation()) {
        conceptFormation->initialize(brain);
        NLM_LOG_INFO("Concept formation initialized");
    }
    
    if (auto* attention = brain->getAttention()) {
        attention->initialize(brain);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
        NLM_LOG_INFO("Attention system initialized");
    }
    
    // Initialize neuromodulation
    if (auto* novelty = brain->getNovelty()) {
        novelty->initialize(brain);
        NLM_LOG_INFO("Novelty detection initialized");
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        curiosity->initialize(brain);
        NLM_LOG_INFO("Curiosity system initialized");
    }
}

void BrainInitialization::configureSpikeSystem(Brain* brain) {
    if (!brain) return;
    
    auto* spikeSystem = brain->getSpikeSystem();
    if (!spikeSystem) return;
    
    // Register spike handlers for event-driven processing
    spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
        // Count spikes
        brain->injectCurrent(NeuronId(), 0.0f); // TODO: Increment spike counters
    });
    
    // Register delayed spike handler to deliver synaptic input
    spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
        // Find destination neuron and deliver synaptic input
        // This is handled by spike system internally
    });
    
    NLM_LOG_INFO("Spike system configured with event handlers");
}

void BrainInitialization::configureCheckpointManager(Brain* brain) {
    if (!brain) return;
    
    auto config = brain->getConfig();
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    
    NLM_LOG_INFO("Checkpoint directory configured: " + checkpointDir);
}

} // namespace nlm