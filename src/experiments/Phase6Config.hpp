# Phase 6 Configuration Constants

/**
 * Phase 6 configuration constants
 * 
 * These constants define the default values and ranges for the Phase 6
 * integration experiment. They can be overridden by command-line arguments
 * or configuration files.
 */

#pragma once

#include <cstdint>
#include <string>

namespace nlm {

namespace Phase6Constants {
    // Neuron and network parameters
    constexpr uint64_t DEFAULT_NEURON_COUNT = 1000;
    constexpr uint64_t MIN_NEURON_COUNT = 100;
    constexpr uint64_t MAX_NEURON_COUNT = 10000;
    constexpr size_t DEFAULT_REGION_COUNT = 1;
    constexpr size_t MIN_REGION_COUNT = 1;
    constexpr size_t MAX_REGION_COUNT = 10;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    constexpr float MIN_CONNECTION_PROBABILITY = 0.01f;
    constexpr float MAX_CONNECTION_PROBABILITY = 0.5f;
    
    // Simulation parameters
    constexpr uint64_t DEFAULT_MAX_STEPS = 10000;
    constexpr uint64_t MIN_MAX_STEPS = 1000;
    constexpr uint64_t MAX_MAX_STEPS = 100000;
    constexpr double DEFAULT_TIMESTEP = 0.001;
    constexpr double MIN_TIMESTEP = 0.0001;
    constexpr double MAX_TIMESTEP = 0.1;
    constexpr uint64_t DEFAULT_WARMUP_STEPS = 100;
    
    // Plasticity parameters
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
    constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f;
    constexpr float MIN_STDP_WEIGHT = 0.001f;
    constexpr float MAX_STDP_WEIGHT = 0.1f;
    constexpr float MIN_STDP_TIME_CONSTANT = 1.0f;
    constexpr float MAX_STDP_TIME_CONSTANT = 100.0f;
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Neuromodulation parameters
    constexpr float DEFAULT_DOPAMINE_BASELINE = 0.1f;
    constexpr float MIN_DOPAMINE_BASELINE = 0.0f;
    constexpr float MAX_DOPAMINE_BASELINE = 1.0f;
    constexpr float DEFAULT_NOVELTY_THRESHOLD = 0.01f;
    constexpr float MIN_NOVELTY_THRESHOLD = 0.001f;
    constexpr float MAX_NOVELTY_THRESHOLD = 0.1f;
    constexpr float DEFAULT_CURIOUSITY_LEVEL = 0.5f;
    constexpr float MIN_CURIOUSITY_LEVEL = 0.0f;
    constexpr float MAX_CURIOUSITY_LEVEL = 1.0f;
    
    // Reward and motivation parameters
    constexpr float DEFAULT_REWARD_SCALE = 1.0f;
    constexpr float MIN_REWARD_SCALE = 0.1f;
    constexpr float MAX_REWARD_SCALE = 10.0f;
    constexpr float DEFAULT_EXPLORATION_RATE = 0.1f;
    constexpr float MIN_EXPLORATION_RATE = 0.0f;
    constexpr float MAX_EXPLORATION_RATE = 1.0f;
    constexpr float DEFAULT_REWARD_DISCOUNT_FACTOR = 0.99f;
    
    // File and path parameters
    constexpr const char* DEFAULT_CHECKPOINT_PATH = "./checkpoint.bin";
    constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
    constexpr const char* DEFAULT_OUTPUT_FILE = ""; // No output by default
    constexpr size_t MAX_CHECKPOINT_FILENAME_LENGTH = 256;
    
    // Performance and monitoring parameters
    constexpr size_t DEFAULT_PERFORMANCE_SAMPLE_INTERVAL = 1000;
    constexpr double MAX_WALL_CLOCK_TIME = 3600.0; // 1 hour maximum
    
    // Validation patterns
    constexpr const char* VALIDATION_PATTERN_FLOAT = "^[0-9]+(\\.[0-9]+)?$";
    constexpr const char* VALIDATION_PATTERN_INT = "^[0-9]+$";
    constexpr const char* VALIDATION_PATTERN_BOOL = "^(true|false)$";
    constexpr const char* VALIDATION_PATTERN_STRING = ".*";
    constexpr const char* VALIDATION_PATTERN_NON_EMPTY = ".+";
    
    // Logging and verbosity levels
    constexpr const char* DEFAULT_LOG_LEVEL = "INFO";
    constexpr size_t MAX_LOG_HISTORY = 1000;
    
    // Development stages
    constexpr float DEVELOPMENT_INITIAL_STAGE = 1.0f;
    constexpr float DEVELOPMENT_CRITICAL_PERIOD_END = 100.0f;
    constexpr float DEVELOPMENT_MATURATION_START = 1000.0f;
    constexpr float DEVELOPMENT_ADULT_STAGE = 5000.0f;
    
    // Memory parameters
    constexpr size_t MAX_MEMORY_TRACES = 10000;
    constexpr size_t MAX_EPISODIC_EPISODES = 1000;
    constexpr float MEMORY_TRACE_DECAY_RATE = 0.99f;
}

/**
 * Advanced configuration options for Phase 6
 * 
 * This structure provides advanced configuration options for the Phase 6
 * integration experiment, including performance tuning and debugging
 * options.
 */
struct AdvancedPhase6Config {
    // Performance tuning
    bool enableSIMD = true;
    bool enableParallel = true;
    size_t parallelThreads = std::thread::hardware_concurrency();
    bool enableMemoryPools = true;
    
    // Debugging and profiling
    bool enableProfiling = false;
    std::string profileOutputFile = "profile.json";
    bool enableDebugLogging = false;
    std::string debugLogFile = "debug.log";
    
    // Experimental parameters
    bool randomizeInitialWeights = false;
    float weightInitMean = 0.5f;
    float weightInitStd = 0.1f;
    bool enableNoise = false;
    float noiseLevel = 0.01f;
    
    // Environment and world parameters
    std::string worldType = "SimpleWorld";
    uint64_t worldWidth = 20;
    uint64_t worldHeight = 20;
    uint64_t visionWidth = 8;
    uint64_t visionHeight = 8;
    
    // Agent behavior parameters
    float agentSpeed = 1.0f;
    float agentEnergyCapacity = 100.0f;
    float agentEnergyConsumption = 0.1f;
    
    // Learning and adaptation parameters
    float learningRate = 0.01f;
    float discountFactor = 0.99f;
    float explorationEpsilon = 0.1f;
    float temperature = 1.0f;
    
    // Validation and constraints
    bool validateConfiguration = true;
    bool strictValidation = false;
    std::vector<std::string> requiredKeys;
    
    AdvancedPhase6Config() = default;
};

/**
 * Configuration validator for Phase 6
 * 
 * This class provides validation for Phase 6 configuration parameters,
 * ensuring that values are within acceptable ranges and meet validation
 * criteria.
 */
class Phase6ConfigValidator {
public:
    Phase6ConfigValidator();
    ~Phase6ConfigValidator();
    
    // Validate configuration
    bool validate(const Phase6Config& config) const;
    bool validate(const AdvancedPhase6Config& config) const;
    
    // Validate specific parameter
    bool validateParameter(const std::string& key, const std::string& value) const;
    
    // Add custom validator
    void addValidator(const std::string& key, 
                     std::function<bool(const std::string&)> validator);
    
    // Get validation error messages
    std::vector<std::string> getValidationErrors() const { return validationErrors; }
    
    // Clear validation errors
    void clearErrors() { validationErrors.clear(); }
    
    // Set strict validation mode
    void setStrictMode(bool strict) { strictMode = strict; }
    
    // Get strict mode
    bool isStrictMode() const { return strictMode; }

private:
    std::vector<std::string> validationErrors;
    std::map<std::string, std::function<bool(const std::string&)>> validators;
    bool strictMode = false;
    
    // Built-in validators
    static bool validateNeuronCount(const std::string& value);
    static bool validateMaxSteps(const std::string& value);
    static bool validateTimestep(const std::string& value);
    static bool validateConnectionProbability(const std::string& value);
    static bool validateSeed(const std::string& value);
    static bool validateCheckpointPath(const std::string& value);
    static bool validateNoveltyThreshold(const std::string& value);
    static bool validateCuriosityLevel(const std::string& value);
    static bool validateRewardScale(const std::string& value);
    static bool validateExplorationRate(const std::string& value);
};

/**
 * Configuration merger for Phase 6
 * 
 * This class provides functionality to merge multiple configuration sources,
 * with proper precedence handling and conflict resolution.
 */
class Phase6ConfigMerger {
public:
    Phase6ConfigMerger();
    ~Phase6ConfigMerger();
    
    // Merge configurations
    Phase6Config merge(const Phase6Config& base,
                     const AdvancedPhase6Config& advanced,
                     const std::map<std::string, std::string>& commandLine) const;
    
    // Get configuration precedence
    enum class Precedence {
        Default,      // Hardcoded defaults
        CommandLine,  // Command-line arguments
        Runtime,      // Runtime settings
        File          // Configuration file
    };
    
    // Resolve conflicts between configurations
    std::pair<std::string, std::string> resolveConflict(
        const std::string& key,
        const std::string& value1,
        Precedence source1,
        const std::string& value2,
        Precedence source2) const;
    
    // Get final value for key
    std::string getFinalValue(const std::string& key,
                             const Phase6Config& base,
                             const std::map<std::string, std::string>& commandLine) const;
    
    // Get source for key
    Precedence getSource(const std::string& key,
                        const Phase6Config& base,
                        const std::map<std::string, std::string>& commandLine) const;

private:
    // Precedence order: CommandLine > File > Runtime > Default
    static const std::map<std::string, Precedence> defaultPrecedence;
    
    // Validation function
    static bool isValidValue(const std::string& key, const std::string& value);
};

} // namespace nlm
