#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced loadFromFile with JSON support
bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Check if file exists
        if (!std::filesystem::exists(filepath)) {
            NLM_LOG_WARN("Configuration file does not exist: " + filepath);
            return false;
        }

        // Parse JSON file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
            return false;
        }

        nlohmann::json j;
        try {
            file >> j;
        } catch (const nlohmann::json::parse_error& e) {
            NLM_LOG_ERROR("Failed to parse JSON configuration file " + filepath + ": " + std::string(e.what()));
            return false;
        }

        // Clear existing entries
        pImpl->entries.clear();

        // Parse JSON entries
        if (j.contains("config") && j["config"].is_object()) {
            // New JSON format with "config" object
            const auto& config = j["config"];
            for (const auto& [key, value] : config.items()) {
                ConfigValue configValue;
                if (configValueFromJson(value, configValue)) {
                    set(key, configValue, ConfigSource::File);
                } else {
                    NLM_LOG_WARN("Skipping invalid configuration value for key \"" + key + "\"");
                }
            }
        } else {
            // Legacy format - assume top-level key-value pairs
            for (const auto& [key, value] : j.items()) {
                ConfigValue configValue;
                if (configValueFromJson(value, configValue)) {
                    set(key, configValue, ConfigSource::File);
                } else {
                    NLM_LOG_WARN("Skipping invalid configuration value for key \"" + key + "\"");
                }
            }
        }

        NLM_LOG_INFO("Successfully loaded " + std::to_string(pImpl->entries.size()) + " configuration entries from " + filepath);
        return true;

    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Backward compatible loadFromArgs
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

// Enhanced saveToFile with proper JSON support
bool Config::saveToFile(const std::string& filepath) const {
    try {
        nlohmann::json j;
        
        // Create config object
        nlohmann::json configObj = nlohmann::json::object();
        
        // Add all entries to the config object
        for (const auto& entry : pImpl->entries) {
            configObj[entry.key] = configValueToJson(entry.value);
        }
        
        // Wrap in "config" object for backward compatibility
        j["config"] = configObj;
        
        // Add metadata
        j["metadata"] = nlohmann::json::object();
        j["metadata"]["source_count"] = pImpl->entries.size();
        j["metadata"]["timestamp"] = nlohmann::json::now();

        // Write JSON to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
            return false;
        }
        
        // Write with proper indentation for readability
        file << std::setw(2) << j;
        
        NLM_LOG_INFO("Successfully saved " + std::to_string(pImpl->entries.size()) + " configuration entries to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

// Load configuration from JSON string
bool Config::loadFromJsonString(const std::string& jsonString) {
    try {
        nlohmann::json j;
        try {
            j = nlohmann::json::parse(jsonString);
        } catch (const nlohmann::json::parse_error& e) {
            NLM_LOG_ERROR("Failed to parse JSON string: " + std::string(e.what()));
            return false;
        }

        // Clear existing entries
        pImpl->entries.clear();

        // Parse JSON entries
        if (j.contains("config") && j["config"].is_object()) {
            const auto& config = j["config"];
            for (const auto& [key, value] : config.items()) {
                ConfigValue configValue;
                if (configValueFromJson(value, configValue)) {
                    set(key, configValue, ConfigSource::Runtime);
                } else {
                    NLM_LOG_WARN("Skipping invalid configuration value for key \"" + key + "\"");
                }
            }
        } else {
            for (const auto& [key, value] : j.items()) {
                ConfigValue configValue;
                if (configValueFromJson(value, configValue)) {
                    set(key, configValue, ConfigSource::Runtime);
                } else {
                    NLM_LOG_WARN("Skipping invalid configuration value for key \"" + key + "\"");
                }
            }
        }

        NLM_LOG_INFO("Successfully loaded " + std::to_string(pImpl->entries.size()) + " configuration entries from JSON string");
        return true;

    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration from JSON string: " + std::string(e.what()));
        return false;
    }
}

// Save configuration to JSON string
std::string Config::saveToJsonString() const {
    try {
        nlohmann::json j;
        
        // Create config object
        nlohmann::json configObj = nlohmann::json::object();
        
        // Add all entries to the config object
        for (const auto& entry : pImpl->entries) {
            configObj[entry.key] = configValueToJson(entry.value);
        }
        
        // Wrap in "config" object
        j["config"] = configObj;
        
        // Add metadata
        j["metadata"] = nlohmann::json::object();
        j["metadata"]["source_count"] = pImpl->entries.size();
        j["metadata"]["timestamp"] = nlohmann::json::now();

        // Return as formatted string
        return j.dump(2);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to JSON string: " + std::string(e.what()));
        return "";
    }
}

// JSON conversion helpers
nlohmann::json Config::configValueToJson(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return nlohmann::json(arg);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return nlohmann::json(arg);
        }
    }, value);
}

bool Config::configValueFromJson(const nlohmann::json& j, ConfigValue& value) {
    try {
        if (j.is_string()) {
            value = j.get<std::string>();
        } else if (j.is_number_integer()) {
            value = j.get<int>();
        } else if (j.is_number_unsigned()) {
            value = static_cast<int64_t>(j.get<uint64_t>());
        } else if (j.is_number_float()) {
            value = j.get<double>();
        } else if (j.is_boolean()) {
            value = j.get<bool>();
        } else if (j.is_array()) {
            // Try to detect array type
            if (!j.empty()) {
                const auto& first = j[0];
                if (first.is_string()) {
                    std::vector<std::string> vec = j.get<std::vector<std::string>>();
                    value = vec;
                } else if (first.is_number_integer()) {
                    std::vector<int> vec = j.get<std::vector<int>>();
                    value = vec;
                } else if (first.is_number_float()) {
                    std::vector<double> vec = j.get<std::vector<double>>();
                    value = vec;
                } else {
                    NLM_LOG_WARN("Unsupported array element type in JSON configuration");
                    return false;
                }
            } else {
                // Empty array - default to vector of strings
                value = std::vector<std::string>();
            }
        } else {
            NLM_LOG_WARN("Unsupported JSON type for configuration value");
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error converting JSON to ConfigValue: " + std::string(e.what()));
        return false;
    }
}

nlohmann::json Config::vectorToJson(const auto& vec) {
    return nlohmann::json(vec);
}

template<typename T>
std::vector<T> Config::jsonToVector(const nlohmann::json& j) {
    return j.get<std::vector<T>>();
}

void Config::instantiateJsonTemplates() {
    // Force instantiation of templates
    jsonToVector<int>({});
    jsonToVector<double>({});
    jsonToVector<std::string>({});
}

// Get values
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

// Set values
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

// Additional configuration methods for CLI support

// Profile definitions for CLI profiles
const std::string Config::PROFILE_BASIC = "basic";
const std::string Config::PROFILE_ADVANCED = "advanced";
const std::string Config::PROFILE_PRODUCTION = "production";
const std::string Config::PROFILE_DEVELOPMENT = "development";

// Apply a predefined configuration profile
bool Config::applyProfile(const std::string& profileName) {
    NLM_LOG_INFO("Applying configuration profile: " + profileName);
    
    if (profileName == PROFILE_BASIC) {
        set("neuron_count", static_cast<int64_t>(100), ConfigSource::Profile);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Profile);
        set("connection_probability", 0.1f, ConfigSource::Profile);
        set("stdp_ltp_weight", 0.01f, ConfigSource::Profile);
        set("stdp_ltd_weight", 0.005f, ConfigSource::Profile);
        set("enable_structural_plasticity", false, ConfigSource::Profile);
    } else if (profileName == PROFILE_ADVANCED) {
        set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Profile);
        set("region_count", static_cast<int64_t>(2), ConfigSource::Profile);
        set("connection_probability", 0.15f, ConfigSource::Profile);
        set("stdp_ltp_weight", 0.02f, ConfigSource::Profile);
        set("stdp_ltd_weight", 0.015f, ConfigSource::Profile);
        set("synaptogenesis_rate", 0.0002f, ConfigSource::Profile);
        set("pruning_rate", 0.00002f, ConfigSource::Profile);
        set("enable_curiosity", true, ConfigSource::Profile);
        set("curiosity_intensity", 0.7f, ConfigSource::Profile);
        set("enable_reward_modulation", true, ConfigSource::Profile);
    } else if (profileName == PROFILE_PRODUCTION) {
        set("neuron_count", static_cast<int64_t>(500), ConfigSource::Profile);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Profile);
        set("connection_probability", 0.12f, ConfigSource::Profile);
        set("stdp_ltp_weight", 0.015f, ConfigSource::Profile);
        set("stdp_ltd_weight", 0.01f, ConfigSource::Profile);
        set("enable_reward_modulation", true, ConfigSource::Profile);
        set("enable_structural_plasticity", true, ConfigSource::Profile);
        set("enable_development", false, ConfigSource::Profile);
    } else if (profileName == PROFILE_DEVELOPMENT) {
        set("neuron_count", static_cast<int64_t>(200), ConfigSource::Profile);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Profile);
        set("connection_probability", 0.2f, ConfigSource::Profile);
        set("stdp_ltp_weight", 0.03f, ConfigSource::Profile);
        set("stdp_ltd_weight", 0.02f, ConfigSource::Profile);
        set("synaptogenesis_rate", 0.0005f, ConfigSource::Profile);
        set("pruning_rate", 0.00005f, ConfigSource::Profile);
        set("enable_development", true, ConfigSource::Profile);
        set("developmental_stage", "Initial", ConfigSource::Profile);
        set("critical_period_duration", 100.0f, ConfigSource::Profile);
    } else {
        NLM_LOG_ERROR("Unknown profile: " + profileName);
        return false;
    }
    
    NLM_LOG_INFO("Profile applied successfully");
    return true;
}

// Validate configuration with JSON schema
bool Config::validateWithSchema(const std::string& schemaPath) const {
    NLM_LOG_INFO("Validating configuration with schema: " + schemaPath);
    
    // Simple validation - check for required fields
    std::vector<std::string> requiredFields = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    bool isValid = true;
    for (const auto& field : requiredFields) {
        if (!has(field)) {
            NLM_LOG_WARN("Missing required field: " + field);
            isValid = false;
        }
    }
    
    // Check type consistency
    auto checkType = [this](const std::string& key, const std::type_info& expectedType) {
        auto value = getConfigEntry(key);
        if (value && typeid(*value) != expectedType) {
            NLM_LOG_WARN("Type mismatch for field " + key + ": expected " + 
                        std::string(expectedType.name()) + " but got " + 
                        std::string(typeid(*value).name()));
            return false;
        }
        return true;
    };
    
    if (isValid) {
        NLM_LOG_INFO("Configuration validation completed successfully");
    } else {
        NLM_LOG_ERROR("Configuration validation failed");
    }
    
    return isValid;
}

// Get configuration entry by key
ConfigEntry Config::getConfigEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return *it;
    }
    
    return ConfigEntry();
}

// Get configuration value for export/import
std::string Config::getExportString() const {
    nlohmann::json j;
    
    nlohmann::json configObj = nlohmann::json::object();
    for (const auto& entry : pImpl->entries) {
        configObj[entry.key] = configValueToJson(entry.value);
    }
    
    j["config"] = configObj;
    j["metadata"] = nlohmann::json::object();
    j["metadata"]["source_count"] = pImpl->entries.size();
    j["metadata"]["timestamp"] = nlohmann::json::now();
    j["metadata"]["version"] = "2.0";
    
    return j.dump(2);
}

// Load configuration from export string
bool Config::loadFromExportString(const std::string& exportString) {
    try {
        nlohmann::json j = nlohmann::json::parse(exportString);
        
        if (j.contains("config") && j["config"].is_object()) {
            clear();
            const auto& config = j["config"];
            for (const auto& [key, value] : config.items()) {
                ConfigValue configValue;
                if (configValueFromJson(value, configValue)) {
                    set(key, configValue, ConfigSource::Runtime);
                } else {
                    NLM_LOG_WARN("Skipping invalid configuration value for key \"" + key + "\"");
                }
            }
            
            NLM_LOG_INFO("Successfully loaded " + std::to_string(pImpl->entries.size()) + 
                        " configuration entries from export string");
            return true;
        } else {
            NLM_LOG_ERROR("Invalid export string format - missing 'config' object");
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration from export string: " + std::string(e.what()));
        return false;
    }
}

// Get all configuration keys
std::vector<std::string> Config::getAllKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

// Get configuration summary in CSV format
std::string Config::getCsvSummary() const {
    std::ostringstream oss;
    oss << "key,value,type,source\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "\"" << entry.key << "\",";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes and newlines in string values
                std::string str = arg;
                std::replace(str.begin(), str.end(), '\"', '\"\"');
                std::replace(str.begin(), str.end(), '\n', "\\n");
                oss << "\"" << str << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << "," << static_cast<int>(entry.source) << "\n";
    }
    
    return oss.str();
}

// Convert configuration to JSON string with metadata
std::string Config::toJsonWithMetadata() const {
    nlohmann::json j;
    
    nlohmann::json configObj = nlohmann::json::object();
    for (const auto& entry : pImpl->entries) {
        configObj[entry.key] = configValueToJson(entry.value);
    }
    
    j["config"] = configObj;
    j["metadata"] = nlohmann::json::object();
    j["metadata"]["source_count"] = pImpl->entries.size();
    j["metadata"]["timestamp"] = nlohmann::json::now();
    j["metadata"]["version"] = "2.0";
    j["metadata"]["format"] = "NLM_Config_JSON";
    
    return j.dump(2);
}

// Import configuration from metadata string
bool Config::importFromMetadata(const std::string& metadataJson) {
    try {
        nlohmann::json j = nlohmann::json::parse(metadataJson);
        
        if (!j.contains("config") || !j["config"].is_object()) {
            NLM_LOG_ERROR("Invalid metadata format - missing 'config' object");
            return false;
        }
        
        // Clear existing configuration
        clear();
        
        // Load configuration from metadata
        const auto& config = j["config"];
        for (const auto& [key, value] : config.items()) {
            ConfigValue configValue;
            if (configValueFromJson(value, configValue)) {
                set(key, configValue, ConfigSource::Runtime);
            } else {
                NLM_LOG_WARN("Skipping invalid configuration value for key \"" + key + "\"");
            }
        }
        
        // Restore metadata if present
        if (j.contains("metadata") && j["metadata"].is_object()) {
            auto metadata = j["metadata"];
            if (metadata.contains("version")) {
                set("config_version", metadata["version"].get<std::string>(), ConfigSource::Runtime);
            }
        }
        
        NLM_LOG_INFO("Successfully imported " + std::to_string(pImpl->entries.size()) + 
                    " configuration entries from metadata");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error importing configuration from metadata: " + std::string(e.what()));
        return false;
    }
}

// Get configuration profile names
std::vector<std::string> Config::getAvailableProfiles() {
    return {
        PROFILE_BASIC,
        PROFILE_ADVANCED,
        PROFILE_PRODUCTION,
        PROFILE_DEVELOPMENT
    };
}

// Validate configuration field type
bool Config::validateFieldType(const std::string& key, const std::type_info& expectedType) const {
    auto value = getConfigEntry(key);
    if (!value.key.empty()) {
        return typeid(value.value) == expectedType;
    }
    return false;
}

// Get configuration field type name
std::string Config::getFieldTypeName(const std::string& key) const {
    auto value = getConfigEntry(key);
    if (!value.key.empty()) {
        const std::type_info& typeInfo = typeid(value.value);
        if (typeInfo == typeid(int)) return "int";
        else if (typeInfo == typeid(int64_t)) return "int64_t";
        else if (typeInfo == typeid(double)) return "double";
        else if (typeInfo == typeid(bool)) return "bool";
        else if (typeInfo == typeid(std::string)) return "string";
        else if (typeInfo == typeid(std::vector<int>)) return "vector<int>";
        else if (typeInfo == typeid(std::vector<double>)) return "vector<double>";
        else if (typeInfo == typeid(std::vector<std::string>)) return "vector<string>";
    }
    return "unknown";
}

// Export configuration with metadata
bool Config::exportWithMetadata(const std::string& filepath) const {
    std::string jsonString = toJsonWithMetadata();
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    file << jsonString;
    file.close();
    
    NLM_LOG_INFO("Configuration exported with metadata to: " + filepath);
    return true;
}

// Import configuration with metadata
bool Config::importWithMetadata(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for reading: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return importFromMetadata(buffer.str());
}