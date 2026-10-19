#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, Config> presets;
    std::unordered_map<std::string, bool> enabledFeatures;
    std::unordered_map<std::string, std::string> validationRules;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaults() {
    // Set up default NLM configuration
    set("neuron_count", size_t(500), ConfigSource::Default, "range:100-5000");
    set("region_count", size_t(2), ConfigSource::Default, "range:1-10");
    set("connection_probability", 0.1f, ConfigSource::Default, "range:0.01-0.5");
    set("random_seed", uint64_t(42), ConfigSource::Default, "range:1-2^63-1");
    set("simulation_timestep", 0.001f, ConfigSource::Default, "range:0.0001-0.01");
    
    // Set up default validation rules
    validationRules["neuron_count"] = "positive,<=5000";
    validationRules["region_count"] = "positive,<=10";
    validationRules["connection_probability"] = "range:0.01-0.5";
    validationRules["random_seed"] = "positive";
    validationRules["simulation_timestep"] = "range:0.0001-0.01";
    
    // Enable default features
    enabledFeatures["memory_pools"] = false;
    enabledFeatures["simd_vectorization"] = false;
    enabledFeatures["parallel_processing"] = false;
    enabledFeatures["checkpointing"] = false;
    enabledFeatures["replay"] = false;
    enabledFeatures["development"] = false;
    enabledFeatures["prediction"] = false;
    enabledFeatures["memory_integration"] = false;
    enabledFeatures["cognition_integration"] = false;
    enabledFeatures["performance_monitoring"] = false;
    enabledFeatures["advanced_logging"] = false;
}

bool Config::loadFromFile(const std::string& filepath) {
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
        
        // Parse key=value pairs with validation
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
            
            // Try to parse value based on key type
            bool parsed = false;
            if (key == "neuron_count" || key == "region_count") {
                try {
                    size_t val = std::stoull(value);
                    set(key, val, ConfigSource::File, validationRules[key]);
                    parsed = true;
                } catch (...) {}
            } else if (key == "connection_probability" || key == "simulation_timestep") {
                try {
                    double val = std::stod(value);
                    set(key, val, ConfigSource::File, validationRules[key]);
                    parsed = true;
                } catch (...) {}
            } else if (key == "random_seed") {
                try {
                    uint64_t val = std::stoull(value);
                    set(key, val, ConfigSource::File, validationRules[key]);
                    parsed = true;
                } catch (...) {}
            } else if (key == "enable_memory_pools" || key == "enable_simd_vectorization" ||
                       key == "enable_parallel_processing" || key == "enable_checkpointing" ||
                       key == "enable_replay" || key == "enable_development" ||
                       key == "enable_prediction" || key == "enable_memory_integration" ||
                       key == "enable_cognition_integration" || key == "enable_performance_monitoring" ||
                       key == "enable_advanced_logging") {
                bool val = (value == "true" || value == "1" || value == "yes");
                set(key, val, ConfigSource::File);
                parsed = true;
            } else {
                // Generic string value
                set(key, value, ConfigSource::File);
                parsed = true;
            }
            
            if (!parsed) {
                std::cerr << "Warning: Could not parse value for key '" << key << "' in " << filepath << std::endl;
            }
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format with validation
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine, validationRules[key]);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine, validationRules[key]);
        }
    }
    return true;
}

bool Config::loadFromString(const std::string& configString) {
    std::istringstream stream(configString);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::Runtime);
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
        
        // Convert value to string
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "\n";
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

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& validation, std::function<bool(const ConfigValue&)> validator) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigEntry entry;
    if (it != pImpl->entries.end()) {
        entry = *it;
        entry.value = value;
        entry.source = source;
        entry.validation = validation;
        entry.validator = validator;
    } else {
        entry = ConfigEntry(key, value, source, "", validation, validator);
    }
    
    // Validate if validator is provided
    if (entry.validator && !entry.validator(value)) {
        std::cerr << "Error: Validation failed for key '" << key << "' with value " << value << std::endl;
        return;
    }
    
    if (!it) {
        pImpl->entries.push_back(entry);
    }
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

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    initializeDefaults();
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.validation.empty()) {
            oss << " [" << entry.validation << "]";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Config::validationReport() const {
    std::ostringstream oss;
    oss << "=== Configuration Validation Report ===\n";
    
    ValidationResult result = validate();
    if (result.valid) {
        oss << "Configuration is valid.\n";
    } else {
        oss << "Configuration validation failed:\n";
        for (const auto& error : result.errors) {
            oss << "  ERROR: " << error << "\n";
        }
    }
    
    if (!result.warnings.empty()) {
        oss << "\nWarnings:\n";
        for (const auto& warning : result.warnings) {
            oss << "  WARNING: " << warning << "\n";
        }
    }
    
    return oss.str();
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it == other.pImpl->entries.end() || it->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult keyResult = validateKey(entry.key);
        if (!keyResult.valid) {
            result.errors.insert(result.errors.end(), keyResult.errors.begin(), keyResult.errors.end());
            result.warnings.insert(result.warnings.end(), keyResult.warnings.begin(), keyResult.warnings.end());
        }
    }
    
    return result;
}

ValidationResult Config::validateKey(const std::string& key) const {
    ValidationResult result;
    
    // Find the entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        result.addError("Key not found in configuration");
        return result;
    }
    
    // Check basic type validation
    const auto& entry = *it;
    const std::string& validation = entry.validation;
    
    if (validation.empty()) {
        return result; // No validation rules
    }
    
    // Parse validation rules
    if (validation.find("positive") != std::string::npos) {
        if (!validateIntRange(entry.value, validation) && !validateDoubleRange(entry.value, validation)) {
            result.addError("Value must be positive for key: " + key);
        }
    }
    
    if (validation.find("<=5000") != std::string::npos) {
        if (!validateIntRange(entry.value, validation) && !validateDoubleRange(entry.value, validation)) {
            result.addError("Value must be <= 5000 for key: " + key);
        }
    }
    
    if (validation.find("range:") != std::string::npos) {
        // Parse range specification
        size_t rangePos = validation.find("range:");
        if (rangePos != std::string::npos) {
            std::string rangeSpec = validation.substr(rangePos + 6);
            // Simple range validation - could be enhanced
            if (!validateIntRange(entry.value, "range:" + rangeSpec) && 
                !validateDoubleRange(entry.value, "range:" + rangeSpec)) {
                result.addWarning("Range validation not fully implemented for key: " + key);
            }
        }
    }
    
    return result;
}

void Config::enableFeature(const std::string& feature, bool enable) {
    enabledFeatures[feature] = enable;
}

bool Config::isFeatureEnabled(const std::string& feature) const {
    auto it = enabledFeatures.find(feature);
    return it != enabledFeatures.end() && it->second;
}

bool Config::applyPreset(NLMConfigPreset preset) {
    // Clear existing configuration
    clear();
    
    // Apply preset
    switch (preset) {
        case NLMConfigPreset::Minimal:
            set("neuron_count", size_t(100), ConfigSource::Default, "range:100-5000");
            set("region_count", size_t(1), ConfigSource::Default, "range:1-10");
            break;
            
        case NLMConfigPreset::Standard:
            set("neuron_count", size_t(500), ConfigSource::Default, "range:100-5000");
            set("region_count", size_t(2), ConfigSource::Default, "range:1-10");
            set("connection_probability", 0.1f, ConfigSource::Default, "range:0.01-0.5");
            break;
            
        case NLMConfigPreset::Optimized:
            set("neuron_count", size_t(1000), ConfigSource::Default, "range:100-5000");
            set("region_count", size_t(3), ConfigSource::Default, "range:1-10");
            set("connection_probability", 0.15f, ConfigSource::Default, "range:0.01-0.5");
            set("enable_memory_pools", true, ConfigSource::Default);
            set("enable_simd_vectorization", true, ConfigSource::Default);
            set("enable_parallel_processing", true, ConfigSource::Default);
            break;
            
        case NLMConfigPreset::Advanced:
            set("neuron_count", size_t(2000), ConfigSource::Default, "range:100-5000");
            set("region_count", size_t(5), ConfigSource::Default, "range:1-10");
            set("connection_probability", 0.2f, ConfigSource::Default, "range:0.01-0.5");
            set("enable_memory_pools", true, ConfigSource::Default);
            set("enable_simd_vectorization", true, ConfigSource::Default);
            set("enable_parallel_processing", true, ConfigSource::Default);
            set("enable_checkpointing", true, ConfigSource::Default);
            set("enable_replay", true, ConfigSource::Default);
            set("enable_development", true, ConfigSource::Default);
            set("enable_prediction", true, ConfigSource::Default);
            set("enable_memory_integration", true, ConfigSource::Default);
            set("enable_cognition_integration", true, ConfigSource::Default);
            set("enable_performance_monitoring", true, ConfigSource::Default);
            set("enable_advanced_logging", true, ConfigSource::Default);
            break;
            
        case NLMConfigPreset::Custom:
            // Keep current configuration
            break;
    }
    
    return true;
}

void Config::addPreset(NLMConfigPreset preset, const Config& config) {
    std::string presetName;
    switch (preset) {
        case NLMConfigPreset::Minimal: presetName = "minimal"; break;
        case NLMConfigPreset::Standard: presetName = "standard"; break;
        case NLMConfigPreset::Optimized: presetName = "optimized"; break;
        case NLMConfigPreset::Advanced: presetName = "advanced"; break;
        case NLMConfigPreset::Custom: presetName = "custom"; break;
    }
    
    pImpl->presets[presetName] = config;
}

Config Config::getPreset(NLMConfigPreset preset) const {
    std::string presetName;
    switch (preset) {
        case NLMConfigPreset::Minimal: presetName = "minimal"; break;
        case NLMConfigPreset::Standard: presetName = "standard"; break;
        case NLMConfigPreset::Optimized: presetName = "optimized"; break;
        case NLMConfigPreset::Advanced: presetName = "advanced"; break;
        case NLMConfigPreset::Custom: presetName = "custom"; break;
    }
    
    auto it = pImpl->presets.find(presetName);
    if (it != pImpl->presets.end()) {
        return it->second;
    }
    
    // Return current config if preset not found
    return *this;
}

// Helper function implementations
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

bool Config::validateIntRange(const ConfigValue& value, const std::string& validation) {
    try {
        int val = std::get<int>(value);
        // Parse validation rules (simplified)
        if (validation.find("positive") != std::string::npos && val <= 0) {
            return false;
        }
        return true;
    } catch (...) { return false; }
}

bool Config::validateDoubleRange(const ConfigValue& value, const std::string& validation) {
    try {
        double val = std::get<double>(value);
        // Parse validation rules (simplified)
        if (validation.find("positive") != std::string::npos && val <= 0.0) {
            return false;
        }
        return true;
    } catch (...) { return false; }
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
