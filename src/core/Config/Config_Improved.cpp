// Config_Improved.cpp - Implementation of Improved Configuration System
#include "Config_Improved.hpp"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>  // Optional - if YAML support available

namespace nlm {

// ConfigSchema implementation
void ConfigSchema::addEntry(const std::string& key, const std::string& type,
                           const std::string& description, bool required,
                           const ConfigValue& defaultValue,
                           const std::vector<std::string>& allowedValues) {
    ConfigSchemaEntry entry;
    entry.key = key;
    entry.type = type;
    entry.description = description;
    entry.required = required;
    entry.defaultValue = defaultValue;
    entry.allowedValues = allowedValues;
    
    // Set default validator based on type
    if (type == "int" || type == "int64") {
        entry.validator = [](const ConfigValue& value) {
            return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value) 
                   ? ValidationResult() 
                   : ValidationResult(false, "Expected integer type for key: " + std::string("unknown key"), "type");
        };
    } else if (type == "double" || type == "float") {
        entry.validator = [](const ConfigValue& value) {
            return std::holds_alternative<double>(value) 
                   ? ValidationResult() 
                   : ValidationResult(false, "Expected double/float type", "type");
        };
    } else if (type == "bool") {
        entry.validator = [](const ConfigValue& value) {
            return std::holds_alternative<bool>(value) 
                   ? ValidationResult() 
                   : ValidationResult(false, "Expected boolean type", "type");
        };
    } else if (type == "string") {
        entry.validator = [allowedValues](const ConfigValue& value) {
            if (!std::holds_alternative<std::string>(value)) {
                return ValidationResult(false, "Expected string type", "type");
            }
            if (!allowedValues.empty()) {
                const std::string& strVal = std::get<std::string>(value);
                if (std::find(allowedValues.begin(), allowedValues.end(), strVal) == allowedValues.end()) {
                    return ValidationResult(false, "Value not in allowed list", "allowed_values");
                }
            }
            return ValidationResult();
        };
    } else if (type.find("list<") == 0 && type.back() == '>') {
        entry.validator = [type](const ConfigValue& value) {
            // Validate list types
            if (type == "list<int>") {
                return std::holds_alternative<std::vector<int>>(value) 
                       ? ValidationResult() 
                       : ValidationResult(false, "Expected list<int> type", "type");
            } else if (type == "list<double>") {
                return std::holds_alternative<std::vector<double>>(value) 
                       ? ValidationResult() 
                       : ValidationResult(false, "Expected list<double> type", "type");
            } else if (type == "list<string>") {
                return std::holds_alternative<std::vector<std::string>>(value) 
                       ? ValidationResult() 
                       : ValidationResult(false, "Expected list<string> type", "type");
            }
            return ValidationResult();
        };
    }
    
    schema[key] = entry;
    if (required) {
        requiredKeys.push_back(key);
    }
}

void ConfigSchema::addValidator(const std::string& key,
                                std::function<ValidationResult(const ConfigValue&)> validator) {
    if (schema.find(key) != schema.end()) {
        schema[key].validator = validator;
    }
}

ValidationResult ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    auto it = schema.find(key);
    if (it == schema.end()) {
        return ValidationResult(true); // Not in schema - pass validation
    }
    
    const ConfigSchemaEntry& entry = it->second;
    
    // Check type
    if (entry.validator) {
        ValidationResult typeResult = entry.validator(value);
        if (!typeResult.isValid) {
            typeResult.path = "type";
            return typeResult;
        }
    }
    
    // Check allowed values (for string types)
    if (!entry.allowedValues.empty() && std::holds_alternative<std::string>(value)) {
        const std::string& strVal = std::get<std::string>(value);
        if (std::find(entry.allowedValues.begin(), entry.allowedValues.end(), strVal) == entry.allowedValues.end()) {
            return ValidationResult(false, "Value not in allowed list: " + strVal, "allowed_values");
        }
    }
    
    return ValidationResult();
}

std::vector<ValidationResult> ConfigSchema::validateAll(const std::map<std::string, ConfigValue>& config) const {
    std::vector<ValidationResult> results;
    
    for (const auto& pair : config) {
        results.push_back(validate(pair.first, pair.second));
    }
    
    return results;
}

const ConfigSchemaEntry* ConfigSchema::getEntry(const std::string& key) const {
    auto it = schema.find(key);
    return (it != schema.end()) ? &it->second : nullptr;
}

std::vector<std::string> ConfigSchema::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(schema.size());
    for (const auto& pair : schema) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool ConfigSchema::hasKey(const std::string& key) const {
    return schema.find(key) != schema.end();
}

std::vector<std::string> ConfigSchema::getRequiredKeys() const {
    return requiredKeys;
}

ConfigValue ConfigSchema::getDefaultValue(const std::string& key) const {
    auto it = schema.find(key);
    if (it != schema.end()) {
        return it->second.defaultValue;
    }
    return ConfigValue{};
}

nlohmann::json ConfigSchema::toJson() const {
    nlohmann::json json;
    for (const auto& pair : schema) {
        const ConfigSchemaEntry& entry = pair.second;
        nlohmann::json entryJson;
        entryJson["key"] = entry.key;
        entryJson["type"] = entry.type;
        entryJson["description"] = entry.description;
        entryJson["required"] = entry.required;
        entryJson["default"] = entry.defaultValue; // This needs to be converted from ConfigValue to JSON
        entryJson["allowedValues"] = entry.allowedValues;
        json.push_back(entryJson);
    }
    return json;
}

bool ConfigSchema::loadFromJson(const nlohmann::json& json) {
    try {
        for (const auto& entryJson : json) {
            std::string key = entryJson["key"].get<std::string>();
            std::string type = entryJson["type"].get<std::string>();
            std::string description = entryJson["description"].get<std::string>();
            bool required = entryJson["required"].get<bool>();
            
            ConfigValue defaultValue;
            if (entryJson.contains("default")) {
                // Convert JSON to ConfigValue (simplified)
                nlohmann::json defaultJson = entryJson["default"];
                if (defaultJson.is_number_integer()) {
                    defaultValue = defaultJson.get<int>();
                } else if (defaultJson.is_number()) {
                    defaultValue = defaultJson.get<double>();
                } else if (defaultJson.is_boolean()) {
                    defaultValue = defaultJson.get<bool>();
                } else if (defaultJson.is_string()) {
                    defaultValue = defaultJson.get<std::string>();
                } else if (defaultJson.is_array()) {
                    // For arrays, we need to infer the type
                    std::vector<int> intList;
                    std::vector<double> doubleList;
                    std::vector<std::string> stringList;
                    
                    for (const auto& item : defaultJson) {
                        if (item.is_number_integer()) {
                            intList.push_back(item.get<int>());
                        } else if (item.is_number()) {
                            doubleList.push_back(item.get<double>());
                        } else if (item.is_string()) {
                            stringList.push_back(item.get<std::string>());
                        }
                    }
                    
                    if (!intList.empty()) {
                        defaultValue = intList;
                    } else if (!doubleList.empty()) {
                        defaultValue = doubleList;
                    } else if (!stringList.empty()) {
                        defaultValue = stringList;
                    }
                }
            }
            
            std::vector<std::string> allowedValues;
            if (entryJson.contains("allowedValues") && entryJson["allowedValues"].is_array()) {
                allowedValues = entryJson["allowedValues"].get<std::vector<std::string>>();
            }
            
            addEntry(key, type, description, required, defaultValue, allowedValues);
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigSchema::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json;
        file >> json;
        
        return loadFromJson(json);
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigSchema::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json = toJson();
        file << json.dump(2);
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::shared_ptr<ConfigSchema> schema;
    std::vector<ValidationResult> validationResults;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    setDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Try JSON first
        nlohmann::json json;
        file >> json;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            if (it->is_number_integer()) {
                value = it->get<int>();
            } else if (it->is_number()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                // Handle arrays
                std::vector<int> intList;
                std::vector<double> doubleList;
                std::vector<std::string> stringList;
                
                for (const auto& item : *it) {
                    if (item.is_number_integer()) {
                        intList.push_back(item.get<int>());
                    } else if (item.is_number()) {
                        doubleList.push_back(item.get<double>());
                    } else if (item.is_string()) {
                        stringList.push_back(item.get<std::string>());
                    }
                }
                
                if (!intList.empty()) {
                    value = intList;
                } else if (!doubleList.empty()) {
                    value = doubleList;
                } else if (!stringList.empty()) {
                    value = stringList;
                }
            }
            
            ConfigEntry entry;
            entry.key = key;
            entry.value = value;
            entry.source = ConfigSource::File;
            entry.description = "";
            
            pImpl->entries.push_back(entry);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
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

bool Config::loadFromJson(const std::string& jsonString) {
    try {
        nlohmann::json json = nlohmann::json::parse(jsonString);
        return loadFromFile("<string>"); // Need to modify to accept JSON string
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYaml(const std::string& yamlString) {
    try {
        // YAML parsing (requires yaml-cpp)
        // YAML::Node yaml = YAML::Load(yamlString);
        // For now, just return false
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        nlohmann::json json;
        
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            json[key] = entry.value; // This needs proper conversion from ConfigValue to JSON
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << json.dump(2);
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void Config::setSchema(std::shared_ptr<ConfigSchema> schema) {
    pImpl->schema = schema;
}

std::shared_ptr<ConfigSchema> Config::getSchema() const {
    return pImpl->schema;
}

void Config::setDefaultSchema() {
    if (!defaultSchema) {
        defaultSchema = std::make_shared<ConfigSchema>();
        
        // Add default schema entries
        defaultSchema->addEntry("random_seed", "int64", "Random seed for reproducible experiments", true, static_cast<int64_t>(42));
        defaultSchema->addEntry("simulation_timestep", "double", "Simulation timestep in seconds", true, 0.001);
        defaultSchema->addEntry("neuron_count", "int64", "Number of neurons in the brain", true, static_cast<int64_t>(500));
        defaultSchema->addEntry("region_count", "int64", "Number of brain regions", true, static_cast<int64_t>(1));
        defaultSchema->addEntry("connection_probability", "double", "Connection probability for random connectivity", true, 0.15);
        defaultSchema->addEntry("plasticity_learning_rate", "double", "Plasticity learning rate", true, 0.01);
        defaultSchema->addEntry("stdp_ltp_weight", "double", "STDP LTP weight", true, 0.02);
        defaultSchema->addEntry("stdp_ltd_weight", "double", "STDP LTD weight", true, 0.015);
        defaultSchema->addEntry("stdp_time_constant", "double", "STDP time constant (ms)", true, 20.0);
        defaultSchema->addEntry("synaptogenesis_rate", "double", "Structural synaptogenesis rate", true, 0.0001);
        defaultSchema->addEntry("pruning_rate", "double", "Structural pruning rate", true, 0.00001);
        defaultSchema->addEntry("dopamine_baseline", "double", "Dopamine baseline level", true, 0.1);
        defaultSchema->addEntry("reward_discount_factor", "double", "Reward discount factor", true, 0.99);
        defaultSchema->addEntry("environment_name", "string", "Environment name", true, "GridWorld");
        defaultSchema->addEntry("environment_width", "int64", "Environment width", true, 10);
        defaultSchema->addEntry("environment_height", "int64", "Environment height", true, 10);
        defaultSchema->addEntry("log_level", "string", "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)", 
                               true, "INFO", {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"});
        defaultSchema->addEntry("log_to_file", "bool", "Log to file", true, false);
        defaultSchema->addEntry("log_filename", "string", "Log filename", true, "nlm.log");
        defaultSchema->addEntry("max_simulation_steps", "int64", "Maximum simulation steps", true, 10000);
        defaultSchema->addEntry("simulation_time_limit", "double", "Simulation time limit (seconds)", true, 0.0);
        defaultSchema->addEntry("visualization_enabled", "bool", "Enable visualization", true, false);
        defaultSchema->addEntry("visualization_update_rate", "double", "Visualization update rate (Hz)", true, 30.0);
        
        // Add custom validators
        defaultSchema->addValidator("random_seed", [](const ConfigValue& value) {
            if (!std::holds_alternative<int64_t>(value)) {
                return ValidationResult(false, "random_seed must be int64", "type");
            }
            int64_t seed = std::get<int64_t>(value);
            if (seed < 0) {
                return ValidationResult(false, "random_seed must be non-negative", "range");
            }
            return ValidationResult();
        });
        
        defaultSchema->addValidator("neuron_count", [](const ConfigValue& value) {
            if (!std::holds_alternative<int64_t>(value)) {
                return ValidationResult(false, "neuron_count must be int64", "type");
            }
            int64_t count = std::get<int64_t>(value);
            if (count < 1) {
                return ValidationResult(false, "neuron_count must be at least 1", "range");
            }
            return ValidationResult();
        });
        
        defaultSchema->addValidator("connection_probability", [](const ConfigValue& value) {
            if (!std::holds_alternative<double>(value)) {
                return ValidationResult(false, "connection_probability must be double", "type");
            }
            double prob = std::get<double>(value);
            if (prob < 0.0 || prob > 1.0) {
                return ValidationResult(false, "connection_probability must be between 0.0 and 1.0", "range");
            }
            return ValidationResult();
        });
    }
    
    pImpl->schema = defaultSchema;
}

std::shared_ptr<ConfigSchema> Config::getDefaultSchema() {
    if (!defaultSchema) {
        Config tempConfig;
        return tempConfig.getSchema();
    }
    return defaultSchema;
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

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate against schema if present
    if (pImpl->schema) {
        ValidationResult validation = pImpl->schema->validate(key, value);
        if (!validation.isValid) {
            pImpl->validationResults.push_back(validation);
            return false;
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        ConfigEntry entry;
        entry.key = key;
        entry.value = value;
        entry.source = source;
        entry.description = "";
        pImpl->entries.push_back(entry);
    }
    
    return true;
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
    return oss.str();
}

std::vector<ValidationResult> Config::validate() const {
    if (!pImpl->schema) {
        return {};
    }
    
    std::map<std::string, ConfigValue> configMap;
    for (const auto& entry : pImpl->entries) {
        configMap[entry.key] = entry.value;
    }
    
    pImpl->validationResults = pImpl->schema->validateAll(configMap);
    return pImpl->validationResults;
}

bool Config::isValid() const {
    return pImpl->validationResults.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    for (const auto& result : pImpl->validationResults) {
        if (!result.isValid) {
            errors.push_back("Key: " + result.path + ", Error: " + result.errorMessage);
        }
    }
    return errors;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            it->value = entry.value;
            it->source = entry.source;
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

void Config::overrideWithArgs(int argc, char** argv) {
    // Parse command line arguments and override configuration
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
}

// Explicit template instantiations
// These need to match the template declarations in the header
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

// Global default schema instance
std::shared_ptr<ConfigSchema> Config::defaultSchema = nullptr;

} // namespace nlm