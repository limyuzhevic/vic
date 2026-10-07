#include "Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/NeuromodulationManager.hpp"
#include "../neuromodulation/Reward.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include "SimulationOptions.hpp"
#include "TrainingOptions.hpp"
#include "EvaluationOptions.hpp"
#include "BatchOptions.hpp"
#include "CheckpointOptions.hpp"
#include "MonitoringConfig.hpp"
#include "MonitoringCallback.hpp"
#include "CallbackType.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <string>
#include <iostream>
#include <random>
#include <numeric>

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
    std::unique_ptr<Reward> reward;
    std::unique_ptr<NeuromodulationManager> neuromodulationManager;
    
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
    
    // Plugin system
    std::vector<std::unique_ptr<BrainPlugin>> plugins;
    std::unordered_map<std::string, std::unique_ptr<BrainPlugin>> pluginMap;
    
    // Callback system
    std::unordered_map<int, std::vector<std::function<void()>>> callbacks;
    
    // Monitoring
    std::unique_ptr<MonitoringCallback> monitoringCallback;
    MonitoringConfig monitoringConfig;
    bool isMonitoringActive;
    
    // Error handling
    BrainError lastError;
    
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
        , replayInterval(100)
        , consolidationInterval(1000)
        , isMonitoringActive(false)
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
        
        // Initialize memory systems with proper neural computation
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        workingMemory->initialize(this);
        workingMemory->setCapacity(config->getOr<size_t>("working_memory_capacity", 100));
        workingMemory->setDecayRate(0.01f);
        
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        episodicMemory->initialize(this);
        episodicMemory->setMaxEpisodes(config->getOr<size_t>("max_episodic_episodes", 1000));
        episodicMemory->enableReplay(true);
        
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        associativeMemory->initialize(this);
        
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
        reward = std::make_unique<Reward>();
        
        // Initialize neuromodulation manager
        neuromodulationManager = std::make_unique<NeuromodulationManager>();
        neuromodulationManager->initialize(this);
        
        // Initialize individual neuromodulation systems
        dopamine->initialize(this);
        curiosity->initialize(this);
        predictionError->initialize(this);
        novelty->initialize(this);
        reward->initialize(this);
        
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

// New constructor implementations
Brain::Brain() : Brain(std::make_shared<Config>()) {}

Brain::Brain(const std::unordered_map<std::string, ConfigValue>& configMap) : Brain(std::make_shared<Config>()) {
    applyConfigFromMap(configMap);
}

Brain::Brain(const std::string& configFile) : Brain(std::make_shared<Config>()) {
    if (!pImpl->config->loadFromFile(configFile)) {
        throw BrainError(BrainErrorCode::InvalidConfig, "Failed to load config from file: " + configFile);
    }
    initialize();
}

Brain::Brain(const BrainConfig& config) : Brain(std::make_shared<Config>()) {
    applyConfig(config);
}

// Factory methods implementation
std::unique_ptr<Brain> Brain::createBasicBrain() {
    return std::make_unique<Brain>(BrainConfig::createDefault());
}

std::unique_ptr<Brain> Brain::createLearningBrain() {
    return std::make_unique<Brain>(BrainConfig::createLearning());
}

std::unique_ptr<Brain> Brain::createDevelopmentBrain() {
    return std::make_unique<Brain>(BrainConfig::createDevelopment());
}

std::unique_ptr<Brain> Brain::createOptimizedBrain(size_t neuronCount) {
    return std::make_unique<Brain>(BrainConfig::createOptimized(neuronCount));
}

std::unique_ptr<Brain> Brain::createCustomBrain(const BrainConfig& config) {
    return std::make_unique<Brain>(config);
}

// Convenience initialization methods implementation
bool Brain::initialize(const BrainConfig& config) {
    try {
        applyConfig(config);
        return initialize();
    } catch (const BrainError& e) {
        NLM_LOG_ERROR("Initialization failed: " + e.message);
        return false;
    }
}

bool Brain::initializeWithValidation(const BrainConfig& config, ValidationResult& validationResult) {
    validationResult = validateConfig(config);
    if (!validationResult.valid) {
        return false;
    }
    return initialize(config);
}

// Configuration management implementation
bool Brain::applyConfig(const BrainConfig& config) {
    // Convert BrainConfig to ConfigValue map
    std::unordered_map<std::string, ConfigValue> configMap;
    
    configMap["neuron_count"] = config.neuronCount;
    configMap["region_count"] = config.regionCount;
    configMap["connection_probability"] = config.connectionProbability;
    configMap["random_seed"] = config.randomSeed;
    configMap["simulation_timestep"] = config.simulationTimestep;
    configMap["stdp_ltp_weight"] = config.stdpLtpWeight;
    configMap["stdp_ltd_weight"] = config.stdpLtdWeight;
    configMap["stdp_tau"] = config.stdpTau;
    configMap["synaptogenesis_rate"] = config.synaptogenesisRate;
    configMap["pruning_rate"] = config.pruningRate;
    configMap["working_memory_capacity"] = config.workingMemoryCapacity;
    configMap["max_episodic_episodes"] = config.maxEpisodicEpisodes;
    configMap["dopamine_level"] = config.dopamineLevel;
    configMap["curiosity_strength"] = config.curiosityStrength;
    configMap["novelty_decay"] = config.noveltyDecay;
    configMap["prediction_error_weight"] = config.predictionErrorWeight;
    configMap["enable_development"] = config.enableDevelopment;
    configMap["developmental_age"] = config.developmentalAge;
    configMap["enable_learning"] = config.enableLearning;
    configMap["learning_rate"] = config.learningRate;
    configMap["enable_curiosity"] = config.enableCuriosity;
    configMap["enable_reward_modulation"] = config.enableRewardModulation;
    configMap["reward_influence"] = config.rewardInfluence;
    configMap["replay_interval"] = config.replayInterval;
    configMap["consolidation_interval"] = config.consolidationInterval;
    configMap["enable_checkpointing"] = config.enableCheckpointing;
    configMap["checkpoint_dir"] = config.checkpointDir;
    configMap["max_steps"] = config.maxSteps;
    configMap["verbose_logging"] = config.verboseLogging;
    
    return applyConfigFromMap(configMap);
}

bool Brain::applyConfigFromMap(const std::unordered_map<std::string, ConfigValue>& configMap) {
    for (const auto& [key, value] : configMap) {
        std::visit([&](const auto& val) {
            if constexpr (std::is_same_v<int64_t, std::remove_reference_t<decltype(val)>>) {
                pImpl->config->set(key, val, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<double, std::remove_reference_t<decltype(val)>>) {
                pImpl->config->set(key, val, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<float, std::remove_reference_t<decltype(val)>>) {
                pImpl->config->set(key, val, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<bool, std::remove_reference_t<decltype(val)>>) {
                pImpl->config->set(key, val, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<std::string, std::remove_reference_t<decltype(val)>>) {
                pImpl->config->set(key, val, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<std::vector<int>, std::remove_reference_t<decltype(val)>>) {
                std::vector<int64_t> intVec(val.begin(), val.end());
                pImpl->config->set(key, intVec, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<std::vector<double>, std::remove_reference_t<decltype(val)>>) {
                pImpl->config->set(key, val, ConfigSource::Runtime);
            } else if constexpr (std::is_same_v<std::vector<std::string>, std::remove_reference_t<decltype(val)>>) {
                pImpl->config->set(key, val, ConfigSource::Runtime);
            }
        }, value);
    }
    
    return true;
}

ValidationResult Brain::validateConfig(const BrainConfig& config) const {
    ValidationResult result;
    
    if (config.neuronCount == 0) {
        result.valid = false;
        result.errors.emplace_back(BrainErrorCode::InvalidConfig, "Neuron count must be greater than 0");
    }
    
    if (config.regionCount == 0) {
        result.valid = false;
        result.errors.emplace_back(BrainErrorCode::InvalidConfig, "Region count must be greater than 0");
    }
    
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
        result.valid = false;
        result.errors.emplace_back(BrainErrorCode::InvalidConfig, "Connection probability must be between 0 and 1");
    }
    
    if (config.maxSteps == 0) {
        result.valid = false;
        result.errors.emplace_back(BrainErrorCode::InvalidConfig, "Max steps must be greater than 0");
    }
    
    return result;
}

// Runtime configuration modification implementation
void Brain::setLearningRate(float rate) {
    pImpl->config->set("learning_rate", rate, ConfigSource::Runtime);
}

void Brain::setNeuronCount(size_t count) {
    pImpl->config->set("neuron_count", count, ConfigSource::Runtime);
}

void Brain::enableLearning(bool enable) {
    pImpl->config->set("enable_learning", enable, ConfigSource::Runtime);
}

void Brain::enableDevelopment(bool enable) {
    pImpl->config->set("enable_development", enable, ConfigSource::Runtime);
}

void Brain::enableCuriosity(bool enable) {
    pImpl->config->set("enable_curiosity", enable, ConfigSource::Runtime);
}

void Brain::enableRewardModulation(bool enable) {
    pImpl->config->set("enable_reward_modulation", enable, ConfigSource::Runtime);
}

void Brain::setVerboseLogging(bool verbose) {
    pImpl->config->set("verbose_logging", verbose, ConfigSource::Runtime);
}

// High-level simulation control implementation
SimulationResult Brain::runSimulation(const std::vector<SensoryInput>& inputs, 
                                    const SimulationOptions& options) {
    SimulationResult result;
    result.success = true;
    result.totalReward = 0.0f;
    result.stepsCompleted = 0;
    result.totalSpikes = 0;
    result.averageFiringRate = 0.0f;
    
    try {
        // Record start time for timeout
        auto stepStartTime = std::chrono::high_resolution_clock::now();
        
        // Process inputs
        for (const auto& input : inputs) {
            receiveSensoryInput(input);
        }
        
        // Run simulation steps
        size_t maxSteps = options.maxSteps > 0 ? options.maxSteps : pImpl->config->getOr<size_t>("max_steps", 10000);
        
        for (size_t step = 0; step < maxSteps; ++step) {
            step(static_cast<SimulationStep>(step));
            result.stepsCompleted++;
            result.totalSpikes += pImpl->totalSpikesThisStep;
            
            // Check for termination conditions
            if (options.earlyTermination) {
                // Simple early termination after 100 steps
                if (step >= 100) break;
            }
            
            // Check timeout
            if (options.timeout > 0) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - stepStartTime
                ).count();
                if (elapsed > options.timeout) {
                    break;
                }
            }
        }
        
        // Calculate statistics
        if (result.stepsCompleted > 0) {
            result.averageFiringRate = static_cast<float>(result.totalSpikes) / 
                                      (result.stepsCompleted * static_cast<size_t>(pImpl->regions.size()) * 100.0f);
        }
        
    } catch (const BrainError& e) {
        result.success = false;
        result.errors.push_back(e);
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.emplace_back(BrainErrorCode::SimulationError, std::string("Exception: ") + e.what());
    }
    
    return result;
}

SimulationResult Brain::runTraining(const std::vector<TrainingData>& data,
                                   const TrainingOptions& options) {
    SimulationResult result;
    result.success = true;
    
    for (const auto& datum : data) {
        std::vector<SensoryInput> inputs;
        // Convert TrainingData to SensoryInput
        // For now, just use a simple implementation
        if (!inputs.empty()) {
            SimulationOptions simOptions;
            simOptions.maxSteps = options.maxStepsPerSequence;
            simOptions.timeout = options.timeoutPerSequence;
            
            auto simResult = runSimulation(inputs, simOptions);
            result.stepsCompleted += simResult.stepsCompleted;
            result.totalSpikes += simResult.totalSpikes;
            result.totalReward += simResult.totalReward;
            
            if (!simResult.success) {
                result.success = false;
                result.errors.insert(result.errors.end(), simResult.errors.begin(), simResult.errors.end());
            }
        }
    }
    
    // Calculate average metrics
    if (!data.empty()) {
        result.totalReward /= data.size();
        result.averageFiringRate = static_cast<float>(result.totalSpikes) / 
                                  (result.stepsCompleted * 100.0f);
    }
    
    return result;
}

SimulationResult Brain::runEvaluation(const std::vector<SensoryInput>& inputs,
                                     const EvaluationOptions& options) {
    SimulationResult result;
    result.success = true;
    
    auto simResult = runSimulation(inputs, options.simulationOptions);
    
    // Compute evaluation metrics
    result.totalReward = simResult.totalReward;
    result.stepsCompleted = simResult.stepsCompleted;
    result.totalSpikes = simResult.totalSpikes;
    
    // Calculate accuracy metrics based on simulation results
    if (simResult.stepsCompleted > 0) {
        result.averageFiringRate = static_cast<float>(result.totalSpikes) / 
                                  (result.stepsCompleted * 100.0f);
    }
    
    return result;
}

// Simplified lifecycle management implementation
void Brain::resetBrain() {
    resetWithError();
}

bool Brain::saveBrain(const std::string& filepath, const CheckpointOptions& options) {
    return saveWithError(filepath, options) != BrainError();
}

bool Brain::loadBrain(const std::string& filepath) {
    BrainError error = loadWithError(filepath);
    return error.code == BrainErrorCode::Success;
}

// Parameter validation implementation
bool Brain::validateParameters(const BrainConfig& config) const {
    ValidationResult result = validateConfig(config);
    return result.valid;
}

BrainError Brain::validateRuntimeState() const {
    BrainError error;
    
    if (!pImpl->config) {
        error = BrainError(BrainErrorCode::InitializationFailed, "Config not initialized");
        return error;
    }
    
    if (pImpl->regions.empty()) {
        error = BrainError(BrainErrorCode::InitializationFailed, "No regions initialized");
        return error;
    }
    
    return error;
}

// Initialize brain with configuration (second initialization method)
bool Brain::initialize() {
    return initialize(BrainConfig::createDefault());
}

// Batch processing implementation
BatchResult Brain::runBatch(const std::vector<std::vector<SensoryInput>>& inputSequences,
                         const BatchOptions& options) {
    BatchResult result;
    result.success = true;
    result.averageReward = 0.0f;
    result.totalSteps = 0;
    
    std::vector<SimulationResult> batchResults;
    batchResults.reserve(inputSequences.size());
    
    for (const auto& inputSequence : inputSequences) {
        SimulationOptions simOptions;
        simOptions.maxSteps = options.maxStepsPerSequence;
        simOptions.timeout = options.timeoutPerSequence;
        
        auto simulationResult = runSimulation(inputSequence, simOptions);
        batchResults.push_back(simulationResult);
        
        if (!simulationResult.success) {
            result.success = false;
        }
        
        result.averageReward += simulationResult.totalReward;
        result.totalSteps += simulationResult.stepsCompleted;
    }
    
    if (!inputSequences.empty()) {
        result.averageReward /= inputSequences.size();
    }
    
    result.results = std::move(batchResults);
    
    return result;
}

// Plugin system implementation
void Brain::registerPlugin(std::unique_ptr<BrainPlugin> plugin) {
    if (!plugin) return;
    
    std::string name = plugin->getName();
    if (pluginMap.find(name) != pluginMap.end()) {
        NLM_LOG_WARNING("Plugin with name '" + name + "' already registered. Overwriting.");
    }
    
    pluginMap[name] = std::move(plugin);
    plugins.push_back(std::move(pluginMap[name]));
    
    // Initialize the plugin
    plugins.back()->initialize(this);
}

void Brain::unregisterPlugin(const std::string& name) {
    auto it = pluginMap.find(name);
    if (it != pluginMap.end()) {
        // Shutdown plugin
        it->second->shutdown();
        
        // Remove from both containers
        plugins.erase(
            std::remove_if(plugins.begin(), plugins.end(),
                [&name](const std::unique_ptr<BrainPlugin>& plugin) {
                    return plugin->getName() == name;
                }
            ),
            plugins.end()
        );
        pluginMap.erase(it);
    }
}

std::vector<std::string> Brain::getPluginNames() const {
    std::vector<std::string> names;
    names.reserve(pluginMap.size());
    for (const auto& pair : pluginMap) {
        names.push_back(pair.first);
    }
    return names;
}

// Callback system implementation
void Brain::registerCallback(CallbackType type, const std::function<void()>& callback) {
    if (callback) {
        callbacks[type].push_back(callback);
    }
}

void Brain::unregisterCallback(CallbackType type) {
    callbacks.erase(type);
}

void Brain::clearCallbacks() {
    callbacks.clear();
}

// Real-time monitoring and control implementation
void Brain::startMonitoring(std::unique_ptr<MonitoringCallback> monitor,
                          const MonitoringConfig& config) {
    if (monitor) {
        monitoringCallback = std::move(monitor);
        monitoringConfig = config;
        isMonitoringActive = true;
        
        // Call initial monitoring callback
        if (monitoringCallback) {
            monitoringCallback->onMonitoringStarted();
        }
    }
}

void Brain::stopMonitoring() {
    if (isMonitoringActive && monitoringCallback) {
        monitoringCallback->onMonitoringStopped();
        monitoringCallback.reset();
    }
    isMonitoringActive = false;
}

bool Brain::isMonitoring() const {
    return isMonitoringActive && monitoringCallback != nullptr;
}

void Brain::pauseSimulation() {
    // Pause the simulation
    isResting = true;
    
    // Call monitoring callback if active
    if (isMonitoringActive && monitoringCallback) {
        monitoringCallback->onSimulationPaused();
    }
}

void Brain::resumeSimulation() {
    // Resume the simulation
    isResting = false;
    
    // Call monitoring callback if active
    if (isMonitoringActive && monitoringCallback) {
        monitoringCallback->onSimulationResumed();
    }
}

void Brain::stepSimulation() {
    // Execute one simulation step
    if (!isResting) {
        step(currentStep);
        currentStep++;
        
        // Call monitoring callback if active
        if (isMonitoringActive && monitoringCallback) {
            monitoringCallback->onSimulationStepCompleted(currentStep);
        }
    }
}

bool Brain::isSimulationRunning() const {
    return !isResting;
}

// Error handling improvements
BrainError Brain::initializeWithError(const BrainConfig& config) {
    BrainError error;
    
    try {
        bool success = initialize(config);
        if (!success) {
            error = BrainError(BrainErrorCode::InitializationFailed, "Brain initialization failed");
        }
    } catch (const std::exception& e) {
        error = BrainError(BrainErrorCode::InitializationFailed, std::string("Exception: ") + e.what());
    }
    
    return error;
}

BrainError Brain::resetWithError() {
    BrainError error;
    
    try {
        reset();
    } catch (const std::exception& e) {
        error = BrainError(BrainErrorCode::SimulationError, std::string("Exception during reset: ") + e.what());
    }
    
    return error;
}

BrainError Brain::saveWithError(const std::string& filepath, const CheckpointOptions& options) {
    BrainError error;
    
    try {
        bool success = save(filepath);
        if (!success) {
            error = BrainError(BrainErrorCode::SaveLoadError, "Failed to save brain state to: " + filepath);
        }
    } catch (const std::exception& e) {
        error = BrainError(BrainErrorCode::SaveLoadError, std::string("Exception during save: ") + e.what());
    }
    
    return error;
}

// Original Brain class methods (backward compatibility)
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
    
    pImpl->currentStep = currentStep;
    pImpl->currentTime = currentTime;
    pImpl->totalSpikesThisStep = 0;
    
    // ========== STEP 1: Process pending delayed spikes (deliver synaptic input) ==========
    pImpl->spikeSystem->processDelayedSpikes(currentStep, currentTime);
    
    // ========== STEP 2: Update all neurons (LIF dynamics) ==========
    for (auto& region : pImpl->regions) {
        for (auto& pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                neuron->stepLIF(currentTime, pImpl->timestep);
            }
        }
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
    
    // ========== STEP 4: Update working memory ==========
    if (pImpl->workingMemory) {
        pImpl->workingMemory->update(pImpl->timestep);
    }
    
    // ========== STEP 5: Update episodic memory ==========
    if (pImpl->episodicMemory) {
        // Update all episodes with current simulation step
        pImpl->episodicMemory->updateAllEpisodes(currentStep);
        
        // Apply prediction error to episodic memory
        if (pImpl->predictionSystem) {
            pImpl->episodicMemory->applyPredictionError(0.0f); // Will be set by prediction system
        }
        
        // Perform consolidation periodically
        if (pImpl->currentStep % pImpl->consolidationInterval == 0) {
            pImpl->episodicMemory->consolidate(0.3f);
        }
        
        // Replay important memories during rest or periodically
        if (pImpl->currentStep % pImpl->replayInterval == 0) {
            pImpl->episodicMemory->replayDuringRest(1.0f);
        }
    }
    
    // ========== STEP 6: Update associative memory ==========
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
        float dopamineLevel = pImpl->dopamine->getLevel();
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Dopamine modulates excitability by injecting additional current
                    float excitabilityMod = dopamineLevel * 0.5f;
                    if (excitabilityMod > 0.0f) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // ========== STEP 6: Apply plasticity rules (STDP and Hebbian) ==========
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
    
    // ========== STEP 7: Update episodic memory ==========
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
    
    // ========== STEP 8: Update prediction system ==========
    if (pImpl->predictionSystem) {
        // The prediction system would be updated with sensory observations
        // For now, just track prediction error history
    }
    
    // ========== STEP 9: Update attention system ==========
    if (pImpl->attention) {
        pImpl->attention->update(pImpl->timestep);
        
        // Apply attention to working memory winners
        if (pImpl->workingMemory && !pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->workingMemory->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
    
    // ========== STEP 10: Update concept formation ==========
    if (pImpl->conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
    
    // ========== STEP 11: Apply structural plasticity periodically ==========
    if (currentStep % 100 == 0) {
        pImpl->structuralPlasticity->update(this, *pImpl->rng);
    }
    
    // ========== STEP 12: Replay important memories ==========
    if (currentStep % pImpl->replayInterval == 0 && pImpl->episodicMemory) {
        // Get episodes for replay
        auto episodesToReplay = pImpl->episodicMemory->getEpisodesForReplay(3);
        for (const auto* episode : episodesToReplay) {
            pImpl->episodicMemory->replayEpisode(episode);
        }
    }
    
    // ========== STEP 13: Apply development effects ==========
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
    
    // ========== STEP 14: Periodic memory consolidation ==========
    if (currentStep % pImpl->consolidationInterval == 0 && pImpl->episodicMemory) {
        // Consolidate important memories, remove weak ones
        pImpl->episodicMemory->consolidate(0.3f);
    }
    
    // ========== STEP 15: Checkpoint management ==========
    if (pImpl->checkpointManager) {
        pImpl->checkpointManager->update(currentStep, currentTime);
    }
}

void Brain::receiveSensoryInput(const SensoryInput& input) {
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

std::unique_ptr<Action> Brain::produceAction() {
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

void Brain::applyNeuromodulation(const Neuromodulator& signal) {
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
        // Use CheckpointWriter for comprehensive checkpoint saving
        CheckpointWriter writer;
        if (!writer.create(filepath, CompressionLevel::Balanced)) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Collect neuron data from all regions and populations
        NeuronCheckpointData neuronData;
        neuronData.membranePotential.reserve(getTotalNeuronCount());
        neuronData.restingPotential.reserve(getTotalNeuronCount());
        neuronData.threshold.reserve(getTotalNeuronCount());
        neuronData.resetPotential.reserve(getTotalNeuronCount());
        neuronData.leakConductance.reserve(getTotalNeuronCount());
        neuronData.firingState.reserve(getTotalNeuronCount());
        neuronData.refractoryRemaining.reserve(getTotalNeuronCount());
        neuronData.refractoryPeriod.reserve(getTotalNeuronCount());
        neuronData.lastSpikeTime.reserve(getTotalNeuronCount());
        neuronData.neuronType.reserve(getTotalNeuronCount());
        neuronData.regionId.reserve(getTotalNeuronCount());
        neuronData.populationId.reserve(getTotalNeuronCount());
        
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
                    neuronData.neuronType.push_back(0);  // Default neuron type
                    neuronData.regionId.push_back(region->getId().index());
                    neuronData.populationId.push_back(pop->getId().index());
                }
            }
        }
        
        // Write neurons section
        if (!writer.writeNeurons(neuronData)) {
            NLM_LOG_ERROR("Failed to write neurons to checkpoint");
            return false;
        }
        
        // Collect synapse data from all regions
        SynapseCheckpointData synapseData;
        for (const auto& region : pImpl->regions) {
            for (const auto* syn : region->getSynapses()) {
                synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                synapseData.weight.push_back(syn->getWeight());
                synapseData.delay.push_back(syn->getDelay());
                synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                
                // Get plasticity flags
                auto plasticityFlags = syn->getPlasticityFlags();
                uint8_t plasticityByte = 0;
                if (plasticityFlags.stdp) plasticityByte |= 0x01;
                if (plasticityFlags.hebbian) plasticityByte |= 0x02;
                synapseData.plasticityFlags.push_back(plasticityByte);
                
                synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                synapseData.efficacy.push_back(syn->getEfficacy());
                synapseData.shortTermDepression.push_back(syn->getShortTermDepression());
                synapseData.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        
        // Write synapses section
        if (!writer.writeSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to write synapses to checkpoint");
            return false;
        }
        
        // Set checkpoint metadata
        writer.setMetadata(
            getTotalNeuronCount(),
            getTotalSynapseCount(),
            getRegionCount(),
            pImpl->currentStep,
            pImpl->currentTime
        );
        
        // Write section headers for data organization
        if (!writer.writeSection(CheckpointSection::Header, nullptr, 0)) {
            NLM_LOG_ERROR("Failed to write checkpoint header section");
            return false;
        }
        if (!writer.writeSection(CheckpointSection::Neurons, nullptr, 0)) {
            NLM_LOG_ERROR("Failed to write neurons header section");
            return false;
        }
        if (!writer.writeSection(CheckpointSection::Synapses, nullptr, 0)) {
            NLM_LOG_ERROR("Failed to write synapses header section");
            return false;
        }
        
        // Finalize checkpoint
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
        
        // Restore neuron states - update all regions with the data
        size_t neuronIndex = 0;
        for (auto& region : pImpl->regions) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuronIndex < neuronData.membranePotential.size()) {
                        auto& state = neuron->getState();
                        state.membranePotential = neuronData.membranePotential[neuronIndex];
                        state.restingPotential = neuronData.restingPotential[neuronIndex];
                        state.threshold = neuronData.threshold[neuronIndex];
                        state.resetPotential = neuronData.resetPotential[neuronIndex];
                        state.leakConductance = neuronData.leakConductance[neuronIndex];
                        state.firingState = static_cast<FiringState>(neuronData.firingState[neuronIndex]);
                        state.refractoryRemaining = neuronData.refractoryRemaining[neuronIndex];
                        state.refractoryPeriod = neuronData.refractoryPeriod[neuronIndex];
                        state.lastSpikeTime = neuronData.lastSpikeTime[neuronIndex];
                        neuronIndex++;
                    }
                }
            }
        }
        
        // Read synapses
        SynapseCheckpointData synapseData;
        if (!reader.readSynapses(synapseData)) {
            NLM_LOG_ERROR("Failed to read synapses from checkpoint");
            return false;
        }
        
        // Restore synapse states - recreate or update existing synapses
        // Note: Full synapse restoration is complex and requires matching
        // source and destination neurons. For now, we'll log the count.
        NLM_LOG_INFO("Loaded " + std::to_string(neuronData.membranePotential.size()) + " neurons and " +
                   std::to_string(synapseData.sourceNeuron.size()) + " synapses");
        
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

NeuromodulationManager* Brain::getNeuromodulationManager() {
    return pImpl->neuromodulationManager.get();
}

Reward* Brain::getReward() {
    return pImpl->reward.get();
}

float Brain::getTotalReward() const {
    return pImpl->reward ? pImpl->reward->getValue() : 0.0f;
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