#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <sstream>

namespace nlm {

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult(bool v = true) : valid(v) {}
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// Configuration constraint for validation
struct ConfigConstraint {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    
    ConfigConstraint(const std::string& k, 
                     std::function<bool(const ConfigValue&)> val,
                     const std::string& msg)
        : key(k), validator(val), errorMessage(msg) {}
};

// Configuration metadata for documentation
struct ConfigMetadata {
    std::string name;
    std::string description;
    std::string type;
    std::string defaultValue;
    std::string category;
    std::vector<std::string> allowedValues;
    std::pair<double, double> range;  // for numeric types
    bool required;
    
    ConfigMetadata(const std::string& n, const std::string& desc)
        : name(n), description(desc), type("unknown"), defaultValue(""),
          category("general"), required(false) {}
};

// Enhanced configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from string (for testing)
    bool loadFromString(const std::string& configString);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get raw ConfigValue
    std::optional<ConfigValue> getRaw(const std::string& key) const;
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    // Typed setter methods
    bool setString(const std::string& key, const std::string& value,
                   ConfigSource source = ConfigSource::Runtime,
                   const std::string& description = "");
    bool setInt(const std::string& key, int value,
                ConfigSource source = ConfigSource::Runtime,
                const std::string& description = "");
    bool setDouble(const std::string& key, double value,
                   ConfigSource source = ConfigSource::Runtime,
                   const std::string& description = "");
    bool setBool(const std::string& key, bool value,
                 ConfigSource source = ConfigSource::Runtime,
                 const std::string& description = "");
    bool setStringVector(const std::string& key, const std::vector<std::string>& values,
                        ConfigSource source = ConfigSource::Runtime,
                        const std::string& description = "");
    
    // Constraint and validation
    void addConstraint(const ConfigConstraint& constraint);
    void addMetadata(const std::string& key, const ConfigMetadata& metadata);
    ValidationResult validate() const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by source
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Get source for a key
    ConfigSource getSource(const std::string& key) const;
    
    // Get description for a key
    std::string getDescription(const std::string& key) const;
    
    // Get metadata for a key
    std::optional<ConfigMetadata> getMetadata(const std::string& key) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration as JSON (for export)
    std::string toJSON() const;
    
    // Import configuration from JSON
    bool fromJSON(const std::string& json);
    
    // Merge with another config
    void merge(const Config& other);
    
    // Validation helpers
    static bool isNumeric(const ConfigValue& value);
    static bool isString(const ConfigValue& value);
    static bool isBoolean(const ConfigValue& value);
    static bool isVector(const ConfigValue& value);
    
    // Type conversion helpers
    template<typename T>
    static T convertToType(const ConfigValue& value);
    
    // Parse command line arguments with better support
    struct ParsedArgs {
        std::vector<std::string> unknownArgs;
        std::vector<std::pair<std::string, std::string>> knownArgs;
        std::vector<std::pair<std::string, std::string>> flags;
    };
    
    static ParsedArgs parseArgs(int argc, char** argv,
                                const std::vector<std::string>& knownKeys = {});

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toJsonString(const std::string& str);
    static std::string escapeJsonString(const std::string& str);
    
    // Validation helpers
    bool validateValue(const ConfigValue& value, const std::string& key) const;
    bool validateConstraints(const std::string& key, const ConfigValue& value) const;
    
    // Type conversion
    template<typename T>
    static bool convertValue(const ConfigValue& from, T& to);
};

// Helper functions for configuration
namespace ConfigHelpers {
    // Common configuration keys used throughout NLM
    struct CommonKeys {
        static const std::string RANDOM_SEED;
        static const std::string SIMULATION_TIMESTEP;
        static const std::string NEURON_COUNT;
        static const std::string REGION_COUNT;
        static const std::string CONNECTION_PROBABILITY;
        static const std::string STDp_LTP_WEIGHT;
        static const std::string STDp_LTD_WEIGHT;
        static const std::string STDp_TAU;
        static const std::string SYNAPTOGENESIS_RATE;
        static const std::string PRUNING_RATE;
        static const std::string MAX_ENERGY;
        static const std::string ENERGY_DECAY_RATE;
        static const std::string WORLD_WIDTH;
        static const std::string WORLD_HEIGHT;
        static const std::string VISION_WIDTH;
        static const std::string VISION_HEIGHT;
        static const std::string AUDIO_ENABLED;
        static const std::string AUDIO_SAMPLE_RATE;
        static const std::string ENABLE_DEBUG_LOGGING;
        static const std::string ENABLE_PERFORMANCE_MEASURING;
        static const std::string CHECKPOINT_INTERVAL;
        static const std::string MAX_EPISODES;
        static const std::string MAX_STEPS_PER_EPISODE;
        static const std::string REWARD_SCALE;
        static const std::string NOISE_LEVEL;
    };
    
    // Common configuration validation functions
    bool isPositive(const ConfigValue& value);
    bool isNonNegative(const ConfigValue& value);
    bool isNonZero(const ConfigValue& value);
    bool isInRange(const ConfigValue& value, double min, double max);
    bool isValidSeed(const ConfigValue& value);
    
    // Get default configuration
    std::unordered_map<std::string, ConfigValue> getDefaultConfig();
    
    // Get configuration metadata
    std::unordered_map<std::string, ConfigMetadata> getConfigMetadata();
}

} // namespace nlm