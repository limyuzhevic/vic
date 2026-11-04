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

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Check if it looks like JSON (starts with { or [
    bool isJson = (!content.empty() && (content[0] == '{' || content[0] == '['));
    
    if (isJson) {
        return loadFromJson(content);
    } else {
        // Fall back to simple key=value format
        return loadFromSimpleFormat(content);
    }
}

bool Config::loadFromJson(const std::string& jsonContent) {
    try {
        // Simple JSON parser implementation
        // This is a basic implementation - for production, use a proper JSON library
        
        // Remove whitespace and comments
        std::string cleanJson;
        for (size_t i = 0; i < jsonContent.size(); ++i) {
            char c = jsonContent[i];
            if (c == '/' && i + 1 < jsonContent.size() && jsonContent[i + 1] == '/') {
                // Skip single-line comment
                while (i + 1 < jsonContent.size() && jsonContent[i + 1] != '\n') {
                    ++i;
                }
                continue;
            } else if (c == '/' && i + 1 < jsonContent.size() && jsonContent[i + 1] == '*') {
                // Skip multi-line comment
                while (i + 1 < jsonContent.size() && !(jsonContent[i + 1] == '*' && i + 2 < jsonContent.size() && jsonContent[i + 2] == '/')) {
                    ++i;
                }
                if (i + 2 < jsonContent.size()) i += 2; // Skip */
                continue;
            } else if (!std::isspace(static_cast<unsigned char>(c))) {
                cleanJson += c;
            }
        }
        
        // Parse key-value pairs from JSON object
        // This is a simplified parser for {"key": "value", "number": 42, "bool": true}
        size_t pos = 0;
        
        if (cleanJson.empty() || cleanJson[0] != '{') {
            return false;
        }
        
        // Remove braces
        cleanJson = cleanJson.substr(1, cleanJson.size() - 2);
        
        while (pos < cleanJson.size()) {
            // Find next key
            size_t keyStart = cleanJson.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = cleanJson.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = cleanJson.substr(keyStart + 1, keyEnd - keyStart - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace
            while (pos < cleanJson.size() && std::isspace(static_cast<unsigned char>(cleanJson[pos]))) {
                ++pos;
            }
            
            if (pos >= cleanJson.size() || cleanJson[pos] != ':') break;
            
            pos++; // Skip colon
            
            // Skip whitespace
            while (pos < cleanJson.size() && std::isspace(static_cast<unsigned char>(cleanJson[pos]))) {
                ++pos;
            }
            
            // Parse value
            ConfigValue value;
            bool parsed = false;
            
            if (cleanJson[pos] == '"') {
                // String value
                size_t valueEnd = cleanJson.find('"', pos + 1);
                if (valueEnd != std::string::npos) {
                    std::string strValue = cleanJson.substr(pos + 1, valueEnd - pos - 1);
                    value = strValue;
                    parsed = true;
                    pos = valueEnd + 1;
                }
            } else if (cleanJson[pos] == 't' || cleanJson[pos] == 'f') {
                // Boolean value
                if (cleanJson.substr(pos, 4) == "true") {
                    value = true;
                    parsed = true;
                    pos += 4;
                } else if (cleanJson.substr(pos, 5) == "false") {
                    value = false;
                    parsed = true;
                    pos += 5;
                }
            } else if (cleanJson[pos] == 'n') {
                // Null value (not supported)
                pos += 4;
            } else if (cleanJson[pos] == '-' || (cleanJson[pos] >= '0' && cleanJson[pos] <= '9')) {
                // Number value
                size_t valueEnd = pos;
                while (valueEnd < cleanJson.size() && (cleanJson[valueEnd] == '.' || cleanJson[valueEnd] == '-' || cleanJson[valueEnd] == 'e' || cleanJson[valueEnd] == 'E' || cleanJson[valueEnd] == '+' ||
                            (cleanJson[valueEnd] >= '0' && cleanJson[valueEnd] <= '9'))) {
                    valueEnd++;
                }
                
                std::string numStr = cleanJson.substr(pos, valueEnd - pos);
                
                // Try to parse as integer first
                try {
                    size_t posAfter;
                    int64_t intVal = std::stoll(numStr, &posAfter);
                    if (posAfter == numStr.size()) {
                        value = intVal;
                        parsed = true;
                        pos = valueEnd;
                    }
                } catch (...) {}
                
                if (!parsed) {
                    // Try to parse as float
                    try {
                        size_t posAfter;
                        double floatVal = std::stod(numStr, &posAfter);
                        if (posAfter == numStr.size()) {
                            value = floatVal;
                            parsed = true;
                            pos = valueEnd;
                        }
                    } catch (...) {}
                }
            } else if (cleanJson[pos] == '[') {
                // Array value (simplified)
                pos++; // Skip [
                std::vector<ConfigValue> array;
                bool inArray = true;
                size_t arrayStart = pos;
                
                while (pos < cleanJson.size() && inArray) {
                    if (cleanJson[pos] == ']') {
                        inArray = false;
                        pos++; // Skip ]
                        
                        // Parse array elements (simplified - only handles strings for now)
                        if (arrayStart < pos - 1) {
                            std::string arrayContent = cleanJson.substr(arrayStart, pos - 1 - arrayStart);
                            // Split by commas (very simplistic)
                            size_t commaPos = 0;
                            while (commaPos < arrayContent.size()) {
                                size_t nextComma = arrayContent.find(',', commaPos);
                                std::string element = arrayContent.substr(commaPos, nextComma - commaPos);
                                // Trim and remove quotes
                                element = trim(element);
                                if (!element.empty() && element.front() == '"' && element.back() == '"') {
                                    element = element.substr(1, element.size() - 2);
                                }
                                array.push_back(element);
                                commaPos = nextComma == std::string::npos ? arrayContent.size() : nextComma + 1;
                            }
                        }
                        break;
                    } else if (cleanJson[pos] == '"') {
                        size_t quoteEnd = cleanJson.find('"', pos + 1);
                        if (quoteEnd != std::string::npos) {
                            std::string element = cleanJson.substr(pos + 1, quoteEnd - pos - 1);
                            array.push_back(element);
                            pos = quoteEnd + 1;
                        } else {
                            pos = arrayContent.size();
                        }
                    } else {
                        pos++;
                    }
                }
                
                value = array;
                parsed = true;
            }
            
            if (!parsed) {
                // Skip to next key or end
                while (pos < cleanJson.size() && cleanJson[pos] != ',') {
                    pos++;
                }
                if (pos < cleanJson.size() && cleanJson[pos] == ',') {
                    pos++;
                }
                continue;
            }
            
            set(key, value, ConfigSource::File);
            
            // Skip whitespace after value
            while (pos < cleanJson.size() && std::isspace(static_cast<unsigned char>(cleanJson[pos]))) {
                pos++;
            }
            
            if (pos < cleanJson.size() && cleanJson[pos] == ',') {
                pos++; // Skip comma
            }
        }
        
        return true;
    }
}

bool Config::loadFromSimpleFormat(const std::string& content) {
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
