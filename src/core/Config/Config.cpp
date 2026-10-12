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

// Enhanced configuration validation and error handling

struct ConfigValidationResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    std::string getSummary() const {
        std::ostringstream oss;
        oss << "Validation Result: ";
        if (valid) {
            oss << "VALID";
        } else {
            oss << "INVALID";
        }
        oss << " (" << errors.size() << " errors, " << warnings.size() << " warnings)";
        return oss.str();
    }
};

ConfigValidationResult Config::validate() const {
    ConfigValidationResult result;
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        // Validate key format
        if (entry.key.empty()) {
            result.errors.push_back("Empty configuration key found");
        } else if (entry.key.find('=') != std::string::npos) {
            result.errors.push_back("Configuration key contains '=' character: " + entry.key);
        } else if (entry.key.find(' ') != std::string::npos) {
            result.warnings.push_back("Configuration key contains spaces: " + entry.key);
        }
        
        // Validate value format based on key
        validateEntryValue(entry, result);
    }
    
    return result;
}

void Config::validateEntryValue(const ConfigEntry& entry, ConfigValidationResult& result) const {
    // Key-specific validation rules
    if (entry.key == "random_seed") {
        if (!std::holds_alternative<int64_t>(entry.value)) {
            result.errors.push_back("random_seed must be an integer");
        } else {
            int64_t seed = std::get<int64_t>(entry.value);
            if (seed < 0) {
                result.warnings.push_back("random_seed should be non-negative");
            }
        }
    }
    else if (entry.key == "simulation_timestep") {
        if (!std::holds_alternative<double>(entry.value)) {
            result.errors.push_back("simulation_timestep must be a floating-point number");
        } else {
            double timestep = std::get<double>(entry.value);
            if (timestep <= 0.0) {
                result.errors.push_back("simulation_timestep must be positive");
            } else if (timestep > 1.0) {
                result.warnings.push_back("simulation_timestep > 1.0 may impact simulation accuracy");
            }
        }
    }
    else if (entry.key == "neuron_count") {
        if (!std::holds_alternative<int64_t>(entry.value)) {
            result.errors.push_back("neuron_count must be an integer");
        } else {
            int64_t neuronCount = std::get<int64_t>(entry.value);
            if (neuronCount <= 0) {
                result.errors.push_back("neuron_count must be positive");
            } else if (neuronCount > 1000000) {
                result.warnings.push_back("neuron_count is very large - may impact performance");
            }
        }
    }
    else if (entry.key == "region_count") {
        if (!std::holds_alternative<int64_t>(entry.value)) {
            result.errors.push_back("region_count must be an integer");
        } else {
            int64_t regionCount = std::get<int64_t>(entry.value);
            if (regionCount <= 0) {
                result.errors.push_back("region_count must be positive");
            }
        }
    }
    else if (entry.key == "connection_probability") {
        if (!std::holds_alternative<float>(entry.value)) {
            result.errors.push_back("connection_probability must be a floating-point number");
        } else {
            float prob = std::get<float>(entry.value);
            if (prob < 0.0f || prob > 1.0f) {
                result.errors.push_back("connection_probability must be between 0.0 and 1.0");
            }
        }
    }
    else if (entry.key == "stdp_ltp_weight" || entry.key == "stdp_ltd_weight") {
        if (!std::holds_alternative<float>(entry.value)) {
            result.errors.push_back(entry.key + " must be a floating-point number");
        } else {
            float weight = std::get<float>(entry.value);
            if (weight < 0.0f) {
                result.warnings.push_back(entry.key + " is negative - may cause instability");
            }
        }
    }
    else if (entry.key == "synaptogenesis_rate" || entry.key == "pruning_rate") {
        if (!std::holds_alternative<float>(entry.value)) {
            result.errors.push_back(entry.key + " must be a floating-point number");
        } else {
            float rate = std::get<float>(entry.value);
            if (rate < 0.0f) {
                result.errors.push_back(entry.key + " must be non-negative");
            } else if (rate > 0.1f) {
                result.warnings.push_back(entry.key + " is high - may cause rapid structural changes");
            }
        }
    }
}

std::string Config::validateAndReport() const {
    auto result = validate();
    
    std::ostringstream oss;
    oss << "Configuration Validation Report:" << std::endl;
    oss << "Total entries: " << pImpl->entries.size() << std::endl;
    oss << result.getSummary() << std::endl;
    
    if (!result.errors.empty()) {
        oss << std::endl << "ERRORS:" << std::endl;
        for (size_t i = 0; i < result.errors.size(); ++i) {
            oss << "  " << (i + 1) << ". " << result.errors[i] << std::endl;
        }
    }
    
    if (!result.warnings.empty()) {
        oss << std::endl << "WARNINGS:" << std::endl;
        for (size_t i = 0; i < result.warnings.size(); ++i) {
            oss << "  " << (i + 1) << ". " << result.warnings[i] << std::endl;
        }
    }
    
    return oss.str();
}

bool Config::validate(const std::vector<std::pair<std::string, std::string>>& validationRules) const {
    ConfigValidationResult result;
    
    for (const auto& rule : validationRules) {
        const std::string& key = rule.first;
        const std::string& ruleType = rule.second;
        
        if (!has(key)) {
            result.errors.push_back("Missing required configuration key: " + key);
            continue;
        }
        
        validateKeyRule(key, ruleType, result);
    }
    
    return result.valid;
}

void Config::validateKeyRule(const std::string& key, const std::string& ruleType, ConfigValidationResult& result) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return; // Key not found (should have been caught earlier)
    }
    
    if (ruleType == "required") {
        // Key is already required (exists)
    }
    else if (ruleType == "non_empty") {
        if (std::holds_alternative<std::string>(it->value) && std::get<std::string>(it->value).empty()) {
            result.errors.push_back("Configuration key '" + key + "' cannot be empty");
        }
    }
    else if (ruleType == "positive") {
        if (std::holds_alternative<int64_t>(it->value)) {
            int64_t val = std::get<int64_t>(it->value);
            if (val <= 0) {
                result.errors.push_back("Configuration key '" + key + "' must be positive");
            }
        } else if (std::holds_alternative<double>(it->value)) {
            double val = std::get<double>(it->value);
            if (val <= 0.0) {
                result.errors.push_back("Configuration key '" + key + "' must be positive");
            }
        }
    }
    else if (ruleType == "range") {
        // For range validation, we'd need to parse min/max from the rule
        // This is a placeholder for future enhancement
        result.warnings.push_back("Range validation for key '" + key + "' not yet implemented");
    }
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

} // namespace nlm
