#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_set>
#include <map>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Schema for validation
    std::map<std::string, std::function<bool(const ConfigValue&)>> validators;
    // Dependencies between parameters
    std::map<std::string, std::vector<std::string>> dependencies;
    // Parameter descriptions
    std::map<std::string, std::string> descriptions;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default validators and descriptions
    initializeDefaultSchema();
}

Config::~Config() = default;

void Config::initializeDefaultSchema() {
    // Define parameter types and constraints
    validators["brain.neuron_count"] = [](const ConfigValue& v) {
        return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0;
    };
    validators["brain.synapse_density"] = [](const ConfigValue& v) {
        return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
    };
    validators["brain.connection_probability"] = [](const ConfigValue& v) {
        return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0;
    };
    validators["brain.v_thresh"] = [](const ConfigValue& v) {
        return std::holds_alternative<double>(v);
    };
    validators["brain.v_rest"] = [](const ConfigValue& v) {
        return std::holds_alternative<double>(v);
    };
    validators["random_seed"] = [](const ConfigValue& v) {
        return std::holds_alternative<int64_t>(v);
    };
    validators["simulation_timestep"] = [](const ConfigValue& v) {
        return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0;
    };

    // Set descriptions
    descriptions["brain.neuron_count"] = "Total number of neurons in the brain";
    descriptions["brain.synapse_density"] = "Synapse connectivity density (0.0 to 1.0)";
    descriptions["brain.connection_probability"] = "Probability of connection between neurons (0.0 to 1.0)";
    descriptions["brain.v_thresh"] = "Neuron threshold potential (mV)";
    descriptions["brain.v_rest"] = "Resting potential (mV)";
    descriptions["random_seed"] = "Random seed for reproducible simulations";
    descriptions["simulation_timestep"] = "Simulation time step in milliseconds";
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear(); // Clear any existing configuration
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        try {
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
                
                // Set value with validation
                if (!validateAndSet(key, value, ConfigSource::File, lineNumber)) {
                    return false; // Validation failed
                }
            } else {
                // Invalid line format
                return false;
            }
        } catch (const std::exception& e) {
            return false; // Parse error
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    clear(); // Clear any existing configuration
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (!validateAndSet(key, value, ConfigSource::CommandLine)) {
                    return false; // Validation failed
                }
            } else {
                return false; // Invalid format
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (!validateAndSet(key, value, ConfigSource::CommandLine)) {
                return false; // Validation failed
            }
        } else {
            return false; // Invalid argument format
        }
    }
    return true;
}

bool Config::validateAndSet(const std::string& key, const std::string& valueStr, 
                           ConfigSource source, int lineNumber) {
    // Parse value based on key type hints
    ConfigValue parsedValue;
    std::string errorMsg;
    
    if (!parseValue(key, valueStr, parsedValue, errorMsg)) {
        if (lineNumber > 0) {
            errorMsg = "Line " + std::to_string(lineNumber) + ": " + errorMsg;
        }
        throw ValidationError(errorMsg);
    }
    
    // Validate against schema
    if (!validateKey(key, parsedValue, errorMsg)) {
        if (lineNumber > 0) {
            errorMsg = "Line " + std::to_string(lineNumber) + ": " + errorMsg;
        }
        throw ValidationError(errorMsg);
    }
    
    // Check dependencies
    if (!checkDependencies(key, parsedValue, errorMsg)) {
        if (lineNumber > 0) {
            errorMsg = "Line " + std::to_string(lineNumber) + ": " + errorMsg;
        }
        throw ValidationError(errorMsg);
    }
    
    // Check conflicts
    if (!checkConflicts(key, parsedValue, errorMsg)) {
        if (lineNumber > 0) {
            errorMsg = "Line " + std::to_string(lineNumber) + ": " + errorMsg;
        }
        throw ValidationError(errorMsg);
    }
    
    set(key, parsedValue, source);
    return true;
}

bool Config::validateAndSet(const std::string& key, const std::string& valueStr, 
                           ConfigSource source) {
    return validateAndSet(key, valueStr, source, -1);
}

bool Config::parseValue(const std::string& key, const std::string& valueStr, 
                       ConfigValue& parsedValue, std::string& errorMsg) {
    // Check if key exists (possibly with different type)
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
        // Key exists, try to preserve type
        const ConfigValue& existingValue = it->value;
        
        if (std::holds_alternative<int>(existingValue)) {
            try {
                int intValue = std::stoi(valueStr);
                parsedValue = intValue;
                return true;
            } catch (...) {
                errorMsg = "Cannot parse '" + valueStr + "' as integer for key '" + key + "'";
                return false;
            }
        } else if (std::holds_alternative<int64_t>(existingValue)) {
            try {
                int64_t intValue = std::stoll(valueStr);
                parsedValue = intValue;
                return true;
            } catch (...) {
                errorMsg = "Cannot parse '" + valueStr + "' as integer for key '" + key + "'";
                return false;
            }
        } else if (std::holds_alternative<double>(existingValue)) {
            try {
                double doubleValue = std::stod(valueStr);
                parsedValue = doubleValue;
                return true;
            } catch (...) {
                errorMsg = "Cannot parse '" + valueStr + "' as double for key '" + key + "'";
                return false;
            }
        } else if (std::holds_alternative<bool>(existingValue)) {
            if (valueStr == "true" || valueStr == "1" || valueStr == "yes" || valueStr == "on") {
                parsedValue = true;
                return true;
            } else if (valueStr == "false" || valueStr == "0" || valueStr == "no" || valueStr == "off") {
                parsedValue = false;
                return true;
            } else {
                errorMsg = "Cannot parse '" + valueStr + "' as boolean for key '" + key + "' (use true/false, 1/0, yes/no, on/off)";
                return false;
            }
        } else if (std::holds_alternative<std::string>(existingValue)) {
            parsedValue = valueStr;
            return true;
        }
    }
    
    // Key doesn't exist or is of unknown type, try to infer from key name
    if (key.find("neuron") != std::string::npos) {
        try {
            int64_t intValue = std::stoll(valueStr);
            parsedValue = intValue;
            return true;
        } catch (...) {
            errorMsg = "Cannot parse neuron count as integer";
            return false;
        }
    } else if (key.find("rate") != std::string::npos || key.find("ratio") != std::string::npos || 
               key.find("scale") != std::string::npos || key.find("density") != std::string::npos ||
               key.find("probability") != std::string::npos || key.find("threshold") != std::string::npos ||
               key.find("potential") != std::string::npos || key.find("timestep") != std::string::npos) {
        try {
            double doubleValue = std::stod(valueStr);
            parsedValue = doubleValue;
            return true;
        } catch (...) {
            errorMsg = "Cannot parse parameter as double";
            return false;
        }
    } else if (key.find("enable") != std::string::npos || key.find("active") != std::string::npos ||
               key.find("enabled") != std::string::npos || key.find("flag") != std::string::npos) {
        if (valueStr == "true" || valueStr == "1" || valueStr == "yes" || valueStr == "on") {
            parsedValue = true;
            return true;
        } else if (valueStr == "false" || valueStr == "0" || valueStr == "no" || valueStr == "off") {
            parsedValue = false;
            return true;
        } else {
            errorMsg = "Cannot parse boolean parameter";
            return false;
        }
    } else {
        // Default to string
        parsedValue = valueStr;
        return true;
    }
}

bool Config::validateKey(const std::string& key, const ConfigValue& value, std::string& errorMsg) {
    // Check if there's a validator for this key
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        if (!validatorIt->second(value)) {
            errorMsg = "Invalid value '" + (std::holds_alternative<std::string>(value) ? std::get<std::string>(value) : 
                                               std::holds_alternative<double>(value) ? std::to_string(std::get<double>(value)) : 
                                               std::holds_alternative<int>(value) ? std::to_string(std::get<int>(value)) :
                                               std::holds_alternative<int64_t>(value) ? std::to_string(std::get<int64_t>(value)) :
                                               std::holds_alternative<bool>(value) ? (std::get<bool>(value) ? "true" : "false") : "") +
                             " for key '" + key + "'";
            return false;
        }
    }
    
    // Validate that value type matches expected type based on key name
    if (key.find("neuron") != std::string::npos && !std::holds_alternative<int64_t>(value)) {
        errorMsg = "Neuron count must be an integer";
        return false;
    } else if ((key.find("rate") != std::string::npos || key.find("ratio") != std::string::npos || 
                key.find("scale") != std::string::npos || key.find("density") != std::string::npos ||
                key.find("probability") != std::string::npos || key.find("threshold") != std::string::npos ||
                key.find("potential") != std::string::npos || key.find("timestep") != std::string::npos) && 
               !std::holds_alternative<double>(value)) {
        errorMsg = "This parameter must be a floating-point number";
        return false;
    } else if ((key.find("enable") != std::string::npos || key.find("active") != std::string::npos ||
                key.find("enabled") != std::string::npos || key.find("flag") != std::string::npos) && 
               !std::holds_alternative<bool>(value)) {
        errorMsg = "This parameter must be a boolean (true/false)";
        return false;
    }
    
    return true;
}

bool Config::checkDependencies(const std::string& key, const ConfigValue& value, std::string& errorMsg) {
    // Check if this key has dependencies on other keys
    auto depIt = pImpl->dependencies.find(key);
    if (depIt != pImpl->dependencies.end()) {
        for (const std::string& depKey : depIt->second) {
            auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&depKey](const ConfigEntry& e) { return e.key == depKey; });
            
            if (existingIt != pImpl->entries.end()) {
                // Dependency exists, check if it's compatible
                // This would depend on specific dependency rules
                // For now, just check that the dependency is present
            }
        }
    }
    
    // For now, just return true (dependency checking can be expanded)
    return true;
}

bool Config::checkConflicts(const std::string& key, const ConfigValue& value, std::string& errorMsg) {
    // Check for conflicting parameter combinations
    if (key == "plasticity.stdp.enable" && std::get<bool>(value) == true) {
        // If STDP is enabled, ensure plasticity is not disabled globally
        auto plasticityIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [](const ConfigEntry& e) { return e.key == "plasticity.enable"; });
        if (plasticityIt != pImpl->entries.end() && !std::get<bool>(plasticityIt->value)) {
            errorMsg = "Cannot enable STDP when global plasticity is disabled";
            return false;
        }
    }
    
        // Add more conflict checks as needed
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
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
    // Validate key format
    if (key.empty() || key.find(' ') != std::string::npos) {
        throw ValidationError("Invalid key format: '" + key + "'");
    }
    
    // Find existing entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, getDescription(key));
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ") - " << getDescription(entry.key) << "\n";
    }
    
    return oss.str();
}

std::string Config::getDescription(const std::string& key) const {
    auto descIt = pImpl->descriptions.find(key);
    if (descIt != pImpl->descriptions.end()) {
        return descIt->second;
    }
    
    // Generate description from key name if not found
    std::string desc = key;
    std::replace(desc.begin(), desc.end(), '.', ' ');
    std::replace(desc.begin(), desc.end(), '_', ' ');
    return desc;
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