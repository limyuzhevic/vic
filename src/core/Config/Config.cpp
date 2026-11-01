#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigChangeCallback> changeCallbacks;
    std::unordered_map<std::string, std::string> groups;  // For group support
    bool validationEnabled;
    
    Impl() : validationEnabled(true) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default, "Random seed for reproducible experiments");
    set("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in seconds");
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default, "Number of neurons in the brain");
    set("region_count", static_cast<int64_t>(2), ConfigSource::Default, "Number of brain regions");
    set("connection_probability", 0.1f, ConfigSource::Default, "Connection probability for random connectivity");
    set("plasticity_learning_rate", 0.01, ConfigSource::Default, "General plasticity learning rate");
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validateSchema) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open config file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return loadFromFile(content, validateSchema);
}

bool Config::loadFromFile(const std::string& content, bool validateSchema) {
    // Try JSON first
    try {
        auto jsonData = json::parse(content);
        return loadFromJson(jsonData, validateSchema);
    } catch (...) {
        // Try YAML
        // For now, fall back to simple key=value parsing for compatibility
    }
    
    // Fallback to simple key=value parsing (original implementation)
    std::istringstream stream(content);
    std::string line;
    int lineNum = 0;
    
    while (std::getline(stream, line)) {
        ++lineNum;
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

bool Config::loadFromYaml(const std::string& filepath, bool validateSchema) {
    // YAML parsing would go here
    // For now, delegate to loadFromFile with validation
    return loadFromFile(filepath, validateSchema);
}

bool Config::loadFromEnvironment(const std::string& prefix, bool validateKeys) {
    bool success = true;
    for (const auto& envEntry : std::filesystem::environment()) {
        std::string key = envEntry.first;
        std::string value = envEntry.second;
        
        if (key.find(prefix) == 0) {
            std::string configKey = key.substr(prefix.length());
            
            ConfigValue configValue;
            if (parseEnvVar(value, configValue)) {
                set(configKey, configValue, ConfigSource::Environment, 
                    "Environment variable: " + key);
            } else {
                NLM_LOG_WARNING("Could not parse environment variable: " + key);
                success = false;
            }
        }
    }
    return success;
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
        // Handle --flag (boolean)
        else if (arg.substr(0, 2) == "--" && arg.find('=') == std::string::npos) {
            std::string key = arg.substr(2);
            set(key, true, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return saveToJson(filepath, prettyPrint);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYaml(filepath, prettyPrint);
    } else {
        // Default to simple key=value format
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else if constexpr (std::is_arithmetic_v<T>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else {
                    file << "";
                }
            }, entry.value);
            
            file << "\n\n";
        }
        return true;
    }
}

bool Config::saveToJson(const std::string& filepath, bool prettyPrint) const {
#ifndef HAS_NLOHMANN_JSON
    NLM_LOG_ERROR("JSON saving requires nlohmann/json library");
    return false;
#endif
    
    try {
        json j;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&j, &entry](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    j[entry.key] = val;
                } else if constexpr (std::is_arithmetic_v<T>) {
                    j[entry.key] = val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    j[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    j[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    j[entry.key] = val;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    j[entry.key] = val;
                }
            }, entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (prettyPrint) {
            file << j.dump(2);
        } else {
            file << j.dump();
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving JSON config: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToYaml(const std::string& filepath, bool prettyPrint) const {
    // YAML saving would require a YAML library
    // For now, fall back to JSON
    return saveToJson(filepath, prettyPrint);
}

// Template implementations for get() and getOr()
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else if constexpr (std::is_same_v<V, std::string> && std::is_same_v<T, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<V, bool> && std::is_same_v<T, bool>) {
            return v;
        } else if constexpr (std::is_same_v<V, std::vector<int>> && std::is_same_v<T, std::vector<int>>) {
            return v;
        } else if constexpr (std::is_same_v<V, std::vector<double>> && std::is_same_v<T, std::vector<double>>) {
            return v;
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
bool Config::tryGet(const std::string& key, T& out) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        out = val.value();
        return true;
    }
    return false;
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, const std::string& description) {
    ConfigValue oldValue;
    bool exists = has(key);
    
    if (exists) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        oldValue = it->value;
    }
    
    // Set or update the entry
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt != pImpl->entries.end()) {
        entryIt->value = value;
        entryIt->source = source;
        entryIt->description = description.empty() ? entryIt->description : description;
        entryIt->timestamp = std::chrono::system_clock::now();
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
    }
    
    // Call change callbacks
    if (exists && std::visit([](const auto& v1, const auto& v2) { return v1 != v2; }, oldValue, value)) {
        for (const auto& callback : pImpl->changeCallbacks) {
            try {
                callback(key, oldValue, value);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Config change callback error: " + std::string(e.what()));
            }
        }
    }
}

void Config::set(const std::string& key, const std::string& value, 
                 ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, int value, 
                 ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, int64_t value, 
                 ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, double value, 
                 ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, bool value, 
                 ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

bool Config::validate() const {
    if (!pImpl->validationEnabled) return true;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key)) {
            NLM_LOG_WARNING("Invalid config key: " + entry.key);
            continue;
        }
        
        if (!validateValue(entry.key, entry.value)) {
            NLM_LOG_WARNING("Invalid config value for key: " + entry.key);
            return false;
        }
    }
    
    return true;
}

bool Config::validateKey(const std::string& key) const {
    // Basic validation: non-empty and reasonable characters
    if (key.empty() || key.find(' ') != std::string::npos) {
        return false;
    }
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Basic validation based on key
    if (key == "random_seed") {
        auto val = get<int64_t>(key);
        if (val.has_value() && (*val < 0)) {
            return false;
        }
    } else if (key == "simulation_timestep") {
        auto val = get<double>(key);
        if (val.has_value() && (*val <= 0.0)) {
            return false;
        }
    } else if (key == "neuron_count") {
        auto val = get<int64_t>(key);
        if (val.has_value() && (*val <= 0)) {
            return false;
        }
    } else if (key == "connection_probability") {
        auto val = get<double>(key);
        if (val.has_value() && ((*val < 0.0) || (*val > 1.0))) {
            return false;
        }
    }
    
    return true;
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    return ConfigSource::Default;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->description;
    }
    return "";
}

std::chrono::system_clock::time_point Config::getTimestamp(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->timestamp;
    }
    return std::chrono::system_clock::time_point();
}

std::string Config::getSourceString(ConfigSource source) const {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "Command Line";
        case ConfigSource::Runtime: return "Runtime";
        case ConfigSource::Environment: return "Environment";
        default: return "Unknown";
    }
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    ConfigValue oldValue;
    bool exists = has(key);
    
    if (exists) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        oldValue = it->value;
        pImpl->entries.erase(it);
        
        // Call change callbacks
        for (const auto& callback : pImpl->changeCallbacks) {
            try {
                callback(key, oldValue, ConfigValue());
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Config change callback error: " + std::string(e.what()));
            }
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getSortedKeys() const {
    std::vector<std::string> keys = getKeys();
    std::sort(keys.begin(), keys.end());
    return keys;
}

std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    return summary(true, false);
}

std::string Config::summary(bool includeSources, bool includeTimestamps) const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else if constexpr (std::is_arithmetic_v<T>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (val ? "true" : "false");
            } else {
                oss << "";
            }
        }, entry.value);
        oss << "]";
        
        if (includeSources) {
            oss << " (" << static_cast<int>(entry.source) << ")";
        }
        
        if (includeTimestamps) {
            auto time = std::chrono::system_clock::to_time_t(entry.timestamp);
            oss << " @ " << std::ctime(&time);
        }
        
        oss << "  # " << entry.description << "\n";
    }
    
    return oss.str();
}

void Config::addChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.push_back(callback);
}

void Config::removeChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.erase(
        std::remove(pImpl->changeCallbacks.begin(), pImpl->changeCallbacks.end(), callback),
        pImpl->changeCallbacks.end());
}

void Config::clearChangeCallbacks() {
    pImpl->changeCallbacks.clear();
}

std::shared_ptr<Config> Config::clone() const {
    auto clone = std::make_shared<Config>();
    clone->pImpl->entries = pImpl->entries;
    clone->pImpl->changeCallbacks = pImpl->changeCallbacks;
    clone->pImpl->validationEnabled = pImpl->validationEnabled;
    return clone;
}

void Config::merge(const Config& other, bool overwrite) {
    for (const auto& otherEntry : other.pImpl->entries) {
        if (overwrite || !has(otherEntry.key)) {
            set(otherEntry.key, otherEntry.value, otherEntry.source, otherEntry.description);
        }
    }
}

bool Config::validateRules(const std::vector<ValidationRule>& rules) const {
    for (const auto& rule : rules) {
        if (!has(rule.key)) {
            NLM_LOG_WARNING("Config validation rule failed: key '" + rule.key + "' not found");
            return false;
        }
        
        auto value = get<ConfigValue>(rule.key);
        if (!value.has_value() || !rule.validator(value.value())) {
            NLM_LOG_WARNING("Config validation rule failed for key '" + rule.key + "': " + rule.errorMessage);
            return false;
        }
    }
    
    return true;
}

void Config::beginGroup(const std::string& groupName) {
    pImpl->groups[groupName] = getFullKey("");
}

void Config::endGroup() {
    if (!pImpl->groups.empty()) {
        pImpl->groups.erase(pImpl->groups.begin());
    }
}

bool Config::inGroup() const {
    return !pImpl->groups.empty();
}

std::string Config::getCurrentGroup() const {
    if (!pImpl->groups.empty()) {
        return pImpl->groups.rbegin()->first;
    }
    return "";
}

std::string Config::getFullKey(const std::string& key) const {
    std::string fullKey = key;
    if (inGroup()) {
        fullKey = getCurrentGroup() + "." + key;
    }
    return fullKey;
}

bool Config::saveProfile(const std::string& name, const std::string& directory) {
    std::filesystem::path dir(directory);
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
    
    std::string filepath = (dir / (name + ".json")).string();
    return saveToJson(filepath, true);
}

bool Config::loadProfile(const std::string& name, const std::string& directory) {
    std::filesystem::path dir(directory);
    std::string filepath = (dir / (name + ".json")).string();
    return loadFromFile(filepath, true);
}

std::vector<std::string> Config::getAvailableProfiles() const {
    std::vector<std::string> profiles;
    std::filesystem::path dir("profiles");
    
    if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (ext == ".json" || ext == ".yaml" || ext == ".cfg") {
                    profiles.push_back(entry.path().stem().string());
                }
            }
        }
    }
    
    return profiles;
}

// JSON implementation
#ifndef HAS_NLOHMANN_JSON
bool Config::jsonParse(const std::string& str, JsonValue& out) {
    // Simple JSON parser for basic types
    // This is a very basic implementation for demonstration
    if (str.empty() || str[0] != '{') return false;
    
    out.type = JsonValue::Type::Object;
    size_t pos = 1;
    while (pos < str.length() && str[pos] != '}') {
        // Parse key-value pair
        size_t keyEnd = str.find('"', pos);
        if (keyEnd == std::string::npos) break;
        size_t keyStart = pos;
        pos = str.find('"', keyEnd + 1);
        if (pos == std::string::npos) break;
        
        std::string key = str.substr(keyEnd + 1, pos - keyEnd - 1);
        pos = str.find(':', pos);
        if (pos == std::string::npos) break;
        
        // Parse value
        size_t valueStart = str.find_first_not_of(' ', pos + 1);
        if (valueStart == std::string::npos) break;
        
        // Very basic parsing - just for demonstration
        if (str[valueStart] == '"') {
            size_t valueEnd = str.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            out.objectValue.push_back({key, JsonValue(str.substr(valueStart + 1, valueEnd - valueStart - 1))});
            pos = valueEnd + 1;
        } else if (isdigit(str[valueStart]) || str[valueStart] == '-') {
            size_t valueEnd = str.find(',', valueStart);
            if (valueEnd == std::string::npos) valueEnd = str.find('}', valueStart);
            if (valueEnd == std::string::npos) break;
            std::string valueStr = str.substr(valueStart, valueEnd - valueStart);
            try {
                double val = std::stod(valueStr);
                out.objectValue.push_back({key, JsonValue(val)});
                pos = valueEnd;
            } catch (...) {
                break;
            }
        }
        
        // Skip comma
        while (pos < str.length() && (str[pos] == ' ' || str[pos] == ',')) pos++;
    }
    
    return true;
}

bool Config::jsonSerialize(const JsonValue& val, std::string& out) {
    // Simple JSON serializer
    switch (val.type) {
        case JsonValue::Type::Null:
            out = "null";
            break;
        case JsonValue::Type::String:
            out = "\"" + val.stringValue + "\"";
            break;
        case JsonValue::Type::Number:
            out = std::to_string(val.numberValue);
            break;
        case JsonValue::Type::Boolean:
            out = val.boolValue ? "true" : "false";
            break;
        case JsonValue::Type::Array:
            out = "[";
            for (size_t i = 0; i < val.arrayValue.size(); ++i) {
                if (i > 0) out += ", ";
                std::string elem;
                jsonSerialize(val.arrayValue[i], elem);
                out += elem;
            }
            out += "]";
            break;
        case JsonValue::Type::Object:
            out = "{";
            for (size_t i = 0; i < val.objectValue.size(); ++i) {
                if (i > 0) out += ", ";
                out += "\"" + val.objectValue[i].first + "\": ";
                std::string elem;
                jsonSerialize(val.objectValue[i].second, elem);
                out += elem;
            }
            out += "}";
            break;
    }
    return true;
}
#endif

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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(trim(str));
    return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
}

double Config::stringToDouble(const std::string& str) {
    return std::stod(trim(str));
}

int64_t Config::stringToInt64(const std::string& str) {
    return std::stoll(trim(str));
}

// Environment variable parsing
bool Config::parseEnvVar(const std::string& value, ConfigValue& out) {
    if (value.empty()) {
        out = false;
        return true;
    }
    
    // Try boolean
    std::string lower = toLower(value);
    if (lower == "true" || lower == "false") {
        out = (lower == "true");
        return true;
    }
    
    // Try integer
    try {
        if (value.find('.') == std::string::npos) {
            int64_t intVal = std::stoll(value);
            out = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try double
    try {
        double doubleVal = std::stod(value);
        out = doubleVal;
        return true;
    } catch (...) {}
    
    // Default to string
    out = value;
    return true;
}

// JSON serialization for ConfigValue
JsonValue Config::configValueToJson(const ConfigValue& value) {
#ifdef HAS_NLOHMANN_JSON
    return value; // For nlohmann/json, we can directly assign
#else
    JsonValue jsonVal;
    
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            jsonVal = JsonValue(static_cast<double>(val));
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonVal = JsonValue(static_cast<double>(val));
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            jsonVal = JsonValue(static_cast<double>(val));
        } else if constexpr (std::is_same_v<T, double>) {
            jsonVal = JsonValue(val);
        } else if constexpr (std::is_same_v<T, float>) {
            jsonVal = JsonValue(static_cast<double>(val));
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonVal = JsonValue(val);
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonVal = JsonValue(val);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::vector<JsonValue> arr;
            for (const auto& item : val) {
                arr.push_back(JsonValue(static_cast<double>(item)));
            }
            jsonVal = JsonValue(arr);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonVal = JsonValue(std::vector<JsonValue>(val.begin(), val.end()));
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::vector<JsonValue> arr;
            for (const auto& item : val) {
                arr.push_back(JsonValue(item));
            }
            jsonVal = JsonValue(arr);
        }
    }, value);
    
    return jsonVal;
#endif
    return JsonValue();
}

bool Config::jsonToConfigValue(const JsonValue& json, ConfigValue& value) {
    // This would be implemented to convert from JsonValue to ConfigValue
    // For now, return false to indicate not implemented
    return false;
}

} // namespace nlm
