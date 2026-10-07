#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <cctype>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // JSON parsing state
    enum class JsonTokenType {
        OBJECT_BEGIN, OBJECT_END,
        ARRAY_BEGIN, ARRAY_END,
        STRING, NUMBER, BOOLEAN, NULL_VALUE,
        COMMA, COLON, END_OF_INPUT
    };
    
    struct JsonToken {
        JsonTokenType type;
        std::string stringValue;
        double numberValue;
        bool boolValue;
        size_t line, column;
    };
    
    // Simple JSON parser implementation
    class JsonParser {
    public:
        bool parse(const std::string& json, std::vector<ConfigEntry>& entries) {
            std::string remaining = json;
            std::stack<char> brackets;
            
            while (!remaining.empty()) {
                // Skip whitespace
                remaining = trim(remaining);
                if (remaining.empty()) break;
                
                char firstChar = remaining[0];
                
                if (firstChar == '{') {
                    // Parse object
                    bool success = parseObject(remaining, entries);
                    if (!success) return false;
                    // Update remaining after parsing
                    remaining = remaining.substr(remaining.find('{') + 1);
                } else if (firstChar == '[') {
                    // Parse array (treat as string for simplicity)
                    bool success = parseArray(remaining, entries);
                    if (!success) return false;
                    remaining = remaining.substr(remaining.find('[') + 1);
                } else {
                    // Parse simple key-value pairs (old format)
                    bool success = parseSimpleFormat(remaining, entries);
                    if (!success) {
                        // Try to parse JSON object
                        remaining = trim(remaining);
                        if (remaining[0] == '{') {
                            bool objSuccess = parseObject(remaining, entries);
                            if (!objSuccess) return false;
                            remaining = remaining.substr(remaining.find('{') + 1);
                        } else {
                            return false;
                        }
                    }
                }
            }
            
            return true;
        }
        
    private:
        bool parseObject(const std::string& json, std::vector<ConfigEntry>& entries) {
            std::string remaining = json;
            bool inString = false;
            char quoteChar = '\0';
            size_t braceDepth = 0;
            
            while (!remaining.empty()) {
                char current = remaining[0];
                
                if (!inString && current == '{') {
                    braceDepth++;
                } else if (!inString && current == '}') {
                    braceDepth--;
                    if (braceDepth == 0) {
                        // End of object
                        break;
                    }
                } else if (!inString && (current == '\"' || current == '\'')) {
                    inString = true;
                    quoteChar = current;
                } else if (inString && current == quoteChar) {
                    // Check if escaped
                    if (remaining.size() > 1 && remaining[1] == quoteChar) {
                        // Escaped quote, skip
                        remaining = remaining.substr(1);
                    } else {
                        inString = false;
                        quoteChar = '\0';
                    }
                }
                
                remaining = remaining.substr(1);
            }
            
            // For simplicity, we'll parse as key=value format for now
            // In a real implementation, we'd properly parse JSON
            return parseSimpleFormat(json, entries);
        }
        
        bool parseArray(const std::string& json, std::vector<ConfigEntry>& entries) {
            // Parse array as key=value pairs
            std::string remaining = json;
            bool success = parseSimpleFormat(remaining, entries);
            return success;
        }
        
        bool parseSimpleFormat(const std::string& json, std::vector<ConfigEntry>& entries) {
            std::string line = json;
            size_t pos = 0;
            bool firstEntry = true;
            
            while ((pos = line.find('\n')) != std::string::npos || !line.empty()) {
                std::string currentLine;
                if (pos != std::string::npos) {
                    currentLine = line.substr(0, pos);
                    line = line.substr(pos + 1);
                } else {
                    currentLine = line;
                    line.clear();
                }
                
                // Trim whitespace
                currentLine = trim(currentLine);
                if (currentLine.empty()) continue;
                
                // Skip comments
                if (currentLine[0] == '#' || currentLine[0] == '/') {
                    continue;
                }
                
                // Parse key=value pairs
                size_t eqPos = currentLine.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = trim(currentLine.substr(0, eqPos));
                    std::string value = trim(currentLine.substr(eqPos + 1));
                    
                    // Remove quotes
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    // Try to parse as different types
                    ConfigValue configValue;
                    bool parsed = false;
                    
                    // Try boolean
                    if (value == "true" || value == "false") {
                        configValue = (value == "true");
                        parsed = true;
                    }
                    // Try number
                    else {
                        try {
                            size_t pos;
                            double numValue = std::stod(value, &pos);
                            if (pos == value.size()) {
                                configValue = numValue;
                                parsed = true;
                            }
                        } catch (...) {
                            // Not a number
                        }
                    }
                    
                    if (!parsed) {
                        configValue = value;
                    }
                    
                    entries.emplace_back(key, configValue, ConfigSource::File);
                }
            }
            
            return true;
        }
        
        std::string trim(const std::string& str) {
            size_t start = str.find_first_not_of(" \t\r\n");
            if (start == std::string::npos) return "";
            size_t end = str.find_last_not_of(" \t\r\n");
            return str.substr(start, end - start + 1);
        }
    };
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try to parse as JSON
    if (isJson(content)) {
        return loadFromJson(content);
    } else {
        // Fall back to simple key=value format
        return loadFromSimpleFormat(content);
    }
}

bool Config::loadFromJson(const std::string& json) {
    // Simple JSON parser implementation
    // This is a basic implementation that handles common JSON patterns
    
    pImpl->entries.clear();
    
    // Remove whitespace for easier parsing
    std::string compact = removeWhitespace(json);
    
    if (compact.empty() || compact[0] != '{') {
        // Not a valid JSON object, treat as simple format
        return loadFromSimpleFormat(json);
    }
    
    // Simple recursive descent parser for JSON objects
    bool success = parseJsonObject(compact, 0, "", pImpl->entries);
    return success;
}

bool Config::parseJsonObject(const std::string& json, size_t& pos, 
                           const std::string& parentKey, 
                           std::vector<ConfigEntry>& entries) {
    if (pos >= json.size() || json[pos] != '{') {
        return false;
    }
    
    pos++; // Skip '{'
    bool first = true;
    
    while (pos < json.size() && json[pos] != '}') {
        if (!first && json[pos] == ',') {
            pos++;
        }
        first = false;
        
        // Parse key
        pos = skipWhitespace(json, pos);
        if (pos >= json.size() || (json[pos] != '\"' && json[pos] != '\'')) {
            return false;
        }
        
        char quoteChar = json[pos];
        pos++; // Skip opening quote
        
        size_t keyStart = pos;
        while (pos < json.size() && json[pos] != quoteChar) {
            if (json[pos] == '\\' && pos + 1 < json.size()) {
                pos += 2; // Skip escaped character
            }
            pos++;
        }
        
        if (pos >= json.size()) {
            return false;
        }
        
        std::string key = json.substr(keyStart, pos - keyStart);
        pos++; // Skip closing quote
        
        // Parse colon
        pos = skipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] != ':') {
            return false;
        }
        pos++; // Skip ':'
        
        // Parse value
        pos = skipWhitespace(json, pos);
        bool valueSuccess = parseJsonValue(json, pos, key, parentKey, entries);
        if (!valueSuccess) {
            return false;
        }
    }
    
    if (pos >= json.size() || json[pos] != '}') {
        return false;
    }
    
    pos++; // Skip '}'
    return true;
}

bool Config::parseJsonValue(const std::string& json, size_t& pos,
                           const std::string& key, const std::string& parentKey,
                           std::vector<ConfigEntry>& entries) {
    pos = skipWhitespace(json, pos);
    if (pos >= json.size()) {
        return false;
    }
    
    char current = json[pos];
    
    if (current == '\"' || current == '\'') {
        // Parse string
        return parseJsonString(json, pos, key, parentKey, entries);
    } else if (current == '{') {
        // Parse nested object
        std::string nestedKey = parentKey.empty() ? key : parentKey + "." + key;
        return parseJsonObject(json, pos, nestedKey, entries);
    } else if (current == '[') {
        // Parse array
        return parseJsonArray(json, pos, key, parentKey, entries);
    } else if (std::isdigit(current) || current == '-') {
        // Parse number
        return parseJsonNumber(json, pos, key, parentKey, entries);
    } else if (current == 't' || current == 'f') {
        // Parse boolean
        return parseJsonBoolean(json, pos, key, parentKey, entries);
    } else if (current == 'n') {
        // Parse null
        return parseJsonNull(json, pos, key, parentKey, entries);
    }
    
    return false;
}

bool Config::parseJsonString(const std::string& json, size_t& pos,
                            const std::string& key, const std::string& parentKey,
                            std::vector<ConfigEntry>& entries) {
    if (pos >= json.size()) {
        return false;
    }
    
    char quoteChar = json[pos];
    pos++; // Skip opening quote
    
    size_t start = pos;
    while (pos < json.size() && json[pos] != quoteChar) {
        if (json[pos] == '\\' && pos + 1 < json.size()) {
            pos += 2; // Skip escaped character
        }
        pos++;
    }
    
    if (pos >= json.size()) {
        return false;
    }
    
    std::string value = json.substr(start, pos - start);
    pos++; // Skip closing quote
    
    // Combine key and parent key
    std::string fullKey = parentKey.empty() ? key : parentKey + "." + key;
    entries.emplace_back(fullKey, value, ConfigSource::File);
    return true;
}

bool Config::parseJsonNumber(const std::string& json, size_t& pos,
                            const std::string& key, const std::string& parentKey,
                            std::vector<ConfigEntry>& entries) {
    size_t start = pos;
    if (json[pos] == '-') pos++;
    
    while (pos < json.size() && std::isdigit(json[pos])) {
        pos++;
    }
    
    if (pos == start) {
        return false;
    }
    
    // Check for decimal point
    if (pos < json.size() && json[pos] == '.') {
        pos++; // Skip decimal point
        while (pos < json.size() && std::isdigit(json[pos])) {
            pos++;
        }
    }
    
    // Check for exponent
    if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E')) {
        pos++; // Skip exponent indicator
        if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
            pos++; // Skip exponent sign
        }
        while (pos < json.size() && std::isdigit(json[pos])) {
            pos++;
        }
    }
    
    std::string numStr = json.substr(start, pos - start);
    try {
        double value = std::stod(numStr);
        
        // Determine if it's an int or double
        bool isInt = true;
        for (char c : numStr) {
            if (c != '-' && c != '.' && !std::isdigit(c)) {
                isInt = false;
                break;
            }
        }
        
        // For simplicity, use double
        std::string fullKey = parentKey.empty() ? key : parentKey + "." + key;
        entries.emplace_back(fullKey, value, ConfigSource::File);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::parseJsonBoolean(const std::string& json, size_t& pos,
                             const std::string& key, const std::string& parentKey,
                             std::vector<ConfigEntry>& entries) {
    if (pos + 4 <= json.size() && json.substr(pos, 4) == "true") {
        pos += 4;
        std::string fullKey = parentKey.empty() ? key : parentKey + "." + key;
        entries.emplace_back(fullKey, true, ConfigSource::File);
        return true;
    } else if (pos + 5 <= json.size() && json.substr(pos, 5) == "false") {
        pos += 5;
        std::string fullKey = parentKey.empty() ? key : parentKey + "." + key;
        entries.emplace_back(fullKey, false, ConfigSource::File);
        return true;
    }
    return false;
}

bool Config::parseJsonNull(const std::string& json, size_t& pos,
                          const std::string& key, const std::string& parentKey,
                          std::vector<ConfigEntry>& entries) {
    if (pos + 4 <= json.size() && json.substr(pos, 4) == "null") {
        pos += 4;
        // Null values are typically ignored or stored as empty strings
        std::string fullKey = parentKey.empty() ? key : parentKey + "." + key;
        entries.emplace_back(fullKey, "", ConfigSource::File);
        return true;
    }
    return false;
}

bool Config::parseJsonArray(const std::string& json, size_t& pos,
                           const std::string& key, const std::string& parentKey,
                           std::vector<ConfigEntry>& entries) {
    if (pos >= json.size() || json[pos] != '[') {
        return false;
    }
    
    pos++; // Skip '['
    bool first = true;
    
    while (pos < json.size() && json[pos] != ']') {
        if (!first && json[pos] == ',') {
            pos++;
        }
        first = false;
        
        pos = skipWhitespace(json, pos);
        bool valueSuccess = parseJsonValue(json, pos, key, parentKey, entries);
        if (!valueSuccess) {
            return false;
        }
        
        pos = skipWhitespace(json, pos);
        if (pos < json.size() && json[pos] == ',') {
            pos++; // Skip comma
        }
    }
    
    if (pos >= json.size() || json[pos] != ']') {
        return false;
    }
    
    pos++; // Skip ']'
    return true;
}

std::string Config::removeWhitespace(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (!std::isspace(c)) {
            result += c;
        }
    }
    return result;
}

size_t Config::skipWhitespace(const std::string& str, size_t pos) {
    while (pos < str.size() && std::isspace(str[pos])) {
        pos++;
    }
    return pos;
}

bool Config::isJson(const std::string& str) {
    std::string compact = removeWhitespace(str);
    return !compact.empty() && (compact[0] == '{' || compact[0] == '[');
}

bool Config::loadFromSimpleFormat(const std::string& content) {
    pImpl->entries.clear();
    
    std::string line;
    std::istringstream stream(content);
    
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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse as different types
            ConfigValue configValue;
            bool parsed = false;
            
            // Try boolean
            if (value == "true" || value == "false") {
                configValue = (value == "true");
                parsed = true;
            }
            // Try number
            else {
                try {
                    size_t pos;
                    double numValue = std::stod(value, &pos);
                    if (pos == value.size()) {
                        configValue = numValue;
                        parsed = true;
                    }
                } catch (...) {
                    // Not a number
                }
            }
            
            if (!parsed) {
                configValue = value;
            }
            
            set(key, configValue, ConfigSource::File);
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
        
        file << "\n";
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