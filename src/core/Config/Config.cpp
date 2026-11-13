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
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Try to parse as JSON first (starts with { or [)
    if (!content.empty() && (content.find("{') != std::string::npos || content.find("['") != std::string::npos)) {
        return loadFromJson(content);
    }
    
    // Fall back to simple key=value format
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

bool Config::loadFromJson(const std::string& jsonString) {
    // Simple JSON parser implementation
    // Supports basic structure: { "key": value, "nested": { "key2": value } }
    // Value types: string (quoted), number (int/double), bool (true/false), array ([1,2,3])
    
    try {
        // Find the outermost braces
        if (jsonString.empty() || jsonString.front() != '{' || jsonString.back() != '}') {
            return false;
        }
        
        size_t pos = 1;  // Skip opening brace
        while (pos < jsonString.size() - 1) {
            // Skip whitespace
            while (pos < jsonString.size() - 1 && std::isspace(jsonString[pos])) {
                pos++;
            }
            
            if (pos < jsonString.size() - 1 && jsonString[pos] == '"') {
                // Parse key
                size_t keyEnd = jsonString.find('"', pos + 1);
                if (keyEnd == std::string::npos) return false;
                
                std::string key = jsonString.substr(pos + 1, keyEnd - pos - 1);
                pos = keyEnd + 1;
                
                // Skip whitespace
                while (pos < jsonString.size() - 1 && std::isspace(jsonString[pos])) {
                    pos++;
                }
                
                if (pos >= jsonString.size() - 1 || jsonString[pos] != ':') {
                    return false;
                }
                
                pos++;  // Skip colon
                
                // Skip whitespace
                while (pos < jsonString.size() - 1 && std::isspace(jsonString[pos])) {
                    pos++;
                }
                
                // Parse value
                if (pos < jsonString.size() - 1 && jsonString[pos] == '"') {
                    // String value
                    size_t valueEnd = jsonString.find('"', pos + 1);
                    if (valueEnd == std::string::npos) return false;
                    
                    std::string value = jsonString.substr(pos + 1, valueEnd - pos - 1);
                    set(key, value, ConfigSource::File);
                    pos = valueEnd + 1;
                } else if (pos < jsonString.size() - 1 && jsonString[pos] == '{') {
                    // Object value - for now, skip nested objects
                    size_t braceCount = 1;
                    size_t objectEnd = pos;
                    for (size_t i = pos + 1; i < jsonString.size(); ++i) {
                        if (jsonString[i] == '{') braceCount++;
                        else if (jsonString[i] == '}') braceCount--;
                        if (braceCount == 0) {
                            objectEnd = i;
                            break;
                        }
                    }
                    // Skip the entire object
                    pos = objectEnd + 1;
                } else if (pos < jsonString.size() - 1 && jsonString[pos] == '[') {
                    // Array value - for now, skip arrays
                    size_t bracketCount = 1;
                    size_t arrayEnd = pos;
                    for (size_t i = pos + 1; i < jsonString.size(); ++i) {
                        if (jsonString[i] == '[') bracketCount++;
                        else if (jsonString[i] == ']') bracketCount--;
                        if (bracketCount == 0) {
                            arrayEnd = i;
                            break;
                        }
                    }
                    // Skip the entire array
                    pos = arrayEnd + 1;
                } else {
                    // Number or boolean value
                    size_t valueEnd = pos;
                    while (valueEnd < jsonString.size() - 1 && 
                           (std::isdigit(jsonString[valueEnd]) || jsonString[valueEnd] == '.' || 
                            jsonString[valueEnd] == '-' || jsonString[valueEnd] == '+' ||
                            jsonString[valueEnd] == 't' || jsonString[valueEnd] == 'f')) {
                        valueEnd++;
                    }
                    
                    std::string valueStr = jsonString.substr(pos, valueEnd - pos);
                    
                    // Try to determine type
                    try {
                        // Try boolean
                        if (valueStr == "true" || valueStr == "false") {
                            set(key, valueStr == "true", ConfigSource::File);
                        } else {
                            // Try integer
                            try {
                                size_t intPos;
                                long long intVal = std::stoll(valueStr, &intPos);
                                if (intPos == valueStr.size()) {
                                    set(key, intVal, ConfigSource::File);
                                    pos = valueEnd;
                                    continue;
                                }
                            } catch (...) {}
                            
                            // Try double
                            try {
                                size_t doublePos;
                                double doubleVal = std::stod(valueStr, &doublePos);
                                if (doublePos == valueStr.size()) {
                                    set(key, doubleVal, ConfigSource::File);
                                    pos = valueEnd;
                                    continue;
                                }
                            } catch (...) {}
                            
                            // Fall back to string
                            set(key, valueStr, ConfigSource::File);
                        }
                    } catch (...) {
                        set(key, valueStr, ConfigSource::File);
                    }
                    
                    pos = valueEnd;
                }
                
                // Skip whitespace
                while (pos < jsonString.size() - 1 && std::isspace(jsonString[pos])) {
                    pos++;
                }
                
                // Skip comma if present
                if (pos < jsonString.size() - 1 && jsonString[pos] == ',') {
                    pos++;
                }
            } else {
                // Invalid JSON format
                return false;
            }
        }
        
        return true;
    } catch (...) {
        return false;
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
    
    // Create JSON format for better compatibility with modern tools
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << arg[j] << "\"";
                    } else {
                        file << arg[j];
                    }
                }
                file << "]";
            } else {
                file << "null";
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
