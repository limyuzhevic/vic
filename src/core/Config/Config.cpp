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

// Helper function to check if a value is within bounds
template<typename T>
bool isInBounds(const T& value, const T& min, const T& max) {
    return (min <= value && value <= max);
}

// Helper function to get numeric bounds for a key
template<typename T>
std::pair<T, T> getBounds(const std::string& key) {
    // Return typical bounds for common configuration parameters
    if (key == "neuron_count") {
        return std::make_pair(1, 10000);
    } else if (key == "region_count") {
        return std::make_pair(1, 100);
    } else if (key == "connection_probability") {
        return std::make_pair(0.0, 1.0);
    } else if (key == "random_seed") {
        return std::make_pair(0, 2147483647);
    } else if (key == "simulation_timestep") {
        return std::make_pair(0.0, 1.0);
    } else if (key == "plasticity_learning_rate") {
        return std::make_pair(0.0, 1.0);
    } else if (key == "stdp_ltp_weight") {
        return std::make_pair(0.0, 1.0);
    } else if (key == "stdp_ltd_weight") {
        return std::make_pair(0.0, 1.0);
    } else if (key == "development_synaptogenesis_rate") {
        return std::make_pair(0.0, 1.0);
    } else if (key == "development_pruning_rate") {
        return std::make_pair(0.0, 1.0);
    } else if (key == "dopamine_baseline") {
        return std::make_pair(0.0, 10.0);
    } else {
        return std::make_pair(T(), T());  // Default no bounds
    }
}

// Configuration validation
bool Config::validate() const {
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    for (const auto& entry : pImpl->entries) {
        const std::string& key = entry.key;
        const ConfigValue& value = entry.value;
        
        // Check for null/empty values where not expected
        if (std::holds_alternative<std::string>(value)) {
            const std::string& strValue = std::get<std::string>(value);
            if (strValue.empty() && key != "log_filename") {
                warnings.push_back("Empty string value for key: " + key);
            }
        }
        
        // Validate numeric values with bounds
        if (std::holds_alternative<int64_t>(value)) {
            int64_t intValue = std::get<int64_t>(value);
            auto bounds = getBounds<int64_t>(key);
            if (bounds.first != bounds.second) { // If bounds were defined
                if (!isInBounds(intValue, bounds.first, bounds.second)) {
                    errors.push_back("Value " + std::to_string(intValue) + " for key " + key +
                                   " is out of bounds [" + std::to_string(bounds.first) +
                                   ", " + std::to_string(bounds.second) + "]");
                }
            }
        }
        
        if (std::holds_alternative<double>(value)) {
            double doubleValue = std::get<double>(value);
            auto bounds = getBounds<double>(key);
            if (bounds.first != bounds.second) { // If bounds were defined
                if (!isInBounds(doubleValue, bounds.first, bounds.second)) {
                    errors.push_back("Value " + std::to_string(doubleValue) + " for key " + key +
                                   " is out of bounds [" + std::to_string(bounds.first) +
                                   ", " + std::to_string(bounds.second) + "]");
                }
            }
        }
        
        // Validate boolean values
        if (std::holds_alternative<bool>(value)) {
            bool boolValue = std::get<bool>(value);
            // Boolean values are always valid
        }
        
        // Validate string values (basic sanity checks)
        if (std::holds_alternative<std::string>(value)) {
            const std::string& strValue = std::get<std::string>(value);
            // Check for reasonable string length
            if (strValue.length() > 1000) {
                warnings.push_back("Very long string value for key: " + key);
            }
            
            // Check for potentially problematic characters
            if (key.find("path") != std::string::npos || key.find("file") != std::string::npos) {
                if (strValue.find("...") != std::string::npos || strValue.find("(") != std::string::npos) {
                    warnings.push_back("Potentially problematic file path: " + key);
                }
            }
        }
    }
    
    // Check for required configuration values
    std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    for (const std::string& requiredKey : requiredKeys) {
        if (!has(requiredKey)) {
            errors.push_back("Missing required configuration key: " + requiredKey);
        }
    }
    
    // Log validation results
    if (!errors.empty() || !warnings.empty()) {
        std::ostringstream oss;
        oss << "Configuration validation:";
        
        if (!errors.empty()) {
            oss << "\n  ERRORS:";
            for (const std::string& error : errors) {
                oss << "\n    - " << error;
            }
        }
        
        if (!warnings.empty()) {
            oss << "\n  WARNINGS:";
            for (const std::string& warning : warnings) {
                oss << "\n    - " << warning;
            }
        }
        
        NLM_LOG_INFO(oss.str());
    }
    
    return errors.empty();
}

bool Config::validateWithDetails(std::vector<std::string>& errors, std::vector<std::string>& warnings) const {
    errors.clear();
    warnings.clear();
    
    for (const auto& entry : pImpl->entries) {
        const std::string& key = entry.key;
        const ConfigValue& value = entry.value;
        
        // Basic validation for null/empty values where not expected
        if (std::holds_alternative<std::string>(value)) {
            const std::string& strValue = std::get<std::string>(value);
            if (strValue.empty() && key != "log_filename") {
                warnings.push_back("Empty string value for key: " + key);
            }
        }
        
        // Validate numeric values with bounds
        if (std::holds_alternative<int64_t>(value)) {
            int64_t intValue = std::get<int64_t>(value);
            auto bounds = getBounds<int64_t>(key);
            if (bounds.first != bounds.second) { // If bounds were defined
                if (intValue < bounds.first || intValue > bounds.second) {
                    errors.push_back("Value " + std::to_string(intValue) + " for key " + key +
                                   " is out of bounds [" + std::to_string(bounds.first) +
                                   ", " + std::to_string(bounds.second) + "]");
                }
            }
        }
        
        if (std::holds_alternative<double>(value)) {
            double doubleValue = std::get<double>(value);
            auto bounds = getBounds<double>(key);
            if (bounds.first != bounds.second) { // If bounds were defined
                if (doubleValue < bounds.first || doubleValue > bounds.second) {
                    errors.push_back("Value " + std::to_string(doubleValue) + " for key " + key +
                                   " is out of bounds [" + std::to_string(bounds.first) +
                                   ", " + std::to_string(bounds.second) + "]");
                }
            }
        }
        
        // Validate boolean values
        if (std::holds_alternative<bool>(value)) {
            // Booleans are always valid
        }
        
        // Validate string values
        if (std::holds_alternative<std::string>(value)) {
            const std::string& strValue = std::get<std::string>(value);
            if (strValue.length() > 1000) {
                warnings.push_back("Very long string value for key: " + key);
            }
        }
    }
    
    // Check for required configuration values
    std::vector<std::string> requiredKeys = {
        "neuron_count",
        "region_count", 
        "connection_probability"
    };
    
    for (const std::string& requiredKey : requiredKeys) {
        if (!has(requiredKey)) {
            errors.push_back("Missing required configuration key: " + requiredKey);
        }
    }
    
    return errors.empty();
}

std::vector<std::string> Config::validateKeys(const std::vector<std::string>& validKeys) const {
    std::vector<std::string> invalidKeys;
    
    for (const auto& entry : pImpl->entries) {
        if (std::find(validKeys.begin(), validKeys.end(), entry.key) == validKeys.end()) {
            invalidKeys.push_back(entry.key);
        }
    }
    
    return invalidKeys;
}

std::vector<std::string> Config::getInvalidValues(const std::string& key) const {
    std::vector<std::string> invalidValues;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Check if the value is valid for this key
        const ConfigValue& value = it->value;
        
        if (std::holds_alternative<int64_t>(value)) {
            int64_t intValue = std::get<int64_t>(value);
            auto bounds = getBounds<int64_t>(key);
            if (bounds.first != bounds.second && (intValue < bounds.first || intValue > bounds.second)) {
                invalidValues.push_back(std::to_string(intValue));
            }
        } else if (std::holds_alternative<double>(value)) {
            double doubleValue = std::get<double>(value);
            auto bounds = getBounds<double>(key);
            if (bounds.first != bounds.second && (doubleValue < bounds.first || doubleValue > bounds.second)) {
                invalidValues.push_back(std::to_string(doubleValue));
            }
        } else if (std::holds_alternative<bool>(value)) {
            // Booleans are always valid
        }
    }
    
    return invalidValues;
}

bool Config::validateConfigurationFile(const std::string& filepath) {
    // Create a temporary config and validate it
    Config tempConfig;
    if (tempConfig.loadFromFile(filepath)) {
        return tempConfig.validate();
    }
    return false;
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
}
 // namespace nlm
