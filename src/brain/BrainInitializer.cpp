#include "BrainInitializer.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "../performance/CheckpointManager.hpp"

namespace nlm {

BrainInitializer::BrainInitializer(Brain* brain) : brain_(brain) {}

bool BrainInitializer::initialize() {
    if (!brain_) return false;
    
    auto* config = brain_->getConfig().get();
    if (!config) return false;
    
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6 - Integrated)...");
    
    // Get configuration values
    size_t neuronCount = getNeuronCount(config);
    size_t regionCount = getRegionCount(config);
    float connectionProbability = getConnectionProbability(config);
    
    NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                 std::to_string(regionCount) + " regions");
    
    // Create regions
    createRegions(brain_);
    
    // Create neurons across regions
    createPopulations(brain_);
    
    // Initialize connectivity with random weights
    auto* rng = brain_->getRandomGenerator();
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain_->getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
        }
    }
    
    // Initialize memory systems
    initializeMemorySystems(brain_);
    
    // Initialize prediction system
    initializePredictionSystem(brain_);
    
    // Initialize cognition systems
    initializeCognitionSystems(brain_);
    
    // Initialize neuromodulation systems
    initializeNeuromodulationSystems(brain_);
    
    // Configure STDP parameters
    configureSTDP(brain_);
    
    // Configure structural plasticity
    configureStructuralPlasticity(brain_);
    
    // Configure spike system handlers
    configureSpikeSystem(brain_);
    
    // Configure checkpoint manager
    configureCheckpointManager(brain_);
    
    logInitializationStatus(brain_);
    
    return true;
}

// Get configuration values
BrainInitializer::getNeuronCount = [](const Config* config) {
    return config->getOr<size_t>("neuron_count", 1000);
};

BrainInitializer::getRegionCount = [](const Config* config) {
    return config->getOr<size_t>("region_count", 1);
};

BrainInitializer::getConnectionProbability = [](const Config* config) {
    return config->getOr<float>("connection_probability", 0.1f);
};

// Create regions with proper organization
void BrainInitializer::createRegions(Brain* brain) {
    auto* config = brain->getConfig().get();
    size_t regionCount = getRegionCount(config);
    
    for (size_t i = 0; i < regionCount; ++i) {
        brain->addRegion("Region_" + std::to_string(i + 1));
    }
}

// Create populations within regions
void BrainInitializer::createPopulations(Brain* brain) {
    auto* config = brain->getConfig().get();
    size_t neuronCount = getNeuronCount(config);
    size_t regionCount = getRegionCount(config);
    
    size_t neuronsPerRegion = neuronCount / regionCount;
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            // Add populations
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
        }
    }
}

// Initialize connectivity
void BrainInitializer::initializeConnectivity(Brain* brain, RandomGenerator& rng, float connectionProbability) {
    size_t regionCount = brain->getRegionCount();
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = brain->getRegion(RegionId(i + 1));
        if (region) {
            region->initializeRandomConnectivity(rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

// Initialize memory systems
void BrainInitializer::initializeMemorySystems(Brain* brain) {
    size_t neuronCount = brain->getTotalNeuronCount();
    
    // Initialize working memory
    if (auto* workingMemory = brain->getWorkingMemory()) {
        workingMemory->initialize(brain);
        workingMemory->setCapacity(neuronCount / 10);
    }
    
    // Initialize episodic memory
    if (auto* episodicMemory = brain->getEpisodicMemory()) {
        episodicMemory->initialize(brain);
        episodicMemory->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    if (auto* associativeMemory = brain->getAssociativeMemory()) {
        associativeMemory->initialize(brain);
    }
}

// Initialize prediction system
void BrainInitializer::initializePredictionSystem(Brain* brain) {
    // PredictionSystem doesn't have initialize method currently
    // but we could add one in the future
}

// Initialize cognition systems
void BrainInitializer::initializeCognitionSystems(Brain* brain) {
    // Initialize cognition systems
    if (auto* planner = brain->getPlanner()) {
        planner->initialize(brain);
        planner->setPlanningDepth(5);
    }
    
    if (auto* conceptFormation = brain->getConceptFormation()) {
        conceptFormation->initialize(brain);
    }
    
    if (auto* attention = brain->getAttention()) {
        attention->initialize(brain);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
}

// Initialize neuromodulation systems
void BrainInitializer::initializeNeuromodulationSystems(Brain* brain) {
    // Initialize neuromodulation systems
    if (auto* novelty = brain->getNovelty()) {
        novelty->initialize(brain);
    }
    
    if (auto* curiosity = brain->getCuriosity()) {
        curiosity->initialize(brain);
    }
    
    // Dopamine doesn't have initialize method
}

// Configure STDP parameters
void BrainInitializer::configureSTDP(Brain* brain) {
    auto* config = brain->getConfig().get();
    auto* stdp = brain->getSTDP();
    
    if (!stdp || !config) return;
    
    float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
    float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
    float tau = config->getOr<float>("stdp_tau", 20.0f);
    
    stdp->configure(ltpWeight, ltdWeight, tau);
}

// Configure structural plasticity
void BrainInitializer::configureStructuralPlasticity(Brain* brain) {
    auto* config = brain->getConfig().get();
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    
    if (!structuralPlasticity || !config) return;
    
    float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
    float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
    
    structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
    structuralPlasticity->setPruningRate(pruningRate);
}

// Configure spike system handlers
void BrainInitializer::configureSpikeSystem(Brain* brain) {
    // Register spike handlers for event-driven processing
    if (auto* spikeSystem = brain->getSpikeSystem()) {
        // Count spikes
        spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
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
}

// Configure checkpoint manager
void BrainInitializer::configureCheckpointManager(Brain* brain) {
    auto* config = brain->getConfig().get();
    auto* checkpointManager = brain->getCheckpointManager();
    
    if (!checkpointManager || !config) return;
    
    std::string checkpointDir = config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    checkpointManager->configure(checkpointDir, 10000, 5, true);
}

// Log initialization status
void BrainInitializer::logInitializationStatus(Brain* brain) const {
    if (!brain) return;
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
}

} // namespace nlm
