#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cassert>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigValidationRule> validationRules;
    std::vector<ConfigCrossValidationRule> crossValidationRules;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Add default validation rules for Phase 2
    addValidationRule(ConfigValidationRule("random_seed", 
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
        "random_seed must be a positive integer"));
    
    addValidationRule(ConfigValidationRule("simulation_timestep", 
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0; },
        "simulation_timestep must be positive"));
    
    addValidationRule(ConfigValidationRule("neuron_count", 
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
        "neuron_count must be positive"));
    
    addValidationRule(ConfigValidationRule("region_count", 
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
        "region_count must be positive"));
    
    addValidationRule(ConfigValidationRule("connection_probability", 
        isBetweenZeroOne, "connection_probability must be between 0.0 and 1.0"));
    
    addValidationRule(ConfigValidationRule("stdp_ltp_weight", 
        isFinite, "stdp_ltp_weight must be finite"));
    
    addValidationRule(ConfigValidationRule("stdp_ltd_weight", 
        isFinite, "stdp_ltd_weight must be finite"));
    
    addValidationRule(ConfigValidationRule("stdp_tau", 
        isPositive, "stdp_tau must be positive"));
    
    addValidationRule(ConfigValidationRule("synaptogenesis_rate", 
        isBetweenZeroOne, "synaptogenesis_rate must be between 0.0 and 1.0"));
    
    addValidationRule(ConfigValidationRule("pruning_rate", 
        isBetweenZeroOne, "pruning_rate must be between 0.0 and 1.0"));
    
    // Add cross-validation rules
    addCrossValidationRule(ConfigCrossValidationRule({"simulation_timestep", "neuron_count", "region_count"},
        validateConsistentTimestep, "Inconsistent timestep or neuron/region counts"));
    
    addCrossValidationRule(ConfigCrossValidationRule({"stdp_ltp_weight", "stdp_ltd_weight"},
        validateConsistentPlasticity, "STDP LTP/LTD weights should have reasonable ratio"));
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

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate before setting if validation rules exist for this key
    if (!validationRules.empty()) {
        // If validation fails, we could either:
        // 1. Reject the set operation (current behavior)
        // 2. Log a warning and allow it (more flexible)
        // 3. Apply a corrected value
        // For now, we'll be strict and reject invalid values
        auto validationResult = validateValue(key, value);
        if (!validationResult.isValid()) {
            return;  // Reject invalid value
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
    
    // Cross-validate all affected keys
    if (!crossValidationRules.empty()) {
        validateCrossDependencies(key);
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

// Configuration validation implementation
ConfigValidationResult Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigValidationResult(ConfigValidationError::None, "Key not found", key);
    }
    
    return validateValue(key, it->value);
}

std::vector<ConfigValidationResult> Config::validateAll() const {
    std::vector<ConfigValidationResult> results;
    
    for (const auto& entry : pImpl->entries) {
        results.push_back(validateValue(entry.key, entry.value));
    }
    
    return results;
}

bool Config::isValid(const std::string& key) const {
    return validate(key).isValid();
}

bool Config::isValidAll() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value).isValid()) {
            return false;
        }
    }
    return true;
}

void Config::addValidationRule(const ConfigValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
}

void Config::addCrossValidationRule(const ConfigCrossValidationRule& rule) {
    pImpl->crossValidationRules.push_back(rule);
}

void Config::removeValidationRule(const std::string& key) {
    pImpl->validationRules.erase(
        std::remove_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
            [&key](const ConfigValidationRule& rule) { return rule.key == key; }),
        pImpl->validationRules.end()
    );
}

void Config::clearValidationRules() {
    pImpl->validationRules.clear();
    pImpl->crossValidationRules.clear();
}

std::vector<ConfigValidationResult> Config::getValidationErrors(const std::string& key) const {
    std::vector<ConfigValidationResult> errors;
    
    for (const auto& rule : pImpl->validationRules) {
        if (rule.key == key && !rule.validator(pImpl->entries[key])) {
            errors.emplace_back(ConfigValidationError::InvalidType, rule.errorMessage, key);
        }
    }
    
    return errors;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Check type-specific validation rules
    for (const auto& rule : pImpl->validationRules) {
        if (rule.key == key) {
            if (!rule.validator(value)) {
                return false;
            }
        }
    }
    
    return validateCrossDependencies(key);
}

bool Config::validateCrossDependencies(const std::string& key) const {
    // Find all cross-validation rules that involve this key
    for (const auto& rule : pImpl->crossValidationRules) {
        bool keyInvolved = false;
        for (const auto& depKey : rule.dependentKeys) {
            if (depKey == key) {
                keyInvolved = true;
                break;
            }
        }
        
        if (keyInvolved) {
            // Collect all dependent values
            std::vector<const ConfigValue*> dependentValues;
            for (const auto& depKey : rule.dependentKeys) {
                auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&depKey](const ConfigEntry& e) { return e.key == depKey; });
                if (it != pImpl->entries.end()) {
                    dependentValues.push_back(&it->value);
                } else {
                    // If any dependent key is missing, validation fails
                    return false;
                }
            }
            
            if (!rule.validator(dependentValues)) {
                return false;
            }
        }
    }
    
    return true;
}

bool Config::validateRange(const ConfigValue& value, double min, double max, const std::string& key) {
    try {
        if (std::holds_alternative<double>(value)) {
            double v = std::get<double>(value);
            return v >= min && v <= max;
        } else if (std::holds_alternative<float>(value)) {
            float v = std::get<float>(value);
            return v >= static_cast<float>(min) && v <= static_cast<float>(max);
        } else if (std::holds_alternative<int>(value)) {
            int v = std::get<int>(value);
            return v >= static_cast<int>(min) && v <= static_cast<int>(max);
        } else if (std::holds_alternative<int64_t>(value)) {
            int64_t v = std::get<int64_t>(value);
            return v >= static_cast<int64_t>(min) && v <= static_cast<int64_t>(max);
        }
    } catch (const std::bad_variant_access&) {}
    return false;
}

bool Config::validateIntRange(const ConfigValue& value, int64_t min, int64_t max, const std::string& key) {
    try {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t v = std::get<int64_t>(value);
            return v >= min && v <= max;
        } else if (std::holds_alternative<int>(value)) {
            int v = std::get<int>(value);
            return v >= static_cast<int>(min) && v <= static_cast<int>(max);
        }
    } catch (const std::bad_variant_access&) {}
    return false;
}

bool Config::validatePositive(const ConfigValue& value, const std::string& key) {
    return validateIntRange(value, 1, std::numeric_limits<int64_t>::max(), key);
}

bool Config::validateNonNegative(const ConfigValue& value, const std::string& key) {
    return validateIntRange(value, 0, std::numeric_limits<int64_t>::max(), key);
}

bool Config::validateBetweenZeroOne(const ConfigValue& value, const std::string& key) {
    return validateRange(value, 0.0, 1.0, key);
}

bool Config::validateConsistentTimestep(const std::vector<const ConfigValue*>& values) {
    if (values.size() < 3) return true;
    
    // Extract timestep, neuron_count, region_count
    double timestep = -1.0;
    int64_t neuronCount = -1;
    int64_t regionCount = -1;
    
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i]) {
            if (i == 0 && std::holds_alternative<double>(*values[i])) {
                timestep = std::get<double>(*values[i]);
            } else if (i == 1 && std::holds_alternative<int64_t>(*values[i])) {
                neuronCount = std::get<int64_t>(*values[i]);
            } else if (i == 2 && std::holds_alternative<int64_t>(*values[i])) {
                regionCount = std::get<int64_t>(*values[i]);
            }
        }
    }
    
    if (timestep <= 0.0 || neuronCount <= 0 || regionCount <= 0) {
        return false;
    }
    
    // Check that neuron count is reasonable for region count
    if (neuronCount < regionCount) {
        return false;
    }
    
    return true;
}

bool Config::validateConsistentNeuronCounts(const std::vector<const ConfigValue*>& values) {
    if (values.size() < 2) return true;
    
    // Check that LTP and LTD weights have reasonable ratio
    if (values[0] && values[1]) {
        if (std::holds_alternative<double>(*values[0]) && std::holds_alternative<double>(*values[1])) {
            double ltp = std::get<double>(*values[0]);
            double ltd = std::get<double>(*values[1]);
            
            if (ltp <= 0.0 || ltd <= 0.0) {
                return false;
            }
            
            // Ratio should be reasonable (LTP typically slightly stronger than LTD)
            double ratio = ltp / ltd;
            return ratio >= 0.1 && ratio <= 10.0;
        }
    }
    
    return true;
}

bool Config::validateConsistentPlasticity(const std::vector<const ConfigValue*>& values) {
    return validateConsistentNeuronCounts(values);
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

// Helper function implementations for ConfigSchema

const std::vector<ConfigValidationRule> ConfigSchema::getPhase2ValidationRules() {
    std::vector<ConfigValidationRule> rules;
    
    rules.push_back(ConfigValidationRule("random_seed", 
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
        "random_seed must be a positive integer"));
    
    rules.push_back(ConfigValidationRule("simulation_timestep", 
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0; },
        "simulation_timestep must be positive"));
    
    rules.push_back(ConfigValidationRule("neuron_count", 
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
        "neuron_count must be positive"));
    
    rules.push_back(ConfigValidationRule("region_count", 
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
        "region_count must be positive"));
    
    rules.push_back(ConfigValidationRule("connection_probability", 
        isBetweenZeroOne, "connection_probability must be between 0.0 and 1.0"));
    
    rules.push_back(ConfigValidationRule("stdp_ltp_weight", 
        isFinite, "stdp_ltp_weight must be finite"));
    
    rules.push_back(ConfigValidationRule("stdp_ltd_weight", 
        isFinite, "stdp_ltd_weight must be finite"));
    
    rules.push_back(ConfigValidationRule("stdp_tau", 
        isPositive, "stdp_tau must be positive"));
    
    rules.push_back(ConfigValidationRule("synaptogenesis_rate", 
        isBetweenZeroOne, "synaptogenesis_rate must be between 0.0 and 1.0"));
    
    rules.push_back(ConfigValidationRule("pruning_rate", 
        isBetweenZeroOne, "pruning_rate must be between 0.0 and 1.0"));
    
    return rules;
}

const std::vector<ConfigCrossValidationRule> ConfigSchema::getPhase2CrossValidationRules() {
    std::vector<ConfigCrossValidationRule> rules;
    
    rules.push_back(ConfigCrossValidationRule({"simulation_timestep", "neuron_count", "region_count"},
        validateConsistentTimestep, "Inconsistent timestep or neuron/region counts"));
    
    rules.push_back(ConfigCrossValidationRule({"stdp_ltp_weight", "stdp_ltd_weight"},
        validateConsistentPlasticity, "STDP LTP/LTD weights should have reasonable ratio"));
    
    return rules;
}

ConfigValidationResult ConfigSchema::validateRandomSeed(const ConfigValue& value, const std::string& key) {
    if (!std::holds_alternative<int64_t>(value) || std::get<int64_t>(value) <= 0) {
        return ConfigValidationResult(ConfigValidationError::OutOfRange, 
            "random_seed must be a positive integer", key);
    }
    return ConfigValidationResult();
}

ConfigValidationResult ConfigSchema::validateTimestep(const ConfigValue& value, const std::string& key) {
    if (!std::holds_alternative<double>(value) || std::get<double>(value) <= 0.0) {
        return ConfigValidationResult(ConfigValidationError::OutOfRange,
            "simulation_timestep must be positive", key);
    }
    return ConfigValidationResult();
}

ConfigValidationResult ConfigSchema::validateNeuronCount(const ConfigValue& value, const std::string& key) {
    if (!std::holds_alternative<int64_t>(value) || std::get<int64_t>(value) <= 0) {
        return ConfigValidationResult(ConfigValidationError::OutOfRange,
            "neuron_count must be positive", key);
    }
    return ConfigValidationResult();
}

ConfigValidationResult ConfigSchema::validateRegionCount(const ConfigValue& value, const std::string& key) {
    if (!std::holds_alternative<int64_t>(value) || std::get<int64_t>(value) <= 0) {
        return ConfigValidationResult(ConfigValidationError::OutOfRange,
            "region_count must be positive", key);
    }
    return ConfigValidationResult();
}

ConfigValidationResult ConfigSchema::validateConnectionProbability(const ConfigValue& value, const std::string& key) {
    double v = 0.0;
    if (std::holds_alternative<double>(value)) {
        v = std::get<double>(value);
    } else if (std::holds_alternative<float>(value)) {
        v = static_cast<double>(std::get<float>(value));
    } else if (std::holds_alternative<int>(value)) {
        v = static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        v = static_cast<double>(std::get<int64_t>(value));
    }
    
    if (v < 0.0 || v > 1.0) {
        return ConfigValidationResult(ConfigValidationError::OutOfRange,
            "connection_probability must be between 0.0 and 1.0", key);
    }
    return ConfigValidationResult();
}

ConfigValidationResult ConfigSchema::validateSTDPParameters(const ConfigValue& value, const std::string& key) {
    if (!isFinite(value)) {
        return ConfigValidationResult(ConfigValidationError::OutOfRange,
            "STDP parameters must be finite", key);
    }
    return ConfigValidationResult();
}

ConfigValidationResult ConfigSchema::validatePlasticityParameters(const ConfigValue& value, const std::string& key) {
    if (!isBetweenZeroOne(value, key)) {
        return ConfigValidationResult(ConfigValidationError::OutOfRange,
            "plasticity parameters must be between 0.0 and 1.0", key);
    }
    return ConfigValidationResult();
}

