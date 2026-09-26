#pragma once

#include "CoreTypes.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

namespace nlm {

// Forward declaration for Action types
class Action;
class MotorCommand;
struct ActionResult;
struct SensoryPercept;
struct Goal;

// Main nLM interface for users
class NLM {
public:
    NLM();
    ~NLM();
    
    // Disable copying, enable moving
    NLM(const NLM&) = delete;
    NLM& operator=(const NLM&) = delete;
    NLM(NLM&&) noexcept = default;
    NLM& operator=(NLM&&) noexcept = default;
    
    // Initialize NLM with configuration
    bool initialize(const std::string& configFile = "configs/default.cfg");
    
    // Run simulation for specified number of steps
    void run(int steps);
    
    // Run complete experiment
    void runExperiment(const std::string& experimentName, int steps);
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Get brain reference
    std::shared_ptr<Brain> getBrain() const { return brain_; }
    
    // Get world reference
    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }
    
    // Get agent brain reference
    std::shared_ptr<AgentBrain> getAgentBrain() const { return agentBrain_; }
    
    // Get simulation clock
    std::shared_ptr<SimulationClock> getClock() const { return clock_; }
    
    // Get logger reference
    std::shared_ptr<Logger> getLogger() const { return logger_; }
    
    // Set plugin manager
    void setPluginManager(std::shared_ptr<PluginManager> pluginManager) {
        pluginManager_ = pluginManager;
    }
    
    // Get plugin manager
    std::shared_ptr<PluginManager> getPluginManager() const { return pluginManager_; }
    
    // Get component factory
    std::shared_ptr<ComponentFactory> getComponentFactory() const { return componentFactory_; }
    
    // Save current state to checkpoint
    bool saveCheckpoint(const std::string& filepath) const;
    
    // Load state from checkpoint
    bool loadCheckpoint(const std::string& filepath);
    
    // Get simulation statistics
    std::unordered_map<std::string, float> getStatistics() const;
    
    // Print simulation status
    void printStatus() const;
    
    // Run all Phase 6 demos
    void runAllPhase6Demos();
    
    // Run specific demo
    void runPhaseDemo(int phase, int demo);

private:
    // Core components
    std::shared_ptr<Config> config_;
    std::shared_ptr<SimulationClock> clock_;
    std::shared_ptr<RandomGenerator> random_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<SimpleWorld> world_;
    
    // Plugin system
    std::shared_ptr<PluginManager> pluginManager_;
    std::shared_ptr<ComponentFactory> componentFactory_;
    
    // Configuration
    std::string configFile_;
    
    // State
    bool initialized_;
    SimulationStep currentStep_;
    
    // Setup methods
    void setupPluginSystem();
    void setupComponentFactory();
    void initializePluginSystem();
    void createDefaultBrain();
    void setupExperimentSystem();
    void setupVisualization();
    
    // Run methods
    void runPhase6Integration();
    void runPhase6MemorySystem();
    void runPhase6Neuromodulation();
    void runPhase6Prediction();
    void runPhase6Cognition();
    void runPhase6Development();
    
    // Statistics and monitoring
    void updateStatistics();
    void logProgress() const;
};

// Convenience function to create and run NLM
always_inline std::unique_ptr<NLM> createNLM(const std::string& configFile = "configs/default.cfg") {
    auto nlm = std::make_unique<NLM>();
    if (!nlm->initialize(configFile)) {
        return nullptr;
    }
    return nlm;
}

// Advanced NLM builder for complex configurations
class NLMBuilder {
public:
    NLMBuilder();
    ~NLMBuilder();
    
    // Disable copying, enable moving
    NLMBuilder(const NLMBuilder&) = delete;
    NLMBuilder& operator=(const NLMBuilder&) = delete;
    NLMBuilder(NLMBuilder&&) noexcept = default;
    NLMBuilder& operator=(NLMBuilder&&) noexcept = default;
    
    // Set configuration
    NLMBuilder& withConfig(const Config& config);
    
    // Set configuration file
    NLMBuilder& withConfigFile(const std::string& configFile);
    
    // Set neuron count
    NLMBuilder& withNeuronCount(size_t count);
    
    // Set region count
    NLMBuilder& withRegionCount(size_t count);
    
    // Set connection probability
    NLMBuilder& withConnectionProbability(float prob);
    
    // Set random seed
    NLMBuilder& withRandomSeed(uint64_t seed);
    
    // Enable plasticity rules
    NLMBuilder& withPlasticityRules(const std::vector<std::string>& ruleNames);
    
    // Add custom plasticity rule
    NLMBuilder& withCustomPlasticityRule(std::shared_ptr<IPlasticityRule> rule);
    
    // Enable memory systems
    NLMBuilder& withMemorySystems(const std::vector<std::string>& systemNames);
    
    // Add custom memory system
    NLMBuilder& withCustomMemorySystem(std::shared_ptr<IMemorySystem> system);
    
    // Enable behavior modules
    NLMBuilder& withBehaviorModules(const std::vector<std::string>& moduleNames);
    
    // Add custom behavior module
    NLMBuilder& withCustomBehaviorModule(std::shared_ptr<IBehaviorModule> module);
    
    // Set world size
    NLMBuilder& withWorldSize(float width, float height);
    
    // Enable visualization
    NLMBuilder& withVisualization(bool enabled);
    
    // Enable checkpointing
    NLMBuilder& withCheckpointing(const std::string& checkpointDir);
    
    // Add experiment to run
    NLMBuilder& addExperiment(const std::string& name, int steps);
    
    // Add demo to run
    NLMBuilder& addDemo(int phase, int demo);
    
    // Build NLM instance
    std::unique_ptr<NLM> build();
    
    // Build and run
    bool buildAndRun();
    
    // Get build summary
    std::string getBuildSummary() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm