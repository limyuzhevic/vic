#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to trim whitespace and quotes from a string
static std::string trimString(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function to parse JSON values
static std::string parseJSONValue(const std::string& jsonStr, size_t& pos) {
    skipWhitespace(jsonStr, pos);
    
    if (pos >= jsonStr.size()) {
        return "";
    }
    
    char ch = jsonStr[pos];
    
    if (ch == '"') {
        // String value
        return parseJSONString(jsonStr, pos);
    } else if (ch == '{') {
        // Object value - for now, treat as string representation
        return parseJSONObject(jsonStr, pos);
    } else if (ch == '[') {
        // Array value - for now, treat as string representation
        return parseJSONArray(jsonStr, pos);
    } else if (ch == 't' || ch == 'f') {
        // Boolean value
        return parseJSONBoolean(jsonStr, pos);
    } else if (ch == 'n') {
        // Null value
        return parseJSONNull(jsonStr, pos);
    } else {
        // Number value
        return parseJSONNumber(jsonStr, pos);
    }
}

// Helper function to skip whitespace
static void skipWhitespace(const std::string& str, size_t& pos) {
    while (pos < str.size() && std::isspace(str[pos])) {
        pos++;
    }
}

// Helper function to parse JSON string
static std::string parseJSONString(const std::string& jsonStr, size_t& pos) {
    if (jsonStr[pos] != '"') {
        return "";
    }
    
    pos++; // Skip opening quote
    std::string result;
    
    while (pos < jsonStr.size() && jsonStr[pos] != '"') {
        if (jsonStr[pos] == '\\' && pos + 1 < jsonStr.size()) {
            // Escape sequence
            pos++;
            if (pos < jsonStr.size()) {
                result += jsonStr[pos];
            }
        } else {
            result += jsonStr[pos];
        }
        pos++;
    }
    
    if (pos < jsonStr.size() && jsonStr[pos] == '"') {
        pos++; // Skip closing quote
    }
    
    return result;
}

// Helper function to parse JSON object
static std::string parseJSONObject(const std::string& jsonStr, size_t& pos) {
    if (jsonStr[pos] != '{') {
        return "";
    }
    
    pos++; // Skip opening brace
    std::string result = "{";
    bool first = true;
    
    skipWhitespace(jsonStr, pos);
    
    while (pos < jsonStr.size() && jsonStr[pos] != '}') {
        if (!first) {
            result += ",";
        }
        first = false;
        
        // Parse key
        skipWhitespace(jsonStr, pos);
        std::string key = parseJSONString(jsonStr, pos);
        
        skipWhitespace(jsonStr, pos);
        if (pos < jsonStr.size() && jsonStr[pos] == ':') {
            pos++; // Skip colon
        }
        
        // Parse value
        std::string value = parseJSONValue(jsonStr, pos);
        result += "\"" + key + "\":\"" + value + "\"";
        
        skipWhitespace(jsonStr, pos);
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++; // Skip comma
        }
        
        skipWhitespace(jsonStr, pos);
    }
    
    if (pos < jsonStr.size() && jsonStr[pos] == '}') {
        pos++; // Skip closing brace
    }
    
    result += "}";
    return result;
}

// Helper function to parse JSON array
static std::string parseJSONArray(const std::string& jsonStr, size_t& pos) {
    if (jsonStr[pos] != '[') {
        return "";
    }
    
    pos++; // Skip opening bracket
    std::string result = "[";
    bool first = true;
    
    skipWhitespace(jsonStr, pos);
    
    while (pos < jsonStr.size() && jsonStr[pos] != ']') {
        if (!first) {
            result += ",";
        }
        first = false;
        
        // Parse array element
        std::string element = parseJSONValue(jsonStr, pos);
        result += "\"" + element + "\"";
        
        skipWhitespace(jsonStr, pos);
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++; // Skip comma
        }
        
        skipWhitespace(jsonStr, pos);
    }
    
    if (pos < jsonStr.size() && jsonStr[pos] == ']') {
        pos++; // Skip closing bracket
    }
    
    result += "]";
    return result;
}

// Helper function to parse JSON boolean
static std::string parseJSONBoolean(const std::string& jsonStr, size_t& pos) {
    if (jsonStr.substr(pos, 4) == "true") {
        pos += 4;
        return "true";
    } else if (jsonStr.substr(pos, 5) == "false") {
        pos += 5;
        return "false";
    }
    return "";
}

// Helper function to parse JSON null
static std::string parseJSONNull(const std::string& jsonStr, size_t& pos) {
    if (jsonStr.substr(pos, 4) == "null") {
        pos += 4;
        return "null";
    }
    return "";
}

// Helper function to parse JSON number
static std::string parseJSONNumber(const std::string& jsonStr, size_t& pos) {
    size_t start = pos;
    bool isFloat = false;
    
    while (pos < jsonStr.size() && (std::isdigit(jsonStr[pos]) || 
                                   jsonStr[pos] == '-' || 
                                   jsonStr[pos] == '+' || 
                                   jsonStr[pos] == '.' || 
                                   jsonStr[pos] == 'e' || 
                                   jsonStr[pos] == 'E')) {
        if (jsonStr[pos] == '.' || jsonStr[pos] == 'e' || jsonStr[pos] == 'E') {
            isFloat = true;
        }
        pos++;
    }
    
    return jsonStr.substr(start, pos - start);
}

// Helper function to parse JSON string value
static std::string parseJSONStringValue(const std::string& jsonStr, size_t& pos) {
    skipWhitespace(jsonStr, pos);
    if (pos >= jsonStr.size() || jsonStr[pos] != '"') {
        return "";
    }
    return parseJSONString(jsonStr, pos);
}

// Helper function to parse JSON number value
static double parseJSONNumberValue(const std::string& jsonStr, size_t& pos) {
    skipWhitespace(jsonStr, pos);
    std::string numStr = parseJSONNumber(jsonStr, pos);
    try {
        return std::stod(numStr);
    } catch (...) {
        return 0.0;
    }
}

// Helper function to parse JSON boolean value
static bool parseJSONBooleanValue(const std::string& jsonStr, size_t& pos) {
    skipWhitespace(jsonStr, pos);
    std::string boolStr = parseJSONBoolean(jsonStr, pos);
    return boolStr == "true";
}

// Helper function to parse JSON null value
static bool parseJSONNullValue(const std::string& jsonStr, size_t& pos) {
    skipWhitespace(jsonStr, pos);
    std::string nullStr = parseJSONNull(jsonStr, pos);
    return nullStr == "null";
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Try to parse as JSON first
    try {
        if (parseJSON(content)) {
            return true;
        }
    } catch (...) {
        // If JSON parsing fails, fall back to key=value format
    }
    
    // Fallback to simple key=value format
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
    
    return true;
}

bool Config::parseJSON(const std::string& jsonStr) {
    size_t pos = 0;
    skipWhitespace(jsonStr, pos);
    
    if (jsonStr[pos] != '{') {
        return false;
    }
    
    pos++; // Skip opening brace
    skipWhitespace(jsonStr, pos);
    
    while (pos < jsonStr.size() && jsonStr[pos] != '}') {
        // Parse key
        skipWhitespace(jsonStr, pos);
        std::string key = parseJSONString(jsonStr, pos);
        
        skipWhitespace(jsonStr, pos);
        if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
            return false;
        }
        
        pos++; // Skip colon
        skipWhitespace(jsonStr, pos);
        
        // Parse value based on content
        std::string value;
        
        if (pos < jsonStr.size() && jsonStr[pos] == '"') {
            value = parseJSONString(jsonStr, pos);
        } else if (pos < jsonStr.size() && (std::isdigit(jsonStr[pos]) || jsonStr[pos] == '-' || jsonStr[pos] == '+')) {
            value = parseJSONNumber(jsonStr, pos);
        } else if (pos < jsonStr.size() && jsonStr[pos] == 't') {
            value = parseJSONBoolean(jsonStr, pos);
        } else if (pos < jsonStr.size() && jsonStr[pos] == 'n') {
            value = parseJSONNull(jsonStr, pos);
        } else if (pos < jsonStr.size() && jsonStr[pos] == '[') {
            value = parseJSONArray(jsonStr, pos);
        } else if (pos < jsonStr.size() && jsonStr[pos] == '{') {
            value = parseJSONObject(jsonStr, pos);
        }
        
        if (!key.empty() && !value.empty()) {
            set(key, value, ConfigSource::File);
        }
        
        skipWhitespace(jsonStr, pos);
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++; // Skip comma
        }
        
        skipWhitespace(jsonStr, pos);
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
    
    // Write in JSON format for better readability
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": \"" << entryToString(entry) << "\"";
        
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "}\n";
    
    return true;
}

std::string Config::entryToString(const ConfigEntry& entry) const {
    std::ostringstream oss;
    
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ",";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ",";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ",";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        }
    }, entry.value);
    
    return oss.str();
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
