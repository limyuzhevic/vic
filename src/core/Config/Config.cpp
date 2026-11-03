#include "Config.hpp"
#include <regex>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <cctype>

namespace nlm {

static std::string unescapeString(const std::string& str);
static bool parseJSONValue(const std::string& json, size_t& pos, ConfigValue& value);
static bool parseJSONString(const std::string& json, size_t& pos, std::string& value);
static bool parseJSONNumber(const std::string& json, size_t& pos, ConfigValue& value);
static bool parseJSONBoolean(const std::string& json, size_t& pos, ConfigValue& value);
static bool parseJSONNull(const std::string& json, size_t& pos);
static void skipWhitespace(const std::string& json, size_t& pos);

// Helper function implementations
static std::string unescapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[++i]) {
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

static bool parseJSONString(const std::string& json, size_t& pos, std::string& value) {
    if (pos >= json.size() || json[pos] != '"') return false;
    
    size_t start = ++pos;
    while (pos < json.size()) {
        if (json[pos] == '\\') {
            pos += 2; // Skip escaped character
            continue;
        } else if (json[pos] == '"') {
            value = unescapeString(json.substr(start, pos - start));
            pos++; // Skip closing quote
            return true;
        }
        pos++;
    }
    return false;
}

static bool parseJSONNumber(const std::string& json, size_t& pos, ConfigValue& value) {
    size_t start = pos;
    bool isDouble = false;
    bool isNegative = false;
    
    if (json[pos] == '-') {
        isNegative = true;
        pos++;
    }
    
    while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '.')) {
        if (json[pos] == '.') {
            isDouble = true;
        }
        pos++;
    }
    
    if (pos == start) return false;
    
    std::string numStr = json.substr(start, pos - start);
    
    if (isDouble) {
        double val = std::stod(numStr);
        if (isNegative) val = -val;
        value = val;
    } else {
        // Try int first, then int64_t
        try {
            int val = std::stoi(numStr);
            if (isNegative) val = -val;
            value = val;
        } catch (...) {
            try {
                int64_t val = std::stoll(numStr);
                if (isNegative) val = -val;
                value = val;
            } catch (...) {
                return false;
            }
        }
    }
    
    return true;
}

static bool parseJSONBoolean(const std::string& json, size_t& pos, ConfigValue& value) {
    if (pos + 4 <= json.size() && json.substr(pos, 4) == "true") {
        value = true;
        pos += 4;
        return true;
    } else if (pos + 5 <= json.size() && json.substr(pos, 5) == "false") {
        value = false;
        pos += 5;
        return true;
    }
    return false;
}

static bool parseJSONNull(const std::string& json, size_t& pos) {
    if (pos + 4 <= json.size() && json.substr(pos, 4) == "null") {
        pos += 4;
        return true;
    }
    return false;
}

static bool parseJSONValue(const std::string& json, size_t& pos, ConfigValue& value) {
    skipWhitespace(json, pos);
    
    if (pos >= json.size()) return false;
    
    char c = json[pos];
    
    if (c == '"') {
        std::string strVal;
        if (!parseJSONString(json, pos, strVal)) return false;
        value = strVal;
        return true;
    } else if (std::isdigit(c) || c == '-') {
        return parseJSONNumber(json, pos, value);
    } else if (c == 't' || c == 'f') {
        return parseJSONBoolean(json, pos, value);
    } else if (c == 'n') {
        if (parseJSONNull(json, pos)) {
            // For null values, use empty string as default
            value = std::string("");
            return true;
        }
        return false;
    } else if (c == '[') {
        // Parse array - for now treat as string
        size_t start = ++pos;
        while (pos < json.size() && json[pos] != ']') {
            pos++;
        }
        if (pos >= json.size()) return false;
        std::string arrayStr = json.substr(start, pos - start);
        value = arrayStr; // Simple approach for now
        pos++; // Skip closing bracket
        return true;
    }
    
    return false;
}

static void skipWhitespace(const std::string& json, size_t& pos) {
    while (pos < json.size() && std::isspace(json[pos])) {
        pos++;
    }

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    
    // Skip whitespace at beginning
    size_t pos = 0;
    skipWhitespace(jsonContent, pos);
    
    if (pos >= jsonContent.size()) {
        return false; // Empty file
    }
    
    // Try JSON object parsing
    std::map<std::string, ConfigValue> tempMap;
    if (parseJSONObject(jsonContent, pos, tempMap)) {
        // Successfully parsed as JSON
        for (const auto& pair : tempMap) {
            set(pair.first, pair.second, ConfigSource::File);
        }
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
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
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = trim(line.substr(0, eqPos));
            std::string value = trim(line.substr(eqPos + 1));
            
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
        // Fix: Output actual value instead of placeholder
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
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
