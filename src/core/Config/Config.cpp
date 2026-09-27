// NLM Config.cpp - Simplified and Improved Implementation
// Implements the Config class with JSON support (Phase 2) and proper error handling

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <cmath>

namespace nlm {
namespace {

// Helper functions for string manipulation
inline bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

inline std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

inline std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Simple JSON parsing implementation (Phase 2)
class SimpleJsonParser {
public:
    static bool parseJson(const std::string& jsonStr, std::map<std::string, std::string>& outMap);
    static std::string toJson(const std::map<std::string, std::string>& data);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static std::string trimWhitespace(const std::string& str);
};

std::string SimpleJsonParser::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size() * 2);
    
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    // Control characters - use \u escape
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    result += buf;
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

std::string SimpleJsonParser::unescapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '\"': result += '\"'; i++; break;
                case '\\': result += '\\'; i++; break;
                case 'b': result += '\b'; i++; break;
                case 'f': result += '\f'; i++; break;
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                case 't': result += '\t'; i++; break;
                case 'u':
                    if (i + 5 < str.size()) {
                        // Parse unicode escape
                        std::string hex = str.substr(i + 2, 4);
                        try {
                            unsigned int code = std::stoul(hex, nullptr, 16);
                            result += static_cast<char>(code);
                            i += 5;
                        } catch (...) {
                            result += '\\'; // Keep original on error
                        }
                    } else {
                        result += '\\'; // Keep original if incomplete
                    }
                    break;
                default: result += str[i]; break; // Keep unknown escape
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string SimpleJsonParser::trimWhitespace(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

bool SimpleJsonParser::parseJson(const std::string& jsonStr, std::map<std::string, std::string>& outMap) {
    outMap.clear();
    std::string content = trimWhitespace(jsonStr);
    
    if (content.empty() || content[0] != '{') {
        return false;
    }
    
    content = content.substr(1); // Remove opening brace
    size_t pos = content.find('}');
    if (pos == std::string::npos) {
        return false;
    }
    
    std::string entries = content.substr(0, pos);
    size_t entryPos = 0;
    
    while (entryPos < entries.size()) {
        // Find next key
        size_t keyStart = entries.find('"', entryPos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = entries.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = unescapeString(entries.substr(keyStart + 1, keyEnd - keyStart - 1));
        
        // Find value separator
        size_t colonPos = entries.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valueStart = entries.find_first_not_of(" \t", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        // Parse value based on type
        std::string valueStr;
        char valueChar = entries[valueStart];
        
        if (valueChar == '"') {
            // String value
            size_t quoteEnd = entries.find('"', valueStart + 1);
            if (quoteEnd == std::string::npos) break;
            valueStr = unescapeString(entries.substr(valueStart + 1, quoteEnd - valueStart - 1));
            entryPos = quoteEnd + 1;
        } else if (valueChar == 't' || valueChar == 'f') {
            // Boolean value
            size_t endPos = entries.find_first_of(", }", valueStart);
            if (endPos == std::string::npos) break;
            valueStr = entries.substr(valueStart, endPos - valueStart);
            entryPos = endPos;
        } else if (valueChar == 'n') {
            // null value
            size_t endPos = entries.find_first_of(", }", valueStart);
            if (endPos == std::string::npos) break;
            valueStr = "null";
            entryPos = endPos;
        } else {
            // Numeric value
            size_t endPos = entries.find_first_of(", }", valueStart);
            if (endPos == std::string::npos) break;
            valueStr = entries.substr(valueStart, endPos - valueStart);
            entryPos = endPos;
        }
        
        outMap[key] = valueStr;
        
        // Skip comma and whitespace
        while (entryPos < entries.size() && (entries[entryPos] == ',' || isWhitespace(entries[entryPos]))) {
            entryPos++;
        }
    }
    
    return true;
}

std::string SimpleJsonParser::toJson(const std::map<std::string, std::string>& data) {
    std::string json = "{\n";
    bool first = true;
    
    for (const auto& pair : data) {
        if (!first) json += ",\n";
        first = false;
        
        json += "  \"" + escapeString(pair.first) + "\": ";
        
        // Determine value type and format appropriately
        if (pair.second.empty()) {
            json += "null";
        } else if (pair.second == "true" || pair.second == "false") {
            json += pair.second;
        } else if (pair.second.find_first_not_of("0123456789.-eE+") == std::string::npos) {
            // Numeric value
            json += pair.second;
        } else {
            // String value
            json += "\"" + escapeString(pair.second) + "\"";
        }
    }
    
    json += "\n}";
    return json;
}

// Helper function to convert ConfigValue to string representation
std::string configValueToString(const ConfigValue& value) {
    std::string result;
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            result = std::to_string(val);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = std::to_string(val);
        } else if constexpr (std::is_same_v<T, double>) {
            // Format double to avoid excessive precision
            std::stringstream ss;
            ss.precision(6);
            ss << std::fixed << val;
            result = ss.str();
        } else if constexpr (std::is_same_v<T, bool>) {
            result = val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = "\"" + SimpleJsonParser::escapeString(val) + "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(val[i]);
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result += ", ";
                std::stringstream ss;
                ss.precision(6);
                ss << std::fixed << val[i];
                result += ss.str();
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + SimpleJsonParser::escapeString(val[i]) + "\"";
            }
            result += "]";
        }
    }, value);
    return result;
}

}} // namespace nlm::anonymous

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // PHASE 2: JSON configuration file support
    // Support both legacy key=value format and modern JSON format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.empty()) {
        return true; // Empty file is valid
    }
    
    // Try to parse as JSON first (modern format)
    bool isJson = false;
    std::map<std::string, std::string> jsonData;
    
    // Quick check if it looks like JSON
    std::string trimmedContent = trimWhitespace(content);
    if (trimmedContent.size() >= 2 && trimmedContent.front() == '{' && 
        trimmedContent.back() == '}') {
        isJson = SimpleJsonParser::parseJson(trimmedContent, jsonData);
    }
    
    if (isJson) {
        // JSON format parsing
        for (const auto& pair : jsonData) {
            // Convert JSON string values to appropriate types
            ConfigValue configValue;
            const std::string& strValue = pair.second;
            
            if (strValue == "true" || strValue == "false") {
                configValue = (strValue == "true");
            } else if (strValue.find_first_not_of("0123456789.-eE+") == std::string::npos) {
                // Try to parse as double first (handles both int and float)
                try {
                    size_t pos;
                    double doubleVal = std::stod(strValue, &pos);
                    if (pos == strValue.size()) {
                        // Check if it looks like an integer
                        size_t pos2;
                        int64_t intVal = std::stoll(strValue, &pos2);
                        if (pos2 == strValue.size() && !strValue.empty() && strValue[0] != '0') {
                            configValue = intVal;
                        } else {
                            configValue = doubleVal;
                        }
                    } else {
                        configValue = doubleVal;
                    }
                } catch (...) {
                    configValue = strValue;
                }
            } else if (strValue == "null") {
                // Keep as string "null" for backward compatibility
                configValue = strValue;
            } else {
                configValue = strValue;
            }
            
            set(pair.first, configValue, ConfigSource::File);
        }
    } else {
        // Legacy key=value format parsing (Phase 1 style)
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
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
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write JSON format (modern)
    file << "{\n";
    bool first = true;
    
    for (const auto& entry : pImpl->entries) {
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << entry.key << "\": " << configValueToString(entry.value);
        
        // Add comment with source if not default
        if (entry.source != ConfigSource::Default) {
            file << "  // Source: " << static_cast<int>(entry.source);
        }
        file << "\n";
    }
    
    file << "}\n";
    
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

