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

bool Config::loadFromString(const std::string& content, ConfigSource source) {
    return parseConfigFormat(content, source);
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

// Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Helper functions for parsing
    bool parseJSONValue(const std::string& str, ConfigValue& value);
    bool parseYAMLValue(const std::string& str, ConfigValue& value);
    bool parseSimpleValue(const std::string& str, ConfigValue& value);
    
    // Parse both JSON and YAML formats
    bool parseConfigFormat(const std::string& content, ConfigSource source);
    
    // Internal parser implementations
    bool parseJSON(const std::string& content, ConfigSource source);
    bool parseYAML(const std::string& content, ConfigSource source);
    bool parseSimpleFormat(const std::string& content, ConfigSource source);

// Helper functions for parsing
namespace {

bool isJSON(const std::string& str) {
    if (str.empty()) return false;
    char first = str.front();
    return first == '{' || first == '[';
}

bool isYAML(const std::string& str) {
    if (str.empty()) return false;
    // Simple heuristic: YAML usually has colons and isn't wrapped in brackets
    return !isJSON(str) && str.find(':') != std::string::npos;
}

bool parseJSONValue(const std::string& str, ConfigValue& value) {
    // Remove surrounding quotes for strings
    std::string trimmed = trim(str);
    if (trimmed.size() >= 2 && ((trimmed.front() == '"' && trimmed.back() == '"') ||
                                (trimmed.front() == '\'' && trimmed.back() == '\''))) {
        value = trimmed.substr(1, trimmed.size() - 2);
        return true;
    }
    
    // Try to parse as boolean
    if (trimmed == "true") {
        value = true;
        return true;
    } else if (trimmed == "false") {
        value = false;
        return true;
    }
    
    // Try to parse as number
    try {
        if (trimmed.find('.') != std::string::npos || trimmed.find('e') != std::string::npos || 
            trimmed.find('E') != std::string::npos) {
            size_t pos;
            float floatVal = std::stof(trimmed, &pos);
            if (pos == trimmed.size()) {
                value = floatVal;
                return true;
            }
            double doubleVal = std::stod(trimmed, &pos);
            if (pos == trimmed.size()) {
                value = doubleVal;
                return true;
            }
        } else {
            size_t pos;
            int intVal = std::stoi(trimmed, &pos);
            if (pos == trimmed.size()) {
                value = intVal;
                return true;
            }
            int64_t int64Val = std::stoll(trimmed, &pos);
            if (pos == trimmed.size()) {
                value = int64Val;
                return true;
            }
        }
    } catch (...) {
    }
    
    return false;
}

bool parseYAMLValue(const std::string& str, ConfigValue& value) {
    return parseJSONValue(str, value);
}

bool parseSimpleValue(const std::string& str, ConfigValue& value) {
    return parseJSONValue(str, value);
}

bool parseConfigFormat(const std::string& content, ConfigSource source) {
    if (isJSON(content)) {
        return parseJSON(content, source);
    } else if (isYAML(content)) {
        return parseYAML(content, source);
    } else {
        return parseSimpleFormat(content, source);
    }
}

bool parseJSON(const std::string& content, ConfigSource source) {
    // Very simple JSON parser for key=value pairs
    // Real implementation would use a proper JSON library
    size_t pos = 0;
    while (pos < content.size()) {
        // Find key
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) {
            break; // No more keys
        }
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            break;
        }
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find value (skip whitespace)
        size_t valuePos = content.find(':', keyEnd);
        if (valuePos == std::string::npos) {
            break;
        }
        
        size_t valueStart = content.find_first_not_of(": \t\n\r", valuePos + 1);
        if (valueStart == std::string::npos) {
            break;
        }
        
        // Parse value based on first character
        ConfigValue value;
        if (content[valueStart] == '"') {
            size_t valueEnd = content.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) {
                break;
            }
            value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            pos = valueEnd + 1;
        } else if (content[valueStart] == '{') {
            // Array - simplified parsing
            size_t valueEnd = content.find('}', valueStart);
            if (valueEnd == std::string::npos) {
                break;
            }
            // For arrays, we'd need to parse individual elements
            // For now, treat as string
            value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
            pos = valueEnd + 1;
        } else {
            // Number or boolean - parse until comma or closing brace
            size_t valueEnd = content.find_first_of(",}", valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = content.size();
            }
            std::string valueStr = content.substr(valueStart, valueEnd - valueStart);
            if (!parseJSONValue(valueStr, value)) {
                // Try simple parsing
                if (!parseSimpleValue(valueStr, value)) {
                    break;
                }
            }
            pos = valueEnd;
        }
        
        set(key, value, source);
        
        // Skip to next key
        pos = content.find_first_not_of(", \t\n\r", pos);
    }
    
    return true;
}

bool parseYAML(const std::string& content, ConfigSource source) {
    // Very simple YAML parser for key=value pairs
    // Real implementation would use a proper YAML library
    size_t pos = 0;
    while (pos < content.size()) {
        // Find key (YAML format: key: value)
        size_t colonPos = content.find(':', pos);
        if (colonPos == std::string::npos) {
            break;
        }
        
        // Find key start (before colon, skip whitespace)
        size_t keyEnd = content.find_last_not_of(" \t\n\r", colonPos - 1);
        if (keyEnd == std::string::npos) {
            break;
        }
        
        size_t keyStart = content.find_last_of(" \t\n\r", keyEnd);
        if (keyStart == std::string::npos) {
            keyStart = 0;
        } else {
            keyStart++;
        }
        
        std::string key = content.substr(keyStart, keyEnd - keyStart + 1);
        
        // Find value start (after colon)
        size_t valuePos = content.find_first_not_of(": \t\n\r", colonPos + 1);
        if (valuePos == std::string::npos) {
            break;
        }
        
        // Parse value based on first character
        ConfigValue value;
        if (content[valuePos] == '"') {
            size_t valueEnd = content.find('"', valuePos + 1);
            if (valueEnd == std::string::npos) {
                break;
            }
            value = content.substr(valuePos + 1, valueEnd - valuePos - 1);
            pos = valueEnd + 1;
        } else {
            // Number or boolean - parse until newline or comment
            size_t valueEnd = content.find_first_of("\n#", valuePos);
            if (valueEnd == std::string::npos) {
                valueEnd = content.size();
            }
            std::string valueStr = content.substr(valuePos, valueEnd - valuePos);
            if (!parseJSONValue(valueStr, value)) {
                // Try simple parsing
                if (!parseSimpleValue(valueStr, value)) {
                    break;
                }
            }
            pos = valueEnd;
        }
        
        set(key, value, source);
        
        // Skip to next line
        pos = content.find_first_not_of("\n\r", pos);
    }
    
    return true;
}

bool parseSimpleFormat(const std::string& content, ConfigSource source) {
    // Parse simple key=value format (original format)
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
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
            
            set(key, value, source);
        }
    }
    
    return true;
}

} // anonymous namespace


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
