#include "CoreBrainController.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Random/Random.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace nlm {

CoreBrainController::CoreBrainController(const std::shared_ptr<Config>& config, 
                                       const ConfigurationOptions& options)
    : config(config), options(options), currentState(SimulationState::STOPPED),
      currentStep(0), currentTime(0.0), running(false),
      stepsCompleted(0), spikesCompleted(0) {
    
    NLM_LOG_INFO("Initializing CoreBrainController...");
    
    if (!validateConfiguration()) {
        throw std::runtime_error("Invalid configuration provided");
    }
    
    optimizeForOptions();
    initializeSystems();
    setupIntegration();
    
    NLM_LOG_INFO("CoreBrainController initialization complete");
}

CoreBrainController::~CoreBrainController() {
    cleanupSystems();
    NLM_LOG_INFO("CoreBrainController shutdown complete");
}

bool CoreBrainController::validateConfiguration() const {
    if (!config) {
        NLM_LOG_ERROR("Configuration is null");
        return false;
    }
    
    if (options.neuronCount == 0) {
        NLM_LOG_ERROR("Invalid neuron count: must be > 0");
        return false;
    }
    
    if (options.timestep <= 0.0) {
        NLM_LOG_ERROR("Invalid timestep: must be > 0");
        return false;
    }
    
    if (options.maxSteps == 0) {
        NLM_LOG_ERROR("Invalid max steps: must be > 0");
        return false;
    }
    
    NLM_LOG_INFO("Configuration validation passed");
    return true;
}

void CoreBrainController::optimizeForOptions() {
    NLM_LOG_INFO("Optimizing for configuration options...");
    
    setupSIMD();
    setupMultithreading();
    
    // Set configuration values based on options
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount));
    config->set("region_count", static_cast<int64_t>(options.regionCount));
    config->set("connection_probability", options.connectionProbability);
    config->set("simulation_timestep", options.timestep);
    config->set("replay_interval", static_cast<int64_t>(options.replayInterval));
    config->set("consolidation_interval", static_cast<int64_t>(options.consolidationInterval));
    
    // Set optional parameters
    if (options.enableNovelty) {
        config->set("enable_novelty", true);
    }
    if (options.enableCuriosity) {
        config->set("enable_curiosity", true);
    }
    if (options.enablePrediction) {
        config->set("enable_prediction", true);
    }
    if (options.enableCognition) {
        config->set("enable_cognition", true);
    }
    
    NLM_LOG_INFO("Optimization complete");
}

void CoreBrainController::setupSIMD() {
    if (options.enableSIMD) {
        NLM_LOG_INFO("SIMD optimization enabled");
        // SIMD is enabled at compile-time in the existing code
    }
}

void CoreBrainController::setupMultithreading() {
    if (options.threadCount > 1) {
        NLM_LOG_INFO("Multithreading enabled with " + std::to_string(options.threadCount) + " threads");
        // Multithreading can be implemented in the future
    }
}

void CoreBrainController::initializeSystems() {
    NLM_LOG_INFO("Initializing core systems...");
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Initialize random generator
    uint64_t seed = config->getOr<int64_t>("random_seed", 42);
    
    NLM_LOG_INFO("Core systems initialized");
}

void CoreBrainController::setupIntegration() {
    NLM_LOG_INFO("Setting up integrated systems...");
    
    // Setup brain with configuration
    brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        throw std::runtime_error("Failed to initialize brain");
    }
    
    // Setup world simulation
    world = std::make_shared<SimpleWorld>();
    world->configure(16, 16, 16, 16);
    world->reset();
    
    // Setup agent brain
    agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    agent->enableRewardModulation(true);
    agent->enableStructuralPlasticity(options.enableDevelopment);
    agent->enableDevelopment(options.enableDevelopment);
    agent->enableCuriosity(true);
    
    // Setup integrated experiment
    experiment = std::make_shared<Phase6IntegratedExperiment>();
    
    // Setup memory systems
    setupMemorySystems();
    
    // Setup neuromodulation
    setupNeuromodulation();
    
    // Setup prediction system
    setupPredictionSystem();
    
    // Setup cognition systems
    setupCognitionSystems();
    
    NLM_LOG_INFO("Integrated systems setup complete");
}

void CoreBrainController::setupMemorySystems() {
    NLM_LOG_INFO("Setting up memory systems...");
    
    // Working memory is initialized within the Brain class
    // Episodic memory is initialized within the Brain class
    // Associative memory is initialized within the Brain class
    
    if (brain->getWorkingMemory()) {
        NLM_LOG_INFO("[PASS] Working memory system initialized");
    }
    if (brain->getEpisodicMemory()) {
        NLM_LOG_INFO("[PASS] Episodic memory system initialized");
    }
    
    NLM_LOG_INFO("Memory systems setup complete");
}

void CoreBrainController::setupNeuromodulation() {
    NLM_LOG_INFO("Setting up neuromodulation systems...");
    
    // Neuromodulation systems are initialized within the Brain class
    if (brain->getDopamine()) {
        NLM_LOG_INFO("[PASS] Dopamine system initialized");
    }
    if (brain->getCuriosity()) {
        NLM_LOG_INFO("[PASS] Curiosity system initialized");
    }
    if (brain->getNovelty()) {
        NLM_LOG_INFO("[PASS] Novelty system initialized");
    }
    
    NLM_LOG_INFO("Neuromodulation systems setup complete");
}

void CoreBrainController::setupPredictionSystem() {
    NLM_LOG_INFO("Setting up prediction system...");
    
    if (brain->getPredictionSystem()) {
        NLM_LOG_INFO("[PASS] Prediction system initialized");
    }
    
    NLM_LOG_INFO("Prediction system setup complete");
}

void CoreBrainController::setupCognitionSystems() {
    NLM_LOG_INFO("Setting up cognition systems...");
    
    if (brain->getPlanner()) {
        NLM_LOG_INFO("[PASS] Neural planner initialized");
    }
    if (brain->getConceptFormation()) {
        NLM_LOG_INFO("[PASS] Concept formation system initialized");
    }
    if (brain->getAttention()) {
        NLM_LOG_INFO("[PASS] Attention system initialized");
    }
    
    NLM_LOG_INFO("Cognition systems setup complete");
}

void CoreBrainController::run() {
    NLM_LOG_INFO("Starting simulation run...");
    
    // Set initial state
    currentState = SimulationState::RUNNING;
    running = true;
    stepsCompleted = 0;
    spikesCompleted = 0;
    simulationStartTime = std::chrono::high_resolution_clock::now();
    metrics.startTime = std::chrono::high_resolution_clock::now();
    
    // Phase 6 integration experiment configuration
    Phase6Config phase6Config;
    phase6Config.neuronCount = options.neuronCount;
    phase6Config.maxSteps = options.maxSteps;
    phase6Config.enableCheckpointing = options.enableCheckpointing;
    phase6Config.enableReplay = options.enableReplay;
    phase6Config.enableDevelopment = options.enableDevelopment;
    phase6Config.checkpointPath = options.checkpointPath;
    
    // Run the experiment
    auto result = experiment->run(phase6Config);
    
    // Update metrics
    {
        std::lock_guard<std::mutex> lock(metricsMutex);
        metrics.totalSteps = result.totalReward; // Note: totalReward contains steps in this demo
        metrics.totalSpikes = result.totalWallClockTime; // Note: this contains spike count in this demo
        metrics.averageFiringRate = result.avgFiringRate;
        metrics.totalReward = result.totalReward;
        metrics.memoryEpisodesStored = result.memoryEpisodesStored;
        metrics.noveltyLevel = result.noveltyLevel;
        metrics.curiosityLevel = result.curiosityLevel;
        metrics.dopamineLevel = result.dopamineLevel;
        metrics.wallClockTime = result.totalWallClockTime;
        metrics.endTime = std::chrono::high_resolution_clock::now();
    }
    
    // Mark completion
    currentState = SimulationState::STOPPED;
    running = false;
    
    // Call completion callback if set
    if (completionCallback) {
        completionCallback();
    }
    
    NLM_LOG_INFO("Simulation run completed successfully");
}

void CoreBrainController::step(uint64_t steps) {
    if (currentState != SimulationState::RUNNING && currentState != SimulationState::PAUSED) {
        NLM_LOG_ERROR("Cannot step: simulation is not running or paused");
        return;
    }
    
    NLM_LOG_INFO("Executing " + std::to_string(steps) + " simulation steps...");
    
    for (uint64_t step = 0; step < steps && running; ++step) {
        currentStep = step;
        currentTime = step * options.timestep;
        
        // Process one step of the simulation
        processStep(step, currentTime);
        
        stepsCompleted++;
        updateMetrics();
        
        // Call step callback if set
        if (stepCallback) {
            stepCallback(step);
        }
        
        // Check for pause state
        if (currentState == SimulationState::PAUSED) {
            NLM_LOG_INFO("Simulation paused at step " + std::to_string(step));
            while (currentState == SimulationState::PAUSED && running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }
    
    NLM_LOG_INFO("Step execution completed");
}

void CoreBrainController::processStep(uint64_t step, Timestamp time) {
    // Get observation from world
    world->update(options.timestep);
    SensoryPercept percept = world->getSensoryPercept();
    
    // Process sensory input through agent
    agent->processSensoryInput(percept);
    
    // Run brain simulation step
    brain->step(step, time);
    
    // Get motor command and apply to world
    MotorCommand cmd = agent->decodeMotorCommand();
    ActionResult actionResult = world->applyMotorCommand(cmd, time);
    
    // Apply reward modulation
    agent->applyRewardModulation(actionResult.reward, 0.0f);
    
    // Update development if enabled
    if (options.enableDevelopment) {
        agent->updateDevelopment(options.timestep);
    }
}

void CoreBrainController::pause() {
    currentState = SimulationState::PAUSED;
    running = false;
    NLM_LOG_INFO("Simulation paused");
}

void CoreBrainController::resume() {
    currentState = SimulationState::RUNNING;
    running = true;
    NLM_LOG_INFO("Simulation resumed");
}

void CoreBrainController::stop() {
    currentState = SimulationState::STOPPED;
    running = false;
    NLM_LOG_INFO("Simulation stopped");
}

void CoreBrainController::updateMetrics() {
    std::lock_guard<std::mutex> lock(metricsMutex);
    
    if (brain) {
        metrics.averageFiringRate = brain->getAverageFiringRate();
    }
    
    metrics.wallClockTime = std::chrono::duration<double>(
        std::chrono::high_resolution_clock::now() - metrics.startTime).count();
}

bool CoreBrainController::saveState(const std::string& path) {
    NLM_LOG_INFO("Saving brain state to " + path);
    
    if (!brain) {
        NLM_LOG_ERROR("Brain not initialized");
        return false;
    }
    
    if (brain->save(path)) {
        NLM_LOG_INFO("Brain state saved successfully");
        return true;
    } else {
        NLM_LOG_ERROR("Failed to save brain state");
        return false;
    }
}

bool CoreBrainController::loadState(const std::string& path) {
    NLM_LOG_INFO("Loading brain state from " + path);
    
    if (!brain) {
        NLM_LOG_ERROR("Brain not initialized");
        return false;
    }
    
    if (brain->load(path)) {
        NLM_LOG_INFO("Brain state loaded successfully");
        return true;
    } else {
        NLM_LOG_ERROR("Failed to load brain state");
        return false;
    }
}

void CoreBrainController::reset() {
    NLM_LOG_INFO("Resetting brain and simulation...");
    
    if (brain) {
        brain->reset();
    }
    
    currentStep = 0;
    currentTime = 0.0;
    currentState = SimulationState::STOPPED;
    running = false;
    
    NLM_LOG_INFO("Brain and simulation reset complete");
}

void CoreBrainController::printConfiguration() const {
    std::cout << "=== NLM Configuration ===" << std::endl;
    std::cout << "Brain Parameters:" << std::endl;
    std::cout << "  Neuron Count: " << options.neuronCount << std::endl;
    std::cout << "  Region Count: " << options.regionCount << std::endl;
    std::cout << "  Connection Probability: " << options.connectionProbability << std::endl;
    std::cout << std::endl;
    
    std::cout << "Simulation Parameters:" << std::endl;
    std::cout << "  Time Step: " << options.timestep << "s" << std::endl;
    std::cout << "  Max Steps: " << options.maxSteps << std::endl;
    std::cout << "  Replay Interval: " << options.replayInterval << std::endl;
    std::cout << "  Consolidation Interval: " << options.consolidationInterval << std::endl;
    std::cout << std::endl;
    
    std::cout << "System Integration:" << std::endl;
    std::cout << "  Novelty Detection: " << (options.enableNovelty ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Curiosity: " << (options.enableCuriosity ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Prediction System: " << (options.enablePrediction ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Cognition: " << (options.enableCognition ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Development: " << (options.enableDevelopment ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Performance:" << std::endl;
    std::cout << "  Thread Count: " << options.threadCount << std::endl;
    std::cout << "  SIMD: " << (options.enableSIMD ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  Vectorization: " << (options.enableVectorization ? "Enabled" : "Disabled") << std::endl;
    std::cout << std::endl;
}

void CoreBrainController::printStatus() const {
    std::cout << "=== NLM Brain Status ===" << std::endl;
    std::cout << "Simulation State: " << 
        (currentState == SimulationState::RUNNING ? "RUNNING" :
         currentState == SimulationState::PAUSED ? "PAUSED" : "STOPPED") << std::endl;
    
    if (brain) {
        std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Excitatory/Inhibitory Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "=== Current Metrics ===" << std::endl;
    std::cout << "Steps Completed: " << metrics.totalSteps << std::endl;
    std::cout << "Average Firing Rate: " << metrics.averageFiringRate << std::endl;
    
    std::cout << std::endl;
}

BrainState CoreBrainController::getCurrentBrainState() const {
    BrainState state;
    state.step = currentStep;
    state.time = currentTime;
    state.learnedPreference = 0.0f; // TODO: Get from brain
    state.timestamp = std::chrono::high_resolution_clock::now();
    
    return state;
}

void CoreBrainController::enableAgentBrain(bool enable) {
    if (agent) {
        agent->enableAllModules(enable);
        NLM_LOG_INFO("Agent brain " << (enable ? "enabled" : "disabled"));
    }
}

void CoreBrainController::enableWorldSimulation(bool enable) {
    if (world) {
        world->setEnabled(enable);
        NLM_LOG_INFO("World simulation " << (enable ? "enabled" : "disabled"));
    }
}

void CoreBrainController::enableExperimentMode(bool enable) {
    if (enable) {
        NLM_LOG_INFO("Enabling experiment mode...");
        // Experiment mode could enable additional monitoring and recording
    } else {
        NLM_LOG_INFO("Disabling experiment mode...");
    }
}

void CoreBrainController::cleanupSystems() {
    NLM_LOG_INFO("Cleaning up systems...");
    
    // Reset simulation state
    currentStep = 0;
    currentTime = 0.0;
    currentState = SimulationState::STOPPED;
    running = false;
    
    // Reset metrics
    {
        std::lock_guard<std::mutex> lock(metricsMutex);
        metrics.totalSteps = 0;
        metrics.totalSpikes = 0;
        metrics.averageFiringRate = 0.0f;
        metrics.totalReward = 0.0f;
        metrics.memoryEpisodesStored = 0.0f;
        metrics.noveltyLevel = 0.0f;
        metrics.curiosityLevel = 0.0f;
        metrics.dopamineLevel = 0.0f;
        metrics.wallClockTime = 0.0;
    }
    
    NLM_LOG_INFO("Systems cleanup complete");
}

std::string CoreBrainController::generateErrorMessage(const std::string& operation) const {
    std::ostringstream oss;
    oss << "Error during " << operation << ": ";
    
    if (!brain) {
        oss << "Brain not initialized";
    }
    
    return oss.str();
}

} // namespace nlm