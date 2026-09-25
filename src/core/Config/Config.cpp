#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cctype>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigSchema> schema;
    std::string version;
    bool validateOnLoad;
    bool saveDefaults;
};

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

// Conversion functions
nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    nlohmann::json jsonValue;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = arg;
        }
    }, value);
    return jsonValue;
}

std::optional<ConfigValue> Config::jsonToConfigValue(const nlohmann::json& jsonValue, ConfigType expectedType) {
    try {
        switch (expectedType) {
            case ConfigType::Integer:
                if (jsonValue.is_number_integer()) {
                    return jsonValue.get<int>();
                } else if (jsonValue.is_number()) {
                    return jsonValue.get<int>();
                }
                return std::nullopt;
                
            case ConfigType::Int64:
                if (jsonValue.is_number_integer()) {
                    return jsonValue.get<int64_t>();
                } else if (jsonValue.is_number()) {
                    return jsonValue.get<int64_t>();
                }
                return std::nullopt;
                
            case ConfigType::Double:
                if (jsonValue.is_number()) {
                    return jsonValue.get<double>();
                }
                return std::nullopt;
                
            case ConfigType::Boolean:
                if (jsonValue.is_boolean()) {
                    return jsonValue.get<bool>();
                }
                return std::nullopt;
                
            case ConfigType::String:
                if (jsonValue.is_string()) {
                    return jsonValue.get<std::string>();
                }
                return std::nullopt;
                
            case ConfigType::IntArray:
                if (jsonValue.is_array()) {
                    std::vector<int> result;
                    for (const auto& item : jsonValue) {
                        if (item.is_number_integer()) {
                            result.push_back(item.get<int>());
                        } else if (item.is_number()) {
                            result.push_back(static_cast<int>(item.get<double>()));
                        }
                    }
                    return result;
                }
                return std::nullopt;
                
            case ConfigType::DoubleArray:
                if (jsonValue.is_array()) {
                    std::vector<double> result;
                    for (const auto& item : jsonValue) {
                        if (item.is_number()) {
                            result.push_back(item.get<double>());
                        }
                    }
                    return result;
                }
                return std::nullopt;
                
            case ConfigType::StringArray:
                if (jsonValue.is_array()) {
                    std::vector<std::string> result;
                    for (const auto& item : jsonValue) {
                        if (item.is_string()) {
                            result.push_back(item.get<std::string>());
                        }
                    }
                    return result;
                }
                return std::nullopt;
        }
    } catch (const std::exception&) {
        return std::nullopt;
    }
    return std::nullopt;
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::validateConfigValue(const ConfigValue& value, ConfigType expectedType, 
                                 const std::string& key, const ConfigSchema* schema) {
    bool typeValid = false;
    switch (expectedType) {
        case ConfigType::Integer:
            typeValid = std::holds_alternative<int>(value);
            break;
        case ConfigType::Int64:
            typeValid = std::holds_alternative<int64_t>(value);
            break;
        case ConfigType::Double:
            typeValid = std::holds_alternative<double>(value);
            break;
        case ConfigType::Boolean:
            typeValid = std::holds_alternative<bool>(value);
            break;
        case ConfigType::String:
            typeValid = std::holds_alternative<std::string>(value);
            break;
        case ConfigType::IntArray:
            typeValid = std::holds_alternative<std::vector<int>>(value);
            break;
        case ConfigType::DoubleArray:
            typeValid = std::holds_alternative<std::vector<double>>(value);
            break;
        case ConfigType::StringArray:
            typeValid = std::holds_alternative<std::vector<std::string>>(value);
            break;
    }
    
    if (!typeValid) {
        return false;
    }
    
    if (schema) {
        const auto* entry = schema->getEntry(key);
        if (entry && entry->validationRule && !entry->validationRule(value)) {
            return false;
        }
    }
    
    return true;
}

void Config::applySchemaDefaults(std::vector<ConfigEntry>& entries, const ConfigSchema& schema) {
    for (const auto& schemaEntry : schema.entries) {
        auto it = std::find_if(entries.begin(), entries.end(),
                               [&schemaEntry](const ConfigEntry& e) { return e.key == schemaEntry.key; });
        
        if (it == entries.end()) {
            if (schemaEntry.defaultValue.has_value()) {
                ConfigSource source = ConfigSource::Default;
                entries.emplace_back(schemaEntry.key, schemaEntry.defaultValue.value(), source, schemaEntry.description);
            } else if (schemaEntry.required) {
                throw ConfigException("Required configuration key not found: " + schemaEntry.key, 
                                   ConfigException::ErrorType::MissingRequired);
            }
        }
    }
}

std::string Config::errorToString(const std::exception& e) {
    return e.what();
}

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->schema = std::make_unique<ConfigSchema>("1.0");
    pImpl->validateOnLoad = true;
    pImpl->saveDefaults = true;
    
    // Initialize with default schema
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchema() {
    // Add default configuration schema
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "random_seed", ConfigType::Int64, 42,
        "Random seed for reproducible experiments",
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v); }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "simulation_timestep", ConfigType::Double, 0.001,
        "Simulation timestep in seconds",
        [](const ConfigValue& v) { 
            if (const double* val = std::get_if<double>(&v)) return *val > 0.0;
            return false; 
        }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "neuron_count", ConfigType::Int64, 1000,
        "Number of neurons in the brain",
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "region_count", ConfigType::Int64, 2,
        "Number of brain regions",
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "connection_probability", ConfigType::Double, 0.1,
        "Connection probability for random connectivity",
        [](const ConfigValue& v) { 
            if (const double* val = std::get_if<double>(&v)) return *val >= 0.0 && *val <= 1.0;
            return false; 
        }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "plasticity_learning_rate", ConfigType::Double, 0.01,
        "Plasticity learning rate",
        [](const ConfigValue& v) { 
            if (const double* val = std::get_if<double>(&v)) return *val >= 0.0 && *val <= 1.0;
            return false; 
        }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "stdp_ltp_weight", ConfigType::Double, 0.01,
        "STDP LTP weight",
        [](const ConfigValue& v) { 
            if (const double* val = std::get_if<double>(&v)) return *val >= 0.0;
            return false; 
        }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "stdp_ltd_weight", ConfigType::Double, 0.012,
        "STDP LTD weight",
        [](const ConfigValue& v) { 
            if (const double* val = std::get_if<double>(&v)) return *val >= 0.0;
            return false; 
        }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "stdp_time_constant", ConfigType::Double, 20.0,
        "STDP time constant",
        [](const ConfigValue& v) { 
            if (const double* val = std::get_if<double>(&v)) return *val > 0.0;
            return false; 
        }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "development_synaptogenesis_rate", ConfigType::Double, 0.001,
        "Development synaptogenesis rate",
        [](const ConfigValue& v) { 
            if (const double* val = std::get_if<double>(&v)) return *val >= 0.0 && *val <= 1.0;
            return false; 
        }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "development_pruning_rate", ConfigType::Double, 0.0001,
        "Development pruning rate",
        [](const ConfigValue& v) { 
            if (const double* val = std::get_if<double>(&v)) return *val >= 0.0 && *val <= 1.0;
            return false; 
        }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "dopamine_baseline", ConfigType::Double, 0.1,
        "Dopamine baseline value",
        [](const ConfigValue& v) { 
            if (const double* val = std::get_if<double>(&v)) return *val >= 0.0 && *val <= 1.0;
            return false; 
        }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "reward_discount_factor", ConfigType::Double, 0.99,
        "Reward discount factor",
        [](const ConfigValue& v) { 
            if (const double* val = std::get_if<double>(&v)) return *val >= 0.0 && *val <= 1.0;
            return false; 
        }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "environment_name", ConfigType::String, std::string("GridWorld"),
        "Name of the environment"
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "environment_width", ConfigType::Int64, 10,
        "Environment width in cells",
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "environment_height", ConfigType::Int64, 10,
        "Environment height in cells",
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "log_level", ConfigType::String, std::string("INFO"),
        "Logging level (DEBUG, INFO, WARN, ERROR, CRITICAL)"
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "log_to_file", ConfigType::Boolean, false,
        "Whether to log to file"
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "log_filename", ConfigType::String, std::string("nlm.log"),
        "Log filename when log_to_file is true"
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "max_simulation_steps", ConfigType::Int64, 10000,
        "Maximum number of simulation steps",
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "simulation_time_limit", ConfigType::Double, 0.0,
        "Simulation time limit (0 = no limit)",
        [](const ConfigValue& v) { 
            if (const double* val = std::get_if<double>(&v)) return *val >= 0.0;
            return false; 
        }
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "visualization_enabled", ConfigType::Boolean, false,
        "Whether visualization is enabled"
    ));
    
    pImpl->schema->addEntry(ConfigSchemaEntry(
        "visualization_update_rate", ConfigType::Double, 30.0,
        "Visualization update rate in Hz",
        [](const ConfigValue& v) { 
            if (const double* val = std::get_if<double>(&v)) return *val > 0.0;
            return false; 
        }
    ));
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigException("Cannot open configuration file: " + filepath, 
                               ConfigException::ErrorType::FileError);
        }
        
        // Try to parse as JSON first
        nlohmann::json jsonData;
        try {
            file >> jsonData;
            // Successfully parsed as JSON
            clear();
            return parseJson(jsonData, ConfigSource::File);
        } catch (const nlohmann::json::parse_error&) {
            // Try to parse as YAML (basic YAML support)
            file.clear();
            file.seekg(0, std::ios::beg);
            
            std::string line;
            std::string currentSection;
            std::unordered_map<std::string, std::string> keyValues;
            
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                
                if (line.find(':') != std::string::npos) {
                    size_t colonPos = line.find(':');
                    std::string key = trim(line.substr(0, colonPos));
                    std::string value = trim(line.substr(colonPos + 1));
                    
                    // Remove quotes
                    if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                             (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    keyValues[key] = value;
                }
            }
            
            // Convert keyValues to json-like structure
            nlohmann::json yamlJson = nlohmann::json::object();
            for (const auto& pair : keyValues) {
                // Try to infer type
                nlohmann::json value;
                if (pair.second.empty()) {
                    value = pair.second;
                } else if (pair.second == "true" || pair.second == "false") {
                    value = (pair.second == "true");
                } else {
                    // Try to parse as number
                    try {
                        if (pair.second.find('.') != std::string::npos) {
                            value = std::stod(pair.second);
                        } else {
                            value = std::stoll(pair.second);
                        }
                    } catch (...) {
                        value = pair.second;
                    }
                }
                yamlJson[pair.first] = value;
            }
            
            clear();
            return parseJson(yamlJson, ConfigSource::File);
        }
    } catch (const ConfigException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigException("Error loading configuration file " + filepath + ": " + std::string(e.what()),
                           ConfigException::ErrorType::ParseError);
    }
    
    return false;
}

bool Config::parseJson(const nlohmann::json& jsonData, ConfigSource source) {
    clear();
    
    try {
        // Convert JSON to ConfigEntry objects
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            const std::string& key = it.key();
            const nlohmann::json& jsonValue = it.value();
            
            // Determine expected type from schema
            ConfigType expectedType = ConfigType::String;
            const auto* schemaEntry = pImpl->schema->getEntry(key);
            if (schemaEntry) {
                expectedType = schemaEntry->type;
            }
            
            // Try to convert JSON to ConfigValue
            auto configValueOpt = jsonToConfigValue(jsonValue, expectedType);
            if (!configValueOpt.has_value()) {
                // Try to infer type if conversion failed
                if (jsonValue.is_string()) {
                    configValueOpt = jsonValue.get<std::string>();
                } else if (jsonValue.is_number_integer()) {
                    configValueOpt = jsonValue.get<int64_t>();
                } else if (jsonValue.is_number()) {
                    configValueOpt = jsonValue.get<double>();
                } else if (jsonValue.is_boolean()) {
                    configValueOpt = jsonValue.get<bool>();
                } else if (jsonValue.is_array()) {
                    // Try to determine array type
                    if (!jsonValue.empty()) {
                        if (jsonValue[0].is_number_integer()) {
                            std::vector<int> intArray;
                            for (const auto& item : jsonValue) {
                                if (item.is_number_integer()) {
                                    intArray.push_back(item.get<int>());
                                }
                            }
                            configValueOpt = intArray;
                        } else if (jsonValue[0].is_number()) {
                            std::vector<double> doubleArray;
                            for (const auto& item : jsonValue) {
                                if (item.is_number()) {
                                    doubleArray.push_back(item.get<double>());
                                }
                            }
                            configValueOpt = doubleArray;
                        } else if (jsonValue[0].is_string()) {
                            std::vector<std::string> stringArray;
                            for (const auto& item : jsonValue) {
                                if (item.is_string()) {
                                    stringArray.push_back(item.get<std::string>());
                                }
                            }
                            configValueOpt = stringArray;
                        }
                    }
                }
            }
            
            if (configValueOpt.has_value()) {
                // Validate the value
                if (pImpl->validateOnLoad) {
                    if (!validateConfigValue(configValueOpt.value(), expectedType, key, pImpl->schema.get())) {
                        throw ConfigException("Invalid configuration value for key '" + key + "'",
                                           ConfigException::ErrorType::ValidationError);
                    }
                }
                
                // Get description from schema
                std::string description = "";
                if (schemaEntry) {
                    description = schemaEntry->description;
                }
                
                // Add entry
                pImpl->entries.emplace_back(key, configValueOpt.value(), source, description);
            }
        }
        
        // Apply schema defaults if needed
        if (pImpl->validateOnLoad && source != ConfigSource::Default) {
            applySchemaDefaults(pImpl->entries, *pImpl->schema);
        }
        
        return true;
    } catch (const ConfigException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigException("Error parsing configuration data: " + std::string(e.what()),
                           ConfigException::ErrorType::ParseError);
    }
    
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    // Apply type conversion based on schema
                    ConfigType expectedType = ConfigType::String;
                    const auto* schemaEntry = pImpl->schema->getEntry(key);
                    if (schemaEntry) {
                        expectedType = schemaEntry->type;
                    }
                    
                    setFromString(key, value, expectedType, ConfigSource::CommandLine);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                ConfigType expectedType = ConfigType::String;
                const auto* schemaEntry = pImpl->schema->getEntry(key);
                if (schemaEntry) {
                    expectedType = schemaEntry->type;
                }
                
                setFromString(key, value, expectedType, ConfigSource::CommandLine);
            }
        }
        
        return true;
    } catch (const ConfigException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigException("Error loading configuration from command line: " + std::string(e.what()),
                           ConfigException::ErrorType::ParseError);
    }
    
    return false;
}

void Config::setFromString(const std::string& key, const std::string& valueStr, 
                          ConfigType expectedType, ConfigSource source) {
    ConfigValue configValue;
    
    try {
        switch (expectedType) {
            case ConfigType::Integer:
                try {
                    configValue = std::stoll(valueStr);
                } catch (...) {
                    configValue = 0;
                }
                break;
                
            case ConfigType::Int64:
                try {
                    configValue = std::stoll(valueStr);
                } catch (...) {
                    configValue = 0;
                }
                break;
                
            case ConfigType::Double:
                try {
                    configValue = std::stod(valueStr);
                } catch (...) {
                    configValue = 0.0;
                }
                break;
                
            case ConfigType::Boolean:
                configValue = (toLower(valueStr) == "true" || valueStr == "1");
                break;
                
            case ConfigType::String:
                configValue = valueStr;
                break;
                
            case ConfigType::IntArray: {
                std::vector<int> intArray;
                // Parse comma-separated values
                std::stringstream ss(valueStr);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    try {
                        intArray.push_back(std::stoi(trim(item)));
                    } catch (...) {
                        intArray.push_back(0);
                    }
                }
                configValue = intArray;
                break;
            }
                
            case ConfigType::DoubleArray: {
                std::vector<double> doubleArray;
                std::stringstream ss(valueStr);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    try {
                        doubleArray.push_back(std::stod(trim(item)));
                    } catch (...) {
                        doubleArray.push_back(0.0);
                    }
                }
                configValue = doubleArray;
                break;
            }
                
            case ConfigType::StringArray: {
                std::vector<std::string> stringArray;
                std::stringstream ss(valueStr);
                std::string item;
                while (std::getline(ss, item, ',')) {
                    stringArray.push_back(trim(item));
                }
                configValue = stringArray;
                break;
            }
        }
        
        // Validate
        if (!validateConfigValue(configValue, expectedType, key, pImpl->schema.get())) {
            throw ConfigException("Invalid configuration value for key '" + key + "' with type " + 
                               std::to_string(static_cast<int>(expectedType)),
                           ConfigException::ErrorType::ValidationError);
        }
        
        // Check dependencies
        const auto* schemaEntry = pImpl->schema->getEntry(key);
        if (schemaEntry) {
            for (const std::string& depKey : schemaEntry->dependencies) {
                if (!has(depKey)) {
                    throw ConfigException("Missing required dependency for key '" + key + "': '" + depKey + "'",
                                       ConfigException::ErrorType::MissingRequired);
                }
            }
        }
        
        // Get description
        std::string description = "";
        if (schemaEntry) {
            description = schemaEntry->description;
        }
        
        // Set the value
        set(key, configValue, source, description);
        
    } catch (const ConfigException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigException("Error setting configuration key '" + key + "': " + std::string(e.what()),
                           ConfigException::ErrorType::ParseError);
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigException("Cannot open configuration file for writing: " + filepath,
                               ConfigException::ErrorType::FileError);
        }
        
        // Write in JSON format for better compatibility
        nlohmann::json jsonData = nlohmann::json::object();
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = configValueToJson(entry.value);
        }
        
        file << jsonData.dump(2) << std::endl;
        return true;
    } catch (const ConfigException& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigException("Error saving configuration file " + filepath + ": " + std::string(e.what()),
                           ConfigException::ErrorType::ParseError);
    }
    
    return false;
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
                 const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) {
            it->description = description;
        }
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source, "");
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source, "");
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source, "");
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source, "");
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source, "");
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source, "");
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source, "");
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source, "");
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
    oss << "Configuration (v" << pImpl->version << ") (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << "\"" << item << "\"";
                    first = false;
                }
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    return oss.str();
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

// Schema class implementations (inline for simplicity)
void ConfigSchema::addEntry(const ConfigSchemaEntry& entry) {
    entries.push_back(entry);
    keyToEntry[entry.key] = &entries.back();
}

const ConfigSchemaEntry* ConfigSchema::getEntry(const std::string& key) const {
    auto it = keyToEntry.find(key);
    return it != keyToEntry.end() ? it->second : nullptr;
}

bool ConfigSchema::hasEntry(const std::string& key) const {
    return keyToEntry.find(key) != keyToEntry.end();
}

std::vector<std::string> ConfigSchema::getRequiredKeys() const {
    std::vector<std::string> required;
    for (const auto& entry : entries) {
        if (entry.required) {
            required.push_back(entry.key);
        }
    }
    return required;
}

std::string ConfigSchema::summary() const {
    std::ostringstream oss;
    oss << "Configuration Schema (v" << version << ") with " << entries.size() << " entries:\n";
    for (const auto& entry : entries) {
        oss << "  " << entry.key << ": ";
        switch (entry.type) {
            case ConfigType::Integer: oss << "int"; break;
            case ConfigType::Int64: oss << "int64"; break;
            case ConfigType::Double: oss << "double"; break;
            case ConfigType::Boolean: oss << "bool"; break;
            case ConfigType::String: oss << "string"; break;
            case ConfigType::IntArray: oss << "int[]"; break;
            case ConfigType::DoubleArray: oss << "double[]"; break;
            case ConfigType::StringArray: oss << "string[]"; break;
        }
        if (entry.required) oss << " [REQUIRED]";
        if (entry.defaultValue.has_value()) oss << " [default available]";
        oss << " - " << entry.description << "\n";
    }
    return oss.str();
}

// Config class method implementations
const ConfigSchema& Config::getSchema() const {
    return *pImpl->schema;
}

void Config::setValidateOnLoad(bool validate) {
    pImpl->validateOnLoad = validate;
}

void Config::setSaveDefaults(bool save) {
    pImpl->saveDefaults = save;
}

void Config::setVersion(const std::string& version) {
    pImpl->version = version;
}

} // namespace nlm
