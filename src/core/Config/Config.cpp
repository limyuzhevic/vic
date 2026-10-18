#include "Config.hpp"
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

#include "Config.hpp"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <ctime>
#include <cstdlib>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Additional data for enhanced configuration
    std::map<std::string, std::string> groups;
    std::map<std::string, std::vector<std::string>> parameterGroups;
    std::map<std::string, std::string> conditionalParameters;
};

Config::Config() : pImpl(std::make_unique<Impl>()), configVersion_("1.0") {
    // Initialize with default schema
    schema_ = createBrainSystemSchema();
    
    // Add default brain system parameters to schema
    addDefaultBrainSystemParameters();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

// Helper to convert hex to timestamp string
static std::string generateTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void Config::addDefaultBrainSystemParameters() {
    // Neuron parameters
    addParameterToSchema("neuron_count", "size_t", true, 
                        "Number of neurons in the brain (must be > 0)", "1000");
    addParameterToSchema("connection_probability", "float", true,
                        "Probability of connection between neurons (0-1)", "0.1");
    addParameterToSchema("simulation_timestep", "double", true,
                        "Simulation timestep in seconds (must be > 0)", "0.001");
    
    // Plasticity parameters
    addParameterToSchema("stdp_ltp_weight", "float", true,
                        "STDP long-term potentiation weight", "0.01");
    addParameterToSchema("stdp_ltd_weight", "float", true,
                        "STDP long-term depression weight (must be < stdp_ltp_weight)", "0.012");
    addParameterToSchema("stdp_tau", "float", true,
                        "STDP time constant (must be > 0)", "20.0");
    
    // Memory parameters
    addParameterToSchema("memory_capacity", "size_t", true,
                        "Memory system capacity (must be > 0)", "1000");
    addParameterToSchema("max_episodes", "size_t", true,
                        "Maximum number of episodes for episodic memory (must be > 0)", "1000");
    
    // Development parameters
    addParameterToSchema("synaptogenesis_rate", "float", true,
                        "Rate of new synapse formation (plasticity rate)", "0.0001");
    addParameterToSchema("pruning_rate", "float", true,
                        "Rate of synapse elimination (plasticity rate)", "0.00001");
    
    // Neuromodulation parameters
    addParameterToSchema("dopamine_threshold", "float", true,
                        "Dopamine neuromodulation threshold", "0.5");
    addParameterToSchema("curiosity_threshold", "float", true,
                        "Curiosity neuromodulation threshold", "0.3");
    
    // Other parameters
    addParameterToSchema("region_count", "size_t", true,
                        "Number of brain regions", "1");
    addParameterToSchema("random_seed", "uint64_t", true,
                        "Random seed for reproducibility", "42");
    addParameterToSchema("replay_interval", "size_t", true,
                        "Memory replay interval (steps)", "100");
    addParameterToSchema("consolidation_interval", "size_t", true,
                        "Memory consolidation interval (steps)", "1000");
    addParameterToString("checkpoint_dir", "string", false,
                         "Directory for checkpoint saves", "./checkpoints");
}

void Config::addParameterToSchema(const std::string& name, const std::string& type,
                                 bool required, const std::string& description,
                                 const std::string& defaultValue) {
    ConfigSchema::ParameterInfo param;
    param.name = name;
    param.type = type;
    param.required = required;
    param.description = description;
    
    // Add specific validators based on type
    if (type == "float" || type == "double") {
        param.addValidator(std::make_unique<RangeValidator>(0.0, std::numeric_limits<double>::max(),
                                                           RangeValidator::BoundType::Inclusive,
                                                           RangeValidator::BoundType::Inclusive,
                                                           "Value must be positive"));
    } else if (type == "size_t") {
        param.addValidator(std::make_unique<RangeValidator>(1.0, std::numeric_limits<double>::max(),
                                                           RangeValidator::BoundType::Inclusive,
                                                           RangeValidator::BoundType::Inclusive,
                                                           "Value must be >= 1"));
    } else if (type == "int") {
        param.addValidator(std::make_unique<RangeValidator>(-1000.0, 1000.0,
                                                           RangeValidator::BoundType::Inclusive,
                                                           RangeValidator::BoundType::Inclusive,
                                                           "Value must be in reasonable range"));
    } else if (type == "string") {
        param.addValidator(std::make_unique<StringValidator>("", StringValidator::PatternType::Regex,
                                                            1, std::numeric_limits<size_t>::max(),
                                                            "String must be non-empty"));
    }
    
    // Special validation for brain system parameters
    if (name == "connection_probability") {
        param.addValidator(std::make_unique<RangeValidator>(0.0, 1.0,
                                                           RangeValidator::BoundType::Inclusive,
                                                           RangeValidator::BoundType::Inclusive,
                                                           "Connection probability must be between 0 and 1"));
    } else if (name == "stdp_ltd_weight") {
        // This will be validated conditionally against stdp_ltp_weight
        auto validator = std::make_unique<RangeValidator>(-1000.0, 1000.0,
                                                          RangeValidator::BoundType::Inclusive,
                                                          RangeValidator::BoundType::Inclusive,
                                                          "STDP LTD weight must be reasonable");
        validator->conditionalCheck_ = [this](const ValidationContext& context) {
            auto ltpOpt = get<double>("stdp_ltp_weight");
            if (ltpOpt) {
                double ltd = context.getValueAs<double>();
                return ltd < *ltpOpt;
            }
            return true; // Can't validate without LTP value
        };
        param.addValidator(std::move(validator));
    } else if (name == "stdp_tau") {
        param.addValidator(std::make_unique<RangeValidator>(0.1, std::numeric_limits<double>::max(),
                                                           RangeValidator::BoundType::Inclusive,
                                                           RangeValidator::BoundType::Inclusive,
                                                           "STDP tau must be > 0"));
    }
    
    schema_.addParameter(param);
}

// Enhanced parser methods
bool Config::loadFromFile(const std::string& filepath) {
    // Try key=value format first
    std::ifstream file(filepath);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        file.close();
        return true;
    }
    
    // Try JSON format
    if (parseJsonFile(filepath)) {
        return true;
    }
    
    // Try YAML format
    if (parseYamlFile(filepath)) {
        return true;
    }
    
    return false;
}

bool Config::loadFromJson(const std::string& filepath) {
    return parseJsonFile(filepath);
}

bool Config::loadFromYaml(const std::string& filepath) {
    return parseYamlFile(filepath);
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    envPrefix_ = prefix;
    // Placeholder for Phase 2 implementation
    return true;
}

void Config::addGroup(const std::string& name, const std::string& description) {
    groups_[name] = description;
}

std::vector<std::string> Config::getGroups() const {
    std::vector<std::string> groupNames;
    for (const auto& pair : pImpl->groups) {
        groupNames.push_back(pair.first);
    }
    return groupNames;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file type by extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    if (ext == "json") {
        return parseJsonFile(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return parseYamlFile(filepath);
    } else {
        // Default to key=value format
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

bool Config::parseJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            std::string valueStr = it.value().dump();
            
            // Remove quotes if string
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            set(key, valueStr, ConfigSource::File);
        }
        
        return true;
    } catch (const json::exception&) {
        return false;
    }
}

bool Config::parseYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        for (auto it = doc.begin(); it != doc.end(); ++it) {
            std::string key = it->first.as<std::string>();
            std::string valueStr;
            
            if (it->second.IsScalar()) {
                valueStr = it->second.as<std::string>();
            } else if (it->second.IsSequence()) {
                // Convert sequence to string representation
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < it->second.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << it->second[i].as<std::string>();
                }
                ss << "]";
                valueStr = ss.str();
            } else {
                valueStr = it->second.as<std::string>();
            }
            
            set(key, valueStr, ConfigSource::File);
        }
        
        return true;
    } catch (const YAML::Exception&) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

bool Config::saveToJson(const std::string& filepath) const {
    return writeJsonFile(filepath);
}

bool Config::writeJsonFile(const std::string& filepath, const std::map<std::string, std::string>& configMap) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    json j;
    for (const auto& pair : configMap) {
        // Try to determine type and convert appropriately
        j[pair.first] = pair.second;
    }
    
    file << std::setw(2) << j;
    return true;
}

bool Config::writeJsonFile(const std::string& filepath) const {
    std::map<std::string, std::string> configMap;
    
    for (const auto& entry : pImpl->entries) {
        std::string valueStr;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else if constexpr (std::is_integral_v<T>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_floating_point_v<T>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << arg[i];
                }
                ss << "]";
                valueStr = ss.str();
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << arg[i];
                }
                ss << "]";
                valueStr = ss.str();
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::stringstream ss;
                ss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << "\"" << arg[i] << "\"";
                }
                ss << "]";
                valueStr = ss.str();
            }
        }, entry.value);
        
        configMap[entry.key] = valueStr;
    }
    
    return writeJsonFile(filepath, configMap);
}

bool Config::writeYamlFile(const std::string& filepath, const std::map<std::string, std::string>& configMap) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    YAML::Emitter out;
    out << YAML::BeginMap;
    
    for (const auto& pair : configMap) {
        // Simple scalar values - could be enhanced for vectors
        out << YAML::Key << pair.first;
        out << YAML::Value << pair.second;
    }
    
    out << YAML::EndMap;
    file << out.c_str();
    
    return true;
}

bool Config::writeYamlFile(const std::string& filepath) const {
    std::map<std::string, std::string> configMap;
    
    for (const auto& entry : pImpl->entries) {
        std::string valueStr;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else {
                valueStr = std::to_string(arg);
            }
        }, entry.value);
        
        configMap[entry.key] = valueStr;
    }
    
    return writeYamlFile(filepath, configMap);
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::stringstream ss;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << arg[i] << "\"";
            }
            ss << "]";
        } else {
            ss << arg;
        }
    }, value);
    
    return ss.str();
}

std::string Config::generateConfigId() const {
    std::stringstream ss;
    ss << "config_" << generateTimestamp();
    return ss.str();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    ValidationResult result = validateBrainSystem();
    
    for (const auto& error : result.errors) {
        errors.push_back(error);
    }
    
    return errors;
}

std::vector<std::string> Config::getValidationWarnings() const {
    std::vector<std::string> warnings;
    ValidationResult result = validateBrainSystem();
    
    for (const auto& warning : result.warnings) {
        warnings.push_back(warning);
    }
    
    return warnings;
}

std::string Config::generateDocumentation(const std::string& title) const {
    std::stringstream ss;
    ss << "# " << title << "\n\n";
    ss << "Generated: " << generateTimestamp() << "\n\n";
    
    auto keys = getKeys();
    ss << "## Configuration Parameters (" << keys.size() << " total)\n\n";
    
    for (const auto& key : keys) {
        ss << "### " << key << "\n";
        
        // Try to get value with different types
        auto valueOpt = get<std::string>(key);
        std::string valueStr;
        if (valueOpt) {
            valueStr = *valueOpt;
        } else {
            auto intVal = get<int>(key);
            if (intVal) {
                valueStr = std::to_string(*intVal);
            } else {
                auto doubleVal = get<double>(key);
                if (doubleVal) {
                    valueStr = std::to_string(*doubleVal);
                } else {
                    auto boolVal = get<bool>(key);
                    if (boolVal) {
                        valueStr = *boolVal ? "true" : "false";
                    }
                }
            }
        }
        
        ss << "- **Value**: `" << valueStr << "`\n";
        ss << "- **Type**: " << (valueOpt ? "string" : 
                            intVal ? "int" :
                            doubleVal ? "double" :
                            boolVal ? "bool" : "unknown") << "\n";
        
        // Get description from schema if available
        const auto* paramInfo = schema_.getParameter(key);
        if (paramInfo) {
            ss << "- **Description**: " << paramInfo->description << "\n";
        }
        
        ss << "\n";
    }
    
    return ss.str();
}

std::string Config::compareWith(const Config& otherConfig) const {
    return ConfigurationManager::compareConfigs(shared_from_this(), std::const_pointer_cast<Config>(&otherConfig));
}

ValidationResult Config::validateBrainSystem() const {
    ValidationResult result;
    
    // Get the brain system schema
    ConfigSchema brainSchema = createBrainSystemSchema();
    
    // Validate against the schema
    return brainSchema.validate(shared_from_this());
}

ValidationResult Config::validate(const ConfigSchema& schema) const {
    return schema.validate(shared_from_this());
}

std::optional<std::string> Config::validateParameter(const std::string& parameterName) const {
    const auto* paramInfo = schema_.getParameter(parameterName);
    if (!paramInfo) {
        return "Parameter not defined in schema: " + parameterName;
    }
    
    ValidationContext context;
    context.config = shared_from_this();
    context.parameterName = parameterName;
    context.schema = &schema_;
    
    if (!paramInfo->validate(context)) {
        return paramInfo->getErrorMessages(context);
    }
    
    return std::nullopt;
}

template<typename T>
void Config::addValidator(const std::string& parameterName, const T& validator) {
    const auto* paramInfo = schema_.getParameter(parameterName);
    if (!paramInfo) {
        throw std::runtime_error("Parameter not found in schema: " + parameterName);
    }
    
    paramInfo->addValidator(std::make_unique<T>(validator));
}

bool Config::shouldValidateParameter(const std::string& parameterName) const {
    const auto* paramInfo = schema_.getParameter(parameterName);
    if (!paramInfo) {
        return false;
    }
    
    // Check conditional dependencies
    if (!paramInfo->conditionalOnParameter.empty()) {
        auto dependentValue = get<std::string>(paramInfo->conditionalOnParameter);
        if (!dependentValue) {
            return false; // Skip validation if dependent parameter is missing
        }
        
        return *dependentValue == paramInfo->conditionalValue;
    }
    
    return true;
}

std::string Config::getEnvironmentVariable(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    return value ? value : "";
}

bool Config::setEnvironmentVariable(const std::string& name, const std::string& value) {
    return std::setenv(name.c_str(), value.c_str(), 1) == 0;
}

ConfigSchema Config::createBrainSystemSchema() {
    ConfigSchema schema("Brain System Configuration");
    
    // Add all the default brain system parameters
    Config defaultConfig;
    
    // Get all parameter names and create schema entries
    auto keys = defaultConfig.getKeys();
    
    for (const auto& key : keys) {
        ConfigSchema::ParameterInfo param;
        param.name = key;
        param.type = "unknown"; // Could determine type from value
        param.required = true;
        
        // Get description based on parameter name
        if (key == "neuron_count") {
            param.description = "Number of neurons in the brain (must be > 0)";
            param.addValidator(std::make_unique<RangeValidator>(1.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Neuron count must be >= 1"));
        } else if (key == "connection_probability") {
            param.description = "Probability of connection between neurons (0-1)";
            param.addValidator(std::make_unique<RangeValidator>(0.0, 1.0,
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Connection probability must be between 0 and 1"));
        } else if (key == "simulation_timestep") {
            param.description = "Simulation timestep in seconds (must be > 0)";
            param.addValidator(std::make_unique<RangeValidator>(0.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Timestep must be positive"));
        } else if (key == "stdp_ltp_weight") {
            param.description = "STDP long-term potentiation weight";
            param.addValidator(std::make_unique<RangeValidator>(0.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "STDP LTP weight must be positive"));
        } else if (key == "stdp_ltd_weight") {
            param.description = "STDP long-term depression weight (must be < stdp_ltp_weight)";
            param.addValidator(std::make_unique<RangeValidator>(-1000.0, 1000.0,
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "STDP LTD weight must be reasonable"));
        } else if (key == "stdp_tau") {
            param.description = "STDP time constant (must be > 0)";
            param.addValidator(std::make_unique<RangeValidator>(0.1, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "STDP tau must be > 0.1"));
        } else if (key == "memory_capacity") {
            param.description = "Memory system capacity (must be > 0)";
            param.addValidator(std::make_unique<RangeValidator>(1.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Memory capacity must be >= 1"));
        } else if (key == "max_episodes") {
            param.description = "Maximum number of episodes for episodic memory (must be > 0)";
            param.addValidator(std::make_unique<RangeValidator>(1.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Max episodes must be >= 1"));
        } else if (key == "synaptogenesis_rate") {
            param.description = "Rate of new synapse formation (plasticity rate)";
            param.addValidator(std::make_unique<RangeValidator>(0.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Synaptogenesis rate must be positive"));
        } else if (key == "pruning_rate") {
            param.description = "Rate of synapse elimination (plasticity rate)";
            param.addValidator(std::make_unique<RangeValidator>(0.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Pruning rate must be positive"));
        } else if (key == "dopamine_threshold") {
            param.description = "Dopamine neuromodulation threshold";
            param.addValidator(std::make_unique<RangeValidator>(0.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Dopamine threshold must be positive"));
        } else if (key == "curiosity_threshold") {
            param.description = "Curiosity neuromodulation threshold";
            param.addValidator(std::make_unique<RangeValidator>(0.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Curiosity threshold must be positive"));
        } else if (key == "region_count") {
            param.description = "Number of brain regions";
            param.addValidator(std::make_unique<RangeValidator>(1.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Region count must be >= 1"));
        } else if (key == "random_seed") {
            param.description = "Random seed for reproducibility";
            param.addValidator(std::make_unique<RangeValidator>(0.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Random seed must be >= 0"));
        } else if (key == "replay_interval") {
            param.description = "Memory replay interval (steps)";
            param.addValidator(std::make_unique<RangeValidator>(1.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Replay interval must be >= 1"));
        } else if (key == "consolidation_interval") {
            param.description = "Memory consolidation interval (steps)";
            param.addValidator(std::make_unique<RangeValidator>(1.0, std::numeric_limits<double>::max(),
                                                               RangeValidator::BoundType::Inclusive,
                                                               RangeValidator::BoundType::Inclusive,
                                                               "Consolidation interval must be >= 1"));
        } else {
            param.description = "Configuration parameter: " + key;
        }
        
        schema.addParameter(param);
    }
    
    return schema;
}

EnhancedConfigSchema Config::createEnhancedBrainSystemSchema(const ConfigSchema* baseSchema) {
    EnhancedConfigSchema enhancedSchema("Enhanced Brain System Configuration");
    
    // Create groups for organization
    EnhancedConfigSchema::SchemaGroup neuronGroup("Neuron System", "Parameters related to neurons");
    EnhancedConfigSchema::SchemaGroup plasticityGroup("Plasticity", "Parameters related to synaptic plasticity");
    EnhancedConfigSchema::SchemaGroup memoryGroup("Memory", "Parameters related to memory systems");
    EnhancedConfigSchema::SchemaGroup neuromodulationGroup("Neuromodulation", "Parameters related to neuromodulation");
    EnhancedConfigGroup("Development", "Parameters related to system development");
    
    // Add parameters to groups
    neuronGroup.parameters = {"neuron_count", "connection_probability", "region_count"};
    plasticityGroup.parameters = {"stdp_ltp_weight", "stdp_ltd_weight", "stdp_tau", 
                                  "synaptogenesis_rate", "pruning_rate"};
    memoryGroup.parameters = {"memory_capacity", "max_episodes", "replay_interval", "consolidation_interval"};
    neuromodGroup.parameters = {"dopamine_threshold", "curiosity_threshold"};
    developmentGroup.parameters = {"plasticity_rates", "stage_transitions"};
    
    // Add groups to schema
    enhancedSchema.addGroup(neuronGroup);
    enhancedSchema.addGroup(plasticityGroup);
    enhancedSchema.addGroup(memoryGroup);
    enhancedSchema.addGroup(neuromodGroup);
    enhancedSchema.addGroup(developmentGroup);
    
    return enhancedSchema;
}

bool Config::backup(const std::string& backupPath) const {
    return ConfigurationManager::backupConfig(shared_from_this(), backupPath);
}

bool Config::restore(const std::string& backupPath) {
    return ConfigurationManager::restoreConfig(std::const_pointer_cast<Config>(shared_from_this()), backupPath);
}

} // namespace nlm
