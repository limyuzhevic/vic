#include "EnhancedConfig.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <system_error>
#include <regex>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <any>
#include <sstream>
#include <iomanip>
#include <ctime>

using json = nlohmann::json;

namespace nlm {

void ConfigSchema::addEntry(const ConfigSchemaEntry& entry) {
    entries_[entry.key] = entry;
}

const ConfigSchemaEntry* ConfigSchema::getEntry(const std::string& key) const {
    auto it = entries_.find(key);
    if (it != entries_.end()) {
        return &it->second;
    }
    return nullptr;
}

bool ConfigSchema::hasEntry(const std::string& key) const {
    return entries_.find(key) != entries_.end();
}

std::vector<std::string> ConfigSchema::getRequiredKeys() const {
    std::vector<std::string> required;
    for (const auto& pair : entries_) {
        if (pair.second.required) {
            required.push_back(pair.first);
        }
    }
    return required;
}

std::vector<std::string> ConfigSchema::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(entries_.size());
    for (const auto& pair : entries_) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<SchemaValidationError> ConfigSchema::validate(const std::string& key, 
                                                         const std::string& value,
                                                         const std::map<std::string, std::string>& config) const {
    std::vector<SchemaValidationError> errors;
    
    const ConfigSchemaEntry* entry = getEntry(key);
    if (!entry) {
        errors.emplace_back(key, "Key not defined in schema", ValidationRule::Custom);
        return errors;
    }
    
    // Check required field
    if (entry->required && value.empty()) {
        errors.emplace_back(key, "Required field cannot be empty", ValidationRule::Required);
    }
    
    // Type validation
    if (!value.empty()) {
        bool isValidType = false;
        switch (entry->type) {
            case ConfigDataType::Integer:
                isValidType = std::regex("^-?\d+$").match(value);
                break;
            case ConfigDataType::Float:
                isValidType = std::regex("^-?\d*\.?\d+$|^-?\d+\.?\d*[eE][+-]?\d+$").match(value);
                break;
            case ConfigDataType::Boolean:
                isValidType = value == "true" || value == "false" || value == "1" || value == "0";
                break;
            case ConfigDataType::String:
                isValidType = true; // Any string is valid
                break;
            case ConfigDataType::VectorInt:
                isValidType = std::regex("^\s*\[(?:\s*-?\d+\s*,\s*)*\s*-?\d+\s*\]$").match(value);
                break;
            case ConfigDataType::VectorFloat:
                isValidType = std::regex("^\s*\[(?:\s*-?\d*\.?\d+\s*,\s*)*\s*-?\d*\.?\d+\s*\]$").match(value);
                break;
            case ConfigDataType::VectorString:
                isValidType = std::regex("^\s*\"[^\"]*\"\s*(?:,\s*\"[^\"]*\"\s*)*\]$").match(value);
                break;
            case ConfigDataType::StringList:
                isValidType = std::regex("^\s*\[(?:\s*\"[^\"]*\"\s*,\s*)*\s*\"[^\"]*\"\s*\]$").match(value);
                break;
            case ConfigDataType::Object:
                isValidType = std::regex("^\s*{\s*[a-zA-Z_][a-zA-Z0-9_]*\s*:\s*.*\s*}\s*$").match(value);
                break;
        }
        
        if (!isValidType && entry->required) {
            errors.emplace_back(key, "Value does not match required type: " + 
                                 std::to_string(static_cast<int>(entry->type)), 
                                 ValidationRule::Custom, "Type " + std::to_string(static_cast<int>(entry->type)));
        }
    }
    
    // Pattern validation
    auto patternIt = entry->validationRules.find(ValidationRule::Pattern);
    if (patternIt != entry->validationRules.end() && !value.empty()) {
        if (!std::regex(patternIt->second).match(value)) {
            errors.emplace_back(key, "Value does not match pattern: " + patternIt->second, 
                                 ValidationRule::Pattern, patternIt->second);
        }
    }
    
    // Enum validation
    auto enumIt = entry->validationRules.find(ValidationRule::Enum);
    if (enumIt != entry->validationRules.end() && !value.empty()) {
        std::string enumStr = enumIt->second;
        std::string delimiter = ";";
        size_t pos = 0;
        std::string token;
        bool found = false;
        while ((pos = enumStr.find(delimiter)) != std::string::npos) {
            token = enumStr.substr(0, pos);
            if (token == value) {
                found = true;
                break;
            }
            enumStr.erase(0, pos + delimiter.length());
        }
        if (!found && !enumStr.empty()) {
            token = enumStr;
            found = (token == value);
        }
        if (!found) {
            errors.emplace_back(key, "Value must be one of: " + enumIt->second, 
                                 ValidationRule::Enum, enumIt->second);
        }
    }
    
    // Range validation
    auto rangeIt = entry->validationRules.find(ValidationRule::Range);
    if (rangeIt != entry->validationRules.end() && !value.empty()) {
        std::string rangeStr = rangeIt->second;
        // Parse range (e.g., "min-max" or "min,max")
        size_t dashPos = rangeStr.find('-');
        size_t commaPos = rangeStr.find(',');
        
        double minVal = std::numeric_limits<double>::lowest();
        double maxVal = std::numeric_limits<double>::max();
        
        if (dashPos != std::string::npos) {
            minVal = std::stod(rangeStr.substr(0, dashPos));
            maxVal = std::stod(rangeStr.substr(dashPos + 1));
        } else if (commaPos != std::string::npos) {
            minVal = std::stod(rangeStr.substr(0, commaPos));
            maxVal = std::stod(rangeStr.substr(commaPos + 1));
        }
        
        try {
            double val = std::stod(value);
            if (val < minVal || val > maxVal) {
                errors.emplace_back(key, "Value " + value + " is out of range [" + 
                                     std::to_string(minVal) + ", " + std::to_string(maxVal) + "]", 
                                     ValidationRule::Range, "[" + std::to_string(minVal) + ", " + std::to_string(maxVal) + "]");
            }
        } catch (...) {
            errors.emplace_back(key, "Cannot parse value as number for range validation", 
                                 ValidationRule::Range);
        }
    }
    
    // Cross-key validation
    if (!entry->dependsOn.empty()) {
        for (const auto& depKey : entry->dependsOn) {
            auto it = config.find(depKey);
            if (it == config.end() || it->second.empty()) {
                errors.emplace_back(key, "Depends on missing or empty key: " + depKey, 
                                     ValidationRule::CrossKey, "Key " + depKey + " must be set");
            }
        }
    }
    
    // Mutual exclusivity validation
    if (!entry->mutuallyExclusive.empty()) {
        int count = 0;
        std::string foundKey;
        for (const auto& exclKey : entry->mutuallyExclusive) {
            auto it = config.find(exclKey);
            if (it != config.end() && !it->second.empty()) {
                count++;
                foundKey = exclKey;
            }
        }
        if (count > 1) {
            errors.emplace_back(key, "Cannot have multiple mutually exclusive keys: " + foundKey, 
                                 ValidationRule::Exclusive);
        }
    }
    
    return errors;
}

std::vector<SchemaValidationError> ConfigSchema::validateAll(const std::map<std::string, std::string>& config) const {
    std::vector<SchemaValidationError> allErrors;
    
    for (const auto& pair : entries_) {
        const std::string& key = pair.first;
        const ConfigSchemaEntry& entry = pair.second;
        
        auto it = config.find(key);
        std::string value = (it != config.end()) ? it->second : "";
        
        auto errors = validate(key, value, config);
        allErrors.insert(allErrors.end(), errors.begin(), errors.end());
    }
    
    return allErrors;
}

ConfigDataType ConfigSchema::getType(const std::string& key) const {
    const ConfigSchemaEntry* entry = getEntry(key);
    return entry ? entry->type : ConfigDataType::String;
}

bool ConfigSchema::isRequired(const std::string& key) const {
    const ConfigSchemaEntry* entry = getEntry(key);
    return entry ? entry->required : false;
}

std::string ConfigSchema::getDefault(const std::string& key) const {
    const ConfigSchemaEntry* entry = getEntry(key);
    return entry ? entry->defaultValue : "";
}

std::string ConfigSchema::getDescription(const std::string& key) const {
    const ConfigSchemaEntry* entry = getEntry(key);
    return entry ? entry->description : "";
}

std::map<ValidationRule, std::string> ConfigSchema::getValidationRules(const std::string& key) const {
    const ConfigSchemaEntry* entry = getEntry(key);
    return entry ? entry->validationRules : std::map<ValidationRule, std::string>();
}

std::vector<std::string> ConfigSchema::getDependencies(const std::string& key) const {
    const ConfigSchemaEntry* entry = getEntry(key);
    return entry ? std::vector<std::string>(entry->dependsOn.begin(), entry->dependsOn.end()) : std::vector<std::string>();
}

std::vector<std::string> ConfigSchema::getMutuallyExclusive(const std::string& key) const {
    const ConfigSchemaEntry* entry = getEntry(key);
    return entry ? std::vector<std::string>(entry->mutuallyExclusive.begin(), entry->mutuallyExclusive.end()) : std::vector<std::string>();
}

struct EnhancedConfig::Impl {
    std::map<std::string, std::any> values;
    std::map<std::string, ConfigSource> sources;
    std::map<std::string, std::string> stringValues;
    std::map<std::string, std::string> descriptions;
    std::map<std::string, ConfigSchemaEntry> schema;
    std::map<std::string, std::chrono::system_clock::time_point> changeHistory;
    
    size_t totalLookups = 0;
    size_t cacheHits = 0;
    double averageLookupTime = 0.0;
    size_t memoryUsage = 0;
    
    std::string version = "1.0.0";
    bool historyEnabled = true;
    bool optimized = false;
    std::vector<std::string> lookupIndex;
    
    std::chrono::system_clock::time_point lastSaveTime;
    bool hasChanged = false;
    
    std::vector<std::string> validationErrors;
};

EnhancedConfig::EnhancedConfig(const ConfigSchema& schema) : pImpl(std::make_unique<Impl>()) {
    // Copy schema entries
    for (const auto& pair : schema.getAllKeys()) {
        const ConfigSchemaEntry* entry = schema.getEntry(pair);
        if (entry) {
            pImpl->schema[pair] = *entry;
            
            // Set default value if provided
            if (!entry->defaultValue.empty()) {
                setString(pair, entry->defaultValue, ConfigSource::Default);
            }
        }
    }
}

EnhancedConfig::~EnhancedConfig() = default;

EnhancedConfig::EnhancedConfig(EnhancedConfig&&) noexcept = default;
EnhancedConfig& EnhancedConfig::operator=(EnhancedConfig&&) noexcept = default;

bool EnhancedConfig::loadFromFile(const std::string& filepath) {
    clearLastError();
    
    // Validate filepath
    if (filepath.empty()) {
        lastError = "Configuration file path cannot be empty";
        return false;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        lastError = "Configuration file does not exist: " + filepath;
        return false;
    }
    
    // Determine file format by extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJson(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYaml(filepath);
    } else {
        // Try to auto-detect format
        std::ifstream file(filepath);
        if (!file.is_open()) {
            lastError = "Failed to open configuration file: " + filepath + ". Error: " +
                std::strerror(errno);
            return false;
        }
        
        std::string firstLine;
        std::getline(file, firstLine);
        file.clear();
        file.seekg(0, std::ios::beg);
        
        // Check if it looks like JSON
        if (firstLine.find('{') != std::string::npos || firstLine.find('[') != std::string::npos) {
            return loadFromJson(filepath);
        } else {
            // Treat as simple key=value format (backward compatibility)
            clear();
            std::string line;
            size_t lineNum = 0;
            
            while (std::getline(file, line)) {
                lineNum++;
                
                // Skip empty lines and comments
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                // Parse simple key=value pairs
                size_t pos = line.find('=');
                if (pos == std::string::npos) {
                    lastError = "Invalid configuration format at line " + std::to_string(lineNum) +
                        ": missing '=' separator. Line: " + line;
                    return false;
                }
                
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Validate key
                if (key.empty()) {
                    lastError = "Empty key found at line " + std::to_string(lineNum);
                    return false;
                }
                
                if (!validateKeyPattern(key)) {
                    lastError = "Invalid key pattern at line " + std::to_string(lineNum) +
                        ": key must match pattern ^[a-zA-Z][a-zA-Z0-9_]*(\.[a-zA-Z][a-zA-Z0-9_]*)*$: " + key;
                    return false;
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                setString(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

bool EnhancedConfig::loadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError = "Failed to open JSON configuration file: " + filepath + ". Error: " +
            std::strerror(errno);
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to string representation
            std::string value;
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = std::to_string(it->get<int64_t>());
            } else if (it->is_number_float()) {
                value = std::to_string(it->get<double>());
            } else if (it->is_boolean()) {
                value = it->get<bool>() ? "true" : "false";
            } else if (it->is_array()) {
                // Convert array to JSON string representation
                json arr = *it;
                value = arr.dump();
            }
            
            setString(key, value, ConfigSource::File);
        }
        
        // Validate loaded configuration
        auto errors = validate();
        if (!errors.empty()) {
            lastError = "Configuration validation failed after loading from " + filepath +
                ": " + validationErrorsToString(errors);
            return false;
        }
        
    } catch (const json::parse_error& e) {
        lastError = "JSON parse error in configuration file " + filepath + ": " + e.what();
        return false;
    } catch (const json::type_error& e) {
        lastError = "JSON type error in configuration file " + filepath + ": " + e.what();
        return false;
    } catch (const std::exception& e) {
        lastError = "Unexpected error loading JSON configuration file " + filepath + ": " + e.what();
        return false;
    }
    
    return true;
}

bool EnhancedConfig::loadFromYaml(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        lastError = "Failed to open YAML configuration file: " + filepath + ". Error: " +
            std::strerror(errno);
        return false;
    }
    
    try {
        YAML::Node doc = YAML::LoadFile(filepath);
        
        for (const auto& pair : doc) {
            std::string key = pair.first.as<std::string>();
            
            // Convert YAML value to string representation
            std::string value;
            if (pair.second.IsScalar()) {
                if (pair.second.IsDouble()) {
                    value = std::to_string(pair.second.as<double>());
                } else if (pair.second.IsInt()) {
                    value = std::to_string(pair.second.as<int64_t>());
                } else if (pair.second.IsBool()) {
                    value = pair.second.as<bool>() ? "true" : "false";
                } else {
                    value = pair.second.as<std::string>();
                }
            } else {
                // Convert complex type to YAML string representation
                value = YAML::Dump(pair.second);
            }
            
            setString(key, value, ConfigSource::File);
        }
        
        // Validate loaded configuration
        auto errors = validate();
        if (!errors.empty()) {
            lastError = "Configuration validation failed after loading from " + filepath +
                ": " + validationErrorsToString(errors);
            return false;
        }
        
    } catch (const YAML::Exception& e) {
        lastError = "YAML parse error in configuration file " + filepath + ": " + e.what();
        return false;
    } catch (const std::exception& e) {
        lastError = "Unexpected error loading YAML configuration file " + filepath + ": " + e.what();
        return false;
    }
    
    return true;
}

void EnhancedConfig::loadFromEnvironment(const std::string& prefix) {
    // Iterate through all environment variables
    const char* envPtr = nullptr;
    while ((envPtr = std::getenv(nullptr)) != nullptr) {
        std::string envName(envPtr);
        std::string value = std::getenv(envPtr);
        
        // Check if variable starts with the specified prefix
        if (envName.size() > prefix.size() && 
            envName.compare(0, prefix.size(), prefix) == 0) {
            
            // Remove prefix and convert to lowercase for key
            std::string key = envName.substr(prefix.size());
            
            // Convert key format (dot notation for nested values)
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            
            setString(key, value, ConfigSource::Environment);
        }
    }
}

bool EnhancedConfig::loadFromCommandLine(int argc, char** argv) {
    clearLastError();
    
    if (!argv) {
        lastError = "Invalid command line arguments: argv is null";
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        if (!argv[i]) {
            lastError = "Null argument pointer at position " + std::to_string(i);
            return false;
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key before setting
                if (!validateKeyPattern(key)) {
                    lastError = "Invalid key pattern in command line argument: " + key;
                    return false;
                }
                
                setString(key, value, ConfigSource::CommandLine);
            } else {
                lastError = "Invalid command line argument format: missing '=' in --key=value: " + arg;
                return false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key before setting
            if (!validateKeyPattern(key)) {
                lastError = "Invalid key pattern in command line argument: " + key;
                return false;
            }
            
            setString(key, value, ConfigSource::CommandLine);
        } else {
            lastError = "Invalid command line argument format: " + arg;
            return false;
        }
    }
    return true;
}

bool EnhancedConfig::saveToFile(const std::string& filepath) const {
    clearLastError();
    
    // Validate filepath
    if (filepath.empty()) {
        lastError = "Configuration file path cannot be empty for saving";
        return false;
    }
    
    // Check if directory exists and is writable
    std::filesystem::path filePath(filepath);
    std::filesystem::path parentDir = filePath.parent_path();
    
    if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
        lastError = "Configuration directory does not exist: " + parentDir.string();
        return false;
    }
    
    std::ofstream file(filepath, std::ios::trunc);
    if (!file.is_open()) {
        lastError = "Failed to open configuration file for writing: " + filepath + ". Error: " +
            std::strerror(errno);
        return false;
    }
    
    try {
        file << "# NLM Enhanced Configuration File\n";
        file << "# Generated at: " << std::chrono::system_clock::now() << "\n\n";
        
        for (const auto& pair : pImpl->values) {
            const std::string& key = pair.first;
            const std::any& value = pair.second;
            
            file << "# " << (pImpl->descriptions.find(key) != pImpl->descriptions.end() ? 
                               pImpl->descriptions.at(key) : "") << "\n";
            
            // Convert value to string safely
            std::ostringstream valueStream;
            std::visit([&valueStream](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    valueStream << '"' << arg << '"';
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    valueStream << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) valueStream << ", ";
                        valueStream << arg[i];
                    }
                    valueStream << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    valueStream << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) valueStream << ", ";
                        valueStream << arg[i];
                    }
                    valueStream << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    valueStream << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) valueStream << ", ";
                        valueStream << '"' << arg[i] << '"';
                    }
                    valueStream << "]";
                } else {
                    valueStream << arg;
                }
            }, value);
            
            file << key << " = " << valueStream.str() << "\n\n";
        }
        
        file << "# Configuration saved at: " << std::filesystem::current_path() << "\n";
        
    } catch (const std::exception& e) {
        lastError = "Failed to write configuration to file " + filepath + ": " + e.what();
        return false;
    }
    
    return true;
}

std::string EnhancedConfig::exportToJson() const {
    json j;
    
    for (const auto& pair : pImpl->values) {
        const std::string& key = pair.first;
        const std::any& value = pair.second;
        
        if (std::holds_alternative<int>(value)) {
            j[key] = std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            j[key] = std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            j[key] = std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            j[key] = std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            j[key] = std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            j[key] = std::get<std::vector<int>>(value);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            j[key] = std::get<std::vector<double>>(value);
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            j[key] = std::get<std::vector<std::string>>(value);
        }
    }
    
    return j.dump(2);
}

std::string EnhancedConfig::exportToYaml() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& pair : pImpl->values) {
        const std::string& key = pair.first;
        const std::any& value = pair.second;
        
        if (std::holds_alternative<int>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            emitter << YAML::Key << key << YAML::Value << std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            emitter << YAML::Key << key << YAML::Value;
            emitter << YAML::Flow;
            emitter << YAML::BeginSeq;
            for (const auto& item : std::get<std::vector<int>>(value)) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            emitter << YAML::Key << key << YAML::Value;
            emitter << YAML::Flow;
            emitter << YAML::BeginSeq;
            for (const auto& item : std::get<std::vector<double>>(value)) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            emitter << YAML::Key << key << YAML::Value;
            emitter << YAML::Flow;
            emitter << YAML::BeginSeq;
            for (const auto& item : std::get<std::vector<std::string>>(value)) {
                emitter << item;
            }
            emitter << YAML::EndSeq;
        }
    }
    
    emitter << YAML::EndMap;
    
    std::stringstream buffer;
    buffer << emitter;
    return buffer.str();
}

// Helper functions implementation
template<typename T>
std::optional<T> EnhancedConfig::get(const std::string& key) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (key.empty()) {
        lastError = "Cannot get value for empty key";
        return std::nullopt;
    }
    
    auto it = pImpl->values.find(key);
    if (it == pImpl->values.end()) {
        lastError = "Configuration key not found: " + key;
        return std::nullopt;
    }
    
    try {
        auto value = it->second;
        
        // Type conversion to T
        if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int>(value)) {
                return std::get<int>(value);
            }
        } else if constexpr (std::is_same_v<T, int64_t>) {
            if (std::holds_alternative<int64_t>(value)) {
                return std::get<int64_t>(value);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(value)) {
                return std::get<std::string>(value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (std::holds_alternative<std::vector<int>>(value)) {
                return std::get<std::vector<int>>(value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(value)) {
                return std::get<std::vector<double>>(value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (std::holds_alternative<std::vector<std::string>>(value)) {
                return std::get<std::vector<std::string>>(value);
            }
        }
        
        lastError = "Type mismatch when retrieving key " + key + ": expected " +
            typeid(T).name();
        return std::nullopt;
        
    } catch (const std::bad_variant_access&) {
        lastError = "Type mismatch when retrieving key " + key + ": expected " +
            typeid(T).name();
        return std::nullopt;
    }
    
    // Update performance metrics
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    pImpl->totalLookups++;
    pImpl->averageLookupTime = 
        (pImpl->averageLookupTime * (pImpl->totalLookups - 1) + duration.count()) / 
        pImpl->totalLookups;
    
    return std::nullopt;
}

// Rest of the implementation...

} // namespace nlm