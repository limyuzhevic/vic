#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <memory>
#include <cmath>
#include <string>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try to parse as JSON first
    std::vector<ConfigEntry> jsonEntries;
    if (SimpleJSONParser::parseJSONFile(filepath, jsonEntries)) {
        // Clear existing entries and add JSON entries
        clear();
        for (const auto& entry : jsonEntries) {
            // Try to convert value to appropriate type
            set(entry.key, entry.value, ConfigSource::File);
        }
        return true;
    }
    
    // Fallback to simple key=value format (Phase 1 compatibility)
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
        file << entry.key << " = ";
        
        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format to avoid scientific notation
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
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
                    file << std::fixed << std::setprecision(6) << arg[i];
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
                file << "PLACEHOLDER_VALUE";
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

// Simple JSON parser for configuration files
class SimpleJSONParser {
public:
    static bool parseJSONFile(const std::string& filepath, std::vector<ConfigEntry>& entries) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return parseJSON(content, entries);
    }
    
    static bool parseJSON(const std::string& json, std::vector<ConfigEntry>& entries) {
        // Remove comments
        std::string cleanedJSON = removeComments(json);
        
        // Simple recursive descent parser for basic JSON
        size_t pos = 0;
        return parseObject(cleanedJSON, entries, pos) && pos == cleanedJSON.size();
    }
    
    static std::string removeComments(const std::string& json) {
        std::regex commentRegex(R"(//.*|\/\*[\s\S]*?\*\/)");
        std::string result = std::regex_replace(json, commentRegex, "");
        
        // Remove trailing commas before } or ]
        std::regex trailingCommaRegex(R",\s*([}\]])");
        result = std::regex_replace(result, trailingCommaRegex, "$1");
        
        return result;
    }
    
    static bool parseObject(const std::string& json, std::vector<ConfigEntry>& entries, size_t& pos) {
        if (pos >= json.size() || json[pos] != '{') {
            return false;
        }
        
        pos++; // Skip '{'
        skipWhitespace(json, pos);
        
        while (pos < json.size() && json[pos] != '}') {
            // Parse key
            std::string key;
            if (!parseString(json, key, pos)) {
                return false;
            }
            
            skipWhitespace(json, pos);
            if (pos >= json.size() || json[pos] != ':') {
                return false;
            }
            
            pos++; // Skip ':'
            skipWhitespace(json, pos);
            
            // Parse value
            ConfigValue value;
            if (!parseValue(json, value, pos)) {
                return false;
            }
            
            // Add entry
            entries.emplace_back(key, value, ConfigSource::File);
            
            skipWhitespace(json, pos);
            if (pos < json.size() && json[pos] == ',') {
                pos++; // Skip ','
                skipWhitespace(json, pos);
            }
        }
        
        if (pos >= json.size() || json[pos] != '}') {
            return false;
        }
        
        pos++; // Skip '}'
        return true;
    }
    
    static bool parseArray(const std::string& json, ConfigValue& value, size_t& pos) {
        if (pos >= json.size() || json[pos] != '[') {
            return false;
        }
        
        pos++; // Skip '['
        skipWhitespace(json, pos);
        
        std::vector<ConfigValue> array;
        
        if (pos < json.size() && json[pos] == ']') {
            pos++; // Skip ']'
            value = array;
            return true;
        }
        
        while (pos < json.size() && json[pos] != ']') {
            ConfigValue element;
            if (!parseValue(json, element, pos)) {
                return false;
            }
            
            array.push_back(element);
            
            skipWhitespace(json, pos);
            if (pos < json.size() && json[pos] == ',') {
                pos++; // Skip ','
                skipWhitespace(json, pos);
            }
        }
        
        if (pos >= json.size() || json[pos] != ']') {
            return false;
        }
        
        pos++; // Skip ']'
        value = array;
        return true;
    }
    
    static bool parseValue(const std::string& json, ConfigValue& value, size_t& pos) {
        skipWhitespace(json, pos);
        
        if (pos >= json.size()) {
            return false;
        }
        
        char ch = json[pos];
        
        if (ch == '"') {
            std::string str;
            if (parseString(json, str, pos)) {
                value = str;
                return true;
            }
            return false;
        } else if (ch == 't' || ch == 'f') {
            // Parse boolean
            std::string boolStr = parseIdentifier(json, pos);
            if (boolStr == "true") {
                value = true;
                return true;
            } else if (boolStr == "false") {
                value = false;
                return true;
            }
            return false;
        } else if (ch == 'n') {
            // Parse null (ignore)
            std::string nullStr = parseIdentifier(json, pos);
            if (nullStr == "null") {
                return true; // null values are ignored
            }
            return false;
        } else if (ch == '-' || std::isdigit(ch)) {
            // Parse number
            std::string numStr;
            bool hasDecimal = false;
            bool hasExp = false;
            
            while (pos < json.size()) {
                ch = json[pos];
                if (std::isdigit(ch)) {
                    numStr += ch;
                    pos++;
                } else if (ch == '.' && !hasDecimal) {
                    numStr += ch;
                    hasDecimal = true;
                    pos++;
                } else if ((ch == 'e' || ch == 'E') && !hasExp) {
                    numStr += ch;
                    hasExp = true;
                    pos++;
                    // Handle exponent sign
                    if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
                        numStr += json[pos];
                        pos++;
                    }
                } else {
                    break;
                }
            }
            
            try {
                if (hasDecimal || hasExp) {
                    value = std::stod(numStr);
                } else {
                    if (numStr.size() > 1 && numStr[0] == '0' && std::isdigit(numStr[1])) {
                        // Octal or hex number
                        if (numStr[1] == 'x' || numStr[1] == 'X') {
                            // Hex
                            value = std::stoll(numStr, nullptr, 16);
                        } else {
                            // Treat as decimal to avoid octal interpretation
                            value = std::stoll(numStr);
                        }
                    } else {
                        value = std::stoll(numStr);
                    }
                }
                return true;
            } catch (const std::exception&) {
                return false;
            }
        } else {
            return parseArray(json, value, pos);
        }
    }
    
    static bool parseString(const std::string& json, std::string& result, size_t& pos) {
        if (pos >= json.size() || json[pos] != '"') {
            return false;
        }
        
        pos++; // Skip '"'
        result.clear();
        
        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\') {
                pos++; // Skip escape character
                if (pos >= json.size()) {
                    return false;
                }
                
                char escaped = json[pos];
                switch (escaped) {
                    case '"': result += '"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        // Unicode escape (simple implementation)
                        if (pos + 4 >= json.size()) {
                            return false;
                        }
                        std::string hexCode = json.substr(pos + 1, 4);
                        try {
                            int code = std::stoi(hexCode, nullptr, 16);
                            result += static_cast<char>(code);
                            pos += 4;
                        } catch (const std::exception&) {
                            return false;
                        }
                        break;
                    }
                    default: result += escaped; break;
                }
            } else {
                result += json[pos];
            }
            pos++;
        }
        
        if (pos >= json.size() || json[pos] != '"') {
            return false;
        }
        
        pos++; // Skip '"'
        return true;
    }
    
    static std::string parseIdentifier(const std::string& json, size_t& pos) {
        std::string result;
        while (pos < json.size() && (std::isalpha(json[pos]) || json[pos] == '_')) {
            result += json[pos];
            pos++;
        }
        return result;
    }
    
    static void skipWhitespace(const std::string& json, size_t& pos) {
        while (pos < json.size() && std::isspace(json[pos])) {
            pos++;
        }
    }
};

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