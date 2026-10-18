#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <memory>

namespace nlm {

// Configuration validation schemas and constants
namespace ConfigValidation {
    
    // Configuration key validation
    inline bool isValidKey(const std::string& key) {
        return !key.empty() && std::all_of(key.begin(), key.end(), 
            [](char c) { return std::isalnum(c) || c == '_' || c == '-'; });
    }
    
    // Configuration value validation
    inline bool validateValue(const ConfigValue& value) {
        return !std::holds_alternative<std::monostate>(value);
    }
    
    // Configuration source priority
    inline int getSourcePriority(ConfigSource source) {
        switch (source) {
            case ConfigSource::Runtime: return 4;
            case ConfigSource::CommandLine: return 3;
            case ConfigSource::File: return 2;
            case ConfigSource::Default: return 1;
            default: return 0;
        }
    }
    
} // namespace ConfigValidation

// Forward declaration for JSON helper functions
class ConfigJsonHelper;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSource> keySources;  // Track sources for each key
    size_t maxEntries;
    bool strictMode;
    std::shared_ptr<ConfigJsonHelper> jsonHelper;
    
    Impl()
        : entries()
        , keySources()
        , maxEntries(1000)  // Prevent excessive memory usage
        , strictMode(false)
        , jsonHelper(nullptr) {
    }
    
    // Validate and add entry safely
    bool addEntry(const ConfigEntry& entry) {
        if (!ConfigValidation::isValidKey(entry.key)) {
            return false;
        }
        
        if (!ConfigValidation::validateValue(entry.value)) {
            return false;
        }
        
        if (entries.size() >= maxEntries) {
            return false;
        }
        
        // Check for duplicates
        auto it = keySources.find(entry.key);
        if (it != keySources.end()) {
            // Keep higher priority source
            if (ConfigValidation::getSourcePriority(entry.source) <= 
                ConfigValidation::getSourcePriority(it->second)) {
                return false;
            }
        }
        
        entries.push_back(entry);
        keySources[entry.key] = entry.source;
        return true;
    }
    
    // Remove entry and source tracking
    void removeEntry(const std::string& key) {
        auto it = keySources.find(key);
        if (it != keySources.end()) {
            keySources.erase(it);
        }
        
        entries.erase(
            std::remove_if(entries.begin(), entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            entries.end()
        );
    }
    
    // Merge configuration from another config
    void merge(const Config& other) {
        for (const auto& entry : other.pImpl->entries) {
            addEntry(entry);
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration
    initializeDefaults();
}

Config::~Config() = default;

void Config::initializeDefaults() {
    // Set default values for common configuration options
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    
    // STDP parameters
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory system parameters
    set("working_memory_capacity", 100, ConfigSource::Default);
    set("episodic_memory_max_episodes", 1000, ConfigSource::Default);
    set("replay_interval", 100, ConfigSource::Default);
    set("consolidation_interval", 1000, ConfigSource::Default);
    
    // Cognitive system parameters
    set("planning_depth", 5, ConfigSource::Default);
    set("attention_inhibition_strength", 0.5f, ConfigSource::Default);
    set("attention_excitation_strength", 1.5f, ConfigSource::Default);
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first
    if (loadFromJson(filepath)) {
        NLM_LOG_DEBUG("Loaded configuration from JSON file: " + filepath);
        return true;
    }
    
    // Fall back to YAML
    if (loadFromYaml(filepath)) {
        NLM_LOG_DEBUG("Loaded configuration from YAML file: " + filepath);
        return true;
    }
    
    // Fall back to simple key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNum = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        
        // Skip empty lines and comments
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
            
            if (!setFromString(key, value, ConfigSource::File)) {
                NLM_LOG_WARNING("Invalid configuration at line " + std::to_string(lineNum) + ": " + key);
                success = false;
            }
        } else {
            NLM_LOG_WARNING("Invalid configuration format at line " + std::to_string(lineNum));
            success = false;
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Loaded configuration from simple file: " + filepath);
    }
    
    return success;
}

bool Config::loadFromJson(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        return loadFromJsonInternal(jsonData, ConfigSource::File);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYaml(const std::string& filepath) {
    // For now, fall back to simple parsing
    // In a full implementation, this would use a YAML library
    return false;
}

bool Config::loadFromJsonInternal(const nlohmann::json& json, ConfigSource source) {
    if (!json.is_object()) {
        return false;
    }
    
    bool success = true;
    
    for (const auto& item : json.items()) {
        const std::string& key = item.key();
        const nlohmann::json& value = item.value();
        
        std::string error;
        if (!setFromJsonValue(key, value, source, error)) {
            NLM_LOG_WARNING("Invalid JSON configuration key '" + key + "': " + error);
            success = false;
        }
    }
    
    return success;
}

bool Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Parse string value to appropriate type
    try {
        // Try to detect type
        if (valueStr == "true" || valueStr == "false") {
            bool val = (valueStr == "true");
            set(key, val, source);
            return true;
        }
        
        // Try integer
        try {
            size_t pos;
            int64_t intVal = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, intVal, source);
                return true;
            }
        } catch (...) {}
        
        // Try float/double
        try {
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, doubleVal, source);
                return true;
            }
        } catch (...) {}
        
        // Default to string
        set(key, valueStr, source);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing configuration value for key '" + key + "': " + std::string(e.what()));
        return false;
    }
}

bool Config::setFromJsonValue(const std::string& key, const nlohmann::json& value, 
                             ConfigSource source, std::string& error) {
    try {
        if (value.is_null()) {
            error = "Null value not allowed";
            return false;
        }
        
        if (value.is_boolean()) {
            set(key, value.get<bool>(), source);
            return true;
        } else if (value.is_number_integer()) {
            set(key, value.get<int64_t>(), source);
            return true;
        } else if (value.is_number_float()) {
            set(key, value.get<double>(), source);
            return true;
        } else if (value.is_string()) {
            set(key, value.get<std::string>(), source);
            return true;
        } else if (value.is_array()) {
            // Convert array to appropriate vector type based on content
            if (value.empty()) {
                error = "Empty arrays not supported";
                return false;
            }
            
            // Check if all elements are integers
            bool allInts = std::all_of(value.begin(), value.end(), 
                                       [](const nlohmann::json& v) { return v.is_number_integer(); });
            if (allInts) {
                std::vector<int64_t> intArray;
                for (const auto& v : value) {
                    intArray.push_back(v.get<int64_t>());
                }
                set(key, intArray, source);
                return true;
            }
            
            // Check if all elements are floats
            bool allFloats = std::all_of(value.begin(), value.end(),
                                         [](const nlohmann::json& v) { return v.is_number_float(); });
            if (allFloats) {
                std::vector<double> floatArray;
                for (const auto& v : value) {
                    floatArray.push_back(v.get<double>());
                }
                set(key, floatArray, source);
                return true;
            }
            
            // Check if all elements are strings
            bool allStrings = std::all_of(value.begin(), value.end(),
                                          [](const nlohmann::json& v) { return v.is_string(); });
            if (allStrings) {
                std::vector<std::string> stringArray;
                for (const auto& v : value) {
                    stringArray.push_back(v.get<std::string>());
                }
                set(key, stringArray, source);
                return true;
            }
            
            error = "Unsupported array type combination";
            return false;
        } else {
            error = "Unsupported JSON type";
            return false;
        }
        
    } catch (const std::exception& e) {
        error = std::string(e.what());
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    // Determine file format based on extension
    std::string ext = getFileExtension(filepath);
    
    if (ext == "json") {
        return saveToJson(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return saveToYaml(filepath);
    } else {
        // Default to simple key=value format
        return saveToSimpleFile(filepath);
    }
}

bool Config::saveToJson(const std::string& filepath) const {
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = convertToJsonValue(entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open file for writing: " + filepath);
            return false;
        }
        
        // Write with pretty formatting
        file << std::setw(2) << jsonData;
        file.flush();
        
        NLM_LOG_INFO("Saved configuration to JSON file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to JSON: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToYaml(const std::string& filepath) const {
    // For now, fall back to simple file format
    // In a full implementation, this would use a YAML library
    return saveToSimpleFile(filepath);
}

bool Config::saveToSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated on: " << getCurrentTimestamp() << std::endl;
    file << "# Format: key = value" << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << std::endl;
        file << entry.key << " = " << convertToStringValue(entry.value) << std::endl;
        file << "# Source: " << static_cast<int>(entry.source) << std::endl;
        file << std::endl;
    }
    
    file.flush();
    NLM_LOG_INFO("Saved configuration to simple file: " + filepath);
    return true;
}

nlohmann::json Config::convertToJsonValue(const ConfigValue& value) const {
    nlohmann::json result;
    
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : arg) {
                array.push_back(item);
            }
            result = array;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : arg) {
                array.push_back(item);
            }
            result = array;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : arg) {
                array.push_back(item);
            }
            result = array;
        } else {
            result = "null";
        }
    }, value);
    
    return result;
}

std::string Config::convertToStringValue(const ConfigValue& value) const {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i]; if (i < arg.size() - 1) oss << ", ";
            } oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i]; if (i < arg.size() - 1) oss << ", ";
            } oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "["; for (size_t i = 0; i < arg.size(); ++i) {
                oss << "\"" << arg[i] << "\""; if (i < arg.size() - 1) oss << ", ";
            } oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    
    return oss.str();
}

std::string Config::getFileExtension(const std::string& filename) const {
    size_t pos = filename.rfind('.');
    if (pos == std::string::npos) {
        return "";
    }
    
    std::string ext = filename.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

std::string Config::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!ConfigValidation::isValidKey(entry.key)) {
            NLM_LOG_ERROR("Invalid configuration key: " + entry.key);
            return false;
        }
        
        if (!ConfigValidation::validateValue(entry.value)) {
            NLM_LOG_ERROR("Invalid configuration value for key: " + entry.key);
            return false;
        }
    }
    return true;
}

bool Config::validate(const ConfigSchema& schema) const {
    std::vector<std::string> errors;
    return schema.validate(*this, errors);
}

std::string Config::getConfigSummary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):" << std::endl;
    
    // Sort by source priority for better readability
    std::vector<ConfigEntry> sortedEntries = pImpl->entries;
    std::sort(sortedEntries.begin(), sortedEntries.end(),
              [](const ConfigEntry& a, const ConfigEntry& b) {
                  return ConfigValidation::getSourcePriority(a.source) >
                         ConfigValidation::getSourcePriority(b.source);
              });
    
    for (const auto& entry : sortedEntries) {
        oss << "  " << entry.key << " = " << convertToStringValue(entry.value)
            << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    return oss.str();
}

Config& Config::merge(const Config& other) {
    pImpl->merge(other);
    return *this;
}

Config& Config::merge(const std::string& jsonFile) {
    Config other;
    if (other.loadFromFile(jsonFile)) {
        merge(other);
    }
    return *this;
}

bool Config::equals(const Config& other) const {
    return pImpl->entries.size() == other.pImpl->entries.size() &&
           std::equal(pImpl->entries.begin(), pImpl->entries.end(),
                     other.pImpl->entries.begin(),
                     [](const ConfigEntry& a, const ConfigEntry& b) {
                         return a.key == b.key && a.value == b.value && a.source == b.source;
                     });
}

// Add missing method implementations
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

// Add missing getOr method implementation for all types
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Helper class implementation
class ConfigJsonHelper {
public:
    static bool validateJsonSchema(const nlohmann::json& json) {
        // Simple schema validation
        return json.is_object();
    }
    
    static std::string getSchemaDescription() {
        return "NLM Configuration Schema: {key: value, ...}";
    }
};

} // namespace nlm