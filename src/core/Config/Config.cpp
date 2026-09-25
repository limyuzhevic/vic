#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <variant>
#include <optional>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace nlm {

// JSON helper functions
std::string toJson(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "["; 
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

ConfigValue fromJson(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        // Determine array type
        if (!j.empty() && j[0].is_string()) {
            std::vector<std::string> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<std::string>());
            }
            return vec;
        } else if (!j.empty() && j[0].is_number_integer()) {
            std::vector<int> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<int>());
            }
            return vec;
        } else {
            std::vector<double> vec;
            for (const auto& item : j) {
                vec.push_back(item.get<double>());
            }
            return vec;
        }
    }
    throw std::runtime_error("Unsupported JSON type in configuration");
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Configuration metadata for validation
    std::map<std::string, std::string> descriptions;
    std::map<std::string, std::string> valueTypes;
    std::map<std::string, std::pair<double, double>> valueRanges;
    std::map<std::string, bool> requiredFields;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration metadata
    initializeDefaults();
}

void Config::initializeDefaults() {
    // Register configuration parameters with validation rules
    
    // Basic parameters
    pImpl->descriptions["nlm_version"] = "NLM configuration version";
    pImpl->valueTypes["nlm_version"] = "string";
    pImpl->requiredFields["nlm_version"] = true;
    
    pImpl->descriptions["brain_region_count"] = "Number of brain regions";
    pImpl->valueTypes["brain_region_count"] = "int";
    pImpl->valueRanges["brain_region_count"] = {1, 100};
    pImpl->requiredFields["brain_region_count"] = true;
    
    pImpl->descriptions["brain_neuron_count"] = "Number of neurons in the brain";
    pImpl->valueTypes["brain_neuron_count"] = "int";
    pImpl->valueRanges["brain_neuron_count"] = {10, 10000};
    pImpl->requiredFields["brain_neuron_count"] = true;
    
    // Neural dynamics
    pImpl->descriptions["simulation_timestep"] = "Simulation timestep in seconds";
    pImpl->valueTypes["simulation_timestep"] = "double";
    pImpl->valueRanges["simulation_timestep"] = {1e-6, 1.0};
    pImpl->requiredFields["simulation_timestep"] = true;
    
    pImpl->descriptions["neuron_resting_potential"] = "Neuron resting potential (mV)";
    pImpl->valueTypes["neuron_resting_potential"] = "double";
    pImpl->valueRanges["neuron_resting_potential"] = {-100.0, -40.0};
    
    pImpl->descriptions["neuron_threshold_potential"] = "Neuron firing threshold (mV)";
    pImpl->valueTypes["neuron_threshold_potential"] = "double";
    pImpl->valueRanges["neuron_threshold_potential"] = {-60.0, -30.0};
    
    // Plasticity
    pImpl->descriptions["stdp_ltp_weight"] = "STDP long-term potentiation weight";
    pImpl->valueTypes["stdp_ltp_weight"] = "double";
    pImpl->valueRanges["stdp_ltp_weight"] = {0.0, 1.0};
    
    pImpl->descriptions["stdp_ltd_weight"] = "STDP long-term depression weight";
    pImpl->valueTypes["stdp_ltd_weight"] = "double";
    pImpl->valueRanges["stdp_ltd_weight"] = {0.0, 1.0};
    
    // Environment
    pImpl->descriptions["environment_type"] = "Type of environment";
    pImpl->valueTypes["environment_type"] = "string";
    pImpl->requiredFields["environment_type"] = true;
    
    pImpl->descriptions["environment_width"] = "Environment width in units";
    pImpl->valueTypes["environment_width"] = "int";
    pImpl->valueRanges["environment_width"] = {1, 1000};
    
    // Logging
    pImpl->descriptions["log_level"] = "Logging level";
    pImpl->valueTypes["log_level"] = "string";
    pImpl->requiredFields["log_level"] = true;
    
    pImpl->descriptions["log_to_file"] = "Whether to log to file";
    pImpl->valueTypes["log_to_file"] = "bool";
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // First try to load as JSON
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Fall back to legacy key=value format
    return loadFromLegacyFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Configuration file not found: " + filepath);
            return false;
        }
        
        nlohmann::json j;
        file >> j;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON
        if (j.contains("nlm_version")) {
            set("nlm_version", j["nlm_version"].get<std::string>(), ConfigSource::File);
        }
        
        // Process configuration section
        if (j.contains("configuration") && j["configuration"].is_object()) {
            const auto& config = j["configuration"];
            for (auto it = config.begin(); it != config.end(); ++it) {
                try {
                    ConfigValue value = fromJson(it.value());
                    set(it.key(), value, ConfigSource::File);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Failed to parse configuration value " + it.key() + ": " + e.what());
                }
            }
        }
        
        // Process neural dynamics section
        if (j.contains("neural_dynamics") && j["neural_dynamics"].is_object()) {
            const auto& dynamics = j["neural_dynamics"];
            for (auto it = dynamics.begin(); it != dynamics.end(); ++it) {
                set("neural_" + it.key(), fromJson(it.value()), ConfigSource::File);
            }
        }
        
        // Process plasticity section
        if (j.contains("plasticity") && j["plasticity"].is_object()) {
            const auto& plasticity = j["plasticity"];
            for (auto it = plasticity.begin(); it != plasticity.end(); ++it) {
                set("plasticity_" + it.key(), fromJson(it.value()), ConfigSource::File);
            }
        }
        
        // Process neuromodulation section
        if (j.contains("neuromodulation") && j["neuromodulation"].is_object()) {
            const auto& neuromod = j["neuromodulation"];
            for (auto it = neuromod.begin(); it != neuromod.end(); ++it) {
                set("neuromod_" + it.key(), fromJson(it.value()), ConfigSource::File);
            }
        }
        
        // Process environment section
        if (j.contains("environment") && j["environment"].is_object()) {
            const auto& env = j["environment"];
            for (auto it = env.begin(); it != env.end(); ++it) {
                set("env_" + it.key(), fromJson(it.value()), ConfigSource::File);
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON: " + filepath);
        validateConfiguration();
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load JSON configuration: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromLegacyFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Configuration file not found, using defaults: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from legacy format: " + filepath);
    validateConfiguration();
    return true;
}

void Config::validateConfiguration() {
    std::vector<std::string> errors;
    
    for (const auto& [key, isRequired] : pImpl->requiredFields) {
        if (isRequired && !has(key)) {
            errors.push_back("Missing required configuration parameter: " + key);
        }
    }
    
    for (const auto& entry : pImpl->entries) {
        const auto& key = entry.key;
        
        // Check value type
        if (pImpl->valueTypes.find(key) != pImpl->valueTypes.end()) {
            const std::string& expectedType = pImpl->valueTypes[key];
            const std::string& actualType = getValueTypeName(entry.value);
            
            if (expectedType != "any" && actualType != expectedType) {
                errors.push_back("Configuration parameter " + key + 
                               " has wrong type. Expected " + expectedType + ", got " + actualType);
            }
        }
        
        // Check value ranges
        if (pImpl->valueRanges.find(key) != pImpl->valueRanges.end()) {
            const auto& [min, max] = pImpl->valueRanges[key];
            double value = getValueAsDouble(entry.value);
            if (value < min || value > max) {
                errors.push_back("Configuration parameter " + key + 
                               " value " + std::to_string(value) + 
                               " outside valid range [" + std::to_string(min) + ", " + std::to_string(max) + "]");
            }
        }
    }
    
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  " + error);
        }
        throw std::runtime_error("Configuration validation failed");
    } else {
        NLM_LOG_INFO("Configuration validation passed");
    }
}

std::string Config::getValueTypeName(const ConfigValue& value) {
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

double Config::getValueAsDouble(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return static_cast<double>(std::get<int>(value));
    if (std::holds_alternative<int64_t>(value)) return static_cast<double>(std::get<int64_t>(value));
    if (std::holds_alternative<double>(value)) return std::get<double>(value);
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? 1.0 : 0.0;
    if (std::holds_alternative<std::string>(value)) {
        // Try to parse as double
        try {
            return std::stod(std::get<std::string>(value));
        } catch (...) {
            return 0.0; // Default for non-numeric strings
        }
    }
    return 0.0;
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
        NLM_LOG_ERROR("Cannot save configuration to file: " + filepath);
        return false;
    }
    
    file << "# Enhanced NLM Configuration File" << std::endl;
    file << "# Generated on " << getCurrentTime() << std::endl;
    file << "# Version: " << getOr<std::string>("nlm_version", "2.0") << std::endl;
    file << std::endl;
    
    // Write configuration in organized sections
    file << "# === BASIC CONFIGURATION ===" << std::endl;
    writeSection(file, "Basic settings", {"nlm_version", "brain_region_count", "brain_neuron_count"});
    
    file << "# === NEURAL DYNAMICS ===" << std::endl;
    writeSection(file, "Neural dynamics", {"simulation_timestep", "neuron_resting_potential", 
                                          "neuron_threshold_potential", "neuron_reset_potential",
                                          "neuron_time_constant", "neuron_membrane_capacitance"});
    
    file << "# === PLASTICITY ===" << std::endl;
    writeSection(file, "Plasticity", {"stdp_ltp_weight", "stdp_ltd_weight", "stdp_time_constant",
                                       "hebbian_learning_rate", "structural_plasticity_rate", "pruning_rate"});
    
    file << "# === NEUROMODULATION ===" << std::endl;
    writeSection(file, "Neuromodulation", {"dopamine_baseline", "dopamine_modulation_strength",
                                          "curiosity_baseline", "reward_prediction_error_scale"});
    
    file << "# === ENVIRONMENT ===" << std::endl;
    writeSection(file, "Environment", {"environment_type", "environment_width", "environment_height",
                                       "sensory_resolution"});
    
    file << "# === MEMORY SYSTEMS ===" << std::endl;
    writeSection(file, "Memory", {"working_memory_capacity", "episodic_memory_duration",
                                  "associative_memory_patterns"});
    
    file << "# === DEVELOPMENT ===" << std::endl;
    writeSection(file, "Development", {"developmental_stage", "development_plasticity_boost",
                                       "development_synaptogenesis_rate", "development_pruning_rate"});
    
    file << "# === SIMULATION ===" << std::endl;
    writeSection(file, "Simulation", {"enable_plasticity", "enable_structural_plasticity",
                                       "enable_development", "enable_replay", "enable_checkpointing",
                                       "replay_interval", "consolidation_interval"});
    
    file << "# === LOGGING ===" << std::endl;
    writeSection(file, "Logging", {"log_level", "log_to_file", "log_filename", "console_output",
                                   "summary_interval"});
    
    file << "# === PERFORMANCE ===" << std::endl;
    writeSection(file, "Performance", {"enable_optimizations", "use_event_queue",
                                       "enable_parallelization", "simd_optimization",
                                       "memory_pool_enabled"});
    
    file << "# === VISUALIZATION ===" << std::endl;
    writeSection(file, "Visualization", {"visualization_enabled", "visualization_update_rate",
                                          "visualize_neurons", "visualize_synapses",
                                          "visualize_activity", "visualize_weights"});
    
    // Write any additional custom entries
    file << "# === ADDITIONAL SETTINGS ===" << std::endl;
    for (const auto& entry : pImpl->entries) {
        if (!isInStandardSection(entry.key)) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << toJson(entry.value) << "\n";
            file << std::endl;
        }
    }
    
    file << "# End of configuration file" << std::endl;
    return true;
}

void Config::writeSection(std::ofstream& file, const std::string& sectionName, 
                         const std::vector<std::string>& keys) const {
    file << "# " << sectionName << "\n";
    for (const auto& key : keys) {
        if (has(key)) {
            file << "# " << (pImpl->descriptions.find(key) != pImpl->descriptions.end() ? 
                            pImpl->descriptions.at(key) : "") << "\n";
            file << key << " = " << toJson(getOr<ConfigValue>(key)) << "\n";
        }
    }
    file << std::endl;
}

bool Config::isInStandardSection(const std::string& key) const {
    std::vector<std::string> standardSections = {
        "nlm_version", "brain_region_count", "brain_neuron_count",
        "simulation_timestep", "neuron_resting_potential", "neuron_threshold_potential",
        "neuron_reset_potential", "neuron_time_constant", "neuron_membrane_capacitance",
        "stdp_ltp_weight", "stdp_ltd_weight", "stdp_time_constant", "hebbian_learning_rate",
        "structural_plasticity_rate", "pruning_rate", "dopamine_baseline", "dopamine_modulation_strength",
        "curiosity_baseline", "reward_prediction_error_scale", "environment_type", "environment_width",
        "environment_height", "sensory_resolution", "working_memory_capacity", "episodic_memory_duration",
        "associative_memory_patterns", "developmental_stage", "development_plasticity_boost",
        "development_synaptogenesis_rate", "development_pruning_rate", "enable_plasticity",
        "enable_structural_plasticity", "enable_development", "enable_replay", "enable_checkpointing",
        "replay_interval", "consolidation_interval", "log_level", "log_to_file", "log_filename",
        "console_output", "summary_interval", "enable_optimizations", "use_event_queue",
        "enable_parallelization", "simd_optimization", "memory_pool_enabled", "visualization_enabled",
        "visualization_update_rate", "visualize_neurons", "visualize_synapses", "visualize_activity",
        "visualize_weights"
    };
    
    return std::find(standardSections.begin(), standardSections.end(), key) != standardSections.end();
}

std::string Config::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
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

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate before setting
    if (pImpl->valueTypes.find(key) != pImpl->valueTypes.end()) {
        const std::string& expectedType = pImpl->valueTypes[key];
        const std::string& actualType = getValueTypeName(value);
        
        if (expectedType != "any" && actualType != expectedType) {
            throw std::runtime_error("Configuration parameter " + key + 
                                   " has wrong type. Expected " + expectedType + ", got " + actualType);
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, 
                                   pImpl->descriptions.find(key) != pImpl->descriptions.end() ? 
                                   pImpl->descriptions.at(key) : "");
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
    oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ") - " 
            << (pImpl->descriptions.find(entry.key) != pImpl->descriptions.end() ? 
                pImpl->descriptions.at(entry.key) : "") << "\n";
    }
    
    oss << "\nConfiguration validation: ";
    try {
        validateConfiguration();
        oss << "PASSED";
    } catch (const std::exception& e) {
        oss << "FAILED: " << e.what();
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
namespace nlm {

// Explicit instantiations for common types
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

} // namespace nlm

