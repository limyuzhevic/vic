#include "ConfigManager.hpp"
#include "../Exceptions/Exceptions.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <regex>

namespace nlm {

// ConfigSchema implementation - standalone class methods
void ConfigSchema::addField(const std::string& key, ConfigSource source, 
                            const std::string& description, bool required,
                            const std::string& validationPattern) {
    ConfigEntry field;
    field.key = key;
    field.source = source;
    field.description = description;
    field.isRequired = required;
    field.validationPattern = validationPattern;
    field.defaultValue = "";
    fields.push_back(field);
}

bool ConfigSchema::validateField(const ConfigEntry& entry) const {
    // Check if value exists for required fields
    if (entry.isRequired) {
        bool hasValue = false;
        if (std::holds_alternative<int>(entry.value) && std::get<int>(entry.value) != 0) hasValue = true;
        else if (std::holds_alternative<int64_t>(entry.value) && std::get<int64_t>(entry.value) != 0) hasValue = true;
        else if (std::holds_alternative<double>(entry.value) && std::get<double>(entry.value) != 0.0) hasValue = true;
        else if (std::holds_alternative<bool>(entry.value)) hasValue = true;
        else if (std::holds_alternative<std::string>(entry.value) && !std::get<std::string>(entry.value).empty()) hasValue = true;
        
        if (!hasValue) {
            return false;
        }
    }
    
    // Validate against pattern if specified
    if (!entry.validationPattern.empty()) {
        std::string valueStr;
        if (std::holds_alternative<std::string>(entry.value)) {
            valueStr = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<int>(entry.value)) {
            valueStr = std::to_string(std::get<int>(entry.value));
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            valueStr = std::to_string(std::get<int64_t>(entry.value));
        } else if (std::holds_alternative<double>(entry.value)) {
            valueStr = std::to_string(std::get<double>(entry.value));
        } else if (std::holds_alternative<bool>(entry.value)) {
            valueStr = std::get<bool>(entry.value) ? "true" : "false";
        }
        
        try {
            std::regex pattern(entry.validationPattern);
            return std::regex_match(valueStr, pattern);
        } catch (const std::regex_error&) {
            return true; // If pattern is invalid, skip validation
        }
    }
    
    return true;
}

// Implementation for ConfigManager
struct ConfigManager::Impl {
    std::vector<ConfigEntry> entries;
    ConfigSchema schema;
    mutable std::vector<std::string> validationErrors;
};

ConfigManager::ConfigManager() : pImpl(std::make_unique<Impl>()) {}

ConfigManager::~ConfigManager() = default;

ConfigManager::ConfigManager(ConfigManager&&) noexcept = default;

ConfigManager& ConfigManager::operator=(ConfigManager&&) noexcept = default;

bool ConfigManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool ConfigManager::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool ConfigManager::saveToFile(const std::string& filepath, bool pretty) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << " = " << valueToString(entry.value) << "\n";
    }
    
    return true;
}

void ConfigManager::set(const std::string& key, const ConfigValue& value, 
                       ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

template<typename T>
std::optional<T> ConfigManager::get(const std::string& key) const {
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
T ConfigManager::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
void ConfigManager::set(const std::string& key, const T& value, 
                       ConfigSource source) {
    if constexpr (std::is_same_v<T, std::string>) {
        set(key, ConfigValue(std::in_place_type<std::string>, value), source);
    } else if constexpr (std::is_same_v<T, int>) {
        set(key, ConfigValue(std::in_place_type<int>, value), source);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        set(key, ConfigValue(std::in_place_type<int64_t>, value), source);
    } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
        set(key, ConfigValue(std::in_place_type<double>, value), source);
    } else if constexpr (std::is_same_v<T, bool>) {
        set(key, ConfigValue(std::in_place_type<bool>, value), source);
    }
}

void ConfigManager::setString(const std::string& key, const std::string& value, 
                              ConfigSource source) {
    set(key, value, source);
}

void ConfigManager::setInt(const std::string& key, int value, 
                           ConfigSource source) {
    set(key, value, source);
}

void ConfigManager::setDouble(const std::string& key, double value, 
                              ConfigSource source) {
    set(key, value, source);
}

void ConfigManager::setBool(const std::string& key, bool value, 
                             ConfigSource source) {
    set(key, value, source);
}

// Schema management implementation
void ConfigManager::setSchema(const ConfigSchema& schema) {
    pImpl->schema = schema;
}

bool ConfigManager::validate() const {
    pImpl->validationErrors.clear();
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = validateField(entry);
        if (!result.isValid) {
            std::string error = generateValidationError(entry, result);
            pImpl->validationErrors.push_back(error);
        }
    }
    
    return pImpl->validationErrors.empty();
}

std::vector<std::string> ConfigManager::getValidationErrors() const {
    return pImpl->validationErrors;
}

// Enhanced file loading implementation
bool ConfigManager::loadFromKeyValue(const std::string& filepath) {
    // This is a simple key=value loader
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty()) {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // Remove quotes from value
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                     (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

// Information and utilities implementation
std::string ConfigManager::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (source=" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

ConfigSource ConfigManager::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    return ConfigSource::Default;
}

// Backward compatibility implementation
bool ConfigManager::loadLegacyKeyValue(const std::string& filepath) {
    return loadFromFile(filepath);
}

// Static methods implementation
std::string ConfigManager::getCLIHelp() {
    return "NLM Configuration Manager\n"
           "  --key=value    Set configuration value\n"
           "  -k value       Short form of --key=value\n"
           "  --help         Show this help message\n"
           "  --version      Show version information\n";
}

// Internal helper implementations
std::string ConfigManager::valueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

ValidationResult ConfigManager::validateField(const ConfigEntry& entry) const {
    ValidationResult result;
    
    // Check if value exists for required fields
    if (entry.isRequired) {
        bool hasValue = false;
        if (std::holds_alternative<int>(entry.value) && std::get<int>(entry.value) != 0) hasValue = true;
        else if (std::holds_alternative<int64_t>(entry.value) && std::get<int64_t>(entry.value) != 0) hasValue = true;
        else if (std::holds_alternative<double>(entry.value) && std::get<double>(entry.value) != 0.0) hasValue = true;
        else if (std::holds_alternative<bool>(entry.value)) hasValue = true;
        else if (std::holds_alternative<std::string>(entry.value) && !std::get<std::string>(entry.value).empty()) hasValue = true;
        
        if (!hasValue) {
            result.addError("Required field '" + entry.key + "' is missing or empty");
        }
    }
    
    // Validate against pattern if specified
    if (!entry.validationPattern.empty()) {
        std::string valueStr;
        if (std::holds_alternative<std::string>(entry.value)) {
            valueStr = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<int>(entry.value)) {
            valueStr = std::to_string(std::get<int>(entry.value));
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            valueStr = std::to_string(std::get<int64_t>(entry.value));
        } else if (std::holds_alternative<double>(entry.value)) {
            valueStr = std::to_string(std::get<double>(entry.value));
        } else if (std::holds_alternative<bool>(entry.value)) {
            valueStr = std::get<bool>(entry.value) ? "true" : "false";
        }
        
        try {
            std::regex pattern(entry.validationPattern);
            if (!std::regex_match(valueStr, pattern)) {
                result.addError("Field '" + entry.key + "' value '" + valueStr + 
                               "' does not match pattern '" + entry.validationPattern + "'");
            }
        } catch (const std::regex_error&) {
            // If pattern is invalid, skip validation
        }
    }
    
    return result;
}

std::string ConfigManager::generateValidationError(const ConfigEntry& entry, const ValidationResult& result) const {
    std::string error = "Validation error for field '" + entry.key + "': ";
    for (const auto& err : result.errors) {
        error += err + "; ";
    }
    if (!error.empty() && error.back() == ' ') error.pop_back();
    return error;
}

// Explicit template instantiations
template std::optional<int> ConfigManager::get<int>(const std::string&) const;
template std::optional<int64_t> ConfigManager::get<int64_t>(const std::string&) const;
template std::optional<double> ConfigManager::get<double>(const std::string&) const;
template std::optional<bool> ConfigManager::get<bool>(const std::string&) const;
template std::optional<std::string> ConfigManager::get<std::string>(const std::string&) const;

template int ConfigManager::getOr<int>(const std::string&, const int&) const;
template int64_t ConfigManager::getOr<int64_t>(const std::string&, const int64_t&) const;
template double ConfigManager::getOr<double>(const std::string&, const double&) const;
template bool ConfigManager::getOr<bool>(const std::string&, const bool&) const;
template std::string ConfigManager::getOr<std::string>(const std::string&, const std::string&) const;

template void ConfigManager::set<int>(const std::string&, const int&, ConfigSource);
template void ConfigManager::set<int64_t>(const std::string&, const int64_t&, ConfigSource);
template void ConfigManager::set<double>(const std::string&, const double&, ConfigSource);
template void ConfigManager::set<bool>(const std::string&, const bool&, ConfigSource);
template void ConfigManager::set<std::string>(const std::string&, const std::string&, ConfigSource);

} // namespace nlm

