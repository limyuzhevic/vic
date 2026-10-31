// NLM - Neural Learning Machine
// Utility functions for simplified API access
// Provides helper functions to make the NLM API easier to use

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <cmath>

namespace nlm {

// Forward declarations
class Brain;
class Config;
class Neuron;
class Synapse;
class Action;
class SimpleWorld;
class SensoryPercept;

// Utility functions for brain management
class BrainUtils {
public:
    // Create a standard brain with recommended configuration
    static std::shared_ptr<Brain> createStandardBrain(
        size_t neuronCount = 500,
        size_t regionCount = 1,
        float connectionProbability = 0.15f,
        std::shared_ptr<Config> config = nullptr
    );
    
    // Create a configuration with common settings
    static std::shared_ptr<Config> createDefaultConfig(
        const std::string& worldWidth = "20.0",
        const std::string& worldHeight = "20.0",
        const std::string& maxEnergy = "100.0",
        const std::string& moveSpeed = "3.0",
        const std::string& turnSpeed = "2.0"
    );
    
    // Run a simple demo with the given configuration
    static void runSimpleDemo(
        const std::shared_ptr<Config>& config,
        int numEpisodes = 5,
        int stepsPerEpisode = 200
    );
};

// Utility functions for sensory processing
class SensoryUtils {
public:
    // Create a standard vision percept
    static SensoryPercept createVisionPercept(
        const std::vector<float>& vision,
        const std::vector<float>& touch = {},
        const std::vector<float>& internal = {},
        const std::vector<float>& proprioception = {}
    );
    
    // Create an empty percept
    static SensoryPercept createEmptyPercept();
    
    // Create a percept with resource detection
    static SensoryPercept createResourcePercept(
        size_t visionIndex = 5,
        float visionValue = 0.7f
    );
};

// Utility functions for motor control
class MotorUtils {
public:
    // Get the best action from a brain
    static MotorCommand getBestAction(std::shared_ptr<Brain> brain);
    
    // Get action with reward bias
    static MotorCommand getActionWithRewardBias(
        std::shared_ptr<Brain> brain,
        float rewardBias = 0.0f
    );
    
    // Get action with curiosity
    static MotorCommand getActionWithCuriosity(
        std::shared_ptr<Brain> brain,
        float curiosityLevel = 0.0f
    );
};

// Utility functions for simulation control
class SimulationUtils {
public:
    // Run a simulation with automatic reset
    static float runSimulation(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        int numEpisodes = 10,
        int stepsPerEpisode = 200
    );
    
    // Run a simulation with detailed metrics
    static struct EpisodeMetrics {
        float totalReward;
        float finalEnergy;
        float finalNovelty;
        float finalCuriosity;
        float predictionError;
        size_t totalSpikes;
        size_t resourcesConsumed;
        size_t hazardsHit;
        size_t emptyInteracts;
        int steps;
    } runSimulationWithMetrics(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        int numEpisodes = 10,
        int stepsPerEpisode = 200
    );
};

// Utility functions for configuration management
class ConfigUtils {
public:
    // Get configuration value with type conversion
    template<typename T>
    static T getConfig(const std::shared_ptr<Config>& config, const std::string& key, const T& defaultValue);
    
    // Set configuration value
    template<typename T>
    static void setConfig(std::shared_ptr<Config> config, const std::string& key, const T& value);
    
    // Print configuration summary
    static void printConfigSummary(const std::shared_ptr<Config>& config);
    
    // Create a configuration file for a specific use case
    static bool saveConfigTemplate(const std::string& filename, const std::string& useCase);
};

// Utility functions for debugging and analysis
class DebugUtils {
public:
    // Print brain state for debugging
    static void printBrainState(std::shared_ptr<Brain> brain);
    
    // Print configuration for debugging
    static void printConfiguration(std::shared_ptr<Config> config);
    
    // Save simulation data to file
    static bool saveSimulationData(
        const std::string& filename,
        const std::vector<std::shared_ptr<Brain>>& brains,
        const std::vector<float>& rewards
    );
    
    // Load simulation data from file
    static bool loadSimulationData(
        const std::string& filename,
        std::vector<std::shared_ptr<Brain>>& brains,
        std::vector<float>& rewards
    );
};

} // namespace nlm
