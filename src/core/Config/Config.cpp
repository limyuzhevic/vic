#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <cctype>
#include <stack>
#include <queue>

// Helper functions for JSON parsing
namespace {
    
    // Remove comments from JSON string (simple line comment removal)
    std::string removeJsonComments(const std::string& input) {
        std::string result;
        bool inString = false;
        bool escaped = false;
        
        for (size_t i = 0; i < input.size(); ++i) {
            char c = input[i];
            
            if (!inString && c == '/') {
                // Check if it's a comment start
                if (i + 1 < input.size()) {
                    if (input[i + 1] == '/' || input[i + 1] == '*') {
                        // Skip comment
                        while (i + 1 < input.size() && !(input[i + 1] == '\n' && input[i] == '/')) {
                            i++;
                        }
                        if (i + 1 < input.size() && input[i + 1] == '*') {
                            // Skip block comment
                            while (i + 1 < input.size() && !(input[i + 1] == '*' && input[i] == '/')) {
                                i++;
                            }
                            if (i + 1 < input.size()) {
                                i++; // Skip closing */
                            }
                        }
                        continue;
                    }
                }
            }
            
            // Handle string escaping
            if (c == '\\' && inString) {
                escaped = true;
                result += c;
                continue;
            }
            
            if (c == '"' && !escaped) {
                inString = !inString;
                result += c;
            } else {
                result += c;
            }
            
            escaped = false;
        }
        
        return result;
    }
    
    // Check if string is a JSON number (integer or float)
    bool isJsonNumber(const std::string& str) {
        if (str.empty() || (str[0] == '-' && str.size() == 1)) {
            return false;
        }
        
        bool hasDecimal = false;
        bool hasExponent = false;
        bool hasSign = false;
        
        for (size_t i = 0; i < str.size(); ++i) {
            char c = str[i];
            if (c == '-' || c == '+') {
                if (i != 0) return false;
                hasSign = true;
            } else if (c == '.') {
                if (hasDecimal || hasExponent) return false;
                hasDecimal = true;
            } else if (c == 'e' || c == 'E') {
                if (hasExponent) return false;
                hasExponent = true;
                hasDecimal = true; // Allow scientific notation without decimal
            } else if (!std::isdigit(static_cast<unsigned char>(c))) {
                return false;
            }
        }
        
        return true;
    }
    
    // Parse JSON value from string (simple implementation)
    std::string parseJsonValue(const std::string& str, size_t& pos) {
        // Skip whitespace
        while (pos < str.size() && std::isspace(static_cast<unsigned char>(str[pos]))) {
            pos++;
        }
        
        if (pos >= str.size()) {
            throw std::runtime_error("Unexpected end of JSON input");
        }
        
        char c = str[pos];
        
        if (c == '"') {
            // String value
            pos++; // Skip opening quote
            size_t start = pos;
            while (pos < str.size() && str[pos] != '"') {
                if (str[pos] == '\\' && pos + 1 < str.size()) {
                    pos += 2; // Skip escaped character
                    continue;
                }
                pos++;
            }
            if (pos >= str.size()) {
                throw std::runtime_error("Unterminated string in JSON");
            }
            std::string result = str.substr(start, pos - start);
            pos++; // Skip closing quote
            return result;
        } else if (c == 't' || c == 'f') {
            // Boolean value
            if (str.substr(pos, 4) == "true") {
                pos += 4;
                return "true";
            } else if (str.substr(pos, 5) == "false") {
                pos += 5;
                return "false";
            } else {
                throw std::runtime_error("Invalid JSON boolean value");
            }
        } else if (c == 'n') {
            // Null value
            if (str.substr(pos, 4) == "null") {
                pos += 4;
                return "null";
            } else {
                throw std::runtime_error("Invalid JSON null value");
            }
        } else if (c == '[') {
            // Array - for now, treat as string for compatibility
            pos++; // Skip opening bracket
            size_t start = pos;
            while (pos < str.size() && str[pos] != ']') {
                if (str[pos] == ',' || str[pos] == ' ') {
                    // Skip elements and commas
                    while (pos < str.size() && (str[pos] == ',' || str[pos] == ' ')) {
                        pos++;
                    }
                    start = pos;
                }
                pos++;
            }
            if (pos >= str.size()) {
                throw std::runtime_error("Unterminated array in JSON");
            }
            std::string result = str.substr(start, pos - start);
            pos++; // Skip closing bracket
            return result;
        } else if (c == '{') {
            // Object - parse as key-value pairs
            pos++; // Skip opening brace
            std::string result;
            bool first = true;
            
            while (pos < str.size() && str[pos] != '}') {
                // Skip whitespace
                while (pos < str.size() && std::isspace(static_cast<unsigned char>(str[pos]))) {
                    pos++;
                }
                
                if (pos >= str.size() || str[pos] == '}') {
                    break;
                }
                
                if (!first) {
                    if (str[pos] == ',') {
                        pos++;
                        // Skip whitespace after comma
                        while (pos < str.size() && std::isspace(static_cast<unsigned char>(str[pos]))) {
                            pos++;
                        }
                    } else {
                        throw std::runtime_error("Expected comma between JSON object members");
                    }
                }
                first = false;
                
                // Parse key
                if (str[pos] != '"') {
                    throw std::runtime_error("Expected quoted string as JSON object key");
                }
                pos++; // Skip opening quote
                size_t keyStart = pos;
                while (pos < str.size() && str[pos] != '"') {
                    if (str[pos] == '\\' && pos + 1 < str.size()) {
                        pos += 2;
                        continue;
                    }
                    pos++;
                }
                if (pos >= str.size()) {
                    throw std::runtime_error("Unterminated string in JSON object key");
                }
                std::string key = str.substr(keyStart, pos - keyStart);
                pos++; // Skip closing quote
                
                // Skip whitespace after key
                while (pos < str.size() && std::isspace(static_cast<unsigned char>(str[pos]))) {
                    pos++;
                }
                
                if (pos >= str.size() || str[pos] != ':') {
                    throw std::runtime_error("Expected colon after JSON object key");
                }
                pos++; // Skip colon
                
                // Skip whitespace before value
                while (pos < str.size() && std::isspace(static_cast<unsigned char>(str[pos]))) {
                    pos++;
                }
                
                // Parse value
                std::string value = parseJsonValue(str, pos);
                
                // Add to result
                if (!result.empty()) result += ',';
                result += '"' + key + '":' + value;
                
                // Skip whitespace after value
                while (pos < str.size() && std::isspace(static_cast<unsigned char>(str[pos]))) {
                    pos++;
                }
            }
            
            if (pos >= str.size()) {
                throw std::runtime_error("Unterminated JSON object");
            }
            
            pos++; // Skip closing brace
            return "{" + result + "}";
        } else if (std::isdigit(static_cast<unsigned char>(c)) || c == '-') {
            // Number value
            size_t start = pos;
            while (pos < str.size() && (std::isdigit(static_cast<unsigned char>(str[pos])) || 
                                        str[pos] == '.' || str[pos] == 'e' || 
                                        str[pos] == 'E' || str[pos] == '+' || str[pos] == '-')) {
                pos++;
            }
            return str.substr(start, pos - start);
        } else {
            throw std::runtime_error("Invalid JSON value type");
        }
    }
    
    // Convert JSON value string to ConfigValue
    ConfigValue parseJsonToConfigValue(const std::string& jsonValue) {
        if (jsonValue == "null") {
            return std::string("null");
        } else if (jsonValue == "true") {
            return true;
        } else if (jsonValue == "false") {
            return false;
        } else if (jsonValue.empty()) {
            return std::string("");
        } else if (jsonValue[0] == '"' && jsonValue.back() == '"') {
            // String
            std::string str = jsonValue.substr(1, jsonValue.size() - 2);
            // Unescape string
            std::string result;
            bool escaped = false;
            for (char c : str) {
                if (c == '\\' && !escaped) {
                    escaped = true;
                    continue;
                }
                if (escaped) {
                    // Handle escape sequences
                    switch (c) {
                        case 'n': result += '\n'; break;
                        case 't': result += '\t'; break;
                        case 'r': result += '\r'; break;
                        case '"': result += '"'; break;
                        case '\\': result += '\\'; break;
                        default: result += c; break;
                    }
                    escaped = false;
                } else {
                    result += c;
                }
            }
            return result;
        } else if (isJsonNumber(jsonValue)) {
            // Try to parse as integer first
            try {
                size_t pos;
                long long_val = std::stoll(jsonValue, &pos);
                if (pos == jsonValue.size()) {
                    return long_val;
                }
            } catch (...) {}
            
            // Try to parse as double
            try {
                size_t pos;
                double double_val = std::stod(jsonValue, &pos);
                if (pos == jsonValue.size()) {
                    return double_val;
                }
            } catch (...) {}
            
            // If all else fails, return as string
            return jsonValue;
        } else {
            // Try to parse as array
            if (jsonValue[0] == '[' && jsonValue.back() == ']') {
                // Simple array parsing - for now treat as string
                return jsonValue;
            }
            return jsonValue;
        }
    }
    
    // Parse JSON configuration string
    bool parseJsonConfig(const std::string& jsonStr, std::vector<ConfigEntry>& entries) {
        try {
            std::string cleanedStr = removeJsonComments(jsonStr);
            
            if (cleanedStr.empty() || (cleanedStr.size() > 1 && cleanedStr[0] == '{' && cleanedStr.back() == '}')) {
                // Handle empty object or complete object
                if (cleanedStr.size() <= 2) {
                    return true; // Empty JSON object
                }
                
                // Remove outer braces
                std::string inner = cleanedStr.substr(1, cleanedStr.size() - 2);
                
                if (inner.empty()) {
                    return true;
                }
                
                // Simple parsing for key-value pairs
                size_t pos = 0;
                bool first = true;
                
                while (pos < inner.size()) {
                    // Skip whitespace
                    while (pos < inner.size() && std::isspace(static_cast<unsigned char>(inner[pos]))) {
                        pos++;
                    }
                    
                    if (pos >= inner.size()) {
                        break;
                    }
                    
                    if (!first) {
                        if (inner[pos] == ',') {
                            pos++;
                            // Skip whitespace after comma
                            while (pos < inner.size() && std::isspace(static_cast<unsigned char>(inner[pos]))) {
                                pos++;
                            }
                        } else {
                            throw std::runtime_error("Expected comma between JSON object members");
                        }
                    }
                    first = false;
                    
                    // Find next key
                    size_t keyStart = pos;
                    while (pos < inner.size() && inner[pos] != '"') {
                        pos++;
                    }
                    
                    if (pos >= inner.size() || inner[pos] != '"') {
                        throw std::runtime_error("Expected quoted string as JSON key");
                    }
                    
                    pos++; // Skip opening quote
                    size_t keyEnd = pos;
                    while (pos < inner.size() && inner[pos] != '"') {
                        if (inner[pos] == '\\' && pos + 1 < inner.size()) {
                            pos += 2;
                            continue;
                        }
                        pos++;
                    }
                    
                    if (pos >= inner.size()) {
                        throw std::runtime_error("Unterminated string in JSON key");
                    }
                    
                    std::string key = inner.substr(keyEnd, pos - keyEnd);
                    pos++; // Skip closing quote
                    
                    // Skip whitespace after key
                    while (pos < inner.size() && std::isspace(static_cast<unsigned char>(inner[pos]))) {
                        pos++;
                    }
                    
                    if (pos >= inner.size() || inner[pos] != ':') {
                        throw std::runtime_error("Expected colon after JSON key");
                    }
                    
                    pos++; // Skip colon
                    
                    // Skip whitespace before value
                    while (pos < inner.size() && std::isspace(static_cast<unsigned char>(inner[pos]))) {
                        pos++;
                    }
                    
                    // Find value end (next comma or end of object)
                    size_t valueStart = pos;
                    while (pos < inner.size() && inner[pos] != ',' && inner[pos] != '}') {
                        pos++;
                    }
                    
                    std::string valueStr = inner.substr(valueStart, pos - valueStart);
                    
                    // Parse value
                    ConfigValue value = parseJsonToConfigValue(valueStr);
                    
                    // Add entry
                    entries.emplace_back(key, value, ConfigSource::File);
                }
                
                return true;
            }
            
            throw std::runtime_error("Invalid JSON format - expected object at root level");
            
        } catch (const std::exception& e) {
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
            return false;
        }
    }
    
} // namespace

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.empty()) {
        return true; // Empty file is valid
    }
    
    // Try to parse as JSON first
    bool isJson = false;
    std::string trimmed = trim(content);
    
    if ((trimmed.size() >= 2 && 
         ((trimmed[0] == '{' && trimmed.back() == '}') || 
          (trimmed[0] == '[' && trimmed.back() == ']'))) ||
        trimmed.find('{') != std::string::npos) {
        isJson = true;
    }
    
    if (isJson) {
        // Parse as JSON
        return parseJsonConfig(content, pImpl->entries);
    } else {
        // Parse as simple key=value format (backward compatibility)
        return parseKeyValueConfig(content);
    }
}

// Parse simple key=value configuration
bool Config::parseKeyValueConfig(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(stream, line)) {
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
            // Invalid format, log warning but continue
            std::cerr << "Warning: Invalid config line " << lineNumber 
                     << ": missing '=' operator in line: " << line << std::endl;
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
    
    file << "{\n"; // Start JSON object
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Convert ConfigValue to JSON string
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, double>) {
                file << arg;
            } else {
                file << "\"" << arg << "\""; // Fallback for unknown types
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    file << "}\n"; // End JSON object
    
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
