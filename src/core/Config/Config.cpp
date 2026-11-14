#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <cmath>
#include <numeric>
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
    // Try JSON first (more robust), then fall back to simple key=value format
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    
    return json_utils::parseJSON(jsonContent, pImpl->entries);
}

bool Config::loadFromJSON(const std::string& jsonContent) {
    pImpl->entries.clear();
    return json_utils::parseJSON(jsonContent, pImpl->entries);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write in a simple key=value format that's both human-readable and machine-parseable
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert ConfigValue to string representation
        std::string valueStr = configValueToString(entry.value);
        
        file << entry.key << " = " << valueStr << "\n";
    }
    
    return true;
}

// Helper function to convert ConfigValue to string
std::string Config::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            // Use fixed precision to avoid scientific notation
            oss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << '\"' << arg << '\"';
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
                oss << std::fixed << std::setprecision(6) << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << '\"' << arg[i] << '\"';
            }
            oss << "]";
        } else {
            oss << "\"unknown_type\"";
        }
    }, value);
    
    return oss.str();
}

// Helper function to escape special characters in strings
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
            case '\0': result += "\\0"; break;
            default: result += c; break;
        }
    }
    
    return result;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });\n    
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
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

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Helper functions for JSON parsing
void json_utils::skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || 
                                 json[pos] == '\r' || json[pos] == '\t')) {
        pos++;
    }
}

bool json_utils::parseJSON(const std::string& json, std::vector<ConfigEntry>& outEntries) {
    size_t pos = 0;
    skipWhitespace(json, pos);
    
    if (json[pos] == '{') {
        return parseJSONObject(json, pos, outEntries);
    } else {
        return false; // Not a valid JSON object
    }
}

bool json_utils::parseJSONString(const std::string& json, size_t& pos, std::string& out) {
    if (pos >= json.size() || json[pos] != '\"') {
        return false;
    }
    
    pos++; // Skip opening quote
    size_t start = pos;
    
    while (pos < json.size()) {
        if (json[pos] == '\"') {
            out = json.substr(start, pos - start);
            pos++; // Skip closing quote
            return true;
        } else if (json[pos] == '\\' && pos + 1 < json.size()) {
            // Skip escaped character
            pos += 2;
        } else {
            pos++;
        }
    }
    
    return false; // Unclosed string
}

bool json_utils::parseJSONNumber(const std::string& json, size_t& pos, ConfigValue& out) {
    size_t start = pos;
    
    // Allow optional sign
    if (json[pos] == '+' || json[pos] == '-') {
        pos++;
    }
    
    // Parse digits
    while (pos < json.size() && std::isdigit(json[pos])) {
        pos++;
    }
    
    // Parse decimal part
    if (pos < json.size() && json[pos] == '.') {
        pos++;
        while (pos < json.size() && std::isdigit(json[pos])) {
            pos++;
        }
    }
    
    // Parse exponent part
    if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E')) {
        pos++;
        if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
            pos++;
        }
        while (pos < json.size() && std::isdigit(json[pos])) {
            pos++;
        }
    }
    
    if (pos == start) {
        return false;
    }
    
    std::string numStr = json.substr(start, pos - start);
    
    // Determine if it's an integer or floating point
    bool hasDecimal = std::find(numStr.begin(), numStr.end(), '.') != numStr.end();
    bool hasExponent = std::find(numStr.begin(), numStr.end(), 'e') != numStr.end() || 
                       std::find(numStr.begin(), numStr.end(), 'E') != numStr.end();
    
    if (hasDecimal || hasExponent) {
        out = std::stod(numStr);
    } else {
        // Try to determine if it's within int64 range
        try {
            long long intVal = std::stoll(numStr);
            if (intVal >= std::numeric_limits<int64_t>::min() && 
                intVal <= std::numeric_limits<int64_t>::max()) {
                out = static_cast<int64_t>(intVal);
            } else {
                out = intVal; // Use double if out of range
            }
        } catch (...) {
            out = std::stod(numStr);
        }
    }
    
    return true;
}

bool json_utils::parseJSONBoolean(const std::string& json, size_t& pos, bool& out) {
    if (pos + 4 < json.size() && json.substr(pos, 4) == "true") {
        out = true;
        pos += 4;
        return true;
    } else if (pos + 5 < json.size() && json.substr(pos, 5) == "false") {
        out = false;
        pos += 5;
        return true;
    }
    return false;
}

bool json_utils::parseJSONArray(const std::string& json, size_t& pos, ConfigValue& out);

bool json_utils::parseJSONObject(const std::string& json, size_t& pos, std::vector<ConfigEntry>& outEntries) {
    if (pos >= json.size() || json[pos] != '{') {
        return false;
    }
    
    pos++; // Skip opening brace
    skipWhitespace(json, pos);
    
    while (pos < json.size() && json[pos] != '}') {
        skipWhitespace(json, pos);
        
        // Parse key
        std::string key;
        if (!parseJSONString(json, pos, key)) {
            return false;
        }
        
        skipWhitespace(json, pos);
        
        if (pos >= json.size() || json[pos] != ':') {
            return false;
        }
        
        pos++; // Skip colon
        skipWhitespace(json, pos);
        
        // Parse value
        ConfigValue value;
        if (!parseJSONValue(json, pos, value)) {
            return false;
        }
        
        outEntries.emplace_back(key, value, ConfigSource::File);
        
        skipWhitespace(json, pos);
        
        if (pos < json.size() && json[pos] == ',') {
            pos++; // Skip comma
        } else if (pos < json.size() && json[pos] != '}') {
            return false;
        }
    }
    
    if (pos >= json.size() || json[pos] != '}') {
        return false;
    }
    
    pos++; // Skip closing brace
    return true;
}

bool json_utils::parseJSONArray(const std::string& json, size_t& pos, ConfigValue& out) {
    if (pos >= json.size() || json[pos] != '[') {
        return false;
    }
    
    pos++; // Skip opening bracket
    skipWhitespace(json, pos);
    
    std::vector<ConfigValue> array;
    
    if (pos < json.size() && json[pos] == ']') {
        pos++; // Skip closing bracket
        out = array;
        return true;
    }
    
    while (pos < json.size() && json[pos] != ']') {
        ConfigValue element;
        if (!parseJSONValue(json, pos, element)) {
            return false;
        }
        
        array.push_back(element);
        
        skipWhitespace(json, pos);
        
        if (pos < json.size() && json[pos] == ',') {
            pos++; // Skip comma
            skipWhitespace(json, pos);
        } else if (pos < json.size() && json[pos] != ']') {
            return false;
        }
    }
    
    if (pos >= json.size() || json[pos] != ']') {
        return false;
    }
    
    pos++; // Skip closing bracket
    out = array;
    return true;
}

bool json_utils::parseJSONValue(const std::string& json, size_t& pos, ConfigValue& out) {
    skipWhitespace(json, pos);
    
    if (pos >= json.size()) {
        return false;
    }
    
    switch (json[pos]) {
        case '\"': {
            std::string str;
            if (parseJSONString(json, pos, str)) {
                out = str;
                return true;
            }
            return false;
        }
        case '{': {
            std::vector<ConfigEntry> obj;
            if (parseJSONObject(json, pos, obj)) {
                // Convert object to string representation for now
                std::string jsonStr = "{";
                for (size_t i = 0; i < obj.size(); ++i) {
                    if (i > 0) jsonStr += ", ";
                    if (std::holds_alternative<std::string>(obj[i].value)) {
                        jsonStr += '\"' + obj[i].key + "\": \"" + std::get<std::string>(obj[i].value) + "\"";
                    } else {
                        jsonStr += '\"' + obj[i].key + "\": \"" + Config::configValueToString(obj[i].value) + "\"";
                    }
                }
                jsonStr += "}";
                out = jsonStr;
                return true;
            }
            return false;
        }
        case '[': {
            if (parseJSONArray(json, pos, out)) {
                return true;
            }
            return false;
        }
        case 't':
        case 'f': {
            bool boolVal;
            if (parseJSONBoolean(json, pos, boolVal)) {
                out = boolVal;
                return true;
            }
            return false;
        }
        default: {
            if (std::isdigit(json[pos]) || json[pos] == '+' || json[pos] == '-') {
                if (parseJSONNumber(json, pos, out)) {
                    return true;
                }
            }
            return false;
        }
    }
    
    return false;
}

}} // namespace nlm

// Alias for getOr as an alternative to getOr
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}