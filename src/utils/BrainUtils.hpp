#pragma once

#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <map>

namespace nlm {

/**
 * @brief Configuration utilities for NLM
 * 
 * Provides helper functions for creating and managing NLM configurations
 */
class ConfigurationUtils {
public:
    /**
     * @brief Create a default configuration with all Phase 6 systems enabled
     * 
     * @return Shared pointer to a configured Config object
     */
    static std::shared_ptr<Config> createPhase6Config();
    
    /**
     * @brief Create a simplified configuration for Phase 2 (basic neural computation)
     * 
     * @return Shared pointer to a simplified Config object
     */
    static std::shared_ptr<Config> createPhase2Config();
    
    /**
     * @brief Create a configuration optimized for simulation performance
     * 
     * @param targetNeurons Target number of neurons for the simulation
     * @param timestep Simulation timestep
     * @return Shared pointer to an optimized Config object
     */
    static std::shared_ptr<Config> createOptimizedConfig(
        size_t targetNeurons = 1000,
        double timestep = 0.001);
    
    /**
     * @brief Load configuration from environment variables or file
     * 
     * @param filename Configuration file path (optional)
     * @param envPrefix Environment variable prefix (optional)
     * @return Shared pointer to loaded Config object
     */
    static std::shared_ptr<Config> loadConfiguration(
        const std::string& filename = "",
        const std::string& envPrefix = "NLM_");
    
    /**
     * @brief Validate configuration for consistency
     * 
     * @param config Configuration to validate
     * @return true if configuration is valid, false otherwise
     */
    static bool validateConfiguration(const Config& config);
    
    /**
     * @brief Print configuration summary to log
     * 
     * @param config Configuration to summarize
     */
    static void logConfigurationSummary(const Config& config);
};

/**
 * @brief Brain utilities for common operations
 * 
 * Provides helper functions for brain creation, initialization, and common operations
 */
class BrainUtils {
public:
    /**
     * @brief Create and initialize a complete brain with all systems
     * 
     * @param config Configuration for the brain
     * @param autoInit Whether to automatically initialize the brain
     * @return Shared pointer to the created Brain object
     */
    static std::shared_ptr<Brain> createAndInitializeBrain(
        std::shared_ptr<Config> config,
        bool autoInit = true);
    
    /**
     * @brief Create a brain and add default regions and populations
     * 
     * @param config Configuration for the brain
     * @param neuronCount Total number of neurons to create
     * @param regionCount Number of regions to create
     * @return Shared pointer to the created Brain object
     */
    static std::shared_ptr<Brain> createDefaultBrain(
        std::shared_ptr<Config> config,
        size_t neuronCount = 1000,
        size_t regionCount = 1);
    
    /**
     * @brief Run a complete simulation cycle
     * 
     * @param brain Brain to simulate
     * @param numSteps Number of simulation steps to run
     * @param timestep Time step for the simulation
     * @param sensoryInput Optional sensory input to provide
     * @return Vector of action results from the simulation
     */
    static std::vector<std::unique_ptr<Action>> runSimulation(
        std::shared_ptr<Brain> brain,
        size_t numSteps,
        double timestep,
        const SensoryInput* sensoryInput = nullptr);
    
    /**
     * @brief Get comprehensive brain statistics
     * 
     * @param brain Brain to query
     * @return Map of statistic names to values
     */
    static std::map<std::string, double> getBrainStatistics(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Reset brain to initial state and reinitialize
     * 
     * @param brain Brain to reset
     * @return true if reset successful, false otherwise
     */
    static bool resetAndReinitialize(std::shared_ptr<Brain> brain);
};

/**
 * @brief Agent utilities for world interaction
 * 
 * Provides helper functions for agent creation, configuration, and world interaction
 */
class AgentUtils {
public:
    /**
     * @brief Create a complete agent system (brain + world + agent brain)
     * 
     * @param config Configuration for the brain
     * @param world World for the agent to interact with
     * @return Shared pointer to the created AgentBrain object
     */
    static std::shared_ptr<AgentBrain> createAgentSystem(
        std::shared_ptr<Config> config,
        std::shared_ptr<SimpleWorld> world);
    
    /**
     * @brief Create a default agent world with typical configuration
     * 
     * @param width World width
     * @param height World height
     * @return Shared pointer to the created SimpleWorld object
     */
    static std::shared_ptr<SimpleWorld> createDefaultWorld(
        float width = 20.0f,
        float height = 20.0f);
    
    /**
     * @brief Create an agent with all learning systems enabled
     * 
     * @param brain Brain to use
     * @param world World to interact with
     * @return Shared pointer to the created AgentBrain object
     */
    static std::shared_ptr<AgentBrain> createLearningAgent(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world);
    
    /**
     * @brief Run a complete agent episode
     * 
     * @param agent AgentBrain to run
     * @param world World to interact with
     * @param numSteps Number of steps to run
     * @return Vector of action results from the episode
     */
    static std::vector<ActionResult> runAgentEpisode(
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        size_t numSteps);
    
    /**
     * @brief Get agent status and statistics
     * 
     * @param agent AgentBrain to query
     * @return Map of status names to values
     */
    static std::map<std::string, double> getAgentStatus(std::shared_ptr<AgentBrain> agent);
};

/**
 * @brief Validation utilities for input validation and checking
 * 
 * Provides helper functions for validating inputs, states, and configurations
 */
class ValidationUtils {
public:
    /**
     * @brief Validate neural network architecture
     * 
     * @param brain Brain to validate
     * @param minNeurons Minimum number of neurons required
     * @param maxRegions Maximum number of regions allowed
     * @return true if architecture is valid, false otherwise
     */
    static bool validateBrainArchitecture(
        std::shared_ptr<Brain> brain,
        size_t minNeurons = 1,
        size_t maxRegions = 100);
    
    /**
     * @brief Validate world configuration
     * 
     * @param world World to validate
     * @param minWidth Minimum world width
     * @param maxWidth Maximum world width
     * @return true if world configuration is valid, false otherwise
     */
    static bool validateWorldConfiguration(
        std::shared_ptr<SimpleWorld> world,
        float minWidth = 1.0f,
        float maxWidth = 1000.0f);
    
    /**
     * @brief Validate sensory input data
     * 
     * @param percept Sensory percept to validate
     * @param maxVisionSize Maximum allowed vision size
     * @param maxTouchSize Maximum allowed touch size
     * @return true if sensory input is valid, false otherwise
     */
    static bool validateSensoryInput(
        const SensoryPercept& percept,
        size_t maxVisionSize = 10000,
        size_t maxTouchSize = 1000);
    
    /**
     * @brief Validate action result
     * 
     * @param result Action result to validate
     * @return true if action result is valid, false otherwise
     */
    static bool validateActionResult(const ActionResult& result);
    
    /**
     * @brief Check if simulation parameters are within valid ranges
     * 
     * @param timestep Time step to validate
     * @param numSteps Number of steps to validate
     * @param maxSteps Maximum allowed steps
     * @return true if parameters are valid, false otherwise
     */
    static bool validateSimulationParameters(
        double timestep,
        size_t numSteps,
        size_t maxSteps = 1000000);
};

/**
 * @brief Benchmarking utilities for performance measurement
 * 
 * Provides helper functions for benchmarking brain simulation performance
 */
class BenchmarkingUtils {
public:
    /**
     * @brief Measure brain simulation performance
     * 
     * @param brain Brain to benchmark
     * @param numSteps Number of simulation steps
     * @param timestep Time step
     * @param iterations Number of benchmark iterations
     * @return Map of benchmark metrics (steps_per_second, memory_usage, etc.)
     */
    static std::map<std::string, double> benchmarkBrainSimulation(
        std::shared_ptr<Brain> brain,
        size_t numSteps,
        double timestep,
        size_t iterations = 3);
    
    /**
     * @brief Measure agent world interaction performance
     * 
     * @param agent Agent to benchmark
     * @param world World to interact with
     * @param numSteps Number of simulation steps
     * @param iterations Number of benchmark iterations
     * @return Map of benchmark metrics
     */
    static std::map<std::string, double> benchmarkAgentWorldInteraction(
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        size_t numSteps,
        size_t iterations = 3);
    
    /**
     * @brief Profile memory usage of brain simulation
     * 
     * @param brain Brain to profile
     * @param numSteps Number of simulation steps
     * @param timestep Time step
     * @return Memory usage statistics
     */
    static std::map<std::string, double> profileMemoryUsage(
        std::shared_ptr<Brain> brain,
        size_t numSteps,
        double timestep);
    
    /**
     * @brief Measure spike generation rate
     * 
     * @param brain Brain to measure
     * @param numSteps Number of simulation steps
     * @param timestep Time step
     * @return Spike generation rate (spikes per second)
     */
    static double measureSpikeGenerationRate(
        std::shared_ptr<Brain> brain,
        size_t numSteps,
        double timestep);
};

/**
 * @brief Serialization utilities for saving/loading brain state
 * 
 * Provides helper functions for serialization and deserialization of brain state
 */
class SerializationUtils {
public:
    /**
     * @brief Save brain state to file
     * 
     * @param brain Brain to save
     * @param filepath File path to save to
     * @param compressionLevel Compression level (0-9)
     * @return true if save successful, false otherwise
     */
    static bool saveBrainState(
        std::shared_ptr<Brain> brain,
        const std::string& filepath,
        int compressionLevel = 5);
    
    /**
     * @brief Load brain state from file
     * 
     * @param filepath File path to load from
     * @return Shared pointer to loaded Brain object
     */
    static std::shared_ptr<Brain> loadBrainState(const std::string& filepath);
    
    /**
     * @brief Save agent state to file
     * 
     * @param agent AgentBrain to save
     * @param world World state to save
     * @param filepath File path to save to
     * @return true if save successful, false otherwise
     */
    static bool saveAgentState(
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        const std::string& filepath);
    
    /**
     * @brief Load agent state from file
     * 
     * @param filepath File path to load from
     * @return Pair of (agent, world) loaded from file
     */
    static std::pair<std::shared_ptr<AgentBrain>, std::shared_ptr<SimpleWorld>> 
        loadAgentState(const std::string& filepath);
    
    /**
     * @brief Export brain state to JSON format
     * 
     * @param brain Brain to export
     * @param filepath File path to export to
     * @return true if export successful, false otherwise
     */
    static bool exportBrainToJSON(
        std::shared_ptr<Brain> brain,
        const std::string& filepath);
    
    /**
     * @brief Import brain state from JSON format
     * 
     * @param filepath File path to import from
     * @return Shared pointer to imported Brain object
     */
    static std::shared_ptr<Brain> importBrainFromJSON(const std::string& filepath);
};

/**
 * @brief Experiment utilities for running and managing experiments
 * 
 * Provides helper functions for running experiments and managing experiment data
 */
class ExperimentUtils {
public:
    /**
     * @brief Run a learning experiment with the given agent
     * 
     * @param agent AgentBrain to run experiment with
     * @param world World to interact with
     * @param numSteps Number of steps to run
     * @param experimentType Type of experiment to run
     * @return Experiment results
     */
    static std::map<std::string, double> runLearningExperiment(
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        size_t numSteps,
        const std::string& experimentType = "exploration");
    
    /**
     * @brief Run a comparison experiment with multiple configurations
     * 
     * @param configs Vector of configurations to compare
     * @param world World to use for all configurations
     * @param numSteps Number of steps to run for each configuration
     * @return Comparison results
     */
    static std::map<std::string, std::vector<double>> runComparisonExperiment(
        const std::vector<std::shared_ptr<Config>>& configs,
        std::shared_ptr<SimpleWorld> world,
        size_t numSteps);
    
    /**
     * @brief Run a developmental experiment
     * 
     * @param agent AgentBrain to develop
     * @param world World to interact with
     * @param numSteps Number of steps to run
     * @return Developmental progress metrics
     */
    static std::map<std::string, double> runDevelopmentalExperiment(
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        size_t numSteps);
    
    /**
     * @brief Run a social learning experiment
     * 
     * @param agent1 First agent
     * @param agent2 Second agent (observer)
     * @param world World to interact with
     * @param numSteps Number of steps to run
     * @return Social learning metrics
     */
    static std::map<std::string, double> runSocialLearningExperiment(
        std::shared_ptr<AgentBrain> agent1,
        std::shared_ptr<AgentBrain> agent2,
        std::shared_ptr<SimpleWorld> world,
        size_t numSteps);
};

/**
 * @brief Debug utilities for debugging and troubleshooting
 * 
 * Provides helper functions for debugging brain simulation and agent behavior
 */
class DebugUtils {
public:
    /**
     * @brief Debug brain internal state
     * 
     * @param brain Brain to debug
     * @return Vector of debug information strings
     */
    static std::vector<std::string> debugBrainState(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Debug agent behavior
     * 
     * @param agent AgentBrain to debug
     * @param world World to debug
     * @return Vector of debug information strings
     */
    static std::vector<std::string> debugAgentBehavior(
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world);
    
    /**
     * @brief Debug sensory input processing
     * 
     * @param percept Sensory percept to debug
     * @param agent AgentBrain processing the input
     * @return Vector of debug information strings
     */
    static std::vector<std::string> debugSensoryInput(
        const SensoryPercept& percept,
        std::shared_ptr<AgentBrain> agent);
    
    /**
     * @brief Debug motor command decoding
     * 
     * @param agent AgentBrain decoding motor commands
     * @return Vector of debug information strings
     */
    static std::vector<std::string> debugMotorDecoding(std::shared_ptr<AgentBrain> agent);
    
    /**
     * @brief Find and fix common issues
     * 
     * @param brain Brain to check for issues
     * @param world World to check for issues
     * @return Vector of suggested fixes
     */
    static std::vector<std::string> findAndFixIssues(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world);
};

/**
 * @brief Visualization utilities for creating visual representations
 * 
 * Provides helper functions for creating visualizations of brain state and behavior
 */
class VisualizationUtils {
public:
    /**
     * @brief Create a brain activity heatmap visualization
     * 
     * @param brain Brain to visualize
     * @param width Image width
     * @param height Image height
     * @return Vector of RGB pixel values (0-255)
     */
    static std::vector<uint8_t> createBrainActivityHeatmap(
        std::shared_ptr<Brain> brain,
        size_t width = 800,
        size_t height = 600);
    
    /**
     * @brief Create a 3D brain model visualization
     * 
     * @param brain Brain to visualize
     * @param resolution Visualization resolution
     * @return Vector of vertex data for 3D model
     */
    static std::vector<float> create3DBrainModel(
        std::shared_ptr<Brain> brain,
        float resolution = 1.0f);
    
    /**
     * @brief Create an agent world trajectory visualization
     * 
     * @param agent AgentBrain to visualize
     * @param world World to visualize
     * @param numSteps Number of steps to visualize
     * @return Vector of trajectory points (x, y, orientation)
     */
    static std::vector<std::tuple<float, float, float>> createWorldTrajectory(
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        size_t numSteps = 100);
    
    /**
     * @brief Create a neural connectivity visualization
     * 
     * @param brain Brain to visualize
     * @param includeWeights Whether to include synaptic weights in visualization
     * @return Vector of connection data (source, target, weight)
     */
    static std::vector<std::tuple<size_t, size_t, float>> createNeuralConnectivity(
        std::shared_ptr<Brain> brain,
        bool includeWeights = true);
    
    /**
     * @brief Export visualization data to file
     * 
     * @param data Visualization data
     * @param filepath File path to export to
     * @param format Output format
     * @return true if export successful, false otherwise
     */
    static bool exportVisualizationData(
        const std::vector<uint8_t>& data,
        const std::string& filepath,
        const std::string& format = "png");
};

/**
 * @brief Performance optimization utilities
 * 
 * Provides helper functions for optimizing brain simulation performance
 */
class PerformanceUtils {
public:
    /**
     * @brief Optimize brain configuration for performance
     * 
     * @param brain Brain to optimize
     * @param targetPerformance Target performance metrics
     * @return Optimized configuration
     */
    static std::shared_ptr<Config> optimizeBrainPerformance(
        std::shared_ptr<Brain> brain,
        const std::map<std::string, double>& targetPerformance);
    
    /**
     * @brief Profile brain simulation for bottlenecks
     * 
     * @param brain Brain to profile
     * @param numSteps Number of simulation steps
     * @param timestep Time step
     * @return Bottleneck analysis results
     */
    static std::map<std::string, double> profileForBottlenecks(
        std::shared_ptr<Brain> brain,
        size_t numSteps,
        double timestep);
    
    /**
     * @brief Optimize memory usage
     * 
     * @param brain Brain to optimize
     * @return Memory optimization suggestions
     */
    static std::vector<std::string> optimizeMemoryUsage(std::shared_ptr<Brain> brain);
    
    /**
     * @brief Benchmark different brain configurations
     * 
     * @param configs Vector of configurations to benchmark
     * @param numSteps Number of simulation steps
     * @param timestep Time step
     * @return Benchmark results
     */
    static std::map<std::string, std::vector<double>> benchmarkConfigurations(
        const std::vector<std::shared_ptr<Config>>& configs,
        size_t numSteps,
        double timestep);
    
    /**
     * @brief Create a performance report
     * 
     * @param brain Brain to analyze
     * @param numSteps Number of simulation steps
     * @param timestep Time step
     * @return Performance report
     */
    static std::map<std::string, double> createPerformanceReport(
        std::shared_ptr<Brain> brain,
        size_t numSteps,
        double timestep);
};

/**
 * @brief Educational utilities for learning and teaching
 * 
 * Provides helper functions for educational purposes and tutorials
 */
class EducationalUtils {
public:
    /**
     * @brief Create a tutorial brain configuration
     * 
     * @param tutorialType Type of tutorial
     * @return Tutorial configuration
     */
    static std::shared_ptr<Config> createTutorialConfiguration(
        const std::string& tutorialType = "basic");
    
    /**
     * @brief Get educational examples
     * 
     * @param exampleType Type of example
     * @return Example code and explanation
     */
    static std::map<std::string, std::string> getEducationalExamples(
        const std::string& exampleType = "basic");
    
    /**
     * @brief Create a learning exercise
     * 
     * @param exerciseType Type of exercise
     * @param config Configuration for the exercise
     * @return Exercise description and instructions
     */
    static std::map<std::string, std::string> createLearningExercise(
        const std::string& exerciseType,
        std::shared_ptr<Config> config);
    
    /**
     * @brief Get debugging help for common issues
     * 
     * @param issueType Type of issue
     * @return Debugging instructions
     */
    static std::vector<std::string> getDebuggingHelp(const std::string& issueType);
    
    /**
     * @brief Create a visualization tutorial
     * 
     * @param tutorialType Type of visualization tutorial
     * @return Tutorial data
     */
    static std::map<std::string, std::vector<float>> createVisualizationTutorial(
        const std::string& tutorialType = "basic");
};

/**
 * @brief Main utility class with all helper functions
 * 
 * This is the main utility class that provides access to all helper functions
 * organized by category.
 */
class Utils {
public:
    // Configuration utilities
    static ConfigurationUtils config;
    
    // Brain utilities
    static BrainUtils brain;
    
    // Agent utilities
    static AgentUtils agent;
    
    // Validation utilities
    static ValidationUtils validation;
    
    // Benchmarking utilities
    static BenchmarkingUtils benchmark;
    
    // Serialization utilities
    static SerializationUtils serialization;
    
    // Experiment utilities
    static ExperimentUtils experiment;
    
    // Debug utilities
    static DebugUtils debug;
    
    // Visualization utilities
    static VisualizationUtils visualization;
    
    // Performance utilities
    static PerformanceUtils performance;
    
    // Educational utilities
    static EducationalUtils educational;
    
    /**
     * @brief Get all available utility categories
     * 
     * @return Vector of utility category names
     */
    static std::vector<std::string> getAvailableUtilities();
    
    /**
     * @brief Get utility help information
     * 
     * @param utilityName Name of utility to get help for
     * @return Help information for the utility
     */
    static std::string getUtilityHelp(const std::string& utilityName);
    
    /**
     * @brief Check if a utility is available
     * 
     * @param utilityName Name of utility to check
     * @return true if utility is available, false otherwise
     */
    static bool isUtilityAvailable(const std::string& utilityName);
};
