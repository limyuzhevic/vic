// EnhancedConfig.cpp implementation
#include "EnhancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <iomanip>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigSchema schema;
    std::map<std::string, ConfigBackup> backups;
    std::map<std::string, std::string> envMapping;
    std::string defaultFileFormat;
    bool validationEnabled;
    bool autoBackupEnabled;
    size_t maxBackups;
};

Config::Config() : pImpl(std::make_unique<Impl>()), currentProfile(ConfigProfile::None) {
    pImpl->validationEnabled = true;
    pImpl->autoBackupEnabled = true;
    pImpl->maxBackups = 10;
    pImpl->defaultFileFormat = "simple";
    
    // Initialize environment mapping
    pImpl->envMapping["random_seed"] = "NLM_RANDOM_SEED";
    pImpl->envMapping["simulation_timestep"] = "NLM_SIMULATION_TIMESTEP";
    pImpl->envMapping["neuron_count"] = "NLM_NEURON_COUNT";
    pImpl->envMapping["log_level"] = "NLM_LOG_LEVEL";
    pImpl->envMapping["visualization_enabled"] = "NLM_VISUALIZATION_ENABLED";
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError("Cannot open configuration file: " + filepath, filepath,
                            "Check that the file exists and is readable");
    }
    
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    bool success = false;
    if (extension == ".json" || extension == ".yaml" || extension == ".yml") {
        success = loadJsonFormat(filepath, validate);
    } else {
        success = loadSimpleFormat(filepath);
    }
    
    if (success && pImpl->autoBackupEnabled) {
        createBackup("auto_save");
    }
    
    lastModified = std::chrono::system_clock::now();
    return success;
}

bool Config::loadFromArgs(int argc, char** argv, bool validate) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = normalizeKey(arg.substr(2, pos - 2));
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = normalizeKey(arg.substr(1));
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
            // Handle --help
            else if (arg == "--help") {
                std::cout << getAllHelp(pImpl->schema) << std::endl;
                return false; // Exit successfully after showing help
            }
            // Handle --validate
            else if (arg == "--validate") {
                if (validate && !pImpl->schema.getKeys().empty()) {
                    auto errors = validateAll(pImpl->schema);
                    if (!errors.empty()) {
                        std::cout << "Configuration validation failed:\n";
                        for (const auto& error : errors) {
                            std::cout << "  " << error.toString() << "\n";
                        }
                        return false;
                    }
                    std::cout << "Configuration validation passed.\n";
                }
                return false;
            }
        }
    } catch (const ConfigException& e) {
        std::cerr << "Error loading from command line: " << e.toString() << std::endl;
        return false;
    }
    
    if (validate && !pImpl->schema.getKeys().empty()) {
        auto errors = validateAll(pImpl->schema);
        if (!errors.empty()) {
            for (const auto& error : errors) {
                std::cerr << "Validation error: " << error.toString() << std::endl;
            }
            return false;
        }
    }
    
    lastModified = std::chrono::system_clock::now();
    return true;
}

bool Config::loadFromEnvironment(const std::map<std::string, std::string>& envVars) {
    bool success = true;
    
    for (const auto& pair : pImpl->envMapping) {
        const std::string& configKey = pair.first;
        const std::string& envKey = pair.second;
        
        // Get environment variable value
        std::string envValue;
        if (!envVars.empty()) {
            auto it = envVars.find(envKey);
            if (it != envVars.end()) envValue = it->second;
        } else {
            // Use actual environment
            const char* envVal = std::getenv(envKey.c_str());
            if (envVal) envValue = envVal;
        }
        
        if (!envValue.empty()) {
            try {
                ConfigValue value = parseValue(envValue);
                set(configKey, value, ConfigSource::Environment);
            } catch (...) {
                std::cerr << "Warning: Failed to parse environment variable " << envKey 
                         << " for config key " << configKey << std::endl;
                success = false;
            }
        }
    }
    
    lastModified = std::chrono::system_clock::now();
    return success;
}

bool Config::loadFromProfile(ConfigProfile profile, const ConfigSchema& schema) {
    clear();
    setProfile(profile, schema);
    
    // Apply profile defaults
    applySmartDefaults(schema, profile);
    
    lastModified = std::chrono::system_clock::now();
    return true;
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (format == "json" || ext == ".json") {
        return saveJsonFormat(filepath);
    } else if (format == "yaml" || ext == ".yaml" || ext == ".yml") {
        return saveYamlFormat(filepath);
    } else {
        return saveSimpleFormat(filepath, pImpl->entries);
    }
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

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    addEntry(key, value, source);
    lastModified = std::chrono::system_clock::now();
    return true;
}

bool Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, int value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, int64_t value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, double value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, bool value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::validate(const ConfigSchema& schema) const {
    return validateAll(schema).empty();
}

std::vector<ConfigError> Config::validateParameter(const std::string& key, const ConfigSchema& schema) const {
    std::vector<ConfigError> errors;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        errors.emplace_back(ConfigErrorType::InvalidValue, 
                         "Parameter not found in configuration",
                         key, "Check parameter name or add the parameter");
        return errors;
    }
    
    const auto& param = schema.getParameter(key);
    if (param.key.empty()) {
        errors.emplace_back(ConfigErrorType::SchemaError,
                         "Parameter not defined in schema",
                         key, "Check if the parameter is defined in the schema");
        return errors;
    }
    
    auto paramErrors = Validator::validate(it->value, param);
    errors.insert(errors.end(), paramErrors.begin(), paramErrors.end());
    
    return errors;
}

std::vector<ConfigError> Config::validateAll(const ConfigSchema& schema) const {
    std::vector<ConfigError> errors;
    
    std::map<std::string, ConfigValue> values;
    for (const auto& entry : pImpl->entries) {
        values[entry.key] = entry.value;
    }
    
    // Validate each parameter
    for (const auto& pair : pImpl->entries) {
        const auto& param = schema.getParameter(pair.first);
        if (!param.key.empty()) {
            auto paramErrors = Validator::validate(pair.value, param);
            errors.insert(errors.end(), paramErrors.begin(), paramErrors.end());
        }
    }
    
    // Validate dependencies
    for (const auto& pair : schema.getParameters()) {
        const auto& param = pair.second;
        for (const auto& dep : param.dependencies) {
            if (values.find(dep) == values.end()) {
                errors.emplace_back(ConfigErrorType::MissingDependency,
                                 "Missing required dependency: '" + dep + "'",
                                 pair.first, "Set dependency '" + dep + "' before using this parameter");
            }
        }
    }
    
    // Check for conflicting keys
    for (const auto& pair : schema.getParameters()) {
        const auto& param = pair.second;
        for (const auto& excl : param.exclusiveKeys) {
            if (values.find(excl) != values.end()) {
                errors.emplace_back(ConfigErrorType::ConflictingValues,
                                 "Parameter '" + pair.first + "' conflicts with '" + excl + "'",
                                 pair.first, "Remove one of the conflicting parameters");
            }
        }
    }
    
    return errors;
}

std::string Config::getHelp(const std::string& key, const ConfigSchema& schema) const {
    return ConfigHelpSystem::getParameterHelp(key, schema);
}

std::string Config::getCategoryHelp(const std::string& category, const ConfigSchema& schema) const {
    return ConfigHelpSystem::getCategoryHelp(category, schema);
}

std::string Config::getAllHelp(const ConfigSchema& schema) const {
    return ConfigHelpSystem::getAllHelp(schema);
}

std::string Config::getProfileHelp() const {
    return ConfigHelpSystem::getProfileHelp();
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
    lastModified = std::chrono::system_clock::now();
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    backups.clear();
}

bool Config::createBackup(const std::string& name, const std::string& description) {
    ConfigBackup backup(name, description);
    
    for (const auto& entry : pImpl->entries) {
        backup.values[entry.key] = entry.value;
    }
    
    backup.version = backups.size() + 1;
    
    backups[name] = backup;
    
    // Clean up old backups
    if (pImpl->autoBackupEnabled && backups.size() > pImpl->maxBackups) {
        auto it = backups.begin();
        std::advance(it, backups.size() - pImpl->maxBackups);
        backups.erase(it);
    }
    
    return true;
}

bool Config::restoreBackup(const std::string& name) {
    auto it = backups.find(name);
    if (it == backups.end()) {
        return false;
    }
    
    clear();
    
    for (const auto& pair : it->second.values) {
        pImpl->entries.emplace_back(pair.first, pair.second, ConfigSource::Runtime);
    }
    
    lastModified = std::chrono::system_clock::now();
    return true;
}

std::vector<std::string> Config::getBackupNames() const {
    std::vector<std::string> names;
    for (const auto& pair : backups) {
        names.push_back(pair.first);
    }
    return names;
}

std::string Config::getBackupInfo(const std::string& name) const {
    auto it = backups.find(name);
    if (it == backups.end()) {
        return "Backup not found: " + name;
    }
    
    std::stringstream ss;
    ss << "Backup: " << it->second.name << "\n";
    ss << "Description: " << it->second.description << "\n";
    ss << "Created: " << it->second.getCreatedString() << "\n";
    ss << "Version: " << it->second.version << "\n";
    ss << "Entries: " << it->second.values.size() << "\n";
    
    return ss.str();
}

std::string Config::diff(const Config& other) const {
    std::stringstream ss;
    ss << "Configuration differences:\n";
    
    std::set<std::string> thisKeys, otherKeys;
    for (const auto& entry : pImpl->entries) {
        thisKeys.insert(entry.key);
    }
    for (const auto& entry : other.pImpl->entries) {
        otherKeys.insert(entry.key);
    }
    
    // Find keys in this but not in other
    for (const auto& key : thisKeys) {
        if (otherKeys.find(key) == otherKeys.end()) {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            if (it != pImpl->entries.end()) {
                ss << "+ " << key << " = " << formatValue(it->value) << " (added)\n";
            }
        } else {
            // Compare values
            auto thisIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            
            if (thisIt != pImpl->entries.end() && otherIt != other.pImpl->entries.end() &&
                thisIt->value != otherIt->value) {
                ss << "~ " << key << ":\n";
                ss << "  this:  " << formatValue(thisIt->value) << "\n";
                ss << "  other: " << formatValue(otherIt->value) << "\n";
            }
        }
    }
    
    // Find keys in other but not in this
    for (const auto& key : otherKeys) {
        if (thisKeys.find(key) == thisKeys.end()) {
            auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            if (it != other.pImpl->entries.end()) {
                ss << "- " << key << " = " << formatValue(it->value) << " (removed)\n";
            }
        }
    }
    
    if (ss.str() == "Configuration differences:\n") {
        ss << "No differences found.\n";
    }
    
    return ss.str();
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (otherIt == other.pImpl->entries.end() || otherIt->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        ss << "  " << entry.key << " = [" << formatValue(entry.value) << "] "
           << "(" << entry.getSourceString() << ") "
           << "[" << entry.getTimestampString() << "]\n";
    }
    
    ss << "\nBackups: " << backups.size() << "\n";
    ss << "Profile: " << getProfileName(currentProfile) << "\n";
    ss << "Last modified: " << getLastModifiedString() << "\n";
    
    return ss.str();
}

std::string Config::summary(const ConfigSchema& schema) const {
    std::stringstream ss;
    ss << summary(); // Basic summary
    
    // Add validation summary
    auto errors = validateAll(schema);
    if (!errors.empty()) {
        ss << "\nValidation errors: " << errors.size() << "\n";
        for (const auto& error : errors) {
            ss << "  " << error.toString() << "\n";
        }
    } else {
        ss << "\nValidation: PASSED\n";
    }
    
    // Add profile-specific info
    ss << "\nProfile-specific defaults: ";
    for (const auto& pair : schema.getParameters()) {
        const auto& param = pair.second;
        if (!param.profileDefaults.empty()) {
            ss << pair.first << " ";
        }
    }
    ss << "\n";
    
    return ss.str();
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCountBySource(ConfigSource source) const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&source](const ConfigEntry& e) { return e.source == source; });
}

double Config::getValidationScore(const ConfigSchema& schema) const {
    auto errors = validateAll(schema);
    size_t totalParams = schema.getParameters().size();
    
    if (totalParams == 0) return 0.0;
    
    return 1.0 - (static_cast<double>(errors.size()) / totalParams);
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it != pImpl->entries.end()) {
            it->value = entry.value;
            it->source = entry.source; // Preserve original source
        } else {
            pImpl->entries.push_back(entry);
        }
    }
    
    lastModified = std::chrono::system_clock::now();
}

bool Config::setProfile(ConfigProfile profile, const ConfigSchema& schema) {
    currentProfile = profile;
    
    // Clear existing values
    clear();
    
    // Apply profile defaults
    applySmartDefaults(schema, profile);
    
    lastModified = std::chrono::system_clock::now();
    return true;
}

std::string Config::getProfileName(ConfigProfile profile) const {
    switch (profile) {
        case ConfigProfile::Research: return "Research";
        case ConfigProfile::Simulation: return "Simulation";
        case ConfigProfile::Production: return "Production";
        case ConfigProfile::Development: return "Development";
        case ConfigProfile::Benchmark: return "Benchmark";
        default: return "None";
    }
}

void Config::applySmartDefaults(const ConfigSchema& schema, ConfigProfile profile) {
    for (const auto& pair : schema.getParameters()) {
        const auto& param = pair.second;
        
        // Check if profile has a default for this parameter
        auto profileIt = param.profileDefaults.find(getProfileName(profile));
        if (profileIt != param.profileDefaults.end()) {
            // Apply profile-specific default
            const auto& profileVal = profileIt->second;
            
            try {
                if (std::holds_alternative<int>(profileVal)) {
                    set(pair.first, std::get<int>(profileVal), ConfigSource::Profile);
                } else if (std::holds_alternative<int64_t>(profileVal)) {
                    set(pair.first, std::get<int64_t>(profileVal), ConfigSource::Profile);
                } else if (std::holds_alternative<double>(profileVal)) {
                    set(pair.first, std::get<double>(profileVal), ConfigSource::Profile);
                } else if (std::holds_alternative<bool>(profileVal)) {
                    set(pair.first, std::get<bool>(profileVal), ConfigSource::Profile);
                } else if (std::holds_alternative<std::string>(profileVal)) {
                    set(pair.first, std::get<std::string>(profileVal), ConfigSource::Profile);
                }
            } catch (...) {
                // Ignore conversion errors
            }
        } else if (param.type != ConfigType::None && param.required) {
            // For required parameters without profile defaults, use regular defaults
            std::string defaultStr = schema.getDefaultValueString(pair.first);
            if (!defaultStr.empty()) {
                try {
                    ConfigValue value = parseValue(defaultStr);
                    set(pair.first, value, ConfigSource::Profile);
                } catch (...) {
                    // Ignore parsing errors
                }
            }
        }
    }
}

std::string Config::getSmartDefaults(const ConfigSchema& schema, ConfigProfile profile) const {
    std::stringstream ss;
    ss << "Smart defaults for " << getProfileName(profile) << " profile:\n";
    
    for (const auto& pair : schema.getParameters()) {
        const auto& param = pair.second;
        
        // Check profile-specific default
        auto profileIt = param.profileDefaults.find(getProfileName(profile));
        std::string defaultVal;
        
        if (profileIt != param.profileDefaults.end()) {
            const auto& profileVal = profileIt->second;
            switch (profileVal.index()) {
                case 0: defaultVal = std::to_string(std::get<int>(profileVal)); break;
                case 1: defaultVal = std::to_string(std::get<int64_t>(profileVal)); break;
                case 2: defaultVal = std::to_string(std::get<double>(profileVal)); break;
                case 3: defaultVal = std::get<bool>(profileVal) ? "true" : "false"; break;
                case 4: defaultVal = "\"" + std::get<std::string>(profileVal) + "\""; break;
            }
        } else {
            defaultVal = schema.getDefaultValueString(pair.first);
        }
        
        if (!defaultVal.empty()) {
            ss << "  " << pair.first << " = " << defaultVal << "\n";
        }
    }
    
    return ss.str();
}

bool Config::isValidType(const ConfigParameter& param, const ConfigValue& value) {
    try {
        switch (param.type) {
            case ConfigType::Integer:
                return std::holds_alternative<int>(value);
            case ConfigType::Integer64:
                return std::holds_alternative<int64_t>(value);
            case ConfigType::Float:
                return std::holds_alternative<double>(value);
            case ConfigType::Boolean:
                return std::holds_alternative<bool>(value);
            case ConfigType::String:
                return std::holds_alternative<std::string>(value);
            case ConfigType::StringList:
                return std::holds_alternative<std::vector<std::string>>(value);
            case ConfigType::IntList:
                return std::holds_alternative<std::vector<int>>(value);
            case ConfigType::FloatList:
                return std::holds_alternative<std::vector<double>>(value);
            default:
                return false;
        }
    } catch (...) {
        return false;
    }
}

bool Config::isValidRange(const ConfigParameter& param, const ConfigValue& value) {
    if (!isValidType(param, value)) return false;
    
    try {
        switch (param.type) {
            case ConfigType::Float:
                if (param.inRange && std::holds_alternative<double>(value)) {
                    double val = std::get<double>(value);
                    if (param.floatRange.has_value()) {
                        return val >= param.floatRange->first && val <= param.floatRange->second;
                    }
                }
                break;
            case ConfigType::Integer64:
                if (param.inRange && std::holds_alternative<int64_t>(value)) {
                    int64_t val = std::get<int64_t>(value);
                    if (param.int64Range.has_value()) {
                        return val >= param.int64Range->first && val <= param.int64Range->second;
                    }
                }
                break;
            case ConfigType::Integer:
                if (param.inRange && std::holds_alternative<int>(value)) {
                    int val = std::get<int>(value);
                    if (param.intRange.has_value()) {
                        return val >= param.intRange->first && val <= param.intRange->second;
                    }
                }
                break;
            case ConfigType::Boolean:
                return true; // Booleans always valid for range checks
            default:
                break;
        }
        
        if (param.positive && isValidType(param, value)) {
            if (std::holds_alternative<int>(value) && std::get<int>(value) < 0) return false;
            if (std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) < 0) return false;
            if (std::holds_alternative<double>(value) && std::get<double>(value) < 0) return false;
        }
        
        if (param.nonNegative && isValidType(param, value)) {
            if (std::holds_alternative<int>(value) && std::get<int>(value) < 0) return false;
            if (std::holds_alternative<int64_t>(value) && std::get<int64_t>(value) < 0) return false;
            if (std::holds_alternative<double>(value) && std::get<double>(value) < 0) return false;
        }
        
    } catch (...) {
        return false;
    }
    
    return true;
}

bool Config::isValidPositive(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return std::get<int>(value) >= 0;
    if (std::holds_alternative<int64_t>(value)) return std::get<int64_t>(value) >= 0;
    if (std::holds_alternative<double>(value)) return std::get<double>(value) >= 0;
    return false;
}

bool Config::isValidNonNegative(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return std::get<int>(value) >= 0;
    if (std::holds_alternative<int64_t>(value)) return std::get<int64_t>(value) >= 0;
    if (std::holds_alternative<double>(value)) return std::get<double>(value) >= 0;
    return false;
}

std::vector<std::string> Config::getValidValueStrings(const ConfigParameter& param) {
    std::vector<std::string> validValues;
    
    for (const auto& val : param.validValues) {
        validValues.push_back(val);
    }
    
    if (param.floatRange.has_value()) {
        validValues.push_back(std::to_string(param.floatRange->first) + " (minimum)");
        validValues.push_back(std::to_string(param.floatRange->second) + " (maximum)");
    } else if (param.int64Range.has_value()) {
        validValues.push_back(std::to_string(param.int64Range->first) + " (minimum)");
        validValues.push_back(std::to_string(param.int64Range->second) + " (maximum)");
    } else if (param.intRange.has_value()) {
        validValues.push_back(std::to_string(param.intRange->first) + " (minimum)");
        validValues.push_back(std::to_string(param.intRange->second) + " (maximum)");
    }
    
    return validValues;
}

std::string Config::getErrorSuggestion(const ConfigError& error) {
    return ConfigRecoveryHelper::getRecoverySuggestion(error);
}

std::string Config::getErrorDetails(const ConfigError& error) {
    std::stringstream ss;
    ss << "Error: " << error.message << "\n";
    if (!error.key.empty()) ss << "Key: " << error.key << "\n";
    if (!error.suggestion.empty()) ss << "Suggestion: " << error.suggestion << "\n";
    return ss.str();
}

std::vector<std::string> Config::suggestValues(const std::string& key, const ConfigSchema& schema) {
    std::vector<std::string> suggestions;
    const auto& param = schema.getParameter(key);
    
    if (param.key.empty()) return suggestions;
    
    // Add profile-specific suggestions
    for (const auto& profilePair : param.profileDefaults) {
        suggestions.push_back("Profile '" + profilePair.first + "': " + getDefaultValueString(param));
    }
    
    // Add valid value suggestions
    for (const auto& val : param.validValues) {
        suggestions.push_back("Use '" + val + "'");
    }
    
    // Add range suggestions
    if (param.floatRange.has_value()) {
        suggestions.push_back("Try " + std::to_string(param.floatRange->first + param.floatRange->second / 2.0) + " (midpoint)");
        suggestions.push_back("Try " + std::to_string(param.floatRange->first) + " (minimum)");
        suggestions.push_back("Try " + std::to_string(param.floatRange->second) + " (maximum)");
    }
    
    return suggestions;
}

bool Config::Validator::validate(const ConfigValue& value, const ConfigParameter& param) {
    return isValidType(param, value) && isValidRange(param, value);
}

std::vector<ConfigError> Config::Validator::validateAll(const std::map<std::string, ConfigValue>& values,
                                                       const ConfigSchema& schema) {
    std::vector<ConfigError> errors;
    
    for (const auto& pair : values) {
        const auto& param = schema.getParameter(pair.first);
        if (!param.key.empty()) {
            if (!validate(pair.second, param)) {
                errors.emplace_back(ConfigErrorType::InvalidValue,
                                 "Invalid value for parameter",
                                 pair.first, "Try a value of the correct type and within range");
            }
        }
    }
    
    return errors;
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

std::string Config::formatValue(const ConfigValue& value) {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << arg[i] << "\"";
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else {
            ss << arg;
        }
    }, value);
    return ss.str();
}

ConfigValue Config::parseValue(const std::string& str) {
    std::string trimmed = trim(str);
    
    if (trimmed.empty()) {
        return std::string("");
    }
    
    if (trimmed == "true") return true;
    if (trimmed == "false") return false;
    
    if (trimmed.front() == '"' && trimmed.back() == '"') {
        return trimmed.substr(1, trimmed.size() - 2);
    }
    
    try {
        if (trimmed.find('.') != std::string::npos) {
            return std::stod(trimmed);
        } else {
            return std::stoi(trimmed);
        }
    } catch (...) {
        return trimmed;
    }
}

void Config::addEntry(const std::string& key, const ConfigValue& value, ConfigSource source,
                      const std::string& description, const std::string& category) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, description, category);
    }
}

bool Config::saveSimpleFormat(const std::string& filepath, const std::vector<ConfigEntry>& entries) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : entries) {
        file << "# " << entry.description << "\n";
        if (!entry.category.empty()) {
            file << "# Category: " << entry.category << "\n";
        }
        file << entry.key << " = " << formatValue(entry.value) << "\n";
        file << "# Source: " << entry.getSourceString() << "\n";
        file << "# Last modified: " << entry.getTimestampString() << "\n\n";
    }
    
    return true;
}

bool Config::loadSimpleFormat(const std::string& filepath, std::vector<ConfigEntry>& entries) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentDescription;
    std::string currentCategory;
    std::string currentKey;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == '#') {
            if (line.find("Description:") != std::string::npos) {
                currentDescription = line.substr(14);
            } else if (line.find("Category:") != std::string::npos) {
                currentCategory = line.substr(10);
            } else if (line.find("Source:") != std::string::npos) {
                // Source comment, skip
            } else if (line.find("Last modified:") != std::string::npos) {
                // Timestamp comment, skip
            }
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            currentKey = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            ConfigValue configValue = parseValue(value);
            entries.emplace_back(currentKey, configValue, ConfigSource::File,
                               currentDescription, currentCategory);
        }
    }
    
    return true;
}

bool Config::loadJsonFormat(const std::string& filepath, bool validate) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, use simple format for compatibility
    // In a full implementation, this would parse JSON/YAML
    return loadSimpleFormat(filepath, pImpl->entries);
}

bool Config::saveJsonFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": " << formatValue(entry.value);
        if (i < pImpl->entries.size() - 1) file << ",";
        file << "\n";
    }
    file << "}\n";
    
    return true;
}

bool Config::saveYamlFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by enhanced configuration system\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << entry.key << ": " << formatValue(entry.value) << "\n";
    }
    
    return true;
}

std::string Config::getLastModifiedString() const {
    auto time = std::chrono::system_clock::to_time_t(lastModified);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool Config::parseArg(const std::string& arg, std::string& key, std::string& value) {
    if (arg.substr(0, 2) == "--") {
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            key = normalizeKey(arg.substr(2, pos - 2));
            value = arg.substr(pos + 1);
            return true;
        }
    } else if (arg[0] == '-' && arg.size() > 1) {
        key = normalizeKey(arg.substr(1));
        return true;
    }
    return false;
}

std::string Config::normalizeKey(const std::string& key) {
    std::string result = key;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    
    // Replace underscores and dashes with dots
    std::replace(result.begin(), result.end(), '_', '.');
    std::replace(result.begin(), result.end(), '-', '.');
    
    return result;
}

std::string Config::getBackupFilePath(const std::string& name) const {
    return "config_backup_" + name + "_" + 
           std::to_string(std::time(nullptr)) + ".cfg";
}

std::string Config::generateBackupName() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << "backup_" << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return ss.str();
}

} // namespace nlm