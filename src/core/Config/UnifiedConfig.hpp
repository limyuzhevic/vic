// NLM Unified Configuration System
// 
// This file defines a unified configuration system that provides:
// - Standardized configuration parameters across all phases
// - Parameter validation and bounds checking
// - Easy access to common configuration values
// - Support for phase-specific configuration extensions
//
// Version: 1.0.0

#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <limits>
#include <cassert>

namespace nlm {

// Configuration parameter categories
enum class ConfigCategory {
    Core,              // Essential parameters for all phases
    Neural,            // Neural network parameters
    Plasticity,        // Plasticity and learning parameters
    Environment,       // Environment and simulation parameters
    PhaseSpecific,     // Phase-specific parameters
    Advanced           // Advanced/experimental parameters
};

// Configuration parameter definition
struct ConfigParameter {
    std::string name;
    ConfigCategory category;
    std::string type;          // "int", "int64", "double", "float", "bool", "string"
    std::string description;
    double defaultValue;        // As double for easy comparison
    double minValue;           // Minimum allowed value
    double maxValue;           // Maximum allowed value
    bool required;             // Whether this parameter must be set
    bool phaseSpecific;        // Can have different values per phase
};

// Unified configuration manager
class UnifiedConfig {
public:
    // Parameter validation exceptions
    class ConfigValidationException : public std::runtime_error {
    public:
        explicit ConfigValidationException(const std::string& message) 
            : std::runtime_error("Configuration validation error: " + message) {}
    };
    
    class ConfigParameterError : public ConfigValidationException {
    public:
        explicit ConfigParameterError(const std::string& param, const std::string& message)
            : ConfigValidationException("Parameter '" + param + "': " + message) {}
    };

    UnifiedConfig() = default;
    ~UnifiedConfig() = default;
    
    // Disable copying
    UnifiedConfig(const UnifiedConfig&) = delete;
    UnifiedConfig& operator=(const UnifiedConfig&) = delete;
    
    // Enable moving
    UnifiedConfig(UnifiedConfig&&) noexcept = default;
    UnifiedConfig& operator=(UnifiedConfig&&) noexcept = default;
    
    // Initialize with configuration profile
    bool initialize(const std::string& profileName = "default");
    
    // Get parameter value with type safety
    template<typename T>
    T get(const std::string& paramName, const T& defaultValue = T{}) const;
    
    // Set parameter value with validation
    template<typename T>
    void set(const std::string& paramName, const T& value, 
             ConfigCategory category = ConfigCategory::PhaseSpecific);
    
    // Check if parameter exists and is valid
    bool has(const std::string& paramName) const;
    
    // Get parameter metadata
    const ConfigParameter* getParameterInfo(const std::string& paramName) const;
    
    // Get all parameters in a category
    std::vector<std::string> getParametersInCategory(ConfigCategory category) const;
    
    // Validate all parameters
    bool validateAll() const;
    
    // Load from configuration file
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Convert to legacy Config format for backward compatibility
    void convertToLegacyConfig(class Config& legacyConfig) const;
    
    // Configuration profiles
    static const std::unordered_map<std::string, std::map<std::string, double>> getDefaultProfiles();
    
    // Get parameter type name for templated access
    template<typename T>
    static std::string getTypeName();
    
    // Get all available parameter names
    std::vector<std::string> getAllParameterNames() const;
    
    // Reset to default values
    void resetToDefaults();
    
    // Merge with another configuration
    void merge(const UnifiedConfig& other);

private:
    // Parameter validation helpers
    template<typename T>
    void validateParameter(const std::string& paramName, const T& value) const;
    
    template<typename T>
    T convertToType(const std::string& paramName) const;
    
    // Parameter registry
    static std::unordered_map<std::string, ConfigParameter> parameterRegistry_;
    
    // Current configuration values
    std::map<std::string, double> currentValues_;
    
    // Parameter constraints
    std::map<std::string, std::pair<double, double>> valueRanges_;
    
    // Error handling
    void throwParameterError(const std::string& paramName, const std::string& error) const;
    
    // Parameter type conversion
    template<typename T>
    static T stringToValue(const std::string& str);
    
    template<typename T>
    static std::string valueToString(const T& value);
};

// Legacy compatibility functions
inline void convertUnifiedToLegacy(const UnifiedConfig& unified, class Config& legacy) {
    unified.convertToLegacyConfig(legacy);
}

inline bool convertLegacyToUnified(const class Config& legacy, UnifiedConfig& unified) {
    // Create a temporary UnifiedConfig and populate it from legacy
    UnifiedConfig tempConfig;
    tempConfig.convertFromLegacy(legacy);
    
    // Deep copy to target
    unified = std::move(tempConfig);
    return true;
}

// Configuration profiles for different use cases
struct ConfigProfiles {
    static const std::string getQuickTestProfile();     // Fast tests with minimal complexity
    static const std::string getRealTimeProfile();       // Optimized for real-time performance
    static const std::string getResearchProfile();       // Maximum realism for research
    static const std::string getDevelopmentProfile();   // Balanced settings for development
    static const std::string getProductionProfile();     // Stable, production-ready settings
};

// Common configuration parameter names (defined in unified_config.cpp)
namespace ConfigParameters {
    // Core parameters
    constexpr const char* NEURON_COUNT = "neuron_count";
    constexpr const char* REGION_COUNT = "region_count";
    constexpr const char* CONNECTION_PROBABILITY = "connection_probability";
    constexpr const char* RANDOM_SEED = "random_seed";
    constexpr const char* SIMULATION_TIMESTEP = "simulation_timestep";
    
    // Neural parameters
    constexpr const char* LIF_THRESHOLD = "lif_threshold";
    constexpr const char* LIF_RESTING_POTENTIAL = "lif_resting_potential";
    constexpr const char* LIF_TIME_CONSTANT = "lif_time_constant";
    constexpr const char* MEMBRANE_CAPACITANCE = "membrane_capacitance";
    
    // Plasticity parameters
    constexpr const char* STDP_LTP_WEIGHT = "stdp_ltp_weight";
    constexpr const char* STDP_LTD_WEIGHT = "stdp_ltd_weight";
    constexpr const char* STDP_TAU = "stdp_tau";
    constexpr const char* HEBBIAN_LEARNING_RATE = "hebbian_learning_rate";
    constexpr const char* SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
    constexpr const char* PRUNING_RATE = "pruning_rate";
    
    // Environment parameters
    constexpr const char* ENVIRONMENT_WIDTH = "environment_width";
    constexpr const char* ENVIRONMENT_HEIGHT = "environment_height";
    constexpr const char* VISION_WIDTH = "vision_width";
    constexpr const char* VISION_HEIGHT = "vision_height";
    
    // Performance parameters
    constexpr const char* MAX_SPIKE_HISTORY = "max_spike_history";
    constexpr const char* NUM_THREADS = "num_threads";
    constexpr const char* ENABLE_VECTORIZATION = "enable_vectorization";
    
    // Advanced parameters
    constexpr const char* ENABLE_DEVELOPMENT = "enable_development";
    constexpr const char* ENABLE_REWARD_MODULATION = "enable_reward_modulation";
    constexpr const char* ENABLE_CURIOUSITY = "enable_curiosity";
}

// Configuration validation utilities
class ConfigValidator {
public:
    static bool isValidNeuronCount(double value);
    static bool isValidRegionCount(double value);
    static bool isValidConnectionProbability(double value);
    static bool isValidRandomSeed(double value);
    static bool isValidSimulationTimestep(double value);
    static bool isValidPlasticityWeight(double value);
    static bool isValidPlasticityTau(double value);
    static bool isValidSynapticRate(double value);
    static bool isValidEnvironmentDimension(double value);
    static bool isValidVisionDimension(double value);
    static bool isValidSpikeHistorySize(double value);
    
    // Generic validation
    template<typename T>
    static bool isValidValue(double value, double min = std::numeric_limits<double>::lowest(), 
                            double max = std::numeric_limits<double>::max());
    
    // Get error message for invalid value
    static std::string getValidationError(const std::string& paramName, double value);
};

// Configuration convenience functions
inline void setupDefaultConfig(UnifiedConfig& config) {
    config.initialize("default");
}

inline void setupQuickTestConfig(UnifiedConfig& config) {
    config.initialize("quick_test");
}

inline void setupResearchConfig(UnifiedConfig& config) {
    config.initialize("research");
}

// Configuration compatibility macros
#define UNIFIED_CONFIG_VALIDATION(config) \
    do { \
        if (!config.validateAll()) { \
            throw std::runtime_error("Unified configuration validation failed"); \
        } \
    } while(0)

#define UNIFIED_CONFIG_GET_OR(config, param, default) (config.has(param) ? config.get<__typeof(default)>(param, default) : default)

} // namespace nlm
