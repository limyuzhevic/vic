#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <regex>
#include <stdexcept>
#include <typeindex>

namespace nlm {

// Forward declarations
class Config;

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
    Runtime,
    UserOverride
};

// Configuration parameter metadata
struct ConfigParameter {
    std::string key;
    std::string description;
    std::string category;
    std::string dataType;
    ConfigValue defaultValue;
    bool required = false;
    bool editable = true;
    
    // Validation rules
    struct ValidationRule {
        enum Type {
            Range,           // For numeric types
            Length,          // For strings and vectors
            Pattern,         // For strings
            OneOf,           // For any type
            Custom          // Custom validation function
        };
        
        Type type;
        std::string errorMessage;
        
        // Rule-specific data
        union RuleData {
            struct RangeData {
                double min;
                double max;
                bool inclusive = true;
            } range;
            
            struct LengthData {
                size_t minLength;
                size_t maxLength;
                bool inclusive = true;
            } length;
            
            struct PatternData {
                std::string pattern;
                std::regex::flags_type flags = std::regex::ECMAScript;
            } pattern;
            
            struct OneOfData {
                std::vector<ConfigValue> allowedValues;
            } oneOf;
            
            struct CustomData {
                std::function<bool(const ConfigValue&)> validator;
            } custom;
            
            RuleData() {}
            ~RuleData() {}
            
            RuleData(const RangeData& r) : range(r) {}
            RuleData(const LengthData& l) : length(l) {}
            RuleData(const PatternData& p) : pattern(p) {}
            RuleData(const OneOfData& o) : oneOf(o) {}
            RuleData(const CustomData& c) : custom(c) {}
        } data;
        
        RuleData* getDataPtr() { return &data; }
        const RuleData* getDataPtr() const { return &data; }
    };
    
    std::vector<ValidationRule> validationRules;
    
    // Unit and scaling information
    std::string unit;
    double scale = 1.0;
    
    // Dependency information
    std::vector<std::string> dependencies;
    std::function<bool(const ConfigValue&)> dependencyValidator;
    
    ConfigParameter() = default;
    ConfigParameter(const std::string& k, const ConfigValue& v, const std::string& desc = "",
                   const std::string& cat = "general", const std::string& type = "auto")
        : key(k), description(desc), category(cat), dataType(type), defaultValue(v), required(false), editable(true) {}
};

// Validation exception
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& msg, const std::string& key = "")
        : std::runtime_error("Configuration validation error" + (key.empty() ? "" : " for key: " + key) + ": " + msg) {}
};

// Configuration change listener
class ConfigChangeListener {
public:
    virtual ~ConfigChangeListener() = default;
    virtual void onConfigChanged(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) = 0;
};

// Main configuration class with advanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Advanced loading with schema validation
    bool loadFromFile(const std::string& filepath, bool validate = true);
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    
    // Save to file with formatting options
    bool saveToFile(const std::string& filepath, const std::string& format = "simple") const;
    
    // Get values with type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation and change notification
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced type conversion
    bool setWithType(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with statistics
    std::string summary() const;
    std::string summaryDetailed() const;
    
    // Validation methods
    bool validate() const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration schema management
    void registerParameter(const ConfigParameter& param);
    bool hasParameter(const std::string& key) const;
    std::optional<ConfigParameter> getParameter(const std::string& key) const;
    std::vector<ConfigParameter> getAllParameters() const;
    std::vector<std::string> getParameterCategories() const;
    
    // Parameter validation
    bool validateParameter(const ConfigParameter& param) const;
    std::string validateParameter(const ConfigParameter& param, const ConfigValue& value) const;
    
    // Configuration presets
    static std::unordered_map<std::string, std::shared_ptr<Config>> getPresets();
    bool applyPreset(const std::string& presetName);
    
    // Value conversion and type checking
    static std::string valueToString(const ConfigValue& value);
    static bool stringToValue(const std::string& str, ConfigValue& value, const std::string& targetType = "auto");
    
    // Configuration comparison
    bool equals(const Config& other) const;
    size_t hash() const;
    
    // File format utilities
    static std::string exportToJSON() const;
    static std::string exportToINI() const;
    static std::string exportToYAML() const;
    static std::shared_ptr<Config> importFromJSON(const std::string& json);
    static std::shared_ptr<Config> importFromINI(const std::string& ini);
    static std::shared_ptr<Config> importFromYAML(const std::string& yaml);
    
    // Change tracking
    void addChangeListener(std::shared_ptr<ConfigChangeListener> listener);
    void removeChangeListener(std::shared_ptr<ConfigChangeListener> listener);
    void notifyChangeListeners(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getChangedEntryCount() const;
    std::vector<std::string> getChangedKeys() const;
    std::chrono::system_clock::time_point getLastModified() const;
    
    // Advanced search and filtering
    std::vector<std::string> findKeysByPattern(const std::string& pattern) const;
    std::vector<std::string> findKeysByCategory(const std::string& category) const;
    std::unordered_map<std::string, ConfigValue> findByPrefix(const std::string& prefix) const;
    
    // Configuration import/export
    std::string exportToString(const std::string& format = "summary") const;
    bool importFromString(const std::string& str, const std::string& format = "simple");
    
    // Batch operations
    void batchSet(const std::unordered_map<std::string, ConfigValue>& values);
    void batchSet(const std::vector<std::pair<std::string, ConfigValue>>& values);
    std::unordered_map<std::string, ConfigValue> getBatch(const std::vector<std::string>& keys) const;
    
    // Configuration consistency checking
    bool checkConsistency() const;
    std::vector<std::string> getConsistencyErrors() const;
    
    // Configuration merging
    void merge(const Config& other, bool overwrite = true);
    void mergeFromString(const std::string& str, const std::string& format = "simple");
    
    // Configuration snapshot
    std::string createSnapshot() const;
    void restoreFromSnapshot(const std::string& snapshot);
    
    // Configuration diff
    std::string diff(const Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToInt(const std::string& str, int& value);
    static bool stringToInt64(const std::string& str, int64_t& value);
    static bool stringToDouble(const std::string& str, double& value);
    static bool stringToBool(const std::string& str, bool& value);
    
    // Validation helpers
    static bool validateRange(const ConfigValue& value, double min, double max, bool inclusive);
    static bool validateLength(const ConfigValue& value, size_t minLen, size_t maxLen, bool inclusive);
    static bool validatePattern(const ConfigValue& value, const std::string& pattern, std::regex::flags_type flags);
    static bool validateOneOf(const ConfigValue& value, const std::vector<ConfigValue>& allowed);
    
    // Change notification helpers
    void notifyListeners(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue);
    
    // Schema validation helpers
    bool validateAgainstSchema() const;
    bool validateParameterConsistency() const;
    
    // Preset definitions
    static std::unordered_map<std::string, std::shared_ptr<Config>> initializePresets();
    
    // Statistics helpers
    size_t calculateChangedEntryCount() const;
    std::chrono::system_clock::time_point calculateLastModified() const;
    
    // Comparison helpers
    bool compareValues(const ConfigValue& val1, const ConfigValue& val2) const;
    
    // Batch operation helpers
    void applyBatchSet(const std::unordered_map<std::string, ConfigValue>& values, ConfigSource source);
    
    // Diff helpers
    std::string createDiffEntry(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) const;
    
    // Import/export helpers
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    
    // Snapshot helpers
    static std::string createSnapshotContent(const Config& config);
    static bool restoreFromSnapshotContent(Config& config, const std::string& content);
    
    // Consistency checking helpers
    std::vector<std::string> checkParameterDependencies() const;
    std::vector<std::string> checkValueRanges() const;
    std::vector<std::string> checkValueFormats() const;
    
    // Constants
    static constexpr const char* DEFAULT_PRESET_NAME = "default";
    static constexpr const char* PRESET_SEPARATOR = ":";
    static constexpr const char* SECTION_PREFIX = "/";
    static constexpr const char* KEY_VALUE_SEPARATOR = "=";
    static constexpr const char* COMMENT_PREFIX = "#";
};

// Convenience functions for common configuration scenarios
inline std::shared_ptr<Config> createDefaultConfig() {
    auto config = std::make_shared<Config>();
    
    // Register common parameters with validation
    std::vector<ConfigParameter> commonParams = {
        {"brain.neuron_count", 1000, "Number of neurons in the brain", "brain", "int", true},
        {"brain.synapse_density", 0.1, "Connection probability between neurons", "brain", "float"},
        {"brain.v_thresh", -50.0, "Neuron threshold potential (mV)", "brain", "float"},
        {"brain.v_rest", -70.0, "Neuron resting potential (mV)", "brain", "float"},
        {"brain.v_reset", -75.0, "Neuron reset potential after spike (mV)", "brain", "float"},
        {"brain.tau_mem", 20.0, "Membrane time constant (ms)", "brain", "float"},
        {"brain.tau_ref", 2.0, "Refractory period (ms)", "brain", "float"},
        {"plasticity.stdp.enable", true, "Enable Spike-Timing-Dependent Plasticity", "plasticity", "bool"},
        {"plasticity.stdp.learning_rate", 0.001, "STDP learning rate", "plasticity", "float"},
        {"plasticity.stdp.tau_plus", 20.0, "STDP LTP time constant (ms)", "plasticity", "float"},
        {"plasticity.stdp.tau_minus", 20.0, "STDP LTD time constant (ms)", "plasticity", "float"},
        {"plasticity.hebbian.enable", true, "Enable Hebbian learning", "plasticity", "bool"},
        {"plasticity.structural.enable", true, "Enable structural plasticity", "plasticity", "bool"},
        {"neuromod.dopamine.scale", 1.0, "Dopamine modulation scale", "neuromod", "float"},
        {"neuromod.curiosity.enable", true, "Enable curiosity-driven exploration", "neuromod", "bool"},
        {"neuromod.novelty.enable", true, "Enable novelty detection", "neuromod", "bool"},
        {"simulation.timestep", 0.001, "Simulation time step (seconds)", "simulation", "float"},
        {"simulation.max_steps", 1000, "Maximum simulation steps", "simulation", "int"},
        {"output.logging.level", "info", "Logging level (debug, info, warning, error)", "output", "string"},
        {"output.logging.colors", true, "Enable colored logging output", "output", "bool"},
    };
    
    for (const auto& param : commonParams) {
        config->registerParameter(param);
    }
    
    return config;
}

inline std::shared_ptr<Config> createDevelopmentConfig() {
    auto config = createDefaultConfig();
    
    // Set development-specific values
    config->set("brain.neuron_count", 500);
    config->set("brain.synapse_density", 0.05);
    config->set("brain.v_thresh", -55.0);
    config->set("brain.tau_mem", 25.0);
    config->set("brain.tau_ref", 5.0);
    config->set("plasticity.stdp.learning_rate", 0.002);
    config->set("plasticity.stdp.tau_plus", 25.0);
    config->set("plasticity.stdp.tau_minus", 25.0);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    config->set("simulation.timestep", 0.0005);
    config->set("simulation.max_steps", 5000);
    config->set("output.logging.level", "debug");
    
    return config;
}

inline std::shared_ptr<Config> createProductionConfig() {
    auto config = createDefaultConfig();
    
    // Set production-specific values (optimized for performance)
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1);
    config->set("brain.v_thresh", -50.0);
    config->set("brain.tau_mem", 20.0);
    config->set("brain.tau_ref", 2.0);
    config->set("plasticity.stdp.learning_rate", 0.001);
    config->set("plasticity.stdp.tau_plus", 20.0);
    config->set("plasticity.stdp.tau_minus", 20.0);
    config->set("neuromod.curiosity.enable", false);
    config->set("neuromod.novelty.enable", false);
    config->set("simulation.timestep", 0.001);
    config->set("simulation.max_steps", 1000);
    config->set("output.logging.level", "info");
    
    return config;
}

inline std::shared_ptr<Config> createResearchConfig() {
    auto config = createDefaultConfig();
    
    // Set research-specific values (comprehensive with all features)
    config->set("brain.neuron_count", 2000);
    config->set("brain.synapse_density", 0.02);
    config->set("brain.v_thresh", -60.0);
    config->set("brain.v_reset", -80.0);
    config->set("brain.tau_mem", 30.0);
    config->set("brain.tau_ref", 10.0);
    config->set("plasticity.stdp.enable", true);
    config->set("plasticity.stdp.learning_rate", 0.005);
    config->set("plasticity.stdp.tau_plus", 30.0);
    config->set("plasticity.stdp.tau_minus", 30.0);
    config->set("plasticity.hebbian.enable", true);
    config->set("plasticity.structural.enable", true);
    config->set("neuromod.dopamine.scale", 2.0);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    config->set("neuromod.prediction.enable", true);
    config->set("simulation.timestep", 0.0001);
    config->set("simulation.max_steps", 10000);
    config->set("output.logging.level", "debug");
    
    return config;
}

// Type checking and conversion utilities
namespace config_utils {
    inline bool isNumberType(const ConfigValue& value) {
        return std::holds_alternative<int>(value) ||
               std::holds_alternative<int64_t>(value) ||
               std::holds_alternative<double>(value);
    }
    
    inline bool isStringType(const ConfigValue& value) {
        return std::holds_alternative<std::string>(value);
    }
    
    inline bool isBoolType(const ConfigValue& value) {
        return std::holds_alternative<bool>(value);
    }
    
    inline bool isVectorType(const ConfigValue& value) {
        return std::holds_alternative<std::vector<int>>(value) ||
               std::holds_alternative<std::vector<double>>(value) ||
               std::holds_alternative<std::vector<std::string>>(value);
    }
    
    inline double toDouble(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        return 0.0;
    }
    
    inline std::string toString(const ConfigValue& value) {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
        return std::to_string(toDouble(value));
    }
    
    inline bool toBool(const ConfigValue& value) {
        if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        }
        return toDouble(value) > 0.5;
    }
    
    inline size_t getVectorSize(const ConfigValue& value) {
        if (std::holds_alternative<std::vector<int>>(value)) {
            return std::get<std::vector<int>>(value).size();
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value).size();
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return std::get<std::vector<std::string>>(value).size();
        }
        return 0;
    }
}

// Configuration comparison operators
inline bool operator==(const Config& a, const Config& b) {
    return a.equals(b);
}

inline bool operator!=(const Config& a, const Config& b) {
    return !(a == b);
}

// Hash function for Config
namespace std {
    template<typename T>
    struct hash<std::reference_wrapper<const T>> {
        size_t operator()(const std::reference_wrapper<const T>& wrapper) const {
            return hash<const T*>{}(&wrapper.get());
        }
    };
}

// Convenience functions for parameter validation
inline bool validateNeuronCount(const ConfigValue& value) {
    double count = config_utils::toDouble(value);
    return count > 0 && count <= 100000;  // Reasonable limits
}

inline bool validateSynapseDensity(const ConfigValue& value) {
    double density = config_utils::toDouble(value);
    return density >= 0.0 && density <= 1.0;  // Probability range
}

inline bool validateVoltage(const ConfigValue& value) {
    double voltage = config_utils::toDouble(value);
    return voltage >= -100.0 && voltage <= 50.0;  // Reasonable voltage range
}

inline bool validateTimeConstant(const ConfigValue& value) {
    double tau = config_utils::toDouble(value);
    return tau > 0 && tau <= 1000.0;  // Time constant range
}

inline bool validateLearningRate(const ConfigValue& value) {
    double rate = config_utils::toDouble(value);
    return rate >= 0.0 && rate <= 1.0;  // Learning rate range
}

} // namespace nlm

// Explicit template instantiations
namespace nlm {
    extern template std::optional<int> Config::get<int>(const std::string&) const;
    extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    extern template std::optional<double> Config::get<double>(const std::string&) const;
    extern template std::optional<bool> Config::get<bool>(const std::string&) const;
    extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    extern template int Config::getOr<int>(const std::string&, const int&) const;
    extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    extern template double Config::getOr<double>(const std::string&, const double&) const;
    extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
    extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}
