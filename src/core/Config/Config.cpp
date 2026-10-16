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

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeValidationRules();
}

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
    // Validate before setting if validation is enabled
    if (validationEnabled) {
        ConfigEntry tempEntry(key, value, source, "");
        if (!validateValue(tempEntry)) {
            throw std::runtime_error("Configuration validation failed for key: " + key);
        }
    }
    
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

// Configuration validation implementation
void Config::validate() const {
    clearValidationErrors();
    
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry)) {
            validationErrors.push_back(ConfigValidationError(
                entry.key, "Validation failed", entry.source));
        }
    }
}

void Config::addValidationRule(const ConfigValidationRule& rule) {
    validationRules.push_back(rule);
}

const std::vector<ConfigValidationRule>& Config::getValidationRules() const {
    return validationRules;
}

bool Config::validateValue(const ConfigEntry& entry) const {
    // First check if the value matches the expected type
    if (!validateType(entry.key, entry.value)) {
        return false;
    }
    
    // Then check if the value passes range/enum validation
    if (!validateRange(entry.key, entry.value)) {
        return false;
    }
    
    return true;
}

bool Config::validateType(const std::string& key, const ConfigValue& value) const {
    // Find validation rule for this key
    auto ruleIt = std::find_if(validationRules.begin(), validationRules.end(),
        [&key](const ConfigValidationRule& rule) { return rule.key == key; });
    
    if (ruleIt == validationRules.end()) {
        // No validation rule for this key, accept any type
        return true;
    }
    
    const auto& rule = *ruleIt;
    
    // Check if the value type matches the rule type
    bool typeMatches = false;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> && rule.type == "int") typeMatches = true;
        else if constexpr (std::is_same_v<T, int64_t> && rule.type == "int64_t") typeMatches = true;
        else if constexpr (std::is_same_v<T, double> && rule.type == "double") typeMatches = true;
        else if constexpr (std::is_same_v<T, bool> && rule.type == "bool") typeMatches = true;
        else if constexpr (std::is_same_v<T, std::string> && rule.type == "string") typeMatches = true;
        else if constexpr (std::is_same_v<T, std::vector<int>> && rule.type == "std::vector<int>") typeMatches = true;
        else if constexpr (std::is_same_v<T, std::vector<double>> && rule.type == "std::vector<double>") typeMatches = true;
        else if constexpr (std::is_same_v<T, std::vector<std::string>> && rule.type == "std::vector<std::string>") typeMatches = true;
    }, value);
    
    if (!typeMatches) {
        return false;
    }
    
    return true;
}

bool Config::validateRange(const std::string& key, const ConfigValue& value) const {
    // Find validation rule for this key
    auto ruleIt = std::find_if(validationRules.begin(), validationRules.end(),
        [&key](const ConfigValidationRule& rule) { return rule.key == key; });
    
    if (ruleIt == validationRules.end()) {
        // No validation rule for this key, accept any value
        return true;
    }
    
    const auto& rule = *ruleIt;
    
    // Parse validation string and apply appropriate checks
    if (rule.validation == "positive") {
        return std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_arithmetic_v<T>) {
                return arg > 0;
            }
            return true; // Non-numeric types pass
        }, value);
    }
    else if (rule.validation == "non-negative") {
        return std::visit([](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_arithmetic_v<T>) {
                return arg >= 0;
            }
            return true; // Non-numeric types pass
        }, value);
    }
    else if (rule.validation.find("range:") == 0) {
        // Parse "range:min-max" format
        size_t colonPos = rule.validation.find(':');
        size_t dashPos = rule.validation.find('-', colonPos + 1);
        if (colonPos != std::string::npos && dashPos != std::string::npos) {
            std::string minStr = rule.validation.substr(colonPos + 1, dashPos - colonPos - 1);
            std::string maxStr = rule.validation.substr(dashPos + 1);
            
            double minVal, maxVal;
            try {
                minVal = std::stod(minStr);
                maxVal = std::stod(maxStr);
            } catch (...) {
                return false; // Invalid range format
            }
            
            return std::visit([&](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_arithmetic_v<T>) {
                    return static_cast<double>(arg) >= minVal && 
                           static_cast<double>(arg) <= maxVal;
                }
                return true; // Non-numeric types pass
            }, value);
        }
    }
    else if (rule.validation.find("enum:") == 0) {
        // Parse "enum:value1,value2,value3" format
        size_t colonPos = rule.validation.find(':');
        if (colonPos != std::string::npos) {
            std::string enumStr = rule.validation.substr(colonPos + 1);
            
            std::vector<std::string> validValues;
            size_t pos = 0;
            while ((pos = enumStr.find(',')) != std::string::npos) {
                validValues.push_back(enumStr.substr(0, pos));
                enumStr.erase(0, pos + 1);
            }
            validValues.push_back(enumStr);
            
            return std::visit([&](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    return std::find(validValues.begin(), validValues.end(), arg) != validValues.end();
                }
                return true; // Non-string types pass
            }, value);
        }
    }
    
    // Unknown validation rule, assume it's valid
    return true;
}

void Config::initializeValidationRules() {
    // Neuron count validation
    addValidationRule(ConfigValidationRule(
        "neuron_count", "int", "positive", 
        "Number of neurons in the brain"));
    
    // Region count validation
    addValidationRule(ConfigValidationRule(
        "region_count", "int", "range:1-100",
        "Number of brain regions"));
    
    // Simulation timestep validation
    addValidationRule(ConfigValidationRule(
        "simulation_timestep", "double", "range:0.0001-1.0",
        "Simulation time step in seconds"));
    
    // Connection probability validation
    addValidationRule(ConfigValidationRule(
        "connection_probability", "double", "range:0.0-1.0",
        "Connection probability for random connectivity"));
    
    // Weight validation
    addValidationRule(ConfigValidationRule(
        "stdp_ltp_weight", "double", "range:0.0-1.0",
        "STDP long-term potentiation weight"));
    
    addValidationRule(ConfigValidationRule(
        "stdp_ltd_weight", "double", "range:0.0-1.0",
        "STDP long-term depression weight"));
    
    addValidationRule(ConfigValidationRule(
        "stdp_tau", "double", "range:1.0-1000.0",
        "STDP time constant"));
    
    // Plasticity rate validation
    addValidationRule(ConfigValidationRule(
        "synaptogenesis_rate", "double", "range:0.0-0.1",
        "Synaptogenesis rate"));
    
    addValidationRule(ConfigValidationRule(
        "pruning_rate", "double", "range:0.0-0.01",
        "Pruning rate"));
    
    // Dopamine validation
    addValidationRule(ConfigValidationRule(
        "dopamine_baseline", "double", "range:0.0-1.0",
        "Dopamine baseline level"));
    
    // Reward validation
    addValidationRule(ConfigValidationRule(
        "reward_discount_factor", "double", "range:0.0-1.0",
        "Reward discount factor"));
    
    // Environment validation
    addValidationRule(ConfigValidationRule(
        "environment_width", "int", "positive",
        "Environment width in cells"));
    
    addValidationRule(ConfigValidationRule(
        "environment_height", "int", "positive",
        "Environment height in cells"));
    
    // Log level validation
    addValidationRule(ConfigValidationRule(
        "log_level", "string", "enum:DEBUG,INFO,WARN,ERROR",
        "Log level"));
    
    // Simulation settings validation
    addValidationRule(ConfigValidationRule(
        "max_simulation_steps", "int", "positive",
        "Maximum number of simulation steps"));
    
    addValidationRule(ConfigValidationRule(
        "visualization_update_rate", "double", "positive",
        "Visualization update rate in Hz"));
    
    // Memory system validation
    addValidationRule(ConfigValidationRule(
        "working_memory_capacity", "int", "positive",
        "Working memory capacity"));
    
    addValidationRule(ConfigValidationRule(
        "episodic_memory_capacity", "int", "positive",
        "Episodic memory capacity"));
}

// Explicit template instantiations
// template std::optional<int> Config::get<int>(const std::string&) const;
// template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
// template std::optional<double> Config::get<double>(const std::string&) const;
// template std::optional<bool> Config::get<bool>(const std::string&) const;
// template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// template int Config::getOr<int>(const std::string&, const int&) const;
// template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
// template double Config::getOr<double>(const std::string&, const double&) const;
// template bool Config::getOr<bool>(const std::string&, const bool&) const;
// template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
