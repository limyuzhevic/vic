#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// JSON/YAML parsing helpers
using json = nlohmann::json;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigMetadata> metadata;
    std::vector<std::string> requiredKeys;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

// Configuration templates implementation
bool Config::loadFromTemplate(Config& config, const std::string& templateName) {
    const auto& templates = ConfigTemplates::DEFAULT;
    
    if (templateName == "development") {
        config.clear();
        for (const auto& pair : ConfigTemplates::DEVELOPMENT) {
            config.set(pair.first, parseValue(pair.second, "string").value_or(ConfigValue("")));
        }
        return true;
    } else if (templateName == "production") {
        config.clear();
        for (const auto& pair : ConfigTemplates::PRODUCTION) {
            config.set(pair.first, parseValue(pair.second, "string").value_or(ConfigValue("")));
        }
        return true;
    } else if (templateName == "minimal") {
        config.clear();
        for (const auto& pair : ConfigTemplates::MINIMAL) {
            config.set(pair.first, parseValue(pair.second, "string").value_or(ConfigValue("")));
        }
        return true;
    } else {
        // Default template
        config.clear();
        for (const auto& pair : templates) {
            config.set(pair.first, parseValue(pair.second, "string").value_or(ConfigValue("")));
        }
        return true;
    }
}

bool Config::applyDefaults(Config& config) {
    // Apply default values for all registered configuration keys
    for (const auto& pair : pImpl->metadata) {
        const std::string& key = pair.first;
        const ConfigMetadata& meta = pair.second;
        
        if (!config.has(key) && meta.required) {
            throw ConfigValidationError(key, "Required configuration key is missing");
        }
        
        if (!config.has(key) && !meta.defaultValue.empty()) {
            auto parsedValue = parseValue(meta.defaultValue, meta.type);
            if (parsedValue) {
                config.set(key, *parsedValue, ConfigSource::Default);
            }
        }
    }
    return true;
}

// Load from file (JSON/YAML format)
bool Config::loadFromFile(const std::string& filepath, bool strict) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigFileError("Cannot open file: " + filepath);
        }
        
        // Try JSON first
        if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
            json j;
            file >> j;
            
            for (auto it = j.begin(); it != j.end(); ++it) {
                const std::string& key = it.key();
                const auto& value = it.value();
                
                ConfigValue configValue;
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_array()) {
                    if (value.empty()) {
                        configValue = std::vector<int>();
                    } else if (std::all_of(value.begin(), value.end(), [](const auto& v) { return v.is_number_integer(); })) {
                        std::vector<int> intArray;
                        for (const auto& v : value) {
                            intArray.push_back(v.get<int>());
                        }
                        configValue = intArray;
                    } else if (std::all_of(value.begin(), value.end(), [](const auto& v) { return v.is_number_float(); })) {
                        std::vector<double> doubleArray;
                        for (const auto& v : value) {
                            doubleArray.push_back(v.get<double>());
                        }
                        configValue = doubleArray;
                    } else {
                        std::vector<std::string> stringArray;
                        for (const auto& v : value) {
                            stringArray.push_back(v.get<std::string>());
                        }
                        configValue = stringArray;
                    }
                } else {
                    throw ConfigValidationError(key, "Unsupported JSON value type");
                }
                
                set(key, configValue, ConfigSource::File);
            }
            
        } else {
            // Try YAML
            YAML::Node doc = YAML::LoadFile(filepath);
            
            for (auto it = doc.begin(); it != doc.end(); ++it) {
                const std::string& key = it->first.as<std::string>();
                const YAML::Node& value = it->second;
                
                ConfigValue configValue;
                if (value.IsScalar()) {
                    if (value.IsInteger()) {
                        configValue = value.as<int64_t>();
                    } else if (value.IsFloat()) {
                        configValue = value.as<double>();
                    } else if (value.IsBoolean()) {
                        configValue = value.as<bool>();
                    } else {
                        configValue = value.as<std::string>();
                    }
                } else if (value.IsSequence()) {
                    if (value.size() == 0) {
                        configValue = std::vector<int>();
                    } else if (std::all_of(value.begin(), value.end(), [](const auto& v) { return v.IsInteger(); })) {
                        std::vector<int> intArray;
                        for (const auto& v : value) {
                            intArray.push_back(v.as<int>());
                        }
                        configValue = intArray;
                    } else if (std::all_of(value.begin(), value.end(), [](const auto& v) { return v.IsFloat(); })) {
                        std::vector<double> doubleArray;
                        for (const auto& v : value) {
                            doubleArray.push_back(v.as<double>());
                        }
                        configValue = doubleArray;
                    } else {
                        std::vector<std::string> stringArray;
                        for (const auto& v : value) {
                            stringArray.push_back(v.as<std::string>());
                        }
                        configValue = stringArray;
                    }
                } else {
                    throw ConfigValidationError(key, "Unsupported YAML value type");
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        if (strict) {
            throw;
        }
        std::cerr << "Warning: Failed to load configuration from " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

// Load from command line arguments
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

// Save to file
bool Config::saveToFile(const std::string& filepath, bool pretty) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (pretty) {
        // Save in pretty JSON format
        json j;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&j, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    j[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    j[entry.key] = arg;
                }
            }, entry.value);
        }
        
        file << j.dump(2);
    } else {
        // Save in simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = [";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "]\n";
        }
    }
    
    return true;
}

// Get values with validation
template<typename T>
std::optional<T> Config::get(const std::string& key, bool validate) const {
    if (validate && !validate(key)) {
        return std::nullopt;
    }
    
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
T Config::getOr(const std::string& key, const T& defaultValue, bool validate) const {
    auto val = get<T>(key, validate);
    return val.has_value() ? val.value() : defaultValue;
}

// Set values with validation
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the value if metadata is available
    if (pImpl->metadata.find(key) != pImpl->metadata.end()) {
        const ConfigMetadata& meta = pImpl->metadata.at(key);
        if (meta.validator && !meta.validator(value)) {
            throw ConfigValidationError(key, "Value validation failed");
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

// Configuration validation
bool Config::validate(const std::string& key) const {
    if (pImpl->metadata.find(key) == pImpl->metadata.end()) {
        // No metadata defined for this key, assume valid
        return true;
    }
    
    const ConfigMetadata& meta = pImpl->metadata.at(key);
    
    if (meta.required && !has(key)) {
        return false;
    }
    
    if (has(key)) {
        auto value = get<ConfigValue>(key, false);
        if (value && meta.validator && !meta.validator(*value)) {
            return false;
        }
    }
    
    return true;
}

bool Config::validateAll() const {
    for (const auto& pair : pImpl->metadata) {
        if (!validate(pair.first)) {
            return false;
        }
    }
    return true;
}

// Configuration information
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

std::vector<std::string> Config::getKeysWithType(const std::string& type) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (pImpl->metadata.find(entry.key) != pImpl->metadata.end()) {
            if (pImpl->metadata.at(entry.key).type == type) {
                keys.push_back(entry.key);
            }
        }
    }
    return keys;
}

// Clear all
void Config::clear() {
    pImpl->entries.clear();
}

// Get configuration summary
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << pImpl->metadata.size() << " with metadata):\n";
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        
        if (pImpl->metadata.find(entry.key) != pImpl->metadata.end()) {
            oss << " *VALIDATED*";
        }
        oss << "\n";
    }
    
    // Check for invalid configurations
    bool valid = validateAll();
    oss << "\nValidation: " << (valid ? "PASSED" : "FAILED") << "\n";
    
    return oss.str();
}

std::string Config::getDocumentation() const {
    std::ostringstream oss;
    oss << "NLM Configuration Documentation\n";
    oss << "=================================\n\n";
    
    for (const auto& pair : pImpl->metadata) {
        const std::string& key = pair.first;
        const ConfigMetadata& meta = pair.second;
        
        oss << "Key: " << key << "\n";
        oss << "  Type: " << meta.type << "\n";
        oss << "  Description: " << meta.description << "\n";
        oss << "  Required: " << (meta.required ? "yes" : "no") << "\n";
        oss << "  Default: " << meta.defaultValue << "\n";
        oss << "  Current: ";
        if (has(key)) {
            auto value = get<ConfigValue>(key, false);
            if (value) {
                std::visit([&oss](auto&& arg) {
                    oss << arg;
                }, *value);
            } else {
                oss << "(invalid type)";
            }
        } else {
            oss << "(not set)";
        }
        oss << "\n\n";
    }
    
    return oss.str();
}

// Configuration metadata management
void Config::registerConfig(const std::string& key, const ConfigMetadata& metadata) {
    pImpl->metadata[key] = metadata;
    
    if (metadata.required) {
        pImpl->requiredKeys.push_back(key);
    }
}

ConfigMetadata Config::getMetadata(const std::string& key) const {
    if (pImpl->metadata.find(key) == pImpl->metadata.end()) {
        throw ConfigException("No metadata found for key: " + key);
    }
    return pImpl->metadata.at(key);
}

bool Config::hasMetadata(const std::string& key) const {
    return pImpl->metadata.find(key) != pImpl->metadata.end();
}

// Internal helpers
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

bool Config::isValidType(const ConfigValue& value, const std::string& type) {
    if (type == "int") {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (type == "float") {
        return std::holds_alternative<double>(value);
    } else if (type == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (type == "bool") {
        return std::holds_alternative<bool>(value);
    } else if (type == "list") {
        return std::holds_alternative<std::vector<int>>(value) ||
               std::holds_alternative<std::vector<double>>(value) ||
               std::holds_alternative<std::vector<std::string>>(value);
    }
    return false;
}

std::optional<ConfigValue> Config::parseValue(const std::string& str, const std::string& type) {
    if (type == "int") {
        try {
            return std::stoi(str);
        } catch (...) {
            return std::nullopt;
        }
    } else if (type == "float") {
        try {
            return std::stod(str);
        } catch (...) {
            return std::nullopt;
        }
    } else if (type == "bool") {
        if (str == "true" || str == "1" || str == "yes") {
            return true;
        } else if (str == "false" || str == "0" || str == "no") {
            return false;
        }
        return std::nullopt;
    } else if (type == "string") {
        return str;
    }
    return std::nullopt;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&, bool) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&, bool) const;
template std::optional<double> Config::get<double>(const std::string&, bool) const;
template std::optional<bool> Config::get<bool>(const std::string&, bool) const;
template std::optional<std::string> Config::get<std::string>(const std::string&, bool) const;

template int Config::getOr<int>(const std::string&, const int&, bool) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&, bool) const;
template double Config::getOr<double>(const std::string&, const double&, bool) const;
template bool Config::getOr<bool>(const std::string&, const bool&, bool) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&, bool) const;

} // namespace nlm
