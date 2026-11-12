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
    if (parseJSON(content)) {
        return true;
    }
    
    // Try to parse as YAML
    std::ifstream file2(filepath);
    if (file2.is_open() && parseYAML(content)) {
        return true;
    }
    
    // Fall back to simple key=value format
    std::ifstream file3(filepath);
    return parseSimpleKeyValue(file3);
}

bool Config::parseJSON(const std::string& jsonStr) {
    // Simple JSON parser for basic key-value pairs
    // Supports: {"key": "value", "key2": 123, "key3": true}
    size_t pos = 0;
    while (pos < jsonStr.size()) {
        // Find next " or {
        size_t keyStart = jsonStr.find('"', pos);
        if (keyStart == std::string::npos) {
            break;
        }
        
        size_t keyEnd = jsonStr.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            break;
        }
        
        std::string key = jsonStr.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find ":"
        size_t colonPos = jsonStr.find(':', keyEnd);
        if (colonPos == std::string::npos) {
            break;
        }
        
        // Find value (skip whitespace)
        size_t valueStart = jsonStr.find_first_not_of(" \t\n\r", colonPos + 1);
        if (valueStart == std::string::npos) {
            break;
        }
        
        // Parse different value types
        ConfigValue value;
        if (jsonStr[valueStart] == '\"') {
            // String value
            size_t valueEnd = jsonStr.find('\"', valueStart + 1);
            if (valueEnd == std::string::npos) {
                break;
            }
            std::string strValue = jsonStr.substr(valueStart + 1, valueEnd - valueStart - 1);
            value = strValue;
            pos = valueEnd + 1;
        } else if (jsonStr[valueStart] == 't' || jsonStr[valueStart] == 'f') {
            // Boolean value
            if (valueStart + 4 < jsonStr.size() && jsonStr.substr(valueStart, 5) == "true") {
                value = true;
                pos = valueStart + 4;
            } else if (valueStart + 5 < jsonStr.size() && jsonStr.substr(valueStart, 6) == "false") {
                value = false;
                pos = valueStart + 5;
            } else {
                break;
            }
        } else if (isdigit(jsonStr[valueStart]) || (jsonStr[valueStart] == '-' && valueStart + 1 < jsonStr.size() && isdigit(jsonStr[valueStart + 1]))) {
            // Numeric value
            size_t valueEnd = valueStart;
            while (valueEnd < jsonStr.size() && (isdigit(jsonStr[valueEnd]) || jsonStr[valueEnd] == '.' || jsonStr[valueEnd] == '-' || jsonStr[valueEnd] == 'e' || jsonStr[valueEnd] == 'E')) {
                valueEnd++;
            }
            std::string numStr = jsonStr.substr(valueStart, valueEnd - valueStart);
            
            // Try to parse as int
            try {
                int intVal = std::stoi(numStr);
                value = intVal;
                pos = valueEnd;
            } catch (...) {
                // Try to parse as double
                try {
                    double doubleVal = std::stod(numStr);
                    value = doubleVal;
                    pos = valueEnd;
                } catch (...) {
                    break;
                }
            }
        } else {
            // Unknown value type, skip to next comma
            size_t commaPos = jsonStr.find(',', valueStart);
            if (commaPos == std::string::npos) {
                break;
            }
            pos = commaPos + 1;
            continue;
        }
        
        // Skip whitespace and comma
        while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\t' || jsonStr[pos] == '\n' || jsonStr[pos] == '\r')) {
            pos++;
        }
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++;
        }
        
        set(key, value, ConfigSource::File);
    }
    
    return true;
}

bool Config::parseYAML(const std::string& yamlStr) {
    // Simple YAML parser for basic key-value pairs
    // Supports: key: value (with various value types)
    std::istringstream stream(yamlStr);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Find ":" that separates key from value
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            continue;
        }
        
        std::string key = trim(line.substr(0, colonPos));
        std::string value = trim(line.substr(colonPos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Parse value
        ConfigValue configValue;
        if (value == "true" || value == "false") {
            configValue = (value == "true");
        } else if (isdigit(value[0]) || (value[0] == '-' && value.size() > 1 && isdigit(value[1]))) {
            // Try to parse as int
            try {
                configValue = std::stoi(value);
            } catch (...) {
                // Try to parse as double
                try {
                    configValue = std::stod(value);
                } catch (...) {
                    continue;
                }
            }
        } else {
            configValue = value;
        }
        
        set(key, configValue, ConfigSource::File);
    }
    
    return true;
}

bool Config::parseSimpleKeyValue(std::ifstream& file) {
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
    // Determine file format based on extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool isJSON = (ext == "json");
    bool isYAML = (ext == "yaml" || ext == "yml");
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (isJSON) {
        file << "{\n";
        bool first = true;
        for (const auto& entry : pImpl->entries) {
            if (!first) {
                file << ",\n";
            } else {
                first = false;
            }
            
            file << "  \"" << entry.key << "\": ";
            
            // Format value based on type
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    file << std::to_string(val);
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                     std::is_same_v<T, std::vector<double>> ||
                                     std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        if constexpr (std::is_same_v<T, std::vector<int>>) {
                            file << val[i];
                        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                            file << val[i];
                        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                            file << "\"" << val[i] << "\"";
                        }
                    }
                    file << "]";
                } else {
                    file << "null";
                }
            }, entry.value);
        }
        file << "\n}\n";
    } else if (isYAML) {
        for (const auto& entry : pImpl->entries) {
            file << entry.key << ": ";
            
            // Format value based on type
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    file << std::to_string(val);
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                     std::is_same_v<T, std::vector<double>> ||
                                     std::is_same_v<T, std::vector<std::string>>) {
                    file << " [";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        if constexpr (std::is_same_v<T, std::vector<int>>) {
                            file << val[i];
                        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                            file << val[i];
                        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                            file << "\"" << val[i] << "\"";
                        }
                    }
                    file << "]";
                } else {
                    file << "null";
                }
            }, entry.value);
            file << "\n";
        }
    } else {
        // Default to simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Format value based on type
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    file << std::to_string(val);
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                     std::is_same_v<T, std::vector<double>> ||
                                     std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        if constexpr (std::is_same_v<T, std::vector<int>>) {
                            file << val[i];
                        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                            file << val[i];
                        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                            file << "\"" << val[i] << "\"";
                        }
                    }
                    file << "]";
                } else {
                    file << "PLACEHOLDER_VALUE";
                }
            }, entry.value);
            file << "\n";
        }
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
