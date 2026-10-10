#include "AdvancedConfig.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

namespace nlm {

// Helper functions for AdvancedConfig
namespace {
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    bool parseConfigLine(const std::string& line, std::string& key, std::string& value) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) return false;
        
        key = trim(line.substr(0, pos));
        value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        return true;
    }
    
    std::string toString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
    
    std::string getTypeString(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
        if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
        return "unknown";
    }
} // anonymous namespace

// AdvancedConfig implementation
bool AdvancedConfig::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        std::string key, value;
        if (!parseConfigLine(line, key, value)) {
            NLM_LOG_WARNING("Skipping invalid config line " + std::to_string(lineNumber) + ": " + line);
            continue;
        }
        
        // Convert string value to appropriate type
        ConfigValue configValue;
        bool converted = false;
        
        // Try different types in order
        try {
            if (value.find('.') != std::string::npos) {
                double doubleVal = std::stod(value);
                configValue = doubleVal;
                converted = true;
            }
        } catch (...) {}
        
        if (!converted) {
            try {
                if (value.find('e') != std::string::npos || value.find('E') != std::string::npos) {
                    double doubleVal = std::stod(value);
                    configValue = doubleVal;
                    converted = true;
                }
            } catch (...) {}
        }
        
        if (!converted) {
            try {
                size_t pos;
                int intVal = std::stoi(value, &pos);
                if (pos == value.size()) {
                    configValue = intVal;
                    converted = true;
                }
            } catch (...) {}
        }
        
        if (!converted) {
            if (value == "true" || value == "false") {
                bool boolVal = (value == "true");
                configValue = boolVal;
                converted = true;
            }
        }
        
        if (!converted) {
            // Check if it's a vector (comma-separated)
            if (value.find(',') != std::string::npos) {
                std::vector<double> doubleVec;
                std::stringstream ss(value);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    item = trim(item);
                    try {
                        doubleVec.push_back(std::stod(item));
                    } catch (...) {
                        // Skip invalid items
                    }
                }
                if (!doubleVec.empty()) {
                    configValue = doubleVec;
                    converted = true;
                }
            }
        }
        
        if (!converted) {
            // Default to string
            configValue = value;
        }
        
        set(key, configValue, ConfigSource::File);
    }
    
    return true;
}

bool AdvancedConfig::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

void AdvancedConfig::addSchema(const ConfigSchemaEntry& schema) {
    schema_.push_back(schema);
}

bool AdvancedConfig::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    if (!errors.empty()) {
        for (const auto& error : errors) {
            NLM_LOG_ERROR(error);
        }
        return false;
    }
    return true;
}

template<typename T>
T AdvancedConfig::get(const std::string& key) const {
    auto value = config_.get<T>(key);
    if (!value.has_value()) {
        throw std::runtime_error("Config key not found: " + key);
    }
    
    // Validate against schema if present
    const ConfigSchemaEntry* schema = findSchema(key);
    if (schema && !validateValue(*schema, ConfigValue(value.value()))) {
        throw ValidationError("Value validation failed for key: " + key);
    }
    
    return value.value();
}

template<typename T>
T AdvancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    auto value = config_.getOr<T>(key, defaultValue);
    
    // Validate against schema if present
    const ConfigSchemaEntry* schema = findSchema(key);
    if (schema && !validateValue(*schema, ConfigValue(value))) {
        // Fall back to default if validation fails
        return defaultValue;
    }
    
    return value;
}

void AdvancedConfig::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate against schema if present
    const ConfigSchemaEntry* schema = findSchema(key);
    if (schema) {
        if (!validateValue(*schema, value)) {
            NLM_LOG_ERROR("Invalid value for key '" + key + "': " + toString(value) + 
                         ". Expected type: " + schema->type);
            return;
        }
    }
    
    config_.set(key, value, source);
}

void AdvancedConfig::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void AdvancedConfig::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void AdvancedConfig::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void AdvancedConfig::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void AdvancedConfig::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void AdvancedConfig::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void AdvancedConfig::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool AdvancedConfig::has(const std::string& key) const {
    return config_.has(key);
}

std::vector<std::string> AdvancedConfig::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Check required fields
    for (const auto& schema : schema_) {
        if (schema.required && !has(schema.key)) {
            errors.push_back("Missing required configuration key: '" + schema.key + "'. " + 
                           schema.description);
        }
    }
    
    // Validate existing values
    for (const auto& entry : config_.getKeys()) {
        auto valueOpt = config_.get<std::string>(entry);
        if (valueOpt.has_value()) {
            const ConfigValue& value = valueOpt.value();
            const ConfigSchemaEntry* schema = findSchema(entry);
            if (schema && !validateValue(*schema, value)) {
                errors.push_back("Invalid value for key '" + entry + "': " + toString(value) + 
                               ". Expected type: " + schema->type);
            }
        }
    }
    
    return errors;
}

std::string AdvancedConfig::summary() const {
    std::ostringstream oss;
    oss << "Advanced Configuration (" << config_.getKeys().size() << " entries):\n";
    
    for (const auto& entry : config_.getKeys()) {
        oss << "  " << entry << " = [";
        auto valueOpt = config_.get<std::string>(entry);
        if (valueOpt.has_value()) {
            oss << toString(valueOpt.value());
        } else {
            oss << "UNKNOWN";
        }
        oss << "] (source: " << static_cast<int>(config_.get(entry).source) << ")";
        
        // Show validation status
        const ConfigSchemaEntry* schema = findSchema(entry);
        if (schema) {
            bool valid = true;
            auto valueOpt = config_.get<std::string>(entry);
            if (valueOpt.has_value()) {
                valid = validateValue(*schema, valueOpt.value());
            }
            oss << " VALID: " << (valid ? "YES" : "NO");
        }
        oss << "\n";
    }
    
    auto errors = getValidationErrors();
    if (!errors.empty()) {
        oss << "\nVALIDATION ERRORS:\n";
        for (const auto& error : errors) {
            oss << "  ERROR: " << error << "\n";
        }
    }
    
    return oss.str();
}

bool AdvancedConfig::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    for (const auto& entry : config_.getKeys()) {
        file << "# " << findSchema(entry)->description << "\n";
        auto valueOpt = config_.get<std::string>(entry);
        if (valueOpt.has_value()) {
            file << entry << " = " << toString(valueOpt.value()) << "\n";
        } else {
            file << entry << " = [UNKNOWN]\n";
        }
        file << "\n";
    }
    
    return true;
}

void AdvancedConfig::resetToDefaults() {
    // Clear all entries
    config_.clear();
    
    // Set defaults from schema
    for (const auto& schema : schema_) {
        if (std::holds_alternative<int>(schema.defaultValue)) {
            int val = std::get<int>(schema.defaultValue);
            set(schema.key, val, ConfigSource::Default);
        } else if (std::holds_alternative<double>(schema.defaultValue)) {
            double val = std::get<double>(schema.defaultValue);
            set(schema.key, val, ConfigSource::Default);
        } else if (std::holds_alternative<std::string>(schema.defaultValue)) {
            std::string val = std::get<std::string>(schema.defaultValue);
            set(schema.key, val, ConfigSource::Default);
        } else if (std::holds_alternative<bool>(schema.defaultValue)) {
            bool val = std::get<bool>(schema.defaultValue);
            set(schema.key, val, ConfigSource::Default);
        } else if (std::holds_alternative<std::vector<int>>(schema.defaultValue)) {
            std::vector<int> val = std::get<std::vector<int>>(schema.defaultValue);
            set(schema.key, val, ConfigSource::Default);
        } else if (std::holds_alternative<std::vector<double>>(schema.defaultValue)) {
            std::vector<double> val = std::get<std::vector<double>>(schema.defaultValue);
            set(schema.key, val, ConfigSource::Default);
        } else if (std::holds_alternative<std::vector<std::string>>(schema.defaultValue)) {
            std::vector<std::string> val = std::get<std::vector<std::string>>(schema.defaultValue);
            set(schema.key, val, ConfigSource::Default);
        }
    }
}

void AdvancedConfig::merge(const AdvancedConfig& other) {
    // Merge other config into this one
    for (const auto& key : other.config_.getKeys()) {
        auto valueOpt = other.config_.get<std::string>(key);
        if (valueOpt.has_value()) {
            config_.set(key, valueOpt.value(), ConfigSource::Runtime);
        }
    }
}

bool AdvancedConfig::validateType(const std::string& expectedType, const ConfigValue& value) const {
    std::string actualType = getTypeString(value);
    
    // Simplify type checking (basic type names)
    if (expectedType == "int" && (actualType == "int" || actualType == "int64_t")) return true;
    if (expectedType == "double" && actualType == "double") return true;
    if (expectedType == "string" && actualType == "string") return true;
    if (expectedType == "bool" && actualType == "bool") return true;
    if (expectedType == "vector<int>" && (actualType == "vector<int>" || actualType == "vector<double>")) return true;
    if (expectedType == "vector<double>" && (actualType == "vector<double>" || actualType == "vector<int>")) return true;
    if (expectedType == "vector<string>" && actualType == "vector<string>") return true;
    
    return expectedType == actualType;
}

bool AdvancedConfig::validateValue(const ConfigSchemaEntry& schema, const ConfigValue& value) const {
    // First check type
    if (!validateType(schema.type, value)) {
        return false;
    }
    
    // Then apply custom validator if present
    if (schema.validator && !schema.validator(value)) {
        return false;
    }
    
    return true;
}

const ConfigSchemaEntry* AdvancedConfig::findSchema(const std::string& key) const {
    for (const auto& schema : schema_) {
        if (schema.key == key) {
            return &schema;
        }
    }
    return nullptr;
}

// Phase6AdvancedConfig to AdvancedConfig conversion
AdvancedConfig Phase6AdvancedConfig::toAdvancedConfig() const {
    AdvancedConfig advanced;
    
    // Add basic schemas
    advanced.addSchema(ConfigSchemaEntry("neuron_count", "int", true, "Number of neurons in the brain", 1000, 
                                         [](const ConfigValue& v) { return std::holds_alternative<int>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("region_count", "int", true, "Number of brain regions", 1,
                                         [](const ConfigValue& v) { return std::holds_alternative<int>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("connection_probability", "double", true, 
                                         "Probability of synaptic connections", 0.1f,
                                         [](const ConfigValue& v) { return std::holds_alternative<double>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("enable_checkpointing", "bool", false, 
                                         "Enable checkpoint save/load functionality", true,
                                         [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("enable_replay", "bool", false, 
                                         "Enable episodic memory replay", true,
                                         [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("enable_development", "bool", false, 
                                         "Enable developmental processes", true,
                                         [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("enable_curiosity", "bool", false, 
                                         "Enable curiosity-driven exploration", true,
                                         [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("enable_prediction", "bool", false,
                                         "Enable prediction system", true,
                                         [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("enable_structural_plasticity", "bool", false,
                                         "Enable structural synaptic plasticity", true,
                                         [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("enable_synaptogenesis", "bool", false,
                                         "Enable synapse formation", true,
                                         [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("enable_pruning", "bool", false,
                                         "Enable synapse elimination", true,
                                         [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("enable_stdp", "bool", false,
                                         "Enable Spike-Timing Dependent Plasticity", true,
                                         [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("enable_hebbian", "bool", false,
                                         "Enable Hebbian learning", true,
                                         [](const ConfigValue& v) { return std::holds_alternative<bool>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("timestep", "double", false, "Simulation timestep in seconds", 1.0,
                                         [](const ConfigValue& v) { return std::holds_alternative<double>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("simulation_timestep", "double", false, "Neuron simulation timestep", 0.001,
                                         [](const ConfigValue& v) { return std::holds_alternative<double>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("stdp_ltp_weight", "double", false, "STDP LTP weight", 0.01f,
                                         [](const ConfigValue& v) { return std::holds_alternative<double>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("stdp_ltd_weight", "double", false, "STDP LTD weight", 0.012f,
                                         [](const ConfigValue& v) { return std::holds_alternative<double>(v); }));
    
    advanced.addSchema(ConfigSchemaEntry("stdp_tau", "double", false, "STDP time constant", 20.0f,
                                         [](const ConfigValue& v) { return std::holds_alternative<double>(v); }));
    
    // Set values
    advanced.set("neuron_count", static_cast<int>(neuronCount));
    advanced.set("region_count", static_cast<int>(regionCount));
    advanced.set("connection_probability", connectionProbability);
    advanced.set("enable_checkpointing", enableCheckpointing);
    advanced.set("enable_replay", enableReplay);
    advanced.set("enable_development", enableDevelopment);
    advanced.set("enable_curiosity", enableCuriosity);
    advanced.set("enable_prediction", enablePrediction);
    advanced.set("enable_structural_plasticity", enableStructuralPlasticity);
    advanced.set("enable_synaptogenesis", enableSynaptogenesis);
    advanced.set("enable_pruning", enablePruning);
    advanced.set("enable_stdp", enableSTDP);
    advanced.set("enable_hebbian", enableHebbian);
    advanced.set("timestep", timestep);
    advanced.set("simulation_timestep", simulationTimestep);
    advanced.set("stdp_ltp_weight", stdpLTPWeight);
    advanced.set("stdp_ltd_weight", stdpLTDWeight);
    advanced.set("stdp_tau", stdpTau);
    
    return advanced;
}

// AdvancedPhase6IntegratedExperiment implementation
AdvancedPhase6IntegratedExperiment::AdvancedPhase6IntegratedExperiment() : pImpl(std::make_unique<Impl>()) {}

AdvancedPhase6IntegratedExperiment::~AdvancedPhase6IntegratedExperiment() = default;

Phase6IntegrationResult AdvancedPhase6IntegratedExperiment::run(const Phase6AdvancedConfig& config) {
    return pImpl->run(config);
}

bool AdvancedPhase6IntegratedExperiment::validateConfig(const Phase6AdvancedConfig& config) {
    // Basic validation
    if (config.neuronCount == 0) return false;
    if (config.maxSteps == 0) return false;
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) return false;
    if (config.simulationTimestep <= 0.0) return false;
    if (config.stdpLTPWeight < 0.0f) return false;
    if (config.stdpLTDWeight < 0.0f) return false;
    if (config.stdptau < 0.0f) return false;
    
    return true;
}

std::vector<std::string> AdvancedPhase6IntegratedExperiment::validateConfigErrors(const Phase6AdvancedConfig& config) {
    std::vector<std::string> errors;
    
    if (config.neuronCount == 0) {
        errors.push_back("neuronCount must be > 0");
    }
    if (config.maxSteps == 0) {
        errors.push_back("maxSteps must be > 0");
    }
    if (config.connectionProbability < 0.0f || config.connectionProbability > 1.0f) {
        errors.push_back("connectionProbability must be between 0.0 and 1.0");
    }
    if (config.simulationTimestep <= 0.0) {
        errors.push_back("simulationTimestep must be > 0");
    }
    if (config.stdpLTPWeight < 0.0f) {
        errors.push_back("stdpLTPWeight must be >= 0");
    }
    if (config.stdpLTDWeight < 0.0f) {
        errors.push_back("stdpLTDWeight must be >= 0");
    }
    if (config.stdpTau < 0.0f) {
        errors.push_back("stdpTau must be >= 0");
    }
    
    return errors;
}

Phase6AdvancedConfig AdvancedPhase6IntegratedExperiment::getDefaultConfig() {
    return Phase6AdvancedConfig();
}

Phase6AdvancedConfig AdvancedPhase6IntegratedExperiment::mergeWithArgs(const Phase6AdvancedConfig& config, int argc, char** argv) {
    Phase6AdvancedConfig merged = config;
    
    // Simple argument parsing for demo
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                try {
                    if (key == "neuron_count" || key == "max_steps" || key == "region_count") {
                        merged.neuronCount = std::stoul(value);
                    } else if (key == "connection_probability") {
                        merged.connectionProbability = std::stof(value);
                    } else if (key == "enable_checkpointing" || key == "enable_replay" || 
                               key == "enable_development" || key == "enable_curiosity" ||
                               key == "enable_prediction") {
                        merged.enableCuriosity = (value == "true" || value == "1");
                    }
                } catch (...) {
                    // Ignore parsing errors
                }
            }
        }
    }
    
    return merged;
}

} // namespace nlm
