#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <numeric>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
    std::unordered_map<std::string, ConfigSource> sourcePriority;
    std::string lastLoadedFile;
    std::chrono::system_clock::time_point lastModifiedTime;
    
    // Constructor initializes with default schemas
    Impl() {
        // Initialize source priority (lower number = higher priority)
        sourcePriority[ConfigSource::CommandLine] = 1;
        sourcePriority[ConfigSource::File] = 2;
        sourcePriority[ConfigSource::Runtime] = 3;
        sourcePriority[ConfigSource::Default] = 4;
        sourcePriority[ConfigSource::SchemaDefault] = 5;
        
        // Initialize default schemas
        initializeDefaultSchemas();
    }
    
    void initializeDefaultSchemas() {
        // Add common NLM configuration schemas
        schemas.push_back(ConfigSchema("random_seed", "int64_t", ConfigSource::SchemaDefault,
                                       static_cast<int64_t>(42), "Random seed for reproducible simulations"));
        
        schemas.push_back(ConfigSchema("simulation_timestep", "double", ConfigSource::SchemaDefault,
                                       0.001, "Time step for simulation in seconds"));
        
        schemas.push_back(ConfigSchema("neuron_count", "int64_t", ConfigSource::SchemaDefault,
                                       static_cast<int64_t>(1000), "Number of neurons in the brain"));
        
        schemas.push_back(ConfigSchema("region_count", "int64_t", ConfigSource::SchemaDefault,
                                       static_cast<int64_t>(1), "Number of neural regions"));
        
        schemas.push_back(ConfigSchema("connection_probability", "double", ConfigSource::SchemaDefault,
                                       0.1f, "Probability of creating synapses between neurons"));
        
        schemas.push_back(ConfigSchema("stdp_ltp_weight", "double", ConfigSource::SchemaDefault,
                                       0.02f, "STDP LTP weight factor"));
        
        schemas.push_back(ConfigSchema("stdp_ltd_weight", "double", ConfigSource::SchemaDefault,
                                       0.015f, "STDP LTD weight factor"));
        
        schemas.push_back(ConfigSchema("stdp_tau", "double", ConfigSource::SchemaDefault,
                                       20.0f, "STDP time constant in simulation steps"));
        
        schemas.push_back(ConfigSchema("synaptogenesis_rate", "double", ConfigSource::SchemaDefault,
                                       0.0001f, "Rate of new synapse formation"));
        
        schemas.push_back(ConfigSchema("pruning_rate", "double", ConfigSource::SchemaDefault,
                                       0.00001f, "Rate of synapse removal"));
        
        schemas.push_back(ConfigSchema("replay_interval", "int64_t", ConfigSource::SchemaDefault,
                                       static_cast<int64_t>(100), "Memory replay interval in steps"));
        
        schemas.push_back(ConfigSchema("consolidation_interval", "int64_t", ConfigSource::SchemaDefault,
                                       static_cast<int64_t>(1000), "Memory consolidation interval in steps"));
        
        schemas.push_back(ConfigSchema("checkpoint_dir", "string", ConfigSource::SchemaDefault,
                                       "./checkpoints", "Directory for brain state checkpoints"));
        
        schemas.push_back(ConfigSchema("checkpoint_interval", "int64_t", ConfigSource::SchemaDefault,
                                       static_cast<int64_t>(10000), "Checkpoint save interval in steps"));
        
        schemas.push_back(ConfigSchema("checkpoint_compression", "bool", ConfigSource::SchemaDefault,
                                       true, "Enable checkpoint compression"));
        
        // Required core configuration
        schemas.push_back(ConfigSchema("brain_name", "string", ConfigSource::SchemaDefault,
                                       "NLM_Brain", "Name of the brain instance", "", true));
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists and get modification time
    std::error_code ec;
    if (!std::filesystem::exists(filepath, ec)) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    auto lastWriteTime = std::filesystem::last_write_time(filepath, ec);
    if (ec) {
        NLM_LOG_WARNING("Failed to get file modification time: " + ec.message());
    }
    
    // Check if file has been modified since last load
    bool fileModified = (lastModifiedTime == decltype(lastModifiedTime){}) || 
                       (lastWriteTime > lastModifiedTime);
    
    if (!fileModified) {
        NLM_LOG_DEBUG("Configuration file not modified: " + filepath);
        return false;
    }
    
    try {
        // Try to parse as JSON first
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
            return false;
        }
        
        std::string jsonContent((std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>());
        
        // Try to parse as JSON
        try {
            json j = json::parse(jsonContent);
            loadFromJSON(jsonContent);
            pImpl->lastLoadedFile = filepath;
            pImpl->lastModifiedTime = std::filesystem::last_write_time(filepath);
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
            return true;
        } catch (const json::parse_error& e) {
            // Fall back to old key=value format
            file.clear();
            file.seekg(0, std::ios::beg);
            
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
            
            pImpl->lastLoadedFile = filepath;
            pImpl->lastModifiedTime = std::filesystem::last_write_time(filepath);
            NLM_LOG_INFO("Loaded configuration from legacy format file: " + filepath);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading configuration file: ") + e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                try {
                    setFromJSON(key, json(value), ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR(std::string("Invalid command line argument " + arg + ": ") + e.what());
                    success = false;
                }
            } else {
                NLM_LOG_WARNING("Malformed command line argument (no =): " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            try {
                setFromJSON(key, json(value), ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR(std::string("Invalid command line argument " + key + " " + value + ": ") + e.what());
                success = false;
            }
        }
    }
    return success;
}

bool Config::loadFromJSON(const std::string& jsonString) {
    try {
        json j = json::parse(jsonString);
        
        // Apply schema defaults first
        for (const auto& schema : pImpl->schemas) {
            if (!has(schema.key)) {
                if (!std::holds_alternative<std::string>(schema.defaultValue) || 
                    !std::get<std::string>(schema.defaultValue).empty()) {
                    set(schema.key, schema.defaultValue, ConfigSource::SchemaDefault);
                }
            }
        }
        
        // Now apply JSON values (overriding defaults)
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            try {
                setFromJSON(key, it.value(), ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING(std::string("Failed to parse config key " + key + ": ") + e.what());
                // Continue with other keys - don't fail entire load for one bad key
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from JSON string");
        return true;
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR(std::string("JSON parse error: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception loading from JSON: ") + e.what());
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        json j;
        
        for (const auto& entry : pImpl->entries) {
            // Only save entries with values (not default empty strings)
            if (!std::holds_alternative<std::string>(entry.value) || 
                !std::get<std::string>(entry.value).empty()) {
                j[entry.key] = configValueToJSON(entry.value);
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
            return false;
        }
        
        // Write with indentation for readability
        file << std::setw(2) << j << std::endl;
        
        NLM_LOG_INFO(std::string("Configuration saved to: ") + filepath);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving configuration file: ") + e.what());
        return false;
    }
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
        NLM_LOG_ERROR(std::string("Type mismatch getting config key " + key));
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate against schema if available
    if (hasSchema(key) && !validateValue(key, value)) {
        throw ConfigException("Configuration value validation failed for key: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry if this source has higher priority
        if (pImpl->sourcePriority.find(source) != pImpl->sourcePriority.end() &&
            pImpl->sourcePriority[source] < pImpl->sourcePriority[it->source]) {
            it->value = value;
            it->source = source;
            NLM_LOG_DEBUG(std::string("Updated configuration key " + key + " from " + 
                                   sourceToString(it->source) + " to " + sourceToString(source)));
        } else {
            NLM_LOG_DEBUG(std::string("Ignored configuration key " + key + 
                                   " (source " + sourceToString(source) + " has lower priority than " + 
                                   sourceToString(it->source) + ")"));
        }
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG(std::string("Added new configuration key: " + key + " = " + 
                               valueToString(value)));
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

void Config::setFromJSON(const std::string& key, const json& j, ConfigSource source) {
    ConfigValue value;
    if (jsonToConfigValue(j, value)) {
        set(key, value, source);
    } else {
        throw ConfigException("Cannot convert JSON value to config type for key: " + key);
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    pImpl->entries.erase(it, pImpl->entries.end());
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
    NLM_LOG_INFO("Configuration cleared");
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << 
           pImpl->schemas.size() << " schemas):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [" << valueToString(entry.value) << "] (" << 
               static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        if (!entry.validationRegex.empty()) {
            oss << " [regex: " << entry.validationRegex << "]";
        }
        oss << "\n";
    }
    
    return oss.str();
}

void Config::addSchema(const ConfigSchema& schema) {
    // Check if schema already exists
    auto it = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&schema](const ConfigSchema& s) { return s.key == schema.key; });n    
    if (it != pImpl->schemas.end()) {
        *it = schema; // Update existing schema
    } else {
        pImpl->schemas.push_back(schema);
    }
}

bool Config::validateSchema() const {
    std::string errors;
    for (const auto& schema : pImpl->schemas) {
        if (schema.required && !has(schema.key)) {
            errors += "Required config key missing: " + schema.key + "\n";
        }
        
        if (has(schema.key)) {
            auto entry = getConfigEntry(schema.key);
            if (!schema.isValid(entry.value)) {
                errors += "Invalid value for config key " + schema.key + " (expected type: " + 
                         schema.type + ")\n";
            }
        }
    }
    
    if (!errors.empty()) {
        NLM_LOG_ERROR(std::string("Schema validation failed:\n" + errors));
        return false;
    }
    
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    auto it = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (it != pImpl->schemas.end()) {
        return it->isValid(value);
    }
    
    // If no schema, check basic type consistency
    for (const auto& schema : pImpl->schemas) {
        if (schema.key == key) {
            return schema.isValid(value);
        }
    }
    
    return true; // No schema means no validation
}

void Config::merge(const Config& other, bool override) {
    for (const auto& entry : other.getEntries()) {
        if (override || !has(entry.key)) {
            set(entry.key, entry.value, entry.source);
        }
    }
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherEntry = other.getConfigEntry(entry.key);
        if (otherEntry.key != entry.key || otherEntry.value != entry.value || 
            otherEntry.source != entry.source) {
            return false;
        }
    }
    
    return true;
}

void Config::updateFromJSON(const std::string& jsonString) {
    loadFromJSON(jsonString);
}

void Config::reloadFromFile(const std::string& filepath) {
    clear();
    loadFromFile(filepath);
}

std::string Config::getConfigErrors() const {
    std::string errors;
    for (const auto& entry : pImpl->entries) {
        if (!entry.isValid()) {
            errors += "Invalid configuration: " + entry.key + "\n";
        }
    }
    
    // Check for missing required schema items
    for (const auto& schema : pImpl->schemas) {
        if (schema.required && !has(schema.key)) {
            errors += "Missing required configuration: " + schema.key + "\n";
        }
    }
    
    return errors;
}

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
}

// Static helper functions
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

std::string Config::getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vec_int";
    if (std::holds_alternative<std::vector<double>>(value)) return "vec_double";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vec_string";
    return "unknown";
}

json Config::configValueToJSON(const ConfigValue& value) {
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

bool Config::jsonToConfigValue(const json& j, ConfigValue& value) {
    if (j.is_number_integer()) {
        value = j.get<int>();
        return true;
    } else if (j.is_number_unsigned()) {
        value = static_cast<int64_t>(j.get<uint64_t>());
        return true;
    } else if (j.is_number_float()) {
        value = j.get<double>();
        return true;
    } else if (j.is_boolean()) {
        value = j.get<bool>();
        return true;
    } else if (j.is_string()) {
        value = j.get<std::string>();
        return true;
    } else if (j.is_array()) {
        // Try to infer array type
        if (!j.empty()) {
            if (j[0].is_number_integer()) {
                value = j.get<std::vector<int>>();
                return true;
            } else if (j[0].is_number_float()) {
                value = j.get<std::vector<double>>();
                return true;
            } else if (j[0].is_string()) {
                value = j.get<std::vector<std::string>>();
                return true;
            }
        }
        // Empty array - default to vector<string>
        value = std::vector<std::string>();
        return true;
    }
    return false;
}

// Helper functions not in class
std::string sourceToString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        case ConfigSource::SchemaDefault: return "SchemaDefault";
        default: return "Unknown";
    }
}

std::string valueToString(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
        return oss.str();
    }
    return "unknown";
}

const ConfigEntry& Config::getConfigEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    static const ConfigEntry emptyEntry;
    return (it != pImpl->entries.end()) ? *it : emptyEntry;
}

bool Config::hasSchema(const std::string& key) const {
    return std::any_of(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
}

} // namespace nlm
