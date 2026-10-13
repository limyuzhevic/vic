#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string filepath;
    bool jsonFormat;
    
    Impl() : jsonFormat(false) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

namespace {
    // Helper functions for parsing and conversion
    ConfigValue stringToConfigValue(const std::string& str) {
        if (str.empty()) {
            return std::string("");
        }
        
        // Try bool first
        if (str == "true" || str == "false" || str == "True" || str == "False") {
            bool val = (str == "true" || str == "True");
            return val;
        }
        
        // Try integer
        try {
            size_t pos;
            long val = std::stol(str, &pos);
            if (pos == str.size()) {
                return static_cast<int>(val);
            }
            // Check if it's actually int64_t
            char* endptr;
            long long llval = std::strtoll(str.c_str(), &endptr, 10);
            if (endptr == str.c_str() + str.size()) {
                return llval;
            }
        } catch (...) {
            // Not an integer
        }
        
        // Try double
        try {
            size_t pos;
            double val = std::stod(str, &pos);
            if (pos == str.size()) {
                return val;
            }
        } catch (...) {
            // Not a double
        }
        
        // Default to string
        return str;
    }
    
    std::string configValueToString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << val[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << val[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << val[i] << "\"";
                }
                oss << "]";
            }
        }, value);
        return oss.str();
    }
    
    // Helper for JSON parsing (simplified version)
    bool parseJsonValue(const std::string& jsonStr, ConfigValue& value) {
        // Very simplified JSON parser for basic types
        // In a real implementation, use a proper JSON library like nlohmann/json
        
        // Skip whitespace
        size_t start = 0;
        while (start < jsonStr.size() && std::isspace(jsonStr[start])) start++;
        
        if (start >= jsonStr.size()) return false;
        
        char first = jsonStr[start];
        
        if (first == '"') {
            // String value
            size_t end = jsonStr.find('"', start + 1);
            if (end == std::string::npos) return false;
            std::string str = jsonStr.substr(start + 1, end - start - 1);
            value = str;
            return true;
        } else if (first == 't' || first == 'f') {
            // Boolean value
            if (jsonStr.substr(start, 4) == "true") {
                value = true;
                return true;
            } else if (jsonStr.substr(start, 5) == "false") {
                value = false;
                return true;
            }
        } else if (first == '-' || (first >= '0' && first <= '9')) {
            // Number value
            size_t end = jsonStr.find_first_not_of("0123456789.+-eE", start);
            if (end == start) return false;
            std::string numStr = jsonStr.substr(start, end - start);
            
            // Try to parse as integer first
            try {
                if (numStr.find('.') == std::string::npos && numStr.find('e') == std::string::npos && 
                    numStr.find('E') == std::string::npos) {
                    long long intVal = std::stoll(numStr);
                    if (intVal >= std::numeric_limits<int>::min() && 
                        intVal <= std::numeric_limits<int>::max()) {
                        value = static_cast<int>(intVal);
                    } else {
                        value = intVal;
                    }
                    return true;
                } else {
                    double doubleVal = std::stod(numStr);
                    value = doubleVal;
                    return true;
                }
            } catch (...) {
                return false;
            }
        }
        
        return false;
    }
    
    // Helper for YAML parsing (simplified version)
    bool parseYamlValue(const std::string& yamlStr, ConfigValue& value) {
        // Very simplified YAML parser for basic types
        // In a real implementation, use a proper YAML library like yaml-cpp
        
        // Skip whitespace and comments
        size_t start = 0;
        while (start < yamlStr.size() && (std::isspace(yamlStr[start]) || yamlStr[start] == '#')) {
            if (yamlStr[start] == '#') {
                // Comment line
                size_t nl = yamlStr.find('\n', start);
                if (nl == std::string::npos) return false;
                start = nl + 1;
                continue;
            }
            start++;
        }
        
        if (start >= yamlStr.size()) return false;
        
        size_t end = yamlStr.size();
        while (end > start && std::isspace(yamlStr[end - 1])) end--;
        
        if (start >= end) return false;
        
        std::string str = yamlStr.substr(start, end - start);
        value = stringToConfigValue(str);
        return true;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    pImpl->filepath = filepath;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read first line to determine format
    std::string firstLine;
    if (std::getline(file, firstLine)) {
        // Reset file pointer
        file.clear();
        file.seekg(0);
        
        // Check if it's JSON (starts with { or [) or YAML (likely has colon after key)
        firstLine = trim(firstLine);
        if (firstLine.empty()) {
            pImpl->jsonFormat = false;
        } else if ((firstLine[0] == '{' || firstLine[0] == '[') && firstLine.back() == '}' && firstLine.back() == ']') {
            pImpl->jsonFormat = true;
        } else if (firstLine.find(':') != std::string::npos) {
            pImpl->jsonFormat = false;
        } else {
            pImpl->jsonFormat = false;
        }
    }
    
    file.clear();
    file.seekg(0);
    
    if (pImpl->jsonFormat) {
        // Parse as JSON
        return loadFromJsonFile(filepath);
    } else {
        // Parse as simple key=value format (original implementation)
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
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

bool Config::loadFromJsonFile(const std::string& filepath) {
    // Simplified JSON parsing
    // In a real implementation, use nlohmann/json
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, fall back to simple parsing
    // TODO: Implement proper JSON parsing with nlohmann/json
    file.close();
    return loadFromSimpleFile(filepath);
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
            } else {
                set(arg.substr(2), "true", ConfigSource::CommandLine);
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

bool Config::loadFromEnvironment(const std::unordered_map<std::string, std::string>& envVars) {
    bool loaded = false;
    for (const auto& pair : envVars) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        if (value.empty()) continue;
        
        set(key, value, ConfigSource::Environment);
        loaded = true;
    }
    return loaded;
}

bool Config::saveToFile(const std::string& filepath, bool format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format) {
        // Save as JSON format
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": " << valueToString(entry.value);
            if (i < pImpl->entries.size() - 1) {
                file << ",";
            }
            file << "\n";
        }
        file << "}\n";
    } else {
        // Save as simple key=value format (original format)
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << valueToString(entry.value) << "\n\n";
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
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

ConfigValue Config::getAny(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::string("");
    }
    
    return it->value;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::validate(const ConfigSchema& schema) const {
    try {
        schema.validateAll(*this);
        return true;
    } catch (const ConfigValidationError&) {
        return false;
    }
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

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigSource::Default;
    }
    
    return it->source;
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

ConfigValue Config::stringToValue(const std::string& str) {
    return stringToConfigValue(str);
}

std::string Config::valueToString(const ConfigValue& value) {
    return configValueToString(value);
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
