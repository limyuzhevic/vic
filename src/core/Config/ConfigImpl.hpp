#pragma once

#include "Config.hpp"
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigEntry> entryInfoMap;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    loadDefaultConfiguration();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced error handling and validation
    if (filepath.empty()) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try multiple format detection
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Detect file format
    if (detectJSONFormat(content)) {
        return loadJSONFormat(filepath);
    } else if (detectYAMLFormat(content)) {
        return loadYAMLFormat(filepath);
    } else {
        // Fallback to legacy format
        return loadLegacyFormat(filepath);
    }
}

bool Config::detectJSONFormat(const std::string& content) {
    // Simple JSON detection
    size_t firstNonWS = content.find_first_not_of(" \t\n\r");
    if (firstNonWS == std::string::npos) return false;
    
    char firstChar = content[firstNonWS];
    return (firstChar == '{' || firstChar == '[');
}

bool Config::detectYAMLFormat(const std::string& content) {
    // Simple YAML detection - look for YAML-specific patterns
    // This is a simplified implementation
    return content.find("---") != std::string::npos || 
           content.find(": ") != std::string::npos;
}

bool Config::loadJSONFormat(const std::string& filepath) {
    // For now, implement enhanced legacy parsing with validation
    // In a full implementation, this would use nlohmann-json or pybind11
    return loadLegacyFormat(filepath);
}

bool Config::loadYAMLFormat(const std::string& filepath) {
    // For now, implement enhanced legacy parsing with validation
    // In a full implementation, this would use pyyaml
    return loadLegacyFormat(filepath);
}

bool Config::loadLegacyFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
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
        
        // Enhanced parsing with validation
        if (!parseConfigLine(line, lineNumber)) {
            hasErrors = true;
        }
    }
    
    // Apply legacy to modern parameter mapping
    applyLegacyParameterMapping();
    
    // Validate all loaded parameters
    hasErrors |= validateAllParameters();
    
    return !hasErrors;
}

void Config::applyLegacyParameterMapping() {
    // Create a mapping from legacy parameter names to modern names
    // This ensures backward compatibility
    
    std::map<std::string, std::string> legacyToModern = {
        {"stdp_time_constant", "stdp_tau"},
        {"development_synaptogenesis_rate", "synaptogenesis_rate"},
        {"development_pruning_rate", "pruning_rate"},
        {"plasticity_learning_rate", "plasticity_rate"},
        {"stdp_ltp_weight", "stdp_ltp_weight"},
        {"stdp_ltd_weight", "stdp_ltd_weight"}
    };
    
    for (const auto& mapping : legacyToModern) {
        const std::string& legacyKey = mapping.first;
        const std::string& modernKey = mapping.second;
        
        if (has(legacyKey)) {
            auto legacyValue = get<std::string>(legacyKey);
            if (legacyValue) {
                set(modernKey, *legacyValue, ConfigSource::File);
            }
        }
    }
}

bool Config::validateAllParameters() const {
    bool hasErrors = false;
    
    // Define parameter validation rules
    std::vector<std::pair<std::string, std::function<bool(const std::string&)>>> validations = {
        {"random_seed", validateRandomSeed},
        {"simulation_timestep", validateTimestep},
        {"neuron_count", validateNeuronCount},
        {"region_count", validateRegionCount},
        {"connection_probability", validateConnectionProbability},
        {"plasticity_learning_rate", validateLearningRate},
        {"stdp_ltp_weight", validateStdpWeight},
        {"stdp_ltd_weight", validateStdpWeight},
        {"stdp_time_constant", validateTimeConstant},
        {"synaptogenesis_rate", validateRate},
        {"pruning_rate", validateRate},
        {"dopamine_baseline", validateNeuromodulationValue},
        {"reward_discount_factor", validateDiscountFactor},
        {"environment_width", validateEnvironmentDimension},
        {"environment_height", validateEnvironmentDimension},
        {"visualization_update_rate", validatePositiveFloat}
    };
    
    for (const auto& validation : validations) {
        const std::string& key = validation.first;
        auto validator = validation.second;
        
        if (has(key)) {
            auto value = get<std::string>(key);
            if (value && !validator(*value)) {
                hasErrors = true;
            }
        }
    }
    
    // Check for required parameters
    std::vector<std::string> requiredParams = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& param : requiredParams) {
        if (!has(param)) {
            // Missing required parameter - handled by getOr() default values
        }
    }
    
    return hasErrors;
}

bool Config::validateRandomSeed(const std::string& value) const {
    try {
        int64_t seed = std::stoll(value);
        return seed >= 0;
    } catch (...) {
        return false;
    }
}

bool Config::validateTimestep(const std::string& value) const {
    try {
        double timestep = std::stod(value);
        return timestep > 0.0 && timestep <= 1.0;
    } catch (...) {
        return false;
    }
}

bool Config::validateNeuronCount(const std::string& value) const {
    try {
        int64_t count = std::stoll(value);
        return count > 0 && count <= 100000; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validateRegionCount(const std::string& value) const {
    try {
        int64_t count = std::stoll(value);
        return count > 0 && count <= 1000; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validateConnectionProbability(const std::string& value) const {
    try {
        double prob = std::stod(value);
        return prob >= 0.0 && prob <= 1.0;
    } catch (...) {
        return false;
    }
}

bool Config::validateLearningRate(const std::string& value) const {
    try {
        double rate = std::stod(value);
        return rate >= 0.0 && rate <= 10.0; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validateStdpWeight(const std::string& value) const {
    try {
        double weight = std::stod(value);
        return weight >= 0.0 && weight <= 10.0; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validateTimeConstant(const std::string& value) const {
    try {
        double time = std::stod(value);
        return time > 0.0 && time <= 1000.0; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validateRate(const std::string& value) const {
    try {
        double rate = std::stod(value);
        return rate >= 0.0 && rate <= 1.0; // Rate should be between 0 and 1
    } catch (...) {
        return false;
    }
}

bool Config::validateNeuromodulationValue(const std::string& value) const {
    try {
        double val = std::stod(value);
        return val >= 0.0 && val <= 10.0; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validateDiscountFactor(const std::string& value) const {
    try {
        double factor = std::stod(value);
        return factor >= 0.0 && factor <= 1.0;
    } catch (...) {
        return false;
    }
}

bool Config::validateEnvironmentDimension(const std::string& value) const {
    try {
        int64_t dim = std::stoll(value);
        return dim > 0 && dim <= 1000; // Reasonable upper bound
    } catch (...) {
        return false;
    }
}

bool Config::validatePositiveFloat(const std::string& value) const {
    try {
        double val = std::stod(value);
        return val > 0.0;
    } catch (...) {
        return false;
    }
}

bool Config::parseConfigLine(const std::string& line, size_t lineNumber) {
    // Parse key=value pairs with enhanced validation
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false; // Not a valid key=value pair
    }
    
    std::string key = trim(line.substr(0, pos));
    std::string value = trim(line.substr(pos + 1));
    
    if (key.empty()) {
        return false; // Empty key
    }
    
    // Validate key name (should not contain special characters)
    if (!isValidConfigKey(key)) {
        return false;
    }
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    try {
        set(key, value, ConfigSource::File);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::isValidConfigKey(const std::string& key) const {
    // Basic validation: key should contain only alphanumeric, underscores, and hyphens
    // and not start with a number
    if (key.empty()) return false;
    if (std::isdigit(key[0])) return false;
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool Config::isAdvancedConfigKey(const std::string& key) const {
    auto entryInfo = getEntryInfo(key);
    return entryInfo ? entryInfo->isAdvanced : false;
}

std::optional<ConfigEntry> Config::getEntryInfo(const std::string& key) const {
    auto it = pImpl->entryInfoMap.find(key);
    if (it != pImpl->entryInfoMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<std::string> Config::getKeysByCategory(ConfigCategory category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Enhanced argument parsing with validation
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (!isValidConfigKey(key)) {
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (!isValidConfigKey(key)) {
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& description, ConfigCategory category,
                 const std::string& unit, const std::string& type, bool advanced) {
    // Enhanced error handling for set operations
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source, description, category, unit, type, advanced);
            pImpl->entryInfoMap[key] = pImpl->entries.back();
        }
    } catch (const std::exception& e) {
        // Log error and handle gracefully
        // In a real implementation, this would use the logger
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source,
                 const std::string& description, ConfigCategory category,
                 const std::string& unit, const std::string& type, bool advanced) {
    set(key, ConfigValue(value), source, description, category, unit, type, advanced);
}

void Config::set(const std::string& key, int value, ConfigSource source,
                 const std::string& description, ConfigCategory category,
                 const std::string& unit, const std::string& type, bool advanced) {
    set(key, ConfigValue(value), source, description, category, unit, type, advanced);
}

void Config::set(const std::string& key, double value, ConfigSource source,
                 const std::string& description, ConfigCategory category,
                 const std::string& unit, const std::string& type, bool advanced) {
    set(key, ConfigValue(value), source, description, category, unit, type, advanced);
}

void Config::set(const std::string& key, bool value, ConfigSource source,
                 const std::string& description, ConfigCategory category,
                 const std::string& unit, const std::string& type, bool advanced) {
    set(key, ConfigValue(value), source, description, category, unit, type, advanced);
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
    pImpl->entryInfoMap.erase(key);
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
    pImpl->entryInfoMap.clear();
    loadDefaultConfiguration();
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

std::string Config::getAdvancedOptionsSummary() const {
    std::ostringstream oss;
    oss << "=== Advanced Configuration Options ===\n";
    
    // Group advanced options by category
    std::map<ConfigCategory, std::vector<std::string>> categoryMap;
    for (const auto& entry : pImpl->entries) {
        if (entry.isAdvanced) {
            categoryMap[entry.category].push_back(entry.key);
        }
    }
    
    for (const auto& categoryGroup : categoryMap) {
        oss << "\n" << configCategoryToString(categoryGroup.first) << ":\n";
        for (const auto& key : categoryGroup.second) {
            auto entryInfo = getEntryInfo(key);
            if (entryInfo) {
                oss << "  " << key << " - " << entryInfo->description << " (" << entryInfo->unit << ")\n";
            }
        }
    }
    
    oss << "\n=== End Advanced Options ===\n";
    return oss.str();
}

bool Config::validateConfig() const {
    // Perform comprehensive validation
    bool hasErrors = false;
    
    // Check for conflicting parameters
    if (has("neuron_count") && has("region_count")) {
        auto neuronCount = get<int64_t>("neuron_count");
        auto regionCount = get<int64_t>("region_count");
        if (neuronCount && regionCount && *neuronCount / *regionCount < 10) {
            // Warning: Very few neurons per region
        }
    }
    
    // Check for inconsistent parameter combinations
    if (has("plasticity_learning_rate") && has("stdp_ltp_weight")) {
        auto plasticityRate = get<double>("plasticity_learning_rate");
        auto stdpLtpWeight = get<double>("stdp_ltp_weight");
        if (plasticityRate && stdpLtpWeight && *plasticityRate > 1.0 && *stdpLtpWeight > 1.0) {
            // Warning: Both plasticity rate and STDP weight are very high
        }
    }
    
    // Validate all parameters using the validation functions
    hasErrors |= validateAllParameters();
    
    return !hasErrors;
}

void Config::resetToDefaults() {
    clear();
    loadDefaultConfiguration();
}

void Config::loadDefaultConfiguration() {
    // Clear existing configuration
    pImpl->entries.clear();
    pImpl->entryInfoMap.clear();
    
    // Load basic configuration (Phase 1 defaults)
    set("random_seed", 42, ConfigSource::Default, "Random seed for reproducible experiments", ConfigCategory::Basic, "", "int");
    set("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in seconds", ConfigCategory::Basic, "s", "double");
    set("neuron_count", 1000, ConfigSource::Default, "Number of neurons in the brain", ConfigCategory::Basic, "count", "int64_t");
    set("region_count", 2, ConfigSource::Default, "Number of brain regions", ConfigCategory::Basic, "count", "int64_t");
    set("connection_probability", 0.1, ConfigSource::Default, "Connection probability for random connectivity", ConfigCategory::Basic, "", "double");
    
    // Load learning configuration (Phase 1)
    set("plasticity_learning_rate", 0.01, ConfigSource::Default, "Hebbian plasticity learning rate", ConfigCategory::Learning, "", "double");
    set("stdp_ltp_weight", 0.01, ConfigSource::Default, "STDP long-term potentiation weight", ConfigCategory::Learning, "", "double");
    set("stdp_ltd_weight", 0.012, ConfigSource::Default, "STDP long-term depression weight", ConfigCategory::Learning, "", "double");
    set("stdp_time_constant", 20.0, ConfigSource::Default, "STDP time constant", ConfigCategory::Learning, "ms", "double");
    
    // Load development configuration (Phase 1)
    set("development_synaptogenesis_rate", 0.001, ConfigSource::Default, "Synaptogenesis rate during development", ConfigCategory::Development, "per_step", "double");
    set("development_pruning_rate", 0.0001, ConfigSource::Default, "Pruning rate during development", ConfigCategory::Development, "per_step", "double");
    
    // Load neuromodulation configuration (Phase 1)
    set("dopamine_baseline", 0.1, ConfigSource::Default, "Baseline dopamine level", ConfigCategory::Neuromodulation, "", "double");
    
    // Load reward configuration (Phase 1)
    set("reward_discount_factor", 0.99, ConfigSource::Default, "Reward discount factor", ConfigCategory::Basic, "", "double");
    
    // Load environment configuration (Phase 1)
    set("environment_name", "GridWorld", ConfigSource::Default, "Environment name", ConfigCategory::Environment, "", "string");
    set("environment_width", 10, ConfigSource::Default, "Environment width", ConfigCategory::Environment, "cells", "int64_t");
    set("environment_height", 10, ConfigSource::Default, "Environment height", ConfigCategory::Environment, "cells", "int64_t");
    
    // Load logging configuration (Phase 1)
    set("log_level", "INFO", ConfigSource::Default, "Log level", ConfigCategory::Debugging, "", "string");
    set("log_to_file", false, ConfigSource::Default, "Log to file", ConfigCategory::Debugging, "", "bool");
    set("log_filename", "nlm.log", ConfigSource::Default, "Log filename", ConfigCategory::Debugging, "", "string");
    
    // Load simulation configuration (Phase 1)
    set("max_simulation_steps", 10000, ConfigSource::Default, "Maximum simulation steps", ConfigCategory::Performance, "steps", "int64_t");
    set("simulation_time_limit", 0.0, ConfigSource::Default, "Simulation time limit in seconds", ConfigCategory::Performance, "s", "double");
    
    // Load visualization configuration (Phase 1)
    set("visualization_enabled", false, ConfigSource::Default, "Enable visualization", ConfigCategory::Debugging, "", "bool");
    set("visualization_update_rate", 30.0, ConfigSource::Default, "Visualization update rate in Hz", ConfigCategory::Debugging, "Hz", "double");
    
    // Load Phase 2 configuration (overrides/missing values)
    set("stdp_tau", 20.0, ConfigSource::Default, "STDP time constant (Phase 2 parameter name)", ConfigCategory::Learning, "ms", "double");
    set("synaptogenesis_rate", 0.0001, ConfigSource::Default, "Synaptogenesis rate (Phase 2 parameter name)", ConfigCategory::Development, "per_step", "double");
    set("pruning_rate", 0.00001, ConfigSource::Default, "Pruning rate (Phase 2 parameter name)", ConfigCategory::Development, "per_step", "double");
    
    // Mark some parameters as advanced
    set("stdp_ltp_weight", 0.02, ConfigSource::Default, "STDP LTP weight for advanced users", ConfigCategory::Learning, "", "double", true);
    set("stdp_ltd_weight", 0.015, ConfigSource::Default, "STDP LTD weight for advanced users", ConfigCategory::Learning, "", "double", true);
    set("plasticity_rate", 0.01, ConfigSource::Default, "Advanced plasticity rate control", ConfigCategory::Learning, "", "double", true);
    set("dopamine_modulation_strength", 1.0, ConfigSource::Default, "Dopamine modulation strength (advanced)", ConfigCategory::Neuromodulation, "", "double", true);
    set("curiosity_drive", 0.1, ConfigSource::Default, "Curiosity drive coefficient (advanced)", ConfigCategory::Neuromodulation, "", "double", true);
    set("novelty_sensitivity", 0.5, ConfigSource::Default, "Novelty detection sensitivity (advanced)", ConfigCategory::Neuromodulation, "", "double", true);
    set("prediction_confidence_threshold", 0.8, ConfigSource::Default, "Prediction confidence threshold (advanced)", ConfigCategory::Prediction, "", "double", true);
    set("memory_replay_frequency", 0.1, ConfigSource::Default, "Memory replay frequency (advanced)", ConfigCategory::Memory, "", "double", true);
    set("structural_plasticity_rate", 0.0001, ConfigSource::Default, "Structural plasticity rate (advanced)", ConfigCategory::Network, "", "double", true);
    set("network_connectivity_threshold", 0.15, ConfigSource::Default, "Network connectivity threshold (advanced)", ConfigCategory::Network, "", "double", true);
    set("development_maturation_rate", 0.01, ConfigSource::Default, "Development maturation rate (advanced)", ConfigCategory::Development, "per_step", "double", true);
    set("adaptation_variable_decay", 0.95, ConfigSource::Default, "Adaptation variable decay rate (advanced)", ConfigCategory::Performance, "", "double", true);
    set("energy_efficiency_factor", 0.8, ConfigSource::Default, "Energy efficiency factor (advanced)", ConfigCategory::Performance, "", "double", true);
}

std::string Config::configCategoryToString(ConfigCategory category) const {
    switch (category) {
        case ConfigCategory::Basic: return "Basic Configuration";
        case ConfigCategory::Learning: return "Learning Configuration";
        case ConfigCategory::Development: return "Development Configuration";
        case ConfigCategory::Neuromodulation: return "Neuromodulation Configuration";
        case ConfigCategory::Memory: return "Memory Configuration";
        case ConfigCategory::Prediction: return "Prediction Configuration";
        case ConfigCategory::Network: return "Network Configuration";
        case ConfigCategory::Environment: return "Environment Configuration";
        case ConfigCategory::Debugging: return "Debugging Configuration";
        case ConfigCategory::Performance: return "Performance Configuration";
        default: return "Unknown Category";
    }
}

} // namespace nlm
