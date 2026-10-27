#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
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
    
    // Read entire file content
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON first (more sophisticated format)
    bool parsedAsJson = parseJsonContent(content);
    
    // If JSON parsing failed or didn't parse all content, try simple key=value format
    if (!parsedAsJson) {
        clear(); // Clear any previous entries
        parseSimpleKeyValue(content);
    }
    
    return !pImpl->entries.empty();
}

bool Config::parseJsonContent(const std::string& content) {
    // Simple JSON parser for key-value pairs
    // Supports: {"key1": value1, "key2": value2, ...}
    // Supports nested objects: {"key1": {"nested": value}}
    // Only supports simple types: strings, numbers, booleans, null
    
    clear();
    
    size_t pos = 0;
    skipWhitespace(content, pos);
    
    if (pos >= content.size() || content[pos] != '{') {
        return false; // Not JSON format
    }
    
    pos++; // Skip '{'
    bool hasItems = false;
    
    while (pos < content.size()) {
        skipWhitespace(content, pos);
        
        if (content[pos] == '}') {
            break; // End of object
        }
        
        // Parse key (must be a string)
        if (!parseJsonString(content, pos)) {
            break; // Invalid format
        }
        
        skipWhitespace(content, pos);
        if (pos >= content.size() || content[pos] != ':') {
            return false; // Missing colon after key
        }
        
        pos++; // Skip ':'
        skipWhitespace(content, pos);
        
        // Parse value
        if (!parseJsonValue(content, pos)) {
            return false;
        }
        
        hasItems = true;
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++; // Skip comma
            continue;
        }
    }
    
    skipWhitespace(content, pos);
    if (pos >= content.size() || content[pos] != '}') {
        return false; // Missing closing brace
    }
    
    return hasItems;
}

bool Config::parseJsonValue(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) {
        return false;
    }
    
    char c = content[pos];
    
    if (c == '"') {
        return parseJsonString(content, pos);
    } else if (c == '{') {
        return parseJsonObject(content, pos);
    } else if (c == '[') {
        return parseJsonArray(content, pos);
    } else if (std::isdigit(c) || c == '-') {
        return parseJsonNumber(content, pos);
    } else if (c == 't' || c == 'f') {
        return parseJsonBoolean(content, pos);
    } else if (c == 'n') {
        return parseJsonNull(content, pos);
    }
    
    return false;
}

bool Config::parseJsonString(const std::string& content, size_t& pos) {
    if (pos >= content.size() || content[pos] != '"') {
        return false;
    }
    
    pos++; // Skip opening quote
    size_t start = pos;
    
    while (pos < content.size()) {
        if (content[pos] == '\\') {
            pos += 2; // Skip escaped character
            continue;
        } else if (content[pos] == '"') {
            break; // Closing quote
        }
        pos++;
    }
    
    if (pos >= content.size()) {
        return false; // Unclosed string
    }
    
    std::string key = content.substr(start, pos - start);
    pos++; // Skip closing quote
    
    skipWhitespace(content, pos);
    if (pos >= content.size() || content[pos] != ':') {
        // This is actually just parsing a string value, not a key-value pair
        return parseJsonStringValue(key, content, pos);
    }
    
    return true; // String key parsed (value parsing is handled by caller)
}

bool Config::parseJsonStringValue(const std::string& key, const std::string& content, size_t& pos) {
    // Parse string value and add to config
    std::string value;
    size_t start = pos;
    
    if (pos < content.size() && content[pos] == '"') {
        pos++; // Skip opening quote
        start = pos;
        
        while (pos < content.size()) {
            if (content[pos] == '\\') {
                pos += 2; // Skip escaped character
                continue;
            } else if (content[pos] == '"') {
                break; // Closing quote
            }
            pos++;
        }
        
        if (pos >= content.size()) {
            return false; // Unclosed string
        }
        
        value = content.substr(start, pos - start);
        pos++; // Skip closing quote
    }
    
    set(key, value, ConfigSource::File);
    return true;
}

bool Config::parseJsonObject(const std::string& content, size_t& pos) {
    if (pos >= content.size() || content[pos] != '{') {
        return false;
    }
    
    pos++; // Skip '{'
    bool hasItems = false;
    
    while (pos < content.size()) {
        skipWhitespace(content, pos);
        
        if (content[pos] == '}') {
            break; // End of object
        }
        
        // Parse key
        if (!parseJsonString(content, pos)) {
            break; // Invalid format
        }
        
        skipWhitespace(content, pos);
        if (pos >= content.size() || content[pos] != ':') {
            return false; // Missing colon after key
        }
        
        pos++; // Skip ':'
        skipWhitespace(content, pos);
        
        // Parse value
        if (!parseJsonValue(content, pos)) {
            return false;
        }
        
        hasItems = true;
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++; // Skip comma
            continue;
        }
    }
    
    skipWhitespace(content, pos);
    if (pos >= content.size() || content[pos] != '}') {
        return false; // Missing closing brace
    }
    
    return hasItems;
}

bool Config::parseJsonArray(const std::string& content, size_t& pos) {
    if (pos >= content.size() || content[pos] != '[') {
        return false;
    }
    
    pos++; // Skip '['
    bool hasItems = false;
    
    while (pos < content.size()) {
        skipWhitespace(content, pos);
        
        if (content[pos] == ']') {
            break; // End of array
        }
        
        // Parse value
        if (!parseJsonValue(content, pos)) {
            return false;
        }
        
        hasItems = true;
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++; // Skip comma
            continue;
        }
    }
    
    skipWhitespace(content, pos);
    if (pos >= content.size() || content[pos] != ']') {
        return false; // Missing closing bracket
    }
    
    return hasItems;
}

bool Config::parseJsonNumber(const std::string& content, size_t& pos) {
    size_t start = pos;
    bool hasDecimal = false;
    bool hasExponent = false;
    bool hasSign = false;
    
    if (content[pos] == '-') {
        pos++;
        hasSign = true;
    }
    
    // Parse integer part
    while (pos < content.size() && std::isdigit(content[pos])) {
        pos++;
    }
    
    if (pos < content.size() && content[pos] == '.') {
        hasDecimal = true;
        pos++; // Skip decimal point
        
        // Parse fractional part
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    if (pos < content.size() && (content[pos] == 'e' || content[pos] == 'E')) {
        hasExponent = true;
        pos++; // Skip 'e' or 'E'
        
        if (pos < content.size() && (content[pos] == '+' || content[pos] == '-')) {
            pos++; // Skip exponent sign
            hasSign = true;
        }
        
        // Parse exponent
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    if (pos == start || (hasDecimal && pos == start + 1) || (hasExponent && pos == start + 1 + hasSign)) {
        return false; // Invalid number format
    }
    
    std::string numStr = content.substr(start, pos - start);
    
    // Try to parse as integer first
    try {
        if (numStr.find('.') == std::string::npos && numStr.find('e') == std::string::npos && numStr.find('E') == std::string::npos) {
            // Try integer
            size_t end;
            long long intVal = std::stoll(numStr, &end);
            if (end == numStr.size()) {
                set("dummy", intVal, ConfigSource::File); // Temporary for testing
                return true;
            }
        }
        
        // Try as double
        size_t end;
        double floatVal = std::stod(numStr, &end);
        if (end == numStr.size()) {
            set("dummy", floatVal, ConfigSource::File); // Temporary for testing
            return true;
        }
    } catch (...) {
        return false;
    }
    
    return false;
}

bool Config::parseJsonBoolean(const std::string& content, size_t& pos) {
    if (pos + 4 <= content.size() && content.substr(pos, 4) == "true") {
        pos += 4;
        set("dummy", true, ConfigSource::File); // Temporary for testing
        return true;
    } else if (pos + 5 <= content.size() && content.substr(pos, 5) == "false") {
        pos += 5;
        set("dummy", false, ConfigSource::File); // Temporary for testing
        return true;
    }
    
    return false;
}

bool Config::parseJsonNull(const std::string& content, size_t& pos) {
    if (pos + 4 <= content.size() && content.substr(pos, 4) == "null") {
        pos += 4;
        set("dummy", std::string("null"), ConfigSource::File); // Temporary for testing
        return true;
    }
    
    return false;
}

void Config::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && std::isspace(content[pos])) {
        pos++;
    }
}

void Config::parseSimpleKeyValue(const std::string& content) {
    clear(); // Clear any previous entries
    
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

void Config::formatConfigValue(std::ofstream& file, const ConfigValue& value) const {
    std::visit([&file](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            file << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            file << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            // Format double to remove trailing zeros
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << arg;
            std::string str = oss.str();
            
            // Remove trailing zeros and possibly the decimal point
            size_t decimalPos = str.find('.');
            if (decimalPos != std::string::npos) {
                // Remove trailing zeros
                size_t lastNonZero = str.find_last_not_of('0');
                if (lastNonZero != std::string::npos && lastNonZero > decimalPos) {
                    str.erase(lastNonZero + 1);
                }
                
                // If only zeros after decimal, remove decimal point
                if (str.back() == '.') {
                    str.pop_back();
                }
            }
            
            file << str;
        } else if constexpr (std::is_same_v<T, bool>) {
            file << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            file << '\"' << escapeString(arg) << '\"';
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
                // Format double
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << arg[i];
                std::string str = oss.str();
                
                // Remove trailing zeros and possibly the decimal point
                size_t decimalPos = str.find('.');
                if (decimalPos != std::string::npos) {
                    // Remove trailing zeros
                    size_t lastNonZero = str.find_last_not_of('0');
                    if (lastNonZero != std::string::npos && lastNonZero > decimalPos) {
                        str.erase(lastNonZero + 1);
                    }
                    
                    // If only zeros after decimal, remove decimal point
                    if (str.back() == '.') {
                        str.pop_back();
                    }
                }
                
                file << str;
            }
            file << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            file << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) file << ", ";
                file << '\"' << escapeString(arg[i]) << '\"';
            }
            file << "]";
        }
    }, value);
}

std::string Config::escapeString(const std::string& str) const {
    std::string result;
    result.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    
    return result;
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
        
        // Format the value based on its type
        file << entry.key << " = ";
        formatConfigValue(file, entry.value);
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
// We need to instantiate all the types that are used in the implementation
// to avoid linker errors
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
