#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"

namespace nlm {

// Logger initialization helpers

/**
 * Initialize global logger with standard configuration
 * 
 * @param level Log level (default: Info)
 * @param addConsole Whether to add console logger (default: true)
 * 
 * This function encapsulates the common logger initialization pattern
 * found in main.cpp, benchmark.cpp, and other files:
 *   auto logger = std::make_shared<Logger>();
 *   auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
 *   logger->addLogger(consoleLogger);
 *   Logger::setGlobal(logger);
 */
void initializeGlobalLogger(LogLevel level = LogLevel::Info, bool addConsole = true);

// Configuration helper functions

/**
 * Create standard brain configuration with validation
 * 
 * @param neuronCount Number of neurons (default: 1000)
 * @param regionCount Number of regions (default: 1)
 * @param connectionProbability Connection probability (default: 0.1)
 * @param stdpLtpWeight STDP LTP weight (default: 0.01)
 * @param stdpLtdWeight STDP LTD weight (default: 0.012)
 * @return Configured brain configuration
 * 
 * This function encapsulates the common configuration pattern
 * found in Brain.cpp, benchmark.cpp, and other files:
 *   auto config = std::make_shared<Config>();
 *   config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
 *   config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
 *   config->set("connection_probability", connectionProbability, ConfigSource::Default);
 *   config->set("stdp_ltp_weight", stdpLtpWeight, ConfigSource::Default);
 *   config->set("stdp_ltd_weight", stdpLtdWeight, ConfigSource::Default);
 */
std::shared_ptr<Config> createStandardBrainConfig(
    size_t neuronCount = 1000,
    size_t regionCount = 1,
    float connectionProbability = 0.1f,
    float stdpLtpWeight = 0.01f,
    float stdpLtdWeight = 0.012f
);

/**
 * Set standard configuration parameters on existing config object
 * 
 * @param config Configuration object to modify
 * @param neuronCount Number of neurons
 * @param regionCount Number of regions
 * @param connectionProbability Connection probability
 * 
 * This function provides a convenient wrapper for setting
 * common configuration parameters.
 */
void setStandardConfigParameters(
    const std::shared_ptr<Config>& config,
    size_t neuronCount = 1000,
    size_t regionCount = 1,
    float connectionProbability = 0.1f
);

/**
 * Create brain with standard configuration and initialize it
 * 
 * @param neuronCount Number of neurons (default: 1000)
 * @param regionCount Number of regions (default: 1)
 * @param connectionProbability Connection probability (default: 0.1)
 * @return Initialized brain
 * 
 * This function encapsulates the common brain creation pattern
 * found in Brain.cpp and benchmark.cpp:
 *   auto config = createStandardBrainConfig(neuronCount, regionCount, connectionProbability);
 *   auto brain = std::make_shared<Brain>(config);
 *   brain->initialize();
 *   return brain;
 */
std::shared_ptr<Brain> createBrainWithConfig(
    size_t neuronCount = 1000,
    size_t regionCount = 1,
    float connectionProbability = 0.1f
);

/**
 * Create regions with standard population distribution
 * 
 * @param brain Brain to add regions to
 * @param neuronCount Total number of neurons
 * @param regionCount Number of regions
 * @param sensoryProportion Proportion of neurons dedicated to sensory (default: 0.25)
 * @param motorProportion Proportion of neurons dedicated to motor (default: 0.25)
 * @param internalProportion Proportion of neurons dedicated to internal (default: 0.5)
 * 
 * This function encapsulates the common region creation pattern
 * found in Brain.cpp:
 *   size_t neuronsPerRegion = neuronCount / regionCount;
 *   for (size_t i = 0; i < regionCount; ++i) {
 *       addRegion("Region_" + std::to_string(i + 1));
 *       // Create populations with standard distribution
 *   }
 */
void createRegionsWithPopulations(
    const std::shared_ptr<Brain>& brain,
    size_t neuronCount,
    size_t regionCount,
    float sensoryProportion = 0.25f,
    float motorProportion = 0.25f,
    float internalProportion = 0.5f
);

/**
 * Run simple simulation with given brain and step count
 * 
 * @param brain Brain to simulate
 * @param steps Number of simulation steps
 * @return Benchmark result containing performance metrics
 * 
 * This function encapsulates common simulation patterns
 * found in benchmark.cpp and other files.
 */
BenchmarkResult runSimpleSimulation(const std::shared_ptr<Brain>& brain, size_t steps);

/**
 * Create neural network with standard configuration and run simulation
 * 
 * @param neuronCount Number of neurons to simulate
 * @param steps Number of simulation steps
 * @param regionCount Number of brain regions (default: 1)
 * @return Benchmark result containing performance metrics
 * 
 * This function combines configuration, brain creation, and simulation
 * into a single convenient interface for benchmarking.
 */
BenchmarkResult runBenchmark(size_t neuronCount, size_t steps, size_t regionCount = 1);

/**
 * Initialize brain with validation and logging
 * 
 * @param brain Brain to initialize
 * @param logPrefix Log prefix for status messages (default: "Brain")
 * 
 * This function wraps brain initialization with proper logging
 * and error handling.
 */
void initializeBrain(const std::shared_ptr<Brain>& brain, const std::string& logPrefix = "Brain");

/**
 * Create and initialize complete neural network for experiment
 * 
 * @param config Configuration to use
 * @param addDevelopment Whether to enable development (default: false)
 * @param addPlasticity Whether to enable plasticity (default: true)
 * @param addNeuromodulation Whether to enable neuromodulation (default: true)
 * @return Initialized brain
 * 
 * This function provides a convenient way to create a complete
 * neural network with all features enabled for experimentation.
 */
std::shared_ptr<Brain> createCompleteBrain(
    const std::shared_ptr<Config>& config,
    bool addDevelopment = false,
    bool addPlasticity = true,
    bool addNeuromodulation = true
);

/**
 * Get standard benchmark parameters for given neuron count
 * 
 * @param neuronCount Number of neurons
 * @return Struct containing benchmark parameters
 */
struct BenchmarkParameters {
    size_t neuronCount;
    size_t regionCount;
    size_t steps;
    float connectionProbability;
    float stdpLtpWeight;
    float stdpLtdWeight;
};

BenchmarkParameters getBenchmarkParameters(size_t neuronCount);

} // namespace nlm