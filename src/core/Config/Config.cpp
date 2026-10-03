#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>
#include <cctype>

namespace nlm {

// JSON parsing utilities
namespace json {
    // Skip whitespace in JSON
    static std::string_view skipWhitespace(std::string_view str) {
        size_t pos = 0;
        while (pos < str.size() && std::isspace(str[pos])) {
            pos++;
        }
        return str.substr(pos);
    }
    
    // Parse JSON string (with quotes)
    static std::string parseString(std::string_view str) {
        if (str.empty() || str.front() != '"') {
            throw std::runtime_error("Invalid JSON string: missing opening quote");
        }
        
        size_t end = str.find('"', 1);
        if (end == std::string::npos) {
            throw std::runtime_error("Invalid JSON string: missing closing quote");
        }
        
        std::string result;
        result.reserve(end - 1);
        
        for (size_t i = 1; i < end; ++i) {
            if (str[i] == '\\' && i + 1 < end) {
                i++; // Skip escape character
                switch (str[i]) {
                    case '\\': result += '\\'; break;
                    case '"': result += '"'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    default: result += str[i]; break;
                }
            } else {
                result += str[i];
            }
        }
        
        return result;
    }
    
    // Parse JSON value (string, number, boolean, null, array, object)
    static ConfigValue parseJSONValue(std::string_view& str) {
        str = skipWhitespace(str);
        
        if (str.empty()) {
            throw std::runtime_error("Unexpected end of JSON input");
        }
        
        switch (str.front()) {
            case '\"': {
                // String
                size_t end = str.find('\"', 1);
                if (end == std::string::npos) {
                    throw std::runtime_error("Unterminated JSON string");
                }
                std::string value = str.substr(1, end - 1);
                str = str.substr(end + 1);
                return value;
            }
            case 't': // true
            case 'f': // false
            case 'n': // null
            case '-': // negative number
            case '0': // number
            case '1': // number
            case '2': // number
            case '3': // number
            case '4': // number
            case '5': // number
            case '6': // number
            case '7': // number
            case '8': // number
            case '9': // number
            case '.': // decimal number
                // Parse number
            {
                size_t end = str.find_first_of(",]}", 0);
                if (end == std::string::npos) {
                    end = str.size();
                }
                std::string numStr = str.substr(0, end);
                
                // Try to parse as integer
                try {
                    size_t pos;
                    long long intVal = std::stoll(numStr, &pos);
                    if (pos == numStr.size()) {
                        str = str.substr(end);
                        return intVal;
                    }
                } catch (...) {}
                
                // Try to parse as double
                try {
                    size_t pos;
                    double doubleVal = std::stod(numStr, &pos);
                    if (pos == numStr.size()) {
                        str = str.substr(end);
                        return doubleVal;
                    }
                } catch (...) {}
                
                // If both fail, treat as string
                str = str.substr(end);
                return numStr;
            }
            case '[': {
                // Array
                str = str.substr(1); // Skip '['
                std::vector<ConfigValue> array;
                
                while (true) {
                    str = skipWhitespace(str);
                    if (str.empty() || str.front() == ']') {
                        break;
                    }
                    
                    array.push_back(parseJSONValue(str));
                    
                    str = skipWhitespace(str);
                    if (str.empty()) {
                        throw std::runtime_error("Unexpected end of array in JSON");
                    }
                    
                    if (str.front() == ']') {
                        break;
                    } else if (str.front() == ',') {
                        str = str.substr(1); // Skip ','
                    } else {
                        throw std::runtime_error("Expected ',' or ']' in JSON array");
                    }
                }
                
                if (str.empty() || str.front() != ']') {
                    throw std::runtime_error("Missing closing ']' in JSON array");
                }
                str = str.substr(1); // Skip ']'
                return array;
            }
            case '{': {
                // Object (for now, we'll handle as key=value pairs)
                std::unordered_map<std::string, ConfigValue> object;
                str = str.substr(1); // Skip '{'
                
                while (true) {
                    str = skipWhitespace(str);
                    if (str.empty() || str.front() == '}') {
                        break;
                    }
                    
                    // Parse key
                    if (str.front() != '\"') {
                        throw std::runtime_error("Expected string key in JSON object");
                    }
                    size_t keyEnd = str.find('\"', 1);
                    if (keyEnd == std::string::npos) {
                        throw std::runtime_error("Unterminated string key in JSON object");
                    }
                    std::string key = str.substr(1, keyEnd - 1);
                    str = str.substr(keyEnd + 1);
                    
                    // Skip whitespace
                    str = skipWhitespace(str);
                    if (str.empty() || str.front() != ':') {
                        throw std::runtime_error("Expected ':' after key in JSON object");
                    }
                    str = str.substr(1); // Skip ':'
                    
                    // Parse value
                    ConfigValue value = parseJSONValue(str);
                    object[key] = value;
                    
                    // Skip whitespace
                    str = skipWhitespace(str);
                    if (str.empty()) {
                        throw std::runtime_error("Unexpected end of object in JSON");
                    }
                    
                    if (str.front() == '}') {
                        break;
                    } else if (str.front() == ',') {
                        str = str.substr(1); // Skip ','
                    } else {
                        throw std::runtime_error("Expected ',' or '}' in JSON object");
                    }
                }
                
                if (str.empty() || str.front() != '}') {
                    throw std::runtime_error("Missing closing '}' in JSON object");
                }
                str = str.substr(1); // Skip '}'
                
                // Convert object to string for now (simplified)
                std::ostringstream oss;
                oss << "{";
                bool first = true;
                for (const auto& [k, v] : object) {
                    if (!first) oss << ", ";
                    oss << "\"" << k << "\": ";
                    std::visit([&oss](auto&& arg) {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, std::string>) {
                            oss << "\"" << arg << "\"";
                        } else if constexpr (std::is_same_v<T, bool>) {
                            oss << (arg ? "true" : "false");
                        } else {
                            oss << arg;
                        }
                    }, v);
                    first = false;
                }
                oss << "}";
                return oss.str();
            }
            default:
                throw std::runtime_error(std::string("Unexpected character in JSON: ") + str.front());
        }
    }
}

// YAML parsing utilities (basic)
namespace yaml {
    // Parse YAML scalar (quoted or unquoted)
    static ConfigValue parseYAMLScalar(std::string_view str) {
        str = skipWhitespace(str);
        
        if (str.empty()) {
            return std::string();
        }
        
        if (str.front() == '\"' || str.front() == '\'') {
            // Quoted string
            char quote = str.front();
            size_t end = str.find(quote, 1);
            if (end == std::string::npos) {
                throw std::runtime_error("Unterminated quoted scalar in YAML");
            }
            std::string value = str.substr(1, end - 1);
            // Handle escape sequences
            std::string result;
            for (size_t i = 0; i < value.size(); ++i) {
                if (value[i] == '\\' && i + 1 < value.size()) {
                    i++; // Skip escape character
                    switch (value[i]) {
                        case '\\': result += '\\'; break;
                        case '\"': result += '\"'; break;
                        case '\'': result += '\''; break;
                        case 'n': result += '\n'; break;
                        case 't': result += '\t'; break;
                        case 'r': result += '\r'; break;
                        default: result += value[i]; break;
                    }
                } else {
                    result += value[i];
                }
            }
            return result;
        }
        
        // Unquoted scalar
        size_t end = str.find_first_of("\n?:\{\}\[\],#\t", 0);
        if (end == std::string::npos) {
            end = str.size();
        }
        
        std::string scalar = str.substr(0, end);
        
        // Try to parse as integer
        try {
            size_t pos;
            long long intVal = std::stoll(scalar, &pos);
            if (pos == scalar.size()) {
                return intVal;
            }
        } catch (...) {}
        
        // Try to parse as double
        try {
            size_t pos;
            double doubleVal = std::stod(scalar, &pos);
            if (pos == scalar.size()) {
                return doubleVal;
            }
        } catch (...) {}
        
        // Try to parse as boolean
        if (scalar == "true") return true;
        if (scalar == "false") return false;
        if (scalar == "null" || scalar == "~") return std::string();
        
        // Treat as string
        return scalar;
    }
}

// Parse key=value format (legacy format)
static bool parseKeyValueFormat(const std::string& line, Config& config) {
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
        
        // Try to parse as different types
        if (value == "true") {
            config.set(key, true, ConfigSource::File);
            return true;
        } else if (value == "false") {
            config.set(key, false, ConfigSource::File);
            return true;
        }
        
        // Try integer
        try {
            size_t pos;
            long long intVal = std::stoll(value, &pos);
            if (pos == value.size()) {
                config.set(key, intVal, ConfigSource::File);
                return true;
            }
        } catch (...) {}
        
        // Try double
        try {
            size_t pos;
            double doubleVal = std::stod(value, &pos);
            if (pos == value.size()) {
                config.set(key, doubleVal, ConfigSource::File);
                return true;
            }
        } catch (...) {}
        
        // Default to string
        config.set(key, value, ConfigSource::File);
        return true;
    }
    return false;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool allowJSON;
    bool allowYAML;
    
    Impl() : entries(), allowJSON(true), allowYAML(true) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    try {
        // Try JSON format first
        if (pImpl->allowJSON && !content.empty() && content.front() == '{') {
            // Parse JSON
            std::string_view str(content);
            str = json::skipWhitespace(str);
            
            if (str.front() != '{') {
                throw std::runtime_error("JSON file must start with '{'");
            }
            
            str = str.substr(1); // Skip '{'
            
            while (true) {
                str = json::skipWhitespace(str);
                if (str.empty() || str.front() == '}') {
                    break;
                }
                
                // Parse key
                if (str.front() != '\"') {
                    throw std::runtime_error("Expected string key in JSON object");
                }
                size_t keyEnd = str.find('\"', 1);
                if (keyEnd == std::string::npos) {
                    throw std::runtime_error("Unterminated string key in JSON object");
                }
                std::string key = str.substr(1, keyEnd - 1);
                str = str.substr(keyEnd + 1);
                
                // Skip whitespace
                str = json::skipWhitespace(str);
                if (str.empty() || str.front() != ':') {
                    throw std::runtime_error("Expected ':' after key in JSON object");
                }
                str = str.substr(1); // Skip ':'
                
                // Parse value
                ConfigValue value = json::parseJSONValue(str);
                
                // Convert JSON types to ConfigValue
                set(key, value, ConfigSource::File);
                
                // Skip whitespace
                str = json::skipWhitespace(str);
                if (str.empty()) {
                    throw std::runtime_error("Unexpected end of JSON object");
                }
                
                if (str.front() == '}') {
                    break;
                } else if (str.front() == ',') {
                    str = str.substr(1); // Skip ','
                } else {
                    throw std::runtime_error("Expected ',' or '}' in JSON object");
                }
            }
            
            return true;
        }
        
        // Try YAML format
        if (pImpl->allowYAML && !content.empty()) {
            // Simple YAML parsing (for key: value format)
            std::istringstream stream(content);
            std::string line;
            
            while (std::getline(stream, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '%') {
                    continue; // Skip comments and directives
                }
                
                // Check if line looks like YAML (contains ':')
                size_t colonPos = line.find(':');
                if (colonPos != std::string::npos && colonPos > 0) {
                    std::string key = trim(line.substr(0, colonPos));
                    std::string value = trim(line.substr(colonPos + 1));
                    
                    if (!key.empty() && !value.empty()) {
                        // Parse the value using YAML scalar parsing
                        try {
                            ConfigValue parsedValue;
                            // Simple YAML value parsing
                            if (value == "true") {
                                parsedValue = true;
                            } else if (value == "false") {
                                parsedValue = false;
                            } else {
                                // Try to parse as number
                                try {
                                    size_t pos;
                                    long long intVal = std::stoll(value, &pos);
                                    if (pos == value.size()) {
                                        parsedValue = intVal;
                                    } else {
                                        double doubleVal = std::stod(value, &pos);
                                        if (pos == value.size()) {
                                            parsedValue = doubleVal;
                                        } else {
                                            parsedValue = value;
                                        }
                                    }
                                } catch (...) {
                                    parsedValue = value;
                                }
                            }
                            
                            set(key, parsedValue, ConfigSource::File);
                        } catch (...) {
                            // Fallback to string
                            set(key, value, ConfigSource::File);
                        }
                    }
                } else {
                    // Try key=value format for backward compatibility
                    parseKeyValueFormat(line, *this);
                }
            }
            return true;
        }
        
        // Fallback to legacy key=value format
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            if (!parseKeyValueFormat(line, *this)) {
                // Skip malformed lines
                continue;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        // Log error and return false
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Config System\n\n";
    
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
