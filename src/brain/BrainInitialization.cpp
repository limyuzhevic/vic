#include "BrainInitialization.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"

namespace nlm {

// Brain::Impl initialization - BrainInitializer class implementation

BrainInitializer::BrainInitializer(Brain::Impl* impl) : pImpl(impl) {}

void BrainInitializer::initializePlasticitySystems() {
    // Initialize plasticity systems
    pImpl->spikeSystem = std::make_unique<SpikeSystem>();
    pImpl->stdp = std::make_unique<STDP>();
    pImpl->hebbian = std::make_unique<Hebbian>();
    pImpl->structuralPlasticity = std::make_unique<StructuralPlasticity>();
}

void BrainInitializer::configurePlasticity() {
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

void BrainInitializer::initializeMemorySystems() {
    // Initialize memory systems
    pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
    pImpl->episodicMemory = std::make_unique<NeuralEpisodicMemory>();
    pImpl->associativeMemory = std::make_unique<NeuralAssociativeMemory>();
}

void BrainInitializer::initializePredictionSystem() {
    // Initialize prediction system
    pImpl->predictionSystem = std::make_unique<PredictionSystem>();
}

void BrainInitializer::initializeCognitionSystems() {
    // Initialize cognition systems
    pImpl->planner = std::make_unique<NeuralPlanner>();
    pImpl->conceptFormation = std::make_unique<ConceptFormation>();
    pImpl->attention = std::make_unique<AttentionalSelection>();
}

void BrainInitializer::initializeDevelopmentSystem() {
    // Initialize development system
    pImpl->developmentSystem = std::make_unique<DevelopmentSystem>();
}

void BrainInitializer::initializeNeuromodulationSystems() {
    // Initialize neuromodulation systems
    pImpl->dopamine = std::make_unique<Dopamine>();
    pImpl->curiosity = std::make_unique<Curiosity>();
    pImpl->predictionError = std::make_unique<PredictionError>();
    pImpl->novelty = std::make_unique<Novelty>();
}

void BrainInitializer::configureSimulationParameters() {
    // Get timestep
    pImpl->timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
    
    // Get integration intervals from config
    pImpl->replayInterval = pImpl->config->getOr<size_t>("replay_interval", 100);
    pImpl->consolidationInterval = pImpl->config->getOr<size_t>("consolidation_interval", 1000);
}

void BrainInitializer::configureCheckpointManager() {
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void BrainInitializer::registerSpikeHandlers() {
    // Register spike handlers for event-driven processing
    pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
        // Count spikes
        ++pImpl->totalSpikesThisStep;
        ++pImpl->totalSpikesTotal;
    });
    
    // Register delayed spike handler to deliver synaptic input
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
}

void BrainInitializer::initializeAllIntegratedSystems() {
    // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
    
    // Initialize working memory
    pImpl->workingMemory->initialize(nullptr);  // Will be initialized later in Brain::initialize()
    pImpl->workingMemory->setCapacity(1000);  // Default capacity, will be updated in Brain::initialize()
    
    // Initialize episodic memory
    pImpl->episodicMemory->initialize(nullptr);  // Will be initialized later in Brain::initialize()
    pImpl->episodicMemory->setMaxEpisodes(1000);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(nullptr);  // Will be initialized later in Brain::initialize()
    
    // Initialize cognition systems
    pImpl->planner->initialize(nullptr);  // Will be initialized later in Brain::initialize()
    pImpl->planner->setPlanningDepth(5);
    
    pImpl->conceptFormation->initialize(nullptr);  // Will be initialized later in Brain::initialize()
    
    pImpl->attention->initialize(nullptr);  // Will be initialized later in Brain::initialize()
    pImpl->attention->setInhibitionStrength(0.5f);
    pImpl->attention->setExcitationStrength(1.5f);
    
    // Initialize neuromodulation
    pImpl->novelty->initialize(nullptr);  // Will be initialized later in Brain::initialize()
    pImpl->curiosity->initialize(nullptr);  // Will be initialized later in Brain::initialize()
}

void BrainInitializer::configureCheckpointManager() {
    // Configure checkpoint manager
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
}

void BrainInitializer::initializeRandomGenerator() {
    // Initialize random generator with seed from config
    uint64_t seed = 42;  // Default seed
    if (auto seedOpt = pImpl->config->get<uint64_t>("random_seed")) {
        seed = *seedOpt;
    }
    pImpl->rng = std::make_unique<RandomGenerator>(seed);
}

void BrainInitializer::initialize() {
    // Initialize random generator with seed from config
    initializeRandomGenerator();
    
    // Initialize plasticity systems
    initializePlasticitySystems();
    
    // Configure plasticity
    configurePlasticity();
    
    // Configure simulation parameters
    configureSimulationParameters();
    
    // Configure checkpoint manager
    configureCheckpointManager();
}
