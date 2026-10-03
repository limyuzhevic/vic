#pragma once

#include "Brain.hpp"
#include "Config.hpp"
#include "AgentBrain.hpp"
#include "SimpleWorld.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>

namespace nlm {

/**
 * Simple environment combining brain, world, and agent for easy simulation
 * 
 * This convenience class provides a simplified interface for running simulations
 * with minimal setup code. It combines the brain, world, and agent into a single
 * object that handles common simulation patterns.
 */
class SimpleEnvironment {
public:
    SimpleEnvironment();
    ~SimpleEnvironment();
    
    // Initialize the environment with specified dimensions
    void initialize(float width = 16.0f, float height = 16.0f,
                    float visionWidth = 8.0f, float visionHeight = 8.0f);
    
    // Run a complete simulation episode
    void runEpisode(size_t numSteps, bool render = false);
    
    // Reset environment for new episode
    void reset();
    
    // Get the brain object
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Get the world object
    SimpleWorld* getWorld() { return world_.get(); }
    const SimpleWorld* getWorld() const { return world_.get(); }
    
    // Get the agent brain object
    AgentBrain* getAgent() { return agent_.get(); }
    const AgentBrain* getAgent() const { return agent_.get(); }
    
    // Configure brain with common settings
    void configureBrain(size_t neuronCount = 1000, size_t regionCount = 1,
                       float connectionProbability = 0.1f);
    
    // Enable all common subsystems
    void enableAllSubsystems();
    
    // Get simulation statistics
    struct EpisodeStats {
        size_t stepsCompleted;
        float totalReward;
        float avgFiringRate;
        size_t activeNeurons;
        float curiosityLevel;
        float noveltyLevel;
        float developmentStage;
        
        EpisodeStats()
            : stepsCompleted(0), totalReward(0.0f), avgFiringRate(0.0f),
              activeNeurons(0), curiosityLevel(0.0f), noveltyLevel(0.0f),
              developmentStage(0.0f) {}
    };
    
    EpisodeStats getEpisodeStats() const;
    
    // Run a single simulation step with automatic environment updates
    bool step(double timestep = 0.1);
    
    // Check if environment is initialized
    bool isInitialized() const { return brain_ != nullptr && world_ != nullptr && agent_ != nullptr; }
    
    // Get environment state description
    std::string getStateDescription() const;

private:
    // Core components
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    
    // Episode tracking
    size_t episodeStepsCompleted_;
    float episodeTotalReward_;
    float episodeAvgFiringRate_;
    bool initialized_;
    
    // Internal helper methods
    void setupBrainComponents();
    void setupWorldComponents();
    void setupAgentComponents();
    void updateStatistics(double timestep);
};

} // namespace nlm