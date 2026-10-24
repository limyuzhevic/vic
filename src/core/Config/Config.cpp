#include <cctype>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <functional>
#include <sstream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON parser for configuration files
    // Supports both .json and .yaml/.yml files
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();
    
    // Try to parse as JSON first
    if (loadFromJSON(content)) {
        return true;
    }
    
    // Try to parse as YAML
    if (loadFromYAML(content)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
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
    
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
    }
    file << "\n}\n";
    
    return true;
}

template<typename T>
std::optional<T> Config::getValidated(const std::string& key, 
                                     const std::function<bool(const T&)>& validator) const {
    auto value = get<T>(key);
    if (!value.has_value()) {
        return std::nullopt;
    }
    
    if (validator && !validator(*value)) {
        return std::nullopt;
    }
    
    return value;
}

bool Config::hasAllRequired(const std::vector<std::string>& requiredKeys) const {
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            return false;
        }
    }
    return true;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Override existing values or add new ones
        set(entry.key, entry.value, source);
    }
}

std::vector<std::string> Config::getDiff(const Config& other) const {
    std::vector<std::string> diff;
    
    // Find keys in this config but not in other
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (entry.key == otherEntry.key) {
                found = true;
                // Check if values differ
                if (entry.value.index() != otherEntry.value.index()) {
                    diff.push_back("Key '" + entry.key + "' differs in type");
                } else {
                    if (!std::holds_alternative<decltype(entry.value)>(entry.value)) {
                        diff.push_back("Key '" + entry.key + "' differs in type");
                    }
                    // For same type, we could compare values more thoroughly
                }
                break;
            }
        }
        if (!found) {
            diff.push_back("Key '" + entry.key + "' missing in other config");
        }
    }
    
    // Find keys in other but not in this
    for (const auto& entry : other.pImpl->entries) {
        bool found = false;
        for (const auto& thisEntry : pImpl->entries) {
            if (entry.key == thisEntry.key) {
                found = true;
                break;
            }
        }
        if (!found) {
            diff.push_back("Key '" + entry.key + "' missing in this config");
        }
    }
    
    return diff;
}

bool Config::validate() const {
    // Basic validation: ensure all keys have valid values
    for (const auto& entry : pImpl->entries) {
        // Check if value can be properly accessed by its type
        bool valueAccessible = false;
        if (entry.value.index() == 0) { // int
            valueAccessible = true;
        } else if (entry.value.index() == 1) { // int64_t
            valueAccessible = true;
        } else if (entry.value.index() == 2) { // double
            valueAccessible = true;
        } else if (entry.value.index() == 3) { // bool
            valueAccessible = true;
        } else if (entry.value.index() == 4) { // string
            valueAccessible = true;
        } else if (entry.value.index() == 5) { // vector<int>
            valueAccessible = true;
        } else if (entry.value.index() == 6) { // vector<double>
            valueAccessible = true;
        } else if (entry.value.index() == 7) { // vector<string>
            valueAccessible = true;
        }
        
        if (!valueAccessible) {
            return false;
        }
    }
    return true;
}

bool Config::loadFromJSON(const std::string& jsonString) {
    // Simplified JSON parser for configuration files
    // This is a basic implementation - for production use, consider a proper JSON library
    
    try {
        size_t pos = 0;
        while (pos < jsonString.size()) {
            // Find next key (between { and })
            size_t keyStart = jsonString.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = jsonString.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonString.substr(keyStart + 1, keyEnd - keyStart - 1);
            key = trim(key);
            
            // Find value start (after ": ")
            size_t valueStart = jsonString.find(':', keyEnd);
            if (valueStart == std::string::npos) break;
            
            size_t valueEnd = jsonString.find_first_of(",}", valueStart + 1);
            if (valueEnd == std::string::npos) break;
            
            std::string valueStr = trim(jsonString.substr(valueStart + 1, valueEnd - valueStart - 1));
            
            // Parse value based on type
            ConfigValue value;
            if (parseJSONValue(valueStr, value)) {
                set(key, value, ConfigSource::File);
            }
            
            pos = valueEnd + 1;
            
            // Skip trailing comma
            if (pos < jsonString.size() && jsonString[pos] == ',') {
                pos++;
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlString) {
    // Simplified YAML parser for configuration files
    // This is a basic implementation - for production use, consider a proper YAML library
    
    try {
        std::istringstream stream(yamlString);
        std::string line;
        
        while (std::getline(stream, line)) {
            // Skip comments and empty lines
            line = trim(line);
            if (line.empty() || line.find('#') == 0) {
                continue;
            }
            
            // Parse YAML key: value format
            size_t colonPos = line.find(':');
            if (colonPos == std::string::npos) {
                continue;
            }
            
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove quotes from YAML values
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            ConfigValue configValue;
            if (parseYAMLValue(value, configValue)) {
                set(key, configValue, ConfigSource::File);
            }
        }
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string Config::saveToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            oss << ",\n";
        }
        first = false;
        
        oss << "  \"" << entry.key << "\": ";
        oss << configValueToJSON(entry.value);
    }
    oss << "\n}\n";
    return oss.str();
}

bool Config::parseJSONValue(const std::string& jsonStr, ConfigValue& value) {
    if (jsonStr.empty()) return false;
    
    if (jsonStr == "null") return false;
    
    if (jsonStr == "true" || jsonStr == "false") {
        value = (jsonStr == "true");
        return true;
    }
    
    if (jsonStr[0] == '"' && jsonStr.back() == '"') {
        value = jsonStr.substr(1, jsonStr.size() - 2);
        return true;
    }
    
    if (jsonStr.find('.') != std::string::npos || jsonStr.find('e') != std::string::npos) {
        try {
            double doubleVal = std::stod(jsonStr);
            value = doubleVal;
            return true;
        } catch (...) {
            return false;
        }
    }
    
    try {
        int intVal = std::stoi(jsonStr);
        value = intVal;
        return true;
    } catch (...) {
        try {
            long long intVal = std::stoll(jsonStr);
            value = intVal;
            return true;
        } catch (...) {
            return false;
        }
    }
}

bool Config::parseYAMLValue(const std::string& yamlStr, ConfigValue& value) {
    if (yamlStr.empty()) return false;
    
    if (yamlStr == "null") return false;
    
    if (yamlStr == "true" || yamlStr == "false") {
        value = (yamlStr == "true");
        return true;
    }
    
    if (yamlStr[0] == '"' && yamlStr.back() == '"') {
        value = yamlStr.substr(1, yamlStr.size() - 2);
        return true;
    }
    
    if (yamlStr.find('.') != std::string::npos || yamlStr.find('e') != std::string::npos) {
        try {
            double doubleVal = std::stod(yamlStr);
            value = doubleVal;
            return true;
        } catch (...) {
            return false;
        }
    }
    
    try {
        int intVal = std::stoi(yamlStr);
        value = intVal;
        return true;
    } catch (...) {
        try {
            long long intVal = std::stoll(yamlStr);
            value = intVal;
            return true;
        } catch (...) {
            return false;
        }
    }
}

std::string Config::configValueToJSON(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << arg[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
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
