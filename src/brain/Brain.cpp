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
#include "../core/Utilities/MathUtils.hpp"
#include "../core/Utilities/ValidationUtils.hpp"
#include "../core/Utilities/StringUtils.hpp"
#include "../core/Utilities/ContainerUtils.hpp"
#include "../core/Utilities/BrainUtils.hpp"
#include "../common/Common.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <numeric>
#include <thread>
#include <future>

namespace nlm {

// Helper functions extracted from Brain.cpp
namespace BrainImpl {
    
    // Advanced plasticity rule helpers
    bool validateCustomSTDPParameters(float learningRate, float tau, float weightDependent) {
        if (learningRate <= 0.0f || learningRate > 1.0f) {
            NLM_LOG_ERROR(std::string("Brain: validateCustomSTDPParameters failed: learningRate (") + 
                         std::to_string(learningRate) + std::string(") must be in range (0.0, 1.0]"));
            return false;
        }
        if (tau <= 0.0f || tau > 100.0f) {
            NLM_LOG_ERROR(std::string("Brain: validateCustomSTDPParameters failed: tau (") + 
                         std::to_string(tau) + std::string(") must be in range (0.0, 100.0]"));
            return false;
        }
        if (weightDependent < 0.0f || weightDependent > 1.0f) {
            NLM_LOG_ERROR(std::string("Brain: validateCustomSTDPParameters failed: weightDependent (") + 
                         std::to_string(weightDependent) + std::string(") must be in range [0.0, 1.0]"));
            return false;
        }
        return true;
    }
    
    bool validateCustomHebbianParameters(float learningRate, float targetCorrelation, float decayRate) {
        if (learningRate <= 0.0f || learningRate > 1.0f) {
            NLM_LOG_ERROR(std::string("Brain: validateCustomHebbianParameters failed: learningRate (") + 
                         std::to_string(learningRate) + std::string(") must be in range (0.0, 1.0]"));
            return false;
        }
        if (targetCorrelation < 0.0f || targetCorrelation > 1.0f) {
            NLM_LOG_ERROR(std::string("Brain: validateCustomHebbianParameters failed: targetCorrelation (") + 
                         std::to_string(targetCorrelation) + std::string(") must be in range [0.0, 1.0]"));
            return false;
        }
        if (decayRate < 0.0f || decayRate > 1.0f) {
            NLM_LOG_ERROR(std::string("Brain: validateCustomHebbianParameters failed: decayRate (") + 
                         std::to_string(decayRate) + std::string(") must be in range [0.0, 1.0]"));
            return false;
        }
        return true;
    }
    
    bool validateRewardModulatedSTDPParameters(float rewardScaling, float baseline, float decay) {
        if (rewardScaling < 0.0f || rewardScaling > 10.0f) {
            NLM_LOG_ERROR(std::string("Brain: validateRewardModulatedSTDPParameters failed: rewardScaling (") + 
                         std::to_string(rewardScaling) + std::string(") must be in range [0.0, 10.0]"));
            return false;
        }
        if (baseline < -1.0f || baseline > 1.0f) {
            NLM_LOG_ERROR(std::string("Brain: validateRewardModulatedSTDPParameters failed: baseline (") + 
                         std::to_string(baseline) + std::string(") must be in range [-1.0, 1.0]"));
            return false;
        }
        if (decay <= 0.0f || decay > 1.0f) {
            NLM_LOG_ERROR(std::string("Brain: validateRewardModulatedSTDPParameters failed: decay (") + 
                         std::to_string(decay) + std::string(") must be in range (0.0, 1.0]"));
            return false;
        }
        return true;
    }
    
    // Advanced simulation control helpers
    bool setupRealTimeParameterAdjustment(std::shared_ptr<Config> config);
    bool setupCheckpointManagement(std::unique_ptr<CheckpointManager>& checkpointManager,
                                   const std::string& checkpointDir, size_t maxCheckpoints);
    
    // Custom neuron model helpers
    bool setupCustomNeuronModels(std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                const std::map<std::string, std::function<double(const NeuronState&)>>& customDynamics);
    
    // Validation helper functions
    bool validateConfiguration(std::shared_ptr<Config> config);
    bool validateNeuronCount(size_t neuronCount);
    bool validateRegionCount(size_t regionCount);
    bool validateConnectionProbability(float probability);
    bool validateTimestep(double timestep);
    bool validateIntegrationIntervals(size_t replayInterval, size_t consolidationInterval);
    
    // Initialization helper functions
    void initializeRandomGenerator(std::unique_ptr<RandomGenerator>& rng, std::shared_ptr<Config> config);
    void initializePlasticitySystems(std::unique_ptr<SpikeSystem>& spikeSystem,
                                    std::unique_ptr<STDP>& stdp,
                                    std::unique_ptr<Hebbian>& hebbian,
                                    std::unique_ptr<StructuralPlasticity>& structuralPlasticity,
                                    std::shared_ptr<Config> config);
    void initializeMemorySystems(std::unique_ptr<NeuralWorkingMemory>& workingMemory,
                                 std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                                 std::unique_ptr<NeuralAssociativeMemory>& associativeMemory);
    void initializePredictionSystem(std::unique_ptr<PredictionSystem>& predictionSystem);
    void initializeCognitionSystems(std::unique_ptr<NeuralPlanner>& planner,
                                    std::unique_ptr<ConceptFormation>& conceptFormation,
                                    std::unique_ptr<AttentionalSelection>& attention);
    void initializeDevelopmentSystem(std::unique_ptr<DevelopmentSystem>& developmentSystem);
    void initializeNeuromodulationSystems(std::unique_ptr<Dopamine>& dopamine,
                                          std::unique_ptr<Curiosity>& curiosity,
                                          std::unique_ptr<PredictionError>& predictionError,
                                          std::unique_ptr<Novelty>& novelty);
    
    // Configuration helper functions
    void configureSTDPParameters(std::unique_ptr<STDP>& stdp, std::shared_ptr<Config> config);
    void configureStructuralPlasticity(std::unique_ptr<StructuralPlasticity>& structuralPlasticity, std::shared_ptr<Config> config);
    void configureCognitionSystems(std::unique_ptr<NeuralPlanner>& planner,
                                   std::unique_ptr<ConceptFormation>& conceptFormation,
                                   std::unique_ptr<AttentionalSelection>& attention,
                                   std::shared_ptr<Config> config);
    
    // Statistics and reporting helper functions
    void collectStatistics(Brain::Impl* pImpl);
    
    // Error handling helper functions
    void logConfigurationError(const std::string& field, double value, double min, double max);
    void logConfigurationWarning(const std::string& field, double value, double min, double max);
    
    // Utility functions
    void setupSensoryMotorNeurons(std::vector<Neuron*>& sensoryNeurons, std::vector<Neuron*>& motorNeurons,
                                 std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
} // namespace BrainImpl

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
        // Initialize using extracted utility functions
        BrainImpl::initializeRandomGenerator(rng, config);
        BrainImpl::initializePlasticitySystems(spikeSystem, stdp, hebbian, structuralPlasticity, config);
        BrainImpl::initializeMemorySystems(workingMemory, episodicMemory, associativeMemory);
        BrainImpl::initializePredictionSystem(predictionSystem);
        BrainImpl::initializeCognitionSystems(planner, conceptFormation, attention);
        BrainImpl::initializeDevelopmentSystem(developmentSystem);
        BrainImpl::initializeNeuromodulationSystems(dopamine, curiosity, predictionError, novelty);
        
        BrainImpl::configureSTDPParameters(stdp, config);
        BrainImpl::configureStructuralPlasticity(structuralPlasticity, config);
        BrainImpl::configureCognitionSystems(planner, conceptFormation, attention, config);
        
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Validate timestep using utility function
        if (!BrainImpl::validateTimestep(timestep)) {
            NLM_LOG_ERROR(std::string("Brain::Impl constructor failed: invalid simulation_timestep (") + 
                         std::to_string(timestep) + "). Must be > 0.0 and <= 1.0");
            return;
        }
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Validate integration intervals using utility function
        if (!BrainImpl::validateIntegrationIntervals(replayInterval, consolidationInterval)) {
            NLM_LOG_ERROR("Brain::Impl constructor failed: invalid integration intervals");
            return;
        }
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
        if (!checkpointManager) {
            NLM_LOG_WARNING("Brain::Impl constructor: checkpoint manager initialization failed - continuing without checkpointing");
        }
    }
};

// Implementation of extracted helper functions
namespace BrainImpl {
    
    bool validateConfiguration(std::shared_ptr<Config> config) {
        if (!config) {
            NLM_LOG_ERROR("Brain::Impl constructor failed: config is null");
            return false;
        }
        return true;
    }
    
    bool validateNeuronCount(size_t neuronCount) {
        if (neuronCount == 0) {
            NLM_LOG_ERROR(std::string("Brain::initialize failed: invalid neuron_count (0). Must be > 0"));
            return false;
        }
        return true;
    }
    
    bool validateRegionCount(size_t regionCount) {
        if (regionCount == 0) {
            NLM_LOG_ERROR(std::string("Brain::initialize failed: invalid region_count (0). Must be > 0"));
            return false;
        }
        return true;
    }
    
    bool validateConnectionProbability(float probability) {
        if (probability < 0.0f || probability > 1.0f) {
            NLM_LOG_ERROR(std::string("Brain::initialize failed: invalid connection_probability (") + 
                         std::to_string(probability) + "). Must be in range [0.0, 1.0]"));
            return false;
        }
        return true;
    }
    
    bool validateTimestep(double timestep) {
        if (timestep <= 0.0 || timestep > 1.0) {
            return false;
        }
        return true;
    }
    
    bool validateIntegrationIntervals(size_t replayInterval, size_t consolidationInterval) {
        if (replayInterval == 0) {
            NLM_LOG_ERROR("Brain::Impl constructor failed: invalid replay_interval (0). Must be > 0");
            return false;
        }
        if (consolidationInterval == 0) {
            NLM_LOG_ERROR("Brain::Impl constructor failed: invalid consolidation_interval (0). Must be > 0");
            return false;
        }
        if (consolidationInterval < replayInterval) {
            NLM_LOG_WARNING(std::string("Brain::Impl constructor: consolidation_interval (") + 
                           std::to_string(consolidationInterval) + ") < replay_interval (" + 
                           std::to_string(replayInterval) + ")");
        }
        return true;
    }
    
    void initializeRandomGenerator(std::unique_ptr<RandomGenerator>& rng, std::shared_ptr<Config> config) {
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Validate random generator initialization
        if (!rng) {
            NLM_LOG_ERROR("Brain::Impl constructor failed: failed to create random generator");
            return;
        }
    }
    
    void initializePlasticitySystems(std::unique_ptr<SpikeSystem>& spikeSystem,
                                    std::unique_ptr<STDP>& stdp,
                                    std::unique_ptr<Hebbian>& hebbian,
                                    std::unique_ptr<StructuralPlasticity>& structuralPlasticity,
                                    std::shared_ptr<Config> config) {
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Validate plasticity systems initialization
        if (!spikeSystem || !stdp || !hebbian || !structuralPlasticity) {
            NLM_LOG_ERROR("Brain::Impl constructor failed: failed to initialize plasticity systems");
            return;
        }
    }
    
    void initializeMemorySystems(std::unique_ptr<NeuralWorkingMemory>& workingMemory,
                                 std::unique_ptr<NeuralEpisodicMemory>& episodicMemory,
                                 std::unique_ptr<NeuralAssociativeMemory>& associativeMemory) {
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Validate memory systems initialization
        if (!workingMemory || !episodicMemory || !associativeMemory) {
            NLM_LOG_ERROR("Brain::Impl constructor failed: failed to initialize memory systems");
            return;
        }
    }
    
    void initializePredictionSystem(std::unique_ptr<PredictionSystem>& predictionSystem) {
        predictionSystem = std::make_unique<PredictionSystem>();
        if (!predictionSystem) {
            NLM_LOG_WARNING("Brain::Impl constructor: prediction system initialization failed - continuing without prediction");
        }
    }
    
    void initializeCognitionSystems(std::unique_ptr<NeuralPlanner>& planner,
                                    std::unique_ptr<ConceptFormation>& conceptFormation,
                                    std::unique_ptr<AttentionalSelection>& attention) {
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Validate cognition systems initialization
        if (!planner || !conceptFormation || !attention) {
            NLM_LOG_ERROR("Brain::Impl constructor failed: failed to initialize cognition systems");
            return;
        }
    }
    
    void initializeDevelopmentSystem(std::unique_ptr<DevelopmentSystem>& developmentSystem) {
        developmentSystem = std::make_unique<DevelopmentSystem>();
        if (!developmentSystem) {
            NLM_LOG_ERROR("Brain::Impl constructor failed: failed to initialize development system");
            return;
        }
    }
    
    void initializeNeuromodulationSystems(std::unique_ptr<Dopamine>& dopamine,
                                          std::unique_ptr<Curiosity>& curiosity,
                                          std::unique_ptr<PredictionError>& predictionError,
                                          std::unique_ptr<Novelty>& novelty) {
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Validate neuromodulation systems initialization
        if (!dopamine || !curiosity || !predictionError || !novelty) {
            NLM_LOG_ERROR("Brain::Impl constructor failed: failed to initialize neuromodulation systems");
            return;
        }
    }
    
    void configureSTDPParameters(std::unique_ptr<STDP>& stdp, std::shared_ptr<Config> config) {
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        
        // Validate plasticity parameter ranges using utility function
        if (ltpWeight < 0.0f || ltpWeight > 1.0f) {
            NLM_LOG_WARNING(std::string("Brain::Impl constructor: stdp_ltp_weight (") + 
                           std::to_string(ltpWeight) + ") outside expected range [0.0, 1.0]"));
        }
        if (ltdWeight < 0.0f || ltdWeight > 1.0f) {
            NLM_LOG_WARNING(std::string("Brain::Impl constructor: stdp_ltd_weight (") + 
                           std::to_string(ltdWeight) + ") outside expected range [0.0, 1.0]"));
        }
        if (tau <= 0.0f) {
            NLM_LOG_WARNING(std::string("Brain::Impl constructor: stdp_tau (") + 
                           std::to_string(tau) + ") must be > 0.0");
        }
        
        stdp->configure(ltpWeight, ltdWeight, tau);
    }
    
    void configureStructuralPlasticity(std::unique_ptr<StructuralPlasticity>& structuralPlasticity, std::shared_ptr<Config> config) {
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        
        // Validate plasticity parameter ranges using utility function
        if (synaptogenesisRate < 0.0f || synaptogenesisRate > 0.1f) {
            NLM_LOG_WARNING(std::string("Brain::Impl constructor: synaptogenesis_rate (") + 
                           std::to_string(synaptogenesisRate) + ") outside expected range [0.0, 0.1]"));
        }
        if (pruningRate < 0.0f || pruningRate > 0.01f) {
            NLM_LOG_WARNING(std::string("Brain::Impl constructor: pruning_rate (") + 
                           std::to_string(pruningRate) + ") outside expected range [0.0, 0.01]"));
        }
        
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
    
    void configureCognitionSystems(std::unique_ptr<NeuralPlanner>& planner,
                                   std::unique_ptr<ConceptFormation>& conceptFormation,
                                   std::unique_ptr<AttentionalSelection>& attention,
                                   std::shared_ptr<Config> config) {
        planner->initialize(nullptr);
        planner->setPlanningDepth(5);
        
        conceptFormation->initialize(nullptr);
        
        attention->initialize(nullptr);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
    
    void setupSensoryMotorNeurons(std::vector<Neuron*>& sensoryNeurons, std::vector<Neuron*>& motorNeurons,
                                 std::vector<std::unique_ptr<NeuralRegion>>& regions) {
        for (size_t i = 0; i < regions.size(); ++i) {
            auto* region = regions[i].get();
            if (!region) continue;
            
            // Add populations
            auto sensoryPopId = region->addPopulation(100, NeuronType::Sensory);
            auto internalPopId = region->addPopulation(200, NeuronType::Internal);
            auto motorPopId = region->addPopulation(50, NeuronType::Motor);
            
            // Collect sensory and motor neurons for I/O
            auto* sensoryPop = region->getPopulation(sensoryPopId);
            auto* motorPop = region->getPopulation(motorPopId);
            if (sensoryPop) {
                for (auto* neuron : sensoryPop->getNeurons()) {
                    sensoryNeurons.push_back(neuron);
                }
            }
            if (motorPop) {
                for (auto* neuron : motorPop->getNeurons()) {
                    motorNeurons.push_back(neuron);
                }
            }
        }
    }
    
    void collectStatistics(Brain::Impl* pImpl) {
        // Basic statistics collection
        size_t totalNeurons = 0;
        size_t totalPopulations = 0;
        size_t totalSynapses = 0;
        
        for (auto& region : pImpl->regions) {
            totalNeurons += region->getTotalNeuronCount();
            totalPopulations += region->getPopulationCount();
            // Calculate synapses from populations
            for (auto& pop : region->getPopulations()) {
                totalSynapses += pop->getTotalSynapseCount();
            }
        }
        
        NLM_LOG_INFO("Statistics: " + Utils::StringUtils::formatEfficient(
            "Total neurons: {}, Total populations: {}, Total synapses: {}",
            totalNeurons, totalPopulations, totalSynapses));
    }
    
    void logConfigurationError(const std::string& field, double value, double min, double max) {
        NLM_LOG_ERROR(Utils::StringUtils::formatEfficient(
            "Configuration error: {} = {} is outside expected range [{}, {}]",
            field, value, min, max));
    }
    
    void logConfigurationWarning(const std::string& field, double value, double min, double max) {
        NLM_LOG_WARNING(Utils::StringUtils::formatEfficient(
            "Configuration warning: {} = {} is outside expected range [{}, {}]",
            field, value, min, max));
    }
    
} // namespace BrainImpl

bool Brain::initialize() {
    // Validate configuration using extracted utility function
    if (!BrainImpl::validateConfiguration(pImpl->config)) {
        return false;
    }
    
    // Get and validate configuration parameters using extracted utility functions
    size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
    size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
    float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
    
    // Validate parameter ranges using extracted utility functions
    if (!BrainImpl::validateNeuronCount(neuronCount)) {
        return false;
    }
    if (!BrainImpl::validateRegionCount(regionCount)) {
        return false;
    }
    if (!BrainImpl::validateConnectionProbability(connectionProbability)) {
        return false;
    }
    
    // Check for existing regions to avoid duplicate initialization
    if (!pImpl->regions.empty()) {
        NLM_LOG_WARNING(std::string("Brain::initialize: brain already initialized with " + 
                     std::to_string(pImpl->regions.size()) + " regions. Resetting first."));
        reset();
    }
    
    NLM_LOG_INFO("Initializing NLM Brain (Phase 6: Integrated Artificial Brain)...");
    
    NLM_LOG_INFO(Utils::StringUtils::formatEfficient(
        "Configuration: {} neurons, {} regions, connection probability: {}",
        neuronCount, regionCount, connectionProbability));
    
    // Validate that we can create regions
    if (regionCount > 1000) {
        NLM_LOG_ERROR(std::string("Brain::initialize failed: region_count (") + 
                     std::to_string(regionCount) + ") exceeds maximum allowed (1000)"));
        return false;
    }
    
    // Create regions
    for (size_t i = 0; i < regionCount; ++i) {
        try {
            addRegion(Utils::BrainUtils::formatRegionName("Region_", i + 1));
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(Utils::StringUtils::formatEfficient(
                "Brain::initialize failed: failed to create region {}: {}",
                i + 1, e.what()));
            return false;
        }
    }
    
    // Validate that regions were created successfully
    if (pImpl->regions.size() != regionCount) {
        NLM_LOG_ERROR(Utils::StringUtils::formatEfficient(
            "Brain::initialize failed: expected to create {} regions, but created {}",
            regionCount, pImpl->regions.size()));
        return false;
    }
    
    // Setup sensory and motor neurons using extracted utility function
    BrainImpl::setupSensoryMotorNeurons(pImpl->sensoryNeurons, pImpl->motorNeurons, pImpl->regions);
    
    // Initialize working memory
    if (pImpl->workingMemory) {
        pImpl->workingMemory->initialize(this);
        pImpl->workingMemory->setCapacity(neuronCount / 10);
    }
    
    // Initialize episodic memory
    if (pImpl->episodicMemory) {
        pImpl->episodicMemory->initialize(this);
        pImpl->episodicMemory->setMaxEpisodes(1000);
    }
    
    // Initialize associative memory
    if (pImpl->associativeMemory) {
        pImpl->associativeMemory->initialize(this);
    }
    
    // Initialize prediction system
    if (pImpl->predictionSystem) {
        // (PredictionSystem doesn't have initialize method currently)
    }
    
    // Initialize cognition systems
    if (pImpl->planner) {
        pImpl->planner->initialize(this);
        pImpl->planner->setPlanningDepth(5);
    }
    
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->initialize(this);
    }
    
    if (pImpl->attention) {
        pImpl->attention->initialize(this);
        pImpl->attention->setInhibitionStrength(0.5f);
        pImpl->attention->setExcitationStrength(1.5f);
    }
    
    // Initialize neuromodulation
    if (pImpl->novelty) {
        pImpl->novelty->initialize(this);
    }
    if (pImpl->curiosity) {
        pImpl->curiosity->initialize(this);
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
    std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->configure(checkpointDir, 10000, 5, true);
    }
    
    // Collect statistics using extracted utility function
    BrainImpl::collectStatistics(pImpl);
    
    NLM_LOG_INFO("NLM Brain initialization complete (Phase 6 - Integrated)");
    NLM_LOG_INFO("Total neurons: " + std::to_string(getTotalNeuronCount()));
    NLM_LOG_INFO("Total synapses: " + std::to_string(getTotalSynapseCount()));
    NLM_LOG_INFO("Sensory neurons: " + std::to_string(pImpl->sensoryNeurons.size()));
    NLM_LOG_INFO("Motor neurons: " + std::to_string(pImpl->motorNeurons.size()));
    
    return true;
}

void Brain::step(SimulationStep currentStep) {
    step(currentStep, static_cast<Timestamp>(currentStep) * pImpl->timestep);
}

void Brain::step(SimulationStep currentStep, Timestamp currentTime) {
    // Validate essential components
    if (!pImpl->spikeSystem) {
        NLM_LOG_ERROR("Brain::step failed: spike system not initialized");
        return;
    }
    
    if (pImpl->regions.empty()) {
        NLM_LOG_ERROR("Brain::step failed: no regions initialized");
        return;
    }
    
    if (pImpl->currentStep == 0 && pImpl->regions.empty()) {
        NLM_LOG_WARNING("Brain::step: warning - brain initialized but no regions present");
    }
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    try {
        // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
        if (!pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime)) {
            NLM_LOG_WARNING("Brain::step: warning - spike system failed to process delayed spikes");
        }
        
        // ========== STEP 2: Update all neurons (LIF dynamics) ==========
        size_t totalNeuronsUpdated = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    neuron->stepLIF(currentTime, pImpl->timestep);
                    totalNeuronsUpdated++;
                }
            }
        }
        
        if (totalNeuronsUpdated == 0) {
            NLM_LOG_WARNING("Brain::step: warning - no neurons were updated during this step");
        }
        
        // ========== STEP 3: Detect spikes and schedule spike events ==========
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
                        
                        // Register detailed spike event
                        DetailedSpikeEvent detailedEvent(neuron->getId(), currentTime, currentStep,
                                                         neuron->getMembranePotential(), neuron->getThreshold(),
                                                         neuron->getRefractoryPeriod(), neuron->getAdaptationCurrent());
                        pImpl->spikeSystem->registerDetailedSpike(detailedEvent);
                    }
                }
            }
        }
        
        // ========== STEP 4: Update working memory ==========
        if (pImpl->workingMemory) {
            pImpl->workingMemory->update(currentTime, pImpl->timestep);
        }
        
        // ========== STEP 5: Apply neuromodulation effects ==========
        if (pImpl->dopamine) {
            pImpl->dopamine->applyModulation(currentTime, pImpl->timestep);
        }
        if (pImpl->curiosity) {
            pImpl->curiosity->applyModulation(currentTime, pImpl->timestep);
        }
        if (pImpl->predictionError) {
            pImpl->predictionError->applyModulation(currentTime, pImpl->timestep);
        }
        if (pImpl->novelty) {
            pImpl->novelty->applyModulation(currentTime, pImpl->timestep);
        }
        
        // ========== STEP 6: Apply plasticity rules ==========
        if (pImpl->stdp) {
            pImpl->stdp->applyPlasticity(currentTime, pImpl->timestep);
        }
        if (pImpl->hebbian) {
            pImpl->hebbian->applyPlasticity(currentTime, pImpl->timestep);
        }
        
        // ========== STEP 7: Apply structural plasticity ==========
        if (pImpl->structuralPlasticity) {
            pImpl->structuralPlasticity->applyPlasticity(currentTime, pImpl->timestep);
        }
        
        // ========== STEP 8: Update episodic memory ==========
        if (pImpl->episodicMemory) {
            pImpl->episodicMemory->update(currentTime, pImpl->timestep);
        }
        
        // ========== STEP 9: Update prediction system ==========
        if (pImpl->predictionSystem) {
            // (PredictionSystem doesn't have update method currently)
        }
        
        // ========== STEP 10: Update attention system ==========
        if (pImpl->attention) {
            pImpl->attention->update(currentTime, pImpl->timestep);
        }
        
        // ========== STEP 11: Update concept formation ==========
        if (pImpl->conceptFormation) {
            pImpl->conceptFormation->update(currentTime, pImpl->timestep);
        }
        
        // ========== STEP 12: Apply development effects ==========
        if (pImpl->developmentSystem) {
            pImpl->developmentSystem->update(currentTime, pImpl->timestep);
        }
        
        // ========== STEP 13: Replay important memories ==========
        pImpl->stepsSinceLastEpisode++;
        if (pImpl->stepsSinceLastEpisode >= pImpl->replayInterval) {
            if (pImpl->episodicMemory) {
                pImpl->episodicMemory->replay(pImpl->replayInterval);
            }
            pImpl->stepsSinceLastEpisode = 0;
        }
        
        // ========== STEP 14: Collect statistics ==========
        collectStatistics(pImpl);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain::step failed: exception caught: ") + e.what());
    }
}

void Brain::addRegion(const std::string& name) {
    auto region = std::make_unique<NeuralRegion>(nextRegionId++, name);
    if (!region) {
        NLM_LOG_ERROR(std::string("Brain::addRegion failed: failed to create region: ") + name);
        throw std::runtime_error("Failed to create region");
    }
    
    // Try to initialize the region
    if (!region->initialize(*pImpl->rng, 0.1f)) {
        NLM_LOG_ERROR(std::string("Brain::addRegion failed: failed to initialize region: ") + name);
        throw std::runtime_error("Failed to initialize region");
    }
    
    regions.push_back(std::move(region));
    NLM_LOG_INFO(std::string("Brain::addRegion: created region ") + name + 
                 std::string(" (id: ") + std::to_string(nextRegionId - 1) + ")");
}

NeuralRegion* Brain::getRegion(RegionId id) {
    auto it = std::find_if(regions.begin(), regions.end(),
        [id](const std::unique_ptr<NeuralRegion>& region) {
            return region->getId() == id;
        });
    
    return (it != regions.end()) ? it->get() : nullptr;
}

const NeuralRegion* Brain::getRegion(RegionId id) const {
    auto it = std::find_if(regions.begin(), regions.end(),
        [id](const std::unique_ptr<NeuralRegion>& region) {
            return region->getId() == id;
        });
    
    return (it != regions.end()) ? it->get() : nullptr;
}

void Brain::reset() {
    NLM_LOG_INFO("Brain::reset: resetting brain state");
    
    // Clear all regions
    regions.clear();
    interRegionConnections.clear();
    
    // Clear neuron collections
    sensoryNeurons.clear();
    motorNeurons.clear();
    
    // Reset statistics
    totalSpikesThisStep = 0;
    totalSpikesTotal = 0;
    currentStep = 0;
    currentTime = 0.0;
    isResting = false;
    stepsSinceLastEpisode = 0;
    
    // Reset integrated systems if they exist
    if (workingMemory) workingMemory->reset();
    if (episodicMemory) episodicMemory->reset();
    if (associativeMemory) associativeMemory->reset();
    if (predictionSystem) predictionSystem->reset();
    if (planner) planner->reset();
    if (conceptFormation) conceptFormation->reset();
    if (attention) attention->reset();
    if (developmentSystem) developmentSystem->reset();
    if (dopamine) dopamine->reset();
    if (curiosity) curiosity->reset();
    if (predictionError) predictionError->reset();
    if (novelty) novelty->reset();
    if (spikeSystem) spikeSystem->reset();
    if (stdp) stdp->reset();
    if (hebbian) hebbian->reset();
    if (structuralPlasticity) structuralPlasticity->reset();
    if (checkpointManager) checkpointManager->reset();
    
    NLM_LOG_INFO("Brain::reset: brain state reset complete");
}

bool Brain::isResting() const {
    return pImpl->isResting;
}

void Brain::setResting(bool resting) {
    pImpl->isResting = resting;
    NLM_LOG_INFO(std::string("Brain::setResting: ") + (resting ? "enabled" : "disabled") + 
                 std::string(" rest mode"));
}

void Brain::setReplayInterval(size_t steps) {
    if (steps == 0) {
        NLM_LOG_ERROR(std::string("Brain::setReplayInterval failed: invalid interval (0). Must be > 0"));
        return;
    }
    pImpl->replayInterval = steps;
    NLM_LOG_INFO(std::string("Brain::setReplayInterval: set to " + std::to_string(steps) + " steps"));
}

void Brain::setConsolidationInterval(size_t steps) {
    if (steps == 0) {
        NLM_LOG_ERROR(std::string("Brain::setConsolidationInterval failed: invalid interval (0). Must be > 0"));
        return;
    }
    pImpl->consolidationInterval = steps;
    NLM_LOG_INFO(std::string("Brain::setConsolidationInterval: set to " + std::to_string(steps) + " steps"));
}

TimestepDuration Brain::getTimestep() const {
    return pImpl->timestep;
}

SimulationStep Brain::getCurrentStep() const {
    return pImpl->currentStep;
}

Timestamp Brain::getCurrentTime() const {
    return pImpl->currentTime;
}

size_t Brain::getTotalNeuronCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getTotalNeuronCount();
    }
    return total;
}

size_t Brain::getTotalPopulationCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        total += region->getPopulationCount();
    }
    return total;
}

size_t Brain::getTotalSynapseCount() const {
    size_t total = 0;
    for (const auto& region : pImpl->regions) {
        for (const auto& pop : region->getPopulations()) {
            total += pop->getTotalSynapseCount();
        }
    }
    return total;
}

Neuron* Brain::getSensoryNeuron(size_t index) const {
    if (index < pImpl->sensoryNeurons.size()) {
        return pImpl->sensoryNeurons[index];
    }
    return nullptr;
}

Neuron* Brain::getMotorNeuron(size_t index) const {
    if (index < pImpl->motorNeurons.size()) {
        return pImpl->motorNeurons[index];
    }
    return nullptr;
}

size_t Brain::getSensoryNeuronCount() const {
    return pImpl->sensoryNeurons.size();
}

size_t Brain::getMotorNeuronCount() const {
    return pImpl->motorNeurons.size();
}

bool Brain::isInitialized() const {
    return !pImpl->regions.empty() && pImpl->spikeSystem != nullptr;
}

Config* Brain::getConfig() const {
    return pImpl->config.get();
}

void Brain::exportToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR(std::string("Brain::exportToJSON failed: unable to open file: ") + filepath);
        return;
    }
    
    file << "{\"brain\": {\n";
    file << "  \"neuronCount\": " << getTotalNeuronCount() << ",\n";
    file << "  \"regionCount\": " << pImpl->regions.size() << ",\n";
    file << "  \"populationCount\": " << getTotalPopulationCount() << ",\n";
    file << "  \"synapseCount\": " << getTotalSynapseCount() << ",\n";
    file << "  \"sensoryNeurons\": " << getSensoryNeuronCount() << ",\n";
    file << "  \"motorNeurons\": " << getMotorNeuronCount() << ",\n";
    file << "  \"timestep\": " << pImpl->timestep << ",\n";
    file << "  \"currentStep\": " << pImpl->currentStep << ",\n";
    file << "  \"currentTime\": " << pImpl->currentTime << ",\n";
    file << "  \"isResting\": " << (pImpl->isResting ? "true" : "false") << "\n";
    
    file << "}\n";
    file.close();
    
    NLM_LOG_INFO(std::string("Brain::exportToJSON: exported brain state to " + filepath));
}

std::string Brain::getBrainStateJSON() const {
    std::ostringstream oss;
    oss << "{\"brain\": {\n";
    oss << "  \"neuronCount\": " << getTotalNeuronCount() << ",\n";
    oss << "  \"regionCount\": " << pImpl->regions.size() << ",\n";
    oss << "  \"populationCount\": " << getTotalPopulationCount() << ",\n";
    oss << "  \"synapseCount\": " << getTotalSynapseCount() << ",\n";
    oss << "  \"sensoryNeurons\": " << getSensoryNeuronCount() << ",\n";
    oss << "  \"motorNeurons\": " << getMotorNeuronCount() << ",\n";
    oss << "  \"timestep\": " << pImpl->timestep << ",\n";
    oss << "  \"currentStep\": " << pImpl->currentStep << ",\n";
    oss << "  \"currentTime\": " << pImpl->currentTime << ",\n";
    oss << "  \"isResting\": " << (pImpl->isResting ? "true" : "false") << "\n";
    oss << "}\n";
    
    return oss.str();
}

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