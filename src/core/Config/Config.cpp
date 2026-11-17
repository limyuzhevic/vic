#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::parseJSONFile(const std::string& filepath) {
    // Simple JSON parser implementation
    // This is a basic implementation that handles:
    // - Key-value objects
    // - Strings (quoted)
    // - Numbers (integers and floats)
    // - Booleans (true/false)
    // - Arrays of numbers, strings, booleans
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Simple state machine parser
    size_t pos = 0;
    skipWhitespace(content, pos);
    
    if (pos >= content.size() || content[pos] != '{') {
        return false;  // Not a JSON object
    }
    
    pos++;  // Skip '{'
    
    while (pos < content.size()) {
        skipWhitespace(content, pos);
        
        if (content[pos] == '}') {
            pos++;  // Skip '}'
            break;
        }
        
        // Parse key
        std::string key = parseString(content, pos);
        if (key.empty()) {
            break;  // Error
        }
        
        skipWhitespace(content, pos);
        if (pos >= content.size() || content[pos] != ':') {
            break;  // Expected ':'
        }
        
        pos++;  // Skip ':'
        skipWhitespace(content, pos);
        
        // Parse value
        ConfigValue value = parseJSONValue(content, pos);
        if (value.index() == -1) {
            break;  // Error parsing value
        }
        
        set(key, value, ConfigSource::File);
        
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++;  // Skip ','
        } else if (pos < content.size() && content[pos] == '}') {
            pos++;  // Skip '}'
            break;
        }
    }
    
    return true;
}

std::string Config::parseString(const std::string& content, size_t& pos) {
    std::string result;
    
    skipWhitespace(content, pos);
    
    if (pos >= content.size() || content[pos] != '\"') {
        return "";  // Not a string
    }
    
    pos++;  // Skip opening quote
    
    while (pos < content.size()) {
        if (content[pos] == '\"') {
            pos++;  // Skip closing quote
            return result;
        } else if (content[pos] == '\\' && pos + 1 < content.size()) {
            // Handle escape sequences
            pos += 2;  // Skip escape character and escaped char
        } else {
            result += content[pos];
            pos++;
        }
    }
    
    return "";  // Unterminated string
}

ConfigValue Config::parseJSONValue(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) {
        return ConfigValue();  // Empty
    }
    
    char ch = content[pos];
    
    if (ch == '\"') {
        // String
        std::string value = parseString(content, pos);
        return value;
    } else if (ch == 't' || ch == 'f') {
        // Boolean
        if (content.substr(pos, 4) == "true") {
            pos += 4;
            return true;
        } else if (content.substr(pos, 5) == "false") {
            pos += 5;
            return false;
        }
    } else if (ch == '-' || (ch >= '0' && ch <= '9')) {
        // Number
        return parseNumber(content, pos);
    } else if (ch == '[') {
        // Array
        return parseJSONArray(content, pos);
    }
    
    return ConfigValue();  // Unknown type
}

ConfigValue Config::parseNumber(const std::string& content, size_t& pos) {
    size_t start = pos;
    bool decimal = false;
    
    if (content[pos] == '-') {
        pos++;
    }
    
    while (pos < content.size()) {
        char ch = content[pos];
        if (ch >= '0' && ch <= '9') {
            pos++;
        } else if (ch == '.' && !decimal) {
            decimal = true;
            pos++;
        } else {
            break;
        }
    }
    
    std::string numStr = content.substr(start, pos - start);
    
    // Try to parse as integer first
    try {
        size_t endPos;
        int64_t intVal = std::stoll(numStr, &endPos);
        if (endPos == numStr.size()) {
            return intVal;
        }
    } catch (...) {}
    
    try {
        size_t endPos;
        double doubleVal = std::stod(numStr, &endPos);
        if (endPos == numStr.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    return numStr;  // Fallback to string
}

ConfigValue Config::parseJSONArray(const std::string& content, size_t& pos) {
    pos++;  // Skip '['
    
    std::vector<ConfigValue> array;
    
    while (pos < content.size()) {
        skipWhitespace(content, pos);
        
        if (content[pos] == ']') {
            pos++;  // Skip ']'
            break;
        }
        
        ConfigValue value = parseJSONValue(content, pos);
        if (value.index() == -1) {
            break;  // Error
        }
        
        array.push_back(value);
        
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++;  // Skip ','
        } else if (pos < content.size() && content[pos] == ']') {
            pos++;  // Skip ']'
            break;
        }
    }
    
    return array;
}

void Config::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r')) {
        pos++;
    }
}

std::string Config::valueToJSONString(const ConfigValue& value) const {
    std::string result;
    
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            result = std::to_string(val);
        } else if constexpr (std::is_same_v<T, double>) {
            result = std::to_string(val);
            // Remove trailing zeros from decimal
            if (result.find('.') != std::string::npos) {
                result.erase(result.find_last_not_of('0') + 1, std::string::npos);
                if (result.back() == '.') {
                    result.pop_back();
                }
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            result = val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = "\"" + escapeString(val) + "\"";
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
                std::string str = std::to_string(val[i]);
                // Remove trailing zeros
                if (str.find('.') != std::string::npos) {
                    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
                    if (str.back() == '.') {
                        str.pop_back();
                    }
                }
                result += str;
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + escapeString(val[i]) + "\"";
            }
            result += "]";
        } else {
            result = "null";
        }
    }, value);
    
    return result;
}

std::string Config::escapeString(const std::string& str) const {
    std::string result;
    for (char ch : str) {
        switch (ch) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += ch; break;
        }
    }
    return result;
}

bool Config::parseJSONFile(const std::string& filepath) {
    // Simple JSON parser implementation
    // This is a basic implementation that handles:
    // - Key-value objects
    // - Strings (quoted)
    // - Numbers (integers and floats)
    // - Booleans (true/false)
    // - Arrays of numbers, strings, booleans
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Simple state machine parser
    size_t pos = 0;
    skipWhitespace(content, pos);
    
    if (pos >= content.size() || content[pos] != '{') {
        return false;  // Not a JSON object
    }
    
    pos++;  // Skip '{'
    
    while (pos < content.size()) {
        skipWhitespace(content, pos);
        
        if (content[pos] == '}') {
            pos++;  // Skip '}'
            break;
        }
        
        // Parse key
        std::string key = parseString(content, pos);
        if (key.empty()) {
            break;  // Error
        }
        
        skipWhitespace(content, pos);
        if (pos >= content.size() || content[pos] != ':') {
            break;  // Expected ':'
        }
        
        pos++;  // Skip ':'
        skipWhitespace(content, pos);
        
        // Parse value
        ConfigValue value = parseJSONValue(content, pos);
        if (value.index() == -1) {
            break;  // Error parsing value
        }
        
        set(key, value, ConfigSource::File);
        
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++;  // Skip ','
        } else if (pos < content.size() && content[pos] == '}') {
            pos++;  // Skip '}'
            break;
        }
    }
    
    return true;
}

std::string Config::parseString(const std::string& content, size_t& pos) {
    std::string result;
    
    skipWhitespace(content, pos);
    
    if (pos >= content.size() || content[pos] != '\"') {
        return "";  // Not a string
    }
    
    pos++;  // Skip opening quote
    
    while (pos < content.size()) {
        if (content[pos] == '\"') {
            pos++;  // Skip closing quote
            return result;
        } else if (content[pos] == '\\' && pos + 1 < content.size()) {
            // Handle escape sequences
            pos += 2;  // Skip escape character and escaped char
        } else {
            result += content[pos];
            pos++;
        }
    }
    
    return "";  // Unterminated string
}

ConfigValue Config::parseJSONValue(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) {
        return ConfigValue();  // Empty
    }
    
    char ch = content[pos];
    
    if (ch == '\"') {
        // String
        std::string value = parseString(content, pos);
        return value;
    } else if (ch == 't' || ch == 'f') {
        // Boolean
        if (content.substr(pos, 4) == "true") {
            pos += 4;
            return true;
        } else if (content.substr(pos, 5) == "false") {
            pos += 5;
            return false;
        }
    } else if (ch == '-' || (ch >= '0' && ch <= '9')) {
        // Number
        return parseNumber(content, pos);
    } else if (ch == '[') {
        // Array
        return parseJSONArray(content, pos);
    }
    
    return ConfigValue();  // Unknown type
}

ConfigValue Config::parseNumber(const std::string& content, size_t& pos) {
    size_t start = pos;
    bool decimal = false;
    
    if (content[pos] == '-') {
        pos++;
    }
    
    while (pos < content.size()) {
        char ch = content[pos];
        if (ch >= '0' && ch <= '9') {
            pos++;
        } else if (ch == '.' && !decimal) {
            decimal = true;
            pos++;
        } else {
            break;
        }
    }
    
    std::string numStr = content.substr(start, pos - start);
    
    // Try to parse as integer first
    try {
        size_t endPos;
        int64_t intVal = std::stoll(numStr, &endPos);
        if (endPos == numStr.size()) {
            return intVal;
        }
    } catch (...) {}
    
    try {
        size_t endPos;
        double doubleVal = std::stod(numStr, &endPos);
        if (endPos == numStr.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    return numStr;  // Fallback to string
}

ConfigValue Config::parseJSONArray(const std::string& content, size_t& pos) {
    pos++;  // Skip '['
    
    std::vector<ConfigValue> array;
    
    while (pos < content.size()) {
        skipWhitespace(content, pos);
        
        if (content[pos] == ']') {
            pos++;  // Skip ']'
            break;
        }
        
        ConfigValue value = parseJSONValue(content, pos);
        if (value.index() == -1) {
            break;  // Error
        }
        
        array.push_back(value);
        
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++;  // Skip ','
        } else if (pos < content.size() && content[pos] == ']') {
            pos++;  // Skip ']'
            break;
        }
    }
    
    return array;
}

void Config::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == '\n' || content[pos] == '\r')) {
        pos++;
    }
}

std::vector<std::string> Config::getJSONString() const {
    std::vector<std::string> lines;
    lines.push_back("{");
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            lines.push_back(",");
        }
        first = false;
        
        lines.push_back("  \"" + entry.key + "\": ");
        
        lines.push_back(valueToJSONString(entry.value));
    }
    
    lines.push_back("}");
    
    std::string result;
    for (const auto& line : lines) {
        result += line + "\n";
    }
    
    return result;
}

std::string Config::valueToJSONString(const ConfigValue& value) {
    std::string result;
    
    std::visit([&](auto&& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            result = std::to_string(val);
        } else if constexpr (std::is_same_v<T, double>) {
            result = std::to_string(val);
            // Remove trailing zeros from decimal
            if (result.find('.') != std::string::npos) {
                result.erase(result.find_last_not_of('0') + 1, std::string::npos);
                if (result.back() == '.') {
                    result.pop_back();
                }
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            result = val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = "\"" + escapeString(val) + "\"";
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
                std::string str = std::to_string(val[i]);
                // Remove trailing zeros
                if (str.find('.') != std::string::npos) {
                    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
                    if (str.back() == '.') {
                        str.pop_back();
                    }
                }
                result += str;
            }
            result += "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + escapeString(val[i]) + "\"";
            }
            result += "]";
        } else {
            result = "null";
        }
    }, value);
    
    return result;
}

std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char ch : str) {
        switch (ch) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += ch; break;
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
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
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

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
