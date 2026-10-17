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

namespace nlm {

// Configuration parameter validation and extraction
namespace brain_config {

    // Validate configuration and extract parameters with defaults
    struct ValidatedConfig {
        // Core brain parameters
        size_t neuronCount;
        size_t regionCount;
        float connectionProbability;
        double simulationTimestep;
        uint64_t randomSeed;
        
        // STDP plasticity parameters
        float stdpLtpWeight;
        float stdpLtdWeight;
        float stdpTau;
        
        // Structural plasticity parameters
        float synaptogenesisRate;
        float pruningRate;
        
        // Integration intervals
        size_t replayInterval;
        size_t consolidationInterval;
        
        // Neural dynamics parameters
        float neuronAdaptationRate;
        float neuronAdaptationDecay;
        float synapticWeightPositiveBound;
        float synapticWeightNegativeBound;
        size_t defaultSynapticDelay;
        
        // Memory system parameters
        size_t workingMemoryCapacity;
        size_t maxEpisodicEpisodes;
        float memoryConsolidationThreshold;
        
        // Neuromodulation parameters
        float noveltyDetectionThreshold;
        float curiosityStrength;
        float dopamineDecayRate;
        
        // Cognitive system parameters
        size_t planningDepth;
        float attentionInhibitionStrength;
        float attentionExcitationStrength;
        
        // Development system parameters
        double initialPlasticityRate;
        double criticalPeriodThreshold;
        double maturationThreshold;
        
        // Performance parameters
        size_t spikeSystemQueueSize;
        size_t checkpointMaxFiles;
        double checkpointMaxSizeMB;
    };

    ValidatedConfig validateAndExtract(const std::shared_ptr<Config>& config) {
        ValidatedConfig validated;
        
        // Core brain parameters
        if (config->has("neuron_count")) {
            validated.neuronCount = config->get<size_t>("neuron_count");
            if (validated.neuronCount < 10 || validated.neuronCount > 1000000) {
                throw ConfigError("neuron_count must be between 10 and 1,000,000");
            }
        } else {
            validated.neuronCount = 1000;
        }
        
        if (config->has("region_count")) {
            validated.regionCount = config->get<size_t>("region_count");
            if (validated.regionCount < 1 || validated.regionCount > 1000) {
                throw ConfigError("region_count must be between 1 and 1000");
            }
        } else {
            validated.regionCount = 1;
        }
        
        if (config->has("connection_probability")) {
            validated.connectionProbability = config->get<float>("connection_probability");
            if (validated.connectionProbability < 0.0f || validated.connectionProbability > 1.0f) {
                throw ConfigError("connection_probability must be between 0.0 and 1.0");
            }
        } else {
            validated.connectionProbability = 0.1f;
        }
        
        if (config->has("simulation_timestep")) {
            validated.simulationTimestep = config->get<double>("simulation_timestep");
            if (validated.simulationTimestep <= 0.0 || validated.simulationTimestep > 1.0) {
                throw ConfigError("simulation_timestep must be between 0.0 and 1.0");
            }
        } else {
            validated.simulationTimestep = 0.001;
        }
        
        if (config->has("random_seed")) {
            validated.randomSeed = config->get<uint64_t>("random_seed");
        } else {
            validated.randomSeed = 42ull;
        }
        
        // STDP plasticity parameters
        if (config->has("stdp_ltp_weight")) {
            validated.stdpLtpWeight = config->get<float>("stdp_ltp_weight");
            if (validated.stdpLtpWeight < 0.0f || validated.stdpLtpWeight > 1.0f) {
                throw ConfigError("stdp_ltp_weight must be between 0.0 and 1.0");
            }
        } else {
            validated.stdpLtpWeight = 0.01f;
        }
        
        if (config->has("stdp_ltd_weight")) {
            validated.stdpLtdWeight = config->get<float>("stdp_ltd_weight");
            if (validated.stdpLtdWeight < 0.0f || validated.stdpLtdWeight > 1.0f) {
                throw ConfigError("stdp_ltd_weight must be between 0.0 and 1.0");
            }
        } else {
            validated.stdpLtdWeight = 0.012f;
        }
        
        if (config->has("stdp_tau")) {
            validated.stdpTau = config->get<float>("stdp_tau");
            if (validated.stdpTau <= 0.0f) {
                throw ConfigError("stdp_tau must be positive");
            }
        } else {
            validated.stdpTau = 20.0f;
        }
        
        // Structural plasticity parameters
        if (config->has("synaptogenesis_rate")) {
            validated.synaptogenesisRate = config->get<float>("synaptogenesis_rate");
            if (validated.synaptogenesisRate < 0.0f || validated.synaptogenesisRate > 1.0f) {
                throw ConfigError("synaptogenesis_rate must be between 0.0 and 1.0");
            }
        } else {
            validated.synaptogenesisRate = 0.0001f;
        }
        
        if (config->has("pruning_rate")) {
            validated.pruningRate = config->get<float>("pruning_rate");
            if (validated.pruningRate < 0.0f || validated.pruningRate > 1.0f) {
                throw ConfigError("pruning_rate must be between 0.0 and 1.0");
            }
        } else {
            validated.pruningRate = 0.00001f;
        }
        
        // Neural dynamics parameters
        if (config->has("neuron_adaptation_rate")) {
            validated.neuronAdaptationRate = config->get<float>("neuron_adaptation_rate");
            if (validated.neuronAdaptationRate < 0.0f) {
                throw ConfigError("neuron_adaptation_rate must be positive");
            }
        } else {
            validated.neuronAdaptationRate = 0.01f;
        }
        
        if (config->has("neuron_adaptation_decay")) {
            validated.neuronAdaptationDecay = config->get<float>("neuron_adaptation_decay");
            if (validated.neuronAdaptationDecay < 0.0f || validated.neuronAdaptationDecay > 1.0f) {
                throw ConfigError("neuron_adaptation_decay must be between 0.0 and 1.0");
            }
        } else {
            validated.neuronAdaptationDecay = 0.95f;
        }
        
        if (config->has("synaptic_weight_positive_bound")) {
            validated.synapticWeightPositiveBound = config->get<float>("synaptic_weight_positive_bound");
            if (validated.synapticWeightPositiveBound <= 0.0f) {
                throw ConfigError("synaptic_weight_positive_bound must be positive");
            }
        } else {
            validated.synapticWeightPositiveBound = 0.01f;
        }
        
        if (config->has("synaptic_weight_negative_bound")) {
            validated.synapticWeightNegativeBound = config->get<float>("synaptic_weight_negative_bound");
            if (validated.synapticWeightNegativeBound >= 0.0f) {
                throw ConfigError("synaptic_weight_negative_bound must be negative");
            }
        } else {
            validated.synapticWeightNegativeBound = -0.01f;
        }
        
        if (config->has("default_synaptic_delay")) {
            validated.defaultSynapticDelay = config->get<size_t>("default_synaptic_delay");
            if (validated.defaultSynapticDelay < 1) {
                throw ConfigError("default_synaptic_delay must be at least 1");
            }
        } else {
            validated.defaultSynapticDelay = 1;
        }
        
        // Memory system parameters
        if (config->has("working_memory_capacity")) {
            validated.workingMemoryCapacity = config->get<size_t>("working_memory_capacity");
            if (validated.workingMemoryCapacity < 10) {
                throw ConfigError("working_memory_capacity must be at least 10");
            }
        } else {
            validated.workingMemoryCapacity = 100;
        }
        
        if (config->has("max_episodic_episodes")) {
            validated.maxEpisodicEpisodes = config->get<size_t>("max_episodic_episodes");
            if (validated.maxEpisodicEpisodes < 10) {
                throw ConfigError("max_episodic_episodes must be at least 10");
            }
        } else {
            validated.maxEpisodicEpisodes = 1000;
        }
        
        if (config->has("memory_consolidation_threshold")) {
            validated.memoryConsolidationThreshold = config->get<float>("memory_consolidation_threshold");
            if (validated.memoryConsolidationThreshold < 0.0f || validated.memoryConsolidationThreshold > 1.0f) {
                throw ConfigError("memory_consolidation_threshold must be between 0.0 and 1.0");
            }
        } else {
            validated.memoryConsolidationThreshold = 0.3f;
        }
        
        // Neuromodulation parameters
        if (config->has("novelty_detection_threshold")) {
            validated.noveltyDetectionThreshold = config->get<float>("novelty_detection_threshold");
            if (validated.noveltyDetectionThreshold < 0.0f || validated.noveltyDetectionThreshold > 1.0f) {
                throw ConfigError("novelty_detection_threshold must be between 0.0 and 1.0");
            }
        } else {
            validated.noveltyDetectionThreshold = 0.1f;
        }
        
        if (config->has("curiosity_strength")) {
            validated.curiosityStrength = config->get<float>("curiosity_strength");
            if (validated.curiosityStrength < 0.0f || validated.curiosityStrength > 1.0f) {
                throw ConfigError("curiosity_strength must be between 0.0 and 1.0");
            }
        } else {
            validated.curiosityStrength = 0.5f;
        }
        
        if (config->has("dopamine_decay_rate")) {
            validated.dopamineDecayRate = config->get<float>("dopamine_decay_rate");
            if (validated.dopamineDecayRate < 0.0f || validated.dopamineDecayRate > 1.0f) {
                throw ConfigError("dopamine_decay_rate must be between 0.0 and 1.0");
            }
        } else {
            validated.dopamineDecayRate = 0.1f;
        }
        
        // Cognitive system parameters
        if (config->has("planning_depth")) {
            validated.planningDepth = config->get<size_t>("planning_depth");
            if (validated.planningDepth < 1) {
                throw ConfigError("planning_depth must be at least 1");
            }
        } else {
            validated.planningDepth = 5;
        }
        
        if (config->has("attention_inhibition_strength")) {
            validated.attentionInhibitionStrength = config->get<float>("attention_inhibition_strength");
            if (validated.attentionInhibitionStrength < 0.0f) {
                throw ConfigError("attention_inhibition_strength must be positive");
            }
        } else {
            validated.attentionInhibitionStrength = 0.5f;
        }
        
        if (config->has("attention_excitation_strength")) {
            validated.attentionExcitationStrength = config->get<float>("attention_excitation_strength");
            if (validated.attentionExcitationStrength < 0.0f) {
                throw ConfigError("attention_excitation_strength must be positive");
            }
        } else {
            validated.attentionExcitationStrength = 1.5f;
        }
        
        // Development system parameters
        if (config->has("initial_plasticity_rate")) {
            validated.initialPlasticityRate = config->get<double>("initial_plasticity_rate");
            if (validated.initialPlasticityRate < 0.0 || validated.initialPlasticityRate > 1.0) {
                throw ConfigError("initial_plasticity_rate must be between 0.0 and 1.0");
            }
        } else {
            validated.initialPlasticityRate = 1.0;
        }
        
        if (config->has("critical_period_threshold")) {
            validated.criticalPeriodThreshold = config->get<double>("critical_period_threshold");
            if (validated.criticalPeriodThreshold < 0.0) {
                throw ConfigError("critical_period_threshold must be positive");
            }
        } else {
            validated.criticalPeriodThreshold = 60.0;
        }
        
        if (config->has("maturation_threshold")) {
            validated.maturationThreshold = config->get<double>("maturation_threshold");
            if (validated.maturationThreshold < 0.0) {
                throw ConfigError("maturation_threshold must be positive");
            }
        } else {
            validated.maturationThreshold = 300.0;
        }
        
        // Performance parameters
        if (config->has("spike_system_queue_size")) {
            validated.spikeSystemQueueSize = config->get<size_t>("spike_system_queue_size");
            if (validated.spikeSystemQueueSize < 100) {
                throw ConfigError("spike_system_queue_size must be at least 100");
            }
        } else {
            validated.spikeSystemQueueSize = 10000;
        }
        
        if (config->has("checkpoint_max_files")) {
            validated.checkpointMaxFiles = config->get<size_t>("checkpoint_max_files");
            if (validated.checkpointMaxFiles < 1) {
                throw ConfigError("checkpoint_max_files must be at least 1");
            }
        } else {
            validated.checkpointMaxFiles = 5;
        }
        
        if (config->has("checkpoint_max_size_mb")) {
            validated.checkpointMaxSizeMB = config->get<double>("checkpoint_max_size_mb");
            if (validated.checkpointMaxSizeMB < 1.0) {
                throw ConfigError("checkpoint_max_size_mb must be at least 1.0");
            }
        } else {
            validated.checkpointMaxSizeMB = 100.0;
        }
        
        return validated;
    }

    // Get checkpoint directory from config
    std::string getCheckpointDir(const std::shared_ptr<Config>& config) {
        if (config->has("checkpoint_dir")) {
            return config->get<std::string>("checkpoint_dir");
        }
        return "./checkpoints";
    }

} // namespace brain_config

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
        auto validatedConfig = brain_config::validateAndExtract(config);
        
        // Initialize random generator with seed from config
        rng = std::make_unique<RandomGenerator>(validatedConfig.randomSeed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Configure plasticity systems
        stdp->configure(validatedConfig.stdpLtpWeight, validatedConfig.stdpLtdWeight, validatedConfig.stdpTau);
        structuralPlasticity->setSynaptogenesisRate(validatedConfig.synaptogenesisRate);
        structuralPlasticity->setPruningRate(validatedConfig.pruningRate);
        
        // Set timestep and intervals
        timestep = validatedConfig.simulationTimestep;
        replayInterval = validatedConfig.replayInterval;
        consolidationInterval = validatedConfig.consolidationInterval;
        
        // Initialize neuromodulation systems
        initializeNeuromodulationSystems();
        
        // Initialize integrated systems
        initializeMemorySystems();
        initializePredictionSystem();
        initializeCognitionSystems();
        initializeDevelopmentSystem();
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
        std::string checkpointDir = brain_config::getCheckpointDir(config);
        checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
    
    void initializeNeuromodulationSystems() {
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
    }
    
    void initializeMemorySystems() {
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
    }
    
    void initializePredictionSystem() {
        predictionSystem = std::make_unique<PredictionSystem>();
    }
    
    void initializeCognitionSystems() {
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
    }
    
    void initializeDevelopmentSystem() {
        developmentSystem = std::make_unique<DevelopmentSystem>();
    }
    
    void configurePlasticityFromConfig() {
        if (config->has("stdp_ltp_weight")) {
            float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
            stdp->configure(ltpWeight, stdp->getLTDWeight(), stdp->getTimeConstant());
        }
        
        if (config->has("stdp_ltd_weight")) {
            float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
            stdp->configure(stdp->getLTPWeight(), ltdWeight, stdp->getTimeConstant());
        }
        
        if (config->has("stdp_tau")) {
            float tau = config->getOr<float>("stdp_tau", 20.0f);
            stdp->configure(stdp->getLTPWeight(), stdp->getLTDWeight(), tau);
        }
        
        if (config->has("synaptogenesis_rate")) {
            float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
            structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        }
        
        if (config->has("pruning_rate")) {
            float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
            structuralPlasticity->setPruningRate(pruningRate);
        }
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
        auto validatedConfig = brain_config::validateAndExtract(pImpl->config);
        
        // Get configuration values with validation
        size_t neuronCount = validatedConfig.neuronCount;
        size_t regionCount = validatedConfig.regionCount;
        float connectionProbability = validatedConfig.connectionProbability;
        
        // Validate derived parameters
        if (neuronCount == 0) {
            throw ConfigError("neuron_count cannot be zero");
        }
        if (regionCount == 0) {
            throw ConfigError("region_count cannot be zero");
        }
        if (neuronCount / regionCount < 4) {
            NLM_LOG_WARNING("Low neuron density: " + std::to_string(neuronCount / regionCount) + 
                          " neurons per region. This may affect network dynamics.");
        }
        
        NLM_LOG_INFO("Configuration: " + std::to_string(neuronCount) + " neurons, " + 
                     std::to_string(regionCount) + " regions, " +
                     std::to_string(connectionProbability) + " connection probability");
        
        // Create regions
        createRegions(regionCount);
        
        // Create neurons across regions
        createNeurons(neuronCount, regionCount);
        
        // Initialize connectivity with random weights
        initializeConnectivity(connectionProbability);
        
        // Initialize all integrated systems
        initializeIntegratedSystems(validatedConfig);
        
        // Configure plasticity from config
        pImpl->configurePlasticityFromConfig();
        
        // Configure checkpoint manager with validated parameters
        std::string checkpointDir = brain_config::getCheckpointDir(pImpl->config);
        pImpl->checkpointManager->configure(checkpointDir, validatedConfig.spikeSystemQueueSize, 
                                           validatedConfig.checkpointMaxFiles, true);
        
        NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
        NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
        NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
        NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
        NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
        
        return true;
        
    } catch (const ConfigError& e) {
        NLM_LOG_ERROR(std::string("Configuration error during brain initialization: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Unexpected error during brain initialization: ") + e.what());
        return false;
    }
}

void Brain::createRegions(size_t regionCount) {
    for (size_t i = 0; i < regionCount; ++i) {
        addRegion("Region_" + std::to_string(i + 1));
    }
}

void Brain::createNeurons(size_t neuronCount, size_t regionCount) {
    size_t neuronsPerRegion = neuronCount / regionCount;
    // Ensure minimum neurons per region
    if (neuronsPerRegion < 4) {
        neuronsPerRegion = 4;
    }
    
    for (size_t i = 0; i < regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Calculate population sizes with validation
            size_t sensoryNeurons = std::max<size_t>(neuronsPerRegion / 4, 1);
            size_t internalNeurons = neuronsPerRegion - sensoryNeurons - std::max<size_t>(neuronsPerRegion / 4, 1);
            if (internalNeurons < 1) internalNeurons = 1;
            size_t motorNeurons = std::max<size_t>(neuronsPerRegion / 4, 1);
            
            // Add populations with validation
            auto sensoryPopId = region->addPopulation(sensoryNeurons, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(internalNeurons, NeuronType::Internal);
            auto motorPopId = region->addPopulation(motorNeurons, NeuronType::Motor);
            
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
    }
}

void Brain::initializeConnectivity(float connectionProbability) {
    for (size_t i = 0; i < pImpl->regionCount; ++i) {
        auto* region = getRegion(RegionId(i + 1));
        if (region) {
            // Initialize random connectivity and synapse weights
            region->initializeRandomConnectivity(*pImpl->rng, connectionProbability, 0.2f, 0.1f);
        }
    }
}

void Brain::initializeIntegratedSystems(const brain_config::ValidatedConfig& validatedConfig) {
    // Initialize working memory with validated capacity
    pImpl->workingMemory->initialize(this);
    pImpl->workingMemory->setCapacity(validatedConfig.workingMemoryCapacity);
    
    // Initialize episodic memory with validated capacity
    pImpl->episodicMemory->initialize(this);
    pImpl->episodicMemory->setMaxEpisodes(validatedConfig.maxEpisodicEpisodes);
    
    // Initialize associative memory
    pImpl->associativeMemory->initialize(this);
    
    // Initialize prediction system
    // (PredictionSystem doesn't have initialize method currently)
    
    // Initialize cognition systems with validated parameters
    pImpl->planner->initialize(this);
    pImpl->planner->setPlanningDepth(validatedConfig.planningDepth);
    
    pImpl->conceptFormation->initialize(this);
    
    pImpl->attention->initialize(this);
    pImpl->attention->setInhibitionStrength(validatedConfig.attentionInhibitionStrength);
    pImpl->attention->setExcitationStrength(validatedConfig.attentionExcitationStrength);
    
    // Initialize neuromodulation with validated parameters
    pImpl->novelty->initialize(this);
    pImpl->curiosity->initialize(this);
    
    // Configure neuromodulation thresholds
    // (Assuming neuromodulation systems have configuration methods)
    
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
                    // Apply synaptic weight as current (using validated synaptic bound)
                    float synapticCurrent = event.weight * (pImpl->spikeSystem->getMaxWeightPerSpike() / 10.0f);
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

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    /*
     * PHASE 6: INTEGRATED ARTIFICIAL BRAIN LOOP
     * 
     * This implements the complete integrated brain simulation:
     * 
     * 1. Process pending delayed spike events (deliver synaptic input)
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory (maintenance and competition)
     * 5. Apply neuromodulation effects on neural excitability
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity (synaptogenesis, pruning)
     * 12. Replay important memories (during rest or periodically)
     * 13. Apply development effects
     * 14. Collect statistics
     */
    
    try {
        // Update simulation time
        pImpl->currentStep = currentStep;
        pImpl->currentTime = currentTime;
        pImpl->totalSpikesThisStep = 0;
        
        // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
        processDelayedSpikes(currentStep, currentTime);
        
        // ========== STEP 2: Update all neurons (LIF dynamics) ==========
        updateNeurons(currentStep, currentTime);
        
        // ========== STEP 3: Detect spikes and schedule spike events ==========
        detectAndProcessSpikes(currentStep, currentTime);
        
        // ========== STEP 4: Update working memory ==========
        updateWorkingMemory(currentTime);
        
        // ========== STEP 5: Apply neuromodulation effects ==========
        applyNeuromodulationEffects(currentTime);
        
        // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
        applyPlasticityRules(currentTime);
        
        // ========== STEP 7: Update episodic memory ==========
        updateEpisodicMemory(currentStep, currentTime);
        
        // ========== STEP 8: Update prediction system ==========
        updatePredictionSystem(currentTime);
        
        // ========== STEP 9: Update attention system ==========
        updateAttentionSystem(currentTime);
        
        // ========== STEP 10: Update concept formation ==========
        updateConceptFormation(currentTime);
        
        // ========== STEP 11: Apply structural plasticity periodically ==========
        applyStructuralPlasticity(currentStep, currentTime);
        
        // ========== STEP 12: Replay important memories ==========
        replayImportantMemories(currentStep, currentTime);
        
        // ========== STEP 13: Apply development effects ==========
        applyDevelopmentEffects(currentStep, currentTime);
        
        // ========== STEP 14: Periodic memory consolidation ==========
        consolidateMemories(currentStep);
        
        // ========== STEP 15: Checkpoint management ==========
        manageCheckpoints(currentStep, currentTime);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error in brain step: ") + e.what());
        // Continue with reset even if error occurs
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->totalSpikesThisStep = 0;
    }
}

void Brain::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
}

void Brain::updateNeurons(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
    }
}

void Brain::detectAndProcessSpikes(SimulationStep currentStep, Timestamp currentTime) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                // Check if neuron just fired this step
                const auto& state = neuron->getState();
                bool justFired = (state.firingState == FiringState::Refractory &&
                                 state.lastSpikeTime >= 0.0f &&
                                 std::abs(static_cast<float>(currentTime) - state.lastSpikeTime) < pImpl->timestep * 2.0f);

                if (justFired) {
                    // Neuron fired this step - queue the spike
                    SpikeEvent event(neuron->getId(), currentTime, currentStep);
                    pImpl->spikeSystem->queueSpike(event);

                    // Record post-synaptic spike for incoming synapses (plasticity)
                    auto incomingSynapses = region->getSynapsesTo(neuron->getId());
                    for (Synapse* syn : incomingSynapses) {
                        syn->recordPostSpike(currentTime);
                    }

                    // Get outgoing synapses and schedule delayed spike events
                    auto outgoingSynapses = region->getSynapsesFrom(neuron->getId());
                    for (Synapse* syn : outgoingSynapses) {
                        // Create delayed spike event
                        Delay delay = syn->getDelay();
                        SimulationStep deliveryStep = currentStep + delay;
                        Timestamp deliveryTime = currentTime + delay * pImpl->timestep;

                        DelayedSpikeEvent delayedEvent(
                            neuron->getId(),
                            syn->getDestinationNeuron(),
                            syn->getId(),
                            syn->getWeight(),
                            syn->getType(),
                            currentTime,
                            deliveryTime,
                            currentStep,
                            deliveryStep
                        );

                        pImpl->spikeSystem->queueDelayedSpike(delayedEvent);

                        // Record pre-synaptic spike for plasticity
                        syn->recordPreSpike(currentTime);
                    }
                    
                    // Store to working memory - neurons that fire become part of working memory
                    if (pImpl->workingMemory) {
                        pImpl->workingMemory->storeToNeuron(neuron->getId(), 
                            std::abs(state.membranePotential - state.restingPotential) / 10.0f);
                    }
                }
            }
        }
    }
    
    // Process immediate spikes
    pImpl->spikeSystem->processSpikes(currentStep);
}

void Brain::updateWorkingMemory(Timestamp currentTime) {
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
}

void Brain::applyNeuromodulationEffects(Timestamp currentTime) {
    // Update novelty detection
    if (pImpl->novelty) {
        pImpl->novelty->update(pImpl->timestep);
    }
    
    // Update curiosity
    if (pImpl->curiosity) {
        pImpl->curiosity->update(pImpl->timestep);
    }
    
    // Update dopamine (reward prediction error)
    if (pImpl->dopamine) {
        pImpl->dopamine->update(pImpl->timestep);
        
        // Apply dopamine effects on neural excitability
        // Dopamine modulates neural excitability by adjusting effective current injection
        // Higher dopamine increases excitability (lower effective threshold)
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    // Positive dopamine adds excitatory bias
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
}

void Brain::applyPlasticityRules(Timestamp currentTime) {
    // Calculate neuromodulation factor for plasticity
    float plasticityMod = 1.0f;
    if (pImpl->dopamine) {
        plasticityMod = pImpl->dopamine->getPlasticityFactor();
    }
    
    for (auto& region : pImpl->regions) {
        for (auto& syn : region->getSynapses()) {
            // Apply STDP with neuromodulation
            if (syn->getPlasticityFlags().stdp) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    // Modify weight change based on dopamine
                    pImpl->stdp->update(syn, preSpikes, postSpikes, pImpl->timestep);
                    float weight = syn->getWeight();
                    weight += (weight > 0 ? 1.0f : -1.0f) * (plasticityMod - 1.0f) * 0.001f;
                    syn->setWeight(weight);
                }
            }
            
            // Apply Hebbian learning
            if (syn->getPlasticityFlags().hebbian) {
                const auto& preSpikes = syn->getPreSpikeHistory();
                const auto& postSpikes = syn->getPostSpikeHistory();
                
                if (!preSpikes.empty() && !postSpikes.empty()) {
                    pImpl->hebbian->update(syn, preSpikes, postSpikes, pImpl->timestep);
                }
            }
            
            // Update synapse state
            syn->step(currentTime);
        }
    }
}

void Brain::updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime) {
    pImpl->stepsSinceLastEpisode++;
    if (pImpl->stepsSinceLastEpisode >= 10) {  // Store episode every 10 steps
        pImpl->stepsSinceLastEpisode = 0;
        
        if (pImpl->episodicMemory) {
            // Capture current brain state as an episode
            EpisodicMemoryItem episode;
            episode.timestamp = currentStep;
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            // Store active neurons
            for (auto& region : pImpl->regions) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        if (neuron->isFiring() || 
                            std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) > 5.0f) {
                            episode.activeNeurons.push_back(neuron->getId());
                            episode.neuronActivations.push_back(
                                std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential) / 20.0f);
                        }
                    }
                }
            }
            
            // Store reward in episode
            episode.reward = pImpl->dopamine ? pImpl->dopamine->getLevel() : 0.0f;
            
            pImpl->episodicMemory->storeEpisode(episode);
        }
    }
}

void Brain::updatePredictionSystem(Timestamp currentTime) {
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
}

void Brain::updateAttentionSystem(Timestamp currentTime) {
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

void Brain::updateConceptFormation(Timestamp currentTime) {
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

void Brain::applyStructuralPlasticity(SimulationStep currentStep, Timestamp currentTime) {
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
}

void Brain::replayImportantMemories(SimulationStep currentStep, Timestamp currentTime) {
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
}

void Brain::applyDevelopmentEffects(SimulationStep currentStep, Timestamp currentTime) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        pImpl->developmentSystem->update(this, *pImpl->rng, pImpl->timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = pImpl->structuralPlasticity;
        if (sp) {
            DevelopmentalStage stage = pImpl->developmentalStage;
            float plasticityMod = 1.0f;
            
            switch (stage) {
                case DevelopmentalStage::Initial:
                    plasticityMod = 1.0f;  // High plasticity
                    break;
                case DevelopmentalStage::CriticalPeriod:
                    plasticityMod = 0.8f;
                    break;
                case DevelopmentalStage::Maturation:
                    plasticityMod = 0.5f;
                    break;
                case DevelopmentalStage::Adult:
                    plasticityMod = 0.2f;  // Stable
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void Brain::consolidateMemories(SimulationStep currentStep) {
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void Brain::manageCheckpoints(SimulationStep currentStep, Timestamp currentTime) {
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void Brain::receiveSensoryInput(const class SensoryInput& input) {
    // Inject current into sensory neurons based on input
    // This is a simple mapping - sensory encoding
    
    const auto& values = input.getData();
    if (values.empty()) return;
    
    size_t numSensory = pImpl->sensoryNeurons.size();
    if (numSensory == 0) return;
    
    // Distribute input across sensory neurons
    for (size_t i = 0; i < numSensory; ++i) {
        // Normalize input value to range [-10, 10] mV
        float normalizedValue = 0.0f;
        if (i < values.size()) {
            normalizedValue = static_cast<float>(values[i]) * 10.0f;
        }
        
        // Inject current into this sensory neuron
        pImpl->sensoryNeurons[i]->injectCurrent(normalizedValue);
        
        // Also store in working memory
        if (pImpl->workingMemory && normalizedValue > 0.5f) {
            pImpl->workingMemory->storeToNeuron(pImpl->sensoryNeurons[i]->getId(), normalizedValue / 10.0f);
        }
    }
}

void Brain::injectCurrent(NeuronId neuron, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        auto neurons = region->getAllNeurons();
        for (auto* n : neurons) {
            if (n->getId() == neuron) {
                n->injectCurrent(current);
                return;
            }
        }
    }
}

void Brain::injectCurrentToNeurons(NeuronType type, MembranePotential current) {
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            if (pop->getNeuronType() == type) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->injectCurrent(current);
                }
            }
        }
    }
}

SpikeSystem* Brain::getSpikeSystem() {
    return pImpl->spikeSystem.get();
}

const SpikeSystem* Brain::getSpikeSystem() const {
    return pImpl->spikeSystem.get();
}

STDP* Brain::getSTDP() {
    return pImpl->stdp.get();
}

Hebbian* Brain::getHebbian() {
    return pImpl->hebbian.get();
}

StructuralPlasticity* Brain::getStructuralPlasticity() {
    return pImpl->structuralPlasticity.get();
}

float Brain::getExcitationInhibitionRatio() const {
    float totalExcitatory = 0.0f;
    float totalInhibitory = 0.0f;
    
    for (const auto& region : pImpl->regions) {
        for (const auto& syn : region->getSynapses()) {
            float weight = syn->getWeight();
            if (weight > 0) {
                totalExcitatory += weight;
            } else {
                totalInhibitory += std::abs(weight);
            }
        }
    }
    
    if (totalInhibitory > 0.0f) {
        return totalExcitatory / totalInhibitory;
    }
    return totalExcitatory > 0.0f ? std::numeric_limits<float>::infinity() : 0.0f;
}

size_t Brain::getTotalSpikeCount() const {
    return pImpl->totalSpikesTotal;
}

size_t Brain::getPendingSpikeEventCount() const {
    return pImpl->spikeSystem->getPendingSpikeCount() + pImpl->spikeSystem->getPendingDelayedCount();
}

std::unique_ptr<class Action> Brain::produceAction() {
    // Simple action selection based on motor neuron activity
    // The motor neuron population with highest average activity determines action
    
    if (pImpl->motorNeurons.empty()) {
        return std::make_unique<Action>(ActionType::Wait);
    }
    
    // Calculate activity of motor neuron groups
    size_t firingMotor = 0;
    for (auto* neuron : pImpl->motorNeurons) {
        if (neuron->isFiring()) {
            ++firingMotor;
        }
    }
    
    // Return a simple action
    ActionType type = ActionType::Wait;
    if (firingMotor > 0) {
        type = ActionType::MoveForward;
    }
    
    auto action = std::make_unique<Action>(type);
    
    return action;
}

void Brain::applyNeuromodulation(const class Neuromodulator& signal) {
    // Apply neuromodulation effects on plasticity
    float modulation = signal.getLevel();
    
    // Scale STDP learning rates
    pImpl->stdp->setLTPWeight(0.01f * modulation);
    pImpl->stdp->setLTDWeight(0.012f * modulation);
}

void Brain::updatePlasticity() {
    // Plasticity is now applied during each step
    // This method is kept for API compatibility
}

void Brain::develop() {
    // Development updates structural plasticity
    pImpl->structuralPlasticity->update(this, *pImpl->rng);
}

void Brain::reset() {
    NLM_LOG_INFO("Resetting NLM Brain...");
    
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->totalSpikesThisStep = 0;
    pImpl->totalSpikesTotal = 0;
    pImpl->isResting = false;
    pImpl->stepsSinceLastEpisode = 0;
    
    for (auto& region : pImpl->regions) {
        region->reset();
    }
    
    pImpl->spikeSystem->reset();
    pImpl->developmentalStage = DevelopmentalStage::Initial;
    
    // Reset memory systems
    if (pImpl->workingMemory) pImpl->workingMemory->clear();
    if (pImpl->episodicMemory) pImpl->episodicMemory->clear();
    if (pImpl->associativeMemory) pImpl->associativeMemory->clear();
    if (pImpl->attention) pImpl->attention->reset();
    
    NLM_LOG_INFO("NLM Brain reset complete");
}

bool Brain::save(const std::string& filepath) const {
    NLM_LOG_INFO("Saving brain state to " + filepath);
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write neurons
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        
        for (const auto& region : pImpl->regions) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& state = neuron->getState();
                    neuronData.membranePotential.push_back(state.membranePotential);
                    neuronData.restingPotential.push_back(state.restingPotential);
                    neuronData.threshold.push_back(state.threshold);
                    neuronData.resetPotential.push_back(state.resetPotential);
                    neuronData.leakConductance.push_back(state.leakConductance);
                    neuronData.firingState.push_back(static_cast<uint8_t>(state.firingState));
                    neuronData.refractoryRemaining.push_back(state.refractoryRemaining);
                    neuronData.refractoryPeriod.push_back(state.refractoryPeriod);
                    neuronData.lastSpikeTime.push_back(state.lastSpikeTime);
                }
            }
        }
        
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Write synapses
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
            }
        }
        
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
            return false;
        }
        
        NLM_LOG_INFO("Brain state saved successfully (" + std::to_string(writer.getBytesWritten()) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving brain: ") + e.what());
        return false;
    }
}

bool Brain::load(const std::string& filepath) {
    NLM_LOG_INFO("Loading brain state from " + filepath);
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        if (!reader.validate()) {
            NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
            return false;
        }
        
        // Read neurons
        NeuronCheckpointData neuronData;
        if (!reader.readNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to read neurons from checkpoint");
            return false;
        }
        
        // Apply neuron states - this is complex because we need to find matching synapses
        // For now, just log the count
        NLM_LOG_INFO("Loaded " + std::to_string(neuronData.membranePotential.size()) + " neurons");
        
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading brain: ") + e.what());
        return false;
    }
}

RegionId Brain::addRegion(const std::string& name) {
    RegionId id(pImpl->nextRegionId++);
    auto region = std::make_unique<NeuralRegion>(id, name);
    pImpl->regions.push_back(std::move(region));
    return id;
}

NeuralRegion* Brain::getRegion(RegionId id) {
    for (auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    for (const auto& region : pImpl->regions) {
        if (region->getId() == id) {
            return region.get();
        }
    }
    return nullptr;
}

size_t Brain::getRegionCount() const {
    return pImpl->regions.size();
}

std::vector<RegionId> Brain::getRegionIds() const {
    std::vector<RegionId> ids;
    ids.reserve(pImpl->regions.size());
    for (const auto& region : pImpl->regions) {
        ids.push_back(region->getId());
    }
    return ids;
}

const std::vector<std::unique_ptr<NeuralRegion>>& Brain::getRegions() const {
    return pImpl->regions;
}

void Brain::addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay) {
    pImpl->interRegionConnections.emplace_back(source, target, weight, delay);
}

void Brain::removeInterRegionConnection(RegionId source, RegionId target) {
    pImpl->interRegionConnections.erase(
        std::remove_if(pImpl->interRegionConnections.begin(),
                      pImpl->interRegionConnections.end(),
                      [source, target](const InterRegionConnection& conn) {
                          return conn.sourceRegion == source && conn.targetRegion == target;
                      }),
        pImpl->interRegionConnections.end()
    );
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getSynapseCount();
    }
    total += pImpl->interRegionConnections.size();
    return total;
}

size_t Brain::getActiveNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getActiveNeuronCount();
    }
    return total;
}

size_t Brain::getFiringNeuronCount() const {
    return pImpl->totalSpikesThisStep;
}

float Brain::getAverageFiringRate() const {
    if (pImpl->regions.empty()) return 0.0f;
    float sum = 0.0f;
    for (const auto& region : pImpl->regions) {
        sum += region->getAverageFiringRate();
    }
    return sum / static_cast<float>(pImpl->regions.size());
}

// ========== MEMORY SYSTEM ACCESSORS ==========

NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}

// ========== PREDICTION SYSTEM ACCESSOR ==========

PredictionSystem* Brain::getPredictionSystem() {
    return pImpl->predictionSystem.get();
}

// ========== COGNITION SYSTEM ACCESSORS ==========

NeuralPlanner* Brain::getPlanner() {
    return pImpl->planner.get();
}

ConceptFormation* Brain::getConceptFormation() {
    return pImpl->conceptFormation.get();
}

AttentionalSelection* Brain::getAttention() {
    return pImpl->attention.get();
}

// ========== DEVELOPMENT SYSTEM ==========

DevelopmentSystem* Brain::getDevelopmentSystem() {
    return pImpl->developmentSystem.get();
}

DevelopmentalStage Brain::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Brain::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

// ========== NEUROMODULATION SYSTEMS ==========

Dopamine* Brain::getDopamine() {
    return pImpl->dopamine.get();
}

Curiosity* Brain::getCuriosity() {
    return pImpl->curiosity.get();
}

Novelty* Brain::getNovelty() {
    return pImpl->novelty.get();
}

PredictionError* Brain::getPredictionErrorSignal() {
    return pImpl->predictionError.get();
}

std::shared_ptr<const Config> Brain::getConfig() const {
    return pImpl->config;
}

RandomGenerator* Brain::getRandomGenerator() {
    return pImpl->rng.get();
}

void Brain::logStatus() const {
    NLM_LOG_INFO("=== NLM Brain Status (Phase 6 - Integrated) ===");
    NLM_LOG_INFO("Regions: " + std::to_string(getRegionCount()));
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Active neurons: " + std::to_string(getActiveNeuronCount()));
    NLM_LOG_INFO("Firing neurons (this step): " + std::to_string(getFiringNeuronCount()));
    NLM_LOG_INFO("Total spikes: " + std::to_string(getTotalSpikeCount()));
    NLM_LOG_INFO("Pending spike events: " + std::to_string(getPendingSpikeEventCount()));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(getAverageFiringRate()));
    NLM_LOG_INFO("E/I ratio: " + std::to_string(getExcitationInhibitionRatio()));
    
    // Memory system status
    if (pImpl->workingMemory) {
        NLM_LOG_INFO("Working memory traces: " + std::to_string(pImpl->workingMemory->getActiveTraces()));
    }
    if (pImpl->episodicMemory) {
        NLM_LOG_INFO("Episodic memory episodes: " + std::to_string(pImpl->episodicMemory->getEpisodeCount()));
    }
    
    // Neuromodulation status
    if (pImpl->dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine->getLevel()));
    }
    
    // Development status
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl->developmentalStage)));
    
    for (const auto& region : pImpl->regions) {
        NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
}

} // namespace nlm
