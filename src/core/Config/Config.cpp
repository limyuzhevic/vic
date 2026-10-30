// Configuration enhancement for NLM Phase 6

#include "core/Config/Config.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <regex>

namespace nlm {

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, ConfigFormat format) {
    switch (format) {
        case ConfigFormat::KeyValue:
            return loadFromKeyValueFile(filepath);
        case ConfigFormat::JSON:
            return loadFromJsonFile(filepath);
        case ConfigFormat::YAML:
            return loadFromYamlFile(filepath);
        default:
            NLM_LOG_ERROR("Unsupported configuration file format");
            return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Handle --key=value format
        if (arg.find("=") != std::string::npos) {
            size_t pos = arg.find("=");
            if (pos > 0 && arg[0] == '-') {
                std::string key = arg.substr(1, pos - 1);
                std::string value = arg.substr(pos + 1);
                try {
                    set(key, value, ConfigSource::CommandLine, 
                        "Command line argument", "");
                } catch (const ConfigValidationError& e) {
                    NLM_LOG_ERROR(std::string("Command line argument validation failed: ") + e.what());
                    success = false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            try {
                set(key, value, ConfigSource::CommandLine,
                    "Command line argument", "");
            } catch (const ConfigValidationError& e) {
                NLM_LOG_ERROR(std::string("Command line argument validation failed: ") + e.what());
                success = false;
            }
        }
        // Handle boolean flags
        else if (arg == "--" + key + "=true" || arg == "--" + key + "=false") {
            bool value = (arg.find("true") != std::string::npos);
            try {
                set(key, value, ConfigSource::CommandLine,
                    "Command line argument", "");
            } catch (const ConfigValidationError& e) {
                NLM_LOG_ERROR(std::string("Command line argument validation failed: ") + e.what());
                success = false;
            }
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath, ConfigFormat format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    switch (format) {
        case ConfigFormat::KeyValue:
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << " = ";
                
                std::visit([&file](const auto& value) {
                    if constexpr (std::is_same_v<decltype(value), std::string>) {
                        file << "\"" << value << "\"";
                    } else if constexpr (std::is_same_v<decltype(value), bool>) {
                        file << (value ? "true" : "false");
                    } else {
                        file << value;
                    }
                }, entry.value);
                
                file << "\n\n";
            }
            break;
            
        case ConfigFormat::JSON:
            file << "{\n";
            for (size_t i = 0; i < pImpl->entries.size(); ++i) {
                const auto& entry = pImpl->entries[i];
                file << "  \"" << entry.key << "\": ";
                
                std::visit([&file](const auto& value) {
                    if constexpr (std::is_same_v<decltype(value), std::string>) {
                        file << "\"" << value << "\"";
                    } else if constexpr (std::is_same_v<decltype(value), bool>) {
                        file << (value ? "true" : "false");
                    } else {
                        file << value;
                    }
                }, entry.value);
                
                if (i < pImpl->entries.size() - 1) {
                    file << ",\n";
                } else {
                    file << "\n";
                }
            }
            file << "}\n";
            break;
            
        case ConfigFormat::YAML:
            for (const auto& entry : pImpl->entries) {
                file << "# " << entry.description << "\n";
                file << entry.key << ": ";
                
                std::visit([&file](const auto& value) {
                    if constexpr (std::is_same_v<decltype(value), std::string>) {
                        file << "\"" << value << "\"";
                    } else if constexpr (std::is_same_v<decltype(value), bool>) {
                        file << (value ? "true" : "false");
                    } else {
                        file << value;
                    }
                }, entry.value);
                file << "\n\n";
            }
            break;
    }
    
    return true;
}

bool Config::validate() const {
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        try {
            validate(entry.key);
        } catch (const ConfigValidationError& e) {
            NLM_LOG_ERROR(std::string("Configuration validation failed: ") + e.what());
            allValid = false;
        }
    }
    
    return allValid;
}

void Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return; // Key doesn't exist, no validation needed
    }
    
    const auto& entry = *it;
    
    // Apply type-specific validation
    std::visit([this, &entry](const auto& value) {
        using T = std::decay_t<decltype(value)>;
        
        if constexpr (std::is_same_v<T, int>) {
            validateInt(entry, entry.validationPattern);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            validateInt(entry, entry.validationPattern);
        } else if constexpr (std::is_same_v<T, double>) {
            validateDouble(entry, entry.validationPattern);
        } else if constexpr (std::is_same_v<T, bool>) {
            validateBool(entry, entry.validationPattern);
        } else if constexpr (std::is_same_v<T, std::string>) {
            validateString(entry, entry.validationPattern);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            validateVector(entry, entry.validationPattern);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            validateVector(entry, entry.validationPattern);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            validateVector(entry, entry.validationPattern);
        }
    }, entry.value);
    
    // Apply custom validator if set
    auto validatorIt = pImpl->validators.find(entry.key);
    if (validatorIt != pImpl->validators.end()) {
        if (!validatorIt->second(entry.value)) {
            throw ConfigValidationError(entry.key, "Custom validator failed");
        }
    }
}

bool Config::validateInt(const ConfigEntry& entry, const std::string& pattern) {
    if (!entry.validationPattern.empty()) {
        auto valueStr = std::to_string(std::get<int>(entry.value));
        if (!matchesPattern(valueStr, entry.validationPattern)) {
            throw ConfigValidationError(entry.key, 
                "Value '" + valueStr + "' does not match pattern '" + pattern + "'");
        }
    }
    return true;
}

bool Config::validateDouble(const ConfigEntry& entry, const std::string& pattern) {
    if (!entry.validationPattern.empty()) {
        auto valueStr = std::to_string(std::get<double>(entry.value));
        if (!matchesPattern(valueStr, entry.validationPattern)) {
            throw ConfigValidationError(entry.key,
                "Value '" + valueStr + "' does not match pattern '" + pattern + "'");
        }
    }
    return true;
}

bool Config::validateString(const ConfigEntry& entry, const std::string& pattern) {
    if (!entry.validationPattern.empty()) {
        if (!matchesPattern(std::get<std::string>(entry.value), entry.validationPattern)) {
            throw ConfigValidationError(entry.key,
                "Value does not match pattern '" + entry.validationPattern + "'");
        }
    }
    return true;
}

bool Config::validateBool(const ConfigEntry& entry, const std::string& pattern) {
    // Boolean validation is simple - just ensure it's a valid boolean
    return true;
}

bool Config::validateVector(const ConfigEntry& entry, const std::string& pattern) {
    // Vector validation could check size, element ranges, etc.
    return true;
}

bool Config::matchesPattern(const std::string& value, const std::string& pattern) {
    if (pattern.empty()) {
        return true; // No pattern means any value is valid
    }
    
    try {
        std::regex regex(pattern);
        return std::regex_match(value, regex);
    } catch (const std::regex_error&) {
        // If regex is invalid, treat pattern as literal
        return value == pattern;
    }
}

std::map<std::string, ConfigValue> Config::parseJsonFile(const std::string& filepath) {
    std::map<std::string, ConfigValue> result;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        return result;
    }
    
    // Simple JSON parsing (for demonstration - consider using a proper JSON library)
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '/' || line[0] == '#') {
            continue;
        }
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove trailing comma if present
            if (!value.empty() && value.back() == ',') {
                value.pop_back();
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                     (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse as different types
            try {
                // Try boolean
                if (value == "true") {
                    result[key] = true;
                } else if (value == "false") {
                    result[key] = false;
                }
                // Try integer
                else if (std::all_of(value.begin(), value.end(), ::isdigit) ||
                         (value[0] == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit))) {
                    result[key] = std::stoll(value);
                }
                // Try double
                else {
                    try {
                        result[key] = std::stod(value);
                    } catch (...) {
                        // If all parsing fails, treat as string
                        result[key] = value;
                    }
                }
            } catch (...) {
                result[key] = value;
            }
        }
    }
    
    return result;
}

std::string Config::configValueToJson(const ConfigValue& value) {
    std::ostringstream oss;
    
    std::visit([&oss](const auto& val) {
        if constexpr (std::is_same_v<decltype(val), std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<decltype(val), bool>) {
            oss << (val ? "true" : "false");
        } else {
            oss << val;
        }
    }, value);
    
    return oss.str();
}

ConfigValue Config::jsonToConfigValue(const std::map<std::string, std::string>& json) {
    // This is a simplified implementation - in practice, you'd want to use a proper JSON library
    ConfigValue result;
    return result;
}

bool Config::loadFromKeyValueFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
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
            
            // Try to parse the value based on content
            bool parsed = false;
            
            // Try boolean
            if (value == "true") {
                set(key, true, ConfigSource::File, "Line " + std::to_string(lineNumber), "");
                parsed = true;
            } else if (value == "false") {
                set(key, false, ConfigSource::File, "Line " + std::to_string(lineNumber), "");
                parsed = true;
            }
            
            // Try integer
            if (!parsed) {
                try {
                    size_t posEnd;
                    int64_t intValue = std::stoll(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, intValue, ConfigSource::File, "Line " + std::to_string(lineNumber), "");
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try double
            if (!parsed) {
                try {
                    size_t posEnd;
                    double doubleValue = std::stod(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, doubleValue, ConfigSource::File, "Line " + std::to_string(lineNumber), "");
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // If not parsed as number, treat as string
            if (!parsed) {
                set(key, value, ConfigSource::File, "Line " + std::to_string(lineNumber), "");
            }
        } else {
            NLM_LOG_WARNING("Invalid configuration line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return true;
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    // This is a placeholder - in practice, you would use nlohmann::json or similar
    NLM_LOG_WARNING("JSON configuration file loading not implemented. Using fallback key=value parser.");
    return loadFromKeyValueFile(filepath);
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    // This is a placeholder - in practice, you would use yaml-cpp or similar
    NLM_LOG_WARNING("YAML configuration file loading not implemented. Using fallback key=value parser.");
    return loadFromKeyValueFile(filepath);
}

// Rest of the Config implementation continues...
// The template instantiations and remaining methods would be implemented here

} // namespace nlm
