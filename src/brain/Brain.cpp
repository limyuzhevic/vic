#include "Brain.hpp"
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
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <limits>

namespace nlm {

struct Brain::Impl {
    std::shared_ptr<Config> config;
    std::unique_ptr<RandomGenerator> rng;
    std::vector<std::unique_ptr<NeuralRegion>> regions;
    std::vector<InterRegionConnection> interRegionConnections;
    
    // ========== INTEGRATED MEMORY SYSTEMS ==========
    std::unique_ptr<NeuralWorkingMemory> workingMemory;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
    
    // ========== INTEGRATED PREDICTION SYSTEM ==========
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    // ========== INTEGRATED COGNITION SYSTEMS ==========
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    // ========== DEVELOPMENT SYSTEM ==========
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
    
    // Phase 2: Real neural computation components
    std::unique_ptr<SpikeSystem> spikeSystem;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Simulation parameters
    TimestepDuration timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Statistics
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    
    // Sensory neurons for input injection
    std::vector<Neuron*> sensoryNeurons;
    std::vector<Neuron*> motorNeurons;
    
    // Integration state
    bool isResting;  // For sleep/rest cycle
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Checkpoint system
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    Impl(std::shared_ptr<Config> cfg)
        : config(cfg)
        , rng(nullptr)
        , developmentalStage(DevelopmentalStage::Initial)
        , nextRegionId(1)
        , timestep(0.001)
        , currentStep(0)
        , currentTime(0.0)
        , totalSpikesThisStep(0)
        , totalSpikesTotal(0)
        , isResting(false)
        , stepsSinceLastEpisode(0)
        , replayInterval(100)      // Replay every 100 steps
        , consolidationInterval(1000)  // Consolidate every 1000 steps
    {
        // Validate configuration
        if (!cfg) {
            throw std::invalid_argument("Config pointer cannot be null");
        }
        
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // ========== INITIALIZE INTEGRATED SYSTEMS ==========
        
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
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};

Brain::Brain(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

Brain::~Brain() = default;

Brain::Brain(Brain&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Brain& Brain::operator=(Brain&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool Brain::initialize() {
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    try {
        // Validate configuration
        if (!pImpl->config) {
            NLM_LOG_ERROR("Brain initialization failed: Config is null");
            return false;
        }
        
        // Get configuration values
        size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
        float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
        
        // Validate configuration values
        if (neuronCount == 0) {
            NLM_LOG_ERROR("Brain initialization failed: neuron_count must be greater than 0");
            return false;
        }
        
        if (regionCount == 0) {
            NLM_LOG_ERROR("Brain initialization failed: region_count must be greater than 0");
            return false;
        }
        
        if (connectionProbability < 0.0f || connectionProbability > 1.0f) {
            NLM_LOG_ERROR("Brain initialization failed: connection_probability must be between 0.0 and 1.0");
            return false;
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Create regions
        for (size_t i = 0; i < regionCount; ++i) {
            addRegion("Region_" + std::to_string(i + 1));
        }
        
        // Create neurons across regions
        if (regionCount > 0) {
            size_t neuronsPerRegion = neuronCount / regionCount;
            for (size_t i = 0; i < regionCount; ++i) {
                auto* region = getRegion(RegionId(i + 1));
                if (!region) {
                    NLM_LOG_ERROR("Brain initialization failed: Could not create region " + std::to_string(i + 1));
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
                        if (neuron) {
                            pImpl->sensoryNeurons.push_back(neuron);
                        }
                    }
                }
                if (motorPop) {
                    for (auto* neuron : motorPop->getNeurons()) {
                        if (neuron) {
                            pImpl->motorNeurons.push_back(neuron);
                        }
                    }
                }
                
                NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                            ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                            std::to_string(region->getTotalNeuronCount()) + " neurons");
            }
        }
        
        // Initialize connectivity with random weights
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = getRegion(RegionId(i + 1));
            if (region) {
                // Initialize random connectivity and synapse weights
                region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
            }
        }
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        
        // Initialize working memory
        if (!pImpl->workingMemory) {
            NLM_LOG_ERROR("Brain initialization failed: workingMemory is null");
            return false;
        }
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount > 0 ? neuronCount / 10 : 100);
        
        // Initialize episodic memory
        if (!pImpl->episodicMemory) {
            NLM_LOG_ERROR("Brain initialization failed: episodicMemory is null");
            return false;
        }
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
        
        // Initialize associative memory
        if (!pImpl->associativeMemory) {
            NLM_LOG_ERROR("Brain initialization failed: associativeMemory is null");
            return false;
        }
        pImpl->associativeMemory->initialize(this);
        
        // Initialize prediction system
        // (PredictionSystem doesn't have initialize method currently)
        
        // Initialize cognition systems
        if (!pImpl->planner) {
            NLM_LOG_ERROR("Brain initialization failed: planner is null");
            return false;
        }
        pImpl->planner->initialize(this);
        pImpl->planner->setPlanningDepth(5);
        
        if (!pImpl->conceptFormation) {
            NLM_LOG_ERROR("Brain initialization failed: conceptFormation is null");
            return false;
        }
        pImpl->conceptFormation->initialize(this);
        
        if (!pImpl->attention) {
            NLM_LOG_ERROR("Brain initialization failed: attention is null");
            return false;
        }
        pImpl->attention->initialize(this);
        pImpl->attention->setInhibitionStrength(0.5f);
        pImpl->attention->setExcitationStrength(1.5f);
        
        // Initialize neuromodulation
        if (!pImpl->novelty) {
            NLM_LOG_ERROR("Brain initialization failed: novelty is null");
            return false;
        }
        pImpl->novelty->initialize(this);
        
        if (!pImpl->curiosity) {
            NLM_LOG_ERROR("Brain initialization failed: curiosity is null");
            return false;
        }
        pImpl->curiosity->initialize(this);
        
        // Initialize other neuromodulators
        if (!pImpl->dopamine) {
            NLM_LOG_ERROR("Brain initialization failed: dopamine is null");
            return false;
        }
        
        if (!pImpl->predictionError) {
            NLM_LOG_ERROR("Brain initialization failed: predictionError is null");
            return false;
        }
        
        // Register spike handlers for event-driven processing
        if (pImpl->spikeSystem) {
            pImpl->spikeSystem->registerHandler([this](const DetailedSpikeEvent& event) {
                // Count spikes
                ++pImpl->totalSpikesThisStep;
                ++pImpl->totalSpikesTotal;
            });
            
            // Register delayed spike handler to deliver synaptic input
            pImpl->spikeSystem->registerDelayedHandler([this](const DelayedSpikeEvent& event) {
                // Find destination neuron and deliver synaptic input
                for (auto& region : pImpl->regions) {
                    if (!region) continue;
                    auto neurons = region->getAllNeurons();
                    for (auto* neuron : neurons) {
                        if (neuron && neuron->getId() == event.destination_neuron) {
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
        std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
        if (pImpl->checkpointManager) {
            pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
        }
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Brain initialization failed with exception: " + std::string(e.what()));
        return false;
    }
}