// Comprehensive modern configuration system with JSON/YAML support
// Maintains full backward compatibility with key=value format
// Supports nested sections, arrays, type conversion, validation, inheritance, and more

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <optional>
#include <stdexcept>
#include <cctype>

// Try to include nlohmann/json if available
#ifdef __has_include
    #if __has_include(<nlohmann/json.hpp>)
        #include <nlohmann/json.hpp>
    #endif
#endif

// Try to include YAML-cpp if available  
#ifdef __has_include
    #if __has_include(<yaml-cpp/yaml.h>)
        #include <yaml-cpp/yaml.h>
    #endif
#endif

namespace nlm {

// Helper function for environment variable substitution
static std::string expandEnvVars(const std::string& input) {
    std::string result;
    result.reserve(input.size());
    
    for (size_t i = 0; i < input.size(); ++i) {
        if (i + 1 < input.size() && input[i] == '$' && input[i + 1] == '{') {
            // Find closing brace
            size_t bracePos = input.find('}', i + 2);
            if (bracePos != std::string::npos) {
                std::string varName = input.substr(i + 2, bracePos - (i + 2));
                const char* value = std::getenv(varName.c_str());
                if (value) {
                    result.append(value);
                }
                i = bracePos;
            } else {
                // If no closing brace, treat as normal text
                result.push_back(input[i]);
            }
        } else if (input[i] == '$' && i + 1 < input.size() && std::isalnum(input[i + 1])) {
            // Handle $VAR format
            size_t endPos = i + 1;
            while (endPos < input.size() && std::isalnum(input[endPos])) {
                endPos++;
            }
            std::string varName = input.substr(i + 1, endPos - (i + 1));
            const char* value = std::getenv(varName.c_str());
            if (value) {
                result.append(value);
            }
            i = endPos - 1;
        } else {
            result.push_back(input[i]);
        }
    }
    
    return result;
}

// Modern configuration implementation with JSON/YAML support
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> envVariables;
    std::map<std::string, std::shared_ptr<Config>> inheritedConfigs;
    
    // Schema for validation
    struct FieldSchema {
        std::string name;
        std::type_index type;
        bool required;
        std::string description;
    };
    
    std::vector<FieldSchema> schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Load environment variables on startup
    updateEnvVars();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

void Config::Impl::updateEnvVars() {
    const char* env = std::getenv("NLM_CONFIG_DIR");
    if (env) {
        envVariables["NLM_CONFIG_DIR"] = env;
    }
    
    const char* seed = std::getenv("NLM_RANDOM_SEED");
    if (seed) {
        envVariables["NLM_RANDOM_SEED"] = seed;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Fallback to legacy key=value format
        return loadFromLegacyFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef __has_include
    #if __has_include(<nlohmann/json.hpp>)
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        try {
            nlohmann::json jsonData = nlohmann::json::parse(file);
            
            // Clear existing entries
            clear();
            
            // Handle nested configuration
            if (jsonData.contains("configuration")) {
                jsonData = jsonData["configuration"];
            }
            
            // Flatten nested JSON for backward compatibility
            flattenJSON(jsonData, "");
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "JSON parsing error in " << filepath << ": " << e.what() << std::endl;
            return false;
        }
    #endif
#endif
    
    // nlohmann/json not available, fallback to legacy format
    return loadFromLegacyFormat(filepath);
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef __has_include
    #if __has_include(<yaml-cpp/yaml.h>)
        try {
            YAML::Node doc = YAML::LoadFile(filepath);
            
            // Clear existing entries
            clear();
            
            // Handle nested configuration
            if (doc["configuration"]) {
                doc = doc["configuration"];
            }
            
            // Flatten YAML for backward compatibility
            flattenYAML(doc, "");
            
            return true;
        } catch (const YAML::Exception& e) {
            std::cerr << "YAML parsing error in " << filepath << ": " << e.what() << std::endl;
            return false;
        }
    #endif
#endif
    
    // YAML-cpp not available, try legacy format
    return loadFromLegacyFormat(filepath);
}

bool Config::loadFromLegacyFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
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
            
            // Expand environment variables
            value = expandEnvVars(value);
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

void Config::Impl::flattenJSON(const nlohmann::json& json, const std::string& prefix) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = prefix.empty() ? it.key() : prefix + "." + it.key();
        
        if (it->is_object()) {
            flattenJSON(*it, key);
        } else if (it->is_array()) {
            // Convert arrays to JSON string representation for compatibility
            std::string arrayStr = "[";
            for (size_t i = 0; i < it->size(); ++i) {
                if (i > 0) arrayStr += ", ";
                arrayStr += std::to_string((*it)[i].get<double>());
            }
            arrayStr += "]";
            set(flattenKey(key), arrayStr, ConfigSource::File);
        } else {
            // Convert basic types to string for storage in variant
            std::string valueStr;
            if (it->is_string()) {
                valueStr = it->get<std::string>();
            } else if (it->is_number_integer()) {
                valueStr = std::to_string(it->get<int64_t>());
            } else if (it->is_number_float()) {
                valueStr = std::to_string(it->get<double>());
            } else if (it->is_boolean()) {
                valueStr = it->get<bool>() ? "true" : "false";
            }
            
            set(flattenKey(key), valueStr, ConfigSource::File);
        }
    }
}

void Config::Impl::flattenYAML(const YAML::Node& node, const std::string& prefix) {
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            std::string key = prefix.empty() ? it->first.as<std::string>() : prefix + "." + it->first.as<std::string>();
            flattenYAML(*it, key);
        }
    } else if (node.IsSequence()) {
        std::string arrayStr = "[";
        for (size_t i = 0; i < node.size(); ++i) {
            if (i > 0) arrayStr += ", ";
            if (node[i].IsScalar()) {
                arrayStr += node[i].as<std::string>();
            }
        }
        arrayStr += "]";
        set(flattenKey(key), arrayStr, ConfigSource::File);
    } else {
        std::string value = node.as<std::string>();
        set(flattenKey(prefix), value, ConfigSource::File);
    }
}

std::string Config::Impl::flattenKey(const std::string& key) {
    std::string result = key;
    std::replace(result.begin(), result.end(), '-', '_');
    std::replace(result.begin(), result.end(), '.', '_');
    return result;
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
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return saveToJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYAML(filepath);
    } else {
        // Legacy format
        return saveToLegacyFormat(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
#ifdef __has_include
    #if __has_include(<nlohmann/json.hpp>)
        nlohmann::json jsonData;
        
        // Create nested structure
        jsonData["configuration"] = nlohmann::json::object();
        
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            std::string nestedKey = unflattenKey(key);
            
            // Navigate to the right nested object
            nlohmann::json* current = &jsonData["configuration"];
            size_t dotPos = nestedKey.find('.');
            while (dotPos != std::string::npos) {
                std::string segment = nestedKey.substr(0, dotPos);
                nestedKey = nestedKey.substr(dotPos + 1);
                
                if (!current->contains(segment)) {
                    (*current)[segment] = nlohmann::json::object();
                }
                current = &((*current)[segment]);
                
                dotPos = nestedKey.find('.');
            }
            
            // Set the value
            setJSONValue(*current, nestedKey, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << jsonData.dump(4);
        return true;
    #endif
#endif
    
    // nlohmann/json not available, fallback to legacy format
    return saveToLegacyFormat(filepath);
}

bool Config::saveToYAML(const std::string& filepath) const {
#ifdef __has_include
    #if __has_include(<yaml-cpp/yaml.h>)
        YAML::Emitter emitter;
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "configuration";
        emitter << YAML::Value << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            std::string nestedKey = unflattenKey(key);
            
            // Navigate to the right nested object
            YAML::Node* current = &emitter;
            size_t dotPos = nestedKey.find('.');
            while (dotPos != std::string::npos) {
                std::string segment = nestedKey.substr(0, dotPos);
                nestedKey = nestedKey.substr(dotPos + 1);
                
                emitter << YAML::Key << segment;
                emitter << YAML::Value << YAML::BeginMap;
                current = &emitter;
                
                dotPos = nestedKey.find('.');
            }
            
            // Set the value
            emitter << YAML::Key << nestedKey;
            emitter << YAML::Value << getConfigValueAsString(entry.value);
        }
        
        emitter << YAML::EndMap;
        emitter << YAML::EndMap;
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << emitter.c_str();
        return true;
    #endif
#endif
    
    // YAML-cpp not available, fallback to legacy format
    return saveToLegacyFormat(filepath);
}

bool Config::saveToLegacyFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << getConfigValueAsString(entry.value) << "\n\n";
    }
    
    return true;
}

void Config::setJSONValue(nlohmann::json& json, const std::string& key, const ConfigValue& value) {
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            json[key] = static_cast<int>(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json[key] = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json[key] = arg;
        }
    }, value);
}

std::string Config::getConfigValueAsString(const ConfigValue& value) const {
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

std::string Config::unflattenKey(const std::string& flattenedKey) {
    std::string result = flattenedKey;
    std::replace(result.begin(), result.end(), '_', '.');
    return result;
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
    // Expand environment variables in the value
    std::string expandedValue = expandEnvVars(value);
    set(key, ConfigValue(expandedValue), source);
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

bool Config::validate() const {
    for (const auto& schemaEntry : pImpl->schema) {
        bool found = false;
        for (const auto& entry : pImpl->entries) {
            if (entry.key == schemaEntry.name) {
                found = true;
                
                // Check type
                if (!checkType(entry.value, schemaEntry.type)) {
                    std::cerr << "Validation error: Field '" << entry.key << "' has incorrect type" << std::endl;
                    return false;
                }
                break;
            }
        }
        
        if (schemaEntry.required && !found) {
            std::cerr << "Validation error: Required field '" << schemaEntry.name << "' is missing" << std::endl;
            return false;
        }
    }
    
    return true;
}

void Config::addSchema(const std::string& name, std::type_index type, bool required, const std::string& description) {
    pImpl->schema.push_back({name, type, required, description});
}

bool Config::mergeWith(const Config& other) {
    bool changed = false;
    for (const auto& entry : other.pImpl->entries) {
        bool found = false;
        for (auto& thisEntry : pImpl->entries) {
            if (thisEntry.key == entry.key) {
                // Only override if the other source has higher priority
                if (static_cast<int>(entry.source) > static_cast<int>(thisEntry.source)) {
                    thisEntry = entry;
                    changed = true;
                }
                found = true;
                break;
            }
        }
        
        if (!found) {
            pImpl->entries.push_back(entry);
            changed = true;
        }
    }
    
    return changed;
}

void Config::inheritFrom(const std::shared_ptr<Config>& parent) {
    if (parent) {
        pImpl->inheritedConfigs[parent->getKeys()[0]] = parent;
    }
}

std::string Config::generateDocumentation() const {
    std::ostringstream oss;
    oss << "# Configuration Documentation\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "## " << entry.key << "\n";
        oss << "- Type: " << getTypeName(entry.value) << "\n";
        oss << "- Source: " << static_cast<int>(entry.source) << "\n";
        oss << "- Value: " << getConfigValueAsString(entry.value) << "\n";
        oss << "- Description: " << (entry.description.empty() ? "(No description)" : entry.description) << "\n\n";
    }
    
    return oss.str();
}

std::string Config::getTypeName(const ConfigValue& value) const {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return "string";
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return "int64";
        } else if constexpr (std::is_same_v<T, double>) {
            return "double";
        } else if constexpr (std::is_same_v<T, bool>) {
            return "bool";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return "int[]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return "double[]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return "string[]";
        }
    }, value);
}

bool Config::checkType(const ConfigValue& value, std::type_index type) const {
    if (type == typeid(std::string)) {
        return std::holds_alternative<std::string>(value);
    } else if (type == typeid(int64_t)) {
        return std::holds_alternative<int64_t>(value);
    } else if (type == typeid(double)) {
        return std::holds_alternative<double>(value) || std::holds_alternative<int64_t>(value);
    } else if (type == typeid(bool)) {
        return std::holds_alternative<bool>(value);
    } else if (type == typeid(std::vector<int>)) {
        return std::holds_alternative<std::vector<int>>(value);
    } else if (type == typeid(std::vector<double>)) {
        return std::holds_alternative<std::vector<double>>(value);
    } else if (type == typeid(std::vector<std::string>)) {
        return std::holds_alternative<std::vector<std::string>>(value);
    }
    return false;
}

} // namespace nlm