#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <regex>
#include <limits>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schema;
    std::unordered_map<std::string, ConfigSchema> schemaMap;
    
    Impl() {
        initializeDefaultSchema();
    }
    
    void initializeDefaultSchema() {
        // Default configuration schema
        schema = {
            {"random_seed", "int", "Random seed for reproducible experiments", false, 42, {}, 0, 1000000},
            {"simulation_timestep", "double", "Simulation timestep in seconds", false, 0.001, {}, 0.0, 1.0},
            {"neuron_count", "int", "Total number of neurons in the brain", false, 1000, {}, 1, 1000000},
            {"region_count", "int", "Number of brain regions", false, 2, {}, 1, 1000},
            {"connection_probability", "double", "Connection probability for random connectivity", false, 0.1, {}, 0.0, 1.0},
            {"stdp_ltp_weight", "double", "STDP LTP weight", false, 0.01, {}, 0.0, 1.0},
            {"stdp_ltd_weight", "double", "STDP LTD weight", false, 0.012, {}, 0.0, 1.0},
            {"stdp_time_constant", "double", "STDP time constant", false, 20.0, {}, 0.0, 1000.0},
            {"development_synaptogenesis_rate", "double", "Development synaptogenesis rate", false, 0.001, {}, 0.0, 1.0},
            {"development_pruning_rate", "double", "Development pruning rate", false, 0.0001, {}, 0.0, 1.0},
            {"reward_discount_factor", "double", "Reward discount factor", false, 0.99, {}, 0.0, 1.0},
            {"environment_width", "int", "Environment width", false, 10, {}, 1, 1000},
            {"environment_height", "int", "Environment height", false, 10, {}, 1, 1000},
            {"log_level", "string", "Log level", false, std::string("INFO"), {"DEBUG", "INFO", "WARN", "ERROR", "FATAL"}, 0, std::numeric_limits<double>::max()},
            {"max_simulation_steps", "int", "Maximum simulation steps", false, 10000, {}, 0, 1000000},
            {"visualization_enabled", "bool", "Enable visualization", false, false, {}, 0, 1},
            {"visualization_update_rate", "double", "Visualization update rate", false, 30.0, {}, 0.0, 1000.0}
        };
        
        for (const auto& entry : schema) {
            schemaMap[entry.key] = entry;
        }
    }
    
    bool validateValue(const std::string& key, const ConfigValue& value) const {
        auto it = schemaMap.find(key);
        if (it == schemaMap.end()) {
            // Key not in schema - allow it (backward compatibility)
            return true;
        }
        
        const ConfigSchema& schema = it->second;
        
        // Type validation
        bool typeValid = false;
        switch (schema.type[0]) { // First character for type check
            case 'i':
                typeValid = std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
                break;
            case 'd':
                typeValid = std::holds_alternative<double>(value);
                break;
            case 'b':
                typeValid = std::holds_alternative<bool>(value);
                break;
            case 's':
                typeValid = std::holds_alternative<std::string>(value);
                break;
            case 'v': // vector type - check based on specific type
                if (std::holds_alternative<std::vector<int>>(value) || 
                    std::holds_alternative<std::vector<double>>(value) ||
                    std::holds_alternative<std::vector<std::string>>(value)) {
                    typeValid = true;
                }
                break;
        }
        
        if (!typeValid) return false;
        
        // Range and allowed value validation
        double numValue = 0.0;
        
        if (std::holds_alternative<int>(value)) {
            numValue = static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            numValue = static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            numValue = std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            numValue = std::get<bool>(value) ? 1.0 : 0.0;
        } else if (std::holds_alternative<std::string>(value)) {
            // For string, check against allowed values
            if (!schema.allowedValues.empty()) {
                bool allowed = false;
                for (const auto& allowedVal : schema.allowedValues) {
                    if (std::holds_alternative<std::string>(allowedVal) && 
                        std::get<std::string>(allowedVal) == std::get<std::string>(value)) {
                        allowed = true;
                        break;
                    }
                }
                if (!allowed) return false;
            }
            return true; // String validation passed
        }
        
        // Numeric range validation
        if (numValue < schema.minValue || numValue > schema.maxValue) {
            return false;
        }
        
        return true;
    }
    
    std::string getValidationError(const std::string& key, const ConfigValue& value) const {
        auto it = schemaMap.find(key);
        if (it == schemaMap.end()) {
            return "Key not in schema (but allowed for backward compatibility)";
        }
        
        const ConfigSchema& schema = it->second;
        
        if (!validateValue(key, value)) {
            return "Value does not match schema type or range";  
        }
        
        return "";
    }
};

namespace nlm {

// Helper function to convert ConfigValue to JSON
json Config::configValueToJson(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return std::get<std::vector<int>>(value);
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return std::get<std::vector<double>>(value);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return std::get<std::vector<std::string>>(value);
    }
    return nullptr;
}

// Helper function to add default values for missing schema entries
void Config::addDefaultsForSchema() {
    for (const auto& schema : pImpl->schema) {
        if (!has(schema.key)) {
            ConfigValue defaultValue;
            if (schema.type == "int") {
                if (std::holds_alternative<int>(schema.defaultValue)) {
                    defaultValue = std::get<int>(schema.defaultValue);
                } else if (std::holds_alternative<int64_t>(schema.defaultValue)) {
                    defaultValue = std::get<int64_t>(schema.defaultValue);
                }
            } else if (schema.type == "double") {
                defaultValue = std::get<double>(schema.defaultValue);
            } else if (schema.type == "bool") {
                defaultValue = std::get<bool>(schema.defaultValue);
            } else if (schema.type == "string") {
                defaultValue = std::get<std::string>(schema.defaultValue);
            }
            
            if (defaultValue.index()) {
                set(schema.key, defaultValue, ConfigSource::Default);
            }
        }
    }
}

// Helper function to validate ConfigValue against schema constraints
bool Config::validateConfigValue(const std::string& key, const ConfigValue& value, const std::vector<ConfigSchema>& schema) {
    for (const auto& entry : schema) {
        if (entry.key == key) {
            bool typeValid = false;
            if (entry.type == "int" && (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value))) {
                typeValid = true;
            } else if (entry.type == "double" && std::holds_alternative<double>(value)) {
                typeValid = true;
            } else if (entry.type == "bool" && std::holds_alternative<bool>(value)) {
                typeValid = true;
            } else if (entry.type == "string" && std::holds_alternative<std::string>(value)) {
                typeValid = true;
            } else if (entry.type == "int_array" && std::holds_alternative<std::vector<int>>(value)) {
                typeValid = true;
            } else if (entry.type == "double_array" && std::holds_alternative<std::vector<double>>(value)) {
                typeValid = true;
            } else if (entry.type == "string_array" && std::holds_alternative<std::vector<std::string>>(value)) {
                typeValid = true;
            }
            
            if (!typeValid) {
                return false;
            }
            
            if (entry.type == "int") {
                int val = std::holds_alternative<int>(value) ? std::get<int>(value) : std::get<int64_t>(value);
                if (val < entry.minValue || val > entry.maxValue) {
                    return false;
                }
            } else if (entry.type == "double") {
                double val = std::get<double>(value);
                if (val < entry.minValue || val > entry.maxValue) {
                    return false;
                }
            } else if (entry.type == "bool") {
                bool val = std::get<bool>(value);
                if (val < entry.minValue || val > entry.maxValue) {
                    return false;
                }
            }
            
            if (!entry.allowedValues.empty()) {
                bool allowed = false;
                for (const auto& allowedVal : entry.allowedValues) {
                    if (std::holds_alternative<std::string>(allowedVal) && 
                        std::get<std::string>(allowedVal) == std::get<std::string>(value)) {
                        allowed = true;
                        break;
                    }
                }
                if (!allowed) {
                    return false;
                }
            }
            
            return true;
        }
    }
    
    return true;
}

// Helper function to validate value and log error
bool Config::validateAndLog(const std::string& key, const ConfigValue& value, const std::string& operation) {
    bool isValid = pImpl->validateValue(key, value);
    if (!isValid) {
        NLM_LOG_WARN("Invalid configuration value for key '" + key + "' during " + operation + ": " + 
                    pImpl->getValidationError(key, value));
    }
    return isValid;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    json j;
    
    for (const auto& entry : pImpl->entries) {
        j[entry.key] = configValueToJson(entry.value);
    }
    
    try {
        file << j.dump(2);
        return true;
    } catch (const json::exception& e) {
        NLM_LOG_ERROR("Failed to serialize config to JSON: " + std::string(e.what()));
        return false;
    }
}

// Config constructor/destructor
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // Clear existing entries
        pImpl->clear();
        
        // Parse JSON object
        if (j.is_object()) {
            for (auto it = j.begin(); it != j.end(); ++it) {
                const std::string& key = it.key();
                
                // Convert JSON value to ConfigValue
                ConfigValue configValue;
                const json& value = it.value();
                
                if (value.is_number_integer()) {
                    // Check if it's within int64_t range
                    if (value.get<int64_t>() <= std::numeric_limits<int>::max() &&
                        value.get<int64_t>() >= std::numeric_limits<int>::min()) {
                        configValue = value.get<int>();
                    } else {
                        configValue = value.get<int64_t>();
                    }
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_array()) {
                    // Handle arrays
                    std::vector<int> intArray;
                    std::vector<double> doubleArray;
                    std::vector<std::string> stringArray;
                    
                    bool allInts = true;
                    bool allDoubles = true;
                    bool allStrings = true;
                    
                    for (const auto& item : value) {
                        if (item.is_number_integer()) {
                            intArray.push_back(item.get<int>());
                            if (!item.is_number_float()) allDoubles = false;
                            if (!item.is_string()) allStrings = false;
                        } else if (item.is_number_float()) {
                            doubleArray.push_back(item.get<double>());
                            if (!item.is_integer()) allInts = false;
                            if (!item.is_string()) allStrings = false;
                        } else if (item.is_string()) {
                            stringArray.push_back(item.get<std::string>());
                            allInts = false;
                            allDoubles = false;
                        } else {
                            allInts = false;
                            allDoubles = false;
                            allStrings = false;
                            break;
                        }
                    }
                    
                    if (allInts) {
                        configValue = intArray;
                    } else if (allDoubles) {
                        configValue = doubleArray;
                    } else if (allStrings) {
                        configValue = stringArray;
                    } else {
                        // Mixed type array - convert to string representation
                        configValue = value.dump();
                    }
                } else if (value.is_null()) {
                    configValue = std::string("");
                } else {
                    // Fallback to string representation
                    configValue = value.dump();
                }
                
                // Validate the value
                std::string validationError = pImpl->getValidationError(key, configValue);
                if (!validationError.empty()) {
                    NLM_LOG_WARN("Invalid configuration value for key '" + key + "': " + validationError);
                    // Skip invalid values but continue processing
                    continue;
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
        
        // Set default values for missing schema entries
        pImpl->addDefaultsForSchema();
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

// Template instantiations

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
    validateAndLog(key, value, "set");
    
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