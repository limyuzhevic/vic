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

#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

// Add JSON and YAML includes for enhanced configuration support
using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Configuration schema for validation
    std::map<std::string, std::string> schema;
    // Environment variable prefix for configuration lookup
    std::string envPrefix = "NLM_";
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // First try to load as JSON
    if (loadFromJSON(filepath)) {
        NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Then try YAML
    if (loadFromYAML(filepath)) {
        NLM_LOG_INFO("Loaded configuration from YAML file: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format
    NLM_LOG_INFO("Using legacy key=value format for: " + filepath);
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // Clear existing configuration
        clear();
        
        // Load from JSON object
        if (j.is_object()) {
            for (const auto& [key, value] : j.items()) {
                ConfigSource source = ConfigSource::File;
                std::string description = "Configuration value from JSON";
                
                // Extract metadata if available
                if (j[key].is_object() && j[key].contains("_source")) {
                    std::string sourceStr = j[key]["_source"];
                    if (sourceStr == "default") source = ConfigSource::Default;
                    else if (sourceStr == "command_line") source = ConfigSource::CommandLine;
                    else if (sourceStr == "runtime") source = ConfigSource::Runtime;
                }
                
                if (j[key].is_object() && j[key].contains("_description")) {
                    description = j[key]["_description"];
                }
                
                setConfigValueFromJSON(key, value, source, description);
            }
        }
        
        return true;
    } catch (const json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading JSON: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node y;
        file >> y;
        
        // Clear existing configuration
        clear();
        
        // Convert YAML to configuration entries
        convertYAMLToConfig(y, ConfigSource::File, "Configuration value from YAML");
        
        return true;
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading YAML: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
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
        } else {
            NLM_LOG_WARNING("Invalid line " + std::to_string(lineNum) + 
                           " in config file: " + line);
        }
    }
    
    return true;
}

void Config::convertYAMLToConfig(const YAML::Node& y, ConfigSource source, const std::string& description) {
    if (y.IsMap()) {
        for (const auto& it : y) {
            std::string key = it.first.as<std::string>();
            YAML::Node value = it.second;
            
            setConfigValueFromYAML(key, value, source, description + " -> " + key);
        }
    }
}

void Config::setConfigValueFromJSON(const std::string& key, const json& value, ConfigSource source, const std::string& description) {
    if (value.is_number_integer()) {
        set(key, value.get<int64_t>(), source);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), source);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), source);
    } else if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_array()) {
        // Handle arrays
        if (value.empty()) {
            set(key, std::vector<int>(), source);
        } else if (value.front().is_number_integer()) {
            std::vector<int64_t> array;
            for (const auto& item : value) {
                array.push_back(item.get<int64_t>());
            }
            // Note: This might need adjustment based on the actual ConfigValue variant types
            // For now, we'll handle it as a placeholder
        }
    }
}

void Config::setConfigValueFromYAML(const std::string& key, const YAML::Node& value, ConfigSource source, const std::string& description) {
    if (value.IsScalar()) {
        if (value.IsInt()) {
            set(key, value.as<int64_t>(), source);
        } else if (value.IsFloat()) {
            set(key, value.as<double>(), source);
        } else if (value.IsBoolean()) {
            set(key, value.as<bool>(), source);
        } else {
            set(key, value.as<std::string>(), source);
        }
    } else if (value.IsSequence()) {
        // Handle YAML sequences
        std::vector<int64_t> array;
        for (const auto& item : value) {
            if (item.IsInt()) {
                array.push_back(item.as<int64_t>());
            }
        }
        // Note: This might need adjustment based on the actual ConfigValue variant types
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    // Auto-detect file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return saveToJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYAML(filepath);
    } else {
        return saveToSimpleFormat(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        
        for (const auto& entry : pImpl->entries) {
            json value;
            
            // Convert ConfigValue to JSON
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    value = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    value = arg;
                } else {
                    value = "unknown_type";
                }
            }, entry.value);
            
            // Add metadata
            j[entry.key] = value;
        }
        
        file << j.dump(2); // Pretty-print with 2-space indentation
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON config: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Emitter y;
        y << YAML::BeginMap;
        
        for (const auto& entry : pImpl->entries) {
            y << YAML::Key << entry.key;
            
            // Convert ConfigValue to YAML
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    y << arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    y << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    y << arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    y << arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    y << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    y << YAML::Flow << arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    y << YAML::Flow << arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    y << YAML::Flow << arg;
                } else {
                    y << "unknown_type";
                }
            }, entry.value);
        }
        
        y << YAML::EndMap;
        file << y.c_str();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving YAML config: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert ConfigValue to string
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else {
                file << "UNKNOWN";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

void Config::validate() const {
    NLM_LOG_INFO("Validating configuration...");
    
    // Check for required keys
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Configuration key missing: " + key + ". Using default.");
        }
    }
    
    // Validate value ranges
    if (has("random_seed")) {
        auto seed = get<int64_t>("random_seed");
        if (seed && *seed < 0) {
            NLM_LOG_ERROR("Invalid random_seed value: " + std::to_string(*seed) +
                         ". Must be non-negative.");
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (timestep && *timestep <= 0.0) {
            NLM_LOG_ERROR("Invalid simulation_timestep value: " + std::to_string(*timestep) +
                         ". Must be positive.");
        }
    }
    
    if (has("neuron_count")) {
        auto neuronCount = get<int64_t>("neuron_count");
        if (neuronCount && *neuronCount <= 0) {
            NLM_LOG_ERROR("Invalid neuron_count value: " + std::to_string(*neuronCount) +
                         ". Must be positive.");
        }
    }
    
    NLM_LOG_INFO("Configuration validation complete.");
}

void Config::loadFromEnvironment() {
    NLM_LOG_INFO("Loading configuration from environment variables...");
    
    for (const auto& entry : pImpl->entries) {
        std::string envKey = pImpl->envPrefix + entry.key;
        
        char* envValue = std::getenv(envKey.c_str());
        if (envValue != nullptr) {
            std::string valueStr(envValue);
            NLM_LOG_INFO("Loaded from environment: " + envKey + " = " + valueStr);
            
            // Try to parse and set the value
            try {
                setFromEnvironmentString(entry.key, valueStr);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to parse environment variable " + envKey +
                             ": " + std::string(e.what()));
            }
        }
    }
}

void Config::setFromEnvironmentString(const std::string& key, const std::string& valueStr) {
    // Try different types based on existing entry or heuristics
    if (has(key)) {
        // Check the type of the existing entry
        auto existing = getFromKey(key);
        if (existing.has_value()) {
            // Try to match the type
            try {
                // This is a simplified version - would need proper type detection
                set(key, valueStr, ConfigSource::Runtime);
            } catch (...) {
                // Fall back to string
                set(key, valueStr, ConfigSource::Runtime);
            }
        }
    } else {
        // Guess type based on value
        if (valueStr == "true" || valueStr == "false") {
            set(key, valueStr == "true", ConfigSource::Runtime);
        } else {
            // Try integer
            try {
                size_t pos;
                int64_t intVal = std::stoll(valueStr, &pos);
                if (pos == valueStr.length()) {
                    set(key, intVal, ConfigSource::Runtime);
                    return;
                }
            } catch (...) {}
            
            // Try double
            try {
                size_t pos;
                double doubleVal = std::stod(valueStr, &pos);
                if (pos == valueStr.length()) {
                    set(key, doubleVal, ConfigSource::Runtime);
                    return;
                }
            } catch (...) {}
            
            // Default to string
            set(key, valueStr, ConfigSource::Runtime);
        }
    }
}

std::optional<Config::ConfigValue> Config::getFromKey(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->value;
    }
    
    return std::nullopt;
}

bool Config::validateSchema() const {
    NLM_LOG_INFO("Validating configuration schema...");
    
    // Check if schema is defined
    if (pImpl->schema.empty()) {
        NLM_LOG_INFO("No schema defined, skipping schema validation");
        return true;
    }
    
    // Validate against schema
    for (const auto& [key, schemaType] : pImpl->schema) {
        if (!has(key)) {
            NLM_LOG_WARNING("Schema key missing: " + key);
            continue;
        }
        
        auto value = getFromKey(key);
        if (!value.has_value()) {
            continue;
        }
        
        // Validate type
        bool typeValid = validateSchemaType(key, *value, schemaType);
        if (!typeValid) {
            NLM_LOG_ERROR("Schema validation failed for key: " + key +
                         " (expected type: " + schemaType + ")");
            return false;
        }
    }
    
    NLM_LOG_INFO("Schema validation successful.");
    return true;
}

bool Config::validateSchemaType(const std::string& key, const ConfigValue& value, const std::string& schemaType) const {
    switch (hashString(schemaType)) {
        case hashString("int"):
        case hashString("integer"):
            return std::holds_alternative<int>(value) ||
                   std::holds_alternative<int64_t>(value);
        
        case hashString("float"):
        case hashString("double"):
            return std::holds_alternative<double>(value);
        
        case hashString("bool"):
        case hashString("boolean"):
            return std::holds_alternative<bool>(value);
        
        case hashString("string"):
            return std::holds_alternative<std::string>(value);
        
        case hashString("int_array"):
        case hashString("array_int"):
            return std::holds_alternative<std::vector<int>>(value);
        
        case hashString("float_array"):
        case hashString("array_float"):
            return std::holds_alternative<std::vector<double>>(value);
        
        case hashString("string_array"):
        case hashString("array_string"):
            return std::holds_alternative<std::vector<std::string>>(value);
        
        default:
            NLM_LOG_WARNING("Unknown schema type: " + schemaType);
            return true; // Allow unknown types for flexibility
    }
}

std::size_t Config::hashString(const std::string& str) const {
    std::hash<std::string> hasher;
    return hasher(str);
}

void Config::setSchema(const std::string& key, const std::string& type) {
    pImpl->schema[key] = type;
    NLM_LOG_INFO("Schema set for key '" + key + "': " + type);
}

void Config::clearSchema() {
    pImpl->schema.clear();
    NLM_LOG_INFO("Schema cleared");
}

std::map<std::string, std::string> Config::getSchema() const {
    return pImpl->schema;
}

void Config::exportToJSON(const std::string& filepath, const std::map<std::string, std::string>& selectedKeys) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }
    
    json j;
    
    for (const auto& [key, type] : selectedKeys) {
        if (has(key)) {
            j[key] = getConfigValueAsJSON(key);
        }
    }
    
    file << j.dump(2);
}

void Config::exportToYAML(const std::string& filepath, const std::map<std::string, std::string>& selectedKeys) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }
    
    YAML::Emitter y;
    y << YAML::BeginMap;
    
    for (const auto& [key, type] : selectedKeys) {
        if (has(key)) {
            y << YAML::Key << key;
            y << getConfigValueAsYAML(key);
        }
    }
    
    y << YAML::EndMap;
    file << y.c_str();
}

json Config::getConfigValueAsJSON(const std::string& key) const {
    auto value = getFromKey(key);
    if (!value.has_value()) {
        return nullptr;
    }
    
    json j;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        } else {
            j = "unknown_type";
        }
    }, *value);
    
    return j;
}

YAML::Node Config::getConfigValueAsYAML(const std::string& key) const {
    auto value = getFromKey(key);
    if (!value.has_value()) {
        return YAML::Node();
    }
    
    YAML::Emitter y;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            y << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            y << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            y << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            y << arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            y << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            y << YAML::Flow << arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            y << YAML::Flow << arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            y << YAML::Flow << arg;
        } else {
            y << "unknown_type";
        }
    }, *value);
    
    return YAML::Node(y.c_str());
}

std::string Config::generateConfigReport() const {
    std::ostringstream oss;
    oss << "NLM Configuration Report" << std::endl;
    oss << "=" << std::string(50, '=') << std::endl;
    oss << std::endl;
    
    oss << "Configuration Entries: " << pImpl->entries.size() << std::endl;
    oss << "Schema Entries: " << pImpl->schema.size() << std::endl;
    oss << std::endl;
    
    oss << "Detailed Configuration:" << std::endl;
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << "UNKNOWN";
            }
        }, entry.value);
        
        oss << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    return oss.str();
}

std::vector<std::string> Config::findSimilarKeys(const std::string& pattern) const {
    std::vector<std::string> similarKeys;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(pattern) != std::string::npos) {
            similarKeys.push_back(entry.key);
        }
    }
    
    return similarKeys;
}

std::string Config::getValueSummary(const std::string& key) const {
    if (!has(key)) {
        return "Key not found";
    }
    
    auto value = getFromKey(key);
    if (!value.has_value()) {
        return "Key not found";
    }
    
    std::ostringstream oss;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << "int: " << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << "int64_t: " << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << "double: " << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << "bool: " << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "string: \"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "int[]:";
            for (const auto& item : arg) oss << " " << item;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "double[]:";
            for (const auto& item : arg) oss << " " << std::fixed << std::setprecision(6) << item;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "string[]:";
            for (const auto& item : arg) oss << " \"" << item << "\"";
        } else {
            oss << "unknown_type";
        }
    }, *value);
    
    return oss.str();
}

void Config::merge(const Config& other) {
    NLM_LOG_INFO("Merging configuration from another source...");
    
    for (const auto& entry : other.pImpl->entries) {
        // Override if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (it != pImpl->entries.end()) {
            // Update existing entry with new value and source
            it->value = entry.value;
            it->source = entry.source;
        } else {
            pImpl->entries.push_back(entry);
        }
    }
    
    NLM_LOG_INFO("Configuration merge complete. Total entries: " + std::to_string(pImpl->entries.size()));
}

bool Config::diff(const Config& other) const {
    NLM_LOG_INFO("Comparing configurations...");
    
    bool hasDiffs = false;
    
    // Check entries in this config that aren't in other
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (entry.key == otherEntry.key && entry.value == otherEntry.value) {
                found = true;
                break;
            }
        }
        if (!found) {
            NLM_LOG_INFO("Entry added: " + entry.key);
            hasDiffs = true;
        }
    }
    
    // Check entries in other config that aren't in this
    for (const auto& entry : other.pImpl->entries) {
        bool found = false;
        for (const auto& thisEntry : pImpl->entries) {
            if (entry.key == thisEntry.key && entry.value == thisEntry.value) {
                found = true;
                break;
            }
        }
        if (!found) {
            NLM_LOG_INFO("Entry removed: " + entry.key);
            hasDiffs = true;
        }
    }
    
    if (!hasDiffs) {
        NLM_LOG_INFO("Configurations are identical.");
    }
    
    return hasDiffs;
}

std::map<std::string, std::string> Config::exportAsEnvVars() const {
    std::map<std::string, std::string> envVars;
    
    for (const auto& entry : pImpl->entries) {
        std::string envKey = pImpl->envPrefix + entry.key;
        
        // Convert ConfigValue to string
        std::string valueStr;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                valueStr = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStr = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::string>) {
                valueStr = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::ostringstream oss;
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
                valueStr = oss.str();
            } else {
                valueStr = "unknown";
            }
        }, entry.value);
        
        envVars[envKey] = valueStr;
    }
    
    return envVars;
}

void Config::importFromEnvVars(const std::map<std::string, std::string>& envVars) {
    NLM_LOG_INFO("Importing configuration from environment variables...");
    
    for (const auto& [envKey, envValue] : envVars) {
        // Remove prefix to get config key
        std::string key = envKey;
        if (key.rfind(pImpl->envPrefix, 0) == 0) {
            key = key.substr(pImpl->envPrefix.length());
        }
        
        try {
            setFromEnvironmentString(key, envValue);
            NLM_LOG_INFO("Imported from env: " + envKey + " = " + envValue);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to import from env " + envKey + ": " + std::string(e.what()));
        }
    }
}

void Config::addValidationRule(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
    NLM_LOG_INFO("Added validation rule for key: " + key);
    // TODO: Implement validation rule storage and enforcement
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Basic validation
    if (has("random_seed")) {
        auto seed = get<int64_t>("random_seed");
        if (seed && *seed < 0) {
            errors.push_back("random_seed must be non-negative");
        }
    }
    
    if (has("simulation_timestep")) {
        auto timestep = get<double>("simulation_timestep");
        if (timestep && *timestep <= 0.0) {
            errors.push_back("simulation_timestep must be positive");
        }
    }
    
    if (has("neuron_count")) {
        auto neuronCount = get<int64_t>("neuron_count");
        if (neuronCount && *neuronCount <= 0) {
            errors.push_back("neuron_count must be positive");
        }
    }
    
    // Schema validation
    if (!validateSchema()) {
        errors.push_back("Schema validation failed");
    }
    
    return errors;
}

bool Config::isValid() const {
    auto errors = getValidationErrors();
    return errors.empty();
}

std::string Config::getEnvironmentVariablePrefix() const {
    return pImpl->envPrefix;
}

void Config::setEnvironmentVariablePrefix(const std::string& prefix) {
    pImpl->envPrefix = prefix;
    NLM_LOG_INFO("Environment variable prefix set to: " + prefix);
}

std::chrono::system_clock::time_point Config::getCreationTime() const {
    return creationTime;
}

void Config::setCreationTime(const std::chrono::system_clock::time_point& time) {
    creationTime = time;
}

std::chrono::system_clock::time_point Config::getLastModifiedTime() const {
    return lastModifiedTime;
}

void Config::setLastModifiedTime(const std::chrono::system_clock::time_point& time) {
    lastModifiedTime = time;
}

std::string Config::getSourceSummary() const {
    std::map<ConfigSource, int> sourceCount;
    
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    std::ostringstream oss;
    oss << "Configuration source summary:" << std::endl;
    
    for (const auto& [source, count] : sourceCount) {
        oss << "  Source " << static_cast<int>(source) << ": " << count << " entries" << std::endl;
    }
    
    return oss.str();
}

std::size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

std::size_t Config::getSchemaEntryCount() const {
    return pImpl->schema.size();
}

bool Config::hasSchema() const {
    return !pImpl->schema.empty();
}

const std::vector<ConfigEntry>& Config::getAllEntries() const {
    return pImpl->entries;
}

const std::map<std::string, std::string>& Config::getSchemaMap() const {
    return pImpl->schema;
}

// Helper function for configuration comparison
bool operator==(const Config& lhs, const Config& rhs) {
    return lhs.pImpl->entries == rhs.pImpl->entries;
}

bool operator!=(const Config& lhs, const Config& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& os, const Config& config) {
    os << config.summary();
    return os;
}

} // namespace nlm
