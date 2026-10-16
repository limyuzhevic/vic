#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

namespace nlm {

// JSON parsing helper functions
namespace json {
    
    // Skip whitespace in JSON string
    size_t skipWhitespace(const std::string& str, size_t pos) {
        while (pos < str.size() && std::isspace(str[pos])) {
            pos++;
        }
        return pos;
    }
    
    // Parse JSON string literal
    std::pair<size_t, std::string> parseString(const std::string& str, size_t pos) {
        size_t start = pos;
        if (pos < str.size() && str[pos] == '"') {
            pos++;
            std::string result;
            while (pos < str.size() && str[pos] != '"') {
                if (str[pos] == '\\' && pos + 1 < str.size()) {
                    pos++;
                    switch (str[pos]) {
                        case '\\': result += '\\'; break;
                        case '\"': result += '"'; break;
                        case '/': result += '/'; break;
                        case 'b': result += '\b'; break;
                        case 'f': result += '\f'; break;
                        case 'n': result += '\n'; break;
                        case 'r': result += '\r'; break;
                        case 't': result += '\t'; break;
                        default: result += str[pos]; break;
                    }
                    pos++;
                } else if (str[pos] == '\n' || str[pos] == '\r') {
                    throw std::runtime_error("Invalid escape sequence in JSON string at position " + std::to_string(pos));
                } else {
                    result += str[pos];
                    pos++;
                }
            }
            if (pos >= str.size() || str[pos] != '"') {
                throw std::runtime_error("Unterminated string literal in JSON at position " + std::to_string(start));
            }
            return {pos + 1, result};
        }
        throw std::runtime_error("Expected string literal at position " + std::to_string(pos));
    }
    
    // Parse JSON number (integer, double, or scientific notation)
    std::pair<size_t, ConfigValue> parseNumber(const std::string& str, size_t pos) {
        size_t start = pos;
        bool isDouble = false;
        bool isNegative = false;
        
        if (str[pos] == '-') {
            isNegative = true;
            pos++;
        }
        
        // Check for 0
        if (str[pos] == '0') {
            pos++;
            if (pos < str.size() && (str[pos] == '.' || str[pos] == 'e' || str[pos] == 'E')) {
                isDouble = true;
            }
        } else {
            // Parse integer part
            while (pos < str.size() && std::isdigit(str[pos])) {
                pos++;
            }
            
            // Check for fractional part
            if (pos < str.size() && str[pos] == '.') {
                isDouble = true;
                pos++;
                while (pos < str.size() && std::isdigit(str[pos])) {
                    pos++;
                }
            }
            
            // Check for exponent part
            if (pos < str.size() && (str[pos] == 'e' || str[pos] == 'E')) {
                isDouble = true;
                pos++;
                if (pos < str.size() && (str[pos] == '+' || str[pos] == '-')) {
                    pos++;
                }
                while (pos < str.size() && std::isdigit(str[pos])) {
                    pos++;
                }
            }
        }
        
        std::string numStr = str.substr(start, pos - start);
        
        if (isDouble) {
            try {
                return {pos, std::stod(numStr)};
            } catch (const std::exception&) {
                throw std::runtime_error("Invalid number format in JSON: " + numStr);
            }
        } else {
            try {
                if (numStr.size() > 19) { // > int64 range
                    return {pos, std::stoll(numStr)};
                }
                long long intVal = std::stoll(numStr);
                if (intVal >= INT_MIN && intVal <= INT_MAX) {
                    return {pos, static_cast<int>(intVal)};
                }
                return {pos, intVal};
            } catch (const std::exception&) {
                throw std::runtime_error("Invalid integer format in JSON: " + numStr);
            }
        }
    }
    
    // Parse JSON boolean
    std::pair<size_t, ConfigValue> parseBoolean(const std::string& str, size_t pos) {
        if (str.compare(pos, 4, "true") == 0) {
            return {pos + 4, true};
        } else if (str.compare(pos, 5, "false") == 0) {
            return {pos + 5, false};
        }
        throw std::runtime_error("Invalid boolean value at position " + std::to_string(pos));
    }
    
    // Parse JSON null
    std::pair<size_t, ConfigValue> parseNull(const std::string& str, size_t pos) {
        if (str.compare(pos, 4, "null") == 0) {
            return {pos + 4, ""}; // Use empty string for null since ConfigValue doesn't have null type
        }
        throw std::runtime_error("Expected null value at position " + std::to_string(pos));
    }
    
    // Parse JSON array
    std::pair<size_t, ConfigValue> parseArray(const std::string& str, size_t pos);
    
    // Parse JSON object
    std::pair<size_t, ConfigValue> parseObject(const std::string& str, size_t pos);
    
    // Parse any JSON value
    std::pair<size_t, ConfigValue> parseValue(const std::string& str, size_t pos) {
        pos = skipWhitespace(str, pos);
        
        if (pos >= str.size()) {
            throw std::runtime_error("Unexpected end of input while parsing JSON value");
        }
        
        switch (str[pos]) {
            case '"':
                return parseString(str, pos);
            case 't': case 'f':
                return parseBoolean(str, pos);
            case 'n':
                return parseNull(str, pos);
            case '[':
                return parseArray(str, pos);
            case '{':
                return parseObject(str, pos);
            default:
                if (std::isdigit(str[pos]) || str[pos] == '-') {
                    return parseNumber(str, pos);
                }
                throw std::runtime_error("Unexpected character '" + std::string(1, str[pos]) + "' at position " + std::to_string(pos));
        }
    }
    
    // Parse JSON array
    std::pair<size_t, ConfigValue> parseArray(const std::string& str, size_t pos) {
        if (pos >= str.size() || str[pos] != '[') {
            throw std::runtime_error("Expected '[' at position " + std::to_string(pos));
        }
        
        pos++;
        pos = skipWhitespace(str, pos);
        
        if (pos < str.size() && str[pos] == ']') {
            return {pos + 1, std::vector<std::string>{}; // Empty array
        }
        
        std::vector<std::string> arrayStrs;
        
        while (true) {
            auto [newPos, value] = parseValue(str, pos);
            
            // Convert value to string representation for storage
            if (std::holds_alternative<int>(value)) {
                arrayStrs.push_back(std::to_string(std::get<int>(value)));
            } else if (std::holds_alternative<double>(value)) {
                arrayStrs.push_back(std::to_string(std::get<double>(value)));
            } else if (std::holds_alternative<bool>(value)) {
                arrayStrs.push_back(std::get<bool>(value) ? "true" : "false");
            } else if (std::holds_alternative<std::string>(value)) {
                arrayStrs.push_back('"' + std::get<std::string>(value) + '"');
            }
            
            pos = skipWhitespace(str, newPos);
            
            if (pos >= str.size()) {
                throw std::runtime_error("Unexpected end of input while parsing JSON array");
            }
            
            if (str[pos] == ']') {
                return {pos + 1, arrayStrs};
            } else if (str[pos] == ',') {
                pos++;
                pos = skipWhitespace(str, pos);
                continue;
            } else {
                throw std::runtime_error("Expected ',' or ']' in JSON array at position " + std::to_string(pos));
            }
        }
    }
    
    // Parse JSON object
    std::pair<size_t, ConfigValue> parseObject(const std::string& str, size_t pos) {
        if (pos >= str.size() || str[pos] != '{') {
            throw std::runtime_error("Expected '{' at position " + std::to_string(pos));
        }
        
        pos++;
        pos = skipWhitespace(str, pos);
        
        std::unordered_map<std::string, std::string> objectMap;
        
        if (pos < str.size() && str[pos] == '}') {
            return {pos + 1, objectMap}; // Empty object
        }
        
        while (true) {
            // Parse key (must be a string)
            auto [newPos, key] = parseString(str, skipWhitespace(str, pos));
            pos = skipWhitespace(str, newPos);
            
            // Skip whitespace before colon
            pos = skipWhitespace(str, pos);
            
            if (pos >= str.size() || str[pos] != ':') {
                throw std::runtime_error("Expected ':' after key at position " + std::to_string(pos));
            }
            pos++;
            pos = skipWhitespace(str, pos);
            
            // Parse value
            auto [valuePos, value] = parseValue(str, pos);
            
            // Convert value to string representation
            std::string valueStr;
            if (std::holds_alternative<int>(value)) {
                valueStr = std::to_string(std::get<int>(value));
            } else if (std::holds_alternative<double>(value)) {
                valueStr = std::to_string(std::get<double>(value));
            } else if (std::holds_alternative<bool>(value)) {
                valueStr = std::get<bool>(value) ? "true" : "false";
            } else if (std::holds_alternative<std::string>(value)) {
                valueStr = '"' + std::get<std::string>(value) + '"';
            }
            
            objectMap[key] = valueStr;
            
            pos = skipWhitespace(str, valuePos);
            
            if (pos >= str.size()) {
                throw std::runtime_error("Unexpected end of input while parsing JSON object");
            }
            
            if (str[pos] == '}') {
                return {pos + 1, objectMap};
            } else if (str[pos] == ',') {
                pos++;
                pos = skipWhitespace(str, pos);
                continue;
            } else {
                throw std::runtime_error("Expected ',' or '}' in JSON object at position " + std::to_string(pos));
            }
        }
    }
    
    // Parse a simple key=value line (for backward compatibility)
    ConfigEntry parseSimpleEntry(const std::string& line) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            throw std::runtime_error("Invalid simple format: missing '=' in line: " + line);
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        ConfigSource source = ConfigSource::File;
        return {key, ConfigValue(value), source, ""};
    }
} // namespace json

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

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

std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    result += "\\\\u";
                    char hex[5];
                    snprintf(hex, sizeof(hex), "%04x", static_cast<unsigned int>(c));
                    result += hex;
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file into memory
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.empty()) {
        return true; // Empty file is valid
    }
    
    pImpl->entries.clear();
    
    try {
        // First try to parse as JSON
        size_t pos = json::skipWhitespace(content, 0);
        
        // Check if this is JSON (starts with { or [)
        if (pos < content.size() && (content[pos] == '{' || content[pos] == '[')) {
            // Parse as JSON object or array
            auto [endPos, value] = json::parseValue(content, pos);
            
            pos = json::skipWhitespace(content, endPos);
            if (pos != content.size()) {
                // Extra content after JSON
                return false;
            }
            
            if (std::holds_alternative<std::unordered_map<std::string, std::string>>(value)) {
                // Parse JSON object
                auto& obj = std::get<std::unordered_map<std::string, std::string>>(value);
                for (const auto& pair : obj) {
                    // Parse the value string back to appropriate type
                    ConfigValue parsedValue;
                    try {
                        if (pair.second.size() >= 2 && pair.second.front() == '\"' && pair.second.back() == '\"') {
                            parsedValue = pair.second.substr(1, pair.second.size() - 2);
                        } else if (pair.second == "true") {
                            parsedValue = true;
                        } else if (pair.second == "false") {
                            parsedValue = false;
                        } else {
                            // Try to parse as number
                            try {
                                size_t posEnd;
                                double doubleVal = std::stod(pair.second, &posEnd);
                                if (posEnd == pair.second.size()) {
                                    // Check if it's an integer
                                    try {
                                        long long intVal = std::stoll(pair.second);
                                        if (intVal >= INT_MIN && intVal <= INT_MAX) {
                                            parsedValue = static_cast<int>(intVal);
                                        } else {
                                            parsedValue = intVal;
                                        }
                                    } catch (...) {
                                        parsedValue = doubleVal;
                                    }
                                } else {
                                    parsedValue = doubleVal;
                                }
                            } catch (...) {
                                parsedValue = pair.second;
                            }
                        }
                        pImpl->entries.emplace_back(pair.first, parsedValue, ConfigSource::File);
                    } catch (const std::exception& e) {
                        // If parsing fails, use the raw string value
                        pImpl->entries.emplace_back(pair.first, pair.second, ConfigSource::File);
                    }
                }
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                // Parse JSON array - store as vector of strings
                auto& arr = std::get<std::vector<std::string>>(value);
                // For arrays, we'll use the key "array" and store the array values
                // This is a limitation - arrays would be better as a separate ConfigEntry type
                // For now, we'll store as a comma-separated string
                std::string arrayStr;
                for (size_t i = 0; i < arr.size(); ++i) {
                    if (i > 0) arrayStr += ",";
                    arrayStr += arr[i];
                }
                pImpl->entries.emplace_back("array", arrayStr, ConfigSource::File);
            }
        } else {
            // Parse as backward-compatible simple format (key=value)
            std::string line;
            std::stringstream contentStream(content);
            while (std::getline(contentStream, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                // Parse simple key=value pair
                try {
                    ConfigEntry entry = json::parseSimpleEntry(line);
                    pImpl->entries.push_back(entry);
                } catch (const std::exception& e) {
                    // Skip malformed lines but continue parsing
                    continue;
                }
            }
        }
    } catch (const std::exception& e) {
        // JSON parsing failed, fall back to simple format if available
        pImpl->entries.clear();
        std::string line;
        std::stringstream contentStream(content);
        while (std::getline(contentStream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            try {
                ConfigEntry entry = json::parseSimpleEntry(line);
                pImpl->entries.push_back(entry);
            } catch (const std::exception& e) {
                // Skip malformed lines
                continue;
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
    
    // Save as JSON format
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Serialize the ConfigValue based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, int> || 
                          std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Check if it's an integer value
                if (std::abs(arg - std::round(arg)) < 1e-9) {
                    file << static_cast<long long>(arg);
                } else {
                    file << arg;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Escape string for JSON
                file << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    // Check if it's an integer value
                    if (std::abs(arg[j] - std::round(arg[j])) < 1e-9) {
                        file << static_cast<long long>(arg[j]);
                    } else {
                        file << arg[j];
                    }
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escapeString(arg[j]) << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    return true;
}
return result;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file into memory
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.empty()) {
        return true; // Empty file is valid
    }
    
    pImpl->entries.clear();
    
    try {
        // First try to parse as JSON
        size_t pos = json::skipWhitespace(content, 0);
        
        // Check if this is JSON (starts with { or [)
        if (pos < content.size() && (content[pos] == '{' || content[pos] == '[')) {
            // Parse as JSON object or array
            auto [endPos, value] = json::parseValue(content, pos);
            
            pos = json::skipWhitespace(content, endPos);
            if (pos != content.size()) {
                // Extra content after JSON
                return false;
            }
            
            if (std::holds_alternative<std::unordered_map<std::string, std::string>>(value)) {
                // Parse JSON object
                auto& obj = std::get<std::unordered_map<std::string, std::string>>(value);
                for (const auto& pair : obj) {
                    // Parse the value string back to appropriate type
                    ConfigValue parsedValue;
                    try {
                        if (pair.second.size() >= 2 && pair.second.front() == '\"' && pair.second.back() == '\"') {
                            parsedValue = pair.second.substr(1, pair.second.size() - 2);
                        } else if (pair.second == "true") {
                            parsedValue = true;
                        } else if (pair.second == "false") {
                            parsedValue = false;
                        } else {
                            // Try to parse as number
                            try {
                                size_t posEnd;
                                double doubleVal = std::stod(pair.second, &posEnd);
                                if (posEnd == pair.second.size()) {
                                    // Check if it's an integer
                                    try {
                                        long long intVal = std::stoll(pair.second);
                                        if (intVal >= INT_MIN && intVal <= INT_MAX) {
                                            parsedValue = static_cast<int>(intVal);
                                        } else {
                                            parsedValue = intVal;
                                        }
                                    } catch (...) {
                                        parsedValue = doubleVal;
                                    }
                                } else {
                                    parsedValue = doubleVal;
                                }
                            } catch (...) {
                                parsedValue = pair.second;
                            }
                        }
                        pImpl->entries.emplace_back(pair.first, parsedValue, ConfigSource::File);
                    } catch (const std::exception& e) {
                        // If parsing fails, use the raw string value
                        pImpl->entries.emplace_back(pair.first, pair.second, ConfigSource::File);
                    }
                }
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                // Parse JSON array - store as vector of strings
                auto& arr = std::get<std::vector<std::string>>(value);
                // For arrays, we'll use the key "array" and store the array values
                // This is a limitation - arrays would be better as a separate ConfigEntry type
                // For now, we'll store as a comma-separated string
                std::string arrayStr;
                for (size_t i = 0; i < arr.size(); ++i) {
                    if (i > 0) arrayStr += ",";
                    arrayStr += arr[i];
                }
                pImpl->entries.emplace_back("array", arrayStr, ConfigSource::File);
            }
        } else {
            // Parse as backward-compatible simple format (key=value)
            std::string line;
            std::stringstream contentStream(content);
            while (std::getline(contentStream, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                // Parse simple key=value pair
                try {
                    ConfigEntry entry = json::parseSimpleEntry(line);
                    pImpl->entries.push_back(entry);
                } catch (const std::exception& e) {
                    // Skip malformed lines but continue parsing
                    continue;
                }
            }
        }
    } catch (const std::exception& e) {
        // JSON parsing failed, fall back to simple format if available
        pImpl->entries.clear();
        std::string line;
        std::stringstream contentStream(content);
        while (std::getline(contentStream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            try {
                ConfigEntry entry = json::parseSimpleEntry(line);
                pImpl->entries.push_back(entry);
            } catch (const std::exception& e) {
                // Skip malformed lines
                continue;
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
    
    // Save as JSON format
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Serialize the ConfigValue based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, int> || 
                          std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Check if it's an integer value
                if (std::abs(arg - std::round(arg)) < 1e-9) {
                    file << static_cast<long long>(arg);
                } else {
                    file << arg;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Escape string for JSON
                file << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    // Check if it's an integer value
                    if (std::abs(arg[j] - std::round(arg[j])) < 1e-9) {
                        file << static_cast<long long>(arg[j]);
                    } else {
                        file << arg[j];
                    }
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escapeString(arg[j]) << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    return true;
}

// Explicit template instantiations
