// Simulation controller for CLI
// Provides high-level simulation management including run, step, and episode execution

#pragma once

#include "cli_parser.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <thread>
#include <chrono>

namespace nlm {

// Simulation controller states
enum class SimulationState {
    Idle,
    Running,
    Paused,
    Completed,
    Error
};

// Simulation statistics
struct SimulationStats {
    size_t totalSteps;
    size_t totalSpikes;
    float averageFiringRate;
    size_t activeNeurons;
    size_t firingNeurons;
    double simulationTime;
    SimulationState state;
    
    SimulationStats() : totalSteps(0), totalSpikes(0), averageFiringRate(0.0f),
                       activeNeurons(0), firingNeurons(0), simulationTime(0.0), 
                       state(SimulationState::Idle) {}
};

// High-level simulation controller
class SimulationController {
public:
    SimulationController(std::shared_ptr<Config> config);
    ~SimulationController();
    
    // Run simulation for specified steps
    CommandResult runSteps(size_t steps, bool withAgent = false,
                          const std::string& brainConfig = "",
                          const std::string& worldConfig = "");
    
    // Execute single step
    CommandResult step(size_t steps = 1);
    
    // Run complete episode
    CommandResult runEpisode(size_t steps);
    
    // Train agent for specified episodes
    CommandResult train(size_t episodes);
    
    // Get current simulation statistics
    const SimulationStats& getStats() const { return stats_; }
    
    // Get brain instance
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Get agent brain instance (if created)
    std::shared_ptr<AgentBrain> getAgentBrain() const { return agentBrain_; }
    
    // Get world instance (if created)
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    
    // Reset simulation
    CommandResult reset();
    
    // Save brain state to file
    CommandResult saveBrain(const std::string& filepath) const;
    
    // Load brain state from file
    CommandResult loadBrain(const std::string& filepath);
    
    // Export simulation statistics
    CommandResult exportStats(const std::string& filepath) const;
    
    // Print current status
    void printStatus() const;
    
    // Set verbose mode
    void setVerbose(bool verbose) { verboseMode_ = verbose; }
    
private:
    // Initialize brain with configuration
    bool initializeBrain(const std::string& config = "");
    
    // Initialize agent brain
    bool initializeAgentBrain(const std::string& config = "");
    
    // Initialize world
    bool initializeWorld(const std::string& config = "");
    
    // Run simulation loop
    CommandResult runSimulationLoop(size_t steps, bool withAgent);
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<SimpleWorld> world_;
    SimulationStats stats_;
    bool verboseMode_;
    
    // Thread for simulation
    std::thread simulationThread_;
    bool simulationRunning_;
    
    // Performance timing
    std::chrono::steady_clock::time_point startTime_;
};

} // namespace nlm