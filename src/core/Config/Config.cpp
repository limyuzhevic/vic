#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
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

// Configuration validation and error handling implementation
bool Config::validateConfig() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        std::string error = validateValue(entry.key, entry.value);
        if (!error.empty()) {
            errors.push_back("Key \"" + entry.key + "\": " + error);
        }
    }
    
    // Check for required parameters
    const auto& paramInfos = getParameterInfos();
    for (const auto& paramInfo : paramInfos) {
        if (paramInfo.required && !has(paramInfo.name)) {
            errors.push_back("Required parameter \"" + paramInfo.name + "\" is missing");
        }
    }
    
    // Check for sensible defaults
    if (!hasSensibleDefaults()) {
        auto suggestions = getConfigurationSuggestions();
        for (const auto& suggestion : suggestions) {
            errors.push_back("Configuration suggestion: " + suggestion);
        }
    }
    
    return errors.empty();
}

std::string Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Parameter validation
    for (const auto& entry : pImpl->entries) {
        std::string error = validateValue(entry.key, entry.value);
        if (!error.empty()) {
            errors.push_back("Key \"" + entry.key + "\": " + error);
        }
    }
    
    // Check for required parameters
    const auto& paramInfos = getParameterInfos();
    for (const auto& paramInfo : paramInfos) {
        if (paramInfo.required && !has(paramInfo.name)) {
            errors.push_back("Required parameter \"" + paramInfo.name + "\" is missing");
        }
    }
    
    // Format errors
    std::ostringstream oss;
    if (errors.empty()) {
        oss << "Configuration is valid.";
    } else {
        oss << "Configuration validation errors:";
        for (const auto& error : errors) {
            oss << "\n  - " << error;
        }
    }
    
    return oss.str();
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Check if key exists in parameter definitions
    const auto& paramInfos = getParameterInfos();
    auto paramIt = std::find_if(paramInfos.begin(), paramInfos.end(),
                                [&key](const ParameterInfo& info) { return info.name == key; });
    
    if (paramIt == paramInfos.end()) {
        // Unknown parameter - could be a custom extension
        return true;
    }
    
    const ParameterInfo& paramInfo = *paramIt;
    
    // Validate based on parameter type
    try {
        if (std::holds_alternative<int>(value)) {
            int intValue = std::get<int>(value);
            if (paramInfo.minValue != std::numeric_limits<float>::lowest() && 
                intValue < paramInfo.minValue) {
                return "Value " + std::to_string(intValue) + " is less than minimum " + std::to_string(paramInfo.minValue);
            }
            if (paramInfo.maxValue != std::numeric_limits<float>::max() &&
                intValue > paramInfo.maxValue) {
                return "Value " + std::to_string(intValue) + " exceeds maximum " + std::to_string(paramInfo.maxValue);
            }
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t int64Value = std::get<int64_t>(value);
            if (paramInfo.minValue != std::numeric_limits<float>::lowest() && 
                static_cast<float>(int64Value) < paramInfo.minValue) {
                return "Value " + std::to_string(int64Value) + " is less than minimum " + std::to_string(paramInfo.minValue);
            }
            if (paramInfo.maxValue != std::numeric_limits<float>::max() &&
                static_cast<float>(int64Value) > paramInfo.maxValue) {
                return "Value " + std::to_string(int64Value) + " exceeds maximum " + std::to_string(paramInfo.maxValue);
            }
        } else if (std::holds_alternative<double>(value)) {
            double doubleValue = std::get<double>(value);
            if (paramInfo.minValue != std::numeric_limits<float>::lowest() && 
                doubleValue < paramInfo.minValue) {
                return "Value " + std::to_string(doubleValue) + " is less than minimum " + std::to_string(paramInfo.minValue);
            }
            if (paramInfo.maxValue != std::numeric_limits<float>::max() &&
                doubleValue > paramInfo.maxValue) {
                return "Value " + std::to_string(doubleValue) + " exceeds maximum " + std::to_string(paramInfo.maxValue);
            }
        } else if (std::holds_alternative<bool>(value)) {
            // Boolean validation - no range checks needed
        } else if (std::holds_alternative<std::string>(value)) {
            const std::string& strValue = std::get<std::string>(value);
            if (!paramInfo.validValues.empty()) {
                bool valid = std::find(paramInfo.validValues.begin(), paramInfo.validValues.end(), strValue) != paramInfo.validValues.end();
                if (!valid) {
                    std::string validStr = "";
                    for (size_t i = 0; i < paramInfo.validValues.size(); ++i) {
                        if (i > 0) validStr += ", ";
                        validStr += "\"" + paramInfo.validValues[i] + "\"";
                    }
                    return "Value must be one of: " + validStr;
                }
            }
        }
    } catch (const std::bad_variant_access&) {
        return "Invalid value type for parameter \"" + key + "\"";
    }
    
    return "";
}

// Static parameter information
const std::vector<Config::ParameterInfo>& Config::getParameterInfos() {
    static const std::vector<ParameterInfo> parameterInfos = {
        // Basic configuration
        ParameterInfo("random_seed", "Random seed for reproducible experiments", 
                     static_cast<int64_t>(42), ConfigSource::Default, false, 0, 2147483647),
        
        ParameterInfo("simulation_timestep", "Simulation timestep in seconds",
                     static_cast<double>(0.001), ConfigSource::Default, false, 0.0001, 1.0),
        
        ParameterInfo("neuron_count", "Number of neurons in the brain",
                     static_cast<int64_t>(500), ConfigSource::Default, true, 1, 100000),
        
        ParameterInfo("region_count", "Number of brain regions",
                     static_cast<int64_t>(1), ConfigSource::Default, false, 1, 1000),
        
        ParameterInfo("connection_probability", "Connection probability for random connectivity",
                     static_cast<double>(0.15), ConfigSource::Default, false, 0.0, 1.0),
        
        // STDP parameters
        ParameterInfo("stdp_ltp_weight", "STDP long-term potentiation weight",
                     static_cast<double>(0.02), ConfigSource::Default, false, 0.0, 1.0),
        
        ParameterInfo("stdp_ltd_weight", "STDP long-term depression weight",
                     static_cast<double>(0.015), ConfigSource::Default, false, 0.0, 1.0),
        
        ParameterInfo("stdp_tau", "STDP time constant",
                     static_cast<double>(20.0), ConfigSource::Default, false, 1.0, 1000.0),
        
        // Hebbian plasticity
        ParameterInfo("hebbian_learning_rate", "Hebbian learning rate",
                     static_cast<double>(0.01), ConfigSource::Default, false, 0.0, 1.0),
        
        // Structural plasticity
        ParameterInfo("synaptogenesis_rate", "Structural plasticity synaptogenesis rate",
                     static_cast<double>(0.0001), ConfigSource::Default, false, 0.0, 0.01),
        
        ParameterInfo("pruning_rate", "Structural plasticity pruning rate",
                     static_cast<double>(0.00001), ConfigSource::Default, false, 0.0, 0.01),
        
        // Neuromodulation
        ParameterInfo("dopamine_level", "Neuromodulation dopamine level",
                     static_cast<double>(0.1), ConfigSource::Default, false, 0.0, 1.0),
        
        ParameterInfo("curiosity_level", "Neuromodulation curiosity level",
                     static_cast<double>(0.5), ConfigSource::Default, false, 0.0, 1.0),
        
        ParameterInfo("novelty_threshold", "Novelty detection threshold",
                     static_cast<double>(0.05), ConfigSource::Default, false, 0.0, 1.0),
        
        // Simulation features
        ParameterInfo("enable_structural_plasticity", "Enable structural plasticity",
                     true, ConfigSource::Default, false),
        
        ParameterInfo("enable_neuromodulation", "Enable neuromodulation",
                     true, ConfigSource::Default, false),
        
        ParameterInfo("enable_prediction_learning", "Enable prediction learning",
                     true, ConfigSource::Default, false),
        
        ParameterInfo("enable_cognitive_functions", "Enable cognitive functions",
                     true, ConfigSource::Default, false),
        
        // Performance and logging
        ParameterInfo("log_level", "Logging level", std::string("INFO"), ConfigSource::Default, false),
        ParameterInfo("log_to_file", "Log to file", false, ConfigSource::Default, false),
        
        ParameterInfo("max_simulation_steps", "Maximum simulation steps",
                     static_cast<int64_t>(10000), ConfigSource::Default, false, 100, 1000000),
        
        ParameterInfo("visualization_enabled", "Enable visualization",
                     false, ConfigSource::Default, false),
        
        ParameterInfo("visualization_update_rate", "Visualization update rate (Hz)",
                     static_cast<double>(30.0), ConfigSource::Default, false, 1.0, 1000.0),
    };
    
    return parameterInfos;
}

Config::ParameterInfo Config::getParameterInfo(const std::string& key) {
    const auto& paramInfos = getParameterInfos();
    auto it = std::find_if(paramInfos.begin(), paramInfos.end(),
                          [&key](const ParameterInfo& info) { return info.name == key; });n    
    if (it != paramInfos.end()) {
        return *it;
    }
    
    // Return empty info for unknown keys
    return ParameterInfo();
}

bool Config::hasSensibleDefaults() const {
    bool hasIssues = false;
    
    // Check for reasonable neuron counts
    if (auto neuronCount = get<int64_t>("neuron_count")) {
        if (*neuronCount < 10 || *neuronCount > 100000) {
            hasIssues = true;
        }
    }
    
    // Check for reasonable connection probability
    if (auto connProb = get<double>("connection_probability")) {
        if (*connProb < 0.0 || *connProb > 1.0) {
            hasIssues = true;
        }
    }
    
    // Check for reasonable timesteps
    if (auto timestep = get<double>("simulation_timestep")) {
        if (*timestep < 0.0001 || *timestep > 1.0) {
            hasIssues = true;
        }
    }
    
    // Check that critical parameters are present
    const auto& paramInfos = getParameterInfos();
    bool hasCriticalMissing = false;
    for (const auto& paramInfo : paramInfos) {
        if (paramInfo.required && !has(paramInfo.name)) {
            hasCriticalMissing = true;
            break;
        }
    }
    
    if (hasCriticalMissing) {
        hasIssues = true;
    }
    
    return !hasIssues;
}

std::vector<std::string> Config::getConfigurationSuggestions() const {
    std::vector<std::string> suggestions;
    
    // Suggest smaller network for testing
    if (auto neuronCount = get<int64_t>("neuron_count")) {
        if (*neuronCount > 10000) {
            suggestions.push_back("Consider reducing neuron_count for faster testing (e.g., 1000-5000 neurons)");
        }
    }
    
    // Suggest appropriate connection probability
    if (auto connProb = get<double>("connection_probability")) {
        if (*connProb > 0.5) {
            suggestions.push_back("Connection probability > 0.5 may create overly dense networks; consider 0.1-0.3 for typical neural networks");
        }
    }
    
    // Suggest appropriate timesteps
    if (auto timestep = get<double>("simulation_timestep")) {
        if (*timestep > 0.1) {
            suggestions.push_back("Simulation timestep > 0.1 may cause instability; consider 0.001-0.01 for typical simulations");
        }
    }
    
    // Suggest enabling features if disabled
    if (auto enablePlasticity = get<bool>("enable_structural_plasticity")) {
        if (!*enablePlasticity) {
            suggestions.push_back("Structural plasticity is disabled; consider enabling for learning capabilities");
        }
    }
    
    return suggestions;
}

} // namespace nlm
