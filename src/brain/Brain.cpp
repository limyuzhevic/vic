#pragma once

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

namespace nlm {

// Implementation of NLM Brain: Central coordinator of the neural system
// Provides real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems
// Part 1: Brain initialization and system management

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

namespace nlm {

// BrainInitializer: Manages brain initialization process
// Provides static methods for initializing brain components and systems
// Separates initialization logic from the main Brain class for better organization

class BrainInitializer {
public:
    // Initialize the complete brain with configuration
    // Creates regions, neurons, populations, and initializes all integrated systems
    static bool initializeBrain(Brain* brain) {
        NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
        
        // Get configuration values with validation
        size_t neuronCount = brain->getConfig()->getOr<size_t>("neuron_count", 1000);
        size_t regionCount = brain->getConfig()->getOr<size_t>("region_count", 1);
        float connectionProbability = brain->getConfig()->getOr<float>("connection_probability", 0.1f);
        
        // Validate configuration parameters
        if (neuronCount == 0) {
            NLM_LOG_ERROR("Invalid neuron_count: must be > 0");
            return false;
        }
        if (regionCount == 0) {
            NLM_LOG_ERROR("Invalid region_count: must be > 0");
            return false;
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions");
        
        // Create regions with naming
        for (size_t i = 0; i < regionCount; ++i) {
            brain->addRegion("Region_" + std::to_string(i + 1));
        }
        
        // Create neurons and populations across regions
        size_t neuronsPerRegion = neuronCount / regionCount;
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = brain->getRegion(RegionId(i + 1));
            if (!region) {
                NLM_LOG_ERROR("Failed to create region " + std::to_string(i + 1));
                continue;
            }
            
            // Add neural populations with balanced distribution
            auto sensoryPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(neuronsPerRegion / 2, NeuronType::Internal);
            auto motorPopId = region->addPopulation(neuronsPerRegion / 4, NeuronType::Motor);
            
            // Collect sensory and motor neurons for external I/O operations
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    brain->getBrainImpl()->sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    brain->getBrainImpl()->motorNeurons.push_back(neuron);
                }
            }
            
            NLM_LOG_INFO("Created populations in region " + std::to_string(i + 1) + 
                         ": " + std::to_string(region->getPopulationCount()) + " populations, " +
                         std::to_string(region->getTotalNeuronCount()) + " neurons");
        }
        
        // Initialize synaptic connectivity between neurons
        for (size_t i = 0; i < regionCount; ++i) {
            auto* region = brain->getRegion(RegionId(i + 1));
            if (!region) continue;
            
            // Initialize random connectivity with configured probability
            region->initializeRandomConnectivity(*brain->getBrainImpl()->rng, connectionProbability, 0.2f, 0.1f);
        }
        
        // ========== INITIALIZE ALL INTEGRATED SYSTEMS ==========
        NLM_LOG_INFO("Initializing integrated memory, cognition, and neuromodulation systems...");
        
        // Initialize working memory system for transient information storage
        brain->getBrainImpl()->workingMemory->initialize(brain);
        brain->getBrainImpl()->workingMemory->setCapacity(neuronCount / 10);
        
        // Initialize episodic memory system for experience recording
        brain->getBrainImpl()->episodicMemory->initialize(brain);
        brain->getBrainImpl()->episodicMemory->setMaxEpisodes(1000);
        
        // Initialize associative memory system for pattern associations
        brain->getBrainImpl()->associativeMemory->initialize(brain);
        
        // Initialize cognition systems
        brain->getBrainImpl()->planner->initialize(brain);
        brain->getBrainImpl()->planner->setPlanningDepth(5);
        
        brain->getBrainImpl()->conceptFormation->initialize(brain);
        
        brain->getBrainImpl()->attention->initialize(brain);
        brain->getBrainImpl()->attention->setInhibitionStrength(0.5f);
        brain->getBrainImpl()->attention->setExcitationStrength(1.5f);
        
        // Initialize neuromodulation systems
        brain->getBrainImpl()->novelty->initialize(brain);
        brain->getBrainImpl()->curiosity->initialize(brain);
        
        // Register spike handlers for event-driven spike processing
        brain->getBrainImpl()->spikeSystem->registerHandler([brain](const DetailedSpikeEvent& event) {
            // Count spikes for statistics
            ++brain->getBrainImpl()->totalSpikesThisStep;
            ++brain->getBrainImpl()->totalSpikesTotal;
        });
        
        // Register delayed spike handler for synaptic input delivery
        brain->getBrainImpl()->spikeSystem->registerDelayedHandler([brain](const DelayedSpikeEvent& event) {
            // Find destination neuron and deliver synaptic input based on spike weight
            for (auto& region : brain->getBrainImpl()->regions) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == event.destination_neuron) {
                        // Apply synaptic weight as current (scaled factor of 10.0)
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
        
        // Configure checkpoint manager for state persistence
        std::string checkpointDir = brain->getConfig()->getOr<std::string>("checkpoint_dir", "./checkpoints");
        brain->getBrainImpl()->checkpointManager->configure(checkpointDir, 10000, 5, true);
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(brain->getBrainImpl()->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(brain->getBrainImpl()->motorNeurons.size()));
        
        return true;
    }
};

class BrainSystemsManager {
public:
    // Configure plasticity systems based on configuration parameters
    static void configurePlasticitySystems(Brain* brain) {
        // Configure STDP (Spike-Timing Dependent Plasticity) parameters
        float ltpWeight = brain->getConfig()->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = brain->getConfig()->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = brain->getConfig()->getOr<float>("stdp_tau", 20.0f);
        brain->getSTDP()->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity parameters
        float synaptogenesisRate = brain->getConfig()->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = brain->getConfig()->getOr<float>("pruning_rate", 0.00001f);
        brain->getStructuralPlasticity()->setSynaptogenesisRate(synaptogenesisRate);
        brain->getStructuralPlasticity()->setPruningRate(pruningRate);
        
        // Get simulation timestep from configuration
        brain->getBrainImpl()->timestep = brain->getConfig()->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from configuration for memory consolidation
        brain->getBrainImpl()->replayInterval = brain->getConfig()->getOr<size_t>("replay_interval", 100);
        brain->getBrainImpl()->consolidationInterval = brain->getConfig()->getOr<size_t>("consolidation_interval", 1000);
    }
};

class BrainStatistics {
public:
    // Generate comprehensive brain status report
    static void logStatus(Brain* brain) {
        NLM_LOG_INFO("=== Brain Status ===");
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Pending spike events: " + std::to_string(brain->getPendingSpikeEventCount()));
        NLM_LOG_INFO("Excitation/inhibition ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    }
};

class BrainIO {
public:
    // Generate input pattern for testing and experimentation
    static void injectInputPattern(Brain* brain, SimulationStep step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        // Log progress every 100 steps for monitoring
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                         " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                         " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
};

class BrainSystemsManager {
public:
    static void configurePlasticitySystems(Brain* brain) {
        // Configure STDP parameters
        float ltpWeight = brain->getConfig()->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = brain->getConfig()->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = brain->getConfig()->getOr<float>("stdp_tau", 20.0f);
        brain->getSTDP()->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = brain->getConfig()->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = brain->getConfig()->getOr<float>("pruning_rate", 0.00001f);
        brain->getStructuralPlasticity()->setSynaptogenesisRate(synaptogenesisRate);
        brain->getStructuralPlasticity()->setPruningRate(pruningRate);
        
        // Get timestep
        brain->getBrainImpl()->timestep = brain->getConfig()->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        brain->getBrainImpl()->replayInterval = brain->getConfig()->getOr<size_t>("replay_interval", 100);
        brain->getBrainImpl()->consolidationInterval = brain->getConfig()->getOr<size_t>("consolidation_interval", 1000);
    }
};

class BrainStatistics {
public:
    static void logStatus(Brain* brain) {
        NLM_LOG_INFO("=== Brain Status ===");
        NLM_LOG_INFO("Total neurons: " + std::to_string(brain->getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(brain->getTotalSynapseCount()));
        NLM_LOG_INFO("Active neurons: " + std::to_string(brain->getActiveNeuronCount()));
        NLM_LOG_INFO("Firing neurons: " + std::to_string(brain->getFiringNeuronCount()));
        NLM_LOG_INFO("Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("Pending spike events: " + std::to_string(brain->getPendingSpikeEventCount()));
        NLM_LOG_INFO("Excitation/inhibition ratio: " + std::to_string(brain->getExcitationInhibitionRatio()));
        NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()));
    }
};

class BrainIO {
public:
    static void injectInputPattern(Brain* brain, SimulationStep step) {
        // Create input pattern - inject current into sensory neurons
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                         " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                         " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
};

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
    
    return BrainInitializer::initializeBrain(this);
}

// Add the rest of the Brain methods implementation...
// This file should continue with implementations of:
// - step() methods
// - receiveSensoryInput()
// - injectCurrent()
// - injectCurrentToNeurons()
// - getSpikeSystem(), getSTDP(), getHebbian(), getStructuralPlasticity()
// - getExcitationInhibitionRatio(), getTotalSpikeCount(), getPendingSpikeEventCount()
// - produceAction()
// - applyNeuromodulation()
// - updatePlasticity()
// - develop()
// - reset()
// - save(), load()
// - addRegion(), getRegion(), getRegionCount(), getRegionIds()
// - addInterRegionConnection(), removeInterRegionConnection()
// - getTotalNeuronCount(), getTotalSynapseCount(), getActiveNeuronCount(), getFiringNeuronCount()
// - getExcitationInhibitionRatio(), getAverageFiringRate()
// - getWorkingMemory(), getEpisodicMemory(), getAssociativeMemory()
// - getPredictionSystem()
// - getPlanner(), getConceptFormation(), getAttention()
// - getDevelopmentSystem(), getDevelopmentalStage(), setDevelopmentalStage()
// - getDopamine(), getCuriosity(), getNovelty(), getPredictionErrorSignal()
// - getConfig()
// - getRandomGenerator()
// - logStatus()

} // namespace nlm