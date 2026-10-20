#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <map>
#include <unordered_set>
#include <utility>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// ConfigError implementations
std::string ConfigError::categoryToString(ConfigErrorCategory category) {
    switch (category) {
        case ConfigErrorCategory::Validation: return "Validation";
        case ConfigErrorCategory::TypeMismatch: return "TypeMismatch";
        case ConfigErrorCategory::MissingRequired: return "MissingRequired";
        case ConfigErrorCategory::ConstraintViolation: return "ConstraintViolation";
        case ConfigErrorCategory::DependencyFailure: return "DependencyFailure";
        case ConfigErrorCategory::Serialization: return "Serialization";
        case ConfigErrorCategory::Transaction: return "Transaction";
        case ConfigErrorCategory::Schema: return "Schema";
        default: return "Unknown";
    }
}

std::string ConfigError::severityToString(ConfigErrorSeverity severity) {
    switch (severity) {
        case ConfigErrorSeverity::Warning: return "Warning";
        case ConfigErrorSeverity::Error: return "Error";
        case ConfigErrorSeverity::Critical: return "Critical";
        default: return "Unknown";
    }
}

// ConfigSchema implementations
ConfigSchema& ConfigSchema::addValidation(const std::string& key,
                                         ConstraintType type,
                                         const std::string& constraintValue,
                                         const std::string& errorMessage) {
    ValidationRule rule;
    rule.key = key;
    rule.type = type;
    rule.constraintValue = constraintValue;
    rule.errorMessage = errorMessage;
    rules_.push_back(rule);
    return *this;
}

ConfigSchema& ConfigSchema::addRequired(const std::string& key, const std::string& errorMessage) {
    ValidationRule rule;
    rule.key = key;
    rule.type = ConstraintType::None;
    rule.isRequired = true;
    rule.errorMessage = errorMessage.empty() ? ("Required key missing: " + key) : errorMessage;
    rules_.push_back(rule);
    return *this;
}

ConfigSchema& ConfigSchema::addDependency(const std::string& dependentKey,
                                          const std::string& requiredKey,
                                          const std::string& errorMessage) {
    ValidationRule rule;
    rule.key = dependentKey;
    rule.type = ConstraintType::None;
    rule.dependsOn = requiredKey;
    rule.errorMessage = errorMessage.empty() ? ("Key " + dependentKey + " depends on " + requiredKey) : errorMessage;
    rules_.push_back(rule);
    return *this;
}

ConfigSchema& ConfigSchema::addEnum(const std::string& key,
                                    const std::vector<std::string>& allowedValues,
                                    const std::string& errorMessage) {
    std::ostringstream oss;
    for (size_t i = 0; i < allowedValues.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << allowedValues[i];
    }
    
    ValidationRule rule;
    rule.key = key;
    rule.type = ConstraintType::Enum;
    rule.constraintValue = oss.str();
    rule.errorMessage = errorMessage.empty() ? ("Invalid value for key " + key + ". Allowed: " + oss.str()) : errorMessage;
    rules_.push_back(rule);
    return *this;
}

ConfigSchema& ConfigSchema::addRange(const std::string& key,
                                     double minValue,
                                     double maxValue,
                                     const std::string& errorMessage) {
    std::ostringstream oss;
    oss << minValue << " - " << maxValue;
    
    ValidationRule rule;
    rule.key = key;
    rule.type = ConstraintType::None;
    rule.constraintValue = oss.str();
    rule.errorMessage = errorMessage.empty() ? ("Value for key " + key + " must be in range: " + oss.str()) : errorMessage;
    rules_.push_back(rule);
    return *this;
}

ConfigSchema& ConfigSchema::addRegex(const std::string& key,
                                     const std::string& pattern,
                                     const std::string& errorMessage) {
    ValidationRule rule;
    rule.key = key;
    rule.type = ConstraintType::Regex;
    rule.constraintValue = pattern;
    rule.errorMessage = errorMessage.empty() ? ("Key " + key + " must match pattern: " + pattern) : errorMessage;
    rules_.push_back(rule);
    return *this;
}

std::vector<ConfigError> ConfigSchema::validate(const Config& config) const {
    std::vector<ConfigError> errors;
    
    for (const auto& rule : rules_) {
        if (rule.isRequired && !config.has(rule.key)) {
            errors.emplace_back("Required key missing: " + rule.key,
                               ConfigErrorCategory::MissingRequired,
                               ConfigErrorSeverity::Error,
                               rule.key);
        }
        
        if (!rule.dependsOn.empty() && !config.has(rule.dependsOn)) {
            errors.emplace_back("Dependency key missing: " + rule.dependsOn,
                               ConfigErrorCategory::DependencyFailure,
                               ConfigErrorSeverity::Error,
                               rule.dependsOn);
        }
        
        if (config.has(rule.key)) {
            auto value = config.get<std::string>(rule.key);
            if (!value) continue;
            
            bool valid = true;
            
            if (rule.type == ConstraintType::Enum) {
                std::istringstream iss(rule.constraintValue);
                std::string allowedValue;
                std::vector<std::string> allowedList;
                while (std::getline(iss, allowedValue, ',')) {
                    allowedList.push_back(allowedValue);
                }
                if (std::find(allowedList.begin(), allowedList.end(), *value) == allowedList.end()) {
                    valid = false;
                }
            }
            
            else if (rule.type == ConstraintType::Regex) {
                std::regex regex(rule.constraintValue);
                if (!std::regex_match(*value, regex)) {
                    valid = false;
                }
            }
            
            if (!valid) {
                errors.emplace_back(rule.errorMessage,
                                   ConfigErrorCategory::ConstraintViolation,
                                   ConfigErrorSeverity::Error,
                                   rule.key);
            }
        }
    }
    
    return errors;
}

std::vector<std::string> ConfigSchema::getRequiredKeys() const {
    std::vector<std::string> requiredKeys;
    for (const auto& rule : rules_) {
        if (rule.isRequired) {
            requiredKeys.push_back(rule.key);
        }
    }
    return requiredKeys;
}

std::string ConfigSchema::constraintTypeToString(ConstraintType type) {
    switch (type) {
        case ConstraintType::None: return "None";
        case ConstraintType::Min: return "Min";
        case ConstraintType::Max: return "Max";
        case ConstraintType::Enum: return "Enum";
        case ConstraintType::Regex: return "Regex";
        case ConstraintType::Length: return "Length";
        case ConstraintType::Custom: return "Custom";
        default: return "Unknown";
    }
}

bool ConfigSchema::matchesConstraint(const ConfigValue& value, const SchemaConstraint& constraint) {
    // Implementation placeholder
    return true;
}

// Config implementations
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

// Type-safe get methods
template<typename T>
std::optional<TypedConfigEntry<T>> Config::getTyped(const std::string& key) const {
    auto valueOpt = get<T>(key);
    if (!valueOpt) {
        return std::nullopt;
    }
    
    // Find the source and description
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return TypedConfigEntry<T>(key, *valueOpt, it->source, it->description);
    }
    
    return std::nullopt;
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
            } else if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << arg;
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
                oss << "[unknown type]";
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

// Type conversion utilities
std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, int>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << arg;
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
            oss << "[unknown type]";
        }
    }, value);
    return oss.str();
}

template<typename T>
std::optional<T> Config::convertToType(const ConfigValue& value) {
    try {
        return std::get<T>(value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
bool Config::canConvertToType(const ConfigValue& value) {
    try {
        std::get<T>(value);
        return true;
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

// Advanced querying capabilities
std::vector<std::string> Config::queryByPattern(const std::string& regex_pattern) const {
    std::vector<std::string> results;
    std::regex pattern(regex_pattern);
    
    for (const auto& entry : pImpl->entries) {
        if (std::regex_search(entry.key, pattern)) {
            results.push_back(entry.key);
        }
    }
    
    return results;
}

std::vector<std::string> Config::queryBySource(ConfigSource source_filter) const {
    std::vector<std::string> results;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source_filter) {
            results.push_back(entry.key);
        }
    }
    
    return results;
}

std::vector<std::string> Config::queryByValue(ConstraintType operation, const std::string& threshold) const {
    std::vector<std::string> results;
    
    // Value-based filtering implementation
    return results;
}

// Configuration tree navigation
std::string Config::configTreeNavigate(const std::string& path_separator) const {
    std::ostringstream oss;
    
    // Group keys by their path
    std::map<std::string, std::vector<std::string>> hierarchy;
    
    for (const auto& entry : pImpl->entries) {
        size_t pos = entry.key.find(path_separator);
        if (pos != std::string::npos) {
            std::string root_key = entry.key.substr(0, pos);
            hierarchy[root_key].push_back(entry.key);
        } else {
            hierarchy[entry.key].push_back(entry.key);
        }
    }
    
    oss << "Configuration Hierarchy:\n";
    for (const auto& [root, keys] : hierarchy) {
        oss << "  " << root << ":\n";
        for (const auto& key : keys) {
            if (key.find(path_separator) != std::string::npos) {
                std::string indent = "    ";
                size_t lastPos = 0;
                size_t nextPos = key.find(path_separator);
                while (nextPos != std::string::npos) {
                    std::string segment = key.substr(lastPos, nextPos - lastPos);
                    oss << indent << segment << ".\n";
                    indent += "    ";
                    lastPos = nextPos + path_separator.length();
                    nextPos = key.find(path_separator, lastPos);
                }
                oss << indent << key.substr(lastPos) << "\n";
            } else {
                oss << "    " << key << "\n";
            }
        }
    }
    
    return oss.str();
}

std::map<std::string, std::vector<std::string>> Config::getConfigHierarchy(const std::vector<std::string>& root_keys) const {
    std::map<std::string, std::vector<std::string>> hierarchy;
    
    for (const auto& root_key : root_keys) {
        std::vector<std::string> children;
        for (const auto& entry : pImpl->entries) {
            if (entry.key.find(root_key + ".") == 0) {
                children.push_back(entry.key);
            }
        }
        hierarchy[root_key] = children;
    }
    
    return hierarchy;
}

// Find similar configurations
std::vector<std::string> Config::findSimilarConfigurations(double threshold, const std::vector<std::string>& exclude_keys) const {
    std::vector<std::string> results;
    
    // Similarity implementation
    return results;
}

// Configuration statistics
std::string Config::getConfigStatistics(bool include_source_stats) const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    
    if (include_source_stats) {
        oss << "  By source:\n";
        std::map<ConfigSource, int> sourceCounts;
        for (const auto& entry : pImpl->entries) {
            sourceCounts[entry.source]++;
        }
        
        for (const auto& [source, count] : sourceCounts) {
            oss << "    " << static_cast<int>(source) << ": " << count << "\n";
        }
    }
    
    // Count value types
    oss << "  By type:\n";
    int intCount = 0, int64Count = 0, doubleCount = 0, boolCount = 0, stringCount = 0;
    int vectorIntCount = 0, vectorDoubleCount = 0, vectorStringCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) intCount++;
            else if constexpr (std::is_same_v<T, int64_t>) int64Count++;
            else if constexpr (std::is_same_v<T, double>) doubleCount++;
            else if constexpr (std::is_same_v<T, bool>) boolCount++;
            else if constexpr (std::is_same_v<T, std::string>) stringCount++;
            else if constexpr (std::is_same_v<T, std::vector<int>>) vectorIntCount++;
            else if constexpr (std::is_same_v<T, std::vector<double>>) vectorDoubleCount++;
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) vectorStringCount++;
        }, entry.value);
    }
    
    oss << "    int: " << intCount << "\n";
    oss << "    int64_t: " << int64Count << "\n";
    oss << "    double: " << doubleCount << "\n";
    oss << "    bool: " << boolCount << "\n";
    oss << "    string: " << stringCount << "\n";
    oss << "    vector<int>: " << vectorIntCount << "\n";
    oss << "    vector<double>: " << vectorDoubleCount << "\n";
    oss << "    vector<string>: " << vectorStringCount << "\n";
    
    return oss.str();
}

// Batch operations
void Config::batchSet(const std::map<std::string, ConfigValue>& config_dict, ConfigSource source) {
    for (const auto& [key, value] : config_dict) {
        set(key, value, source);
    }
}

void Config::batchUpdate(const std::vector<std::pair<std::string, ConfigValue>>& config_updates) {
    for (const auto& [key, value] : config_updates) {
        if (has(key)) {
            set(key, value, ConfigSource::Runtime);
        }
    }
}

void Config::batchRemove(const std::vector<std::string>& keys) {
    for (const auto& key : keys) {
        remove(key);
    }
}

bool Config::batchImport(const std::string& json_data, const std::string& merge_strategy) {
    try {
        nlohmann::json json_obj = nlohmann::json::parse(json_data);
        
        if (merge_strategy == "merge") {
            for (const auto& [key, value] : json_obj.items()) {
                auto configValue = jsonToConfigValue(value);
                if (configValue) {
                    set(key, *configValue, ConfigSource::File);
                }
            }
        } else if (merge_strategy == "replace") {
            clear();
            for (const auto& [key, value] : json_obj.items()) {
                auto configValue = jsonToConfigValue(value);
                if (configValue) {
                    set(key, *configValue, ConfigSource::File);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string Config::batchExport(bool include_sensitive) const {
    nlohmann::json json_obj;
    
    for (const auto& entry : pImpl->entries) {
        if (!include_sensitive && entry.key.find("password") != std::string::npos) {
            continue;
        }
        
        auto value = configValueToJson(entry.value);
        json_obj[entry.key] = value;
    }
    
    return json_obj.dump(2);
}

// Transaction support
ConfigTransaction Config::configTransaction() {
    return ConfigTransaction(this);
}

// Configuration presets
bool Config::applyConfigurationPreset(const std::string& preset_name) {
    if (preset_name == "development") {
        set("debug", true, ConfigSource::Runtime);
        set("log_level", "DEBUG", ConfigSource::Runtime);
        set("auto_reload", true, ConfigSource::Runtime);
        set("database_url", "localhost:5432", ConfigSource::Runtime);
        return true;
    } else if (preset_name == "production") {
        set("debug", false, ConfigSource::Runtime);
        set("log_level", "INFO", ConfigSource::Runtime);
        set("auto_reload", false, ConfigSource::Runtime);
        set("database_url", "prod-db:5432", ConfigSource::Runtime);
        return true;
    } else if (preset_name == "testing") {
        set("debug", true, ConfigSource::Runtime);
        set("log_level", "TRACE", ConfigSource::Runtime);
        set("auto_reload", true, ConfigSource::Runtime);
        set("database_url", ":memory:", ConfigSource::Runtime);
        return true;
    }
    
    return false;
}

// Configuration validation
bool Config::validateConfigurationSet(const std::map<std::string, ConfigValue>& config_dict) const {
    if (!schema_) return true; // No schema, validation passes
    
    std::vector<ConfigError> errors = schema_->validate(*this);
    return errors.empty();
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

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    nlohmann::json json_obj;
    std::visit([&json_obj](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) json_obj = arg;
        else if constexpr (std::is_same_v<T, int64_t>) json_obj = arg;
        else if constexpr (std::is_same_v<T, double>) json_obj = arg;
        else if constexpr (std::is_same_v<T, bool>) json_obj = arg;
        else if constexpr (std::is_same_v<T, std::string>) json_obj = arg;
        else if constexpr (std::is_same_v<T, std::vector<int>>) json_obj = arg;
        else if constexpr (std::is_same_v<T, std::vector<double>>) json_obj = arg;
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) json_obj = arg;
    }, value);
    return json_obj;
}

std::optional<ConfigValue> Config::jsonToConfigValue(const nlohmann::json& json) {
    ConfigValue value;
    
    if (json.is_number_integer()) {
        value = json.get<int>();
    } else if (json.is_number()) {
        value = json.get<double>();
    } else if (json.is_boolean()) {
        value = json.get<bool>();
    } else if (json.is_string()) {
        value = json.get<std::string>();
    } else if (json.is_array()) {
        // Determine vector type
        if (!json.empty()) {
            if (json[0].is_number_integer()) {
                std::vector<int> vec = json.get<std::vector<int>>();
                value = vec;
            } else if (json[0].is_number()) {
                std::vector<double> vec = json.get<std::vector<double>>();
                value = vec;
            } else if (json[0].is_string()) {
                std::vector<std::string> vec = json.get<std::vector<std::string>>();
                value = vec;
            }
        }
    }
    
    return std::optional<ConfigValue>(value);
}

// Configuration transaction implementation
namespace {
    struct ConfigTransaction::Impl {
        std::vector<ConfigTransaction::Operation> operations;
        std::unordered_map<std::string, ConfigValue> originalValues;
        bool rolledBack = false;
    };
}

ConfigTransaction::ConfigTransaction(Config* config) : config_(config), rolledBack_(false) {}

void ConfigTransaction::begin() {
    originalValues_.clear();
    for (const auto& entry : config_->pImpl->entries) {
        originalValues_[entry.key] = entry.value;
    }
    operations_.clear();
    rolledBack_ = false;
}

bool ConfigTransaction::commit() {
    if (operations_.empty()) return true;
    
    bool allSucceeded = true;
    for (const auto& op : operations_) {
        try {
            executeOperation(op);
        } catch (...) {
            allSucceeded = false;
            break;
        }
    }
    
    if (!allSucceeded) {
        rollback();
        return false;
    }
    
    return true;
}

void ConfigTransaction::rollback() {
    if (rolledBack_) return;
    
    for (const auto& [key, value] : originalValues_) {
        config_->set(key, value, ConfigSource::Runtime);
    }
    
    operations_.clear();
    rolledBack_ = true;
}

ConfigTransaction& ConfigTransaction::set(const std::string& key, const ConfigValue& value) {
    operations_.push_back({OperationType::Set, key, value});
    return *this;
}

ConfigTransaction& ConfigTransaction::remove(const std::string& key) {
    operations_.push_back({OperationType::Remove, key, ConfigValue()});
    return *this;
}

void ConfigTransaction::executeOperation(const Operation& op) {
    switch (op.type) {
        case OperationType::Set:
            config_->set(op.key, op.value, ConfigSource::Runtime);
            break;
        case OperationType::Remove:
            config_->remove(op.key);
            break;
    }
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

// TypedConfigEntry template instantiations
template class TypedConfigEntry<int>;
template class TypedConfigEntry<int64_t>;
template class TypedConfigEntry<double>;
template class TypedConfigEntry<bool>;
template class TypedConfigEntry<std::string>;
template class TypedConfigEntry<std::vector<int>>;
template class TypedConfigEntry<std::vector<double>>;
template class TypedConfigEntry<std::vector<std::string>>;

} // namespace nlm