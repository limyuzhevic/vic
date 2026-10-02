#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationRule> validationRules;
    std::unordered_map<std::string, ValidationResult> validationResults;
    std::unordered_map<std::string, std::string> keyDescriptions;
    std::set<std::string> requiredKeys;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with basic validation rules for common keys
    initializeValidationRules();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeValidationRules() {
    // Add validation rules for common configuration keys
    addValidationRule("neuron_count", [this](const ConfigValue& value) {
        return validateIntRange("neuron_count", std::get<int>(value), 1, 10000);
    }, "Neuron count must be between 1 and 10000", "Neuron count should be realistic for simulation performance");
    
    addValidationRule("region_count", [this](const ConfigValue& value) {
        return validateIntRange("region_count", std::get<int>(value), 1, 10);
    }, "Region count must be between 1 and 10", "Too many regions may impact performance");
    
    addValidationRule("connection_probability", [this](const ConfigValue& value) {
        return validateDoubleRange("connection_probability", std::get<double>(value), 0.0, 1.0);
    }, "Connection probability must be between 0.0 and 1.0", "Value should be reasonable for connectivity");
    
    addValidationRule("random_seed", [this](const ConfigValue& value) {
        return true; // Any integer seed is valid
    }, "Invalid random seed", "Use a meaningful seed for reproducible experiments");
    
    addValidationRule("simulation_timestep", [this](const ConfigValue& value) {
        return validateDoubleRange("simulation_timestep", std::get<double>(value), 0.0001, 0.1);
    }, "Simulation timestep must be between 0.0001s and 0.1s", "Too large timestep may affect simulation accuracy");
    
    addValidationRule("stdp_ltp_weight", [this](const ConfigValue& value) {
        return validateDoubleRange("stdp_ltp_weight", std::get<double>(value), 0.0, 1.0);
    }, "STDP LTP weight must be between 0.0 and 1.0", "Weight should be reasonable for synaptic plasticity");
    
    addValidationRule("stdp_ltd_weight", [this](const ConfigValue& value) {
        return validateDoubleRange("stdp_ltd_weight", std::get<double>(value), 0.0, 1.0);
    }, "STDP LTD weight must be between 0.0 and 1.0", "Weight should be reasonable for synaptic plasticity");
    
    addValidationRule("synaptogenesis_rate", [this](const ConfigValue& value) {
        return validateDoubleRange("synaptogenesis_rate", std::get<double>(value), 0.0, 0.1);
    }, "Synaptogenesis rate must be between 0.0 and 0.1", "Rate should be reasonable for structural plasticity");
    
    addValidationRule("pruning_rate", [this](const ConfigValue& value) {
        return validateDoubleRange("pruning_rate", std::get<double>(value), 0.0, 0.1);
    }, "Pruning rate must be between 0.0 and 0.1", "Rate should be reasonable for structural pruning");
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
        file << entry.key << " = " << getJsonValue(entry.value) << "\n";
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        // Invalidate validation for this key
        pImpl->validationResults.erase(key);
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
    pImpl->validationResults.clear();
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
    
    if (!pImpl->validationResults.empty()) {
        oss << "\nValidation Results:" << std::endl;
        for (const auto& result : pImpl->validationResults) {
            oss << "  " << result.first << ": " << (result.second.isValid ? "✓ Valid" : "✗ Invalid");
            if (!result.second.errors.empty()) {
                oss << " - " << result.second.errors[0];
            }
            oss << std::endl;
        }
    }
    
    return oss.str();
}

bool Config::validate() const {
    bool overallValid = true;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = validateEntry(entry);
        pImpl->validationResults[entry.key] = result;
        if (!result.isValid) {
            overallValid = false;
        }
    }
    
    return overallValid;
}

ValidationResult Config::validateEntry(const ConfigEntry& entry) const {
    ValidationResult result;
    
    // Check required keys
    bool isRequired = std::find(pImpl->requiredKeys.begin(), pImpl->requiredKeys.end(), entry.key) != pImpl->requiredKeys.end();
    if (isRequired && !has(entry.key)) {
        result.addError("Required key missing: " + entry.key);
    }
    
    // Validate key format
    if (!validateKey(entry.key)) {
        result.addError("Invalid key format: '" + entry.key + "'");
    }
    
    // Validate value if key exists
    if (has(entry.key)) {
        if (!validateValue(entry.key, entry.value)) {
            result.addError("Invalid value for key '" + entry.key + "'");
        }
        
        // Check custom validation rules
        for (const auto& rule : pImpl->validationRules) {
            if (rule.key == entry.key && !rule.validator(entry.value)) {
                result.addError(rule.errorMessage);
            }
        }
    }
    
    // Add warnings for potential issues
    if (entry.key == "connection_probability" && has("connection_probability")) {
        double prob = std::get<double>(getOr<double>("connection_probability", 0.0));
        if (prob < 0.05 || prob > 0.95) {
            result.addWarning("Extreme connection probability may affect network dynamics");
        }
    }
    
    if (entry.key == "neuron_count" && has("neuron_count")) {
        int count = getOr<int>("neuron_count", 0);
        if (count < 10) {
            result.addWarning("Very small neuron count may not support complex behavior");
        } else if (count > 5000) {
            result.addWarning("Large neuron count may impact performance");
        }
    }
    
    return result;
}

void Config::addValidationRule(const std::string& key, std::function<bool(const ConfigValue&)> validator,
                              const std::string& errorMessage,
                              const std::string& warningMessage,
                              bool isRequired) {
    ValidationRule rule(key, validator, errorMessage, warningMessage, isRequired);
    pImpl->validationRules.push_back(rule);
    
    if (isRequired) {
        pImpl->requiredKeys.insert(key);
    }
}

bool Config::hasValidationRule(const std::string& key) const {
    return std::any_of(pImpl->validationRules.begin(), pImpl->validationRules.end(),
        [&key](const ValidationRule& rule) { return rule.key == key; });
}

bool Config::validateKey(const std::string& key) const {
    if (key.empty()) return false;
    if (key.size() > 100) return false;
    
    // Check for valid characters
    for (char c : key) {
        if (!(std::isalnum(c) || c == '_' || c == '-')) {
            return false;
        }
    }
    
    // Check for common reserved prefixes
    if (key.rfind("nlm_", 0) == 0 || key.rfind("config_", 0) == 0) {
        return true; // Allow internal keys
    }
    
    // Most keys should follow naming conventions
    if (!std::isalnum(key[0])) {
        return false;
    }
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Type-specific validation
    if (validateType(key, value)) {
        return validateRange(key, value);
    }
    return false;
}

bool Config::validateType(const std::string& key, const ConfigValue& value) const {
    // Check if value type matches expected type for key
    if (key == "neuron_count") {
        return std::holds_alternative<int>(value);
    } else if (key == "connection_probability" || key == "stdp_ltp_weight" || 
               key == "stdp_ltd_weight" || key == "synaptogenesis_rate" ||
               key == "pruning_rate" || key == "dopamine_baseline" ||
               key == "survival_reward_weight" || key == "efficiency_reward_weight" ||
               key == "learning_reward_weight" || key == "exploration_reward_weight") {
        return std::holds_alternative<double>(value);
    } else if (key == "random_seed") {
        return std::holds_alternative<int64_t>(value) || std::holds_alternative<int>(value);
    } else if (key == "simulation_timestep") {
        return std::holds_alternative<double>(value);
    } else if (key == "region_count") {
        return std::holds_alternative<int>(value);
    } else if (key == "log_level" || key == "environment_name") {
        return std::holds_alternative<std::string>(value);
    } else if (key == "log_to_file" || key == "visualization_enabled") {
        return std::holds_alternative<bool>(value);
    }
    
    return true; // Unknown keys are accepted with any type
}

bool Config::validateRange(const std::string& key, const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return validateIntRange(key, std::get<int>(value), -10000, 10000);
    } else if (std::holds_alternative<int64_t>(value)) {
        return validateIntRange(key, static_cast<int>(std::get<int64_t>(value)), -10000, 10000);
    } else if (std::holds_alternative<double>(value)) {
        return validateDoubleRange(key, std::get<double>(value), -1000.0, 1000.0);
    } else if (std::holds_alternative<bool>(value)) {
        return true; // Any boolean value is valid
    } else if (std::holds_alternative<std::string>(value)) {
        return validateString(key, std::get<std::string>(value));
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return validateVector(key, std::get<std::vector<int>>(value));
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return validateVector(key, std::get<std::vector<double>>(value));
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return validateVector(key, std::get<std::vector<std::string>>(value));
    }
    
    return false;
}

bool Config::validateIntRange(const std::string& key, int value, int min, int max) const {
    return value >= min && value <= max;
}

bool Config::validateDoubleRange(const std::string& key, double value, double min, double max) const {
    return value >= min && value <= max;
}

bool Config::validateEnum(const std::string& key, const ConfigValue& value) const {
    if (!std::holds_alternative<std::string>(value)) return false;
    
    const std::string& strValue = std::get<std::string>(value);
    
    // Valid enum values for known keys
    if (key == "log_level") {
        return strValue == "DEBUG" || strValue == "INFO" || strValue == "WARNING" || strValue == "ERROR";
    } else if (key == "environment_name") {
        return strValue == "GridWorld" || strValue == "Continuous2D" || strValue == "3DScene";
    }
    
    return true;
}

bool Config::validateString(const std::string& key, const ConfigValue& value) const {
    if (!std::holds_alternative<std::string>(value)) return false;
    
    const std::string& strValue = std::get<std::string>(value);
    
    if (strValue.empty()) return false;
    if (strValue.size() > 1000) return false;
    
    // Basic string validation
    return true;
}

bool Config::validateVector(const std::string& key, const ConfigValue& value) const {
    if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        return vec.size() <= 1000 && vec.size() >= 0;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        return vec.size() <= 1000 && vec.size() >= 0;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        return vec.size() <= 1000 && vec.size() >= 0;
    }
    
    return false;
}

void Config::setKeyDescription(const std::string& key, const std::string& description) {
    pImpl->keyDescriptions[key] = description;
}

std::string Config::getKeyDescription(const std::string& key) const {
    auto it = pImpl->keyDescriptions.find(key);
    return (it != pImpl->keyDescriptions.end()) ? it->second : "";
}

std::string Config::getKeyTypeName(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->typeName;
    }
    return "unknown";
}

std::string Config::getKeySourceName(const ConfigSource& source) const {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "Command Line";
        case ConfigSource::Runtime: return "Runtime";
        case ConfigSource::Validation: return "Validation";
        default: return "Unknown";
    }
}

ValidationResult Config::getValidationResult(const std::string& key) const {
    auto it = pImpl->validationResults.find(key);
    if (it != pImpl->validationResults.end()) {
        return it->second;
    }
    return ValidationResult(); // Return empty result
}

std::string Config::getValidationErrorSummary() const {
    std::ostringstream oss;
    oss << "Configuration Validation Summary:" << std::endl;
    
    int validCount = 0;
    int errorCount = 0;
    int warningCount = 0;
    
    for (const auto& result : pImpl->validationResults) {
        if (result.second.isValid) {
            validCount++;
        } else {
            errorCount++;
        }
        warningCount += result.second.warnings.size();
    }
    
    oss << "  Valid: " << validCount << std::endl;
    oss << "  Invalid: " << errorCount << std::endl;
    oss << "  Warnings: " << warningCount << std::endl;
    
    if (errorCount > 0) {
        oss << "\nErrors:" << std::endl;
        for (const auto& result : pImpl->validationResults) {
            if (!result.second.isValid) {
                oss << "  " << result.first << ": " << result.second.errors[0] << std::endl;
            }
        }
    }
    
    if (warningCount > 0) {
        oss << "\nWarnings:" << std::endl;
        for (const auto& result : pImpl->validationResults) {
            for (const auto& warning : result.second.warnings) {
                oss << "  " << result.first << ": " << warning << std::endl;
            }
        }
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

bool Config::isNumber(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

bool Config::isInteger(const std::string& str) {
    if (str.empty()) return false;
    size_t pos = 0;
    if (str[0] == '-' || str[0] == '+') pos = 1;
    return std::all_of(str.begin() + pos, str.end(), ::isdigit);
}

bool Config::isFloat(const std::string& str) {
    if (str.empty()) return false;
    
    size_t pos = 0;
    if (str[0] == '-' || str[0] == '+') pos = 1;
    
    bool hasDecimal = false;
    bool hasExponent = false;
    
    for (size_t i = pos; i < str.size(); ++i) {
        if (str[i] == '.') {
            if (hasDecimal || hasExponent) return false;
            hasDecimal = true;
        } else if (str[i] == 'e' || str[i] == 'E') {
            if (hasExponent) return false;
            hasExponent = true;
            // Next char should be sign or digit
            if (i + 1 >= str.size() || !(std::isdigit(str[i + 1]) || str[i + 1] == '-' || str[i + 1] == '+')) {
                return false;
            }
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return true;
}

std::string Config::getJsonValue(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

// Explicit template instantiations
namespace nlm {
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
}

} // namespace nlm