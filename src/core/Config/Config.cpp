#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <regex>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>

namespace nlm {

// Helper functions for config parsing
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Simple JSON Parser Implementation (no external dependencies)
class SimpleJsonParser {
public:
    static bool parse(const std::string& json, std::unordered_map<std::string, ConfigValue>& result) {
        result.clear();
        size_t pos = 0;
        return parseObject(json, pos, result);
    }

private:
    static bool parseObject(const std::string& json, size_t& pos, std::unordered_map<std::string, ConfigValue>& result) {
        skipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] != '{') {
            return false;
        }
        pos++; // Skip {
        skipWhitespace(json, pos);
        
        bool first = true;
        while (pos < json.size() && json[pos] != '}') {
            if (!first) {
                if (pos < json.size() && json[pos] == ',') {
                    pos++;
                } else {
                    return false;
                }
            }
            first = false;
            
            skipWhitespace(json, pos);
            
            // Parse key
            std::string key;
            if (!parseString(json, pos, key)) {
                return false;
            }
            
            skipWhitespace(json, pos);
            if (pos >= json.size() || json[pos] != ':') {
                return false;
            }
            pos++; // Skip :
            skipWhitespace(json, pos);
            
            // Parse value
            ConfigValue value;
            if (!parseValue(json, pos, value)) {
                return false;
            }
            
            result[key] = value;
            skipWhitespace(json, pos);
        }
        
        if (pos >= json.size() || json[pos] != '}') {
            return false;
        }
        pos++; // Skip }
        return true;
    }
    
    static bool parseArray(const std::string& json, size_t& pos, ConfigValue& result) {
        skipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] != '[') {
            return false;
        }
        pos++; // Skip [
        skipWhitespace(json, pos);
        
        std::vector<ConfigValue> array;
        bool first = true;
        while (pos < json.size() && json[pos] != ']') {
            if (!first) {
                if (pos < json.size() && json[pos] == ',') {
                    pos++;
                } else {
                    return false;
                }
            }
            first = false;
            
            ConfigValue element;
            if (!parseValue(json, pos, element)) {
                return false;
            }
            
            array.push_back(element);
            skipWhitespace(json, pos);
        }
        
        if (pos >= json.size() || json[pos] != ']') {
            return false;
        }
        pos++; // Skip ]
        
        // Convert vector to appropriate ConfigValue type
        if (!array.empty()) {
            // Try to determine the most appropriate type
            bool allInts = std::all_of(array.begin(), array.end(), [](const ConfigValue& v) {
                return std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v);
            });
            if (allInts) {
                std::vector<int> intArray;
                intArray.reserve(array.size());
                for (const auto& v : array) {
                    if (std::holds_alternative<int>(v)) {
                        intArray.push_back(std::get<int>(v));
                    } else if (std::holds_alternative<int64_t>(v)) {
                        intArray.push_back(static_cast<int>(std::get<int64_t>(v)));
                    }
                }
                result = intArray;
                return true;
            }
            
            bool allDoubles = std::all_of(array.begin(), array.end(), [](const ConfigValue& v) {
                return std::holds_alternative<double>(v);
            });
            if (allDoubles) {
                std::vector<double> doubleArray;
                doubleArray.reserve(array.size());
                for (const auto& v : array) {
                    if (std::holds_alternative<double>(v)) {
                        doubleArray.push_back(std::get<double>(v));
                    }
                }
                result = doubleArray;
                return true;
            }
            
            bool allStrings = std::all_of(array.begin(), array.end(), [](const ConfigValue& v) {
                return std::holds_alternative<std::string>(v);
            });
            if (allStrings) {
                std::vector<std::string> stringArray;
                stringArray.reserve(array.size());
                for (const auto& v : array) {
                    if (std::holds_alternative<std::string>(v)) {
                        stringArray.push_back(std::get<std::string>(v));
                    }
                }
                result = stringArray;
                return true;
            }
            
            // Mixed types - keep as vector<ConfigValue>
            result = array;
            return true;
        }
        
        result = array;
        return true;
    }
    
    static bool parseValue(const std::string& json, size_t& pos, ConfigValue& result) {
        skipWhitespace(json, pos);
        if (pos >= json.size()) {
            return false;
        }
        
        char c = json[pos];
        if (c == '{') {
            std::unordered_map<std::string, ConfigValue> obj;
            if (!parseObject(json, pos, obj)) {
                return false;
            }
            result = obj;
            return true;
        } else if (c == '[') {
            return parseArray(json, pos, result);
        } else if (c == '"') {
            std::string str;
            if (!parseString(json, pos, str)) {
                return false;
            }
            result = str;
            return true;
        } else if (c == 't' || c == 'f') {
            // Boolean
            if (json.substr(pos, 4) == "true") {
                pos += 4;
                result = true;
                return true;
            } else if (json.substr(pos, 5) == "false") {
                pos += 5;
                result = false;
                return true;
            }
            return false;
        } else if (c == 'n') {
            // Null
            if (json.substr(pos, 4) == "null") {
                pos += 4;
                result = std::string("null");
                return true;
            }
            return false;
        } else if (c == '-' || (c >= '0' && c <= '9')) {
            // Number
            size_t endPos = pos;
            while (endPos < json.size() && (json[endPos] == '-' || json[endPos] == '+' || 
                                            json[endPos] == '.' || json[endPos] == 'e' || json[endPos] == 'E' ||
                                            (json[endPos] >= '0' && json[endPos] <= '9'))) {
                endPos++;
            }
            
            std::string numStr = json.substr(pos, endPos - pos);
            try {
                // Try double first
                size_t idx;
                double d = std::stod(numStr, idx);
                if (idx == numStr.size()) {
                    // Check if it's an integer
                    size_t idx2;
                    long long ll = std::stoll(numStr, idx2);
                    if (idx2 == numStr.size()) {
                        result = static_cast<int>(ll);
                    } else {
                        result = d;
                    }
                    pos = endPos;
                    return true;
                } else {
                    result = d;
                    pos = endPos;
                    return true;
                }
            } catch (...) {
                return false;
            }
        }
        
        return false;
    }
    
    static bool parseString(const std::string& json, size_t& pos, std::string& result) {
        skipWhitespace(json, pos);
        if (pos >= json.size() || json[pos] != '"') {
            return false;
        }
        pos++; // Skip "
        
        result.clear();
        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\' && pos + 1 < json.size()) {
                pos++; // Skip backslash
                if (pos >= json.size()) {
                    return false;
                }
                switch (json[pos]) {
                    case '\"': result += '\"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        // Unicode escape
                        if (pos + 4 >= json.size()) {
                            return false;
                        }
                        std::string hex = json.substr(pos + 1, 4);
                        try {
                            char32_t codepoint = static_cast<char32_t>(std::stoul(hex, nullptr, 16));
                            // Convert to UTF-8
                            if (codepoint <= 0x7F) {
                                result += static_cast<char>(codepoint);
                            } else if (codepoint <= 0x7FF) {
                                result += static_cast<char>(0xC0 | (codepoint >> 6));
                                result += static_cast<char>(0x80 | (codepoint & 0x3F));
                            } else {
                                result += static_cast<char>(0xE0 | (codepoint >> 12));
                                result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                                result += static_cast<char>(0x80 | (codepoint & 0x3F));
                            }
                        } catch (...) {
                            return false;
                        }
                        pos += 4;
                        break;
                    }
                    default: return false;
                }
            } else {
                result += json[pos];
            }
            pos++;
        }
        
        if (pos >= json.size() || json[pos] != '"') {
            return false;
        }
        pos++; // Skip "
        return true;
    }
    
    static void skipWhitespace(const std::string& json, size_t& pos) {
        while (pos < json.size() && std::isspace(json[pos])) {
            pos++;
        }
    }
};

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load from file (JSON format with fallback to key=value)
    bool Config::loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Read entire file
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Try JSON parsing first (modern config format)
        std::unordered_map<std::string, ConfigValue> jsonData;
        if (SimpleJsonParser::parse(content, jsonData)) {
            // Successfully parsed as JSON
            for (const auto& pair : jsonData) {
                set(pair.first, pair.second, ConfigSource::File);
            }
            return true;
        }
        
        // Fallback to simple key=value format (legacy support)
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
    
    // Save as modern JSON format by default
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Serialize config value based on its type
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int>) {
                file << value;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << value;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format doubles without unnecessary trailing zeros
                std::ostringstream oss;
                oss.precision(6);
                oss << std::fixed << value;
                std::string str = oss.str();
                // Remove trailing zeros
                size_t pos = str.find('.');
                if (pos != std::string::npos) {
                    size_t lastNonZero = str.find_last_not_of('0');
                    if (lastNonZero != std::string::npos && lastNonZero > pos) {
                        str.erase(lastNonZero + 1);
                    }
                }
                file << str;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(value) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                const auto& vec = value;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                const auto& vec = value;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    std::ostringstream oss;
                    oss.precision(6);
                    oss << std::fixed << vec[i];
                    std::string str = oss.str();
                    // Remove trailing zeros
                    size_t pos = str.find('.');
                    if (pos != std::string::npos) {
                        size_t lastNonZero = str.find_last_not_of('0');
                        if (lastNonZero != std::string::npos && lastNonZero > pos) {
                            str.erase(lastNonZero + 1);
                        }
                    }
                    file << str;
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                const auto& vec = value;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << escapeString(vec[i]) << "\"";
                }
                file << "]";
            } else {
                file << "null"; // Fallback for unknown types
            }
        }, entry.value);
    }
    
    file << "\n}\n";
    
    return true;
}

// Helper function to escape strings for JSON
static std::string escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c >= ' ' && c <= '~') {
                    result += c;
                } else {
                    // Escape non-printable characters
                    std::ostringstream oss;
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') 
                        << static_cast<unsigned int>(c);
                    result += oss.str();
                }
                break;
        }
    }
    
    return result;
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
