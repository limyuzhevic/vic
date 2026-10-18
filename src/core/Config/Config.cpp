#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stack>
#include <unordered_map>
#include <cctype>
#include <iomanip>
#include <cmath>

namespace nlm {

// Helper functions for JSON parsing
static inline bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static inline bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static inline bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static inline bool isAlphanumeric(char c) {
    return isDigit(c) || isAlpha(c);
}

static std::string trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && isWhitespace(str[start])) {
        start++;
    }
    while (end > start && isWhitespace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
}

static std::string unescapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '\\': result += '\\'; break;
                case '"':  result += '"';  break;
                case '/':  result += '/';  break;
                case 'b':  result += '\b'; break;
                case 'f':  result += '\f'; break;
                case 'n':  result += '\n'; break;
                case 'r':  result += '\r'; break;
                case 't':  result += '\t'; break;
                case 'u': {
                    // Unicode escape sequence (simple implementation)
                    if (i + 5 < str.size()) {
                        std::string hex = str.substr(i + 2, 4);
                        bool valid = true;
                        for (char c : hex) {
                            if (!isxdigit(c)) {
                                valid = false;
                                break;
                            }
                        }
                        if (valid) {
                            unsigned int codePoint = std::stoul(hex, nullptr, 16);
                            result += static_cast<char>(codePoint);
                            i += 5;
                            break;
                        }
                    }
                    result += "?";
                    break;
                }
                default: result += str[i + 1]; break;  // Keep unknown escapes as-is
            }
            i++;  // Skip the escaped character
        } else {
            result += str[i];
        }
    }
    
    return result;
}

static ConfigValue parseJSONValue(const std::string& str);

static std::unordered_map<std::string, ConfigValue> parseJSONObject(const std::string& str, size_t& pos);

static std::vector<ConfigValue> parseJSONArray(const std::string& str, size_t& pos);

static std::string parseJSONString(const std::string& str, size_t& pos);

static bool parseJSONBool(const std::string& str);

static double parseJSONNumber(const std::string& str, size_t& pos);

static std::string parseIdentifier(const std::string& str, size_t& pos);

// Main JSON parsing function
ConfigValue parseJSONValue(const std::string& str) {
    size_t pos = 0;
    
    // Skip whitespace
    while (pos < str.size() && isWhitespace(str[pos])) {
        pos++;
    }
    
    if (pos >= str.size()) {
        return std::string();  // Empty value
    }
    
    char firstChar = str[pos];
    
    if (firstChar == '"') {
        return parseJSONString(str, pos);
    } else if (firstChar == '{') {
        size_t endPos = pos;
        int depth = 0;
        bool inString = false;
        char escapeChar = '\0';
        
        while (endPos < str.size()) {
            char c = str[endPos];
            if (!inString) {
                if (c == '{') depth++;
                else if (c == '}') depth--;
                else if (c == '"') inString = true;
            } else {
                if (escapeChar) {
                    escapeChar = '\0';
                } else if (c == '\\') {
                    escapeChar = c;
                } else if (c == '"') {
                    inString = false;
                }
            }
            
            endPos++;
            if (depth == 0) break;
        }
        
        if (endPos > pos) {
            std::string objStr = str.substr(pos, endPos - pos + 1);
            size_t dummy = 0;
            return parseJSONObject(objStr, dummy);
        }
    } else if (firstChar == '[') {
        size_t endPos = pos;
        int depth = 0;
        bool inString = false;
        char escapeChar = '\0';
        
        while (endPos < str.size()) {
            char c = str[endPos];
            if (!inString) {
                if (c == '[') depth++;
                else if (c == ']') depth--;
                else if (c == '"') inString = true;
            } else {
                if (escapeChar) {
                    escapeChar = '\0';
                } else if (c == '\\') {
                    escapeChar = c;
                } else if (c == '"') {
                    inString = false;
                }
            }
            
            endPos++;
            if (depth == 0) break;
        }
        
        if (endPos > pos) {
            std::string arrStr = str.substr(pos, endPos - pos + 1);
            size_t dummy = 0;
            return parseJSONArray(arrStr, dummy);
        }
    } else if (firstChar == 't' || firstChar == 'f') {
        // Boolean value
        if (str.substr(pos, 4) == "true") {
            return true;
        } else if (str.substr(pos, 5) == "false") {
            return false;
        }
    } else if (firstChar == 'n') {
        // null value
        if (str.substr(pos, 4) == "null") {
            return std::string();  // Empty string as placeholder for null
        }
    } else if (firstChar == '-' || isDigit(firstChar)) {
        // Number (including negative)
        size_t endPos = pos;
        while (endPos < str.size() && (isDigit(str[endPos]) || str[endPos] == '.' || str[endPos] == 'e' || str[endPos] == 'E' || str[endPos] == '+' || str[endPos] == '-')) {
            endPos++;
        }
        
        if (endPos > pos) {
            std::string numStr = str.substr(pos, endPos - pos);
            double num = std::stod(numStr);
            return num;
        }
    }
    
    // If parsing fails, treat as string
    return str.substr(pos);
}

std::unordered_map<std::string, ConfigValue> parseJSONObject(const std::string& str, size_t& pos) {
    std::unordered_map<std::string, ConfigValue> obj;
    
    pos++;  // Skip '{'
    skipWhitespace(str, pos);
    
    while (pos < str.size() && str[pos] != '}') {
        skipWhitespace(str, pos);
        
        if (str[pos] == '"') {
            // Parse string key
            size_t keyEnd = str.find('"', pos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = unescapeString(str.substr(pos + 1, keyEnd - pos - 1));
            pos = keyEnd + 1;
            
            skipWhitespace(str, pos);
            
            if (pos >= str.size() || str[pos] != ':') {
                break;
            }
            
            pos++;  // Skip ':'
            skipWhitespace(str, pos);
            
            // Parse value
            ConfigValue value = parseJSONValue(str.substr(pos));
            obj[key] = value;
            
            // Find next comma or closing brace
            while (pos < str.size() && str[pos] != ',' && str[pos] != '}') {
                pos++;
            }
            
            if (pos < str.size() && str[pos] == ',') {
                pos++;  // Skip comma
            }
        } else {
            break;
        }
    }
    
    if (pos < str.size() && str[pos] == '}') {
        pos++;  // Skip '}'
    }
    
    return obj;
}

std::vector<ConfigValue> parseJSONArray(const std::string& str, size_t& pos) {
    std::vector<ConfigValue> arr;
    
    pos++;  // Skip '['
    skipWhitespace(str, pos);
    
    while (pos < str.size() && str[pos] != ']') {
        // Parse element
        ConfigValue element = parseJSONValue(str.substr(pos));
        arr.push_back(element);
        
        skipWhitespace(str, pos);
        
        if (pos < str.size() && str[pos] == ',') {
            pos++;  // Skip comma
            skipWhitespace(str, pos);
        } else if (pos < str.size() && str[pos] != ']') {
            break;
        }
    }
    
    if (pos < str.size() && str[pos] == ']') {
        pos++;  // Skip ']'
    }
    
    return arr;
}

std::string parseJSONString(const std::string& str, size_t& pos) {
    pos++;  // Skip opening quote
    
    std::string result;
    result.reserve(str.size() - pos);
    
    while (pos < str.size() && str[pos] != '"') {
        if (str[pos] == '\\' && pos + 1 < str.size()) {
            result += str[pos + 1];  // Simple escape handling
            pos += 2;
        } else {
            result += str[pos];
            pos++;
        }
    }
    
    if (pos < str.size() && str[pos] == '"') {
        pos++;  // Skip closing quote
    }
    
    return result;
}

bool parseJSONBool(const std::string& str) {
    if (str == "true") return true;
    if (str == "false") return false;
    return false;
}

double parseJSONNumber(const std::string& str, size_t& pos) {
    size_t endPos = pos;
    bool hasDecimal = false;
    bool hasExp = false;
    
    while (endPos < str.size() && (isDigit(str[endPos]) || str[endPos] == '.' || str[endPos] == 'e' || str[endPos] == 'E' || str[endPos] == '+' || str[endPos] == '-')) {
        if (str[endPos] == '.') hasDecimal = true;
        else if (str[endPos] == 'e' || str[endPos] == 'E') hasExp = true;
        endPos++;
    }
    
    if (endPos > pos) {
        std::string numStr = str.substr(pos, endPos - pos);
        double num = std::stod(numStr);
        pos = endPos;
        return num;
    }
    
    return 0.0;
}

std::string parseIdentifier(const std::string& str, size_t& pos) {
    size_t start = pos;
    while (pos < str.size() && isAlphanumeric(str[pos])) {
        pos++;
    }
    
    if (start == pos) {
        return "";
    }
    
    return str.substr(start, pos - start);
}

void skipWhitespace(const std::string& str, size_t& pos) {
    while (pos < str.size() && isWhitespace(str[pos])) {
        pos++;
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to create directory if it doesn't exist
    std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        try {
            std::filesystem::create_directories(dir);
        } catch (const std::filesystem::filesystem_error& e) {
            return false;
        }
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Read entire file
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Try to parse as JSON first
        bool isJSON = false;
        size_t pos = 0;
        
        skipWhitespace(content, pos);
        if (pos < content.size() && (content[pos] == '{' || content[pos] == '[')) {
            isJSON = true;
        }
        
        if (isJSON) {
            // Parse as JSON
            ConfigValue parsedValue = parseJSONValue(content);
            
            // If it's a JSON object, extract key-value pairs
            if (std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(parsedValue)) {
                const auto& obj = std::get<std::unordered_map<std::string, ConfigValue>>(parsedValue);
                for (const auto& pair : obj) {
                    set(pair.first, pair.second, ConfigSource::File);
                }
            } else {
                // Single value JSON - store as "value" key
                set("value", parsedValue, ConfigSource::File);
            }
        } else {
            // Parse as simple key=value format
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
        }
    } catch (const std::exception& e) {
        // Exception during file reading/parsing
        return false;
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
    // Add exception handling for file operations
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Set good precision for floating point values
        file.precision(6);
        file << std::fixed;
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
    } catch (const std::exception& e) {
        // Exception during file writing
        return false;
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
    // Add exception safety for memory operations
    try {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
    } catch (const std::bad_alloc&) {
        // Handle memory allocation failure
        throw;
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
