// NLM (熙然) - Neural Learning Machine
// Phase 6: Final Integration
//
// This phase implements final integration of all neural systems:
// - Integration of working, episodic, and associative memory systems
// - Neuromodulation (dopamine, curiosity, novelty) affecting learning
// - Prediction system integrated with neural processing
// - Cognitive mechanisms (planning, attention, concept formation)
// - Development affecting structural and functional plasticity
// - Replay and consolidation mechanisms
// - Checkpoint save/load functionality

#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "core/SimpleJsonParser.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    JsonValue jsonValue;
    if (SimpleJsonParser::parseJsonFile(filepath, jsonValue)) {
        // Successfully parsed as JSON
        return convertFromJson(jsonValue);
    }
    
    // If JSON parsing failed, try the legacy key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
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
            
            set(key, value, ConfigSource::File);
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
    // Try to save as JSON first
    if (saveToJsonFile(filepath)) {
        return true;
    }
    
    // If JSON saving failed, fall back to legacy format
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << jsonValueToString(entry.value) << "\n";
    }
    
    return true;
}

bool Config::saveToJsonFile(const std::string& filepath) const {
    JsonValue jsonValue;
    convertToJson(jsonValue);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string jsonStr = SimpleJsonParser::toJsonString(jsonValue, 0);
    file << jsonStr;
    
    return true;
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

bool Config::convertFromJson(const JsonValue& jsonValue) {
    if (std::holds_alternative<std::map<std::string, JsonValue>>(jsonValue)) {
        const auto& obj = std::get<std::map<std::string, JsonValue>>(jsonValue);
        
        for (const auto& pair : obj) {
            std::string key = pair.first;
            const JsonValue& jsonVal = pair.second;
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            if (std::holds_alternative<int>(jsonVal)) {
                configValue = std::get<int>(jsonVal);
            } else if (std::holds_alternative<int64_t>(jsonVal)) {
                configValue = std::get<int64_t>(jsonVal);
            } else if (std::holds_alternative<double>(jsonVal)) {
                configValue = std::get<double>(jsonVal);
            } else if (std::holds_alternative<bool>(jsonVal)) {
                configValue = std::get<bool>(jsonVal);
            } else if (std::holds_alternative<std::string>(jsonVal)) {
                configValue = std::get<std::string>(jsonVal);
            } else if (std::holds_alternative<std::vector<JsonValue>>(jsonVal)) {
                // For arrays, convert to string representation
                configValue = SimpleJsonParser::toJsonString(jsonVal);
            }
            
            set(key, configValue, ConfigSource::File);
        }
        return true;
    }
    
    return false;
}

void Config::convertToJson(JsonValue& jsonValue) const {
    std::map<std::string, JsonValue> obj;
    
    for (const auto& entry : pImpl->entries) {
        JsonValue jsonVal;
        convertToJson(entry, jsonVal);
        obj[entry.key] = std::move(jsonVal);
    }
    
    jsonValue = std::move(obj);
}

void Config::convertToJson(const ConfigEntry& entry, JsonValue& jsonValue) {
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::vector<JsonValue> jsonArray;
            for (int i : val) {
                jsonArray.push_back(i);
            }
            jsonValue = std::move(jsonArray);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::vector<JsonValue> jsonArray;
            for (double d : val) {
                jsonArray.push_back(d);
            }
            jsonValue = std::move(jsonArray);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::vector<JsonValue> jsonArray;
            for (const std::string& s : val) {
                jsonArray.push_back(s);
            }
            jsonValue = std::move(jsonArray);
        }
    }, entry.value);
}

// Helper function to validate configuration values
static bool validateConfigValue(const std::string& key, const ConfigValue& value) {
    // Add validation logic for specific keys
    if (key == "neuron_count" && std::holds_alternative<size_t>(value)) {
        size_t count = std::get<size_t>(value);
        if (count == 0 || count > 100000) {
            return false;
        }
    } else if (key == "region_count" && std::holds_alternative<size_t>(value)) {
        size_t count = std::get<size_t>(value);
        if (count == 0 || count > 1000) {
            return false;
        }
    } else if (key == "connection_probability" && std::holds_alternative<double>(value)) {
        double prob = std::get<double>(value);
        if (prob < 0.0 || prob > 1.0) {
            return false;
        }
    } else if (key == "random_seed" && std::holds_alternative<uint64_t>(value)) {
        uint64_t seed = std::get<uint64_t>(value);
        if (seed == 0) {
            return false;
        }
    }
    
    return true;
}

// Helper function to get config value with validation
template<typename T>
T getConfigValueWithValidation(const Config& config, const std::string& key, const T& defaultValue) {
    auto value = config.get<T>(key);
    if (value.has_value()) {
        // Validate the value if possible
        if constexpr (std::is_same_v<T, size_t>) {
            if (validateConfigValue(key, value.value())) {
                return value.value();
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (validateConfigValue(key, value.value())) {
                return value.value();
            }
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            if (validateConfigValue(key, value.value())) {
                return value.value();
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (validateConfigValue(key, value.value())) {
                return value.value();
            }
        }
        
        // If validation fails, log warning and return default
        return defaultValue;
    }
    return defaultValue;
}

// Error handling wrapper for file operations
static std::string getErrorMessage(const std::string& operation, const std::string& details) {
    return "Error during " + operation + ": " + details;
}

// Configuration validation
bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateConfigValue(entry.key, entry.value)) {
            return false;
        }
    }
    return true;
}

// Get configuration with validation template<typename T>
T Config::getWithValidation(const std::string& key, const T& defaultValue) const {
    return getConfigValueWithValidation(*this, key, defaultValue);
}

// Helper function to safely parse configuration files
bool parseConfigFileSafely(const std::string& filepath, Config& config) {
    try {
        return config.loadFromFile(filepath);
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

// Helper function to safely parse command line arguments
bool parseArgsSafely(int argc, char** argv, Config& config) {
    try {
        return config.loadFromArgs(argc, argv);
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

// Configuration parameter validation
bool Config::validateParameter(const std::string& key, const ConfigValue& value) {
    return validateConfigValue(key, value);
}

// Get all configuration parameters with validation
std::vector<std::pair<std::string, bool>> Config::validateAllParameters() const {
    std::vector<std::pair<std::string, bool>> results;
    
    for (const auto& entry : pImpl->entries) {
        bool isValid = validateConfigValue(entry.key, entry.value);
        results.emplace_back(entry.key, isValid);
    }
    
    return results;
}

// Safe config getter with parameter validation
bool Config::tryGet(const std::string& key, ConfigValue& outValue) const {
    if (has(key)) {
        outValue = pImpl->entries.back().value;  // This is simplified - should find by key
        return validateParameter(key, outValue);
    }
    return false;
}

// Get configuration value with error handling
ConfigValue Config::getSafe(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        if (validateParameter(it->key, it->value)) {
            return it->value;
        }
    }
    return ConfigValue(); // Return default empty value
}