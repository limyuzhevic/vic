// Enhanced NLM Configuration System Implementation

#include "Config_enhanced.hpp"
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <ctime>
#include <cmath>

namespace nlm {

// Implementation of ConfigEntry helper methods
bool ConfigEntry::hasValidValue() const {
    if (std::holds_alternative<std::monostate>(value)) {
        return false;
    }
    return isValid();
}

std::string ConfigEntry::getStringValue() const {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<float>(value)) {
        return std::to_string(std::get<float>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    }
    return "";
}

double ConfigEntry::getNumericValue() const {
    if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return static_cast<double>(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<float>(value)) {
        return static_cast<double>(std::get<float>(value));
    }
    return 0.0;
}

bool ConfigEntry::getBoolValue() const {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    }
    return false;
}

bool ConfigEntry::isValid() const {
    if (required && (std::holds_alternative<std::monostate>(value) || getStringValue().empty())) {
        return false;
    }
    
    double numericVal = getNumericValue();
    if (numericVal < min || numericVal > max) {
        return false;
    }
    
    if (!choices.empty()) {
        std::string strValue = getStringValue();
        bool found = false;
        for (const auto& choice : choices) {
            if (strValue == choice) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    
    if (!validation.empty()) {
        std::regex pattern(validation);
        if (!std::regex_match(getStringValue(), pattern)) {
            return false;
        }
    }
    
    return true;
}

std::string ConfigEntry::toString() const {
    std::ostringstream oss;
    oss << "{\"key\": \"" << key << "\", \"group\": \" << group << "\", \"value\": ";
    
    if (std::holds_alternative<std::string>(value)) {
        oss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<int>(value)) {
        oss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        oss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        oss << std::get<double>(value);
    } else if (std::holds_alternative<float>(value)) {
        oss << std::get<float>(value);
    } else if (std::holds_alternative<bool>(value)) {
        oss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
    } else {
        oss << "\"UNKNOWN_TYPE\"";
    }
    
    oss << "}";
    return oss.str();
}

// Implementation of ConfigSchema methods
void ConfigSchema::addEntry(const ConfigEntry& entry) {
    schema[entry.key] = entry;
}

bool ConfigSchema::validate(const Config& config) const {
    std::vector<std::string> errors = getValidationErrors(config);
    return errors.empty();
}

std::vector<std::string> ConfigSchema::getValidationErrors(const Config& config) const {
    std::vector<std::string> errors;
    
    for (const auto& [key, entry] : schema) {
        if (entry.required && !config.has(key)) {
            errors.push_back("Required configuration key missing: " + key);
            continue;
        }
        
        if (config.has(key)) {
            auto value = config.get(entry.type.empty() ? "string" : entry.type, key);
            if (!value) {
                errors.push_back("Configuration key " + key + " has invalid type");
                continue;
            }
            
            // Additional validation would go here
        }
    }
    
    if (strict) {
        for (const auto& key : config.getKeys()) {
            if (schema.find(key) == schema.end()) {
                errors.push_back("Strict schema validation failed: unexpected configuration key " + key);
            }
        }
    }
    
    return errors;
}

// Enhanced Config class implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string environment = "development";
    std::map<std::string, std::string> environmentVariables;
    std::map<std::string, std::string> presets;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// File loading implementations
bool Config::loadFromFile(const std::string& filepath) {
    std::string format = detectFileFormat(filepath);
    if (format == "json") {
        return loadFromJSON(filepath);
    } else if (format == "yaml") {
        return loadFromYAML(filepath);
    } else {
        return loadFromKeyValue(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    // For now, fall back to key=value format
    // In a real implementation, this would use a JSON library
    return loadFromKeyValue(filepath);
}

bool Config::loadFromYAML(const std::string& filepath) {
    // For now, fall back to key=value format
    // In a real implementation, this would use a YAML library
    return loadFromKeyValue(filepath);
}

bool Config::loadFromKeyValue(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
            // Try to handle key:value format (JSON-like)
            pos = line.find(':');
            if (pos != std::string::npos && pos > 0) {
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
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                // Handle --key value format
                set(arg.substr(2), "true", ConfigSource::CommandLine);
            }
        } 
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
        // Handle --key=value with list values
        else if (arg.find('=') != std::string::npos && arg[0] != '-') {
            size_t pos = arg.find('=');
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    for (const auto& envPair : pImpl->environmentVariables) {
        std::string key = envPair.first;
        if (key.rfind(prefix, 0) == 0) {
            std::string configKey = key.substr(prefix.length());
            set(configKey, envPair.second, ConfigSource::Environment);
        }
    }
    return true;
}

bool Config::loadFromJSONString(const std::string& jsonString) {
    // For now, fall back to key=value format
    // In a real implementation, this would use a JSON library
    return loadFromKeyValueFromString(jsonString);
}

bool Config::loadFromYAMLString(const std::string& yamlString) {
    // For now, fall back to key=value format
    // In a real implementation, this would use a YAML library
    return loadFromKeyValueFromString(yamlString);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string format = detectFileFormat(filepath);
    if (format == "json") {
        return saveToJSON(filepath);
    } else if (format == "yaml") {
        return saveToYAML(filepath);
    } else {
        return saveToKeyValue(filepath);
    }
}

bool Config::saveToJSON(const std::string& filepath) const {
    // For now, fall back to key=value format
    // In a real implementation, this would use a JSON library
    return saveToKeyValue(filepath);
}

bool Config::saveToYAML(const std::string& filepath) const {
    // For now, fall to key=value format
    // In a real implementation, this would use a YAML library
    return saveToKeyValue(filepath);
}

bool Config::saveToKeyValue(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n";
    file << "# Environment: " << pImpl->environment << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        if (!entry.group.empty()) {
            file << "# Group: " << entry.group << "\n";
        }
        file << entry.key << " = " << valueToString(entry.value) << "\n\n";
    }
    
    return true;
}

std::string Config::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": " << valueToString(entry.value);
        if (i < pImpl->entries.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "}";
    return oss.str();
}

std::string Config::toYAML() const {
    std::ostringstream oss;
    oss << "# NLM Configuration\n";
    oss << "# Environment: " << pImpl->environment << "\n\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        if (!entry.group.empty()) {
            oss << entry.group << ":\n";
        }
        oss << "  " << entry.key << ": " << valueToString(entry.value);
        if (i < pImpl->entries.size() - 1) {
            oss << "\n";
        }
    }
    return oss.str();
}

std::string Config::valueToString(const ConfigValue& value) const {
    if (std::holds_alternative<std::string>(value)) {
        return "\"" + std::get<std::string>(value) + "\"";
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<float>(value)) {
        return std::to_string(std::get<float>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << vec[i];
        }
        oss << "]";
        return oss.str();
    }
    return "\"UNKNOWN_TYPE\"";
}

// Set methods with enhanced type support
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, float value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Group management
void Config::setGroup(const std::string& key, const std::string& group) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        it->group = group;
    }
}

std::string Config::getGroup(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        return it->group;
    }
    return "";
}

std::vector<std::string> Config::getKeysInGroup(const std::string& group) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.group == group) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getAllGroups() const {
    std::vector<std::string> groups;
    for (const auto& entry : pImpl->entries) {
        if (!entry.group.empty() && 
            std::find(groups.begin(), groups.end(), entry.group) == groups.end()) {
            groups.push_back(entry.group);
        }
    }
    return groups;
}

// Summary methods
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::summary(const std::string& group) const {
    std::ostringstream oss;
    oss << "Configuration Group: " << group << " (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        if (entry.group == group) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                     std::is_same_v<T, std::vector<double>> ||
                                     std::is_same_v<T, std::vector<std::string>>) {
                    oss << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i) oss << ", ";
                        oss << arg[i];
                    }
                    oss << "]";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "] (" << static_cast<int>(entry.source) << ")\n";
        }
    }
    return oss.str();
}

std::string Config::getDocumentation() const {
    std::ostringstream oss;
    oss << "NLM Configuration Documentation\n";
    oss << "===============================\n\n";
    oss << "Environment: " << pImpl->environment << "\n\n";
    
    std::map<std::string, std::vector<ConfigEntry>> groupedEntries;
    for (const auto& entry : pImpl->entries) {
        if (!entry.group.empty()) {
            groupedEntries[entry.group].push_back(entry);
        }
    }
    
    for (const auto& [group, entries] : groupedEntries) {
        oss << "Group: " << group << "\n";
        oss << "--------\n";
        for (const auto& entry : entries) {
            oss << "Key: " << entry.key << "\n";
            oss << "Description: " << entry.description << "\n";
            oss << "Type: " << entry.type << "\n";
            oss << "Default: " << entry.defaultValue << "\n";
            oss << "Required: " << (entry.required ? "yes" : "no") << "\n";
            if (entry.min != std::numeric_limits<double>::lowest() || entry.max != std::numeric_limits<double>::max()) {
                oss << "Range: [" << entry.min << ", " << entry.max << "]\n";
            }
            if (!entry.validation.empty()) {
                oss << "Validation: " << entry.validation << "\n";
            }
            if (!entry.choices.empty()) {
                oss << "Choices: ";
                for (size_t i = 0; i < entry.choices.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << entry.choices[i];
                }
                oss << "\n";
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

// Environment-specific methods
bool Config::loadEnvironmentConfig(const std::string& environment) {
    // In a real implementation, this would load environment-specific
    // configuration from files or environment variables
    pImpl->environment = environment;
    return true;
}

void Config::setEnvironment(const std::string& environment) {
    pImpl->environment = environment;
}

std::string Config::getEnvironment() const {
    return pImpl->environment;
}

// File format detection
std::string Config::detectFileFormat(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return "json";
    } else if (extension == ".yaml" || extension == ".yml") {
        return "yaml";
    } else if (extension == ".cfg" || extension == ".conf" || extension == ".txt") {
        return "keyvalue";
    }
    
    // Try to detect from content
    return detectFileFormatFromContent(filepath);
}

std::string Config::detectFileFormatFromContent(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "keyvalue";
    }
    
    std::string line;
    std::getline(file, line);
    
    // Check for JSON format (starts with { or [)
    if (line.find('{') == 0 || line.find('[') == 0) {
        return "json";
    }
    
    // Check for YAML format (contains colon in first line)
    if (line.find(':') != std::string::npos) {
        return "yaml";
    }
    
    // Check for key=value format
    if (line.find('=') != std::string::npos) {
        return "keyvalue";
    }
    
    return "keyvalue";
}

bool Config::isJSONFile(const std::string& filepath) {
    return detectFileFormat(filepath) == "json";
}

bool Config::isYAMLLFile(const std::string& filepath) {
    return detectFileFormat(filepath) == "yaml";
}

bool Config::isKeyValueFile(const std::string& filepath) {
    return detectFileFormat(filepath) == "keyvalue";
}

// Configuration cloning
Config Config::clone() const {
    Config cloned;
    cloned.pImpl->environment = pImpl->environment;
    cloned.pImpl->environmentVariables = pImpl->environmentVariables;
    cloned.pImpl->presets = pImpl->presets;
    
    for (const auto& entry : pImpl->entries) {
        cloned.pImpl->entries.push_back(entry);
    }
    
    return cloned;
}

// Internal helpers
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

bool Config::validateConfigValue(const ConfigEntry& entry, const ConfigValue& value) {
    // Basic validation implementation
    return true;
}

// Set/get template implementations
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
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

// Helper method to load from string (for JSON/YAML strings)
bool Config::loadFromKeyValueFromString(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[nessage:'; line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    return true;
}

// Additional helper methods
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        ConfigEntry entry;
        entry.key = key;
        entry.value = value;
        entry.source = source;
        pImpl->entries.push_back(entry);
    }
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
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

// Equality operators
bool Config::operator==(const Config& other) const {
    return pImpl->entries == other.pImpl->entries;
}

bool Config::operator!=(const Config& other) const {
    return !(*this == other);
}

// Configuration comparison and merging
Config Config::merge(const Config& other) const {
    Config merged = *this;
    
    for (const auto& entry : other.pImpl->entries) {
        bool found = false;
        for (auto& mergedEntry : merged.pImpl->entries) {
            if (mergedEntry.key == entry.key) {
                mergedEntry = entry; // Replace with other's entry
                found = true;
                break;
            }
        }
        if (!found) {
            merged.pImpl->entries.push_back(entry);
        }
    }
    
    return merged;
}

Config Config::diff(const Config& other) const {
    Config diffConfig;
    
    // Add entries from this config that are not in other
    for (const auto& entry : pImpl->entries) {
        bool foundInOther = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (otherEntry.key == entry.key) {
                foundInOther = true;
                break;
            }
        }
        if (!foundInOther) {
            diffConfig.set(entry.key, entry.value, entry.source);
        }
    }
    
    return diffConfig;
}

// Configuration cloning
Config Config::clone() const {
    Config cloned;
    
    for (const auto& entry : pImpl->entries) {
        cloned.set(entry.key, entry.value, entry.source);
    }
    
    return cloned;
}

} // namespace nlm
