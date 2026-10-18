#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cstring>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Enhanced JSON parser for configuration files
    bool parseJSON(const std::string& content);
    bool parseJSONValue(const std::string& content, size_t& pos, ConfigValue& outValue);
    void skipWhitespace(const std::string& content, size_t& pos);
    std::string escapeString(const std::string& str);
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced JSON parser for configuration files
bool Config::Impl::parseJSON(const std::string& content) {
    size_t pos = 0;
    skipWhitespace(content, pos);
    
    if (pos >= content.size() || content[pos] != '{') {
        return false; // Expected object
    }
    
    pos++; // Skip '{'
    skipWhitespace(content, pos);
    
    while (pos < content.size()) {
        skipWhitespace(content, pos);
        
        // Check for closing brace
        if (content[pos] == '}') {
            break;
        }
        
        // Parse key (string)
        if (content[pos] != '"') {
            return false; // Expected string key
        }
        
        size_t keyStart = ++pos; // Skip opening quote
        while (pos < content.size() && content[pos] != '"') {
            pos++;
        }
        
        if (pos >= content.size()) {
            return false; // Unterminated string
        }
        
        std::string key = content.substr(keyStart, pos - keyStart);
        pos++; // Skip closing quote
        skipWhitespace(content, pos);
        
        if (pos >= content.size() || content[pos] != ':') {
            return false; // Expected colon
        }
        
        pos++; // Skip colon
        skipWhitespace(content, pos);
        
        // Parse value
        ConfigValue value;
        if (!parseJSONValue(content, pos, value)) {
            return false;
        }
        
        entries.emplace_back(key, value, ConfigSource::File);
        
        skipWhitespace(content, pos);
        
        // Check for comma or closing brace
        if (pos < content.size() && content[pos] == ',') {
            pos++; // Skip comma
            skipWhitespace(content, pos);
        }
        else if (pos < content.size() && content[pos] == '}') {
            break;
        }
        else {
            return false;
        }
    }
    
    if (pos >= content.size() || content[pos] != '}') {
        return false; // Missing closing brace
    }
    
    return true;
}

bool Config::Impl::parseJSONValue(const std::string& content, size_t& pos, ConfigValue& outValue) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) {
        return false;
    }
    
    char c = content[pos];
    
    if (c == '"') {
        // String
        size_t start = ++pos; // Skip opening quote
        while (pos < content.size() && content[pos] != '"') {
            if (content[pos] == '\\') {
                pos++; // Skip escaped character
            }
            pos++;
        }
        
        if (pos >= content.size()) {
            return false; // Unterminated string
        }
        
        std::string str = content.substr(start, pos - start);
        // Unescape the string
        size_t outPos = 0;
        std::string unescaped;
        unescaped.reserve(str.size());
        while (outPos < str.size()) {
            if (str[outPos] == '\\' && outPos + 1 < str.size()) {
                switch (str[outPos + 1]) {
                    case '\"': unescaped += '\"'; break;
                    case '\\\\': unescaped += '\\\\'; break;
                    case '\\/': unescaped += '\\/'; break;
                    case '\\b': unescaped += '\b'; break;
                    case '\\f': unescaped += '\f'; break;
                    case '\\n': unescaped += '\n'; break;
                    case '\\r': unescaped += '\r'; break;
                    case '\\t': unescaped += '\t'; break;
                    default: unescaped += str[outPos + 1]; break;
                }
                outPos += 2;
            } else {
                unescaped += str[outPos];
                outPos++;
            }
        }
        
        outValue = unescaped;
        pos++; // Skip closing quote
    }
    else if (c == '-' || (c >= '0' && c <= '9')) {
        // Number
        size_t start = pos;
        bool hasDecimal = false;
        bool hasExponent = false;
        
        while (pos < content.size()) {
            char ch = content[pos];
            if (ch == '.' || ch == 'e' || ch == 'E') {
                if (ch == '.' && hasDecimal) break;
                if ((ch == 'e' || ch == 'E') && hasExponent) break;
                if (ch == 'e' || ch == 'E') {
                    hasExponent = true;
                    pos++;
                    if (pos < content.size() && (content[pos] == '+' || content[pos] == '-')) {
                        pos++;
                    }
                    continue;
                }
                hasDecimal = true;
            } else if (ch == '-' && hasExponent) {
                pos++;
                continue;
            } else if (!(ch >= '0' && ch <= '9')) {
                break;
            }
            pos++;
        }
        
        std::string numStr = content.substr(start, pos - start);
        
        // Try to parse as double first
        try {
            size_t endPos;
            double val = std::stod(numStr, &endPos);
            if (endPos == numStr.size()) {
                outValue = val;
            } else {
                // Try as int
                size_t endPos2;
                long long val = std::stoll(numStr, &endPos2);
                if (endPos2 == numStr.size()) {
                    outValue = val;
                } else {
                    return false;
                }
            }
        } catch (...) {
            return false;
        }
    }
    else if (c == 't' || c == 'f') {
        // Boolean
        if (pos + 3 < content.size() && content.compare(pos, 4, "true") == 0) {
            outValue = true;
            pos += 4;
        } else if (pos + 4 < content.size() && content.compare(pos, 5, "false") == 0) {
            outValue = false;
            pos += 5;
        } else {
            return false;
        }
    }
    else if (c == 'n') {
        // null
        if (pos + 3 < content.size() && content.compare(pos, 4, "null") == 0) {
            outValue = std::string("");
            pos += 4;
        } else {
            return false;
        }
    }
    else if (c == '[') {
        // Array
        pos++; // Skip '['
        skipWhitespace(content, pos);
        
        std::vector<std::string> arrayElements;
        
        while (pos < content.size() && content[pos] != ']') {
            ConfigValue element;
            if (!parseJSONValue(content, pos, element)) {
                return false;
            }
            
            // Store element as string for now (can be enhanced)
            std::string elementStr;
            std::visit([&elementStr](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    elementStr = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    elementStr = std::to_string(arg);
                } else if constexpr (std::is_same_v<T, bool>) {
                    elementStr = arg ? "true" : "false";
                } else if constexpr (std::is_same_v<T, std::string>) {
                    elementStr = arg;
                }
            }, element);
            
            arrayElements.push_back(elementStr);
            
            skipWhitespace(content, pos);
            
            if (pos < content.size() && content[pos] == ',') {
                pos++; // Skip comma
                skipWhitespace(content, pos);
            }
            else if (pos < content.size() && content[pos] == ']') {
                break;
            } else {
                return false;
            }
        }
        
        if (pos >= content.size() || content[pos] != ']') {
            return false; // Missing closing bracket
        }
        
        pos++; // Skip ']'
        
        // Store array as vector<string> for now
        outValue = arrayElements;
    }
    else {
        return false; // Unsupported value type
    }
    
    skipWhitespace(content, pos);
    return true;
}

void Config::Impl::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && std::isspace(static_cast<unsigned char>(content[pos]))) {
        pos++;
    }
}

std::string Config::Impl::escapeString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\\\""; break;
            case '\\\\': result += "\\\\\\\\"; break;
            case '\\/': result += "\\\\/"; break;
            case '\\b': result += "\\\\b"; break;
            case '\\f': result += "\\\\f"; break;
            case '\\n': result += "\\\\n"; break;
            case '\\r': result += "\\\\r"; break;
            case '\\t': result += "\\\\t"; break;
            default:
                if (c < 32 || c > 126) {
                    result += "\\\\u";
                    char hex[5];
                    snprintf(hex, sizeof(hex), "%04x", static_cast<unsigned int>(c));
                    result += hex;
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

// Enhanced configuration loading with JSON support
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try JSON format first
    if (pImpl->parseJSON(content)) {
        return true;
    }
    
    // Fall back to simple key=value format
    file.clear();
    file.seekg(0);
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save in JSON format
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << pImpl->escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                file << "[]"; // Simplified for now
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    return true;
}

// Template implementations for configuration access
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
