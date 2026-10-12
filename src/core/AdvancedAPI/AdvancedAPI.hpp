#pragma once

#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../core/Config/Config.hpp"
#include "../memory/Memory.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// AgentConfiguration: Simplified configuration for common use cases
class AgentConfiguration {
public:
    // Create configuration for learning agent
    static Config createLearningAgentConfig();
    
    // Create configuration for reactive agent  
    static Config createReactiveAgentConfig();
    
    // Create configuration for memory-intensive agent
    static Config createMemoryAgentConfig();
    
    // Create configuration for exploration-focused agent
    static Config createExplorationAgentConfig();
    
    // Create configuration for reinforcement learning agent
    static Config createRLAgentConfig();
    
    // Apply configuration to brain
    static bool applyToBrain(Config& config, Brain* brain);
    
    // Get configuration for specific task type
    static Config getTaskConfig(const std::string& taskType);
};

// SimpleBrain: High-level brain interface for beginners
class SimpleBrain {
public:
    // Create brain from configuration
    static std::unique_ptr<Brain> createFromConfig(const Config& config);
    
    // Create brain with default settings
    static std::unique_ptr<Brain> createDefault();
    
    // Create brain for specific environment
    static std::unique_ptr<Brain> createForEnvironment(const Environment& environment);
    
    // Train brain with episode
    static bool train(Brain* brain, const class SensoryInput& input, float reward);
    
    // Get brain performance metrics
    static std::map<std::string, double> getPerformanceMetrics(Brain* brain);
};

// AgentContext: Convenience context for agent operations
class AgentContext {
public:
    AgentContext(AgentBrain* agent, Brain* brain, const SimpleWorld& world);
    
    // Process sensory input and get action
    MotorCommand process(const SensoryPercept& percept);
    
    // Get current agent status
    std::string getStatus() const;
    
    // Check if agent is ready for operation
    bool isReady() const;
    
    // Get agent state summary
    std::string getStateSummary() const;
    
    // Reset agent for new episode
    void resetForEpisode(const SimpleWorld& world);
    
    // Get comprehensive metrics
    std::map<std::string, double> getMetrics() const;
    
private:
    AgentBrain* agent_;
    Brain* brain_;
    const SimpleWorld* world_;
    mutable std::map<std::string, double> cachedMetrics_;
    bool metricsDirty_;
};

// MemoryManager: High-level memory system interface
class MemoryManager {
public:
    MemoryManager(Brain* brain);
    
    // Store experience
    bool storeExperience(const class SensoryInput& input, float reward, const MotorCommand& action);
    
    // Retrieve similar experiences
    std::vector<class EpisodicMemoryItem> retrieveSimilarExperiences(const class SensoryInput& input, size_t count = 10) const;
    
    // Get memory statistics
    std::map<std::string, double> getMemoryStats() const;
    
    // Clean old memories
    void cleanupOldMemories(SimulationStep maxAge);
    
    // Get memory health
    bool isMemoryHealthy() const;
    
    // Get usage statistics
    std::string getUsageStatistics() const;
    
private:
    Brain* brain_;
};

// BrainTrainer: High-level training interface
class BrainTrainer {
public:
    // Create trainer with configuration
    static std::unique_ptr<BrainTrainer> create(const Config& config);
    
    // Train brain with episode
    bool train(Brain* brain, const class SensoryInput& input, float reward, double timestep);
    
    // Get training progress
    std::map<std::string, double> getTrainingProgress(Brain* brain) const;
    
    // Check if training is complete
    bool isTrainingComplete(Brain* brain) const;
    
    // Reset training progress
    void resetProgress(Brain* brain);
    
    // Get configuration
    Config getConfig() const;
    
    // Apply configuration
    void applyConfig(const Config& config);
    
private:
    Config config_;
    SimulationStep trainingSteps_;
};

// EnvironmentHelper: Convenience methods for environment interaction
class EnvironmentHelper {
public:
    // Create observation from percept
    static SensoryInput createObservation(const SensoryPercept& percept, const SimpleWorld& world);
    
    // Get sensory input size for brain
    static size_t getExpectedSensoryInputSize(const Brain* brain);
    
    // Get motor output size expected by brain
    static size_t getExpectedMotorOutputSize(const Brain* brain);
    
    // Create world from environment state
    static SimpleWorld createWorld(const Environment& environment);
    
    // Update world from environment
    static void updateWorld(SimpleWorld& world, const Environment& environment);
    
    // Get environment statistics
    static std::map<std::string, double> getEnvironmentStats(const Environment& environment);
};

} // namespace nlm
