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
    
    // Try JSON first, fall back to simple key=value format
    std::string content((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
    
    // Remove BOM if present
    if (content.size() >= 3 && content[0] == '\xEF' && content[1] == '\xBB' && content[2] == '\xBF') {
        content = content.substr(3);
    }
    
    // Check if it looks like JSON (starts with { or contains "":
    if (content.find('{') != std::string::npos || content.find('"') != std::string::npos) {
        return loadFromJSON(content);
    } else {
        return loadFromSimpleFormat(content);
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
    
    // Save in a clean key=value format (also compatible with loadFromFile)
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << visitToString(entry.value) << "\n";
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

// Helper function to convert ConfigValue to string representation
std::string Config::visitToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << '"' << arg << '"';
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int> || 
                             std::is_same_v<T, int64_t> || 
                             std::is_same_v<T, uint64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double> || 
                             std::is_same_v<T, float>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                             std::is_same_v<T, std::vector<double>> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            oss << '[';
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i) oss << ', ';
                oss << visitToString(arg[i]);
            }
            oss << ']';
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

// Load from simple key=value format
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to infer type
            try {
                if (value == "true" || value == "false") {
                    bool boolVal = (value == "true");
                    set(key, boolVal, ConfigSource::File);
                } else if (value.find('.') != std::string::npos) {
                    double doubleVal = std::stod(value);
                    set(key, doubleVal, ConfigSource::File);
                } else {
                    // Try as integer
                    size_t posEnd;
                    int intVal = std::stoi(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, intVal, ConfigSource::File);
                    } else {
                        // Try as int64_t
                        int64_t int64Val = std::stoll(value, &posEnd);
                        if (posEnd == value.size()) {
                            set(key, int64Val, ConfigSource::File);
                        } else {
                            // Default to string
                            set(key, value, ConfigSource::File);
                        }
                    }
                }
            } catch (const std::exception&) {
                // If parsing fails, treat as string
                set(key, value, ConfigSource::File);
            }
        }
    }
    return true;
}

// Load from JSON format
bool Config::loadFromJSON(const std::string& jsonContent) {
    try {
        // Simple JSON parser for basic key-value pairs
        // For production, use a proper JSON library like nlohmann/json
        size_t pos = 0;
        std::string key;
        std::string value;
        
        while ((pos = jsonContent.find('"', pos)) != std::string::npos) {
            size_t endQuote = jsonContent.find('"', pos + 1);
            if (endQuote == std::string::npos) break;
            
            key = jsonContent.substr(pos + 1, endQuote - pos - 1);
            pos = endQuote + 1;
            
            // Skip whitespace
            while (pos < jsonContent.size() && std::isspace(jsonContent[pos])) pos++;
            
            // Expect ':' 
            if (pos >= jsonContent.size() || jsonContent[pos] != ':') break;
            pos++;
            
            // Skip whitespace
            while (pos < jsonContent.size() && std::isspace(jsonContent[pos])) pos++;
            
            // Find next value quote or array/object start
            char delimiter = '\0';
            if (pos < jsonContent.size() && jsonContent[pos] == '"') {
                delimiter = '"';
            } else if (pos < jsonContent.size() && jsonContent[pos] == '[') {
                delimiter = '[';
            } else if (pos < jsonContent.size() && jsonContent[pos] == '{') {
                delimiter = '{';
            }
            
            if (delimiter == '\0') {
                // Try to parse literal values (true, false, null, numbers)
                size_t endValue = pos;
                while (endValue < jsonContent.size() && 
                       jsonContent[endValue] != ',' && 
                       jsonContent[endValue] != '}' && 
                       jsonContent[endValue] != ']') {
                    endValue++;
                }
                value = jsonContent.substr(pos, endValue - pos);
                pos = endValue;
            } else if (delimiter == '"') {
                size_t endQuote = jsonContent.find('"', pos + 1);
                if (endQuote == std::string::npos) break;
                value = jsonContent.substr(pos + 1, endQuote - pos - 1);
                pos = endQuote + 1;
            } else {
                // For arrays and objects, we'd need proper parsing
                // For now, skip and use empty string
                value = "";
                pos = jsonContent.find(delimiter == '[' ? ']' : '}', pos);
                if (pos == std::string::npos) break;
                pos++;
            }
            
            // Set the value
            setConfigFromString(key, value);
            
            // Skip whitespace and comma
            while (pos < jsonContent.size() && std::isspace(jsonContent[pos])) pos++;
            if (pos < jsonContent.size() && jsonContent[pos] == ',') pos++;
        }
        
        return true;
    } catch (const std::exception&) {
        // If JSON parsing fails, fall back to simple format
        return loadFromSimpleFormat(jsonContent);
    }
}

// Helper to set config value from string (used by JSON parsing)
void Config::setConfigFromString(const std::string& key, const std::string& valueStr) {
    // Try to infer type from string
    if (valueStr == "true" || valueStr == "false") {
        bool val = (valueStr == "true");
        set(key, val, ConfigSource::File);
    } else if (valueStr == "null") {
        // Skip null values
        return;
    } else {
        // Check if it's a number
        size_t posEnd;
        try {
            // Try double first
            double doubleVal = std::stod(valueStr, &posEnd);
            if (posEnd == valueStr.size()) {
                set(key, doubleVal, ConfigSource::File);
                return;
            }
            
            // Try int64
            int64_t int64Val = std::stoll(valueStr, &posEnd);
            if (posEnd == valueStr.size()) {
                set(key, int64Val, ConfigSource::File);
                return;
            }
            
            // Try int
            int intVal = std::stoi(valueStr, &posEnd);
            if (posEnd == valueStr.size()) {
                set(key, intVal, ConfigSource::File);
                return;
            }
            
            // Default to string
            set(key, valueStr, ConfigSource::File);
        } catch (const std::exception&) {
            set(key, valueStr, ConfigSource::File);
        }
    }
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
