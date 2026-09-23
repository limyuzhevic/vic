#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <json/json.h>  // Modern C++ JSON library
#include <iostream>
#include <ctime>
#include <cmath>
#include <unordered_map>
#include <vector>

namespace nlm {

// Singleton instance for ConfigRegistry
ConfigRegistry& ConfigRegistry::getInstance() {
    static ConfigRegistry instance;
    return instance;
}

void ConfigRegistry::registerConfig(const std::string& key, const ConfigSchema& schema) {
    schemas[key] = schema;
}

bool ConfigRegistry::validate(const std::string& key, const ConfigValue& value) const {
    auto it = schemas.find(key);
    if (it == schemas.end()) {
        return true;  // No schema defined, skip validation
    }
    
    const ConfigSchema& schema = it->second;
    
    // Type validation
    bool typeMatches = false;
    if (schema.type == "int" && std::holds_alternative<int>(value)) typeMatches = true;
    else if (schema.type == "double" && std::holds_alternative<double>(value)) typeMatches = true;
    else if (schema.type == "bool" && std::holds_alternative<bool>(value)) typeMatches = true;
    else if (schema.type == "string" && std::holds_alternative<std::string>(value)) typeMatches = true;
    else if (schema.type == "array_int" && std::holds_alternative<std::vector<int>>(value)) typeMatches = true;
    else if (schema.type == "array_double" && std::holds_alternative<std::vector<double>>(value)) typeMatches = true;
    else if (schema.type == "array_string" && std::holds_alternative<std::vector<std::string>>(value)) typeMatches = true;
    
    if (!typeMatches) return false;
    
    // Range validation
    if (schema.type == "int" && std::holds_alternative<int>(value)) {
        int val = std::get<int>(value);
        if (val < schema.minValue || val > schema.maxValue) return false;
    }
    else if (schema.type == "double" && std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (val < schema.minValue || val > schema.maxValue) return false;
    }
    
    // Allowed values validation
    if (!schema.allowedValues.empty()) {
        if (schema.type == "string" && std::holds_alternative<std::string>(value)) {
            auto& val = std::get<std::string>(value);
            if (std::find(schema.allowedValues.begin(), schema.allowedValues.end(), val) == schema.allowedValues.end()) {
                return false;
            }
        }
        else if (schema.type == "int" && std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            std::string valStr = std::to_string(val);
            if (std::find(schema.allowedValues.begin(), schema.allowedValues.end(), valStr) == schema.allowedValues.end()) {
                return false;
            }
        }
    }
    
    return true;
}

bool ConfigRegistry::validateFull(const std::string& key, const ConfigValue& value) const {
    auto it = schemas.find(key);
    if (it == schemas.end()) {
        return true;  // No schema defined, skip validation
    }
    
    return it->second.validate(value);
}

const ConfigSchema* ConfigRegistry::getSchema(const std::string& key) const {
    auto it = schemas.find(key);
    if (it != schemas.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<std::string> ConfigRegistry::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(schemas.size());
    for (const auto& pair : schemas) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::string ConfigRegistry::getSchemaDescription() const {
    std::ostringstream oss;
    oss << "Configuration Schema (" << schemas.size() << " entries):\n";
    for (const auto& pair : schemas) {
        const ConfigSchema& schema = pair.second;
        oss << "  " << pair.first << " (" << schema.type << ")";
        if (schema.required) oss << " [REQUIRED]";
        oss << ": " << schema.description;
        if (!schema.defaultValue.empty()) oss << " [Default: " << schema.defaultValue << "]";
        oss << "\n";
    }
    return oss.str();
}

std::string ConfigRegistry::getValidationReport(const std::map<std::string, ConfigValue>& values) const {
    std::ostringstream oss;
    oss << "=== Configuration Validation Report ===\n";
    oss << "Total entries validated: " << values.size() << "\n";
    oss << "Schema entries defined: " << schemas.size() << "\n\n";
    
    int validCount = 0;
    int invalidCount = 0;
    
    for (const auto& pair : values) {
        const std::string& key = pair.first;
        const ConfigValue& value = pair.second;
        
        bool isValid = validateFull(key, value);
        if (isValid) {
            validCount++;
            oss << "✓ " << key << " = [";
        } else {
            invalidCount++;
            oss << "✗ " << key << " = [";
        }
        
        // Display value
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, value);
        
        oss << "]";
        
        if (isValid) {
            oss << " (valid)\n";
        } else {
            const ConfigSchema* schema = getSchema(key);
            if (schema) {
                oss << " (invalid: " << schema->validationError << ")\n";
            } else {
                oss << " (invalid: no schema defined)\n";
            }
        }
    }
    
    oss << "\n=== Summary ===\n";
    oss << "Valid: " << validCount << "\n";
    oss << "Invalid: " << invalidCount << "\n";
    oss << "Success Rate: " << (values.size() > 0 ? (validCount * 100.0 / values.size()) : 0) << "%\n";
    oss << "===================\n";
    
    return oss.str();
}

std::vector<std::string> ConfigRegistry::getValidationErrors(const std::map<std::string, ConfigValue>& values) const {
    std::vector<std::string> errors;
    
    for (const auto& pair : values) {
        const std::string& key = pair.first;
        const ConfigValue& value = pair.second;
        
        if (!validateFull(key, value)) {
            const ConfigSchema* schema = getSchema(key);
            if (schema) {
                errors.push_back(key + ": " + schema->validationError);
            } else {
                errors.push_back(key + ": Invalid (no schema defined)");
            }
        }
    }
    
    return errors;
}

bool ConfigRegistry::hasSchema(const std::string& key) const {
    return schemas.find(key) != schemas.end();
}

ConfigRegistry& ConfigRegistry::getInstance() {
    static ConfigRegistry instance;
    return instance;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    mutable ConfigStats stats;
    int configVersion;
    
    Impl() : configVersion(1) {
        stats.lastModified = std::time(nullptr);
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize default configuration schema
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchema() {
    ConfigRegistry& registry = ConfigRegistry::getInstance();
    
    // Core simulation parameters
    registry.registerConfig("random_seed", ConfigSchema{
        "random_seed", "int", "42", 
        "Random seed for reproducible experiments", true, "",
        0, 2147483647, {"42"}
    });
    
    registry.registerConfig("simulation_timestep", ConfigSchema{
        "simulation_timestep", "double", "0.001",
        "Simulation timestep in seconds", true, "",
        0.0001, 1.0
    });
    
    registry.registerConfig("neuron_count", ConfigSchema{
        "neuron_count", "int", "1000",
        "Number of neurons in the brain", true, "",
        1, 100000
    });
    
    registry.registerConfig("region_count", ConfigSchema{
        "region_count", "int", "2",
        "Number of brain regions", true, "",
        1, 100
    });
    
    registry.registerConfig("connection_probability", ConfigSchema{
        "connection_probability", "double", "0.1",
        "Connection probability for random connectivity", true, "",
        0.0, 1.0
    });
    
    // Plasticity settings
    registry.registerConfig("plasticity_learning_rate", ConfigSchema{
        "plasticity_learning_rate", "double", "0.01",
        "Base learning rate for plasticity", false, "",
        0.0, 1.0
    });
    
    registry.registerConfig("stdp_ltp_weight", ConfigSchema{
        "stdp_ltp_weight", "double", "0.01",
        "STDP Long-Term Potentiation weight", false, "",
        0.0, 1.0
    });
    
    registry.registerConfig("stdp_ltd_weight", ConfigSchema{
        "stdp_ltd_weight", "double", "0.012",
        "STDP Long-Term Depression weight", false, "",
        0.0, 1.0
    });
    
    registry.registerConfig("stdp_time_constant", ConfigSchema{
        "stdp_time_constant", "double", "20.0",
        "STDP time constant in milliseconds", false, "",
        1.0, 1000.0
    });
    
    // Development settings
    registry.registerConfig("development_synaptogenesis_rate", ConfigSchema{
        "development_synaptogenesis_rate", "double", "0.001",
        "Development synaptogenesis rate", false, "",
        0.0, 1.0
    });
    
    registry.registerConfig("development_pruning_rate", ConfigSchema{
        "development_pruning_rate", "double", "0.0001",
        "Development pruning rate", false, "",
        0.0, 1.0
    });
    
    // Neuromodulation settings
    registry.registerConfig("dopamine_baseline", ConfigSchema{
        "dopamine_baseline", "double", "0.1",
        "Dopamine baseline level", false, "",
        0.0, 10.0
    });
    
    // Reward settings
    registry.registerConfig("reward_discount_factor", ConfigSchema{
        "reward_discount_factor", "double", "0.99",
        "Reward discount factor for temporal difference learning", false, "",
        0.0, 1.0
    });
    
    // Environment settings
    registry.registerConfig("environment_name", ConfigSchema{
        "environment_name", "string", "GridWorld",
        "Name of the environment", false, "",
        -std::numeric_limits<double>::max(), std::numeric_limits<double>::max()
    });
    
    registry.registerConfig("environment_width", ConfigSchema{
        "environment_width", "int", "10",
        "Environment width in cells", false, "",
        1, 10000
    });
    
    registry.registerConfig("environment_height", ConfigSchema{
        "environment_height", "int", "10",
        "Environment height in cells", false, "",
        1, 10000
    });
    
    registry.registerConfig("vision_width", ConfigSchema{
        "vision_width", "int", "16",
        "Vision sensor width", false, "",
        1, 10000
    });
    
    registry.registerConfig("vision_height", ConfigSchema{
        "vision_height", "int", "16",
        "Vision sensor height", false, "",
        1, 10000
    });
    
    // Logging settings
    registry.registerConfig("log_level", ConfigSchema{
        "log_level", "string", "INFO",
        "Log level (DEBUG, INFO, WARNING, ERROR)", false, "",
        -std::numeric_limits<double>::max(), std::numeric_limits<double>::max()
    });
    
    registry.registerConfig("log_to_file", ConfigSchema{
        "log_to_file", "bool", "false",
        "Whether to log to file in addition to console", false, "",
        -std::numeric_limits<double>::max(), std::numeric_limits<double>::max()
    });
    
    registry.registerConfig("log_filename", ConfigSchema{
        "log_filename", "string", "nlm.log",
        "Log file name", false, "",
        -std::numeric_limits<double>::max(), std::numeric_limits<double>::max()
    });
    
    // Simulation settings
    registry.registerConfig("max_simulation_steps", ConfigSchema{
        "max_simulation_steps", "int", "10000",
        "Maximum number of simulation steps", false, "",
        1, 1000000
    });
    
    registry.registerConfig("simulation_time_limit", ConfigSchema{
        "simulation_time_limit", "double", "0.0",
        "Simulation time limit in seconds (0 = no limit)", false, "",
        0.0, 1000000.0
    });
    
    // Visualization settings
    registry.registerConfig("visualization_enabled", ConfigSchema{
        "visualization_enabled", "bool", "false",
        "Whether to enable visualization", false, "",
        -std::numeric_limits<double>::max(), std::numeric_limits<double>::max()
    });
    
    registry.registerConfig("visualization_update_rate", ConfigSchema{
        "visualization_update_rate", "double", "30.0",
        "Visualization update rate in Hz", false, "",
        1.0, 1000.0
    });
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON format first
    std::ifstream jsonFile(filepath + ".json");
    if (jsonFile.good()) {
        return loadFromJSON(jsonFile);
    }
    
    // Try YAML format
    std::ifstream yamlFile(filepath + ".yaml");
    if (yamlFile.good()) {
        return loadFromYAML(yamlFile);
    }
    
    // Try original .cfg format
    std::ifstream cfgFile(filepath);
    if (!cfgFile.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    std::string line;
    while (std::getline(cfgFile, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    pImpl->stats.lastModified = std::time(nullptr);
    pImpl->stats.fileEntries = static_cast<size_t>(getKeys().size());
    return true;
}

bool Config::loadFromJSON(std::istream& stream) {
    try {
        Json::CharReaderBuilder readerBuilder;
        Json::CharReader* reader = readerBuilder.newCharReader();
        
        std::string jsonContent((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        Json::Value root;
        std::string errors;
        
        bool parsingSuccessful = reader->parse(jsonContent.c_str(), jsonContent.c_str() + jsonContent.size(),
                                             &root, &errors);
        delete reader;
        
        if (!parsingSuccessful) {
            NLM_LOG_ERROR("JSON parsing error: " + errors);
            return false;
        }
        
        // Validate JSON structure
        if (!validateJSON(root)) {
            NLM_LOG_ERROR("JSON validation failed");
            return false;
        }
        
        // Convert JSON to config entries
        convertJSONToConfig(root);
        
        pImpl->stats.lastModified = std::time(nullptr);
        pImpl->stats.fileEntries = static_cast<size_t>(getKeys().size());
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading JSON config: ") + e.what());
        return false;
    }
}

bool Config::loadFromYAML(std::istream& stream) {
    // Placeholder for YAML support
    NLM_LOG_WARNING("YAML config loading not yet implemented, falling back to simple parser");
    
    // Fall back to original .cfg format
    std::string content((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    std::istringstream fallbackStream(content);
    return loadFromFile("fallback"); // This will trigger .cfg parsing
}

bool Config::validateJSON(const Json::Value& root) const {
    if (!root.isObject()) {
        return false;
    }
    
    ConfigRegistry& registry = ConfigRegistry::getInstance();
    
    for (const auto& key : root.getMemberNames()) {
        if (!registry.validate(key, root[key])) {
            NLM_LOG_WARNING("Config validation failed for key: " + key);
            return false;
        }
    }
    
    return true;
}

void Config::convertJSONToConfig(const Json::Value& root) {
    ConfigRegistry& registry = ConfigRegistry::getInstance();
    
    for (const auto& key : root.getMemberNames()) {
        const Json::Value& value = root[key];
        
        ConfigSource source = ConfigSource::File;
        std::string valueStr;
        
        if (value.isBool()) {
            set(key, value.asBool(), source);
        }
        else if (value.isInt()) {
            set(key, value.asInt(), source);
        }
        else if (value.isUInt()) {
            set(key, static_cast<int64_t>(value.asUInt64()), source);
        }
        else if (value.isDouble()) {
            set(key, value.asDouble(), source);
        }
        else if (value.isString()) {
            set(key, value.asString(), source);
        }
        else if (value.isArray()) {
            // Handle arrays
            std::vector<int> intArray;
            std::vector<double> doubleArray;
            std::vector<std::string> stringArray;
            
            bool isInt = true, isDouble = true, isString = true;
            
            for (const auto& item : value) {
                if (item.isInt()) intArray.push_back(item.asInt());
                else if (item.isDouble()) {
                    doubleArray.push_back(item.asDouble());
                    if (item.asDouble() != static_cast<int>(item.asDouble())) isInt = false;
                }
                else if (item.isString()) stringArray.push_back(item.asString());
                else {
                    isInt = isDouble = isString = false;
                    break;
                }
            }
            
            if (isInt) set(key, intArray, source);
            else if (isDouble) set(key, doubleArray, source);
            else if (isString) set(key, stringArray, source);
            else {
                NLM_LOG_WARNING("Unsupported array type for key: " + key);
            }
        }
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    
    pImpl->stats.commandLineEntries = static_cast<size_t>(getKeys().size());
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath + ".json");
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create config file: " + filepath);
        return false;
    }
    
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
    
    Json::Value root;
    
    for (const auto& entry : pImpl->entries) {
        setJSONValue(root[entry.key], entry.value);
    }
    
    writer->write(root, &file);
    file.close();
    
    return true;
}

void Config::setJSONValue(Json::Value& jsonValue, const ConfigValue& configValue) {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        }
        else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = static_cast<Json::Int64>(arg);
        }
        else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        }
        else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        }
        else if constexpr (std::is_same_v<T, std::vector<int>>) {
            Json::Value array(Json::arrayValue);
            for (const auto& item : arg) {
                array.append(item);
            }
            jsonValue = array;
        }
        else if constexpr (std::is_same_v<T, std::vector<double>>) {
            Json::Value array(Json::arrayValue);
            for (const auto& item : arg) {
                array.append(item);
            }
            jsonValue = array;
        }
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            Json::Value array(Json::arrayValue);
            for (const auto& item : arg) {
                array.append(item);
            }
            jsonValue = array;
        }
    }, configValue);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        // Validate the entry before returning
        ConfigRegistry& registry = ConfigRegistry::getInstance();
        if (!registry.validate(key, it->value)) {
            NLM_LOG_WARNING("Config validation failed for key: " + key);
        }
        
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Type mismatch for config key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate before setting
    ConfigRegistry& registry = ConfigRegistry::getInstance();
    if (!registry.validate(key, value)) {
        NLM_LOG_ERROR("Config validation failed for key: " + key);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    pImpl->stats.lastModified = std::time(nullptr);
    pImpl->stats.totalEntries = pImpl->entries.size();
    
    switch (source) {
        case ConfigSource::File:
            pImpl->stats.fileEntries = pImpl->entries.size();
            break;
        case ConfigSource::CommandLine:
            pImpl->stats.commandLineEntries = pImpl->entries.size();
            break;
        case ConfigSource::Runtime:
            pImpl->stats.runtimeEntries = pImpl->entries.size();
            break;
        default:
            break;
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
        std::remove_if(pImpl->entries.begin(),
                      pImpl->entries.end(),
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
    pImpl->stats.totalEntries = 0;
    pImpl->stats.fileEntries = 0;
    pImpl->stats.commandLineEntries = 0;
    pImpl->stats.runtimeEntries = 0;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Statistics (Version " << pImpl->configVersion << "):\n";
    oss << "  Total entries: " << pImpl->stats.totalEntries << "\n";
    oss << "  File entries: " << pImpl->stats.fileEntries << "\n";
    oss << "  Command line entries: " << pImpl->stats.commandLineEntries << "\n";
    oss << "  Runtime entries: " << pImpl->stats.runtimeEntries << "\n";
    oss << "  Last modified: " << pImpl->stats.lastModified << "\n";
    
    oss << "\nConfiguration Entries:";
    for (const auto& entry : pImpl->entries) {
        oss << "\n  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
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

template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm