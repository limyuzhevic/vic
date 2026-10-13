#include "EnhancedConfig.hpp"
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <zlib.h>
#include <xmlParser.h>

namespace nlm {

struct EnhancedConfig::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigProfile> profiles;
    EnhancedConfig* parent;
    std::string outputDirectory;
    bool monitoringEnabled;
    std::vector<std::pair<std::string, ConfigValue>> changedValues;
    time_t lastModified;
    
    // Validation
    std::vector<std::function<bool(const ConfigValue&, const std::string&)>> customValidators;
    
    Impl() : parent(nullptr), monitoringEnabled(false), lastModified(std::time(nullptr)) {
        // Create output directory
        std::filesystem::create_directories("."); // Simplified
    }
};

EnhancedConfig::EnhancedConfig() : pImpl(std::make_unique<Impl>()) {}

EnhancedConfig::~EnhancedConfig() = default;

EnhancedConfig::EnhancedConfig(EnhancedConfig&&) noexcept = default;

EnhancedConfig& EnhancedConfig::operator=(EnhancedConfig&&) noexcept = default;

bool EnhancedConfig::loadFromFile(const std::string& filepath) {
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return loadFromJSON(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Fall back to simple key=value format
        return pImpl->entries.empty() && loadFromSimpleFile(filepath);
    }
}

bool EnhancedConfig::loadFromJSON(const std::string& jsonString) {
    try {
        json j = json::parse(jsonString);
        fromJSONInternal(j);
        return true;
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("Failed to parse JSON configuration: " + std::string(e.what()));
        return false;
    }
}

bool EnhancedConfig::loadFromYAML(const std::string& yamlString) {
    // For now, fall back to simple format
    // In a full implementation, this would use a YAML parser like yaml-cpp
    NLM_LOG_WARNING("YAML loading not implemented, using simple format");
    return loadFromSimpleFile(yamlString);
}

bool EnhancedConfig::loadFromCommandLine(int argc, char** argv) {
    bool success = pImpl->entries.empty();
    
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
        // Handle boolean flags
        else if (arg.substr(0, 2) == "--" && 
                (arg == "--enable" || arg == "--disable" || arg == "--verbose" || 
                 arg == "--quiet" || arg == "--debug")) {
            std::string key = arg.substr(2);
            bool value = (arg == "--enable" || arg == "--verbose" || arg == "--debug");
            set(key, value, ConfigSource::CommandLine);
        }
    }
    
    return success;
}

bool EnhancedConfig::loadFromEnvironment(const std::string& prefix) {
    bool success = pImpl->entries.empty();
    
    for (const auto& envPair : std::getenv()) {
        std::string key = envPair.first;
        std::string value = envPair.second;
        
        if (key.rfind(prefix, 0) == 0) {
            std::string configKey = key.substr(prefix.length());
            // Convert environment variable name to config key (e.g., NLM_BRAIN_NEURON_COUNT -> brain.neuron_count)
            std::replace(configKey.begin(), configKey.end(), '_', '.');
            set(configKey, value, ConfigSource::Runtime);
        }
    }
    
    return success;
}

bool EnhancedConfig::saveToFile(const std::string& filepath, const std::string& format) const {
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (format == "json" || extension == ".json") {
        return saveToJSON(filepath);
    } else if (format == "yaml" || extension == ".yaml" || extension == ".yml") {
        return saveToYAML(filepath);
    } else {
        return saveToSimpleFile(filepath);
    }
}

bool EnhancedConfig::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    json j = toJSONInternal();
    file << j.dump(2);
    
    return true;
}

std::string EnhancedConfig::toJSON() const {
    return toJSONInternal().dump(2);
}

std::string EnhancedConfig::toYAML() const {
    // For now, fall back to JSON format
    return toJSONInternal().dump(2);
}

template<typename T>
T EnhancedConfig::get(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
std::optional<T> EnhancedConfig::getValidated(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (!val.has_value()) return std::optional<T>(defaultValue);
    
    // Validate the value
    if (!validateKey(key)) return val;
    
    // Additional validation based on key type
    if (key.find("neuron_count") != std::string::npos && std::is_integral_v<T>) {
        if (val.value() <= 0) return std::nullopt;
    } else if (key.find("simulation_timestep") != std::string::npos && std::is_floating_point_v<T>) {
        if (val.value() <= 0.0) return std::nullopt;
    }
    
    return val;
}

void EnhancedConfig::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!validateKey(key)) {
        NLM_LOG_WARNING("Attempt to set invalid configuration key: " + key);
        return;
    }
    
    // Check if value changed for monitoring
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it != pImpl->entries.end() && it->value != value) {
        pImpl->changedValues.emplace_back(key, value);
        pImpl->lastModified = std::time(nullptr);
    }
    
    Config::set(key, value, source);
}

void EnhancedConfig::set(const std::string& key, const std::string& value, ConfigSource source) {
    Config::set(key, value, source);
}

void EnhancedConfig::set(const std::string& key, int value, ConfigSource source) {
    Config::set(key, value, source);
}

void EnhancedConfig::set(const std::string& key, double value, ConfigSource source) {
    Config::set(key, value, source);
}

void EnhancedConfig::set(const std::string& key, bool value, ConfigSource source) {
    Config::set(key, value, source);
}

void EnhancedConfig::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    Config::set(key, ConfigValue(value), source);
}

void EnhancedConfig::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    Config::set(key, ConfigValue(value), source);
}

void EnhancedConfig::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    Config::set(key, ConfigValue(value), source);
}

bool EnhancedConfig::has(const std::string& key) const {
    return Config::has(key);
}

bool EnhancedConfig::validate(const std::string& key) const {
    if (!validateKey(key)) return false;
    
    auto val = Config::get<ConfigValue>(key);
    if (!val.has_value()) return true; // Empty value is valid
    
    // Validate based on key pattern
    if (key.find("neuron_count") != std::string::npos) {
        try {
            int count = std::get<int>(val.value());
            return count > 0 && count < 1000000; // Reasonable limits
        } catch (...) {
            return false;
        }
    } else if (key.find("simulation_timestep") != std::string::npos) {
        try {
            double timestep = std::get<double>(val.value());
            return timestep > 0.0 && timestep < 1.0; // Reasonable timestep range
        } catch (...) {
            return false;
        }
    } else if (key.find("connection_probability") != std::string::npos) {
        try {
            double prob = std::get<double>(val.value());
            return prob >= 0.0 && prob <= 1.0; // Probability must be between 0 and 1
        } catch (...) {
            return false;
        }
    }
    
    return true;
}

bool EnhancedConfig::validateConfig() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key)) {
            errors.push_back("Invalid configuration for key: " + entry.key);
        }
    }
    
    // Check for required parameters
    std::vector<std::string> requiredKeys = {
        "brain.neuron_count",
        "brain.region_count",
        "simulation_timestep"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    return errors.empty();
}

std::vector<std::string> EnhancedConfig::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key)) {
            errors.push_back("Invalid configuration for key: " + entry.key);
        }
    }
    
    // Check for required parameters
    std::vector<std::string> requiredKeys = {
        "brain.neuron_count",
        "brain.region_count",
        "simulation_timestep"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    return errors;
}

void EnhancedConfig::addProfile(const ConfigProfile& profile) {
    pImpl->profiles.push_back(profile);
}

bool EnhancedConfig::removeProfile(const std::string& name) {
    auto it = std::find_if(pImpl->profiles.begin(), pImpl->profiles.end(),
                          [&name](const ConfigProfile& p) { return p.name == name; });
    if (it != pImpl->profiles.end()) {
        pImpl->profiles.erase(it);
        return true;
    }
    return false;
}

bool EnhancedConfig::activateProfile(const std::string& name) {
    for (auto& profile : pImpl->profiles) {
        profile.isActive = (profile.name == name);
    }
    return true;
}

std::vector<std::string> EnhancedConfig::getProfileNames() const {
    std::vector<std::string> names;
    for (const auto& profile : pImpl->profiles) {
        names.push_back(profile.name);
    }
    return names;
}

bool EnhancedConfig::hasProfile(const std::string& name) const {
    return std::any_of(pImpl->profiles.begin(), pImpl->profiles.end(),
                      [&name](const ConfigProfile& p) { return p.name == name; });
}

void EnhancedConfig::merge(const EnhancedConfig& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
}

void EnhancedConfig::mergeFromJSON(const std::string& jsonString, ConfigSource source) {
    try {
        json j = json::parse(jsonString);
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            // Convert JSON value to ConfigValue
            if (it->is_number_integer()) {
                value = it->get<int>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else {
                value = it->get<std::string>();
            }
            
            set(key, value, source);
        }
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("Failed to merge JSON configuration: " + std::string(e.what()));
    }
}

void EnhancedConfig::setParent(const EnhancedConfig* parent) {
    pImpl->parent = const_cast<EnhancedConfig*>(parent);
}

EnhancedConfig* EnhancedConfig::getParent() const {
    return pImpl->parent;
}

EnhancedConfig EnhancedConfig::getInheritedConfig() const {
    EnhancedConfig inherited;
    
    // Get parent configuration if exists
    if (pImpl->parent) {
        inherited = *pImpl->parent;
    }
    
    // Merge current configuration (overwrites parent values)
    inherited.merge(*this, ConfigSource::Runtime);
    
    return inherited;
}

EnhancedConfig EnhancedConfig::getPreset(const std::string& presetName) {
    EnhancedConfig config;
    
    if (presetName == "fast") {
        config.set("brain.neuron_count", 100);
        config.set("brain.region_count", 1);
        config.set("simulation_timestep", 0.001);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.hebbian.enable", true);
    } else if (presetName == "detailed") {
        config.set("brain.neuron_count", 5000);
        config.set("brain.region_count", 4);
        config.set("simulation_timestep", 0.0001);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.hebbian.enable", true);
        config.set("plasticity.structural.enable", true);
    } else if (presetName == "balanced") {
        config.set("brain.neuron_count", 1000);
        config.set("brain.region_count", 2);
        config.set("simulation_timestep", 0.0005);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.hebbian.enable", true);
        config.set("memory.enable_working_memory", true);
        config.set("memory.enable_episodic_memory", true);
    }
    
    return config;
}

std::vector<std::string> EnhancedConfig::getPresetNames() {
    return {"fast", "detailed", "balanced"};
}

std::vector<std::string> EnhancedConfig::getKeys() const {
    return Config::getKeys();
}

std::vector<std::string> EnhancedConfig::getKeysOfType(const std::string& type) const {
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(type) != std::string::npos) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

json EnhancedConfig::getAll() const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = entry.value;
    }
    
    return j;
}

size_t EnhancedConfig::getEntryCount() const {
    return pImpl->entries.size();
}

ConfigSource EnhancedConfig::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    return ConfigSource::Default;
}

std::string EnhancedConfig::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    
    return "";
}

void EnhancedConfig::setDescription(const std::string& key, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

bool EnhancedConfig::equals(const EnhancedConfig& other) const {
    return pImpl->entries == other.pImpl->entries;
}

std::vector<std::string> EnhancedConfig::getDifferences(const EnhancedConfig& other) const {
    std::vector<std::string> differences;
    
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (entry.key == otherEntry.key && entry.value == otherEntry.value) {
                found = true;
                break;
            }
        }
        if (!found) {
            differences.push_back("Missing key in other: " + entry.key);
        }
    }
    
    return differences;
}

bool EnhancedConfig::backup(const std::string& filename) const {
    std::string backupFile = filename + ".backup";
    return saveToFile(backupFile);
}

bool EnhancedConfig::restore(const std::string& filename) {
    return loadFromFile(filename);
}

std::string EnhancedConfig::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string EnhancedConfig::getValidationSummary() const {
    std::ostringstream oss;
    oss << "Validation Summary:\n";
    
    std::vector<std::string> errors = getValidationErrors();
    if (errors.empty()) {
        oss << "  All configurations are valid.\n";
    } else {
        oss << "  Validation errors:\n";
        for (const auto& error : errors) {
            oss << "    - " << error << "\n";
        }
    }
    
    return oss.str();
}

void EnhancedConfig::logConfiguration() const {
    NLM_LOG_INFO("=== Enhanced Configuration ===");
    NLM_LOG_INFO(summary());
    NLM_LOG_INFO(getValidationSummary());
}

bool EnhancedConfig::importFromXML(const std::string& filepath) {
    // Placeholder for XML import
    NLM_LOG_WARNING("XML import not yet implemented");
    return false;
}

bool EnhancedConfig::exportToXML(const std::string& filepath) const {
    // Placeholder for XML export
    NLM_LOG_WARNING("XML export not yet implemented");
    return false;
}

void EnhancedConfig::enableMonitoring(bool enable) {
    pImpl->monitoringEnabled = enable;
    if (enable) {
        NLM_LOG_INFO("Configuration monitoring enabled");
    } else {
        NLM_LOG_INFO("Configuration monitoring disabled");
    }
}

bool EnhancedConfig::isMonitoringEnabled() const {
    return pImpl->monitoringEnabled;
}

std::vector<std::pair<std::string, ConfigValue>> EnhancedConfig::getChangedValues() const {
    return pImpl->changedValues;
}

// Implementation of helper methods
bool EnhancedConfig::validateKey(const std::string& key) const {
    if (key.empty()) return false;
    if (key.length() > 255) return false;
    if (key.find('.') == std::string::npos) return false; // Should contain at least one dot for namespace
    return true;
}

void EnhancedConfig::applyValidationRules() {
    // Apply custom validation rules
    for (const auto& validator : pImpl->customValidators) {
        // Apply validator to all entries
    }
}

void EnhancedConfig::mergeProfiles() {
    for (const auto& profile : pImpl->profiles) {
        if (profile.isActive) {
            // Merge profile parameters
            // Implementation would merge profile settings
        }
    }
}

void EnhancedConfig::resolveInheritance() {
    if (pImpl->parent) {
        // Resolve inheritance chain
        // Implementation would resolve parent configuration
    }
}

json EnhancedConfig::toJSONInternal() const {
    json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = entry.value;
    }
    
    return j;
}

void EnhancedConfig::fromJSONInternal(const json& j) {
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        ConfigValue value;
        
        if (it->is_number_integer()) {
            value = it->get<int>();
        } else if (it->is_number_float()) {
            value = it->get<double>();
        } else if (it->is_boolean()) {
            value = it->get<bool>();
        } else if (it->is_array()) {
            // Check if it's an array of numbers or strings
            if (!it->empty() && it->front().is_number()) {
                std::vector<double> doubleArray;
                for (const auto& item : *it) {
                    doubleArray.push_back(item.get<double>());
                }
                value = doubleArray;
            } else {
                std::vector<std::string> stringArray;
                for (const auto& item : *it) {
                    stringArray.push_back(item.get<std::string>());
                }
                value = stringArray;
            }
        } else {
            value = it->get<std::string>();
        }
        
        set(key, value, ConfigSource::File);
    }
}

bool EnhancedConfig::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
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

bool EnhancedConfig::saveToSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}

// Static helper methods for ConfigValidator
namespace ConfigValidator {
    bool validateInteger(const ConfigValue& value, const std::string& key) {
        try {
            std::get<int>(value);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool validateDouble(const ConfigValue& value, const std::string& key) {
        try {
            std::get<double>(value);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool validateBool(const ConfigValue& value, const std::string& key) {
        try {
            std::get<bool>(value);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool validateString(const ConfigValue& value, const std::string& key) {
        try {
            std::get<std::string>(value);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool validateRange(const ConfigValue& value, const std::string& key, double min, double max) {
        try {
            double val = std::get<double>(value);
            return val >= min && val <= max;
        } catch (...) {
            return false;
        }
    }
    
    bool validateLength(const ConfigValue& value, const std::string& key, size_t maxLength) {
        try {
            std::string val = std::get<std::string>(value);
            return val.length() <= maxLength;
        } catch (...) {
            return false;
        }
    }
    
    bool validatePattern(const ConfigValue& value, const std::string& key, const std::string& pattern) {
        try {
            std::string val = std::get<std::string>(value);
            // Simple regex check (in real implementation, would use proper regex)
            return true; // For now, accept any string
        } catch (...) {
            return false;
        }
    }
    
    bool validateBrainParameters(const json& config) {
        if (!config.contains("brain.neuron_count")) return false;
        if (!config.contains("brain.region_count")) return false;
        if (!config.contains("simulation_timestep")) return false;
        
        int neuronCount = config["brain.neuron_count"];
        int regionCount = config["brain.region_count"];
        double timestep = config["simulation_timestep"];
        
        return neuronCount > 0 && regionCount > 0 && timestep > 0.0;
    }
    
    bool validatePlasticityParameters(const json& config) {
        if (config.contains("plasticity.stdp.enable")) {
            bool stdpEnabled = config["plasticity.stdp.enable"];
            if (stdpEnabled && !config.contains("plasticity.stdp.learning_rate")) {
                return false;
            }
        }
        return true;
    }
    
    bool validateMemoryParameters(const json& config) {
        if (config.contains("memory.enable_working_memory")) {
            bool workingMemoryEnabled = config["memory.enable_working_memory"];
            if (workingMemoryEnabled && !config.contains("memory.working_memory_capacity")) {
                return false;
            }
        }
        return true;
    }
    
    bool validateNeuromodulationParameters(const json& config) {
        if (config.contains("neuromod.dopamine.scale")) {
            double dopamineScale = config["neuromod.dopamine.scale"];
            return dopamineScale >= 0.0;
        }
        return true;
    }
    
    bool validateDevelopmentParameters(const json& config) {
        if (config.contains("development.enable")) {
            bool developmentEnabled = config["development.enable"];
            if (developmentEnabled && !config.contains("development.plasticity_modifier")) {
                return false;
            }
        }
        return true;
    }
    
    json getBrainDefaults() {
        json j;
        j["brain.neuron_count"] = 1000;
        j["brain.region_count"] = 1;
        j["brain.connection_probability"] = 0.1;
        j["brain.initial_weight_mean"] = 0.5;
        j["brain.initial_weight_std"] = 0.1;
        j["brain.v_thresh"] = -50.0;
        j["brain.v_rest"] = -70.0;
        j["brain.v_reset"] = -75.0;
        return j;
    }
    
    json getPlasticityDefaults() {
        json j;
        j["plasticity.stdp.enable"] = true;
        j["plasticity.stdp.learning_rate"] = 0.01;
        j["plasticity.stdp.tau_plus"] = 20.0;
        j["plasticity.stdp.tau_minus"] = 20.0;
        j["plasticity.hebbian.enable"] = true;
        j["plasticity.structural.enable"] = true;
        j["plasticity.structural.synaptogenesis_rate"] = 0.0001;
        j["plasticity.structural.pruning_rate"] = 0.00001;
        return j;
    }
    
    json getMemoryDefaults() {
        json j;
        j["memory.enable_working_memory"] = true;
        j["memory.working_memory_capacity"] = 100;
        j["memory.working_memory_decay_rate"] = 0.01;
        j["memory.enable_episodic_memory"] = true;
        j["memory.episodic_memory_max_episodes"] = 1000;
        return j;
    }
    
    json getNeuromodulationDefaults() {
        json j;
        j["neuromod.dopamine.scale"] = 1.0;
        j["neuromod.curiosity.enable"] = true;
        j["neuromod.novelty.enable"] = true;
        j["neuromod.prediction_error.weight"] = 0.5;
        return j;
    }
    
    json getDevelopmentDefaults() {
        json j;
        j["development.enable"] = true;
        j["development.plasticity_modifier"] = 1.0;
        j["development.synaptogenesis_rate"] = 0.0001;
        j["development.pruning_rate"] = 0.00001;
        return j;
    }
}

} // namespace nlm