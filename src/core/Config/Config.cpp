#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <vector>
#include <sstream>

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

bool Config::validate() const {
    // Validate required configuration keys
    std::vector<std::string> requiredKeys = {
        "brain.neuron_count",
        "brain.synapse_density",
        "simulation.timestep"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            return false;
        }
        
        auto value = get<double>(key);
        if (!value || *value <= 0.0) {
            return false;
        }
    }
    
    // Validate ranges
    auto neuronCount = get<int64_t>("brain.neuron_count");
    if (neuronCount && (*neuronCount < 10 || *neuronCount > 1000000)) {
        return false;
    }
    
    auto synDensity = get<double>("brain.synapse_density");
    if (synDensity && (*synDensity < 0.0 || *synDensity > 1.0)) {
        return false;
    }
    
    return true;
}

std::string Config::getConfigurationRecommendations() const {
    std::stringstream report;
    report << "Configuration Optimization Recommendations:\n";
    report << "===========================================\n\n";
    
    // Recommendations based on current values
    auto neuronCount = get<int64_t>("brain.neuron_count");
    auto synDensity = get<double>("brain.synapse_density");
    
    if (neuronCount) {
        if (*neuronCount < 100) {
            report << "• Consider increasing brain.neuron_count for better realism (currently " << *neuronCount << ")\n";
        } else if (*neuronCount > 10000) {
            report << "• Consider reducing brain.neuron_count for better performance (currently " << *neuronCount << ")\n";
        }
    }
    
    if (synDensity) {
        if (*synDensity < 0.05) {
            report << "• Consider increasing brain.synapse_density for better connectivity (currently " << *synDensity << ")\n";
        } else if (*synDensity > 0.5) {
            report << "• Consider reducing brain.synapse_density for better performance (currently " << *synDensity << ")\n";
        }
    }
    
    // Check for advanced features
    if (!has("plasticity.stdp.enable") || !get<bool>("plasticity.stdp.enable")) {
        report << "• Enable STDP plasticity (plasticity.stdp.enable = true) for better learning\n";
    }
    
    if (!has("neuromod.curiosity.enable") || !get<bool>("neuromod.curiosity.enable")) {
        report << "• Enable curiosity neuromodulation (neuromod.curiosity.enable = true) for exploration\n";
    }
    
    if (!has("memory.working.enable") || !get<bool>("memory.working.enable")) {
        report << "• Enable working memory (memory.working.enable = true) for better cognition\n";
    }
    
    report << "\nValidation: " << (validate() ? "✓ Configuration is valid" : "✗ Configuration has issues") << "\n";
    
    return report.str();
}

std::vector<std::string> Config::getOptimizationSuggestions() const {
    std::vector<std::string> suggestions;
    
    auto neuronCount = get<int64_t>("brain.neuron_count");
    auto synDensity = get<double>("brain.synapse_density");
    
    if (neuronCount && *neuronCount < 500) {
        suggestions.push_back("Low neuron count may limit cognitive capabilities - consider increasing to 1000-5000 for better realism");
    }
    
    if (synDensity) {
        if (*synDensity < 0.01) {
            suggestions.push_back("Very low synapse density may result in poor connectivity - consider increasing to 0.05-0.2");
        } else if (*synDensity > 0.8) {
            suggestions.push_back("Very high synapse density may cause performance issues - consider reducing to 0.1-0.5");
        }
    }
    
    if (!has("plasticity.stdp.enable") || !get<bool>("plasticity.stdp.enable")) {
        suggestions.push_back("STDP plasticity is disabled - consider enabling for realistic learning dynamics");
    }
    
    if (!has("neuromod.curiosity.enable") || !get<bool>("neuromod.curiosity.enable")) {
        suggestions.push_back("Curiosity is disabled - consider enabling for exploration behavior");
    }
    
    return suggestions;
}

std::vector<std::string> Config::getValidationWarnings() const {
    std::vector<std::string> warnings;
    
    auto neuronCount = get<int64_t>("brain.neuron_count");
    auto synDensity = get<double>("brain.synapse_density");
    
    if (neuronCount && (*neuronCount < 10 || *neuronCount > 100000)) {
        warnings.push_back("brain.neuron_count is outside recommended range (10-100,000) for good performance");
    }
    
    if (synDensity) {
        if (*synDensity < 0.0 || *synDensity > 1.0) {
            warnings.push_back("brain.synapse_density is outside valid range (0.0-1.0)");
        } else if (*synDensity < 0.01) {
            warnings.push_back("Very low synapse density may result in sparse connectivity and limited cognitive capabilities");
        } else if (*synDensity > 0.5) {
            warnings.push_back("Very high synapse density may cause performance degradation and increased memory usage");
        }
    }
    
    // Check for missing advanced features
    std::vector<std::string> requiredAdvancedFeatures = {
        "plasticity.stdp.enable",
        "neuromod.curiosity.enable",
        "memory.working.enable",
        "prediction.enabled"
    };
    
    for (const auto& key : requiredAdvancedFeatures) {
        if (!has(key)) {
            warnings.push_back("Missing advanced feature: " + key + " - consider enabling for better cognitive capabilities");
        }
    }
    
    return warnings;
}

bool Config::generateConfigTemplate(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration Template\n";
    file << "# This file contains recommended configuration settings for NLM\n\n";
    
    // Basic configuration
    file << "# Basic Configuration\n";
    file << "brain.neuron_count = 1000\n";
    file << "brain.synapse_density = 0.1\n";
    file << "simulation.timestep = 0.01\n\n";
    
    // Neural dynamics
    file << "# Neural Dynamics\n";
    file << "brain.v_thresh = -50.0\n";
    file << "brain.v_rest = -70.0\n";
    file << "brain.v_reset = -75.0\n";
    file << "brain.tau_mem = 20.0\n";
    file << "brain.tau_ref = 2.0\n\n";
    
    // Plasticity
    file << "# Plasticity\n";
    file << "plasticity.stdp.enable = true\n";
    file << "plasticity.stdp.learning_rate = 0.001\n";
    file << "plasticity.stdp.tau_plus = 20.0\n";
    file << "plasticity.stdp.tau_minus = 20.0\n";
    file << "plasticity.hebbian.enable = true\n";
    file << "plasticity.hebbian.learning_rate = 0.001\n\n";
    
    // Neuromodulation
    file << "# Neuromodulation\n";
    file << "neuromod.dopamine.scale = 1.0\n";
    file << "neuromod.curiosity.enable = true\n";
    file << "neuromod.novelty.enable = true\n\n";
    
    // Memory
    file << "# Memory\n";
    file << "memory.working.capacity = 100\n";
    file << "memory.working.decay_rate = 0.01\n";
    file << "memory.episodic.capacity = 500\n";
    file << "memory.associative.capacity = 200\n\n";
    
    // Prediction
    file << "# Prediction\n";
    file << "prediction.enabled = true\n";
    file << "prediction.horizon = 5\n";
    file << "prediction.confidence_threshold = 0.5\n\n";
    
    // Development
    file << "# Development\n";
    file << "development.enabled = true\n";
    file << "development.initial_plasticity = 1.0\n";
    file << "development.final_plasticity = 0.2\n\n";
    
    file << "# End of template\n";
    
    return true;
}

bool Config::validateConfigFile(const std::string& filepath) const {
    Config tempConfig;
    if (!tempConfig.loadFromFile(filepath)) {
        return false;
    }
    
    return tempConfig.validate();
}

bool Config::savePreset(const std::string& name, const std::map<std::string, ConfigValue>& values) {
    // In a real implementation, this would persist presets to a file
    // For now, we'll just store them in memory
    
    // Check if preset already exists
    for (auto& entry : pImpl->entries) {
        if (entry.key == "preset.name" && entry.source == ConfigSource::Runtime) {
            if (std::get<std::string>(entry.value) == name) {
                return true; // Preset already exists
            }
        }
    }
    
    // Save current config as preset
    for (const auto& pair : values) {
        set(pair.first, pair.second, ConfigSource::Runtime);
    }
    
    // Store preset name
    set("preset.name", name, ConfigSource::Runtime);
    
    return true;
}

std::map<std::string, ConfigValue> Config::loadPreset(const std::string& name) const {
    std::map<std::string, ConfigValue> preset;
    
    // In a real implementation, this would load presets from a file
    // For now, we'll return an empty map
    
    return preset;
}

std::vector<std::string> Config::getAvailablePresets() const {
    std::vector<std::string> presets;
    
    // In a real implementation, this would read preset names from a file
    // For now, return empty list
    
    return presets;
}

bool Config::compareWith(const Config& otherConfig) const {
    // Compare entries between this config and another config
    std::vector<std::string> thisKeys = getKeys();
    std::vector<std::string> otherKeys = otherConfig.getKeys();
    
    // Check for differences in keys
    for (const auto& key : thisKeys) {
        if (std::find(otherKeys.begin(), otherKeys.end(), key) == otherKeys.end()) {
            return false; // Key present in this but not in other
        }
        
        auto thisVal = get<std::string>(key);
        auto otherVal = otherConfig.get<std::string>(key);
        
        if (thisVal != otherVal) {
            return false; // Values differ
        }
    }
    
    return true;
}

std::string Config::generateDiffReport(const Config& otherConfig) const {
    std::stringstream report;
    report << "Configuration Comparison Report\n";
    report << "==============================\n\n";
    
    std::vector<std::string> thisKeys = getKeys();
    std::vector<std::string> otherKeys = otherConfig.getKeys();
    
    // Find common keys
    std::vector<std::string> commonKeys;
    for (const auto& key : thisKeys) {
        if (std::find(otherKeys.begin(), otherKeys.end(), key) != otherKeys.end()) {
            commonKeys.push_back(key);
        }
    }
    
    bool hasDifferences = false;
    
    // Compare common keys
    for (const auto& key : commonKeys) {
        auto thisVal = get<std::string>(key);
        auto otherVal = otherConfig.get<std::string>(key);
        
        if (thisVal != otherVal) {
            report << "DIFFERENCE: " << key << "\n";
            report << "  This config:    " << (thisVal ? *thisVal : "(null)") << "\n";
            report << "  Other config:   " << (otherVal ? *otherVal : "(null)") << "\n\n";
            hasDifferences = true;
        }
    }
    
    // Find keys only in this config
    for (const auto& key : thisKeys) {
        if (std::find(otherKeys.begin(), otherKeys.end(), key) == otherKeys.end()) {
            report << "ONLY IN THIS CONFIG: " << key << " = " << 
                (get<std::string>(key).value_or("(null)")) << "\n\n";
            hasDifferences = true;
        }
    }
    
    // Find keys only in other config
    for (const auto& key : otherKeys) {
        if (std::find(thisKeys.begin(), thisKeys.end(), key) == thisKeys.end()) {
            report << "ONLY IN OTHER CONFIG: " << key << " = " << 
                (otherConfig.get<std::string>(key).value_or("(null)")) << "\n\n";
            hasDifferences = true;
        }
    }
    
    if (!hasDifferences) {
        report << "No differences found between configurations.\n";
    } else {
        report << "Configuration comparison completed.\n";
    }
    
    return report.str();
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

} // namespace nlm