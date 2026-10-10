#pragma once

/**
 * Brain Factory Pattern for System Initialization
 * 
 * This factory class encapsulates the creation and initialization of all
 * brain systems in Phase 6. It provides a clean interface for creating
 * a fully integrated brain with all components properly configured and
 * interconnected.
 * 
 * The factory follows these principles:
 * - Dependency inversion: Uses config to configure systems
 * - Single responsibility: Each system has its own factory method
 * - Separation of concerns: Initialization is separated from construction
 * - Extensibility: Easy to add new systems or modify existing ones
 * 
 * The factory creates and initializes all systems in the correct order:
 * 1. Core systems (STDP, Hebbian, StructuralPlasticity)
 * 2. Memory systems (WorkingMemory, EpisodicMemory, AssociativeMemory)
 * 3. Prediction system
 * 4. Cognition systems (NeuralPlanner, ConceptFormation, Attention)
 * 5. Development system
 * 6. Neuromodulation systems (Dopamine, Curiosity, PredictionError, Novelty)
 * 7. Checkpoint manager
 * 
 * This eliminates the need for complex initialization code in the BrainImpl
 * constructor and makes the system more maintainable and testable.
 */

#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/AttentionalSelection.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "neuromodulation/Novelty.hpp"
#include "performance/CheckpointSystem.hpp"
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

namespace nlm {

/**
 * Factory configuration options for brain system creation
 */
struct BrainFactoryConfig {
    // Core configuration from the main config
    std::shared_ptr<Config> config;
    
    // Override options for specific systems
    struct SystemOptions {
        bool enableSTDP = true;
        bool enableHebbian = true;
        bool enableStructuralPlasticity = true;
        bool enableWorkingMemory = true;
        bool enableEpisodicMemory = true;
        bool enableAssociativeMemory = true;
        bool enablePrediction = true;
        bool enableCognition = true;
        bool enableDevelopment = true;
        bool enableNeuromodulation = true;
        bool enableCheckpointing = true;
        
        // System-specific configuration
        size_t workingMemoryCapacity = 100;
        size_t maxEpisodicEpisodes = 1000;
        size_t replayInterval = 100;
        size_t consolidationInterval = 1000;
        float attentionInhibitionStrength = 0.5f;
        float attentionExcitationStrength = 1.5f;
        float planningDepth = 5;
        float noveltySensitivity = 1.0f;
        float curiosityThreshold = 0.1f;
        float dopaminePlasticityFactor = 1.0f;
    };
    
    SystemOptions options;
    
    BrainFactoryConfig(std::shared_ptr<Config> cfg) : config(cfg) {}
};

/**
 * Factory for creating and initializing integrated brain systems
 */
class BrainFactory {
public:
    BrainFactory() = default;
    ~BrainFactory() = default;
    
    /**
     * Create a fully integrated brain with all systems
     * 
     * @param config The configuration object containing system parameters
     * @param options Factory configuration options
     * @return Shared pointer to initialized brain
     * @throws std::runtime_error if initialization fails
     */
    static std::shared_ptr<Brain> createIntegratedBrain(
        const BrainFactoryConfig& factoryConfig
    ) {
        try {
            std::cout << "=== NLM Brain Factory ===" << std::endl;
            std::cout << "Creating integrated artificial brain (Phase 6)..." << std::endl;
            
            // Create brain with config
            auto brain = std::make_shared<Brain>(factoryConfig.config);
            
            // Initialize the brain using factory methods
            initializeCoreSystems(brain, factoryConfig);
            initializeMemorySystems(brain, factoryConfig);
            initializePredictionSystem(brain, factoryConfig);
            initializeCognitionSystems(brain, factoryConfig);
            initializeDevelopmentSystem(brain, factoryConfig);
            initializeNeuromodulationSystems(brain, factoryConfig);
            initializeCheckpointManager(brain, factoryConfig);
            
            // Initialize brain regions and neurons
            initializeBrainArchitecture(brain, factoryConfig);
            
            std::cout << "=== Brain Factory Complete ===" << std::endl;
            std::cout << "Integrated brain created with all systems:" << std::endl;
            
            // Log system status
            logFactoryStatus(brain);
            
            return brain;
            
        } catch (const std::exception& e) {
            std::cerr << "[BrainFactory Error] Failed to create integrated brain: " << e.what() << std::endl;
            throw;
        }
    }
    
    /**
     * Create a brain factory configuration from the main config
     */
    static BrainFactoryConfig createConfigFromMainConfig(std::shared_ptr<Config> config) {
        BrainFactoryConfig factoryConfig(config);
        
        // Configure system options from config if available
        if (config->has("working_memory_capacity")) {
            factoryConfig.options.workingMemoryCapacity = 
                config->getOr<size_t>("working_memory_capacity", 100);
        }
        
        if (config->has("max_episodes")) {
            factoryConfig.options.maxEpisodicEpisodes = 
                config->getOr<size_t>("max_episodes", 1000);
        }
        
        if (config->has("replay_interval")) {
            factoryConfig.options.replayInterval = 
                config->getOr<size_t>("replay_interval", 100);
        }
        
        if (config->has("consolidation_interval")) {
            factoryConfig.options.consolidationInterval = 
                config->getOr<size_t>("consolidation_interval", 1000);
        }
        
        return factoryConfig;
    }
    
private:
    /**
     * Initialize core neural computation systems
     */
    static void initializeCoreSystems(
        const std::shared_ptr<Brain>& brain,
        const BrainFactoryConfig& factoryConfig
    ) {
        std::cout << "Initializing core neural systems..." << std::endl;
        
        auto& impl = *brain->pImpl;
        
        // Create core systems
        impl.spikeSystem = std::make_unique<SpikeSystem>();
        impl.stdp = std::make_unique<STDP>();
        impl.hebbian = std::make_unique<Hebbian>();
        impl.structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Configure STDP if enabled
        if (factoryConfig.options.enableSTDP) {
            float ltpWeight = factoryConfig.config->getOr<float>("stdp_ltp_weight", config::STDP_LTP_WEIGHT);
            float ltdWeight = factoryConfig.config->getOr<float>("stdp_ltd_weight", config::STDP_LTD_WEIGHT);
            float tau = factoryConfig.config->getOr<float>("stdp_tau", config::STDP_TIME_CONSTANT);
            
            impl.stdp->configure(ltpWeight, ltdWeight, tau);
            std::cout << "  STDP configured: LTP=" << ltpWeight << ", LTD=" << ltdWeight << ", tau=" << tau << "ms" << std::endl;
        }
        
        // Create spike system handler for statistics
        impl.spikeSystem->registerHandler([impl](const DetailedSpikeEvent& event) {
            ++impl->totalSpikesThisStep;
            ++impl->totalSpikesTotal;
        });
        
        std::cout << "  Core systems initialized: STDP, Hebbian, StructuralPlasticity, SpikeSystem" << std::endl;
    }
    
    /**
     * Initialize memory systems
     */
    static void initializeMemorySystems(
        const std::shared_ptr<Brain>& brain,
        const BrainFactoryConfig& factoryConfig
    ) {
        std::cout << "Initializing memory systems..." << std::endl;
        
        auto& impl = *brain->pImpl;
        
        if (factoryConfig.options.enableWorkingMemory) {
            impl.workingMemory = std::make_unique<NeuralWorkingMemory>();
            impl.workingMemory->initialize(brain.get());
            impl.workingMemory->setCapacity(factoryConfig.options.workingMemoryCapacity);
            std::cout << "  Working memory initialized (capacity=" << factoryConfig.options.workingMemoryCapacity << ")" << std::endl;
        }
        
        if (factoryConfig.options.enableEpisodicMemory) {
            impl.episodicMemory = std::make_unique<NeuralEpisodicMemory>();
            impl.episodicMemory->initialize(brain.get());
            impl.episodicMemory->setMaxEpisodes(factoryConfig.options.maxEpisodicEpisodes);
            std::cout << "  Episodic memory initialized (max episodes=" << factoryConfig.options.maxEpisodicEpisodes << ")" << std::endl;
        }
        
        if (factoryConfig.options.enableAssociativeMemory) {
            impl.associativeMemory = std::make_unique<NeuralAssociativeMemory>();
            impl.associativeMemory->initialize(brain.get());
            std::cout << "  Associative memory initialized" << std::endl;
        }
        
        // Register episodic memory storage
        impl.stepsSinceLastEpisode = 0;
        std::cout << "  Memory systems initialized" << std::endl;
    }
    
    /**
     * Initialize prediction system
     */
    static void initializePredictionSystem(
        const std::shared_ptr<Brain>& brain,
        const BrainFactoryConfig& factoryConfig
    ) {
        std::cout << "Initializing prediction system..." << std::endl;
        
        auto& impl = *brain->pImpl;
        
        if (factoryConfig.options.enablePrediction) {
            impl.predictionSystem = std::make_unique<PredictionSystem>();
            std::cout << "  Prediction system initialized" << std::endl;
        }
    }
    
    /**
     * Initialize cognition systems
     */
    static void initializeCognitionSystems(
        const std::shared_ptr<Brain>& brain,
        const BrainFactoryConfig& factoryConfig
    ) {
        std::cout << "Initializing cognition systems..." << std::endl;
        
        auto& impl = *brain->pImpl;
        
        if (factoryConfig.options.enableCognition) {
            // Initialize neural planner
            impl.planner = std::make_unique<NeuralPlanner>();
            impl.planner->initialize(brain.get());
            impl.planner->setPlanningDepth(factoryConfig.options.planningDepth);
            std::cout << "  Neural planner initialized (depth=" << factoryConfig.options.planningDepth << ")" << std::endl;
            
            // Initialize concept formation
            impl.conceptFormation = std::make_unique<ConceptFormation>();
            impl.conceptFormation->initialize(brain.get());
            std::cout << "  Concept formation initialized" << std::endl;
            
            // Initialize attention system
            impl.attention = std::make_unique<AttentionalSelection>();
            impl.attention->initialize(brain.get());
            impl.attention->setInhibitionStrength(factoryConfig.options.attentionInhibitionStrength);
            impl.attention->setExcitationStrength(factoryConfig.options.attentionExcitationStrength);
            std::cout << "  Attention system initialized (inhibition=" << factoryConfig.options.attentionInhibitionStrength 
                      << ", excitation=" << factoryConfig.options.attentionExcitationStrength << ")" << std::endl;
        }
    }
    
    /**
     * Initialize development system
     */
    static void initializeDevelopmentSystem(
        const std::shared_ptr<Brain>& brain,
        const BrainFactoryConfig& factoryConfig
    ) {
        std::cout << "Initializing development system..." << std::endl;
        
        auto& impl = *brain->pImpl;
        
        if (factoryConfig.options.enableDevelopment) {
            impl.developmentSystem = std::make_unique<DevelopmentSystem>();
            std::cout << "  Development system initialized" << std::endl;
        }
    }
    
    /**
     * Initialize neuromodulation systems
     */
    static void initializeNeuromodulationSystems(
        const std::shared_ptr<Brain>& brain,
        const BrainFactoryConfig& factoryConfig
    ) {
        std::cout << "Initializing neuromodulation systems..." << std::endl;
        
        auto& impl = *brain->pImpl;
        
        if (factoryConfig.options.enableNeuromodulation) {
            // Initialize novelty detection
            impl.novelty = std::make_unique<Novelty>();
            impl.novelty->initialize(brain.get());
            std::cout << "  Novelty detector initialized" << std::endl;
            
            // Initialize curiosity system
            impl.curiosity = std::make_unique<Curiosity>();
            impl.curiosity->initialize(brain.get());
            std::cout << "  Curiosity system initialized" << std::endl;
            
            // Initialize dopamine (reward prediction error)
            impl.dopamine = std::make_unique<Dopamine>();
            impl.dopamine->initialize(brain.get());
            std::cout << "  Dopamine system initialized" << std::endl;
            
            // Initialize prediction error
            impl.predictionError = std::make_unique<PredictionError>();
            impl.predictionError->initialize(brain.get());
            std::cout << "  Prediction error system initialized" << std::endl;
        }
    }
    
    /**
     * Initialize checkpoint manager
     */
    static void initializeCheckpointManager(
        const std::shared_ptr<Brain>& brain,
        const BrainFactoryConfig& factoryConfig
    ) {
        std::cout << "Initializing checkpoint manager..." << std::endl;
        
        auto& impl = *brain->pImpl;
        
        if (factoryConfig.options.enableCheckpointing) {
            impl.checkpointManager = std::make_unique<CheckpointManager>();
            
            // Configure checkpoint from main config
            std::string checkpointDir = factoryConfig.config->getOr<std::string>("checkpoint_dir", "./checkpoints");
            impl.checkpointManager->configure(checkpointDir, 10000, 5, true);
            
            std::cout << "  Checkpoint manager initialized (directory=" << checkpointDir << ")" << std::endl;
        }
    }
    
    /**
     * Initialize brain architecture (regions and neurons)
     */
    static void initializeBrainArchitecture(
        const std::shared_ptr<Brain>& brain,
        const BrainFactoryConfig& factoryConfig
    ) {
        std::cout << "Initializing brain architecture..." << std::endl;
        
        // Get configuration values
        size_t neuronCount = factoryConfig.config->getOr<size_t>("neuron_count", config::DEFAULT_NEURON_COUNT);
        size_t regionCount = factoryConfig.config->getOr<size_t>("region_count", config::DEFAULT_REGION_COUNT);
        float connectionProbability = factoryConfig.config->getOr<float>("connection_probability", config::DEFAULT_CONNECTION_PROBABILITY);
        double timestep = factoryConfig.config->getOr<double>("simulation_timestep", config::SIMULATION_TIMESTEP);
        
        // Initialize brain with architecture
        auto& impl = *brain->pImpl;
        impl.timestep = timestep;
        
        // Create regions
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
                
                // Initialize random connectivity and synapse weights
                region->initializeRandomConnectivity(*impl.rng, connectionProbability, 0.2f, 0.1f);
            }
        }
        
        std::cout << "  Brain architecture initialized: " << neuronCount << " neurons, " 
                  << regionCount << " regions, " << connectionProbability * 100 << "% connectivity" << std::endl;
    }
    
    /**
     * Log the factory creation status
     */
    static void logFactoryStatus(const std::shared_ptr<Brain>& brain) {
        auto& impl = *brain->pImpl;
        
        std::cout << "Factory Status Report:" << std::endl;
        std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
        
        if (impl.workingMemory) {
            std::cout << "  Working memory: " << impl.workingMemory->getActiveTraces() << " active traces" << std::endl;
        }
        
        if (impl.episodicMemory) {
            std::cout << "  Episodic memory: " << impl.episodicMemory->getEpisodeCount() << " episodes stored" << std::endl;
        }
        
        if (impl.predictionSystem) {
            std::cout << "  Prediction system: Active" << std::endl;
        }
        
        if (impl.attention) {
            std::cout << "  Attention system: Active" << std::endl;
        }
        
        if (impl.developmentSystem) {
            std::cout << "  Development system: Active (stage=" << static_cast<int>(impl.developmentalStage) << ")" << std::endl;
        }
        
        if (impl.dopamine) {
            std::cout << "  Neuromodulation: Dopamine level=" << impl.dopamine->getLevel() << std::endl;
        }
        
        std::cout << "  Integration complete: All systems connected and initialized" << std::endl;
    }
};

} // namespace nlm
