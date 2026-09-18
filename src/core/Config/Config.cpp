#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Forward declaration for validation function
namespace {
    bool validateInternal(const Config* config);
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration values
    setRandomSeed(42);
    setSimulationTimestep(0.001);
    setNeuronCount(1000);
    setRegionCount(2);
    setConnectionProbability(0.1);
    setPlasticityLearningRate(0.01);
    setStdpLtpWeight(0.01);
    setStdpLtdWeight(0.012);
    setStdpTimeConstant(20.0f);
    setDevelopmentSynaptogenesisRate(0.001);
    setDevelopmentPruningRate(0.0001);
    setDopamineBaseline(0.1);
    setRewardDiscountFactor(0.99);
    setEnvironmentName("GridWorld");
    setEnvironmentWidth(10);
    setEnvironmentHeight(10);
    setLogLevel("INFO");
    setLogToFile(false);
    setLogFilename("nlm.log");
    setMaxSimulationSteps(10000);
    setSimulationTimeLimit(0.0);
    setVisualizationEnabled(false);
    setVisualizationUpdateRate(30.0f);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced error handling for configuration file loading
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
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
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_WARNING("Configuration file " + filepath + ", line " + 
                               std::to_string(lineNumber) + ": Empty key name");
                hasErrors = true;
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Configuration file " + filepath + ", line " + 
                           std::to_string(lineNumber) + ": Invalid format, missing '='");
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Configuration file " + filepath + " loaded with warnings");
    }
    
    return true;
}

// Enhanced command-line argument loading with validation
bool Config::loadFromArgs(int argc, char** argv) {
    bool hasErrors = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key format
                if (key.empty()) {
                    NLM_LOG_ERROR("Invalid command-line argument: empty key in " + arg);
                    hasErrors = true;
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_ERROR("Invalid command-line argument format: " + arg);
                hasErrors = true;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key format
            if (key.empty()) {
                NLM_LOG_ERROR("Invalid command-line argument: empty key in " + arg);
                hasErrors = true;
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
        else {
            NLM_LOG_WARNING("Skipping unrecognized command-line argument: " + arg);
        }
    }
    
    if (hasErrors) {
        NLM_LOG_ERROR("Command-line argument parsing completed with errors");
    }
    
    return true;
}

// Enhanced save function with proper error handling
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to save configuration file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM configuration system\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "# Source: " << static_cast<int>(entry.source) << "\n\n";
    }
    
    NLM_LOG_INFO("Configuration saved successfully to: " + filepath);
    return true;
}

// Configuration validation implementation
void validateConfig(const Config* config) {
    // Internal validation logic
    bool isValid = true;
    
    for (const auto& entry : config->pImpl->entries) {
        if (entry.key.empty()) {
            NLM_LOG_ERROR("Invalid configuration entry: empty key");
            isValid = false;
            continue;
        }
        
        // Type-specific validation
        std::string key = entry.key;
        std::string lowerKey = toLower(key);
        
        if (lowerKey == "neuron_count" || lowerKey == "region_count" || 
            lowerKey == "max_simulation_steps") {
            if (std::holds_alternative<int64_t>(entry.value)) {
                int64_t val = std::get<int64_t>(entry.value);
                if (val <= 0) {
                    NLM_LOG_ERROR("Invalid value for " + entry.key + ": " + 
                                 std::to_string(val) + 
                                 ". Must be positive");
                    isValid = false;
                }
            }
        }
        else if (lowerKey == "connection_probability" || 
                 lowerKey == "plasticity_learning_rate" || 
                 lowerKey == "stdp_ltp_weight" || 
                 lowerKey == "stdp_ltd_weight" || 
                 lowerKey == "stdp_time_constant" || 
                 lowerKey == "development_synaptogenesis_rate" || 
                 lowerKey == "development_pruning_rate" || 
                 lowerKey == "dopamine_baseline" || 
                 lowerKey == "reward_discount_factor" || 
                 lowerKey == "visualization_update_rate") {
            if (std::holds_alternative<float>(entry.value)) {
                float val = std::get<float>(entry.value);
                if (val < 0.0f) {
                    NLM_LOG_ERROR("Invalid value for " + entry.key + ": " + 
                                 std::to_string(val) + 
                                 ". Must be non-negative");
                    isValid = false;
                }
            }
        }
        else if (lowerKey == "simulation_timestep" || 
                 lowerKey == "simulation_time_limit") {
            if (std::holds_alternative<double>(entry.value)) {
                double val = std::get<double>(entry.value);
                if (val < 0.0) {
                    NLM_LOG_ERROR("Invalid value for " + entry.key + ": " + 
                                 std::to_string(val) + 
                                 ". Must be non-negative");
                    isValid = false;
                }
            }
        }
        else if (lowerKey == "environment_width" || lowerKey == "environment_height") {
            if (std::holds_alternative<int>(entry.value)) {
                int val = std::get<int>(entry.value);
                if (val <= 0) {
                    NLM_LOG_ERROR("Invalid value for " + entry.key + ": " + 
                                 std::to_string(val) + 
                                 ". Must be positive");
                    isValid = false;
                }
            }
        }
    }
    
    if (!isValid) {
        NLM_LOG_ERROR("Configuration validation failed");
    } else {
        NLM_LOG_INFO("Configuration validation passed");
    }
}

// Add validation method to Config class
bool Config::validate() const {
    return validateInternal(this);
}

// Alias for validation
bool validateConfig(const Config* config) {
    return validateInternal(config);
}

// Internal validation helper - standalone function
bool validateInternal(const Config* config) {
    bool isValid = true;
    
    for (const auto& entry : config->pImpl->entries) {
        if (entry.key.empty()) {
            NLM_LOG_ERROR("Invalid configuration entry: empty key");
            isValid = false;
            continue;
        }
        
        // Type-specific validation
        std::string key = entry.key;
        std::string lowerKey = toLower(key);
        
        if (lowerKey == "neuron_count" || lowerKey == "region_count" || 
            lowerKey == "max_simulation_steps") {
            if (std::holds_alternative<int64_t>(entry.value)) {
                int64_t val = std::get<int64_t>(entry.value);
                if (val <= 0) {
                    NLM_LOG_ERROR("Invalid value for " + entry.key + ": " + 
                                 std::to_string(val) + 
                                 ". Must be positive");
                    isValid = false;
                }
            }
        }
        else if (lowerKey == "connection_probability" || 
                 lowerKey == "plasticity_learning_rate" || 
                 lowerKey == "stdp_ltp_weight" || 
                 lowerKey == "stdp_ltd_weight" || 
                 lowerKey == "stdp_time_constant" || 
                 lowerKey == "development_synaptogenesis_rate" || 
                 lowerKey == "development_pruning_rate" || 
                 lowerKey == "dopamine_baseline" || 
                 lowerKey == "reward_discount_factor" || 
                 lowerKey == "visualization_update_rate") {
            if (std::holds_alternative<float>(entry.value)) {
                float val = std::get<float>(entry.value);
                if (val < 0.0f) {
                    NLM_LOG_ERROR("Invalid value for " + entry.key + ": " + 
                                 std::to_string(val) + 
                                 ". Must be non-negative");
                    isValid = false;
                }
            }
        }
        else if (lowerKey == "simulation_timestep" || 
                 lowerKey == "simulation_time_limit") {
            if (std::holds_alternative<double>(entry.value)) {
                double val = std::get<double>(entry.value);
                if (val < 0.0) {
                    NLM_LOG_ERROR("Invalid value for " + entry.key + ": " + 
                                 std::to_string(val) + 
                                 ". Must be non-negative");
                    isValid = false;
                }
            }
        }
        else if (lowerKey == "environment_width" || lowerKey == "environment_height") {
            if (std::holds_alternative<int>(entry.value)) {
                int val = std::get<int>(entry.value);
                if (val <= 0) {
                    NLM_LOG_ERROR("Invalid value for " + entry.key + ": " + 
                                 std::to_string(val) + 
                                 ". Must be positive");
                    isValid = false;
                }
            }
        }
    }
    
    if (!isValid) {
        NLM_LOG_ERROR("Configuration validation failed");
    } else {
        NLM_LOG_INFO("Configuration validation passed");
    }
    
    return isValid;
}

// Template implementations
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

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
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

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Add type-safe set/get implementations
void Config::setRandomSeed(int value, ConfigSource source) { set("random_seed", value, source); }
void Config::setSimulationTimestep(double value, ConfigSource source) { set("simulation_timestep", value, source); }
void Config::setNeuronCount(int64_t value, ConfigSource source) { set("neuron_count", value, source); }
void Config::setRegionCount(int64_t value, ConfigSource source) { set("region_count", value, source); }
void Config::setConnectionProbability(float value, ConfigSource source) { set("connection_probability", value, source); }
void Config::setPlasticityLearningRate(float value, ConfigSource source) { set("plasticity_learning_rate", value, source); }
void Config::setStdpLtpWeight(float value, ConfigSource source) { set("stdp_ltp_weight", value, source); }
void Config::setStdpLtdWeight(float value, ConfigSource source) { set("stdp_ltd_weight", value, source); }
void Config::setStdpTimeConstant(float value, ConfigSource source) { set("stdp_time_constant", value, source); }
void Config::setDevelopmentSynaptogenesisRate(float value, ConfigSource source) { set("development_synaptogenesis_rate", value, source); }
void Config::setDevelopmentPruningRate(float value, ConfigSource source) { set("development_pruning_rate", value, source); }
void Config::setDopamineBaseline(float value, ConfigSource source) { set("dopamine_baseline", value, source); }
void Config::setRewardDiscountFactor(float value, ConfigSource source) { set("reward_discount_factor", value, source); }
void Config::setEnvironmentName(const std::string& value, ConfigSource source) { set("environment_name", value, source); }
void Config::setEnvironmentWidth(int value, ConfigSource source) { set("environment_width", value, source); }
void Config::setEnvironmentHeight(int value, ConfigSource source) { set("environment_height", value, source); }
void Config::setLogLevel(const std::string& value, ConfigSource source) { set("log_level", value, source); }
void Config::setLogToFile(bool value, ConfigSource source) { set("log_to_file", value, source); }
void Config::setLogFilename(const std::string& value, ConfigSource source) { set("log_filename", value, source); }
void Config::setMaxSimulationSteps(int64_t value, ConfigSource source) { set("max_simulation_steps", value, source); }
void Config::setSimulationTimeLimit(double value, ConfigSource source) { set("simulation_time_limit", value, source); }
void Config::setVisualizationEnabled(bool value, ConfigSource source) { set("visualization_enabled", value, source); }
void Config::setVisualizationUpdateRate(float value, ConfigSource source) { set("visualization_update_rate", value, source); }

// Type-safe getters
int Config::getOrRandomSeed(int defaultValue) const { return getOr<int>("random_seed", defaultValue); }
double Config::getOrSimulationTimestep(double defaultValue) const { return getOr<double>("simulation_timestep", defaultValue); }
int64_t Config::getOrNeuronCount(int64_t defaultValue) const { return getOr<int64_t>("neuron_count", defaultValue); }
int64_t Config::getOrRegionCount(int64_t defaultValue) const { return getOr<int64_t>("region_count", defaultValue); }
float Config::getOrConnectionProbability(float defaultValue) const { return getOr<float>("connection_probability", defaultValue); }
float Config::getOrPlasticityLearningRate(float defaultValue) const { return getOr<float>("plasticity_learning_rate", defaultValue); }
float Config::getOrStdpLtpWeight(float defaultValue) const { return getOr<float>("stdp_ltp_weight", defaultValue); }
float Config::getOrStdpLtdWeight(float defaultValue) const { return getOr<float>("stdp_ltd_weight", defaultValue); }
float Config::getOrStdpTimeConstant(float defaultValue) const { return getOr<float>("stdp_time_constant", defaultValue); }
float Config::getOrDevelopmentSynaptogenesisRate(float defaultValue) const { return getOr<float>("development_synaptogenesis_rate", defaultValue); }
float Config::getOrDevelopmentPruningRate(float defaultValue) const { return getOr<float>("development_pruning_rate", defaultValue); }
float Config::getOrDopamineBaseline(float defaultValue) const { return getOr<float>("dopamine_baseline", defaultValue); }
float Config::getOrRewardDiscountFactor(float defaultValue) const { return getOr<float>("reward_discount_factor", defaultValue); }
std::string Config::getOrEnvironmentName(const std::string& defaultValue) const { return getOr<std::string>("environment_name", defaultValue); }
int Config::getOrEnvironmentWidth(int defaultValue) const { return getOr<int>("environment_width", defaultValue); }
int Config::getOrEnvironmentHeight(int defaultValue) const { return getOr<int>("environment_height", defaultValue); }
std::string Config::getOrLogLevel(const std::string& defaultValue) const { return getOr<std::string>("log_level", defaultValue); }
bool Config::getOrLogToFile(bool defaultValue) const { return getOr<bool>("log_to_file", defaultValue); }
std::string Config::getOrLogFilename(const std::string& defaultValue) const { return getOr<std::string>("log_filename", defaultValue); }
int64_t Config::getOrMaxSimulationSteps(int64_t defaultValue) const { return getOr<int64_t>("max_simulation_steps", defaultValue); }
double Config::getOrSimulationTimeLimit(double defaultValue) const { return getOr<double>("simulation_time_limit", defaultValue); }
bool Config::getOrVisualizationEnabled(bool defaultValue) const { return getOr<bool>("visualization_enabled", defaultValue); }
float Config::getOrVisualizationUpdateRate(float defaultValue) const { return getOr<float>("visualization_update_rate", defaultValue); }

} // namespace nlm
