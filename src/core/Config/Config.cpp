#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace nlm {

// Helper functions for JSON parsing and value conversion
namespace {
    
    // Convert JSON value to ConfigValue
    ConfigValue jsonToConfigValue(const nlohmann::json& j) {
        if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_number_integer()) {
            return j.get<int>();
        } else if (j.is_number_unsigned()) {
            return static_cast<int64_t>(j.get<uint64_t>());
        } else if (j.is_number_float()) {
            return j.get<double>();
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_array()) {
            // Handle different array types
            if (j.empty()) {
                return std::vector<std::string>{};
            }
            
            // Check if all elements are strings
            bool allStrings = std::all_of(j.begin(), j.end(), 
                [](const nlohmann::json& elem) { return elem.is_string(); });
            if (allStrings) {
                std::vector<std::string> result;
                result.reserve(j.size());
                for (const auto& elem : j) {
                    result.push_back(elem.get<std::string>());
                }
                return result;
            }
            
            // Check if all elements are integers
            bool allInts = std::all_of(j.begin(), j.end(),
                [](const nlohmann::json& elem) { return elem.is_number_integer(); });
            if (allInts) {
                std::vector<int> result;
                result.reserve(j.size());
                for (const auto& elem : j) {
                    result.push_back(elem.get<int>());
                }
                return result;
            }
            
            // Check if all elements are floats
            bool allFloats = std::all_of(j.begin(), j.end(),
                [](const nlohmann::json& elem) { return elem.is_number_float(); });
            if (allFloats) {
                std::vector<double> result;
                result.reserve(j.size());
                for (const auto& elem : j) {
                    result.push_back(elem.get<double>());
                }
                return result;
            }
            
            // Fall back to string representation
            std::string result = "[";
            for (size_t i = 0; i < j.size(); ++i) {
                if (i > 0) result += ", ";
                result += j[i].dump();
            }
            result += "]";
            return result;
        } else if (j.is_null()) {
            return std::string("null");
        } else {
            // For other types, use JSON string representation
            return j.dump();
        }
    }
    
    // Convert ConfigValue to JSON value
    nlohmann::json configToJson(const ConfigValue& value) {
        return std::visit([](const auto& val) -> nlohmann::json {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                return val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return val;
            } else if constexpr (std::is_same_v<T, double>) {
                return val;
            } else if constexpr (std::is_same_v<T, bool>) {
                return val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                nlohmann::json result = nlohmann::json::array();
                for (const auto& item : val) {
                    result.push_back(item);
                }
                return result;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                nlohmann::json result = nlohmann::json::array();
                for (const auto& item : val) {
                    result.push_back(item);
                }
                return result;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                nlohmann::json result = nlohmann::json::array();
                for (const auto& item : val) {
                    result.push_back(item);
                }
                return result;
            } else {
                return nlohmann::json(val);
            }
        }, value);
    }
    
    // Check if a value is within bounds (for numeric validation)
    bool isValueValid(const ConfigValue& value, const ConfigSchema& schema) {
        if (!schema.validator) return true;
        
        return std::visit([schema](const auto& val) -> bool {
            if constexpr (std::is_same_v<decltype(val), int> || 
                         std::is_same_v<decltype(val), int64_t> || 
                         std::is_same_v<decltype(val), double> || 
                         std::is_same_v<decltype(val), bool> || 
                         std::is_same_v<decltype(val), std::string> || 
                         std::is_same_v<decltype(val), std::vector<int>> || 
                         std::is_same_v<decltype(val), std::vector<double>> || 
                         std::is_same_v<decltype(val), std::vector<std::string>>) {
                return schema.validator(val);
            }
            return false;
        }, value);
    }
    
    // Parse old key=value format from a file
    bool parseKeyValueFormat(const std::string& filepath, std::vector<ConfigEntry>& entries) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            line = Config::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, pos));
                std::string value = Config::trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                ConfigEntry entry(key, value, ConfigSource::File);
                entries.push_back(entry);
            }
        }
        
        return true;
    }
    
    // Detect file format based on extension and content
    std::string detectFileFormat(const std::string& filepath) {
        if (filepath.empty()) {
            return "unknown";
        }
        
        std::string ext = "." + std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".json") return "json";
        if (ext == ".yaml" || ext == ".yml") return "yaml";
        if (ext == ".cfg") return "keyvalue";
        
        // Try to detect by content
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return "unknown";
        }
        
        char firstChar;
        file >> firstChar;
        file.clear();
        file.seekg(0);
        
        if (firstChar == '{' || firstChar == '[') return "json";
        
        // Simple heuristic for YAML (contains ':' and typical YAML structure)
        std::string firstLine;
        std::getline(file, firstLine);
        if (firstLine.find(':') != std::string::npos) {
            return "yaml";
        }
        
        return "keyvalue";
    }
    
    // Parse nested JSON objects with inheritance
    void parseJsonWithInheritance(const nlohmann::json& jsonData, std::vector<ConfigEntry>& entries, 
                                 const std::vector<std::string>& basePaths, const std::string& prefix = "") {
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = prefix + it.key();
            
            if (it->is_object()) {
                // Handle nested objects by recursively processing them
                parseJsonWithInheritance(*it, entries, basePaths, key + ".");
            } else if (it->is_array()) {
                // Handle arrays
                ConfigValue value = jsonToConfigValue(*it);
                entries.emplace_back(key, value, ConfigSource::File);
            } else {
                // Handle simple values
                ConfigValue value = jsonToConfigValue(*it);
                entries.emplace_back(key, value, ConfigSource::File);
            }
        }
    }
    
    // Variable substitution helper
    bool substituteInValue(std::string& value) {
        // Simple variable substitution - look for ${var} patterns
        size_t pos = 0;
        while ((pos = value.find("$", pos)) != std::string::npos) {
            if (pos + 1 < value.size() && value[pos + 1] == '{') {
                size_t endPos = value.find('}', pos + 2);
                if (endPos != std::string::npos) {
                    std::string varName = value.substr(pos + 2, endPos - pos - 2);
                    // In a real implementation, you would look up the variable
                    // For now, just remove the reference
                    value.erase(pos, endPos - pos + 1);
                } else {
                    break;
                }
            } else {
                pos++;
            }
        }
        return true;
    }
    
    // Environment variable expansion helper
    bool expandEnvInValue(std::string& value) {
        // Simple environment variable substitution - look for $VAR or ${VAR} patterns
        size_t pos = 0;
        while ((pos = value.find('$', pos)) != std::string::npos) {
            if (pos + 1 < value.size() && value[pos + 1] == '{') {
                size_t endPos = value.find('}', pos + 2);
                if (endPos != std::string::npos) {
                    std::string varName = value.substr(pos + 2, endPos - pos - 2);
                    // Try to get environment variable
                    const char* envValue = std::getenv(varName.c_str());
                    if (envValue) {
                        value.replace(pos, endPos - pos + 1, envValue);
                    } else {
                        // Remove if not found
                        value.erase(pos, endPos - pos + 1);
                    }
                } else {
                    break;
                }
            } else {
                // Handle $VAR pattern
                size_t endPos = value.find_first_of(" \t
\r", pos + 1);
                if (endPos == std::string::npos) endPos = value.size();
                std::string varName = value.substr(pos + 1, endPos - pos - 1);
                
                const char* envValue = std::getenv(varName.c_str());
                if (envValue) {
                    value.replace(pos, varName.size() + 1, envValue);
                } else {
                    // Remove if not found
                    value.erase(pos, varName.size() + 1);
                }
            }
        }
        return true;
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSchema> schemas; // For validation
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper functions for backward compatibility
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

std::string Config::escapeJsonString(const std::string& input) {
    std::string result;
    for (char c : input) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\\\': result += "\\\\"; break;
            case '\\/': result += "\\/"; break;
            case '\\b': result += "\\b"; break;
            case '\\f': result += "\\f"; break;
            case '\\n': result += "\\n"; break;
            case '\\r': result += "\\r"; break;
            case '\\t': result += "\\t"; break;
            default:
                if (c < 0x20 || c == 0x7f) {
                    // Control characters - escape as hex
                    result += "\\\\u";
                    char hex[5];
                    snprintf(hex, sizeof(hex), "%04x", static_cast<unsigned char>(c));
                    result += hex;
                } else {
                    result += c;
                }
        }
    }
    return result;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::string format = detectFileFormat(filepath);
    
    try {
        if (format == "json") {
            return parseJsonFormat(filepath, pImpl->entries);
        } else if (format == "yaml") {
            return parseYamlFormat(filepath, pImpl->entries);
        } else {
            // Use old key=value format
            return parseKeyValueFormat(filepath, pImpl->entries);
        }
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromFileWithInheritance(const std::string& filepath, const std::vector<std::string>& basePaths) {
    std::string format = detectFileFormat(filepath);
    
    try {
        if (format == "json") {
            // Parse JSON with inheritance support
            std::ifstream file(filepath);
            if (!file.is_open()) {
                return false;
            }
            
            nlohmann::json jsonData;
            file >> jsonData;
            
            // Clear existing entries
            pImpl->entries.clear();
            
            // Parse with inheritance
            parseJsonWithInheritance(jsonData, pImpl->entries, basePaths);
            return true;
        } else if (format == "yaml") {
            // Parse YAML with inheritance support
            // For now, use the same approach as JSON
            return parseYamlFormat(filepath, pImpl->entries);
        } else {
            // Use old key=value format
            return parseKeyValueFormat(filepath, pImpl->entries);
        }
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::parseJsonFormat(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        entries.clear();
        
        // Process JSON entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = jsonToConfigValue(*it);
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::parseYamlFormat(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    try {
        // For YAML, we'll convert to JSON first (since we don't have yaml-cpp in this basic implementation)
        // In a full implementation, you would use yaml-cpp directly
        
        // For now, use key=value format as fallback
        return parseKeyValueFormat(filepath, entries);
    } catch (const std::exception&) {
        return false;
    }
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
    std::string format = detectFileFormat(filepath);
    
    try {
        if (format == "json") {
            return saveToJson(filepath, true);
        } else if (format == "yaml" || format == "yml") {
            return saveToYaml(filepath, true);
        } else {
            return saveToKeyValue(filepath);
        }
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToJson(const std::string& filepath, bool pretty) const {
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = configToJson(entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (pretty) {
            file << jsonData.dump(4);
        } else {
            file << jsonData.dump();
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToYaml(const std::string& filepath, bool pretty) const {
    try {
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = configToJson(entry.value);
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (pretty) {
            file << "# NLM Configuration File\n# Generated by NLM Configuration System\n\n";
            for (const auto& [key, value] : jsonData.items()) {
                file << key << " = " << value.dump() << "\n";
            }
        } else {
            file << "# NLM Configuration File\n# Generated by NLM Configuration System\n\n";
            for (const auto& [key, value] : jsonData.items()) {
                if (value.is_string()) {
                    file << key << " = \"" << value.get<std::string>() << "\"\n";
                } else {
                    file << key << " = " << value.dump() << "\n";
                }
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::saveToKeyValue(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
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
    // Validate if schema exists
    auto schemaIt = pImpl->schemas.find(key);
    if (schemaIt != pImpl->schemas.end()) {
        if (!isValueValid(value, schemaIt->second)) {
            throw ConfigValidationError("Configuration value validation failed for key: " + key);
        }
    }
    
    // Find existing entry
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            entry.value = value;
            entry.source = source;
            return;
        }
    }
    
    // Add new entry
    pImpl->entries.push_back(ConfigEntry(key, value, source));
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
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    pImpl->entries.erase(it, pImpl->entries.end());
    pImpl->schemas.erase(key);
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
    pImpl->schemas.clear();
}

bool Config::substituteVariables() {
    bool changed = false;
    for (auto& entry : pImpl->entries) {
        // Convert to string, substitute, then convert back
        std::string valueStr;
        std::visit([&valueStr](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = val;
            } else {
                valueStr = std::to_string(val);
            }
        }, entry.value);
        
        bool substituted = substituteInValue(valueStr);
        if (substituted) {
            changed = true;
            // Convert back to ConfigValue (simplified - would need better logic for complex types)
            set(entry.key, valueStr, entry.source);
        }
    }
    return changed;
}

bool Config::expandEnvironmentVariables() {
    bool changed = false;
    for (auto& entry : pImpl->entries) {
        std::string valueStr;
        std::visit([&valueStr](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = val;
            } else {
                valueStr = std::to_string(val);
            }
        }, entry.value);
        
        bool expanded = expandEnvInValue(valueStr);
        if (expanded) {
            changed = true;
            set(entry.key, valueStr, entry.source);
        }
    }
    return changed;
}

bool Config::validateSchema() const {
    for (const auto& entry : pImpl->entries) {
        auto schemaIt = pImpl->schemas.find(entry.key);
        if (schemaIt != pImpl->schemas.end()) {
            if (!isValueValid(entry.value, schemaIt->second)) {
                return false;
            }
        }
    }
    return true;
}

void Config::addSchema(const std::string& key, const ConfigSchema& schema) {
    pImpl->schemas[key] = schema;
}

ConfigSchema Config::getSchema(const std::string& key) const {
    auto it = pImpl->schemas.find(key);
    if (it != pImpl->schemas.end()) {
        return it->second;
    }
    return ConfigSchema();
}

bool Config::hasSchema(const std::string& key) const {
    return pImpl->schemas.find(key) != pImpl->schemas.end();
}

void Config::removeSchema(const std::string& key) {
    pImpl->schemas.erase(key);
}

std::string Config::detectFileFormat(const std::string& filepath) {
    if (filepath.empty()) {
        return "unknown";
    }
    
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") return "json";
    if (ext == ".yaml" || ext == ".yml") return "yaml";
    if (ext == ".cfg") return "keyvalue";
    
    return "unknown";
}

bool Config::isJsonFile(const std::string& filepath) {
    return detectFileFormat(filepath) == "json";
}

bool Config::isYamlFile(const std::string& filepath) {
    std::string format = detectFileFormat(filepath);
    return format == "yaml" || format == "yml";
}

bool Config::isKeyValueFile(const std::string& filepath) {
    return detectFileFormat(filepath) == "keyvalue";
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " 
        << pImpl->schemas.size() << " schemas):\n";
    
    oss << "  Known keys: ";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << pImpl->entries[i].key;
    }
    oss << "\n";
    
    oss << "  Validated keys: ";
    for (size_t i = 0; i < pImpl->schemas.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << pImpl->schemas.begin()->first;
    }
    oss << "\n";
    
    return oss.str();
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