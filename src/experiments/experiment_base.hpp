#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>
#include <chrono>

namespace nlm {

/**
 * Base class for experiment execution
 * Provides common infrastructure for experiment setup, execution, and teardown
 */
class ExperimentBase {
public:
    ExperimentBase() = default;
    virtual ~ExperimentBase() = default;
    
    /**
     * Initialize the experiment with configuration and create all required components
     */
    virtual void initialize(const std::string& configFile = "") {
        setupConfiguration(configFile);
        setupBrain();
        setupWorld();
        setupAgentBrain();
        
        // Record start time
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    /**
     * Run the experiment
     */
    virtual void run() = 0;
    
    /**
     * Cleanup after experiment completion
     */
    virtual void cleanup() {
        // Common cleanup logic
        brain.reset();
        agentBrain.reset();
        world.reset();
        config.reset();
        
        // Record end time and calculate duration
        endTime = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
    }
    
    /**
     * Get the brain instance
     */
    std::shared_ptr<Brain> getBrain() const { return brain; }
    
    /**
     * Get the agent brain instance
     */
    AgentBrain* getAgentBrain() const { return agentBrain.get(); }
    
    /**
     * Get the world instance
     */
    SimpleWorld* getWorld() const { return world.get(); }
    
    /**
     * Get the configuration instance
     */
    std::shared_ptr<Config> getConfig() const { return config; }
    
    /**
     * Get experiment duration in seconds
     */
    double getDuration() const { return duration; }
    
    /**
     * Get start time as timestamp
     */
    time_t getStartTime() const { return startTime; }
    
    /**
     * Get end time as timestamp
     */
    time_t getEndTime() const { return endTime; }
protected:
    /**
     * Setup basic configuration
     */
    void setupConfiguration(const std::string& configFile = "") {
        config = std::make_shared<Config>();
        
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        } else {
            // Apply default configuration
            config->set("neuron_count", static_cast<int64_t>(500));
            config->set("region_count", static_cast<int64_t>(2));
            config->set("connection_probability", 0.1f);
            config->set("random_seed", static_cast<int64_t>(42));
            config->set("simulation_timestep", 0.001f);
            config->set("stdp_ltp_weight", 0.01f);
            config->set("stdp_ltd_weight", 0.012f);
            config->set("synaptogenesis_rate", 0.0001f);
            config->set("pruning_rate", 0.00001f);
        }
        
        Logger::getGlobal().log("Configuration setup complete");
    }
    
    /**
     * Setup brain from configuration
     */
    void setupBrain() {
        brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        Logger::getGlobal().log("Brain setup complete");
    }
    
    /**
     * Setup world
     */
    void setupWorld() {
        world = std::make_unique<SimpleWorld>();
        world->configure(20.0f, 20.0f, 16, 16);
        world->setRandomSeed(42);
        world->reset();
        Logger::getGlobal().log("World setup complete");
    }
    
    /**
     * Setup agent brain interface
     */
    void setupAgentBrain() {
        if (brain && world) {
            agentBrain = std::make_unique<AgentBrain>(brain);
            agentBrain->initialize(*world);
            
            // Enable common features
            agentBrain->enableRewardModulation(true);
            agentBrain->enableCuriosity(true);
            agentBrain->enableStructuralPlasticity(true);
            agentBrain->enableDevelopment(true);
            
            Logger::getGlobal().log("Agent brain setup complete");
        }
    }
    
    // Common experiment components
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::unique_ptr<AgentBrain> agentBrain;
    std::unique_ptr<SimpleWorld> world;
    
    // Timing
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    double duration{0.0};
};

} // namespace nlm
