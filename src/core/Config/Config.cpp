#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <map>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Format detection
std::string Config::detectFormat(const std::string& content) {
    if (content.empty()) {
        return "unknown";
    }
    
    // Skip whitespace for detection
    std::string trimmed = trim(content);
    if (trimmed.empty()) {
        return "unknown";
    }
    
    // JSON format detection
    if (trimmed[0] == '{' || trimmed[0] == '[') {
        return "json";
    }
    
    // YAML format detection - check for YAML-like patterns
    // YAML files typically end with .yaml/.yml but we can also detect
    // by looking for key: value pairs with indentation
    return "yaml";
}

bool Config::isJsonFormat(const std::string& content) {
    return detectFormat(content) == "json";
}

bool Config::isYamlFormat(const std::string& content) {
    return detectFormat(content) == "yaml";
}

// Trim whitespace
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Convert to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Simple JSON parser (fallback implementation)
JsonValue Config::parseJson(const std::string& content) {
    JsonValue result;
    size_t pos = 0;
    
    // Skip whitespace
    while (pos < content.size() && std::isspace(content[pos])) pos++;
    
    if (pos >= content.size()) {
        return std::monostate{}; // null
    }
    
    char c = content[pos];
    
    if (c == '{') {
        // Object
        std::unordered_map<std::string, JsonValue> obj;
        pos++; // Skip {
        
        while (pos < content.size()) {
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            if (pos >= content.size() || content[pos] == '}') {
                pos++; // Skip }
                break;
            }
            
            // Parse key
            size_t keyStart = pos;
            while (pos < content.size() && content[pos] != ':' && content[pos] != '"' && !std::isspace(content[pos])) {
                pos++;
            }
            
            std::string key = content.substr(keyStart, pos - keyStart);
            
            // Skip whitespace and colon
            while (pos < content.size() && (std::isspace(content[pos]) || content[pos] == ':')) pos++;
            
            // Parse value
            JsonValue value = parseJson(content.substr(pos));
            obj[key] = value;
            
            // Skip whitespace and comma
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos < content.size() && content[pos] == ',') {
                pos++; // Skip comma
            }
        }
        
        result = std::move(obj);
    } else if (c == '[') {
        // Array
        std::vector<JsonValue> arr;
        pos++; // Skip [
        
        while (pos < content.size()) {
            // Skip whitespace
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            
            if (pos >= content.size() || content[pos] == ']') {
                pos++; // Skip ]
                break;
            }
            
            // Parse element
            JsonValue element = parseJson(content.substr(pos));
            arr.push_back(element);
            
            // Skip whitespace and comma
            while (pos < content.size() && std::isspace(content[pos])) pos++;
            if (pos < content.size() && content[pos] == ',') {
                pos++; // Skip comma
            }
        }
        
        result = std::move(arr);
    } else if (c == '"') {
        // String
        size_t quotePos = pos + 1;
        while (quotePos < content.size()) {
            if (content[quotePos] == '\\' && quotePos + 1 < content.size()) {
                quotePos += 2; // Skip escaped character
            } else if (content[quotePos] == '"') {
                break;
            }
            quotePos++;
        }
        
        if (quotePos < content.size()) {
            std::string str = content.substr(pos + 1, quotePos - pos - 1);
            // Simple unescape
            std::string resultStr;
            for (size_t i = 0; i < str.size(); ++i) {
                if (str[i] == '\\' && i + 1 < str.size()) {
                    resultStr += str[++i];
                } else {
                    resultStr += str[i];
                }
            }
            result = resultStr;
            pos = quotePos + 1;
        }
    } else if (c == 't' || c == 'f') {
        // Boolean
        if (content.substr(pos, 4) == "true") {
            result = true;
            pos += 4;
        } else if (content.substr(pos, 5) == "false") {
            result = false;
            pos += 5;
        }
    } else if (c == 'n') {
        // null
        if (content.substr(pos, 4) == "null") {
            result = std::monostate{};
            pos += 4;
        }
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        // Number
        size_t numEnd = pos;
        while (numEnd < content.size() && (content[numEnd] == '-' || content[numEnd] == '+' || 
               (content[numEnd] >= '0' && content[numEnd] <= '9') ||
               content[numEnd] == '.' || content[numEnd] == 'e' || content[numEnd] == 'E')) {
            numEnd++;
        }
        
        std::string numStr = content.substr(pos, numEnd - pos);
        try {
            // Try to parse as integer first
            size_t end;
            long long intVal = std::stoll(numStr, &end);
            if (end == numStr.size()) {
                result = int64_t(intVal);
                pos = numEnd;
            } else {
                // Parse as double
                double doubleVal = std::stod(numStr);
                result = doubleVal;
                pos = numEnd;
            }
        } catch (...) {
            result = 0.0;
            pos = numEnd;
        }
    }
    
    return result;
}

// Simple YAML parser (basic implementation)
JsonValue Config::parseYaml(const std::string& content) {
    JsonValue result;
    size_t pos = 0;
    
    // For now, delegate to JSON parser for simplicity
    // TODO: Implement proper YAML parsing
    return parseJson(content);
}

// Convert JsonValue to ConfigValue
ConfigValue Config::convertJsonValue(const JsonValue& jsonVal) {
    if (std::holds_alternative<std::monostate>(jsonVal)) {
        return std::string("null");
    } else if (std::holds_alternative<bool>(jsonVal)) {
        return std::get<bool>(jsonVal);
    } else if (std::holds_alternative<double>(jsonVal)) {
        double val = std::get<double>(jsonVal);
        // Check if it's an integer
        if (val == static_cast<int64_t>(val)) {
            return static_cast<int64_t>(val);
        }
        return val;
    } else if (std::holds_alternative<std::string>(jsonVal)) {
        return std::get<std::string>(jsonVal);
    } else if (std::holds_alternative<std::vector<JsonValue>>(jsonVal)) {
        const auto& arr = std::get<std::vector<JsonValue>>(jsonVal);
        std::vector<ConfigValue> configArr;
        configArr.reserve(arr.size());
        for (const auto& elem : arr) {
            configArr.push_back(convertJsonValue(elem));
        }
        
        // Determine array type based on content
        if (!configArr.empty()) {
            if (std::all_of(configArr.begin(), configArr.end(),
                          [](const ConfigValue& v) { return std::holds_alternative<int>(v); })) {
                std::vector<int> intArr;
                intArr.reserve(configArr.size());
                for (const auto& v : configArr) {
                    intArr.push_back(std::get<int>(v));
                }
                return intArr;
            } else if (std::all_of(configArr.begin(), configArr.end(),
                                 [](const ConfigValue& v) { return std::holds_alternative<double>(v); })) {
                std::vector<double> doubleArr;
                doubleArr.reserve(configArr.size());
                for (const auto& v : configArr) {
                    doubleArr.push_back(std::get<double>(v));
                }
                return doubleArr;
            } else if (std::all_of(configArr.begin(), configArr.end(),
                                 [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); })) {
                std::vector<std::string> stringArr;
                stringArr.reserve(configArr.size());
                for (const auto& v : configArr) {
                    stringArr.push_back(std::get<std::string>(v));
                }
                return stringArr;
            }
        }
        
        // Default to string array
        std::vector<std::string> stringArr;
        stringArr.reserve(configArr.size());
        for (const auto& v : configArr) {
            stringArr.push_back(std::visit([](auto&& arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) return std::to_string(arg);
                else if constexpr (std::is_same_v<T, double>) return std::to_string(arg);
                else if constexpr (std::is_same_v<T, bool>) return arg ? "true" : "false";
                else if constexpr (std::is_same_v<T, std::string>) return arg;
                else return "";
            }, v));
        }
        return stringArr;
    } else if (std::holds_alternative<std::unordered_map<std::string, JsonValue>>(jsonVal)) {
        // Convert object to a flattened key.path=value format
        const auto& obj = std::get<std::unordered_map<std::string, JsonValue>>(jsonVal);
        std::string flattened;
        for (const auto& pair : obj) {
            if (!flattened.empty()) flattened += " ";
            flattened += pair.first + "=" + jsonValueToString(pair.second);
        }
        return flattened;
    }
    
    return std::string();
}

// Convert JsonValue to string for nested object handling
std::string Config::jsonValueToString(const JsonValue& jsonVal) {
    if (std::holds_alternative<std::monostate>(jsonVal)) {
        return "null";
    } else if (std::holds_alternative<bool>(jsonVal)) {
        return std::get<bool>(jsonVal) ? "true" : "false";
    } else if (std::holds_alternative<double>(jsonVal)) {
        return std::to_string(std::get<double>(jsonVal));
    } else if (std::holds_alternative<std::string>(jsonVal)) {
        return "\"" + std::get<std::string>(jsonVal) + "\"";
    } else if (std::holds_alternative<std::vector<JsonValue>>(jsonVal)) {
        const auto& arr = std::get<std::vector<JsonValue>>(jsonVal);
        std::string result = "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) result += ", ";
            result += jsonValueToString(arr[i]);
        }
        result += "]";
        return result;
    } else if (std::holds_alternative<std::unordered_map<std::string, JsonValue>>(jsonVal)) {
        const auto& obj = std::get<std::unordered_map<std::string, JsonValue>>(jsonVal);
        std::string result = "{";
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            if (it != obj.begin()) result += ", ";
            result += "\"" + it->first + "\": " + jsonValueToString(it->second);
        }
        result += "}";
        return result;
    }
    
    return "";
}

// Apply JSON value to config with path
void Config::applyJsonValue(const std::string& path, const JsonValue& jsonVal, Config& config, ConfigSource source) {
    if (path.empty()) {
        return;
    }
    
    // Split path by dots
    std::vector<std::string> pathComponents;
    std::string currentPath = path;
    size_t dotPos;
    
    while ((dotPos = currentPath.find('.')) != std::string::npos) {
        pathComponents.push_back(currentPath.substr(0, dotPos));
        currentPath = currentPath.substr(dotPos + 1);
    }
    pathComponents.push_back(currentPath);
    
    if (pathComponents.size() == 1) {
        // Simple key-value
        std::string key = pathComponents[0];
        ConfigValue value = convertJsonValue(jsonVal);
        config.set(key, value, source);
    } else {
        // Nested structure - flatten to dot notation
        std::string nestedKey = path;
        ConfigValue value = convertJsonValue(jsonVal);
        config.set(nestedKey, value, source);
    }
}

// Load from file implementation
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return loadFromString(content, ConfigSource::File);
}

// Load from string implementation
bool Config::loadFromString(const std::string& content, ConfigSource source) {
    if (content.empty()) {
        return true; // Empty file is valid
    }
    
    std::string format = detectFormat(content);
    
    try {
        JsonValue jsonVal;
        
        if (format == "json") {
            jsonVal = parseJson(content);
        } else {
            jsonVal = parseYaml(content);
        }
        
        // If it's an array, handle each element
        if (std::holds_alternative<std::vector<JsonValue>>(jsonVal)) {
            const auto& arr = std::get<std::vector<JsonValue>>(jsonVal);
            for (size_t i = 0; i < arr.size(); ++i) {
                std::string key = "item_" + std::to_string(i);
                JsonValue element = arr[i];
                
                // If element is an object with a "name" field, use that as key
                if (std::holds_alternative<std::unordered_map<std::string, JsonValue>>(element)) {
                    const auto& obj = std::get<std::unordered_map<std::string, JsonValue>>(element);
                    auto nameIt = obj.find("name");
                    if (nameIt != obj.end() && std::holds_alternative<std::string>(nameIt->second)) {
                        key = std::get<std::string>(nameIt->second);
                        // Remove the name field from the config to avoid duplication
                        for (const auto& pair : obj) {
                            if (pair.first != "name") {
                                std::string nestedPath = key + "." + pair.first;
                                applyJsonValue(nestedPath, pair.second, *this, source);
                            }
                        }
                        continue;
                    }
                }
                
                applyJsonValue(key, element, *this, source);
            }
        } else if (std::holds_alternative<std::unordered_map<std::string, JsonValue>>(jsonVal)) {
            // Handle object - each key-value pair
            const auto& obj = std::get<std::unordered_map<std::string, JsonValue>>(jsonVal);
            for (const auto& pair : obj) {
                applyJsonValue(pair.first, pair.second, *this, source);
            }
        } else {
            // Simple value
            applyJsonValue("value", jsonVal, *this, source);
        }
        
        return true;
    } catch (const ConfigParseError& e) {
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

// Load from command line arguments
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

// Save to file implementation
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": " << jsonSerializeValue(entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    
    return true;
}

// JSON serialization of ConfigValue
std::string Config::jsonSerializeValue(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            // Format double without trailing zeros
            std::stringstream ss;
            ss << std::fixed << arg;
            std::string result = ss.str();
            // Remove trailing zeros
            size_t dotPos = result.find('.');
            if (dotPos != std::string::npos) {
                // Remove trailing zeros
                size_t endPos = result.size() - 1;
                while (endPos > dotPos && result[endPos] == '0') {
                    endPos--;
                }
                if (result[endPos] == '.') {
                    endPos--;
                }
                result = result.substr(0, endPos + 1);
            }
            return result;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            // Escape quotes and backslashes
            std::string escaped = arg;
            size_t pos = 0;
            while ((pos = escaped.find('"', pos)) != std::string::npos) {
                escaped.replace(pos, 1, "\\\"");
                pos += 2;
            }
            pos = 0;
            while ((pos = escaped.find('\\', pos)) != std::string::npos) {
                escaped.replace(pos, 1, "\\\\");
                pos += 2;
            }
            return "\"" + escaped + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(arg[i]);
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                // Format double
                std::stringstream ss;
                ss << std::fixed << arg[i];
                std::string numStr = ss.str();
                // Remove trailing zeros
                size_t dotPos = numStr.find('.');
                if (dotPos != std::string::npos) {
                    size_t endPos = numStr.size() - 1;
                    while (endPos > dotPos && numStr[endPos] == '0') {
                        endPos--;
                    }
                    if (numStr[endPos] == '.') {
                        endPos--;
                    }
                    numStr = numStr.substr(0, endPos + 1);
                }
                result += numStr;
            }
            result += "]";
            return result;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::string result = "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + arg[i] + "\"";
            }
            result += "]";
            return result;
        } else {
            return "\"" + std::to_string(arg) + "\"";
        }
    }, value);
}

// Set implementation for string
void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Set implementation for int
void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Set implementation for double
void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Set implementation for bool
void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Set implementation for int vector
void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Set implementation for double vector
void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Set implementation for string vector
void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Set implementation for ConfigValue
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
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

// Explicit template instantiations
// Note: We've already defined the implementations for all types
// These lines serve as explicit instantiations for the linker
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
EOF