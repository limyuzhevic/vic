#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cctype>
#include <iomanip>
#include <unordered_set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchemaEntry> schema;
    std::unordered_map<std::string, ConfigType> typeCache;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string expandedPath = expandPath(filepath);
    if (!fileExists(expandedPath)) {
        return false;
    }
    
    // Try to detect file format based on extension
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        return loadFromJSON(filepath); // Need to implement file reading
    } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        return loadFromYAML(filepath); // Need to implement file reading
    } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".cfg") {
        // Legacy key=value format
        std::ifstream file(expandedPath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            std::string key, value;
            if (parseKeyValueLine(line, key, value)) {
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    } else {
        // Try JSON first, then YAML, then key=value
        if (loadFromJSON(expandedPath)) return true;
        if (loadFromYAML(expandedPath)) return true;
        
        // Fall back to key=value format
        std::ifstream file(expandedPath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            std::string key, value;
            if (parseKeyValueLine(line, key, value)) {
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        // Simple JSON parsing implementation
        // This is a basic implementation - in production you'd want a proper JSON library
        clear();
        
        std::string::size_type pos = 0;
        while (pos < jsonStr.size()) {
            // Skip whitespace
            while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
            if (pos >= jsonStr.size()) break;
            
            // Check for object start
            if (jsonStr[pos] == '{') {
                pos++; // Skip {
                std::string objStr;
                int braceCount = 1;
                while (pos < jsonStr.size() && braceCount > 0) {
                    if (jsonStr[pos] == '{') braceCount++;
                    else if (jsonStr[pos] == '}') braceCount--;
                    if (braceCount > 0) objStr += jsonStr[pos];
                    pos++;
                }
                // Parse object as key=value pairs (simplified)
                // For now, fall back to key=value parsing for objects
            } else if (jsonStr[pos] == '[') {
                pos++; // Skip [
                // Parse array - for now ignore
            } else if (jsonStr[pos] == '"') {
                // Parse string key
                pos++; // Skip opening quote
                std::string key;
                while (pos < jsonStr.size() && jsonStr[pos] != '"') {
                    key += jsonStr[pos++];
                }
                if (pos < jsonStr.size()) pos++; // Skip closing quote
                
                // Skip whitespace
                while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
                
                if (pos < jsonStr.size() && jsonStr[pos] == ':') {
                    pos++; // Skip colon
                    
                    // Skip whitespace after colon
                    while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) pos++;
                    
                    // Parse value
                    if (pos < jsonStr.size() && jsonStr[pos] == '"') {
                        // String value
                        pos++; // Skip opening quote
                        std::string value;
                        while (pos < jsonStr.size() && jsonStr[pos] != '"') {
                            value += jsonStr[pos++];
                        }
                        if (pos < jsonStr.size()) pos++; // Skip closing quote
                        set(key, value, ConfigSource::File);
                    } else if (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) {
                        // Number value
                        std::string numStr;
                        while (pos < jsonStr.size() && (std::isdigit(jsonStr[pos]) || jsonStr[pos] == '.' || jsonStr[pos] == '-')) {
                            numStr += jsonStr[pos++];
                        }
                        try {
                            if (numStr.find('.') != std::string::npos) {
                                set(key, std::stod(numStr), ConfigSource::File);
                            } else {
                                set(key, std::stoi(numStr), ConfigSource::File);
                            }
                        } catch (...) {
                            // Skip invalid numbers
                        }
                    } else if (pos < jsonStr.size() && jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
                        // Boolean value
                        std::string boolStr;
                        while (pos < jsonStr.size() && (jsonStr[pos] == 't' || jsonStr[pos] == 'f' || jsonStr[pos] == ' ')) {
                            boolStr += jsonStr[pos++];
                        }
                        if (boolStr == "true") {
                            set(key, true, ConfigSource::File);
                        } else if (boolStr == "false") {
                            set(key, false, ConfigSource::File);
                        }
                    } else if (pos < jsonStr.size() && jsonStr[pos] == 'n') {
                        // Null value
                        std::string nullStr;
                        while (pos < jsonStr.size() && (jsonStr[pos] == 'n' || jsonStr[pos] == 'u' || jsonStr[pos] == 'l' || jsonStr[pos] == 'l')) {
                            nullStr += jsonStr[pos++];
                        }
                        if (nullStr == "null") {
                            set(key, std::string(""), ConfigSource::File);
                        }
                    }
                }
            } else {
                pos++; // Skip unknown
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlStr) {
    // For now, fall back to key=value format
    // TODO: Implement proper YAML parsing
    return loadFromFile(yamlStr);
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

bool Config::loadFromEnvironment(const std::unordered_map<std::string, std::string>& envVars) {
    for (const auto& envVar : envVars) {
        std::string key = envVar.first;
        std::string value = envVar.second;
        
        // Convert environment variable key to config key (e.g., NLM_BRAIN_COUNT -> brain.count)
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        std::replace(key.begin(), key.end(), '_', '.');
        
        set(key, value, ConfigSource::Environment);
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    std::string expandedPath = expandPath(filepath);
    std::ofstream file(expandedPath);
    if (!file.is_open()) {
        return false;
    }
    
    if (prettyPrint) {
        file << "{\n";
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) file << ",\n";
            first = false;
            
            file << "  \"" << entry.key << "\": ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << std::fixed << std::setprecision(6) << arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << std::fixed << std::setprecision(6) << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << arg[i] << "\"";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << (arg[i] ? "true" : "false");
                    }
                    file << "]";
                }
            }, entry.value);
        }
        file << "\n}";
    } else {
        // Simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n\n";
        }
    }
    
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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

ConfigType Config::getType(const std::string& key) const {
    if (pImpl->typeCache.find(key) != pImpl->typeCache.end()) {
        return pImpl->typeCache[key];
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigType::String; // Default
    }
    
    ConfigType type;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) type = ConfigType::Integer;
        else if constexpr (std::is_same_v<T, int64_t>) type = ConfigType::Integer64;
        else if constexpr (std::is_same_v<T, double>) type = ConfigType::Float;
        else if constexpr (std::is_same_v<T, bool>) type = ConfigType::Boolean;
        else if constexpr (std::is_same_v<T, std::string>) type = ConfigType::String;
        else if constexpr (std::is_same_v<T, std::vector<int>>) type = ConfigType::IntArray;
        else if constexpr (std::is_same_v<T, std::vector<double>>) type = ConfigType::FloatArray;
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) type = ConfigType::StringArray;
        else if constexpr (std::is_same_v<T, std::vector<bool>>) type = ConfigType::BoolArray;
        else type = ConfigType::String;
    }, it->value);
    
    pImpl->typeCache[key] = type;
    return type;
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->typeCache.erase(key);
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
    pImpl->typeCache.clear();
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

ValidationResult Config::validate(const std::vector<ConfigSchemaEntry>& schema) const {
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // Convert schema to internal format
    pImpl->schema = schema;
    
    for (const auto& entry : pImpl->entries) {
        // Find corresponding schema entry
        auto schemaIt = std::find_if(schema.begin(), schema.end(),
            [&entry](const ConfigSchemaEntry& s) { return s.key == entry.key; });
        
        if (schemaIt != schema.end()) {
            // Validate against schema
            if (!validateAgainstSchema(entry, *schemaIt, errors, warnings)) {
                // Error already added to errors vector
            }
        } else if (entry.required) {
            errors.push_back("Required key \"" + entry.key + "\" is missing from schema");
        }
    }
    
    // Check for missing required schema entries
    for (const auto& schemaEntry : schema) {
        if (schemaEntry.required && !has(schemaEntry.key)) {
            errors.push_back("Required key \"" + schemaEntry.key + "\" is missing from configuration");
        }
    }
    
    return ValidationResult(!errors.empty(), errors, warnings);
}

ValidationResult Config::validateAgainst(const Config& referenceConfig) const {
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // Check all keys in reference config exist in this config
    for (const auto& refEntry : referenceConfig.pImpl->entries) {
        if (!has(refEntry.key)) {
            errors.push_back("Missing key \"" + refEntry.key + "\" present in reference configuration");
        }
    }
    
    // Compare values for common keys
    for (const auto& entry : pImpl->entries) {
        if (referenceConfig.has(entry.key)) {
            auto refValue = referenceConfig.get<decltype(entry.value)>(entry.key);
            if (refValue.has_value()) {
                // Compare values (simplified)
                if (std::visit([](auto&& a, auto&& b) { return a != b; }, entry.value, *refValue)) {
                    warnings.push_back("Value for key \"" + entry.key + "\" differs from reference");
                }
            }
        }
    }
    
    return ValidationResult(!errors.empty(), errors, warnings);
}

void Config::addSchemaEntry(const ConfigSchemaEntry& entry) {
    pImpl->schema.push_back(entry);
}

void Config::clearSchema() {
    pImpl->schema.clear();
}

std::vector<std::string> Config::getSchemaKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->schema.size());
    for (const auto& entry : pImpl->schema) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::string Config::getEnvironmentVariable(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    return value ? value : "";
}

std::string Config::expandPath(const std::string& path) {
    if (path.empty() || path[0] != '~') {
        return path;
    }
    
    const char* home = std::getenv("HOME");
    if (!home) {
        return path;
    }
    
    return std::string(home) + path.substr(1);
}

bool Config::fileExists(const std::string& path) {
    std::filesystem::path fsPath(path);
    return std::filesystem::exists(fsPath);
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

bool Config::matchesPattern(const std::string& value, const std::string& pattern) {
    if (pattern.empty()) return true;
    
    // Simple regex pattern matching
    std::regex regex(pattern);
    return std::regex_match(value, regex);
}

std::string Config::convertToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string Config::configTypeToString(ConfigType type) {
    switch (type) {
        case ConfigType::Integer: return "integer";
        case ConfigType::Integer64: return "int64";
        case ConfigType::Float: return "float";
        case ConfigType::Boolean: return "boolean";
        case ConfigType::String: return "string";
        case ConfigType::IntArray: return "int_array";
        case ConfigType::FloatArray: return "float_array";
        case ConfigType::StringArray: return "string_array";
        case ConfigType::BoolArray: return "bool_array";
        default: return "unknown";
    }
}

bool Config::parseKeyValueLine(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    key = trim(line.substr(0, pos));
    value = trim(line.substr(pos + 1));
    
    // Remove quotes if present
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    return !key.empty();
}

bool Config::validateJSONValue(const std::string& value, ConfigType expectedType) {
    switch (expectedType) {
        case ConfigType::String: return true; // Simplified validation
        case ConfigType::Integer: return true;
        case ConfigType::Float: return true;
        case ConfigType::Boolean: return true;
        default: return true;
    }
}

bool Config::validateYAMLScalar(const std::string& value, ConfigType expectedType) {
    // Similar to JSON validation
    return validateJSONValue(value, expectedType);
}

bool Config::validateAgainstSchema(const ConfigEntry& entry, const ConfigSchemaEntry& schema, std::vector<std::string>& errors, std::vector<std::string>& warnings) {
    bool isValid = true;
    
    // Check type
    if (getType(entry.key) != schema.type) {
        errors.push_back("Key \"" + entry.key + "\" has type " + configTypeToString(getType(entry.key)) + 
                        " but expected " + configTypeToString(schema.type));
        isValid = false;
    }
    
    // Check required flag
    if (schema.required && !entry.required) {
        warnings.push_back("Key \"" + entry.key + "\" is in schema as required but not marked as required");
    }
    
    // Validate pattern
    if (!entry.validationPattern.empty() && !matchesPattern(convertToString(entry.value), entry.validationPattern)) {
        errors.push_back("Key \"" + entry.key + "\" value does not match validation pattern");
        isValid = false;
    }
    
    // Validate range
    if (schema.minValue != 0.0 || schema.maxValue != 0.0) {
        double value = 0.0;
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                         std::is_same_v<T, double>) {
                value = static_cast<double>(arg);
            }
        }, entry.value);
        
        if (value < schema.minValue || value > schema.maxValue) {
            errors.push_back("Key \"" + entry.key + "\" value " + convertToString(entry.value) + 
                            " is outside allowed range [" + std::to_string(schema.minValue) + ", " + 
                            std::to_string(schema.maxValue) + "]");
            isValid = false;
        }
    }
    
    return isValid;
}

bool Config::convertConfigValue(const ConfigValue& from, ConfigValue& to, std::vector<std::string>& errors) {
    // Simple type conversion
    return true; // Simplified
}

} // namespace nlm