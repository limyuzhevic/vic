#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <cmath>
#include <unordered_map>

namespace nlm {

// Forward declarations for JSON parsing
static bool parseJSON(const std::string& jsonStr, std::unordered_map<std::string, ConfigValue>& entries);
static ConfigValue parseJSONValue(const std::string& jsonStr, size_t& pos);
static std::string parseJSONString(const std::string& jsonStr, size_t& pos);
static double parseJSONNumber(const std::string& jsonStr, size_t& pos);
static bool parseJSONBool(const std::string& jsonStr, size_t& pos);
static std::vector<ConfigValue> parseJSONArray(const std::string& jsonStr, size_t& pos);
static void skipWhitespace(const std::string& jsonStr, size_t& pos);
static bool isDigit(char c);
static bool isHexDigit(char c);
static bool isAlpha(char c);
static bool isAlphaNumeric(char c);

struct Config::Impl {
    std::vector<ConfigEntry> entries;
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
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Skip BOM if present
    if (content.size() >= 3 && content[0] == '\xEF' && content[1] == '\xBB' && content[2] == '\xBF') {
        content = content.substr(3);
    }
    
    // Try to parse as JSON
    std::unordered_map<std::string, ConfigValue> jsonEntries;
    if (parseJSON(content, jsonEntries)) {
        NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
        // Clear existing entries and add JSON entries
        pImpl->entries.clear();
        for (const auto& pair : jsonEntries) {
            set(pair.first, pair.second, ConfigSource::File);
        }
        return true;
    }
    
    // Fall back to simple key=value format
    file.clear();
    file.seekg(0);
    
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

// JSON parsing helper functions
static bool parseJSON(const std::string& jsonStr, std::unordered_map<std::string, ConfigValue>& entries) {
    size_t pos = 0;
    skipWhitespace(jsonStr, pos);
    
    if (jsonStr[pos] != '{') {
        return false; // Not a JSON object
    }
    
    pos++; // Skip {
    skipWhitespace(jsonStr, pos);
    
    while (pos < jsonStr.size() && jsonStr[pos] != '}') {
        // Parse key
        skipWhitespace(jsonStr, pos);
        if (jsonStr[pos] != '"') {
            return false; // Expected string key
        }
        
        std::string key = parseJSONString(jsonStr, pos);
        skipWhitespace(jsonStr, pos);
        
        if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
            return false; // Expected colon
        }
        
        pos++; // Skip colon
        skipWhitespace(jsonStr, pos);
        
        // Parse value
        ConfigValue value = parseJSONValue(jsonStr, pos);
        entries[key] = value;
        
        skipWhitespace(jsonStr, pos);
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++; // Skip comma
            skipWhitespace(jsonStr, pos);
        }
    }
    
    if (pos < jsonStr.size() && jsonStr[pos] == '}') {
        pos++; // Skip }
    }
    
    return pos == jsonStr.size(); // Should reach end of string
}

static ConfigValue parseJSONValue(const std::string& jsonStr, size_t& pos) {
    skipWhitespace(jsonStr, pos);
    
    if (pos >= jsonStr.size()) {
        return ConfigValue(""); // Empty string as fallback
    }
    
    switch (jsonStr[pos]) {
        case '"':
            return parseJSONString(jsonStr, pos);
        case '[':
            return parseJSONArray(jsonStr, pos);
        case 't':
            return parseJSONBool(jsonStr, pos) ? ConfigValue(true) : ConfigValue("");
        case 'f':
            return parseJSONBool(jsonStr, pos) ? ConfigValue(false) : ConfigValue("");
        case 'n':
            if (jsonStr.compare(pos, 4, "null") == 0) {
                pos += 4;
                return ConfigValue("");
            }
            return ConfigValue("");
        default:
            // Try to parse as number
            return parseJSONNumber(jsonStr, pos);
    }
}

static std::string parseJSONString(const std::string& jsonStr, size_t& pos) {
    if (jsonStr[pos] != '"') {
        return "";
    }
    
    pos++; // Skip opening quote
    std::string result;
    
    while (pos < jsonStr.size() && jsonStr[pos] != '"') {
        if (jsonStr[pos] == '\\' && pos + 1 < jsonStr.size()) {
            // Handle escape sequences
            pos++; // Skip backslash
            if (pos < jsonStr.size()) {
                switch (jsonStr[pos]) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        // Unicode escape - simplified for now
                        if (pos + 4 < jsonStr.size()) {
                            pos += 4;
                            result += "?";
                        }
                        break;
                    }
                    default: result += jsonStr[pos]; break;
                }
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

static double parseJSONNumber(const std::string& jsonStr, size_t& pos) {
    size_t startPos = pos;
    bool isNegative = false;
    bool hasDecimal = false;
    bool hasExponent = false;
    
    // Check for negative sign
    if (jsonStr[pos] == '-') {
        isNegative = true;
        pos++;
    }
    
    // Parse integer part
    while (pos < jsonStr.size() && isDigit(jsonStr[pos])) {
        pos++;
    }
    
    // Check for decimal part
    if (pos < jsonStr.size() && jsonStr[pos] == '.') {
        hasDecimal = true;
        pos++;
        while (pos < jsonStr.size() && isDigit(jsonStr[pos])) {
            pos++;
        }
    }
    
    // Check for exponent
    if (pos < jsonStr.size() && (jsonStr[pos] == 'e' || jsonStr[pos] == 'E')) {
        hasExponent = true;
        pos++; // Skip e/E
        if (pos < jsonStr.size() && (jsonStr[pos] == '+' || jsonStr[pos] == '-')) {
            pos++; // Skip sign
        }
        while (pos < jsonStr.size() && isDigit(jsonStr[pos])) {
            pos++;
        }
    }
    
    if (pos == startPos) {
        return 0.0; // No number found
    }
    
    // Convert to double
    std::string numStr = jsonStr.substr(startPos, pos - startPos);
    return std::stod(numStr) * (isNegative ? -1.0 : 1.0);
}

static bool parseJSONBool(const std::string& jsonStr, size_t& pos) {
    if (jsonStr.compare(pos, 4, "true") == 0) {
        pos += 4;
        return true;
    } else if (jsonStr.compare(pos, 5, "false") == 0) {
        pos += 5;
        return false;
    }
    return false;
}

static std::vector<ConfigValue> parseJSONArray(const std::string& jsonStr, size_t& pos) {
    if (jsonStr[pos] != '[') {
        return {};
    }
    
    pos++; // Skip [
    std::vector<ConfigValue> result;
    skipWhitespace(jsonStr, pos);
    
    while (pos < jsonStr.size() && jsonStr[pos] != ']') {
        // Parse element
        ConfigValue value = parseJSONValue(jsonStr, pos);
        result.push_back(value);
        
        skipWhitespace(jsonStr, pos);
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++; // Skip comma
            skipWhitespace(jsonStr, pos);
        }
    }
    
    if (pos < jsonStr.size() && jsonStr[pos] == ']') {
        pos++; // Skip ]
    }
    
    return result;
}

static void skipWhitespace(const std::string& jsonStr, size_t& pos) {
    while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
        pos++;
    }
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool isHexDigit(char c) {
    return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isAlphaNumeric(char c) {
    return isDigit(c) || isAlpha(c);
}

} // namespace nlm

