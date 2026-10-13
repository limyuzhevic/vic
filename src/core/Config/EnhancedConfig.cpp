#include "EnhancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <nlohmann/json.hpp>

// JSON support for configuration files
namespace json = nlohmann::json;

using namespace nlm;

// Configuration value validator interface
class IConfigValidator {
public:
    virtual ~IConfigValidator() = default;
    virtual bool validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getErrorMessage(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getDescription() const = 0;
};

// Range validator for numeric values
class RangeValidator : public IConfigValidator {
private:
    double minValue_;
    double maxValue_;
    bool allowNaN_ = false;
    bool allowInf_ = false;
    
public:
    RangeValidator(double min, double max, bool allowNaN = false, bool allowInf = false)
        : minValue_(min), maxValue_(max), allowNaN_(allowNaN), allowInf_(allowInf) {}
    
    bool validate(const std::string& key, const ConfigValue& value) const override {
        if (!std::holds_alternative<double>(value) && 
            !std::holds_alternative<float>(value) &&
            !std::holds_alternative<int>(value) &&
            !std::holds_alternative<int64_t>(value)) {
            return false;
        }
        
        double val;
        if (std::holds_alternative<double>(value)) {
            val = std::get<double>(value);
        } else if (std::holds_alternative<float>(value)) {
            val = static_cast<double>(std::get<float>(value));
        } else if (std::holds_alternative<int>(value)) {
            val = static_cast<double>(std::get<int>(value));
        } else {
            val = static_cast<double>(std::get<int64_t>(value));
        }
        
        if (!allowNaN_ && std::isnan(val)) return false;
        if (!allowInf_ && std::isinf(val)) return false;
        
        return val >= minValue_ && val <= maxValue_;
    }
    
    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        std::stringstream ss;
        ss << "Configuration \"" << key << "\" value \"" << getValueString(value) 
           << "\" is outside valid range [" << minValue_ << ", " << maxValue_ << "]";
        return ss.str();
    }
    
    std::string getDescription() const override {
        std::stringstream ss;
        ss << "Range validator: [" << minValue_ << ", " << maxValue_ << "]";
        return ss.str();
    }
    
private:
    std::string getValueString(const ConfigValue& value) const {
        std::stringstream ss;
        std::visit([&ss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                ss << "\"" << arg << "\"";
            } else {
                ss << arg;
            }
        }, value);
        return ss.str();
    }
};

// Type validator for specific types
class TypeValidator : public IConfigValidator {
private:
    std::type_index expectedType_;
    
public:
    template<typename T>
    TypeValidator() : expectedType_(typeid(T)) {}
    
    bool validate(const std::string& key, const ConfigValue& value) const override {
        return std::holds_alternative<T>(value);
    }
    
    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        return "Configuration \"" + key + "\" must be of expected type (" + 
               getTypeName(expectedType_) + "), got " + getTypeName(std::get_type_index(value));
    }
    
    std::string getDescription() const override {
        return "Type validator: " + getTypeName(expectedType_);
    }
    
private:
    std::string getTypeName(const std::type_index& typeIndex) const {
        if (typeIndex == typeid(int)) return "int";
        if (typeIndex == typeid(int64_t)) return "int64_t";
        if (typeIndex == typeid(double)) return "double";
        if (typeIndex == typeid(bool)) return "bool";
        if (typeIndex == typeid(std::string)) return "string";
        return "unknown";
    }
    
    std::type_index getTypeIndex(const ConfigValue& value) const {
        return std::get_type_index(value);
    }
};

// Default values configuration
class DefaultManager {
private:
    std::unordered_map<std::string, ConfigValue> defaultValues_;
    std::unordered_map<std::string, std::string> defaultDescriptions_;
    
public:
    void addDefault(const std::string& key, const ConfigValue& value, const std::string& description = "") {
        defaultValues_[key] = value;
        defaultDescriptions_[key] = description;
    }
    
    bool hasDefault(const std::string& key) const {
        return defaultValues_.find(key) != defaultValues_.end();
    }
    
    ConfigValue getDefault(const std::string& key) const {
        auto it = defaultValues_.find(key);
        if (it != defaultValues_.end()) {
            return it->second;
        }
        throw std::runtime_error("No default value found for key: " + key);
    }
    
    std::string getDescription(const std::string& key) const {
        auto it = defaultDescriptions_.find(key);
        if (it != defaultDescriptions_.end()) {
            return it->second;
        }
        return "";
    }
    
    std::vector<std::string> getAllKeys() const {
        std::vector<std::string> keys;
        keys.reserve(defaultValues_.size());
        for (const auto& pair : defaultValues_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
};

// Enhanced Config class with validation and utilities
class EnhancedConfig : public Config {
private:
    std::unique_ptr<DefaultManager> defaultManager_;
    std::vector<std::unique_ptr<IConfigValidator>> validators_;
    bool validationEnabled_ = true;
    bool strictMode_ = false;
    
public:
    EnhancedConfig() : Config() {
        defaultManager_ = std::make_unique<DefaultManager>();
        setupDefaultValues();
        setupValidators();
    }
    
    ~EnhancedConfig() override = default;
    
    // Enhanced validation
    bool validate() const {
        if (!validationEnabled_) return true;
        
        for (const auto& entry : pImpl->entries) {
            if (!validateEntry(entry)) {
                return false;
            }
        }
        return true;
    }
    
    // Load configuration with validation
    bool loadFromFile(const std::string& filepath) override {
        bool success = Config::loadFromFile(filepath);
        if (success && validationEnabled_) {
            if (!validate()) {
                NLM_LOG_ERROR("Configuration validation failed after loading from file: " + filepath);
                return false;
            }
        }
        return success;
    }
    
    // Load from arguments with validation
    bool loadFromArgs(int argc, char** argv) override {
        bool success = Config::loadFromArgs(argc, argv);
        if (success && validationEnabled_) {
            if (!validate()) {
                NLM_LOG_ERROR("Configuration validation failed after loading from arguments");
                return false;
            }
        }
        return success;
    }
    
    // Set value with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) override {
        if (validationEnabled_) {
            if (!validateNewEntry(key, value)) {
                std::string errorMsg = "Configuration validation failed for key: " + key;
                // Try to get error from validators
                for (const auto& validator : validators_) {
                    if (validator->validate(key, value)) {
                        errorMsg = validator->getErrorMessage(key, value);
                        break;
                    }
                }
                NLM_LOG_ERROR(errorMsg);
                if (strictMode_) {
                    throw std::runtime_error(errorMsg);
                }
                return;
            }
        }
        
        Config::set(key, value, source);
    }
    
    // Add validator
    void addValidator(std::unique_ptr<IConfigValidator> validator) {
        validators_.push_back(std::move(validator));
    }
    
    // Enable/disable validation
    void setValidationEnabled(bool enabled) {
        validationEnabled_ = enabled;
    }
    
    // Set strict mode
    void setStrictMode(bool strict) {
        strictMode_ = strict;
    }
    
    // Get validation summary
    std::string getValidationSummary() const {
        std::ostringstream oss;
        oss << "Configuration Validation Summary:" << std::endl;
        oss << "  Validation enabled: " << (validationEnabled_ ? "yes" : "no") << std::endl;
        oss << "  Strict mode: " << (strictMode_ ? "yes" : "no") << std::endl;
        oss << "  Number of validators: " << validators_.size() << std::endl;
        oss << "  Number of default values: " << (defaultManager_ ? defaultManager_->getAllKeys().size() : 0) << std::endl;
        
        if (validationEnabled_) {
            oss << "\nDefault values:";
            for (const auto& key : defaultManager_->getAllKeys()) {
                oss << "\n    " << key << ": " << defaultManager_->getDescription(key);
            }
        }
        
        return oss.str();
    }
    
    // Check if configuration is complete (has all required values)
    bool isComplete() const {
        if (!defaultManager_) return true;
        
        for (const auto& key : defaultManager_->getAllKeys()) {
            if (!has(key)) {
                NLM_LOG_WARNING("Missing required configuration: " + key + ". Using default.");
                // Automatically apply default if missing
                set(key, defaultManager_->getDefault(key), ConfigSource::Default);
            }
        }
        return true;
    }
    
protected:
    void setupDefaultValues() {
        if (!defaultManager_) return;
        
        // Add common NLM configuration defaults
        defaultManager_->addDefault("random_seed", ConfigValue(static_cast<int64_t>(42)), 
                                   "Random seed for reproducible simulations");
        
        defaultManager_->addDefault("simulation_timestep", ConfigValue(0.001), 
                                   "Time step for simulation in seconds");
        
        defaultManager_->addDefault("neuron_count", ConfigValue(static_cast<int64_t>(500)), 
                                   "Number of neurons in the brain");
        
        defaultManager_->addDefault("region_count", ConfigValue(static_cast<int64_t>(1)), 
                                   "Number of neural regions");
        
        defaultManager_->addDefault("connection_probability", ConfigValue(0.15f), 
                                   "Probability of synaptic connections between neurons");
        
        defaultManager_->addDefault("stdp_ltp_weight", ConfigValue(0.02f), 
                                   "STDP Long-Term Potentiation weight factor");
        
        defaultManager_->addDefault("stdp_ltd_weight", ConfigValue(0.015f), 
                                   "STDP Long-Term Depression weight factor");
        
        defaultManager_->addDefault("stdp_tau", ConfigValue(20.0f), 
                                   "STDP time constant for weight changes");
        
        defaultManager_->addDefault("synaptogenesis_rate", ConfigValue(0.0001f), 
                                   "Rate of new synapse formation");
        
        defaultManager_->addDefault("pruning_rate", ConfigValue(0.00001f), 
                                   "Rate of synapse elimination");
        
        defaultManager_->addDefault("enable_checkpointing", ConfigValue(false), 
                                   "Enable checkpoint save/load functionality");
        
        defaultManager_->addDefault("checkpoint_path", ConfigValue("/tmp/nlm_checkpoint.bin"), 
                                   "Path for checkpoint files");
        
        defaultManager_->addDefault("enable_replay", ConfigValue(false), 
                                   "Enable memory replay functionality");
        
        defaultManager_->addDefault("enable_development", ConfigValue(true), 
                                   "Enable developmental changes");
    }
    
    void setupValidators() {
        // Add range validators
        addValidator(std::make_unique<RangeValidator>(-1e6, 1e6)); // For weight values
        addValidator(std::make_unique<RangeValidator>(0.0, 1.0, false, false)); // For probabilities
        addValidator(std::make_unique<RangeValidator>(1, 10000, true, true)); // For neuron counts
        addValidator(std::make_unique<RangeValidator>(0.0, 1.0, false, true)); // For learning rates
        addValidator(std::make_unique<RangeValidator>(-1000.0, 1000.0, false, true)); // For currents
        
        // Add type validators
        addValidator(std::make_unique<TypeValidator<int64_t>>());
        addValidator(std::make_unique<TypeValidator<double>>());
        addValidator(std::make_unique<TypeValidator<float>>());
        addValidator(std::make_unique<TypeValidator<bool>>());
        addValidator(std::make_unique<TypeValidator<std::string>>());
    }
    
    bool validateEntry(const ConfigEntry& entry) const {
        for (const auto& validator : validators_) {
            if (!validator->validate(entry.key, entry.value)) {
                return false;
            }
        }
        return true;
    }
    
    bool validateNewEntry(const std::string& key, const ConfigValue& value) const {
        for (const auto& validator : validators_) {
            if (!validator->validate(key, value)) {
                return false;
            }
        }
        return true;
    }
};

// Configuration file helper functions
namespace ConfigUtils {
    
    // Load JSON configuration file
    bool loadJSONConfig(const std::string& filepath, EnhancedConfig& config) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            json j;
            file >> j;
            
            // Convert JSON to NLM configuration
            for (auto it = j.begin(); it != j.end(); ++it) {
                std::string key = it.key();
                
                // Convert JSON value to ConfigValue
                ConfigValue value;
                const auto& jsonValue = it.value();
                
                if (jsonValue.is_number_integer()) {
                    value = jsonValue.get<int64_t>();
                } else if (jsonValue.is_number_unsigned()) {
                    value = jsonValue.get<uint64_t>();
                } else if (jsonValue.is_number_float()) {
                    value = jsonValue.get<double>();
                } else if (jsonValue.is_boolean()) {
                    value = jsonValue.get<bool>();
                } else if (jsonValue.is_string()) {
                    value = jsonValue.get<std::string>();
                } else if (jsonValue.is_array()) {
                    // Convert array to vector
                    std::vector<double> vec;
                    for (const auto& item : jsonValue) {
                        if (item.is_number()) {
                            vec.push_back(item.get<double>());
                        }
                    }
                    value = vec;
                }
                
                config.set(key, value, ConfigSource::File);
            }
            
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error loading JSON configuration: " + std::string(e.what()));
            return false;
        }
    }
    
    // Save configuration to JSON file
    bool saveJSONConfig(const std::string& filepath, const EnhancedConfig& config) {
        try {
            json j;
            
            for (const auto& entry : config.getKeys()) {
                auto value = config.get<ConfigValue>(entry);
                if (!value) continue;
                
                // Convert ConfigValue to JSON
                j[entry] = configValueToJSON(*value);
            }
            
            std::ofstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            file << j.dump(2); // Pretty print with 2-space indentation
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error saving JSON configuration: " + std::string(e.what()));
            return false;
        }
    }
    
    // Convert ConfigValue to JSON
    json configValueToJSON(const ConfigValue& value) {
        return std::visit([](auto&& arg) -> json {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                return arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return arg;
            } else if constexpr (std::is_same_v<T, double>) {
                return arg;
            } else if constexpr (std::is_same_v<T, float>) {
                return static_cast<double>(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                json j = json::array();
                for (int val : arg) {
                    j.push_back(val);
                }
                return j;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                json j = json::array();
                for (double val : arg) {
                    j.push_back(val);
                }
                return j;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                json j = json::array();
                for (const std::string& val : arg) {
                    j.push_back(val);
                }
                return j;
            } else {
                throw std::runtime_error("Unsupported ConfigValue type");
            }
        }, value);
    }
    
    // Merge configuration from multiple sources
    void mergeConfigs(EnhancedConfig& target, const EnhancedConfig& source, ConfigSource sourceType = ConfigSource::File) {
        for (const auto& key : source.getKeys()) {
            auto value = source.get<ConfigValue>(key);
            if (value) {
                target.set(key, *value, sourceType);
            }
        }
    }
    
    // Validate configuration constraints
    bool validateConfigConstraints(const EnhancedConfig& config) {
        // Check for required constraints
        auto neuronCount = config.getOr<int64_t>("neuron_count", 500);
        auto regionCount = config.getOr<int64_t>("region_count", 1);
        
        if (neuronCount <= 0) {
            NLM_LOG_ERROR("neuron_count must be positive, got: " + std::to_string(neuronCount));
            return false;
        }
        
        if (regionCount <= 0) {
            NLM_LOG_ERROR("region_count must be positive, got: " + std::to_string(regionCount));
            return false;
        }
        
        auto connectionProb = config.getOr<float>("connection_probability", 0.15f);
        if (connectionProb < 0.0f || connectionProb > 1.0f) {
            NLM_LOG_ERROR("connection_probability must be between 0.0 and 1.0, got: " + std::to_string(connectionProb));
            return false;
        }
        
        return true;
    }
}

// Configuration convenience functions
namespace ConfigPresets {
    
    // Create a simple configuration for testing
    EnhancedConfig createTestConfig() {
        EnhancedConfig config;
        
        config.set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config.set("simulation_timestep", 0.001, ConfigSource::Default);
        config.set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config.set("connection_probability", 0.15f, ConfigSource::Default);
        
        return config;
    }
    
    // Create a configuration for real-time simulation
    EnhancedConfig createRealTimeConfig() {
        EnhancedConfig config;
        
        config.set("random_seed", static_cast<int64_t>(static_cast<int64_t>(std::time(nullptr))), ConfigSource::Default);
        config.set("simulation_timestep", 0.0001, ConfigSource::Default);
        config.set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config.set("connection_probability", 0.2f, ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.009f, ConfigSource::Default);
        
        return config;
    }
    
    // Create a configuration for research experiments
    EnhancedConfig createResearchConfig() {
        EnhancedConfig config;
        
        config.set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
        config.set("simulation_timestep", 0.001, ConfigSource::Default);
        config.set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
        config.set("region_count", static_cast<int64_t>(5), ConfigSource::Default);
        config.set("connection_probability", 0.1f, ConfigSource::Default);
        config.set("stdp_ltp_weight", 0.015f, ConfigSource::Default);
        config.set("stdp_ltd_weight", 0.014f, ConfigSource::Default);
        config.set("stdp_tau", 50.0f, ConfigSource::Default);
        config.set("synaptogenesis_rate", 0.00005f, ConfigSource::Default);
        config.set("pruning_rate", 0.000005f, ConfigSource::Default);
        config.set("enable_checkpointing", true, ConfigSource::Default);
        config.set("enable_replay", true, ConfigSource::Default);
        config.set("enable_development", true, ConfigSource::Default);
        
        return config;
    }
}