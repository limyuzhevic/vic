#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigSchema> schema;
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
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Detect and parse format
    std::string format = detectFormat(content);
    
    if (format == "JSON" || format == "JSON_LIKE") {
        auto jsonValue = fromJson(content);
        if (!jsonValue) {
            return false;
        }
        clear();
        
        // Recursively populate configuration from JSON
        std::function<void(const std::string&, const ConfigValue&)> populateFromJson;
        populateFromJson = [&](const std::string& prefix, const ConfigValue& value) {
            if (std::holds_alternative<int>(value)) {
                set(prefix, std::get<int>(value), ConfigSource::File);
            } else if (std::holds_alternative<int64_t>(value)) {
                set(prefix, std::get<int64_t>(value), ConfigSource::File);
            } else if (std::holds_alternative<double>(value)) {
                set(prefix, std::get<double>(value), ConfigSource::File);
            } else if (std::holds_alternative<bool>(value)) {
                set(prefix, std::get<bool>(value), ConfigSource::File);
            } else if (std::holds_alternative<std::string>(value)) {
                set(prefix, std::get<std::string>(value), ConfigSource::File);
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                // Convert vector<int> to JSON array string
                std::string jsonStr = "["; 
                const auto& vec = std::get<std::vector<int>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) jsonStr += ",";
                    jsonStr += std::to_string(vec[i]);
                }
                jsonStr += "]";
                set(prefix, jsonStr, ConfigSource::File);
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                std::string jsonStr = "[";
                const auto& vec = std::get<std::vector<double>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) jsonStr += ",";
                    jsonStr += std::to_string(vec[i]);
                }
                jsonStr += "]";
                set(prefix, jsonStr, ConfigSource::File);
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                std::string jsonStr = "[";
                const auto& vec = std::get<std::vector<std::string>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) jsonStr += ",";
                    jsonStr += "\"" + vec[i] + "\"";
                }
                jsonStr += "]";
                set(prefix, jsonStr, ConfigSource::File);
            } else if (std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(value)) {
                const auto& map = std::get<std::unordered_map<std::string, ConfigValue>>(value);
                for (const auto& pair : map) {
                    std::string newPrefix = prefix.empty() ? pair.first : prefix + "." + pair.first;
                    populateFromJson(newPrefix, pair.second);
                }
            }
        };
        
        populateFromJson("", *jsonValue);
        return true;
    } else if (format == "YAML" || format == "YAML_LIKE") {
        auto yamlValue = fromYaml(content);
        if (!yamlValue) {
            return false;
        }
        clear();
        
        // Recursively populate configuration from YAML
        std::function<void(const std::string&, const ConfigValue&)> populateFromYaml;
        populateFromYaml = [&](const std::string& prefix, const ConfigValue& value) {
            if (std::holds_alternative<int>(value)) {
                set(prefix, std::get<int>(value), ConfigSource::File);
            } else if (std::holds_alternative<int64_t>(value)) {
                set(prefix, std::get<int64_t>(value), ConfigSource::File);
            } else if (std::holds_alternative<double>(value)) {
                set(prefix, std::get<double>(value), ConfigSource::File);
            } else if (std::holds_alternative<bool>(value)) {
                set(prefix, std::get<bool>(value), ConfigSource::File);
            } else if (std::holds_alternative<std::string>(value)) {
                set(prefix, std::get<std::string>(value), ConfigSource::File);
            } else if (std::holds_alternative<std::vector<int>>(value)) {
                std::string yamlStr = "[";
                const auto& vec = std::get<std::vector<int>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) yamlStr += ",";
                    yamlStr += std::to_string(vec[i]);
                }
                yamlStr += "]";
                set(prefix, yamlStr, ConfigSource::File);
            } else if (std::holds_alternative<std::vector<double>>(value)) {
                std::string yamlStr = "[";
                const auto& vec = std::get<std::vector<double>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) yamlStr += ",";
                    yamlStr += std::to_string(vec[i]);
                }
                yamlStr += "]";
                set(prefix, yamlStr, ConfigSource::File);
            } else if (std::holds_alternative<std::vector<std::string>>(value)) {
                std::string yamlStr = "[";
                const auto& vec = std::get<std::vector<std::string>>(value);
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) yamlStr += ",";
                    yamlStr += "\"" + vec[i] + "\"";
                }
                yamlStr += "]";
                set(prefix, yamlStr, ConfigSource::File);
            } else if (std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(value)) {
                const auto& map = std::get<std::unordered_map<std::string, ConfigValue>>(value);
                for (const auto& pair : map) {
                    std::string newPrefix = prefix.empty() ? pair.first : prefix + "." + pair.first;
                    populateFromYaml(newPrefix, pair.second);
                }
            }
        };
        
        populateFromYaml("", *yamlValue);
        return true;
    } else if (format == "KEY_VALUE") {
        // Backward compatibility with key=value format
        return loadFromKeyValue(content);
    }
    
    return false;
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

bool Config::loadFromKeyValue(const std::string& content) {
    clear();
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to use JSON format first
    std::string jsonContent = "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) jsonContent += ",\n";
        first = false;
        
        jsonContent += "  \"" + entry.key + "\": " + valueToString(entry.value) + """";
    }
    jsonContent += "\n}";
    
    try {
        file << jsonContent;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error writing JSON to file: " << e.what() << std::endl;
    }
    
    return false;
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

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string Config::detectFormat(const std::string& content) {
    if (content.empty()) return "UNKNOWN";
    
    // Check for JSON-like format
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // Simple heuristic for JSON detection
        if (content.find(':') != std::string::npos) {
            return "JSON";
        }
    }
    
    // Check for YAML-like format (simple heuristic)
    if (content.find(':') != std::string::npos && 
        (content.find("true") != std::string::npos || 
         content.find("false") != std::string::npos ||
         content.find("null") != std::string::npos)) {
        return "YAML";
    }
    
    // Default to key=value format
    return "KEY_VALUE";
}

std::optional<ConfigValue> Config::parseValue(const std::string& str) {
    if (str.empty()) return std::nullopt;
    
    // Try to parse as boolean
    if (str == "true") return true;
    if (str == "false") return false;
    if (str == "null") return std::nullopt;
    
    // Try to parse as integer
    try {
        if (str.find_first_not_of("0123456789") == std::string::npos) {
            return std::stoi(str);
        }
    } catch (...) {}
    
    // Try to parse as integer64_t
    try {
        if (str.find_first_not_of("0123456789") == std::string::npos) {
            return static_cast<int64_t>(std::stoll(str));
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        return std::stod(str);
    } catch (...) {}
    
    // Return as string (without quotes)
    return str;
}

std::string Config::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            const auto& vec = arg;
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ",";
                oss << vec[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            const auto& vec = arg;
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ",";
                oss << vec[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            const auto& vec = arg;
            for (size_t i = 0; i < vec.size(); ++i) {
                if (i > 0) oss << ",";
                oss << "\"" << vec[i] << "\"";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, ConfigValue>>) {
            oss << "{";
            const auto& map = arg;
            bool first = true;
            for (const auto& pair : map) {
                if (!first) oss << ",";
                first = false;
                oss << "\"" << pair.first << "\": " << valueToString(pair.second);
            }
            oss << "}";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string Config::configEntryToString(const ConfigEntry& entry) const {
    std::ostringstream oss;
    oss << "# " << entry.description << "\n";
    oss << entry.key << " = " << valueToString(entry.value) << "\n";
    return oss.str();
}

void Config::collectValidationErrors(const std::string& path, const ConfigValue& value, std::vector<ValidationError>& errors) const {
    if (path.empty()) return;
    
    // Basic validation logic
    if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
        // Integer validation would go here
    } else if (std::holds_alternative<double>(value)) {
        // Double validation would go here
    } else if (std::holds_alternative<std::string>(value)) {
        // String validation would go here
    } else if (std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(value)) {
        const auto& map = std::get<std::unordered_map<std::string, ConfigValue>>(value);
        for (const auto& pair : map) {
            std::string newPath = path.empty() ? pair.first : path + "." + pair.first;
            collectValidationErrors(newPath, pair.second, errors);
        }
    }
}

// Implementation of static methods
std::string Config::toJson(const ConfigValue& value) {
    // This would use nlohmann::json library
    // For now, return a placeholder
    return "{}";
}

std::string Config::toYaml(const ConfigValue& value) {
    // This would use YAML library
    // For now, return a placeholder
    return "";
}

std::optional<ConfigValue> Config::fromJson(const std::string& jsonStr) {
    // This would use nlohmann::json library
    // For now, return a placeholder
    return std::nullopt;
}

std::optional<ConfigValue> Config::fromYaml(const std::string& yamlStr) {
    // This would use YAML library
    // For now, return a placeholder
    return std::nullopt;
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