# NLM Integration Guide

## Overview

This guide provides comprehensive instructions for integrating the NLM (熙然) neural simulation framework with external systems, adding new components, and implementing complex scenarios. It covers the major integration points, provides code examples, and documents best practices.

## Chapter 1: Core System Integration

### 1.1 Basic Neural Simulation Integration

#### Quick Start Example
```cpp
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Set basic parameters
    config->set("neuron_count", 1000);
    config->set("connection_probability", 0.1);
    config->set("simulation_timestep", 0.001);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Brain initialization failed!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step);
        
        // Optional: Get output action
        auto action = brain->produceAction();
    }
    
    return 0;
}
```

#### Integration Points

1. **Configuration Integration**
   - Load from file, command line, or runtime
   - Validate parameters before brain initialization
   - Handle configuration source precedence

2. **Sensory Integration**
   ```cpp
   // Create sensory input
   auto visionInput = std::make_unique<Vision>(width, height, channels);
   // Populate with actual data
   brain->receiveSensoryInput(*visionInput);
   ```

3. **Motor Output Integration**
   ```cpp
   // Get action from brain
   auto action = brain->produceAction();
   
   // Convert to external interface
   switch (action->getType()) {
       case ActionType::MoveForward:
           // Implement forward movement
           break;
       case ActionType::TurnLeft:
           // Implement left turn
           break;
       // ... other actions
   }
   ```

### 1.2 Advanced Configuration Integration

#### Multi-Source Configuration
```cpp
class AdvancedConfigManager {
public:
    std::shared_ptr<Config> loadConfiguration() {
        auto config = std::make_shared<Config>();
        
        // 1. Load from file (lowest priority)
        if (!config->loadFromFile("configs/experiment.cfg")) {
            NLM_LOG_WARNING("Config file not found, using defaults");
        }
        
        // 2. Override with command line
        config->loadFromArgs(argc, argv);
        
        // 3. Apply runtime modifications
        applyRuntimeOverrides(config);
        
        // 4. Validate configuration
        validateConfiguration(config);
        
        return config;
    }
    
private:
    void applyRuntimeOverrides(std::shared_ptr<Config>& config) {
        // Add experiment-specific overrides
        config->set("replay_interval", 200);
        config->set("consolidation_interval", 5000);
    }
    
    void validateConfiguration(std::shared_ptr<Config>& config) {
        // Validate critical parameters
        if (!config->has("neuron_count") || 
            config->getOr<size_t>("neuron_count", 0) == 0) {
            config->set("neuron_count", 1000);
        }
        
        if (!config->has("simulation_timestep") ||
            config->getOr<double>("simulation_timestep", 0.0) <= 0.0) {
            config->set("simulation_timestep", 0.001);
        }
    }
};
```

## Chapter 2: Agent Framework Integration

### 2.1 Complete Agent Integration

#### Step-by-Step Integration
```cpp
class NLMPerformanceAgent {
public:
    NLMPerformanceAgent() {
        // Initialize components
        initializeConfiguration();
        initializeBrain();
        initializeWorld();
        initializeAgentBrain();
    }
    
    void runEpisode(int episodeNumber) {
        resetEnvironment();
        
        // Main simulation loop
        SimulationStep step = 0;
        while (!shouldTerminate(step)) {
            // 1. Get sensory input from environment
            auto percept = getSensoryPercept();
            
            // 2. Process through agent-brain
            agentBrain_->processSensoryInput(*percept);
            
            // 3. Run neural simulation step
            brain_->step(step);
            
            // 4. Get motor command
            auto action = getMotorCommand();
            
            // 5. Apply to environment
            applyAction(action, step);
            
            // 6. Record metrics
            recordEpisodeMetrics(step, action);
            
            step++;
            if (step % 100 == 0) {
                logProgress(step);
            }
        }
        
        saveEpisodeResults(episodeNumber);
    }
    
private:
    void initializeConfiguration() {
        config_ = std::make_shared<Config>();
        config_->loadFromFile("configs/agent.cfg");
        config_->loadFromArgs(argc, argv);
    }
    
    void initializeBrain() {
        brain_ = std::make_shared<Brain>(config_);
        if (!brain_->initialize()) {
            throw std::runtime_error("Brain initialization failed");
        }
    }
    
    void initializeWorld() {
        world_ = std::make_shared<SimpleWorld>();
        world_->configure(worldWidth_, worldHeight_, 
                         visionWidth_, visionHeight_);
        world_->setAgentStart(startX_, startY_);
    }
    
    void initializeAgentBrain() {
        agentBrain_ = std::make_shared<AgentBrain>(brain_);
        agentBrain_->initialize(world_);
    }
    
    std::unique_ptr<SensoryPercept> getSensoryPercept() {
        auto percept = std::make_unique<SensoryPercept>();n        
        // Get vision data
        auto vision = world_->getSensoryPercept().getVision();
        percept->setVision(vision);
        
        // Get internal signals
        auto internal = world_->getSensoryPercept().getInternal();
        percept->setInternal(internal);
        
        // Get proprioception
        auto proprioception = world_->getSensoryPercept().getProprioception();
        percept->setProprioception(proprioception);
        
        return percept;
    }
    
    std::unique_ptr<Action> getMotorCommand() {
        return brain_->produceAction();
    }
    
    void applyAction(const Action& action, SimulationStep step) {
        world_->applyMotorCommand(action, static_cast<float>(step));
    }
    
    bool shouldTerminate(SimulationStep step) {
        return step >= maxSteps_ || 
               world_->getAgentBody().health <= 0 ||
               world_->getAgentBody().energy <= 0;
    }
};
```

### 2.2 Learning Integration

#### Neuromodulation Integration
```cpp
class LearningAgent {
public:
    LearningAgent(std::shared_ptr<Brain> brain) : brain_(brain) {
        // Enable learning systems
        setupLearningSystems();
    }
    
    void processReward(float reward, float predictedReward) {
        // Apply reward-based neuromodulation
        brain_->applyNeuromodulation(
            Neuromodulator::createRewardModulator(reward, predictedReward)
        );
    }
    
    void processNovelty(const SensoryInput& input) {
        // Apply novelty detection
        auto novelty = NoveltyDetector::detect(input);
        brain_->applyNeuromodulation(novelty);
    }
    
    void processPredictionError(float error) {
        // Apply prediction error signals
        brain_->applyNeuromodulation(
            Neuromodulator::createPredictionError(error)
        );
    }
    
private:
    void setupLearningSystems() {
        // Enable plasticity
        auto* region = brain_->getRegion(RegionId(1));
        if (region) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, true);
            }
        }
        
        // Enable development
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        
        // Enable curiosity
        if (auto* curiosity = brain_->getCuriosity()) {
            curiosity->enable(true);
        }
    }
};
```

## Chapter 3: Memory System Integration

### 3.1 Working Memory Integration

#### Working Memory Usage Patterns
```cpp
class WorkingMemoryAgent {
public:
    WorkingMemoryAgent(std::shared_ptr<Brain> brain) : brain_(brain) {
        workingMemory_ = brain_->getWorkingMemory();
        episodicMemory_ = brain_->getEpisodicMemory();
    }
    
    std::vector<NeuronId> selectFromWorkingMemory(
        const std::vector<NeuronId>& competitors,
        size_t maxKeep) {
        
        if (!workingMemory_ || competitors.empty()) {
            return {};
        }
        
        // Store competitors in working memory
        for (size_t i = 0; i < std::min(maxKeep, competitors.size()); ++i) {
            workingMemory_->storeToNeuron(
                competitors[i], 
                1.0f - static_cast<float>(i) / competitors.size()
            );
        }
        
        // Update working memory
        workingMemory_->update(0.001);
        
        // Get memory traces
        return workingMemory_->getMemoryNeurons();
    }
    
    void storeEpisode(const std::string& context) {
        // Create episodic memory from current state
        EpisodicMemoryItem episode;
        episode.timestamp = currentStep_;
        episode.context = context;
        episode.reward = getCurrentReward();
        
        // Capture active neurons
        storeActiveNeurons(episode);
        
        // Store in episodic memory
        if (episodicMemory_) {
            episodicMemory_->storeEpisode(episode);
        }
    }
    
private:
    void storeActiveNeurons(EpisodicMemoryItem& episode) {
        auto* region = brain_->getRegion(RegionId(1));
        if (!region) return;
        
        for (auto* pop : region->getPopulations()) {
            for (auto* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                if (neuron->isFiring() || 
                    std::abs(state.membranePotential - state.restingPotential) > 5.0f) {
                    
                    episode.activeNeurons.push_back(neuron->getId());
                    episode.neuronActivations.push_back(
                        std::abs(state.membranePotential - state.restingPotential) / 20.0f
                    );
                }
            }
        }
    }
};
```

### 3.2 Episodic Memory Integration

#### Memory Replay Integration
```cpp
class MemoryReplayAgent {
public:
    MemoryReplayAgent(std::shared_ptr<Brain> brain) : brain_(brain) {
        episodicMemory_ = brain_->getEpisodicMemory();
        setupMemoryReplay();
    }
    
    void updateMemorySystems(SimulationStep step) {
        // Periodic memory replay
        if (step % replayInterval_ == 0 && episodicMemory_) {
            replayImportantMemories(step);
        }
        
        // Memory consolidation
        if (step % consolidationInterval_ == 0 && episodicMemory_) {
            episodicMemory_->consolidate(consolidationStrength_);
        }
        
        // Working memory maintenance
        updateWorkingMemory(step);
    }
    
private:
    void setupMemoryReplay() {
        // Configure replay parameters
        replayInterval_ = brain_->getConfig()->getOr<size_t>("replay_interval", 100);
        consolidationInterval_ = brain_->getConfig()->getOr<size_t>("consolidation_interval", 1000);
        consolidationStrength_ = brain_->getConfig()->getOr<float>("consolidation_strength", 0.3f);
        
        // Enable memory systems
        if (auto* wm = brain_->getWorkingMemory()) {
            wm->enable(true);
        }
        
        if (auto* em = brain_->getEpisodicMemory()) {
            em->enable(true);
        }
    }
    
    void replayImportantMemories(SimulationStep step) {
        if (!episodicMemory_) return;
        
        // Get episodes for replay
        auto episodesToReplay = episodicMemory_->getEpisodesForReplay(replayCount_);
        
        NLM_LOG_INFO(std::string("Replaying ") + 
                     std::to_string(episodesToReplay.size()) + " episodes");
        
        for (const auto* episode : episodesToReplay) {
            episodicMemory_->replayEpisode(episode);
        }
    }
    
    void updateWorkingMemory(SimulationStep step) {
        if (!workingMemory_) return;
        
        // Update working memory with current activity
        workingMemory_->update(brain_->getConfig()->getOr<double>("simulation_timestep", 0.001));
        
        // Clean up old traces
        if (step % workingMemoryCleanupInterval_ == 0) {
            workingMemory_->cleanOldTraces(cleanupThreshold_);
        }
    }
};
```

## Chapter 4: Prediction System Integration

### 4.1 Prediction-Based Decision Making

#### Integration Pattern
```cpp
class PredictiveAgent {
public:
    PredictiveAgent(std::shared_ptr<Brain> brain) : brain_(brain) {
        predictionSystem_ = brain_->getPredictionSystem();
        setupPredictionIntegration();
    }
    
    std::unique_ptr<Action> selectAction(
        const SensoryPercept& percept,
        const std::vector<std::unique_ptr<Action>>& candidates) {
        
        if (!predictionSystem_) {
            // Fallback to simple action selection
            return selectRandomAction(candidates);
        }
        
        // Update prediction system with current percept
        predictionSystem_->update( percept );
        
        // Generate predictions for actions
        std::vector<float> actionPredictions;
        for (const auto& action : candidates) {
            float prediction = predictActionOutcome(action, percept);
            actionPredictions.push_back(prediction);
        }
        
        // Select action with highest predicted reward
        size_t bestAction = std::max_element(
            actionPredictions.begin(), actionPredictions.end()
        ) - actionPredictions.begin();
        
        return candidates[bestAction]->clone();
    }
    
private:
    void setupPredictionIntegration() {
        // Enable prediction system
        if (predictionSystem_) {
            predictionSystem_->enable(true);
            predictionSystem_->setPredictionWindow(predictionWindow_);
            predictionSystem_->setLearningRate(predictionLearningRate_);
        }
        
        // Setup sensory preprocessing
        setupSensoryPreprocessing();
    }
    
    void setupSensoryPreprocessing() {
        // Configure sensory encoding for prediction system
        // This may involve dimensionality reduction,
        // feature extraction, or temporal filtering
    }
    
    float predictActionOutcome(
        const Action* action,
        const SensoryPercept& percept) {
        
        if (!predictionSystem_) return 0.0f;
        
        // Use prediction system to estimate action outcome
        return predictionSystem_->predictActionOutcome(action, percept);
    }
};
```

## Chapter 5: Development System Integration

### 5.1 Developmental Integration

#### Developmental Stage Management
```cpp
class DevelopmentalAgent {
public:
    DevelopmentalAgent(std::shared_ptr<Brain> brain) : brain_(brain) {
        developmentSystem_ = brain_->getDevelopmentSystem();
        setupDevelopmentalIntegration();
    }
    
    void updateDevelopment(SimulationStep step) {
        // Update developmental stage
        if (step % developmentalUpdateInterval_ == 0) {
            updateDevelopmentalStage(step);
        }
        
        // Apply development effects
        applyDevelopmentEffects(step);
        
        // Update plasticity based on development
        updatePlasticityForDevelopment(step);
    }
    
    DevelopmentalStage getCurrentDevelopmentalStage() const {
        return brain_->getDevelopmentalStage();
    }
    
    float getDevelopmentalPlasticityFactor() const {
        DevelopmentalStage stage = brain_->getDevelopmentalStage();
        
        switch (stage) {
            case DevelopmentalStage::Initial:
                return 1.0f;  // High plasticity
            case DevelopmentalStage::CriticalPeriod:
                return 0.8f;
            case DevelopmentalStage::Maturation:
                return 0.5f;
            case DevelopmentalStage::Adult:
                return 0.2f;  // Stable
            default:
                return 0.2f;
        }
    }
    
private:
    void setupDevelopmentalIntegration() {
        // Configure developmental timing
        developmentalUpdateInterval_ = brain_->getConfig()->
            getOr<size_t>("developmental_update_interval", 1000);
        
        // Enable development system
        if (developmentSystem_) {
            developmentSystem_->enable(true);
        }
        
        // Configure developmental stages
        setupDevelopmentalStages();
    }
    
    void setupDevelopmentalStages() {
        // Configure transitions between developmental stages
        // Each stage has different plasticity and learning characteristics
    }
    
    void updateDevelopmentalStage(SimulationStep step) {
        if (!developmentSystem_) return;
        
        // Calculate next developmental stage based on current stage and time
        DevelopmentalStage current = brain_->getDevelopmentalStage();
        DevelopmentalStage next = calculateNextStage(current, step);
        
        if (next != current) {
            brain_->setDevelopmentalStage(next);
            developmentSystem_->onDevelopmentalStageChange(current, next);
            
            NLM_LOG_INFO("Developmental stage changed: " +
                         std::to_string(static_cast<int>(current)) + " -> " +
                         std::to_string(static_cast<int>(next)));
        }
    }
    
    void applyDevelopmentEffects(SimulationStep step) {
        if (!developmentSystem_) return;
        
        // Apply development effects to plasticity systems
        float plasticityFactor = getDevelopmentalPlasticityFactor();
        
        // Update STDP parameters
        if (auto* stdp = brain_->getSTDP()) {
            stdp->setLearningRate(stdp->getLearningRate() * plasticityFactor);
        }
        
        // Update structural plasticity
        if (auto* structuralPlasticity = brain_->getStructuralPlasticity()) {
            structuralPlasticity->setSynaptogenesisRate(
                baseSynaptogenesisRate_ * plasticityFactor);
            structuralPlasticity->setPruningRate(
                basePruningRate_ * (2.0f - plasticityFactor));
        }
    }
};
```

## Chapter 6: Performance Optimization Integration

### 6.1 Performance Monitoring Integration

#### Performance Monitoring Integration
```cpp
class PerformanceMonitoringAgent {
public:
    PerformanceMonitoringAgent(std::shared_ptr<Brain> brain) : brain_(brain) {
        setupPerformanceMonitoring();
    }
    
    struct PerformanceMetrics {
        size_t neuronCount;
        size_t synapseCount;
        size_t activeNeurons;
        size_t firingNeurons;
        float avgFiringRate;
        float eIoratio;
        float memoryUsage;
        double simulationTime;
        SimulationStep stepsProcessed;
    };
    
    PerformanceMetrics collectMetrics() {
        PerformanceMetrics metrics;
        
        // Basic neural metrics
        metrics.neuronCount = brain_->getTotalNeuronCount();
        metrics.synapseCount = brain_->getTotalSynapseCount();
        metrics.activeNeurons = brain_->getActiveNeuronCount();
        metrics.firingNeurons = brain_->getFiringNeuronCount();
        metrics.avgFiringRate = brain_->getAverageFiringRate();
        metrics.eIoratio = brain_->getExcitationInhibitionRatio();
        
        // Memory metrics
        if (auto* wm = brain_->getWorkingMemory()) {
            metrics.memoryUsage = wm->getMemoryUsage();
        }
        
        // System metrics
        metrics.simulationTime = simulationClock_.getElapsedTime();
        metrics.stepsProcessed = simulationClock_.getCurrentStep();
        
        return metrics;
    }
    
    void logPerformanceReport(const PerformanceMetrics& metrics) {
        NLM_LOG_INFO("=== Performance Report ===");
        NLM_LOG_INFO("Neurons: " + std::to_string(metrics.neuronCount));
        NLM_LOG_INFO("Synapses: " + std::to_string(metrics.synapseCount));
        NLM_LOG_INFO("Active: " + std::to_string(metrics.activeNeurons));
        NLM_LOG_INFO("Firing: " + std::to_string(metrics.firingNeurons));
        NLM_LOG_INFO("Avg Rate: " + std::to_string(metrics.avgFiringRate) + " Hz");
        NLM_LOG_INFO("E/I Ratio: " + std::to_string(metrics.eIoratio));
        NLM_LOG_INFO("Memory Usage: " + std::to_string(metrics.memoryUsage) + " MB");
        NLM_LOG_INFO("Simulation Time: " + 
                     std::to_string(metrics.simulationTime) + " s");
        NLM_LOG_INFO("Steps Processed: " + 
                     std::to_string(metrics.stepsProcessed));
    }
    
private:
    void setupPerformanceMonitoring() {
        // Configure monitoring interval
        monitoringInterval_ = brain_->getConfig()->
            getOr<size_t>("monitoring_interval", 1000);
        
        // Initialize simulation clock
        double timestep = brain_->getConfig()->
            getOr<double>("simulation_timestep", 0.001);
        simulationClock_.setTimestep(timestep);
    }
    
    SimulationClock simulationClock_;
    size_t monitoringInterval_;
};
```

## Chapter 7: Error Handling and Recovery Integration

### 7.1 Error Handling Integration

#### Comprehensive Error Handling
```cpp
class ErrorHandlingAgent {
public:
    ErrorHandlingAgent(std::shared_ptr<Brain> brain) : brain_(brain) {
        setupErrorHandling();
    }
    
    bool safeBrainStep(SimulationStep step) {
        try {
            // Validate brain state before stepping
            if (!validateBrainState()) {
                NLM_LOG_WARNING("Invalid brain state, attempting recovery");
                if (!attemptRecovery()) {
                    NLM_LOG_ERROR("Recovery failed");
                    return false;
                }
            }
            
            // Check for performance issues
            if (detectPerformanceIssues()) {
                NLM_LOG_WARNING("Performance issues detected, adjusting parameters");
                adjustParametersForPerformance();
            }
            
            // Execute simulation step with timeout
            if (!executeStepWithTimeout(step, stepTimeout_)) {
                NLM_LOG_ERROR("Step execution timed out");
                return false;
            }
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception during brain step: ") + e.what());
            handleException(e);
            return false;
        }
    }
    
private:
    bool validateBrainState() {
        // Validate neural state
        if (brain_->getTotalNeuronCount() == 0) {
            NLM_LOG_WARNING("Brain has no neurons");
            return false;
        }
        
        // Validate connectivity
        if (brain_->getTotalSynapseCount() == 0) {
            NLM_LOG_WARNING("Brain has no synapses");
            return false;
        }
        
        // Validate memory systems
        if (auto* wm = brain_->getWorkingMemory(); wm && !wm->isValid()) {
            NLM_LOG_WARNING("Working memory is invalid");
            return false;
        }
        
        return true;
    }
    
    bool attemptRecovery() {
        // Attempt to recover from invalid state
        try {
            brain_->reset();
            
            // Re-initialize if possible
            if (!brain_->initialize()) {
                NLM_LOG_ERROR("Failed to re-initialize brain");
                return false;
            }
            
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Recovery failed: ") + e.what());
            return false;
        }
    }
    
    void setupErrorHandling() {
        // Configure error handling parameters
        stepTimeout_ = brain_->getConfig()->
            getOr<double>("step_timeout", 100.0);  // milliseconds
        
        // Setup exception handlers
        setupExceptionHandlers();
    }
};
```

## Chapter 8: Testing and Validation Integration

### 8.1 Integration Testing

#### Integration Test Framework
```cpp
class IntegrationTestFramework {
public:
    IntegrationTestFramework(std::shared_ptr<Brain> brain) : brain_(brain) {
        setupTestFramework();
    }
    
    bool runIntegrationTests() {
        bool allTestsPassed = true;
        
        // Run component integration tests
        if (!runComponentIntegrationTests()) {
            allTestsPassed = false;
        }
        
        // Run system integration tests
        if (!runSystemIntegrationTests()) {
            allTestsPassed = false;
        }
        
        // Run performance integration tests
        if (!runPerformanceIntegrationTests()) {
            allTestsPassed = false;
        }
        
        return allTestsPassed;
    }
    
private:
    bool runComponentIntegrationTests() {
        // Test integration of individual components
        bool passed = true;
        
        // Test sensory integration
        if (!testSensoryIntegration()) {
            NLM_LOG_ERROR("Sensory integration test failed");
            passed = false;
        }
        
        // Test motor integration
        if (!testMotorIntegration()) {
            NLM_LOG_ERROR("Motor integration test failed");
            passed = false;
        }
        
        // Test memory integration
        if (!testMemoryIntegration()) {
            NLM_LOG_ERROR("Memory integration test failed");
            passed = false;
        }
        
        return passed;
    }
    
    bool testSensoryIntegration() {
        // Create test sensory input
        auto testInput = createTestSensoryInput();
        
        // Process through brain
        brain_->receiveSensoryInput(*testInput);
        
        // Verify neural response
        return verifyNeuralResponse(*testInput);
    }
    
    std::unique_ptr<SensoryInput> createTestSensoryInput() {
        // Create test sensory input with known properties
        return std::make_unique<Vision>(10, 10, 3);
    }
    
    bool verifyNeuralResponse(const SensoryInput& input) {
        // Check if brain responded appropriately
        // This depends on the specific integration being tested
        return true;  // Placeholder
    }
    
    void setupTestFramework() {
        // Configure test parameters
        testTimeout_ = brain_->getConfig()->
            getOr<double>("test_timeout", 60.0);  // seconds
        
        // Setup test data
        setupTestData();
    }
};
```

## Chapter 9: Deployment and Production Integration

### 9.1 Production Deployment Integration

#### Production Deployment
```cpp
class ProductionDeploymentAgent {
public:
    ProductionDeploymentAgent(std::shared_ptr<Brain> brain) : brain_(brain) {
        setupProductionIntegration();
    }
    
    void startProductionSimulation() {
        // Initialize production logging
        initializeProductionLogging();
        
        // Setup performance monitoring
        startPerformanceMonitoring();
        
        // Setup health checks
        startHealthChecks();
        
        // Start simulation
        NLM_LOG_INFO("Starting production simulation");
        
        SimulationStep step = 0;
        while (shouldContinueProduction(step)) {
            // Execute production step
            executeProductionStep(step);
            
            // Collect metrics
            collectProductionMetrics(step);
            
            // Log periodically
            if (step % productionLogInterval_ == 0) {
                logProductionStatus(step);
            }
            
            step++;
            
            // Check for recovery actions
            if (checkForRecoveryActions(step)) {
                handleProductionRecovery(step);
            }
        }
        
        stopProductionSimulation();
    }
    
private:
    void initializeProductionLogging() {
        // Configure production logging
        productionLogger_ = std::make_shared<ProductionLogger>(
            "production_nlm.log",
            LogLevel::Info,
            logFileSize_,
            maxLogFiles_
        );
        
        Logger::setGlobal(productionLogger_);
    }
    
    void startPerformanceMonitoring() {
        // Start performance monitoring
        performanceMonitor_ = std::make_unique<ProductionPerformanceMonitor>(brain_);
        performanceMonitor_->startMonitoring();
    }
    
    void startHealthChecks() {
        // Setup health checks
        healthChecker_ = std::make_unique<ProductionHealthChecker>(brain_);
        healthChecker_->startHealthChecks(healthCheckInterval_);
    }
    
    void setupProductionIntegration() {
        // Configure production parameters
        productionLogInterval_ = brain_->getConfig()->
            getOr<size_t>("production_log_interval", 10000);
        
        logFileSize_ = brain_->getConfig()->
            getOr<size_t>("log_file_size_mb", 100);
        
        maxLogFiles_ = brain_->getConfig()->
            getOr<size_t>("max_log_files", 10);
        
        healthCheckInterval_ = brain_->getConfig()->
            getOr<size_t>("health_check_interval", 60000);  // milliseconds
        
        stepTimeout_ = brain_->getConfig()->
            getOr<double>("production_step_timeout", 1000.0);
    }
};
```

## Chapter 10: Documentation and Code Generation

### 10.1 API Documentation Generation

#### Automatic Documentation Generation
```cpp
class APIDocumentationGenerator {
public:
    APIDocumentationGenerator(std::shared_ptr<Brain> brain) : brain_(brain) {
        setupDocumentationGeneration();
    }
    
    void generateAPIDocumentation() {
        // Generate C++ documentation
        generateCppDocumentation();
        
        // Generate Python bindings documentation
        generatePythonDocumentation();
        
        // Generate integration guide
        generateIntegrationGuide();
        
        // Generate API reference
        generateAPIReference();
    }
    
private:
    void setupDocumentationGeneration() {
        // Configure documentation output
        outputDirectory_ = "docs/api_reference";
        documentationFormat_ = DocumentationFormat::Markdown;
        
        // Create output directory
        std::filesystem::create_directories(outputDirectory_);
    }
    
    void generateCppDocumentation() {
        // Extract documentation from source comments
        extractSourceComments();
        
        // Generate class documentation
        generateClassDocumentation();
        
        // Generate method documentation
        generateMethodDocumentation();
    }
    
    void extractSourceComments() {
        // Parse source files for documentation comments
        // Extract Doxygen-style comments, block comments, etc.
    }
    
    void generateIntegrationGuide() {
        // Generate integration guide based on configuration
        // and usage patterns
        
        IntegrationGuide guide;
        
        // Add configuration documentation
        guide.addConfigurationSection();
        
        // Add integration examples
        guide.addIntegrationExamples();
        
        // Add troubleshooting section
        guide.addTroubleshootingSection();
        
        // Save integration guide
        guide.save(outputDirectory_ + "/integration_guide.md");
    }
};
```

## Conclusion

This integration guide provides comprehensive instructions for integrating the NLM neural simulation framework with external systems. It covers:

1. **Core System Integration**: Basic neural simulation setup and configuration
2. **Agent Framework Integration**: Complete agent integration with world interaction
3. **Memory System Integration**: Working memory, episodic memory, and prediction integration
4. **Prediction System Integration**: Predictive decision making and temporal sequence learning
5. **Development System Integration**: Developmental stage management and plasticity modulation
6. **Performance Optimization Integration**: Performance monitoring and optimization
7. **Error Handling and Recovery Integration**: Comprehensive error handling and recovery mechanisms
8. **Testing and Validation Integration**: Integration test framework and validation
9. **Deployment and Production Integration**: Production deployment and monitoring

The integration patterns and code examples provided in this guide enable developers to effectively integrate NLM components with external systems, implement complex scenarios, and build robust neural simulation applications. The modular design and clear interfaces make NLM highly extensible and suitable for a wide range of applications in neuroscience, artificial intelligence, and machine learning.