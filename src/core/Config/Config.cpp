#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cassert>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Internal helpers
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

bool Config::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Clear existing configuration
    clear();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first (modern format)
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try JSON parsing with simple implementation
    if (parseSimpleJSON(content)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    std::ifstream file2(filepath);
    if (!file2.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file2, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || startsWith(line, "#") || startsWith(line, "//")) {
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
            
            // Try to convert value to appropriate type
            setValueFromString(key, value);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (startsWith(arg, "--")) {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setValueFromString(key, value);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setValueFromString(key, value);
        }
    }
    return true;
}

void Config::setValueFromString(const std::string& key, const std::string& value) {
    // Try to determine value type based on content
    if (value == "true" || value == "false") {
        bool boolVal = (value == "true");
        set(key, boolVal, ConfigSource::CommandLine);
    } else {
        try {
            size_t pos;
            long long intVal = std::stoll(value, &pos);
            if (pos == value.size()) {
                if (intVal >= std::numeric_limits<int>::min() && 
                    intVal <= std::numeric_limits<int>::max()) {
                    set(key, static_cast<int>(intVal), ConfigSource::CommandLine);
                    return;
                }
            }
        } catch (...) {
            // Fall through to try double
        }
        
        try {
            size_t pos;
            double doubleVal = std::stod(value, &pos);
            if (pos == value.size()) {
                set(key, doubleVal, ConfigSource::CommandLine);
                return;
            }
        } catch (...) {
            // Fall through to string
        }
        
        // Default to string
        set(key, value, ConfigSource::CommandLine);
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write as improved key=value format with better serialization
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Serialize based on value type
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
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
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                // For int, double, int64_t, etc.
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

bool Config::parseSimpleJSON(const std::string& content) {
    // Simple JSON parser for basic objects and arrays
    // This is a simplified implementation that handles:
    // - { "key": value }
    // - Arrays: [value1, value2, ...]
    // - Basic types: numbers, strings, booleans, null
    
    size_t pos = 0;
    return parseJSONValue(content, pos);
}

bool Config::parseJSONValue(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) {
        return false;
    }
    
    char c = content[pos];
    
    if (c == '{') {
        return parseJSONObject(content, pos);
    } else if (c == '[') {
        return parseJSONArray(content, pos);
    } else if (c == '\"') {
        return parseJSONString(content, pos);
    } else if (c == 't' || c == 'f') {
        return parseJSONBoolean(content, pos);
    } else if (c == 'n') {
        return parseJSONNull(content, pos);
    } else {
        return parseJSONNumber(content, pos);
    }
}

bool Config::parseJSONObject(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size() || content[pos] != '{') {
        return false;
    }
    
    pos++; // Skip '{'
    skipWhitespace(content, pos);
    
    while (pos < content.size() && content[pos] != '}') {
        // Parse key
        if (!parseJSONString(content, pos)) {
            return false;
        }
        
        skipWhitespace(content, pos);
        if (pos >= content.size() || content[pos] != ':') {
            return false;
        }
        
        pos++; // Skip ':'
        skipWhitespace(content, pos);
        
        // Parse value
        if (!parseJSONValue(content, pos)) {
            return false;
        }
        
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++; // Skip ','
            skipWhitespace(content, pos);
        } else if (pos < content.size() && content[pos] != '}') {
            return false;
        }
    }
    
    if (pos < content.size() && content[pos] == '}') {
        pos++; // Skip '}'
        return true;
    }
    
    return false;
}

bool Config::parseJSONArray(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size() || content[pos] != '[') {
        return false;
    }
    
    pos++; // Skip '['
    skipWhitespace(content, pos);
    
    while (pos < content.size() && content[pos] != ']') {
        if (!parseJSONValue(content, pos)) {
            return false;
        }
        
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++; // Skip ','
            skipWhitespace(content, pos);
        } else if (pos < content.size() && content[pos] != ']') {
            return false;
        }
    }
    
    if (pos < content.size() && content[pos] == ']') {
        pos++; // Skip ']'
        return true;
    }
    
    return false;
}

bool Config::parseJSONString(const std::string& content, size_t& pos) {
    if (pos >= content.size() || content[pos] != '\"') {
        return false;
    }
    
    pos++; // Skip '"'
    
    while (pos < content.size() && content[pos] != '\"') {
        if (content[pos] == '\\') {
            pos++; // Skip escape character
            if (pos < content.size()) {
                pos++; // Skip escaped character
            }
        } else {
            pos++;
        }
    }
    
    if (pos < content.size() && content[pos] == '\"') {
        pos++; // Skip '"'
        return true;
    }
    
    return false;
}

bool Config::parseJSONBoolean(const std::string& content, size_t& pos) {
    if (startsWith(content.substr(pos), "true")) {
        pos += 4;
        return true;
    } else if (startsWith(content.substr(pos), "false")) {
        pos += 5;
        return true;
    }
    return false;
}

bool Config::parseJSONNull(const std::string& content, size_t& pos) {
    if (startsWith(content.substr(pos), "null")) {
        pos += 4;
        return true;
    }
    return false;
}

bool Config::parseJSONNumber(const std::string& content, size_t& pos) {
    size_t start = pos;
    bool hasDecimal = false;
    bool hasExponent = false;
    
    // Parse number
    if (pos < content.size() && (content[pos] == '-' || content[pos] == '+')) {
        pos++;
    }
    
    while (pos < content.size() && std::isdigit(content[pos])) {
        pos++;
    }
    
    if (pos < content.size() && content[pos] == '.') {
        hasDecimal = true;
        pos++;
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    if (pos < content.size() && (content[pos] == 'e' || content[pos] == 'E')) {
        hasExponent = true;
        pos++;
        if (pos < content.size() && (content[pos] == '-' || content[pos] == '+')) {
            pos++;
        }
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    return pos > start;
}

void Config::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && std::isspace(content[pos])) {
        pos++;
    }
}

bool Config::parseSimpleJSON(const std::string& content) {
    size_t pos = 0;
    return parseJSONValue(content, pos);
}

bool Config::parseJSONValue(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) {
        return false;
    }
    
    char c = content[pos];
    
    if (c == '{') {
        return parseJSONObject(content, pos);
    } else if (c == '[') {
        return parseJSONArray(content, pos);
    } else if (c == '\"') {
        return parseJSONString(content, pos);
    } else if (c == 't' || c == 'f') {
        return parseJSONBoolean(content, pos);
    } else if (c == 'n') {
        return parseJSONNull(content, pos);
    } else {
        return parseJSONNumber(content, pos);
    }
}

bool Config::parseJSONObject(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size() || content[pos] != '{') {
        return false;
    }
    
    pos++; // Skip '{'
    skipWhitespace(content, pos);
    
    while (pos < content.size() && content[pos] != '}') {
        // Parse key
        if (!parseJSONString(content, pos)) {
            return false;
        }
        
        skipWhitespace(content, pos);
        if (pos >= content.size() || content[pos] != ':') {
            return false;
        }
        
        pos++; // Skip ':'
        skipWhitespace(content, pos);
        
        // Parse value
        if (!parseJSONValue(content, pos)) {
            return false;
        }
        
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++; // Skip ','
            skipWhitespace(content, pos);
        } else if (pos < content.size() && content[pos] != '}') {
            return false;
        }
    }
    
    if (pos < content.size() && content[pos] == '}') {
        pos++; // Skip '}'
        return true;
    }
    
    return false;
}

bool Config::parseJSONArray(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size() || content[pos] != '[') {
        return false;
    }
    
    pos++; // Skip '['
    skipWhitespace(content, pos);
    
    while (pos < content.size() && content[pos] != ']') {
        if (!parseJSONValue(content, pos)) {
            return false;
        }
        
        skipWhitespace(content, pos);
        
        if (pos < content.size() && content[pos] == ',') {
            pos++; // Skip ','
            skipWhitespace(content, pos);
        } else if (pos < content.size() && content[pos] != ']') {
            return false;
        }
    }
    
    if (pos < content.size() && content[pos] == ']') {
        pos++; // Skip ']'
        return true;
    }
    
    return false;
}

bool Config::parseJSONString(const std::string& content, size_t& pos) {
    if (pos >= content.size() || content[pos] != '\"') {
        return false;
    }
    
    pos++; // Skip '"'
    
    while (pos < content.size() && content[pos] != '\"') {
        if (content[pos] == '\\') {
            pos++; // Skip escape character
            if (pos < content.size()) {
                pos++; // Skip escaped character
            }
        } else {
            pos++;
        }
    }
    
    if (pos < content.size() && content[pos] == '\"') {
        pos++; // Skip '"'
        return true;
    }
    
    return false;
}

bool Config::parseJSONBoolean(const std::string& content, size_t& pos) {
    if (startsWith(content.substr(pos), "true")) {
        pos += 4;
        return true;
    } else if (startsWith(content.substr(pos), "false")) {
        pos += 5;
        return true;
    }
    return false;
}

bool Config::parseJSONNull(const std::string& content, size_t& pos) {
    if (startsWith(content.substr(pos), "null")) {
        pos += 4;
        return true;
    }
    return false;
}

bool Config::parseJSONNumber(const std::string& content, size_t& pos) {
    size_t start = pos;
    bool hasDecimal = false;
    bool hasExponent = false;
    
    // Parse number
    if (pos < content.size() && (content[pos] == '-' || content[pos] == '+')) {
        pos++;
    }
    
    while (pos < content.size() && std::isdigit(content[pos])) {
        pos++;
    }
    
    if (pos < content.size() && content[pos] == '.') {
        hasDecimal = true;
        pos++;
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    if (pos < content.size() && (content[pos] == 'e' || content[pos] == 'E')) {
        hasExponent = true;
        pos++;
        if (pos < content.size() && (content[pos] == '-' || content[pos] == '+')) {
            pos++;
        }
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    return pos > start;
}

void Config::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && std::isspace(content[pos])) {
        pos++;
    }
}

// Explicit template instantiations
namespace {
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
}

} // namespace nlm