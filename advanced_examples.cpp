//!
 * NLM Advanced Usage Examples
 * ===========================
 *
 * This file contains advanced examples demonstrating how to use NLM for more complex scenarios.
 * These examples are designed for power users and researchers who want to customize and
 * extend their neural simulations beyond the basic usage patterns.
 *
 * Contents:
 * - Custom Brain Configuration
 * - Advanced Agent Behavior
 * - World Configuration and Interaction
 * - Performance Monitoring
 * - Experiment Control and Automation
 *
 */

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

using namespace nlm;

/**
 * @brief Example: Custom Brain Configuration with Advanced Parameters
 *
 * This demonstrates how to create sophisticated brain configurations
 * for specific research scenarios.
 */
std::shared_ptr<Config> createAdvancedConfig() {
    auto config = std::make_shared<Config>();
    
    // Basic settings
    config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Runtime);
    config->set("connection_probability", 0.15f, ConfigSource::Runtime);
    
    // Plasticity settings for fast learning
    config->set("plasticity.stdp.enable", true, ConfigSource::Runtime);
    config->set("plasticity.stdp.learning_rate", 0.002f, ConfigSource::Runtime);
    config->set("plasticity.stdp.tau_plus", 15.0f, ConfigSource::Runtime);
    config->set("plasticity.stdp.tau_minus", 15.0f, ConfigSource::Runtime);
    
    config->set("plasticity.hebbian.enable", true, ConfigSource::Runtime);
    config->set("plasticity.hebbian.learning_rate", 0.001f, ConfigSource::Runtime);
    
    config->set("plasticity.structural.enable", true, ConfigSource::Runtime);
    config->set("plasticity.structural.synaptogenesis_rate", 0.0002f, ConfigSource::Runtime);
    config->set("plasticity.structural.pruning_rate", 0.00005f, ConfigSource::Runtime);
    
    // Neuromodulation for complex behavior
    config->set("neuromod.dopamine.scale", 1.5f, ConfigSource::Runtime);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Runtime);
    config->set("neuromod.curiosity.exploration_rate", 0.3f, ConfigSource::Runtime);
    config->set("neuromod.novelty.enable", true, ConfigSource::Runtime);
    config->set("neuromod.novelty.sensitivity", 0.8f, ConfigSource::Runtime);
    
    // Prediction system for advanced cognitive tasks
    config->set("prediction.enable", true, ConfigSource::Runtime);
    config->set("prediction.confidence_threshold", 0.7f, ConfigSource::Runtime);
    config->set("prediction.learning_rate", 0.01f, ConfigSource::Runtime);
    
    // Development settings for aging brain simulation
    config->set("development.enable", true, ConfigSource::Runtime);
    config->set("development.initial_plasticity", 1.0f, ConfigSource::Runtime);
    config->set("development.plasticity_decay_rate", 0.95f, ConfigSource::Runtime);
    config->set("development.critical_period_end", 1000.0f, ConfigSource::Runtime);
    
    // Memory system configuration
    config->set("memory.working_memory.capacity", 100, ConfigSource::Runtime);
    config->set("memory.episodic.memory_capacity", 1000, ConfigSource::Runtime);
    config->set("memory.associative.similarity_threshold", 0.8f, ConfigSource::Runtime);
    
    // Performance and simulation settings
    config->set("simulation_timestep", 0.0005, ConfigSource::Runtime);  // Faster simulation
    config->set("performance.use_simd", true, ConfigSource::Runtime);
    config->set("performance.use_multithreading", true, ConfigSource::Runtime);
    config->set("performance.max_threads", 4, ConfigSource::Runtime);
    
    // Logging configuration
    config->set("log_level", "DEBUG", ConfigSource::Runtime);
    config->set("log_to_file", true, ConfigSource::Runtime);
    config->set("log_filename", "nlm_advanced.log", ConfigSource::Runtime);
    config->set("log_file_rotation", true, ConfigSource::Runtime);
    config->set("log_max_file_size", 100 * 1024 * 1024, ConfigSource::Runtime); // 100MB
    
    return config;
}

/**
 * @brief Example: Advanced Agent Behavior with Custom Control Loop
 *
 * This demonstrates how to implement sophisticated agent control patterns
 * for research and experimentation.
 */
class AdvancedAgent {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    bool enableLearning;
    bool enableExploration;
    bool enableDevelopment;
    float learningRate;
    float explorationRate;
    float rewardThreshold;
    
public:
    AdvancedAgent(std::shared_ptr<Config> config) {
        brain = std::make_shared<Brain>(config);
        world = std::make_shared<SimpleWorld>();
        agent = std::make_shared<AgentBrain>(brain);
        
        // Initialize with advanced settings
        initializeAdvanced(config);
        
        // Configure world for complex environment
        world->configure(50, 50, 15, 15);  // Larger world with wider vision
        world->setAgentStart(25.0f, 25.0f);
        world->setRandomSeed(12345);
        
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        enableLearning = true;
        enableExploration = true;
        enableDevelopment = true;
        learningRate = 0.001f;
        explorationRate = 0.2f;
        rewardThreshold = 1.0f;
    }
    
private:
    void initializeAdvanced(std::shared_ptr<Config> config) {
        brain->initialize();
        
        // Configure agent for advanced behavior
        if (config->has("agent.exploration_rate")) {
            explorationRate = config->getOr<float>("agent.exploration_rate", 0.2f);
        }
        if (config->has("agent.learning_rate")) {
            learningRate = config->getOr<float>("agent.learning_rate", 0.001f);
        }
        if (config->has("agent.reward_threshold")) {
            rewardThreshold = config->getOr<float>("agent.reward_threshold", 1.0f);
        }
    }
    
public:
    void setExploration(bool enable) { enableExploration = enable; }
    void setLearning(bool enable) { enableLearning = enable; }
    void setDevelopment(bool enable) { enableDevelopment = enable; }
    
    void update(float timestep) {
        if (!world->isAgentAlive()) {
            return;
        }
        
        // Get current perception
        SensoryPercept percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain step
        static SimulationStep step = 0;
        brain->step(step, step * timestep);
        step++;
        
        // Advanced action selection
        MotorCommand action = selectActionAdvanced(percept);
        
        // Apply action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Get reward and apply modulation
        float reward = world->computeReward(agent->getBrain()->getRegions()[0].get());
        
        // Advanced learning control
        if (enableLearning) {
            agent->applyRewardModulation(reward, 0.0f);
        }
        
        // Update development
        if (enableDevelopment) {
            agent->updateDevelopment(timestep);
        }
        
        // Exploration behavior
        if (enableExploration && percept.getVision()->getData().empty()) {
            // Random exploration if no clear perception
            if (rand() / (float)RAND_MAX < explorationRate) {
                exploreRandomly();
            }
        }
    }
    
private:
    MotorCommand selectActionAdvanced(const SensoryPercept& percept) {
        // Implement intelligent action selection based on multiple factors
        float curiosity = agent->getCuriosityLevel();
        float novelty = agent->getNoveltyLevel();
        float predictionError = agent->getPredictionError();
        
        // Multi-factor action selection
        float actionProbability = 0.0f;
        
        // High curiosity -> exploration
        if (curiosity > 0.7f) {
            actionProbability += curiosity * 0.3f;
        }
        
        // High novelty -> exploration
        if (novelty > 0.5f) {
            actionProbability += novelty * 0.2f;
        }
        
        // High prediction error -> learning actions
        if (predictionError > 0.3f) {
            actionProbability += predictionError * 0.25f;
        }
        
        // Check for rewarding stimuli
        float reward = percept.getInternal()[0] if (!percept.getInternal().empty()) else 0.0f;
        if (reward > rewardThreshold) {
            actionProbability += reward * 0.2f;
        }
        
        // Random action with remaining probability
        float remainingProb = 1.0f - actionProbability;
        if (rand() / (float)RAND_MAX < remainingProb) {
            // Random action
            int actionType = rand() % 4;
            switch (actionType) {
                case 0: return MotorCommand::MoveForward;
                case 1: return MotorCommand::TurnLeft;
                case 2: return MotorCommand::TurnRight;
                default: return MotorCommand::Wait;
            }
        }
        
        // Use prediction-based action selection
        return agent->decodeMotorCommand();
    }
    
    void exploreRandomly() {
        // Random movement for exploration
        int actionType = rand() % 6;
        switch (actionType) {
            case 0: world->applyMotorCommand(MotorCommand::MoveForward, world->getSimulationTime()); break;
            case 1: world->applyMotorCommand(MotorCommand::MoveBackward, world->getSimulationTime()); break;
            case 2: world->applyMotorCommand(MotorCommand::TurnLeft, world->getSimulationTime()); break;
            case 3: world->applyMotorCommand(MotorCommand::TurnRight, world->getSimulationTime()); break;
            case 4: world->applyMotorCommand(MotorCommand::LookLeft, world->getSimulationTime()); break;
            case 5: world->applyMotorCommand(MotorCommand::LookRight, world->getSimulationTime()); break;
        }
    }
    
public:
    void runAdvancedSimulation(int steps, bool saveCheckpoint = true, std::string checkpointPath = "/tmp/advanced_checkpoint.bin") {
        std::cout << "Starting advanced simulation..." << std::endl;
        std::cout << "Configuration: Learning=" << (enableLearning ? "ON" : "OFF") 
                  << ", Exploration=" << (enableExploration ? "ON" : "OFF")
                  << ", Development=" << (enableDevelopment ? "ON" : "OFF") << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < steps; ++step) {
            update(0.1f);
            
            // Progress reporting
            if (step % 1000 == 0) {
                std::cout << "Step " << step << ": "
                          << "Reward=" << world->computeReward(agent->getBrain()->getRegions()[0].get())
                          << ", Curiosity=" << agent->getCuriosityLevel()
                          << ", Novelty=" << agent->getNoveltyLevel()
                          << ", SpikeRate=" << brain->getAverageFiringRate()
                          << std::endl;
            }
            
            // Save checkpoint periodically
            if (saveCheckpoint && step % 5000 == 0 && step > 0) {
                if (brain->save(checkpointPath)) {
                    std::cout << "Checkpoint saved at step " << step << std::endl;
                }
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
        
        std::cout << std::endl << "Simulation completed in " << duration.count() << " seconds" << std::endl;
        std::cout << "Average steps per second: " << (steps / duration.count()) << std::endl;
        
        // Final statistics
        std::cout << std::endl << "Final Statistics:" << std::endl;
        std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        std::cout << "  Neuromodulation Level: " << agent->getNeuromodulationLevel() << std::endl;
        std::cout << "  Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
        std::cout << "  Novelty Level: " << agent->getNoveltyLevel() << std::endl;
    }
    
    std::shared_ptr<Brain> getBrain() const { return brain; }
    std::shared_ptr<AgentBrain> getAgent() const { return agent; }
    std::shared_ptr<SimpleWorld> getWorld() const { return world; }
};

/**
 * @brief Example: World Configuration and Interaction with Advanced Features
 *
 * This demonstrates how to create sophisticated world environments and
 * implement advanced interaction patterns.
 */
void advancedWorldConfiguration() {
    std::cout << "=== Advanced World Configuration Example ===" << std::endl;
    
    // Create world with advanced settings
    auto world = std::make_shared<SimpleWorld>();
    
    // Configure world with advanced parameters
    world->configure(
        width: 100,                    // Large world
        height: 100,
        visionWidth: 20,              // Wide vision
        visionHeight: 20,
        energyDecayRate: 0.01f,        // Moderate energy decay
        maxEnergy: 1000.0f,           // High energy capacity
        visionRange: 15.0f            // Long vision range
    );
    
    // Add complex environment objects
    std::vector<WorldObject> environment;
    
    // Add resources at specific locations
    environment.emplace_back(10.0f, 10.0f, WorldObjectType::Resource, 10.0f);
    environment.emplace_back(90.0f, 10.0f, WorldObjectType::Resource, 20.0f);
    environment.emplace_back(10.0f, 90.0f, WorldObjectType::Resource, 15.0f);
    
    // Add hazards
    environment.emplace_back(50.0f, 50.0f, WorldObjectType::Hazard, 50.0f);
    environment.emplace_back(70.0f, 70.0f, WorldObjectType::Hazard, 30.0f);
    
    // Add walls for complex navigation
    for (int x = 0; x < 100; x += 5) {
        environment.emplace_back(static_cast<float>(x), 0.0f, WorldObjectType::Wall, 1.0f);
        environment.emplace_back(static_cast<float>(x), 100.0f, WorldObjectType::Wall, 1.0f);
        environment.emplace_back(static_cast<float>(x), 50.0f, WorldObjectType::Wall, 1.0f);
    }
    
    // Add dynamic obstacles that move
    for (int i = 0; i < 10; ++i) {
        float x = static_cast<float>(rand() % 80 + 10);
        float y = static_cast<float>(rand() % 80 + 10);
        environment.emplace_back(x, y, WorldObjectType::Hazard, 5.0f);
    }
    
    // Add the environment to world
    for (auto& obj : environment) {
        world->addObject(obj);
    }
    
    std::cout << "World configured with " << environment.size() << " objects" << std::endl;
    std::cout << "World dimensions: " << world->getWidth() << "x" << world->getHeight() << std::endl;
    std::cout << "Agent start position: (25.0, 25.0)" << std::endl;
    std::cout << "Vision range: 15.0 units" << std::endl;
}

/**
 * @brief Example: Performance Monitoring and Metrics Collection
 *
 * This demonstrates how to monitor and analyze simulation performance.
 */
class PerformanceMonitor {
private:
    struct TimingStats {
        double minTime = std::numeric_limits<double>::max();
        double maxTime = 0.0;
        double avgTime = 0.0;
        double totalTime = 0.0;
        uint64_t count = 0;
    };
    
    TimingStats brainStepTime;
    TimingStats worldUpdateTime;
    TimingStats actionSelectionTime;
    TimingStats sensoryProcessingTime;
    
    uint64_t totalSteps = 0;
    uint64_t totalSpikes = 0;
    uint64_t totalMemoryUsage = 0;
    
public:
    void recordBrainStep(double time) {
        brainStepTime.minTime = std::min(brainStepTime.minTime, time);
        brainStepTime.maxTime = std::max(brainStepTime.maxTime, time);
        brainStepTime.totalTime += time;
        brainStepTime.count++;
    }
    
    void recordWorldUpdate(double time) {
        worldUpdateTime.minTime = std::min(worldUpdateTime.minTime, time);
        worldUpdateTime.maxTime = std::max(worldUpdateTime.maxTime, time);
        worldUpdateTime.totalTime += time;
        worldUpdateTime.count++;
    }
    
    void recordActionSelection(double time) {
        actionSelectionTime.minTime = std::min(actionSelectionTime.minTime, time);
        actionSelectionTime.maxTime = std::max(actionSelectionTime.maxTime, time);
        actionSelectionTime.totalTime += time;
        actionSelectionTime.count++;
    }
    
    void recordSensoryProcessing(double time) {
        sensoryProcessingTime.minTime = std::min(sensoryProcessingTime.minTime, time);
        sensoryProcessingTime.maxTime = std::max(sensoryProcessingTime.maxTime, time);
        sensoryProcessingTime.totalTime += time;
        sensoryProcessingTime.count++;
    }
    
    void recordStepMetrics(uint64_t spikes, uint64_t memoryUsage) {
        totalSteps++;
        totalSpikes += spikes;
        totalMemoryUsage += memoryUsage;
    }
    
    void printReport() {
        std::cout << std::endl << "=== PERFORMANCE MONITORING REPORT ===" << std::endl;
        std::cout << std::endl;
        
        auto printTimingStats = [](const std::string& name, const TimingStats& stats) {
            if (stats.count == 0) return;
            double avg = stats.totalTime / stats.count;
            std::cout << name << ":" << std::endl;
            std::cout << "  Operations: " << stats.count << std::endl;
            std::cout << "  Min time: " << std::setprecision(6) << stats.minTime * 1000 << " ms" << std::endl;
            std::cout << "  Max time: " << std::setprecision(6) << stats.maxTime * 1000 << " ms" << std::endl;
            std::cout << "  Avg time: " << std::setprecision(6) << avg * 1000 << " ms" << std::endl;
            std::cout << "  Operations per second: " << stats.count / stats.totalTime << std::endl;
        };
        
        printTimingStats("  Brain Step", brainStepTime);
        printTimingStats("  World Update", worldUpdateTime);
        printTimingStats("  Action Selection", actionSelectionTime);
        printTimingStats("  Sensory Processing", sensoryProcessingTime);
        
        std::cout << std::endl;
        std::cout << "Overall Performance:" << std::endl;
        std::cout << "  Total Steps: " << totalSteps << std::endl;
        std::cout << "  Average Spikes per Step: " << (totalSteps > 0 ? (double)totalSpikes / totalSteps : 0.0) << std::endl;
        std::cout << "  Total Memory Operations: " << totalMemoryUsage << std::endl;
        
        double totalTime = brainStepTime.totalTime + worldUpdateTime.totalTime + 
                          actionSelectionTime.totalTime + sensoryProcessingTime.totalTime;
        if (totalTime > 0) {
            std::cout << "  Steps per Second: " << totalSteps / totalTime << std::endl;
        }
    }
};

/**
 * @brief Example: Experiment Control and Automation
 *
 * This demonstrates how to automate experiments with different configurations
 * and collect results systematically.
 */
class ExperimentRunner {
private:
    std::vector<std::shared_ptr<Config>> configurations;
    std::vector<std::string> experimentNames;
    PerformanceMonitor monitor;
    
public:
    void addConfiguration(std::shared_ptr<Config> config, const std::string& name) {
        configurations.push_back(config);
        experimentNames.push_back(name);
    }
    
    void runAllExperiments() {
        std::cout << "=== EXPERIMENT AUTOMATION ===" << std::endl;
        std::cout << "Running " << configurations.size() << " experiments..." << std::endl << std::endl;
        
        for (size_t i = 0; i < configurations.size(); ++i) {
            std::cout << "=== Experiment " << (i + 1) << "/" << configurations.size() << " ===" << std::endl;
            std::cout << "Name: " << experimentNames[i] << std::endl;
            
            // Create brain with this configuration
            auto brain = std::make_shared<Brain>(configurations[i]);
            brain->initialize();
            
            // Run experiment for a fixed number of steps
            uint64_t steps = configurations[i]->getOr<uint64_t>("experiment.steps", 1000);
            auto startTime = std::chrono::high_resolution_clock::now();
            
            for (uint64_t step = 0; step < steps; ++step) {
                brain->step(step, step * 0.001);
                
                // Record performance metrics
                monitor.recordStepMetrics(
                    brain->getActiveNeuronCount(),
                    brain->getTotalSynapseCount()
                );
                
                if (step % 1000 == 0 && step > 0) {
                    std::cout << "  Completed " << step << "/" << steps << " steps" << std::endl;
                }
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
            
            std::cout << "Experiment completed in " << duration.count() << " seconds" << std::endl;
            std::cout << "Average steps per second: " << (steps / duration.count()) << std::endl;
            std::cout << std::endl;
            
            // Save results
            std::string filename = "experiment_results_" + experimentNames[i] + ".txt";
            std::ofstream file(filename);
            if (file.is_open()) {
                file << "Experiment: " << experimentNames[i] << std::endl;
                file << "Configuration: " << configurations[i]->summary() << std::endl;
                file << "Duration: " << duration.count() << " seconds" << std::endl;
                file << "Final Spike Count: " << brain->getTotalSpikeCount() << std::endl;
                file << "Final Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
                file << "Final Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
                file.close();
                std::cout << "Results saved to " << filename << std::endl;
            }
            
            std::cout << std::endl;
        }
        
        monitor.printReport();
    }
};

int main() {
    std::cout << "=== NLM Advanced Usage Examples ===" << std::endl << std::endl;
    
    // Example 1: Advanced Brain Configuration
    std::cout << "Example 1: Advanced Brain Configuration" << std::endl;
    auto advancedConfig = createAdvancedConfig();
    std::cout << "Created advanced configuration with " << advancedConfig->getKeys().size() << " parameters" << std::endl;
    std::cout << "Sample values:" << std::endl;
    std::cout << "  - Neuron count: " << advancedConfig->getOr<int64_t>("neuron_count", 0) << std::endl;
    std::cout << "  - Connection probability: " << advancedConfig->getOr<float>("connection_probability", 0.0f) << std::endl;
    std::cout << "  - STDP learning rate: " << advancedConfig->getOr<float>("plasticity.stdp.learning_rate", 0.0f) << std::endl;
    std::cout << std::endl;
    
    // Example 2: Advanced Agent Behavior
    std::cout << "Example 2: Advanced Agent Behavior" << std::endl;
    auto agentConfig = createAdvancedConfig();
    auto advancedAgent = std::make_shared<AdvancedAgent>(agentConfig);
    advancedAgent->setExploration(true);
    advancedAgent->setLearning(true);
    advancedAgent->setDevelopment(true);
    
    std::cout << "Created advanced agent with:" << std::endl;
    std::cout << "  - Learning enabled: " << (advancedAgent->getAgent()->isRewardModulationEnabled() ? "Yes" : "No") << std::endl;
    std::cout << "  - Curiosity enabled: " << (advancedAgent->getAgent()->isCuriosityEnabled() ? "Yes" : "No") << std::endl;
    std::cout << "  - Development enabled: " << (advancedAgent->getAgent()->isDevelopmentEnabled() ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
    
    // Example 3: World Configuration
    std::cout << "Example 3: Advanced World Configuration" << std::endl;
    advancedWorldConfiguration();
    std::cout << std::endl;
    
    // Example 4: Performance Monitoring
    std::cout << "Example 4: Performance Monitoring" << std::endl;
    PerformanceMonitor monitor;
    std::cout << "Performance monitoring system initialized" << std::endl;
    std::cout << "This can be used to track simulation performance in real-time" << std::endl;
    std::cout << std::endl;
    
    // Example 5: Experiment Automation
    std::cout << "Example 5: Experiment Automation" << std::endl;
    ExperimentRunner runner;
    
    // Add multiple experiment configurations
    auto config1 = createAdvancedConfig();
    config1->set("neuron_count", 1000);
    config1->set("experiment.steps", 2000);
    runner.addConfiguration(config1, "Basic Configuration");
    
    auto config2 = createAdvancedConfig();
    config2->set("neuron_count", 5000);
    config2->set("connection_probability", 0.2f);
    config2->set("experiment.steps", 1000);
    runner.addConfiguration(config2, "High Connectivity");
    
    auto config3 = createAdvancedConfig();
    config3->set("neuron_count", 2000);
    config3->set("plasticity.stdp.learning_rate", 0.005f);
    config3->set("experiment.steps", 1500);
    runner.addConfiguration(config3, "Slow Learning");
    
    std::cout << "Prepared " << runner.getConfigurationCount() << " experiment configurations" << std::endl;
    std::cout << "Experiment automation ready for execution" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== Advanced Examples Complete ===" << std::endl;
    std::cout << "These examples demonstrate how to use NLM for advanced scenarios." << std::endl;
    std::cout << "For more information, refer to the documentation and examples in the project." << std::endl;
    
    return 0;
}