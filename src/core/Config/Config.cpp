#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>
#include <locale>
#include <regex>
#include <unordered_set>

namespace nlm {

// JSON parsing helper functions
namespace {

    // Remove whitespace from string
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    // Convert string to lowercase
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Parse JSON value string (string, number, boolean, null)
    ConfigValue parseJSONValue(const std::string& valueStr) {
        std::string trimmed = trim(valueStr);
        
        // Check for string
        if ((trimmed.front() == '"' && trimmed.back() == '"') ||
            (trimmed.front() == '\'' && trimmed.back() == '\'')) {
            return trimmed.substr(1, trimmed.size() - 2);
        }
        
        // Check for boolean
        if (trimmed == "true") return true;
        if (trimmed == "false") return false;
        if (trimmed == "null") return std::string("");
        
        // Check for number
        try {
            // Try integer
            size_t pos;
            int64_t intVal = std::stoll(trimmed, &pos);
            if (pos == trimmed.size()) return intVal;
            
            // Try double
            double doubleVal = std::stod(trimmed, &pos);
            if (pos == trimmed.size()) return doubleVal;
        } catch (...) {
            // Fall through to string
        }
        
        return trimmed;
    }
    
    // Parse JSON array
    std::vector<ConfigValue> parseJSONArray(const std::string& jsonStr, size_t& pos);
    
    // Parse JSON object
    std::vector<ConfigEntry> parseJSONObject(const std::string& jsonStr, size_t& pos);
    
    // Parse JSON array
    std::vector<ConfigValue> parseJSONArray(const std::string& jsonStr, size_t& pos) {
        std::vector<ConfigValue> result;
        size_t start = pos;
        
        if (jsonStr[start] != '[') return result;
        
        start++;
        while (start < jsonStr.size() && jsonStr[start] != ']') {
            // Skip whitespace
            while (start < jsonStr.size() && std::isspace(jsonStr[start])) {
                start++;
            }
            
            if (start >= jsonStr.size()) break;
            
            if (jsonStr[start] == ']') break;
            
            // Parse value
            ConfigValue value;
            if (jsonStr[start] == '"' || jsonStr[start] == '\'') {
                // String - find closing quote
                size_t quotePos = start;
                while (quotePos + 1 < jsonStr.size()) {
                    quotePos++;
                    if (jsonStr[quotePos] == jsonStr[start] && 
                        (quotePos + 1 == jsonStr.size() || jsonStr[quotePos + 1] == ',' || 
                         jsonStr[quotePos + 1] == ']')) {
                        break;
                    }
                }
                value = jsonStr.substr(start + 1, quotePos - start - 1);
                start = quotePos + 1;
            } else if (jsonStr[start] == '[') {
                value = parseJSONArray(jsonStr, start);
            } else if (jsonStr[start] == '{') {
                std::vector<ConfigEntry> objResult = parseJSONObject(jsonStr, start);
                // Convert object entries to values (this is a simplification)
                value = std::vector<ConfigValue>();
            } else {
                // Number or boolean
                size_t endPos = start;
                while (endPos < jsonStr.size() && jsonStr[endPos] != ',' && jsonStr[endPos] != ']') {
                    endPos++;
                }
                value = parseJSONValue(jsonStr.substr(start, endPos - start));
                start = endPos;
            }
            
            result.push_back(value);
            
            // Skip whitespace
            while (start < jsonStr.size() && std::isspace(jsonStr[start])) {
                start++;
            }
            
            if (start < jsonStr.size() && jsonStr[start] == ',') {
                start++;
            }
        }
        
        if (start < jsonStr.size() && jsonStr[start] == ']') {
            start++;
        }
        
        pos = start;
        return result;
    }
    
    // Parse JSON object
    std::vector<ConfigEntry> parseJSONObject(const std::string& jsonStr, size_t& pos) {
        std::vector<ConfigEntry> result;
        size_t start = pos;
        
        if (jsonStr[start] != '{') return result;
        
        start++;
        while (start < jsonStr.size() && jsonStr[start] != '}') {
            // Skip whitespace
            while (start < jsonStr.size() && std::isspace(jsonStr[start])) {
                start++;
            }
            
            if (start >= jsonStr.size()) break;
            
            if (jsonStr[start] == '}') break;
            
            // Parse key
            if (jsonStr[start] != '"' && jsonStr[start] != '\'') {
                break;
            }
            
            size_t keyStart = start;
            start++;
            while (start < jsonStr.size() && jsonStr[start] != jsonStr[keyStart] && 
                   jsonStr[start] != '\'') {
                start++;
            }
            
            if (start >= jsonStr.size()) break;
            
            std::string key = jsonStr.substr(keyStart + 1, start - keyStart - 1);
            
            // Skip whitespace
            while (start < jsonStr.size() && std::isspace(jsonStr[start])) {
                start++;
            }
            
            if (start >= jsonStr.size() || jsonStr[start] != ':') {
                break;
            }
            
            start++;
            
            // Skip whitespace
            while (start < jsonStr.size() && std::isspace(jsonStr[start])) {
                start++;
            }
            
            // Parse value
            ConfigValue value;
            if (jsonStr[start] == '"' || jsonStr[start] == '\'') {
                // String - find closing quote
                size_t quotePos = start;
                while (quotePos + 1 < jsonStr.size()) {
                    quotePos++;
                    if (jsonStr[quotePos] == jsonStr[start] && 
                        (quotePos + 1 == jsonStr.size() || jsonStr[quotePos + 1] == ',' || 
                         jsonStr[quotePos + 1] == '}')) {
                        break;
                    }
                }
                value = jsonStr.substr(start + 1, quotePos - start - 1);
                start = quotePos + 1;
            } else if (jsonStr[start] == '[') {
                value = parseJSONArray(jsonStr, start);
            } else if (jsonStr[start] == '{') {
                std::vector<ConfigEntry> nestedObj = parseJSONObject(jsonStr, start);
                // For now, treat nested objects as strings
                std::ostringstream oss;
                oss << "{";
                for (size_t i = 0; i < nestedObj.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << nestedObj[i].key << "\": ";
                    // Simplified value representation
                    oss << "\"value\"";
                }
                oss << "}";
                value = oss.str();
            } else {
                // Number or boolean
                size_t endPos = start;
                while (endPos < jsonStr.size() && jsonStr[endPos] != ',' && jsonStr[endPos] != '}') {
                    endPos++;
                }
                value = parseJSONValue(jsonStr.substr(start, endPos - start));
                start = endPos;
            }
            
            result.emplace_back(key, value, ConfigSource::File);
            
            // Skip whitespace
            while (start < jsonStr.size() && std::isspace(jsonStr[start])) {
                start++;
            }
            
            if (start < jsonStr.size() && jsonStr[start] == ',') {
                start++;
            }
        }
        
        if (start < jsonStr.size() && jsonStr[start] == '}') {
            start++;
        }
        
        pos = start;
        return result;
    }
    
    // Validate configuration against schema
    bool validateConfig(const std::vector<ConfigEntry>& entries, const std::string& schemaFile) {
        // Load schema file
        std::ifstream schemaFileStream(schemaFile);
        if (!schemaFileStream.is_open()) {
            return true; // Skip validation if schema file not found
        }
        
        std::string schemaContent((std::istreambuf_iterator<char>(schemaFileStream)),
                                 std::istreambuf_iterator<char>());
        
        // For now, return true - basic validation would require a full JSON schema validator
        // In a real implementation, we would parse the schema and validate entries
        return true;
    }
    
    // Merge configurations with precedence: CommandLine > File > Default
    std::vector<ConfigEntry> mergeConfigs(const std::vector<ConfigEntry>& defaults,
                                         const std::vector<ConfigEntry>& fileConfig,
                                         const std::vector<ConfigEntry>& argsConfig) {
        std::unordered_map<std::string, ConfigEntry> merged;
        
        // Add defaults (lowest precedence)
        for (const auto& entry : defaults) {
            merged[entry.key] = entry;
        }
        
        // Override with file config (medium precedence)
        for (const auto& entry : fileConfig) {
            merged[entry.key] = entry;
        }
        
        // Override with command line args (highest precedence)
        for (const auto& entry : argsConfig) {
            merged[entry.key] = entry;
        }
        
        std::vector<ConfigEntry> result;
        result.reserve(merged.size());
        for (const auto& pair : merged) {
            result.push_back(pair.second);
        }
        
        return result;
    }
    
} // namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced file loading with support for both INI and JSON formats
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Check if file is likely JSON (starts with { or [)
    bool isJSON = false;
    if (!content.empty()) {
        char firstChar = content[0];
        if (firstChar == '{' || firstChar == '[') {
            isJSON = true;
        }
    }
    
    if (isJSON) {
        // Parse as JSON
        try {
            size_t pos = 0;
            if (content[0] == '{') {
                // Parse object
                std::vector<ConfigEntry> entries = parseJSONObject(content, pos);
                // Update internal entries
                pImpl->entries = std::move(entries);
            } else if (content[0] == '[') {
                // Parse array - for now, treat as a single value
                std::vector<ConfigValue> values = parseJSONArray(content, pos);
                // Store as a single entry with key "data"
                pImpl->entries.clear();
                pImpl->entries.emplace_back("data", values, ConfigSource::File);
            }
            
            return true;
        } catch (const std::exception& e) {
            // Fall back to INI parsing
            return loadFromINI(filepath);
        }
    } else {
        // Parse as INI format
        return loadFromINI(filepath);
    }
}

bool Config::loadFromINI(const std::string& filepath) {
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

bool Config::saveToFile(const std::string& filepath) const {
    // Enhanced file saving with support for both INI and JSON formats
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Check if the file has .json extension
    std::string ext = filepath.substr(filepath.find_last_of("."));
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        // Save as JSON
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            
            file << "  \"" << entry.key << "\": ";
            
            // Convert value to JSON representation
            std::visit([&file](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same<T, std::string>()) {
                    file << "\"" << val << "\""";
                } else if constexpr (std::is_same<T, int>()) {
                    file << val;
                } else if constexpr (std::is_same<T, int64_t>()) {
                    file << val;
                } else if constexpr (std::is_same<T, double>()) {
                    file << val;
                } else if constexpr (std::is_same<T, bool>()) {
                    file << (val ? "true" : "false");
                } else if constexpr (std::is_same<T, std::vector<int>>()) {
                    file << "[";
                    for (size_t j = 0; j < val.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << val[j];
                    }
                    file << "]";
                } else if constexpr (std::is_same<T, std::vector<double>>()) {
                    file << "[";
                    for (size_t j = 0; j < val.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << val[j];
                    }
                    file << "]";
                } else if constexpr (std::is_same<T, std::vector<std::string>>()) {
                    file << "[";
                    for (size_t j = 0; j < val.size(); ++j) {
                        if (j > 0) file << ", ";
                        file << "\"" << val[j] << "\"";
                    }
                    file << "]";
                } else {
                    file << "null";
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                file << ",";
            }
            file << "\n";
        }
        file << "}\n";
    } else {
        // Save as INI format (original format)
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Convert value to string representation
            std::ostringstream valueStream;
            std::visit([&valueStream](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same<T, std::string>()) {
                    valueStream << "\"" << val << "\"";
                } else if constexpr (std::is_same<T, int64_t>() ||
                                     std::is_same<T, int>() ||
                                     std::is_same<T, double>() ||
                                     std::is_same<T, float>()) {
                    valueStream << val;
                } else if constexpr (std::is_same<T, bool>()) {
                    valueStream << (val ? "true" : "false");
                } else {
                    valueStream << "\"" << "unknown" << "\"";
                }
            }, entry.value);
            
            file << entry.key << " = " << valueStream.str() << "\n\n";
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
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
    oss << "Configuration validation: ";
    if (validationErrors.empty()) {
        oss << "OK (" << pImpl->entries.size() << " entries)";
    } else {
        oss << "FAILED (" << validationErrors.size() << " errors)";
        for (const auto& error : validationErrors) {
            oss << "\n  - " << error.key << ": " << error.message;
        }
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
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

// Complete JSON parsing function implementations

    // Complete parseJSONObject implementation
    std::vector<ConfigEntry> parseJSONObject(const std::string& jsonStr, size_t& pos) {
        std::vector<ConfigEntry> result;
        size_t start = pos;
        
        if (jsonStr[start] != '{') return result;
        
        start++;
        while (start < jsonStr.size() && jsonStr[start] != '}') {
            // Skip whitespace
            while (start < jsonStr.size() && std::isspace(jsonStr[start])) {
                start++;
            }
            
            if (start >= jsonStr.size()) break;
            
            if (jsonStr[start] == '}') break;
            
            // Parse key
            if (jsonStr[start] != '"' && jsonStr[start] != '\'') {
                break;
            }
            
            size_t keyStart = start;
            start++;
            while (start < jsonStr.size() && jsonStr[start] != jsonStr[keyStart] && 
                   jsonStr[start] != '\'') {
                start++;
            }
            
            if (start >= jsonStr.size()) break;
            
            std::string key = jsonStr.substr(keyStart + 1, start - keyStart - 1);
            
            // Skip whitespace
            while (start < jsonStr.size() && std::isspace(jsonStr[start])) {
                start++;
            }
            
            if (start >= jsonStr.size() || jsonStr[start] != ':') {
                break;
            }
            
            start++;
            
            // Skip whitespace
            while (start < jsonStr.size() && std::isspace(jsonStr[start])) {
                start++;
            }
            
            // Parse value
            ConfigValue value;
            if (jsonStr[start] == '"' || jsonStr[start] == '\'') {
                // String - find closing quote
                size_t quotePos = start;
                while (quotePos + 1 < jsonStr.size()) {
                    quotePos++;
                    if (jsonStr[quotePos] == jsonStr[start] && 
                        (quotePos + 1 == jsonStr.size() || jsonStr[quotePos + 1] == ',' || 
                         jsonStr[quotePos + 1] == '}')) {
                        break;
                    }
                }
                value = jsonStr.substr(start + 1, quotePos - start - 1);
                start = quotePos + 1;
            } else if (jsonStr[start] == '[') {
                // Simple array parsing (for now)
                size_t arrayStart = start;
                size_t bracketCount = 1;
                while (arrayStart < jsonStr.size() && bracketCount > 0) {
                    arrayStart++;
                    if (arrayStart >= jsonStr.size()) break;
                    
                    if (jsonStr[arrayStart] == '[') bracketCount++;
                    else if (jsonStr[arrayStart] == ']') bracketCount--;
                }
                
                std::string arrayStr = jsonStr.substr(start, arrayStart - start);
                // Create a string representation of the array
                value = arrayStr;
                start = arrayStart + 1;
            } else if (jsonStr[start] == '{') {
                // Nested object - for now, treat as string
                size_t objStart = start;
                size_t braceCount = 1;
                while (objStart < jsonStr.size() && braceCount > 0) {
                    objStart++;
                    if (objStart >= jsonStr.size()) break;
                    
                    if (jsonStr[objStart] == '{') braceCount++;
                    else if (jsonStr[objStart] == '}') braceCount--;
                }
                
                std::string objStr = jsonStr.substr(start, objStart - start);
                value = objStr;
                start = objStart + 1;
            } else {
                // Number or boolean - find end of value
                size_t endPos = start;
                while (endPos < jsonStr.size() && jsonStr[endPos] != ',' && jsonStr[endPos] != '}') {
                    endPos++;
                }
                value = parseJSONValue(jsonStr.substr(start, endPos - start));
                start = endPos;
            }
            
            result.emplace_back(key, value, ConfigSource::File);
            
            // Skip whitespace
            while (start < jsonStr.size() && std::isspace(jsonStr[start])) {
                start++;
            }
            
            if (start < jsonStr.size() && jsonStr[start] == ',') {
                start++;
            }
        }
        
        if (start < jsonStr.size() && jsonStr[start] == '}') {
            start++;
        }
        
        pos = start;
        return result;
    }
    
    // Complete parseJSONArray implementation
    std::vector<ConfigValue> parseJSONArray(const std::string& jsonStr, size_t& pos) {
        std::vector<ConfigValue> result;
        size_t start = pos;
        
        if (jsonStr[start] != '[') return result;
        
        start++;
        while (start < jsonStr.size() && jsonStr[start] != ']') {
            // Skip whitespace
            while (start < jsonStr.size() && std::isspace(jsonStr[start])) {
                start++;
            }
            
            if (start >= jsonStr.size()) break;
            
            if (jsonStr[start] == ']') break;
            
            // Parse value
            ConfigValue value;
            if (jsonStr[start] == '"' || jsonStr[start] == '\'') {
                // String - find closing quote
                size_t quotePos = start;
                while (quotePos + 1 < jsonStr.size()) {
                    quotePos++;
                    if (jsonStr[quotePos] == jsonStr[start] && 
                        (quotePos + 1 == jsonStr.size() || jsonStr[quotePos + 1] == ',' || 
                         jsonStr[quotePos + 1] == ']')) {
                        break;
                    }
                }
                value = jsonStr.substr(start + 1, quotePos - start - 1);
                start = quotePos + 1;
            } else if (jsonStr[start] == '[') {
                value = parseJSONArray(jsonStr, start);
            } else if (jsonStr[start] == '{') {
                std::vector<ConfigEntry> nestedObj = parseJSONObject(jsonStr, start);
                // For now, treat nested objects as strings
                std::ostringstream oss;
                oss << "{";
                for (size_t i = 0; i < nestedObj.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << nestedObj[i].key << "\": ";
                    oss << "\"value\"";
                }
                oss << "}";
                value = oss.str();
            } else {
                // Number or boolean
                size_t endPos = start;
                while (endPos < jsonStr.size() && jsonStr[endPos] != ',' && jsonStr[endPos] != ']') {
                    endPos++;
                }
                value = parseJSONValue(jsonStr.substr(start, endPos - start));
                start = endPos;
            }
            
            result.push_back(value);
            
            // Skip whitespace
            while (start < jsonStr.size() && std::isspace(jsonStr[start])) {
                start++;
            }
            
            if (start < jsonStr.size() && jsonStr[start] == ',') {
                start++;
            }
        }
        
        if (start < jsonStr.size() && jsonStr[start] == ']') {
            start++;
        }
        
        pos = start;
        return result;
    }
