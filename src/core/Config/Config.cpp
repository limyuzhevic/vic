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
    if (parseJSON(file)) {
        return true;
    }
    
    // Reset file position and try as YAML (simple key=value format)
    file.clear();
    file.seekg(0);
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs (original Phase 1 format)
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
            
            // Try to convert to appropriate type
            convertStringValue(key, value);
            
            set(key, getConvertedValue(key), ConfigSource::File);
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
    
    // Write in JSON format for better structure
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Write value based on type
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                file << val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i) file << ", ";
                    file << "\"" << val[i] << "\"";
                }
                file << "]";
            } else {
                file << "\"UNKNOWN_TYPE\"";
            }
        }, entry.value);
        
        file << "\n";
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

// Helper functions for JSON/YAML parsing

bool Config::parseJSON(std::ifstream& file) {
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple JSON parsing - look for { ... } structure
    size_t braceStart = content.find('{');
    size_t braceEnd = content.rfind('}');
    
    if (braceStart == std::string::npos || braceEnd == std::string::npos) {
        return false; // Not JSON format
    }
    
    // Parse key=value pairs within braces
    std::string jsonContent = content.substr(braceStart + 1, braceEnd - braceStart - 1);
    
    // Simple parsing for key=value pairs separated by commas
    size_t pos = 0;
    while (pos < jsonContent.length()) {
        // Find next key
        size_t keyEnd = jsonContent.find(':', pos);
        if (keyEnd == std::string::npos) break;
        
        std::string key = trim(jsonContent.substr(pos, keyEnd - pos));
        // Remove quotes from key
        if (key.size() >= 2 && ((key.front() == '"' && key.back() == '"') ||
                               (key.front() == '\'' && key.back() == '\''))) {
            key = key.substr(1, key.size() - 2);
        }
        
        // Find value
        size_t valueStart = jsonContent.find_first_not_of(' ', keyEnd + 1);
        if (valueStart == std::string::npos) break;
        
        size_t valueEnd = jsonContent.find(',', valueStart);
        if (valueEnd == std::string::npos) {
            valueEnd = jsonContent.length();
        }
        
        std::string value = trim(jsonContent.substr(valueStart, valueEnd - valueStart));
        
        // Remove quotes from value if present
        if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                   (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Try to convert to appropriate type
        convertStringValue(key, value);
        
        set(key, getConvertedValue(key), ConfigSource::File);
        
        // Move to next pair
        pos = valueEnd + 1;
        while (pos < jsonContent.length() && jsonContent[pos] != ',') {
            pos++;
        }
        if (pos < jsonContent.length()) {
            pos++; // Skip comma
        }
    }
    
    return true;
}

void Config::convertStringValue(const std::string& key, const std::string& value) {
    // Store the string value temporarily in a local map for conversion
    // This is a simplified approach - in production, you'd want better handling
    
    // Try to convert to boolean
    if (value == "true" || value == "false") {
        bool boolVal = (value == "true");
        set(key, boolVal, ConfigSource::File);
        return;
    }
    
    // Try to convert to integer
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.length()) {
            set(key, intVal, ConfigSource::File);
            return;
        }
    } catch (...) {}
    
    // Try to convert to double
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.length()) {
            set(key, doubleVal, ConfigSource::File);
            return;
        }
    } catch (...) {}
    
    // If no conversion works, store as string
    // (The set method will handle string conversion)
}

ConfigValue Config::getConvertedValue(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->value;
    }
    
    // Return empty string if key not found
    return std::string();
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

// Helper functions implementation - defined as standalone functions for simplicity
namespace nlm {
    
    // Simple JSON parsing helper
    bool parseSimpleJSON(const std::string& content, std::map<std::string, std::string>& parsed) {
        size_t braceStart = content.find('{');
        size_t braceEnd = content.rfind('}');
        
        if (braceStart == std::string::npos || braceEnd == std::string::npos) {
            return false;
        }
        
        std::string jsonContent = content.substr(braceStart + 1, braceEnd - braceStart - 1);
        size_t pos = 0;
        
        while (pos < jsonContent.length()) {
            size_t keyEnd = jsonContent.find(':', pos);
            if (keyEnd == std::string::npos) break;
            
            std::string key = Config::trim(jsonContent.substr(pos, keyEnd - pos));
            if (key.size() >= 2 && ((key.front() == '"' && key.back() == '"') ||
                                   (key.front() == '\'' && key.back() == '\''))) {
                key = key.substr(1, key.size() - 2);
            }
            
            size_t valueStart = jsonContent.find_first_not_of(' ', keyEnd + 1);
            if (valueStart == std::string::npos) break;
            
            size_t valueEnd = jsonContent.find(',', valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = jsonContent.length();
            }
            
            std::string value = Config::trim(jsonContent.substr(valueStart, valueEnd - valueStart));
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                       (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            parsed[key] = value;
            
            pos = valueEnd + 1;
            while (pos < jsonContent.length() && jsonContent[pos] != ',') {
                pos++;
            }
            if (pos < jsonContent.length()) {
                pos++;
            }
        }
        
        return true;
    }
    
    // Value type conversion helper
    void convertValueType(const std::string& key, const std::string& valueStr, Config& config) {
        if (valueStr == "true" || valueStr == "false") {
            bool boolVal = (valueStr == "true");
            config.set(key, boolVal, ConfigSource::File);
            return;
        }
        
        // Try integer conversion
        try {
            size_t pos;
            int intVal = std::stoi(valueStr, &pos);
            if (pos == valueStr.length()) {
                config.set(key, intVal, ConfigSource::File);
                return;
            }
        } catch (...) {}
        
        // Try double conversion
        try {
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.length()) {
                config.set(key, doubleVal, ConfigSource::File);
                return;
            }
        } catch (...) {}
        
        // Try int64 conversion
        try {
            size_t pos;
            int64_t int64Val = std::stoll(valueStr, &pos);
            if (pos == valueStr.length()) {
                config.set(key, int64Val, ConfigSource::File);
                return;
            }
        } catch (...) {}
        
        // If all conversions fail, store as string
        config.set(key, valueStr, ConfigSource::File);
    }
    
} // namespace nlm
