#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema_(std::make_shared<ConfigSchema>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// File format detection methods
bool Config::isJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    try {
        nlohmann::json::parse(file);
        return true;
    } catch (const nlohmann::json::parse_error&) {
        file.clear();
        file.seekg(0);
    }
    
    return false;
}

bool Config::isYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as YAML
    try {
        YAML::Load(file);
        return true;
    } catch (const YAML::ParserException&) {
        file.clear();
        file.seekg(0);
    }
    
    return false;
}

bool Config::isSimpleConfigFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple config files typically have key=value pairs
    std::string line;
    bool foundKeyValuePair = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            foundKeyValuePair = true;
            // If we find a key=value pair and the file is small, assume it's simple config
            if (file.peek() == EOF) {
                return true;
            }
        } else {
            // Found a line that's not a comment and not a key=value pair
            // Could be JSON or YAML
            file.clear();
            file.seekg(0);
            return false;
        }
    }
    
    return foundKeyValuePair;
}

// Helper methods for nested keys
std::string Config::normalizeKey(const std::string& key) {
    // Normalize key by removing spaces and converting to lowercase for nested access
    std::string result = key;
    // Replace dots with nested structure markers
    std::replace(result.begin(), result.end(), '.', '_');
    // Trim spaces
    size_t start = 0;
    size_t end = result.size();
    while (start < end && std::isspace(result[start])) start++;
    while (end > start && std::isspace(result[end - 1])) end--;
    return result.substr(start, end - start);
}

std::string Config::denormalizeKey(const std::string& key) {
    // Convert normalized key back to original form
    std::string result = key;
    std::replace(result.begin(), result.end(), '_', '.');
    return result;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension or content to determine format
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    try {
        if (ext == ".json") {
            return loadFromJsonFile(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return loadFromYamlFile(filepath);
        } else {
            // Auto-detect format
            if (isJsonFile(filepath)) {
                return loadFromJsonFile(filepath);
            } else if (isYamlFile(filepath)) {
                return loadFromYamlFile(filepath);
            } else if (isSimpleConfigFile(filepath)) {
                return loadFromSimpleFile(filepath);
            } else {
                throw ConfigFileError("Unknown file format or unsupported configuration file: " + filepath);
            }
        }
    } catch (const ConfigFileError& e) {
        std::cerr << "Error loading config file " << filepath << ": " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error loading config file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError("Cannot open file: " + filepath);
    }
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(file);
        
        // Extract version if present
        int version = jsonData.value("version", CONFIG_VERSION);
        
        // Check if entries array exists
        if (jsonData.contains("config")) {
            const auto& configArray = jsonData["config"];
            if (configArray.is_array()) {
                for (const auto& entryJson : configArray) {
                    ConfigEntry entry;
                    from_json(entryJson, entry);
                    
                    // Validate against schema if present
                    if (schema_ && !schema_->validate(entry)) {
                        std::cerr << "Warning: Configuration entry " << entry.key << " failed validation" << std::endl;
                        continue; // Skip invalid entries
                    }
                    
                    pImpl->entries.push_back(entry);
                }
            }
        } else {
            // If no config array, treat the whole JSON as a single configuration object
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                if (it.key() == "version") continue; // Skip version field
                
                ConfigEntry entry;
                entry.key = it.key();
                entry.value = it.value().get<ConfigValue>();
                entry.source = ConfigSource::File;
                entry.description = "Loaded from JSON";
                
                if (schema_ && !schema_->validate(entry)) {
                    std::cerr << "Warning: Configuration entry " << entry.key << " failed validation" << std::endl;
                    continue;
                }
                
                pImpl->entries.push_back(entry);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigFileError("Invalid JSON in file " + filepath + ": " + std::string(e.what()));
    } catch (const nlohmann::json::type_error& e) {
        throw ConfigFileError("JSON type error in file " + filepath + ": " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw ConfigFileError("Error parsing JSON file " + filepath + ": " + std::string(e.what()));
    }
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError("Cannot open file: " + filepath);
    }
    
    try {
        YAML::Node yamlData = YAML::Load(file);
        
        // Extract version if present
        int version = yamlData["version"].as<int>(CONFIG_VERSION);
        
        // Check if config map exists
        if (yamlData["config"]) {
            const YAML::Node& configMap = yamlData["config"];
            for (const auto& entryNode : configMap) {
                ConfigEntry entry;
                entry.key = entryNode["key"].as<std::string>("");
                entry.description = entryNode["description"].as<std::string>("");
                entry.source = static_cast<ConfigSource>(entryNode["source"].as<int>(static_cast<int>(ConfigSource::File)));
                
                // Parse value based on its type
                if (entryNode["value_type"]) {
                    std::string type = entryNode["value_type"].as<std::string>();
                    if (type == "int") {
                        entry.value = entryNode["value"].as<int>();
                    } else if (type == "int64") {
                        entry.value = entryNode["value"].as<int64_t>();
                    } else if (type == "double") {
                        entry.value = entryNode["value"].as<double>();
                    } else if (type == "bool") {
                        entry.value = entryNode["value"].as<bool>();
                    } else if (type == "string") {
                        entry.value = entryNode["value"].as<std::string>();
                    } else if (type == "int_array") {
                        entry.value = entryNode["value"].as<std::vector<int>>();
                    } else if (type == "double_array") {
                        entry.value = entryNode["value"].as<std::vector<double>>();
                    } else if (type == "string_array") {
                        entry.value = entryNode["value"].as<std::vector<std::string>>();
                    }
                }
                
                // Validate against schema if present
                if (schema_ && !schema_->validate(entry)) {
                    std::cerr << "Warning: Configuration entry " << entry.key << " failed validation" << std::endl;
                    continue; // Skip invalid entries
                }
                
                pImpl->entries.push_back(entry);
            }
        } else {
            // If no config map, treat the whole YAML as a single configuration object
            for (const auto& entryNode : yamlData) {
                if (entryNode.first.as<std::string>() == "version") continue; // Skip version field
                
                ConfigEntry entry;
                entry.key = entryNode.first.as<std::string>();
                
                // Try to determine value type
                if (entryNode.second.IsScalar()) {
                    if (entryNode.second.IsInt()) {
                        entry.value = entryNode.second.as<int>();
                    } else if (entryNode.second.IsDouble()) {
                        entry.value = entryNode.second.as<double>();
                    } else if (entryNode.second.IsBoolean()) {
                        entry.value = entryNode.second.as<bool>();
                    } else {
                        entry.value = entryNode.second.as<std::string>();
                    }
                } else if (entryNode.second.IsSequence()) {
                    if (!entryNode.second.empty() && entryNode.second[0].IsInt()) {
                        entry.value = entryNode.second.as<std::vector<int>>();
                    } else if (!entryNode.second.empty() && entryNode.second[0].IsDouble()) {
                        entry.value = entryNode.second.as<std::vector<double>>();
                    } else {
                        entry.value = entryNode.second.as<std::vector<std::string>>();
                    }
                }
                
                entry.source = ConfigSource::File;
                entry.description = "Loaded from YAML";
                
                if (schema_ && !schema_->validate(entry)) {
                    std::cerr << "Warning: Configuration entry " << entry.key << " failed validation" << std::endl;
                    continue;
                }
                
                pImpl->entries.push_back(entry);
            }
        }
        
        return true;
    } catch (const YAML::ParserException& e) {
        throw ConfigFileError("Invalid YAML in file " + filepath + ": " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw ConfigFileError("Error parsing YAML file " + filepath + ": " + std::string(e.what()));
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    // Original simple key=value file loading logic
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError("Cannot open file: " + filepath);
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
            
            // Try to parse the value as different types
            bool parsed = false;
            try {
                // Try bool first
                size_t posEnd;
                bool boolVal = std::stoi(value);
                set(key, boolVal, ConfigSource::File);
                parsed = true;
            } catch (...) {
                try {
                    // Try int
                    int intVal = std::stoi(value);
                    set(key, intVal, ConfigSource::File);
                    parsed = true;
                } catch (...) {
                    try {
                        // Try double
                        double doubleVal = std::stod(value);
                        set(key, doubleVal, ConfigSource::File);
                        parsed = true;
                    } catch (...) {
                        // Keep as string
                        set(key, value, ConfigSource::File);
                        parsed = true;
                    }
                }
            }
            
            if (!parsed) {
                set(key, value, ConfigSource::File);
            }
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
    return saveToJsonFile(filepath);
}

bool Config::loadFromJson(const std::string& jsonString) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Check if entries array exists
        if (jsonData.contains("config")) {
            const auto& configArray = jsonData["config"];
            if (configArray.is_array()) {
                for (const auto& entryJson : configArray) {
                    ConfigEntry entry;
                    from_json(entryJson, entry);
                    
                    if (schema_ && !schema_->validate(entry)) {
                        std::cerr << "Warning: Configuration entry " << entry.key << " failed validation" << std::endl;
                        continue;
                    }
                    
                    pImpl->entries.push_back(entry);
                }
            }
        } else {
            // If no config array, treat the whole JSON as a single configuration object
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                if (it.key() == "version") continue; // Skip version field
                
                ConfigEntry entry;
                entry.key = it.key();
                entry.value = it.value().get<ConfigValue>();
                entry.source = ConfigSource::File;
                entry.description = "Loaded from JSON string";
                
                if (schema_ && !schema_->validate(entry)) {
                    std::cerr << "Warning: Configuration entry " << entry.key << " failed validation" << std::endl;
                    continue;
                }
                
                pImpl->entries.push_back(entry);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigFileError("Invalid JSON string: " + std::string(e.what()));
    } catch (const nlohmann::json::type_error& e) {
        throw ConfigFileError("JSON type error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw ConfigFileError("Error parsing JSON string: " + std::string(e.what()));
    }
}

bool Config::saveToJson(std::string& jsonString) const {
    try {
        nlohmann::json jsonData;
        jsonData["version"] = CONFIG_VERSION;
        
        nlohmann::json configArray = nlohmann::json::array();
        for (const auto& entry : pImpl->entries) {
            nlohmann::json entryJson;
            entryJson["key"] = entry.key;
            entryJson["source"] = static_cast<int>(entry.source);
            entryJson["description"] = entry.description;
            
            // Add the actual value using the to_json helper
            to_json(entryJson["value"], entry.value);
            
            configArray.push_back(entryJson);
        }
        
        jsonData["config"] = configArray;
        
        jsonString = jsonData.dump(4); // Pretty print with 4 spaces
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving to JSON: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromYaml(const std::string& yamlString) {
    try {
        YAML::Node yamlData = YAML::Load(yamlString);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Extract version if present
        int version = yamlData["version"].as<int>(CONFIG_VERSION);
        
        // Check if config map exists
        if (yamlData["config"]) {
            const YAML::Node& configMap = yamlData["config"];
            for (const auto& entryNode : configMap) {
                ConfigEntry entry;
                entry.key = entryNode["key"].as<std::string>("");
                entry.description = entryNode["description"].as<std::string>("");
                entry.source = static_cast<ConfigSource>(entryNode["source"].as<int>(static_cast<int>(ConfigSource::File)));
                
                // Parse value based on its type
                if (entryNode["value_type"]) {
                    std::string type = entryNode["value_type"].as<std::string>();
                    if (type == "int") {
                        entry.value = entryNode["value"].as<int>();
                    } else if (type == "int64") {
                        entry.value = entryNode["value"].as<int64_t>();
                    } else if (type == "double") {
                        entry.value = entryNode["value"].as<double>();
                    } else if (type == "bool") {
                        entry.value = entryNode["value"].as<bool>();
                    } else if (type == "string") {
                        entry.value = entryNode["value"].as<std::string>();
                    } else if (type == "int_array") {
                        entry.value = entryNode["value"].as<std::vector<int>>();
                    } else if (type == "double_array") {
                        entry.value = entryNode["value"].as<std::vector<double>>();
                    } else if (type == "string_array") {
                        entry.value = entryNode["value"].as<std::vector<std::string>>();
                    }
                }
                
                if (schema_ && !schema_->validate(entry)) {
                    std::cerr << "Warning: Configuration entry " << entry.key << " failed validation" << std::endl;
                    continue;
                }
                
                pImpl->entries.push_back(entry);
            }
        } else {
            // If no config map, treat the whole YAML as a single configuration object
            for (const auto& entryNode : yamlData) {
                if (entryNode.first.as<std::string>() == "version") continue; // Skip version field
                
                ConfigEntry entry;
                entry.key = entryNode.first.as<std::string>();
                
                // Try to determine value type
                if (entryNode.second.IsScalar()) {
                    if (entryNode.second.IsInt()) {
                        entry.value = entryNode.second.as<int>();
                    } else if (entryNode.second.IsDouble()) {
                        entry.value = entryNode.second.as<double>();
                    } else if (entryNode.second.IsBoolean()) {
                        entry.value = entryNode.second.as<bool>();
                    } else {
                        entry.value = entryNode.second.as<std::string>();
                    }
                } else if (entryNode.second.IsSequence()) {
                    if (!entryNode.second.empty() && entryNode.second[0].IsInt()) {
                        entry.value = entryNode.second.as<std::vector<int>>();
                    } else if (!entryNode.second.empty() && entryNode.second[0].IsDouble()) {
                        entry.value = entryNode.second.as<std::vector<double>>();
                    } else {
                        entry.value = entryNode.second.as<std::vector<std::string>>();
                    }
                }
                
                entry.source = ConfigSource::File;
                entry.description = "Loaded from YAML string";
                
                if (schema_ && !schema_->validate(entry)) {
                    std::cerr << "Warning: Configuration entry " << entry.key << " failed validation" << std::endl;
                    continue;
                }
                
                pImpl->entries.push_back(entry);
            }
        }
        
        return true;
    } catch (const YAML::ParserException& e) {
        throw ConfigFileError("Invalid YAML string: " + std::string(e.what()));
    } catch (const std::exception& e) {
        throw ConfigFileError("Error parsing YAML string: " + std::string(e.what()));
    }
}

bool Config::saveToYaml(std::string& yamlString) const {
    try {
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        
        emitter << YAML::Key << "version";
        emitter << YAML::Value << CONFIG_VERSION;
        
        emitter << YAML::Key << "config";
        emitter << YAML::Value << YAML::BeginSeq;
        
        for (const auto& entry : pImpl->entries) {
            emitter << YAML::BeginMap;
            emitter << YAML::Key << "key";
            emitter << YAML::Value << entry.key;
            
            emitter << YAML::Key << "description";
            emitter << YAML::Value << entry.description;
            
            emitter << YAML::Key << "source";
            emitter << YAML::Value << static_cast<int>(entry.source);
            
            emitter << YAML::Key << "value_type";
            
            // Determine value type for YAML
            std::string type;
            std::visit([&type](const auto& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    type = "int";
                } else if constexpr (std::is_same_v<T, double>) {
                    type = "double";
                } else if constexpr (std::is_same_v<T, bool>) {
                    type = "bool";
                } else if constexpr (std::is_same_v<T, std::string>) {
                    type = "string";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    type = "int_array";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    type = "double_array";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    type = "string_array";
                }
            }, entry.value);
            
            emitter << YAML::Value << type;
            
            emitter << YAML::Key << "value";
            
            // Emit the actual value based on type
            std::visit([&emitter](const auto& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    emitter << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    emitter << YAML::Flow << arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    emitter << YAML::Flow << arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    emitter << YAML::Flow << arg;
                }
            }, entry.value);
            
            emitter << YAML::EndMap;
        }
        
        emitter << YAML::EndSeq;
        emitter << YAML::EndMap;
        
        yamlString = emitter.c_str();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving to YAML: " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string jsonString;
    if (!saveToJson(jsonString)) {
        return false;
    }
    
    file << jsonString;
    return true;
}

void Config::setSchema(const ConfigSchema& schema) {
    schema_ = std::make_shared<ConfigSchema>(schema);
}

bool Config::validate() const {
    if (!schema_) {
        return true; // No schema, everything is valid
    }
    
    for (const auto& entry : pImpl->entries) {
        if (!schema_->validate(entry)) {
            return false;
        }
    }
    
    return true;
}

std::optional<ConfigValue> Config::getNested(const std::string& key) const {
    // Handle nested keys like "brain.neuron_count"
    std::string normalizedKey = normalizeKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return normalizeKey(e.key) == normalizedKey; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
}

void Config::setNested(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::string normalizedKey = normalizeKey(key);
    std::string denormalizedKey = denormalizeKey(key);
    
    // Find existing entry or create new one
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return normalizeKey(e.key) == normalizedKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(denormalizedKey, value, source);
    }
}

bool Config::hasNested(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return normalizeKey(e.key) == normalizedKey; });
}

// Existing template methods implementation

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