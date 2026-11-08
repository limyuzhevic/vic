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
    
    // Simple JSON parsing for key-value pairs
    // This handles basic JSON objects like {"key": "value", "number": 42}
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        return parseJSON(content);
    } else {
        // Fallback to simple key=value format for backward compatibility
        return parseKeyValueFormat(content);
    }
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

bool Config::parseKeyValueFormat(const std::string& content) {
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

bool Config::parseJSON(const std::string& json) {
    // Simple JSON parsing for basic objects
    // This is a simplified implementation that handles:
    // { "key": "value", "number": 42, "array": [1, 2, 3] }
    // Assumes valid JSON format
    
    size_t start = json.find('{');
    if (start == std::string::npos) {
        return false;
    }
    
    size_t end = json.find('}', start);
    if (end == std::string::npos) {
        return false;
    }
    
    std::string object = json.substr(start + 1, end - start - 1);
    
    // Parse key-value pairs
    size_t pos = 0;
    while (pos < object.size()) {
        // Find next key
        size_t keyStart = object.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = object.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = object.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find colon after key
        size_t colonPos = object.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        // Find value
        size_t valueStart = object.find_first_not_of(' ', colonPos);
        if (valueStart == std::string::npos) break;
        
        // Parse different value types
        if (valueStart < object.size() && object[valueStart] == '"') {
            // String value
            size_t valueEnd = object.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) break;
            std::string value = object.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, value, ConfigSource::File);
            pos = valueEnd + 2; // Skip " and potential comma
        } else if (valueStart < object.size() && (object[valueStart] == '-' || std::isdigit(object[valueStart]))) {
            // Number value - extract digits
            size_t numEnd = valueStart;
            while (numEnd < object.size() && (std::isdigit(object[numEnd]) || object[numEnd] == '.' || object[numEnd] == '-')) {
                numEnd++;
            }
            std::string numStr = object.substr(valueStart, numEnd - valueStart);
            
            // Try to parse as integer first
            try {
                int intVal = std::stoi(numStr);
                set(key, intVal, ConfigSource::File);
            } catch (...) {
                try {
                    double floatVal = std::stod(numStr);
                    set(key, floatVal, ConfigSource::File);
                } catch (...) {
                    // If parsing fails, try string value
                    set(key, numStr, ConfigSource::File);
                }
            }
            pos = numEnd;
        } else if (valueStart < object.size() && object[valueStart] == '[') {
            // Array value - simple implementation for numeric arrays only
            size_t arrayEnd = object.find(']', valueStart);
            if (arrayEnd == std::string::npos) break;
            
            std::vector<double> array;
            size_t arrayPos = valueStart + 1;
            while (arrayPos < arrayEnd) {
                // Skip whitespace and commas
                while (arrayPos < arrayEnd && (std::isspace(object[arrayPos]) || object[arrayPos] == ',')) {
                    arrayPos++;
                }
                if (arrayPos >= arrayEnd) break;
                
                // Extract number
                size_t numStart = arrayPos;
                while (arrayPos < arrayEnd && (std::isdigit(object[arrayPos]) || object[arrayPos] == '.' || object[arrayPos] == '-')) {
                    arrayPos++;
                }
                
                if (numStart < arrayPos) {
                    std::string numStr = object.substr(numStart, arrayPos - numStart);
                    try {
                        double val = std::stod(numStr);
                        array.push_back(val);
                    } catch (...) {
                        // Skip invalid numbers
                    }
                }
            }
            
            // Store array as string for now (simplified)
            std::string arrayStr = "[";
            for (size_t i = 0; i < array.size(); ++i) {
                if (i > 0) arrayStr += ", ";
                arrayStr += std::to_string(array[i]);
            }
            arrayStr += "]";
            set(key, arrayStr, ConfigSource::File);
            
            pos = arrayEnd + 1;
        } else {
            // Unknown value type, skip
            pos = valueStart + 1;
        }
        
        // Skip comma separator
        while (pos < object.size() && (object[pos] == ',' || std::isspace(object[pos]))) {
            pos++;
        }
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
