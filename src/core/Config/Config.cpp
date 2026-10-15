#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <unordered_map>
#include <iomanip>
#include <mutex>

// JSON library inclusion - use nlohmann::json if available, otherwise provide fallback
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#else
// Simple JSON parser for basic operations
class json {
public:
    class json_pointer {};
    class exception : public std::exception {};
    
    static json parse(const std::string& str);
    std::string dump(int indent = -1) const;
    bool is_object() const;
    bool is_array() const;
    bool is_string() const;
    bool is_number() const;
    bool is_boolean() const;
    bool is_null() const;
    size_t size() const;
    const json& operator[](const std::string& key) const;
    json& operator[](const std::string& key);
    const json& operator[](size_t idx) const;
    json& operator[](size_t idx);
    void emplace_back(const json& value);
    template<typename T> T get() const;
    template<typename T> T get_or(const T& default_value) const;
    
private:
    enum class JsonType { OBJECT, ARRAY, STRING, NUMBER, BOOLEAN, NULL_VALUE };
    JsonType type;
    std::string stringValue;
    double numberValue;
    bool booleanValue;
    std::vector<std::pair<std::string, json>> objectValue;
    std::vector<json> arrayValue;
    
    json() : type(JsonType::NULL_VALUE) {}
};
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
    std::unordered_map<std::string, std::string> groups;
    SchemaValidationCallback schemaValidator;
    size_t nextGroupId;
    mutable std::mutex mutex;
    
    Impl() : nextGroupId(0) {}
};

// Global configuration instance management
namespace {
    Config* g_globalConfig = nullptr;
    std::mutex g_configMutex;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

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

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    // Try JSON/YAML format first
    if (parseJsonFile(content) || parseYamlFile(content)) {
        return true;
    }
    
    // Fall back to legacy format
    file.clear();
    file.seekg(0);
    while (std::getline(file, line)) {
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
    
    return true;
}

bool Config::loadFromJson(const std::string& jsonStr) {
    try {
        auto json = parseJson(jsonStr);
        return mergeFromJson(json);
    } catch (...) {
        return false;
    }
}

bool Config::mergeFromJson(const json& j) {
    if (j.is_object()) {
        // For JSON object, recursively parse
        // This would extract nested key=value pairs
        // For simplicity, we'll just extract direct properties
    }
    return true;
}

bool Config::parseJsonFile(const std::string& content) {
    // Implementation for JSON file parsing
    // Could use nlohmann::json or similar library
    return false;
}

bool Config::parseYamlFile(const std::string& content) {
    // Implementation for YAML file parsing
    // Could use YAML-cpp or similar library
    return false;
}

bool Config::parseLegacyFileFormat(const std::string& content) {
    // Parse legacy key=value format
    return false;
}

#ifdef HAVE_NLOHMANN_JSON

json Config::parseJson(const std::string& content) {
    return json::parse(content);
}

#else

json json::parse(const std::string& str) {
    json result;
    // Simple implementation - could be enhanced with proper JSON parsing
    // For now, return empty object
    return result;
}

#endif

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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File - Generated " << getCurrentTimestamp() << "\n";
    file << "# Format: key = value (JSON-style in the future)\n\n";
    
    // Group entries by source for better organization
    std::map<ConfigSource, std::vector<const ConfigEntry*>> grouped;
    for (const auto& entry : pImpl->entries) {
        grouped[entry.source].push_back(&entry);
    }
    
    // Save file entries first
    if (grouped.find(ConfigSource::File) != grouped.end()) {
        file << "# File specified values\n";
        for (const auto* entry : grouped[ConfigSource::File]) {
            file << "# " << entry->description << "\n";
            file << entry->key << " = ";
            saveValueToFile(file, entry->value);
            file << "\n\n";
        }
    }
    
    // Save command line entries
    if (grouped.find(ConfigSource::CommandLine) != grouped.end()) {
        file << "# Command line specified values (override file)\n";
        for (const auto* entry : grouped[ConfigSource::CommandLine]) {
            file << "# " << entry->description << "\n";
            file << entry->key << " = ";
            saveValueToFile(file, entry->value);
            file << "\n\n";
        }
    }
    
    return true;
}

std::string Config::exportToJson() const {
    // Export configuration to JSON string
    // This would implement a full JSON export
    return "{}";
}

#ifdef HAVE_NLOHMANN_JSON

void Config::saveValueToFile(std::ofstream& file, const ConfigValue& value) {
    std::visit([&file](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            file << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            file << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, double>) {
            file << arg;
        } else {
            // For unknown types, use string representation
            file << "\"" << std::to_string(arg) << "\"";
        }
    }, value);
}

#else

void Config::saveValueToFile(std::ofstream& file, const ConfigValue& value) {
    // Simple implementation for basic types
    std::visit([&file](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            file << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            file << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int64_t>) {
            file << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            file << arg;
        } else {
            // For unknown types, try to convert to string
            try {
                file << "\"" << std::to_string(arg) << "\"";
            } catch (...) {
                file << "\"unknown\"";
            }
        }
    }, value);
}

#endif

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

namespace {
    std::string getCurrentTimestampStatic() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
}

bool Config::setValidated(const std::string& key, const ConfigValue& value, 
                         ConfigSource source, const ValidationCallback& validator) {
    // Validate the key format
    if (!isValidKey(key)) {
        NLM_LOG_ERROR("Invalid configuration key: " + key);
        return false;
    }
    
    // Apply validator if provided
    if (validator && !validator(key, value)) {
        NLM_LOG_ERROR("Configuration validation failed for key: " + key);
        return false;
    }
    
    // Validate schema constraints
    if (!validateSchema(key, value)) {
        NLM_LOG_ERROR("Schema validation failed for key: " + key);
        return false;
    }
    
    // Set the value with error handling
    try {
        set(key, value, source);
        NLM_LOG_DEBUG("Configuration set successfully: " + key + " = " + jsonValueToString(value));
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception setting configuration key " + key + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::isValidKey(const std::string& key) const {
    // Check for empty key
    if (key.empty()) {
        return false;
    }
    
    // Check for invalid characters (allow alphanumeric, underscore, dash, dot)
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            return false;
        }
    }
    
    return true;
}

std::string Config::jsonValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << "\"unknown\"";
        }
    }, value);
    return oss.str();
}

void Config::validateAll() const {
    // Validate all schema entries
    for (const auto& schema : pImpl->schemas) {
        if (schema.required && !has(schema.key)) {
            NLM_LOG_ERROR("Required configuration key missing: " + schema.key);
        } else if (has(schema.key)) {
            auto value = getValue(schema.key);
            if (!validateSchema(schema.key, value)) {
                NLM_LOG_ERROR("Schema validation failed for key: " + schema.key);
            }
        }
    }
}

bool Config::validateSchema(const std::string& key, const ConfigValue& value) const {
    // Find schema for this key
    for (const auto& schema : pImpl->schemas) {
        if (schema.key == key) {
            return schema.validator ? schema.validator(key, value) : true;
        }
    }
    return true; // No schema found, assume valid
}

ConfigValue Config::getValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        throw std::runtime_error("Configuration key not found: " + key);
    }
    
    return it->value;
}

std::string Config::statistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:" << std::endl;
    oss << "  Total entries: " << pImpl->entries.size() << std::endl;
    oss << "  Schema entries: " << pImpl->schemas.size() << std::endl;
    
    // Group by source
    std::map<ConfigSource, size_t> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    oss << "  By source:" << std::endl;
    for (const auto& pair : sourceCount) {
        oss << "    " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    // Type distribution
    std::map<std::string, size_t> typeCount;
    for (const auto& entry : pImpl->entries) {
        std::string typeName;
        if (std::holds_alternative<int>(entry.value)) {
            typeName = "int";
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            typeName = "int64_t";
        } else if (std::holds_alternative<double>(entry.value)) {
            typeName = "double";
        } else if (std::holds_alternative<bool>(entry.value)) {
            typeName = "bool";
        } else if (std::holds_alternative<std::string>(entry.value)) {
            typeName = "string";
        } else {
            typeName = "other";
        }
        typeCount[typeName]++;
    }
    
    oss << "  By type:" << std::endl;
    for (const auto& pair : typeCount) {
        oss << "    " << pair.first << ": " << pair.second << std::endl;
    }
    
    return oss.str();
}

void Config::beginGroup(const std::string& group) {
    pImpl->groups[group] = getGroupPrefix(group);
}

void Config::endGroup() {
    pImpl->groups.clear();
}

std::string Config::getCurrentGroup() const {
    return pImpl->groups.empty() ? "" : pImpl->groups.begin()->first;
}

std::string Config::getGroupPrefix(const std::string& key) const {
    // Simple implementation: return key as prefix
    // More sophisticated implementations could use nested key-value pairs
    return key + ".";
}

// Thread safety wrapper for common operations
bool Config::setThreadSafe(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    set(key, value, source);
    return true;
}

ConfigValue Config::getThreadSafe(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return getValue(key);
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

} // namespace nlm

// Global configuration instance management
namespace {
    Config* g_globalConfig = nullptr;
    std::mutex g_configMutex;
}

GlobalConfig& GlobalConfig::getInstance() {
    static GlobalConfig instance;
    return instance;
}

void GlobalConfig::setInstance(Config* config) {
    std::lock_guard<std::mutex> lock(g_configMutex);
    g_globalConfig = config;
}

Config* GlobalConfig::getConfig() {
    std::lock_guard<std::mutex> lock(g_configMutex);
    return g_globalConfig;
}

const Config* GlobalConfig::getConfig() const {
    std::lock_guard<std::mutex> lock(g_configMutex);
    return g_globalConfig;
}

void GlobalConfig::loadFromFile(const std::string& filepath) {
    if (g_globalConfig) {
        g_globalConfig->loadFromFile(filepath);
    }
}

void GlobalConfig::loadFromArgs(int argc, char** argv) {
    if (g_globalConfig) {
        g_globalConfig->loadFromArgs(argc, argv);
    }
}

void GlobalConfig::saveToFile(const std::string& filepath) const {
    if (g_globalConfig) {
        g_globalConfig->saveToFile(filepath);
    }
}

Config* getGlobalConfig() {
    return GlobalConfig::getInstance().getConfig();
}

const Config* getGlobalConfig() const {
    return GlobalConfig::getInstance().getConfig();
}

} // namespace nlm

// Convenience functions for easy configuration access
namespace nlm {
    bool setConfig(const std::string& key, const std::string& value) {
        if (auto* config = getGlobalConfig()) {
            return config->setValidated(key, value);
        }
        return false;
    }
    
    bool setConfig(const std::string& key, int value) {
        if (auto* config = getGlobalConfig()) {
            return config->setValidated(key, value);
        }
        return false;
    }
    
    bool setConfig(const std::string& key, double value) {
        if (auto* config = getGlobalConfig()) {
            return config->setValidated(key, value);
        }
        return false;
    }
    
    bool setConfig(const std::string& key, bool value) {
        if (auto* config = getGlobalConfig()) {
            return config->setValidated(key, value);
        }
        return false;
    }
    
    std::string getConfigString(const std::string& key, const std::string& defaultValue = "") {
        if (auto* config = getGlobalConfig()) {
            auto val = config->get<std::string>(key);
            return val.has_value() ? val.value() : defaultValue;
        }
        return defaultValue;
    }
    
    int getConfigInt(const std::string& key, int defaultValue = 0) {
        if (auto* config = getGlobalConfig()) {
            auto val = config->get<int>(key);
            return val.has_value() ? val.value() : defaultValue;
        }
        return defaultValue;
    }
    
    double getConfigDouble(const std::string& key, double defaultValue = 0.0) {
        if (auto* config = getGlobalConfig()) {
            auto val = config->get<double>(key);
            return val.has_value() ? val.value() : defaultValue;
        }
        return defaultValue;
    }
    
    bool getConfigBool(const std::string& key, bool defaultValue = false) {
        if (auto* config = getGlobalConfig()) {
            auto val = config->get<bool>(key);
            return val.has_value() ? val.value() : defaultValue;
        }
        return defaultValue;
    }
    
    bool hasConfig(const std::string& key) {
        if (auto* config = getGlobalConfig()) {
            return config->has(key);
        }
        return false;
    }
    
    void removeConfig(const std::string& key) {
        if (auto* config = getGlobalConfig()) {
            config->remove(key);
        }
    }
    
    std::string configSummary() {
        if (auto* config = getGlobalConfig()) {
            return config->summary();
        }
        return "No global configuration available";
    }
    
    std::string configStatistics() {
        if (auto* config = getGlobalConfig()) {
            return config->statistics();
        }
        return "No global configuration available";
    }
    
    void validateConfiguration() {
        if (auto* config = getGlobalConfig()) {
            config->validateAll();
        }
    }
}