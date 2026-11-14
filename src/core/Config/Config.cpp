#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>
#include <unordered_map>

namespace nlm {

// Forward declaration for JSON parsing helper
static bool parseJsonFile(const std::string& filepath, Config& config);
static bool parseYamlFile(const std::string& filepath, Config& config);
static bool parseSimpleFile(const std::string& filepath, Config& config);

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// JSON parsing helper
static bool parseJsonFile(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parsing for common cases
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Remove whitespace
    std::string trimmed;
    for (char c : content) {
        if (!std::isspace(c)) {
            trimmed += c;
        }
    }
    
    // Simple key=value pair extraction from JSON-like format
    // This handles basic JSON objects like {"key1": "value1", "key2": 123}
    size_t pos = 0;
    while (pos < trimmed.length()) {
        if (trimmed[pos] == '{' || trimmed[pos] == '}') {
            pos++;
            continue;
        }
        
        // Find key (string in quotes)
        size_t keyStart = trimmed.find('"', pos);
        if (keyStart == std::string::npos) {
            break;
        }
        
        size_t keyEnd = trimmed.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            break;
        }
        
        std::string key = trimmed.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find separator and value
        size_t sepPos = trimmed.find(':', keyEnd);
        if (sepPos == std::string::npos) {
            break;
        }
        
        size_t valueStart = trimmed.find_first_not_of(' ', sepPos + 1);
        if (valueStart == std::string::npos) {
            break;
        }
        
        // Determine value type and parse
        char firstChar = trimmed[valueStart];
        
        if (firstChar == '"') {
            // String value
            size_t valueEnd = trimmed.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) {
                break;
            }
            std::string value = trimmed.substr(valueStart + 1, valueEnd - valueStart - 1);
            config.set(key, value, ConfigSource::File);
            pos = valueEnd + 1;
        } else if (firstChar == 't' || firstChar == 'f') {
            // Boolean value
            size_t valueEnd = trimmed.find_first_of(",}", valueStart);
            if (valueEnd == std::string::npos) {
                break;
            }
            std::string valueStr = trimmed.substr(valueStart, valueEnd - valueStart);
            bool value = (valueStr == "true");
            config.set(key, value, ConfigSource::File);
            pos = valueEnd;
        } else if (firstChar == '-' || std::isdigit(firstChar)) {
            // Number value
            size_t valueEnd = trimmed.find_first_of(",}", valueStart);
            if (valueEnd == std::string::npos) {
                break;
            }
            std::string valueStr = trimmed.substr(valueStart, valueEnd - valueStart);
            
            // Try to parse as integer or double
            try {
                if (valueStr.find('.') != std::string::npos) {
                    double value = std::stod(valueStr);
                    config.set(key, value, ConfigSource::File);
                } else {
                    int64_t value = std::stoll(valueStr);
                    config.set(key, value, ConfigSource::File);
                }
            } catch (...) {
                // Skip invalid numbers
                pos = valueEnd;
                continue;
            }
            pos = valueEnd;
        }
        
        // Skip to next comma or closing brace
        while (pos < trimmed.length() && trimmed[pos] != ',' && trimmed[pos] != '}') {
            pos++;
        }
        
        if (pos < trimmed.length() && trimmed[pos] == ',') {
            pos++;
        }
    }
    
    return true;
}

// YAML parsing helper (simplified)
static bool parseYamlFile(const std::string& filepath, Config& config) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        line = trim(line);
        if (line.empty() || line.find("#") != std::string::npos) {
            continue;
        }
        
        // Handle simple key-value pairs: key: value
        size_t pos = line.find(':');
        if (pos != std::string::npos && pos > 0) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Parse value based on content
            if (value.empty()) {
                continue;
            }
            
            // Check for quotes
            if ((value.front() == '"' && value.back() == '"') ||
                (value.front() == '\'' && value.back() == '\'')) {
                value = value.substr(1, value.size() - 2);
                config.set(key, value, ConfigSource::File);
            } else if (value == "true" || value == "false") {
                config.set(key, value == "true", ConfigSource::File);
            } else {
                // Try numeric parsing
                try {
                    if (value.find('.') != std::string::npos) {
                        double num = std::stod(value);
                        config.set(key, num, ConfigSource::File);
                    } else {
                        int64_t num = std::stoll(value);
                        config.set(key, num, ConfigSource::File);
                    }
                } catch (...) {
                    // If not a number, treat as string
                    config.set(key, value, ConfigSource::File);
                }
            }
        }
    }
    
    return true;
}

// Original simple key=value parser
static bool parseSimpleFile(const std::string& filepath, Config& config) {
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
            
            config.set(key, value, ConfigSource::File);
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
