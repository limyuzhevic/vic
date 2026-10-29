// Unified Configuration System Implementation
// 
// This file implements the UnifiedConfig class defined in UnifiedConfig.hpp.
// It provides configuration management with validation, parameter definitions,
// and backward compatibility with the legacy Config system.

#include "UnifiedConfig.hpp"
#include "Config.hpp"  // For legacy compatibility
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <filesystem>

namespace nlm {

// Parameter registry - static initialization
std::unordered_map<std::string, ConfigParameter> UnifiedConfig::parameterRegistry_ = UnifiedConfig::initializeParameterRegistry();

// Parameter validation function templates
namespace {

// Helper function to trim whitespace
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function to convert string to lowercase
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // anonymous namespace

// Initialize the parameter registry with all known parameters
std::unordered_map<std::string, ConfigParameter> UnifiedConfig::initializeParameterRegistry() {
    std::unordered_map<std::string, ConfigParameter> registry;
    
    // Core parameters
    registry["neuron_count"] = {
        "neuron_count", ConfigCategory::Core, "int",
        "Number of neurons in the brain",
        500.0, 1.0, 1000000.0, false, false
    };
    
    registry["region_count"] = {
        "region_count", ConfigCategory::Core, "int",
        "Number of brain regions",
        1.0, 1.0, 100.0, false, false
    };
    
    registry["connection_probability"] = {
        "connection_probability", ConfigCategory::Core, "float",
        "Probability of connection between neurons",
        0.1, 0.0, 1.0, false, false
    };
    
    registry["random_seed"] = {
        "random_seed", ConfigCategory::Core, "int64",
        "Random seed for reproducible results",
        42.0, 0.0, std::numeric_limits<int64_t>::max(), false, false
    };
    
    registry["simulation_timestep"] = {
        "simulation_timestep", ConfigCategory::Core, "double",
        "Simulation timestep in milliseconds",
        0.001, 0.0001, 1.0, false, false
    };
    
    // Neural parameters
    registry["lif_threshold"] = {
        "lif_threshold", ConfigCategory::Neural, "float",
        "LIF neuron threshold potential",
        -55.0, -100.0, 50.0, false, false
    };
    
    registry["lif_resting_potential"] = {
        "lif_resting_potential", ConfigCategory::Neural, "float",
        "LIF neuron resting potential",
        -70.0, -100.0, 0.0, false, false
    };
    
    registry["lif_time_constant"] = {
        "lif_time_constant", ConfigCategory::Neural, "float",
        "LIF neuron time constant",
        20.0, 1.0, 100.0, false, false
    };
    
    registry["membrane_capacitance"] = {
        "membrane_capacitance", ConfigCategory::Neural, "float",
        "Membrane capacitance in nF",
        1.0, 0.1, 10.0, false, false
    };
    
    // Plasticity parameters
    registry["stdp_ltp_weight"] = {
        "stdp_ltp_weight", ConfigCategory::Plasticity, "float",
        "STDP Long-Term Potentiation weight",
        0.01, 0.0, 1.0, false, false
    };
    
    registry["stdp_ltd_weight"] = {
        "stdp_ltd_weight", ConfigCategory::Plasticity, "float",
        "STDP Long-Term Depression weight",
        0.012, 0.0, 1.0, false, false
    };
    
    registry["stdp_tau"] = {
        "stdp_tau", ConfigCategory::Plasticity, "float",
        "STDP time constant",
        20.0, 1.0, 100.0, false, false
    };
    
    registry["hebbian_learning_rate"] = {
        "hebbian_learning_rate", ConfigCategory::Plasticity, "float",
        "Hebbian learning rate",
        0.01, 0.0, 1.0, false, false
    };
    
    registry["synaptogenesis_rate"] = {
        "synaptogenesis_rate", ConfigCategory::Plasticity, "float",
        "Rate of new synapse formation",
        0.0001, 0.0, 0.1, false, false
    };
    
    registry["pruning_rate"] = {
        "pruning_rate", ConfigCategory::Plasticity, "float",
        "Rate of synapse pruning",
        0.00001, 0.0, 0.01, false, false
    };
    
    // Environment parameters
    registry["environment_width"] = {
        "environment_width", ConfigCategory::Environment, "int",
        "Width of the environment (for Phase 3)",
        20.0, 1.0, 1000.0, false, true
    };
    
    registry["environment_height"] = {
        "environment_height", ConfigCategory::Environment, "int",
        "Height of the environment (for Phase 3)",
        20.0, 1.0, 1000.0, false, true
    };
    
    registry["vision_width"] = {
        "vision_width", ConfigCategory::Environment, "int",
        "Vision width (for Phase 3)",
        16.0, 1.0, 1000.0, false, true
    };
    
    registry["vision_height"] = {
        "vision_height", ConfigCategory::Environment, "int",
        "Vision height (for Phase 3)",
        16.0, 1.0, 1000.0, false, true
    };
    
    // Performance parameters
    registry["max_spike_history"] = {
        "max_spike_history", ConfigCategory::Advanced, "int",
        "Maximum spike history size per neuron",
        100.0, 10.0, 10000.0, false, false
    };
    
    registry["num_threads"] = {
        "num_threads", ConfigCategory::Advanced, "int",
        "Number of threads for parallel processing",
        0.0, 0.0, 64.0, false, false
    };
    
    registry["enable_vectorization"] = {
        "enable_vectorization", ConfigCategory::Advanced, "bool",
        "Enable SIMD vectorization",
        1.0, 0.0, 1.0, false, false
    };
    
    // Advanced parameters
    registry["enable_development"] = {
        "enable_development", ConfigCategory::Advanced, "bool",
        "Enable developmental processes",
        1.0, 0.0, 1.0, false, false
    };
    
    registry["enable_reward_modulation"] = {
        "enable_reward_modulation", ConfigCategory::Advanced, "bool",
        "Enable reward modulation",
        1.0, 0.0, 1.0, false, false
    };
    
    registry["enable_curiosity"] = {
        "enable_curiosity", ConfigCategory::Advanced, "bool",
        "Enable curiosity mechanisms",
        1.0, 0.0, 1.0, false, false
    };
    
    return registry;
}

bool UnifiedConfig::initialize(const std::string& profileName) {
    // Reset to defaults
    currentValues_.clear();
    valueRanges_.clear();
    
    // Load default values from profile
    const auto& profiles = getDefaultProfiles();
    auto it = profiles.find(profileName);
    if (it == profiles.end()) {
        throw ConfigValidationException("Unknown configuration profile: " + profileName);
    }
    
    const auto& profile = it->second;
    
    // Set default values
    for (const auto& paramEntry : parameterRegistry_) {
        const auto& param = paramEntry.second;
        auto valueIt = profile.find(param.name);
        
        if (valueIt != profile.end()) {
            currentValues_[param.name] = valueIt->second;
        } else {
            // Use parameter's default value
            currentValues_[param.name] = param.defaultValue;
        }
        
        // Store value range for validation
        valueRanges_[param.name] = std::make_pair(param.minValue, param.maxValue);
    }
    
    return true;
}

template<typename T>
T UnifiedConfig::get(const std::string& paramName, const T& defaultValue) const {
    auto it = currentValues_.find(paramName);
    if (it == currentValues_.end()) {
        return defaultValue;
    }
    
    return convertToType<T>(paramName);
}

template<typename T>
void UnifiedConfig::set(const std::string& paramName, const T& value,
                         ConfigCategory category) {
    auto paramIt = parameterRegistry_.find(paramName);
    if (paramIt == parameterRegistry_.end()) {
        throw ConfigParameterError(paramName, "Unknown parameter");
    }
    
    const auto& param = paramIt->second;
    
    // Validate parameter type
    if (!validateType<T>(param.type)) {
        throw ConfigParameterError(paramName, "Type mismatch: expected " + param.type);
    }
    
    // Validate value
    validateParameter<T>(paramName, value);
    
    // Store the value
    double doubleValue = valueToDouble<T>(value);
    currentValues_[paramName] = doubleValue;
}

bool UnifiedConfig::has(const std::string& paramName) const {
    return currentValues_.find(paramName) != currentValues_.end();
}

const ConfigParameter* UnifiedConfig::getParameterInfo(const std::string& paramName) const {
    auto it = parameterRegistry_.find(paramName);
    if (it == parameterRegistry_.end()) {
        return nullptr;
    }
    return &it->second;
}

std::vector<std::string> UnifiedConfig::getParametersInCategory(ConfigCategory category) const {
    std::vector<std::string> params;
    
    for (const auto& paramEntry : parameterRegistry_) {
        if (paramEntry.second.category == category) {
            params.push_back(paramEntry.first);
        }
    }
    
    return params;
}

bool UnifiedConfig::validateAll() const {
    for (const auto& valueEntry : currentValues_) {
        const std::string& paramName = valueEntry.first;
        double value = valueEntry.second;
        
        auto rangeIt = valueRanges_.find(paramName);
        if (rangeIt == valueRanges_.end()) {
            continue; // No range defined, skip validation
        }
        
        const auto& range = rangeIt->second;
        if (value < range.first || value > range.second) {
            throw ConfigValidationException("Parameter '" + paramName + "' has invalid value: " +
                std::to_string(value) + " (valid range: " + std::to_string(range.first) +
                " to " + std::to_string(range.second) + ")");
        }
    }
    
    return true;
}

bool UnifiedConfig::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            // Malformed line, skip with warning
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string valueStr = trim(line.substr(pos + 1));
        
        if (key.empty()) {
            continue;
        }
        
        try {
            setParameterFromString(key, valueStr);
        } catch (const std::exception& e) {
            // Invalid parameter, skip with warning
            continue;
        }
    }
    
    return validateAll();
}

bool UnifiedConfig::loadFromArgs(int argc, char** argv) {
    if (argv == nullptr) {
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == nullptr) {
            continue;
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (!key.empty()) {
                    try {
                        setParameterFromString(key, value);
                    } catch (const std::exception&) {
                        // Invalid argument, skip
                        continue;
                    }
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc && argv[i + 1] != nullptr) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (!key.empty() && !value.empty()) {
                try {
                    setParameterFromString(key, value);
                } catch (const std::exception&) {
                    // Invalid argument, skip
                    continue;
                }
            }
        }
    }
    
    return validateAll();
}

void UnifiedConfig::convertToLegacyConfig(class Config& legacyConfig) const {
    legacyConfig.clear();
    
    for (const auto& valueEntry : currentValues_) {
        const std::string& paramName = valueEntry.first;
        double value = valueEntry.second;
        
        // Convert double value to appropriate type
        const ConfigParameter* paramInfo = getParameterInfo(paramName);
        if (!paramInfo) {
            continue;
        }
        
        // Map parameter names between systems
        std::string legacyKey = paramName;
        if (paramName == "lif_threshold") legacyKey = "threshold";
        else if (paramName == "lif_resting_potential") legacyKey = "resting_potential";
        else if (paramName == "lif_time_constant") legacyKey = "time_constant";
        
        // Set the value based on type
        if (paramInfo->type == "int" || paramInfo->type == "int64") {
            legacyConfig.set(legacyKey, static_cast<int64_t>(value), ConfigSource::Runtime);
        } else if (paramInfo->type == "float") {
            legacyConfig.set(legacyKey, static_cast<float>(value), ConfigSource::Runtime);
        } else if (paramInfo->type == "double") {
            legacyConfig.set(legacyKey, value, ConfigSource::Runtime);
        } else if (paramInfo->type == "bool") {
            legacyConfig.set(legacyKey, value > 0.5, ConfigSource::Runtime);
        } else if (paramInfo->type == "string") {
            // String conversion not implemented in this example
            continue;
        }
    }
}

const std::unordered_map<std::string, std::map<std::string, double>> UnifiedConfig::getDefaultProfiles() {
    static const std::unordered_map<std::string, std::map<std::string, double>> profiles = {
        {
            "default",
            {
                {"neuron_count", 500},
                {"region_count", 1},
                {"connection_probability", 0.15},
                {"random_seed", 42},
                {"simulation_timestep", 0.001},
                {"lif_threshold", -55.0},
                {"lif_resting_potential", -70.0},
                {"lif_time_constant", 20.0},
                {"membrane_capacitance", 1.0},
                {"stdp_ltp_weight", 0.02},
                {"stdp_ltd_weight", 0.015},
                {"stdp_tau", 20.0},
                {"hebbian_learning_rate", 0.01},
                {"synaptogenesis_rate", 0.0001},
                {"pruning_rate", 0.00001},
                {"enable_development", 1.0},
                {"enable_reward_modulation", 1.0},
                {"enable_curiosity", 1.0},
            }
        },
        {
            "quick_test",
            {
                {"neuron_count", 100},
                {"region_count", 1},
                {"connection_probability", 0.1},
                {"random_seed", 42},
                {"simulation_timestep", 0.001},
                {"lif_threshold", -55.0},
                {"lif_resting_potential", -70.0},
                {"lif_time_constant", 20.0},
                {"membrane_capacitance", 1.0},
                {"stdp_ltp_weight", 0.01},
                {"stdp_ltd_weight", 0.012},
                {"stdp_tau", 20.0},
                {"hebbian_learning_rate", 0.01},
                {"synaptogenesis_rate", 0.0001},
                {"pruning_rate", 0.00001},
                {"enable_development", 1.0},
                {"enable_reward_modulation", 1.0},
                {"enable_curiosity", 1.0},
            }
        },
        {
            "research",
            {
                {"neuron_count", 5000},
                {"region_count", 5},
                {"connection_probability", 0.2},
                {"random_seed", 12345},
                {"simulation_timestep", 0.001},
                {"lif_threshold", -50.0},
                {"lif_resting_potential", -65.0},
                {"lif_time_constant", 25.0},
                {"membrane_capacitance", 1.5},
                {"stdp_ltp_weight", 0.03},
                {"stdp_ltd_weight", 0.02},
                {"stdp_tau", 30.0},
                {"hebbian_learning_rate", 0.02},
                {"synaptogenesis_rate", 0.001},
                {"pruning_rate", 0.0001},
                {"enable_development", 1.0},
                {"enable_reward_modulation", 1.0},
                {"enable_curiosity", 1.0},
            }
        }
    };
    
    return profiles;
}

template<typename T>
std::string UnifiedConfig::getTypeName() {
    if (std::is_same<T, int>::value) return "int";
    if (std::is_same<T, int64_t>::value) return "int64";
    if (std::is_same<T, float>::value) return "float";
    if (std::is_same<T, double>::value) return "double";
    if (std::is_same<T, bool>::value) return "bool";
    if (std::is_same<T, std::string>::value) return "string";
    return "unknown";
}

std::vector<std::string> UnifiedConfig::getAllParameterNames() const {
    std::vector<std::string> names;
    names.reserve(currentValues_.size());
    
    for (const auto& valueEntry : currentValues_) {
        names.push_back(valueEntry.first);
    }
    
    return names;
}

void UnifiedConfig::resetToDefaults() {
    currentValues_.clear();
    valueRanges_.clear();
    
    // Reinitialize with default profile
    initialize("default");
}

void UnifiedConfig::merge(const UnifiedConfig& other) {
    // Merge other config into this one (other takes precedence)
    for (const auto& valueEntry : other.currentValues_) {
        currentValues_[valueEntry.first] = valueEntry.second;
    }
}

// Template implementation details
template<typename T>
void UnifiedConfig::validateParameter(const std::string& paramName, const T& value) const {
    const ConfigParameter* param = getParameterInfo(paramName);
    if (!param) {
        throw ConfigParameterError(paramName, "Parameter not found");
    }
    
    // Check type
    if (!validateType<T>(param->type)) {
        throw ConfigParameterError(paramName, "Type mismatch: expected " + param->type);
    }
    
    // Check value range
    double doubleValue = valueToDouble<T>(value);
    double minValue = param->minValue;
    double maxValue = param->maxValue;
    
    if (doubleValue < minValue || doubleValue > maxValue) {
        throw ConfigParameterError(paramName, "Value " + std::to_string(doubleValue) + 
            " outside valid range [" + std::to_string(minValue) + ", " +
            std::to_string(maxValue) + "]");
    }
}

template<typename T>
T UnifiedConfig::convertToType(const std::string& paramName) const {
    auto it = currentValues_.find(paramName);
    if (it == currentValues_.end()) {
        throw ConfigParameterError(paramName, "Parameter not found");
    }
    
    return doubleToValue<T>(it->second);
}

template<typename T>
bool UnifiedConfig::validateType(const std::string& typeName) const {
    std::string expectedType = getTypeName<T>();
    return expectedType == typeName;
}

template<typename T>
double UnifiedConfig::valueToDouble(const T& value) {
    if (std::is_same<T, int>::value) return static_cast<double>(value);
    if (std::is_same<T, int64_t>::value) return static_cast<double>(value);
    if (std::is_same<T, float>::value) return static_cast<double>(value);
    if (std::is_same<T, double>::value) return value;
    if (std::is_same<T, bool>::value) return value ? 1.0 : 0.0;
    if (std::is_same<T, std::string>::value) {
        // String conversion not implemented
        return 0.0;
    }
    return 0.0;
}

template<typename T>
T UnifiedConfig::doubleToValue(double doubleValue) {
    if (std::is_same<T, int>::value) return static_cast<T>(static_cast<int>(doubleValue));
    if (std::is_same<T, int64_t>::value) return static_cast<T>(static_cast<int64_t>(doubleValue));
    if (std::is_same<T, float>::value) return static_cast<T>(static_cast<float>(doubleValue));
    if (std::is_same<T, double>::value) return static_cast<T>(doubleValue);
    if (std::is_same<T, bool>::value) return static_cast<T>(doubleValue > 0.5);
    if (std::is_same<T, std::string>::value) {
        // String conversion not implemented
        return T();
    }
    return T();
}

template<typename T>
void UnifiedConfig::throwParameterError(const std::string& paramName, const std::string& error) const {
    throw ConfigParameterError(paramName, error);
}

void UnifiedConfig::throwParameterError(const std::string& paramName, const std::string& error) const {
    throw ConfigParameterError(paramName, error);
}

template<typename T>
T UnifiedConfig::stringToValue(const std::string& str) {
    std::istringstream iss(str);
    T value;
    if (iss >> value) {
        return value;
    }
    throw ConfigParameterError("unknown", "Failed to parse value: " + str);
}

template<typename T>
std::string UnifiedConfig::valueToString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void UnifiedConfig::setParameterFromString(const std::string& key, const std::string& valueStr) {
    // Find parameter info
    const ConfigParameter* param = getParameterInfo(key);
    if (!param) {
        return; // Unknown parameter, skip
    }
    
    // Parse value based on type
    try {
        if (param->type == "int") {
            int value = stringToValue<int>(valueStr);
            set<int>(key, value);
        } else if (param->type == "int64") {
            int64_t value = stringToValue<int64_t>(valueStr);
            set<int64_t>(key, value);
        } else if (param->type == "float") {
            float value = stringToValue<float>(valueStr);
            set<float>(key, value);
        } else if (param->type == "double") {
            double value = stringToValue<double>(valueStr);
            set<double>(key, value);
        } else if (param->type == "bool") {
            bool value = (valueStr == "true" || valueStr == "1" || valueStr == "yes");
            set<bool>(key, value);
        }
    } catch (...) {
        // Parsing failed, skip
        return;
    }
}

// ConfigValidator implementation
bool ConfigValidator::isValidNeuronCount(double value) {
    return isValidValue<size_t>(value, 1.0, 1000000.0);
}

bool ConfigValidator::isValidRegionCount(double value) {
    return isValidValue<size_t>(value, 1.0, 100.0);
}

bool ConfigValidator::isValidConnectionProbability(double value) {
    return isValidValue<double>(value, 0.0, 1.0);
}

bool ConfigValidator::isValidRandomSeed(double value) {
    return isValidValue<int64_t>(value, 0.0, std::numeric_limits<int64_t>::max());
}

bool ConfigValidator::isValidSimulationTimestep(double value) {
    return isValidValue<double>(value, 0.0001, 1.0);
}

bool ConfigValidator::isValidPlasticityWeight(double value) {
    return isValidValue<double>(value, 0.0, 1.0);
}

bool ConfigValidator::isValidPlasticityTau(double value) {
    return isValidValue<double>(value, 1.0, 100.0);
}

bool ConfigValidator::isValidSynapticRate(double value) {
    return isValidValue<double>(value, 0.0, 0.1);
}

bool ConfigValidator::isValidEnvironmentDimension(double value) {
    return isValidValue<size_t>(value, 1.0, 1000.0);
}

bool ConfigValidator::isValidVisionDimension(double value) {
    return isValidValue<size_t>(value, 1.0, 1000.0);
}

bool ConfigValidator::isValidSpikeHistorySize(double value) {
    return isValidValue<size_t>(value, 10.0, 10000.0);
}

template<typename T>
bool ConfigValidator::isValidValue(double value, double min, double max) {
    if (std::is_same<T, int>::value) {
        return value >= min && value <= max && std::floor(value) == value;
    } else if (std::is_same<T, int64_t>::value) {
        return value >= min && value <= max && std::floor(value) == value;
    } else if (std::is_same<T, float>::value || std::is_same<T, double>::value) {
        return value >= min && value <= max;
    } else if (std::is_same<T, bool>::value) {
        return value == 0.0 || value == 1.0;
    }
    return false;
}

std::string ConfigValidator::getValidationError(const std::string& paramName, double value) {
    const ConfigParameter* param = UnifiedConfig::getParameterInfo(paramName);
    if (!param) {
        return "Unknown parameter: " + paramName;
    }
    
    std::string typeName;
    if (param->type == "int" || param->type == "int64") typeName = "integer";
    else if (param->type == "float" || param->type == "double") typeName = "number";
    else if (param->type == "bool") typeName = "boolean";
    else typeName = "value";
    
    return "Invalid " + typeName + " for parameter '" + paramName + "': " +
        std::to_string(value) + " (valid range: " + std::to_string(param->minValue) +
        " to " + std::to_string(param->maxValue) + ")";
}

} // namespace nlm
