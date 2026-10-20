#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    // Schema management
    std::unordered_map<std::string, std::shared_ptr<ConfigSchema>> schemas;
    
    // Configuration entries by category
    std::unordered_map<std::string, std::vector<ConfigEntry>> categoryEntries;
    
    // History tracking
    std::vector<ConfigChange> history;
    bool historyEnabled = false;
    
    // Templates
    std::unordered_map<std::string, std::unordered_map<std::string, TemplateMetadata>> templates;
    
    // Snapshots
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<ConfigEntry>>> snapshots;
    
    // Migration functions
    std::unordered_map<std::string, std::unordered_map<std::string, std::function<void(Config&)>>> migrations;
    
    // Legacy support
    std::vector<ConfigEntry> legacyEntries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Create default schema for backward compatibility
    auto defaultSchema = std::make_shared<ConfigSchema>("legacy", "Default schema for backward compatibility");
    pImpl->schemas["legacy"] = defaultSchema;
    
    // Add legacy parameters based on default.cfg
    ParameterMetadata param1("random_seed", "int", "Random seed for reproducible experiments", false, std::any(42), "Random seed value (0-2147483647)");
    param1.hasMin = true; param1.minValue = 0;
    defaultSchema->addParameter(param1);
    
    ParameterMetadata param2("simulation_timestep", "double", "Simulation timestep in seconds", false, std::any(0.001), "Simulation timestep in seconds (0.0001-1.0)");
    param2.hasMin = true; param2.minValue = 0.0001;
    param2.hasMax = true; param2.maxValue = 1.0;
    defaultSchema->addParameter(param2);
    
    ParameterMetadata param3("neuron_count", "int", "Number of neurons in the brain", false, std::any(1000), "Number of neurons (1-1000000)");
    param3.hasMin = true; param3.minValue = 1;
    param3.hasMax = true; param3.maxValue = 1000000;
    defaultSchema->addParameter(param3);
    
    ParameterMetadata param4("region_count", "int", "Number of brain regions", false, std::any(2), "Number of brain regions (1-100)");
    param4.hasMin = true; param4.minValue = 1;
    param4.hasMax = true; param4.maxValue = 100;
    defaultSchema->addParameter(param4);
    
    ParameterMetadata param5("connection_probability", "double", "Connection probability for random connectivity", false, std::any(0.1), "Connection probability (0.0-1.0)");
    param5.hasMin = true; param5.minValue = 0.0;
    param5.hasMax = true; param5.maxValue = 1.0;
    defaultSchema->addParameter(param5);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void ConfigSchema::addParameter(const ParameterMetadata& param) {
    parameters[param.name] = param;
}

bool ConfigSchema::hasParameter(const std::string& name) const {
    return parameters.find(name) != parameters.end();
}

const ParameterMetadata* ConfigSchema::getParameter(const std::string& name) const {
    auto it = parameters.find(name);
    if (it != parameters.end()) {
        return &it->second;
    }
    return nullptr;
}

void ConfigSchema::setParent(const std::shared_ptr<ConfigSchema>& parentSchema) {
    parent = parentSchema;
}

void ConfigSchema::addChild(const std::shared_ptr<ConfigSchema>& childSchema) {
    children.push_back(childSchema);
    childSchema->setParent(shared_from_this());
}

ValidationResult ConfigSchema::validateParameter(const std::string& name, const std::any& value) const {
    const ParameterMetadata* param = getParameter(name);
    if (!param) {
        return ValidationResult(false, "Parameter not found in schema");
    }
    
    // Get all values including parent schema values
    std::map<std::string, std::any> allValues;
    if (parent) {
        // For simplicity, we're validating against schema parameters, not actual values
        // In a full implementation, we would get all values first
    }
    
    ValidationResult result(true, "Valid");
    
    // Type validation
    if (param->type == "int") {
        if (!std::any_cast<int>(&value)) {
            result = ValidationResult(false, "Invalid type: expected int");
        }
    } else if (param->type == "double") {
        if (!std::any_cast<double>(&value)) {
            result = ValidationResult(false, "Invalid type: expected double");
        }
    } else if (param->type == "bool") {
        if (!std::any_cast<bool>(&value)) {
            result = ValidationResult(false, "Invalid type: expected bool");
        }
    } else if (param->type == "string") {
        if (!std::any_cast<std::string>(&value)) {
            result = ValidationResult(false, "Invalid type: expected string");
        }
    } else if (param->type == "vector<int>") {
        if (!std::any_cast<std::vector<int>>(&value)) {
            result = ValidationResult(false, "Invalid type: expected vector<int>");
        }
    } else if (param->type == "vector<double>") {
        if (!std::any_cast<std::vector<double>>(&value)) {
            result = ValidationResult(false, "Invalid type: expected vector<double>");
        }
    } else if (param->type == "vector<string>") {
        if (!std::any_cast<std::vector<std::string>>(&value)) {
            result = ValidationResult(false, "Invalid type: expected vector<string>");
        }
    }
    
    // Range validation
    if (result.valid) {
        if (param->type == "int" || param->type == "double") {
            double val = param->type == "int" ? std::any_cast<int>(value) : std::any_cast<double>(value);
            if (param->hasMin && val < param->minValue) {
                result = ValidationResult(false, "Value below minimum: " + std::to_string(val) + " < " + std::to_string(param->minValue));
            }
            if (param->hasMax && val > param->maxValue) {
                result = ValidationResult(false, "Value above maximum: " + std::to_string(val) + " > " + std::to_string(param->maxValue));
            }
        }
    }
    
    // Format validation
    if (result.valid && !param->pattern.empty()) {
        std::string strValue;
        if (param->type == "int") strValue = std::to_string(std::any_cast<int>(value));
        else if (param->type == "double") strValue = std::to_string(std::any_cast<double>(value));
        else if (param->type == "bool") strValue = std::any_cast<bool>(value) ? "true" : "false";
        else if (param->type == "string") strValue = std::any_cast<std::string>(value);
        else {
            result = ValidationResult(false, "Cannot validate pattern for type: " + param->type);
        }
        
        if (result.valid) {
            std::regex pattern(param->pattern);
            if (!std::regex_match(strValue, pattern)) {
                result = ValidationResult(false, "Value does not match pattern: " + param->pattern);
            }
        }
    }
    
    return result;
}

SchemaValidationResult ConfigSchema::validate(const std::map<std::string, std::any>& values) const {
    SchemaValidationResult result;
    
    // Get all parameters from this schema and parent schemas
    std::map<std::string, ParameterMetadata> allParams = getAllParameters();
    
    for (const auto& [name, param] : allParams) {
        // Check if parameter exists in values
        if (values.find(name) == values.end()) {
            if (param.required) {
                result.addError("Required parameter \"" + name + "\" is missing");
            }
            continue;
        }
        
        const auto& value = values.at(name);
        
        // Validate parameter
        ValidationResult paramResult = validateParameter(name, value);
        if (!paramResult.valid) {
            result.addParameterError(name, paramResult.message);
        }
        
        // Check conditional dependencies
        for (const auto& dep : param.dependsOn) {
            if (values.find(dep) == values.end()) {
                result.addParameterError(name, "Parameter depends on missing parameter: " + dep);
            }
        }
    }
    
    // Apply cross-parameter validation
    for (const auto& [name, param] : allParams) {
        if (param.crossValidator) {
            ValidationResult crossResult = param.crossValidator(values);
            if (!crossResult.valid) {
                result.addParameterError(name, crossResult.message);
            }
        }
    }
    
    return result;
}

void ConfigSchema::addTemplate(const std::string& name, const std::map<std::string, std::any>& values) {
    templates[name] = values;
}

bool ConfigSchema::merge(const ConfigSchema& other) {
    // For simplicity, we'll just merge parameters, avoiding duplicates
    for (const auto& [name, param] : other.parameters) {
        if (parameters.find(name) == parameters.end()) {
            parameters[name] = param;
        }
    }
    return true;
}

std::map<std::string, ParameterMetadata> ConfigSchema::getAllParameters() const {
    std::map<std::string, ParameterMetadata> allParams = parameters;
    
    if (parent) {
        auto parentParams = parent->getAllParameters();
        for (const auto& [name, param] : parentParams) {
            if (allParams.find(name) == allParams.end()) {
                allParams[name] = param;
            }
        }
    }
    
    return allParams;
}

void Config::registerSchema(const std::string& category, const std::shared_ptr<ConfigSchema>& schema) {
    pImpl->schemas[category] = schema;
}

const std::shared_ptr<ConfigSchema>& Config::getSchema(const std::string& category) const {
    auto it = pImpl->schemas.find(category);
    if (it != pImpl->schemas.end()) {
        return it->second;
    }
    static std::shared_ptr<ConfigSchema> empty;
    return empty;
}

bool Config::hasSchema(const std::string& category) const {
    return pImpl->schemas.find(category) != pImpl->schemas.end();
}

std::vector<std::string> Config::getSchemaCategories() const {
    std::vector<std::string> categories;
    for (const auto& [category, schema] : pImpl->schemas) {
        categories.push_back(category);
    }
    return categories;
}

bool Config::loadFromFile(const std::string& filepath, const std::string& category) {
    ConfigSource source = ConfigSource::File;
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    // Detect file format
    FileFormat format = detectFileFormat(filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, use simple key=value format for backward compatibility
    // TODO: Implement proper JSON, YAML, INI, TOML parsers
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to convert to appropriate type
            ConfigValue configValue;
            try {
                // Try int first
                try {
                    configValue = std::stoi(value);
                } catch (...) {
                    // Try double
                    try {
                        configValue = std::stod(value);
                    } catch (...) {
                        // Try bool
                        if (value == "true") {
                            configValue = true;
                        } else if (value == "false") {
                            configValue = false;
                        } else {
                            configValue = value;
                        }
                    }
                }
            } catch (...) {
                configValue = value;
            }
            
            set(key, configValue, source, "Loaded from file: " + filepath, actualCategory);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath, const std::string& category) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    // Get entries for the specified category
    const auto& entries = pImpl->categoryEntries.find(actualCategory);
    if (entries == pImpl->categoryEntries.end()) {
        return false;
    }
    
    for (const auto& entry : entries->second) {
        file << "# " << entry.description << "\n";
        
        std::visit([&file, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_integral_v<T>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_floating_point_v<T>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n";
            }
        }, entry.value);
    }
    
    return true;
}

bool Config::loadFromSchemaFile(const std::string& filepath, const std::string& category) {
    // For now, use the same implementation as loadFromFile
    // TODO: Implement schema-aware file loading
    return loadFromFile(filepath, category);
}

bool Config::saveToSchemaFile(const std::string& filepath, const std::string& category) const {
    // For now, use the same implementation as saveToFile
    // TODO: Implement schema-aware file saving
    return saveToFile(filepath, category);
}

SchemaValidationResult Config::validateSchema(const std::string& category) const {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    // Get schema
    const auto& schema = getSchema(actualCategory);
    if (!schema) {
        return SchemaValidationResult(false, {"Schema not found: " + actualCategory});
    }
    
    // Convert current entries to map<std::string, std::any>
    std::map<std::string, std::any> values;
    const auto& entries = pImpl->categoryEntries.find(actualCategory);
    if (entries != pImpl->categoryEntries.end()) {
        for (const auto& entry : entries->second) {
            // Convert ConfigValue to std::any
            std::visit([&values, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                values[entry.key] = static_cast<std::any>(arg);
            }, entry.value);
        }
    }
    
    return schema->validate(values);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, 
                 const std::string& reason, const std::string& category) {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    // Determine the actual value to store based on type
    auto it = pImpl->categoryEntries.find(actualCategory);
    if (it == pImpl->categoryEntries.end()) {
        // Create category if it doesn't exist
        pImpl->categoryEntries[actualCategory] = std::vector<ConfigEntry>();
        it = pImpl->categoryEntries.find(actualCategory);
    }
    
    std::vector<ConfigEntry>& entries = it->second;
    
    // Find existing entry
    auto entryIt = std::find_if(entries.begin(), entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    // Get old value for history
    std::any oldValue;
    bool hasOldValue = false;
    if (entryIt != entries.end()) {
        hasOldValue = true;
        // Convert ConfigValue to std::any
        std::visit([&oldValue](auto&& arg) {
            oldValue = static_cast<std::any>(arg);
        }, entryIt->value);
    }
    
    // Set new value
    ConfigEntry newEntry(key, value, source, "");
    
    if (entryIt != entries.end()) {
        *entryIt = newEntry;
    } else {
        entries.push_back(newEntry);
    }
    
    // Add to legacy entries for backward compatibility
    if (actualCategory == "legacy") {
        auto legacyIt = std::find_if(legacyEntries.begin(), legacyEntries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (legacyIt != legacyEntries.end()) {
            *legacyIt = newEntry;
        } else {
            legacyEntries.push_back(newEntry);
        }
    }
    
    // Add to history if enabled
    if (pImpl->historyEnabled && hasOldValue) {
        ConfigChange change(key, oldValue, value, source, reason);
        pImpl->history.push_back(change);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source,
                 const std::string& reason, const std::string& category) {
    set(key, ConfigValue(value), source, reason, category);
}

void Config::set(const std::string& key, int value, ConfigSource source,
                 const std::string& reason, const std::string& category) {
    set(key, ConfigValue(value), source, reason, category);
}

void Config::set(const std::string& key, double value, ConfigSource source,
                 const std::string& reason, const std::string& category) {
    set(key, ConfigValue(value), source, reason, category);
}

void Config::set(const std::string& key, bool value, ConfigSource source,
                 const std::string& reason, const std::string& category) {
    set(key, ConfigValue(value), source, reason, category);
}

template<typename T>
std::optional<T> Config::get(const std::string& key, const std::string& category) const {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    // Get entries for the specified category
    const auto& entries = pImpl->categoryEntries.find(actualCategory);
    if (entries == pImpl->categoryEntries.end()) {
        return std::nullopt;
    }
    
    const std::vector<ConfigEntry>& catEntries = entries->second;
    auto it = std::find_if(catEntries.begin(), catEntries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == catEntries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue, const std::string& category) const {
    auto val = get<T>(key, category);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::has(const std::string& key) const {
    return has(key, "legacy");
}

bool Config::has(const std::string& key, const std::string& category) const {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    const auto& entries = pImpl->categoryEntries.find(actualCategory);
    if (entries == pImpl->categoryEntries.end()) {
        return false;
    }
    
    return std::any_of(entries->second.begin(), entries->second.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    remove(key, "legacy");
}

void Config::remove(const std::string& key, const std::string& category) {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    auto it = pImpl->categoryEntries.find(actualCategory);
    if (it != pImpl->categoryEntries.end()) {
        auto& entries = it->second;
        entries.erase(
            std::remove_if(entries.begin(), entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            entries.end()
        );
    }
}

std::vector<std::string> Config::getKeys() const {
    return getKeys("legacy");
}

std::vector<std::string> Config::getKeys(const std::string& category) const {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    std::vector<std::string> keys;
    
    const auto& entries = pImpl->categoryEntries.find(actualCategory);
    if (entries != pImpl->categoryEntries.end()) {
        const std::vector<ConfigEntry>& catEntries = entries->second;
        keys.reserve(catEntries.size());
        for (const auto& entry : catEntries) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

std::vector<std::pair<std::string, ConfigEntry>> Config::getAllEntries() const {
    return getAllEntries("legacy");
}

std::vector<std::pair<std::string, ConfigEntry>> Config::getAllEntries(const std::string& category) const {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    std::vector<std::pair<std::string, ConfigEntry>> allEntries;
    
    const auto& entries = pImpl->categoryEntries.find(actualCategory);
    if (entries != pImpl->categoryEntries.end()) {
        const std::vector<ConfigEntry>& catEntries = entries->second;
        allEntries.reserve(catEntries.size());
        for (const auto& entry : catEntries) {
            allEntries.push_back(std::make_pair(actualCategory + ":" + entry.key, entry));
        }
    }
    
    return allEntries;
}

void Config::clear() {
    clear("legacy");
}

void Config::clear(const std::string& category) {
    std::string actualCategory = category.empty() ? "legacy" : category;
    pImpl->categoryEntries.erase(actualCategory);
}

void Config::enableHistory(bool enabled) {
    pImpl->historyEnabled = enabled;
    if (!enabled) {
        pImpl->history.clear();
    }
}

bool Config::isHistoryEnabled() const {
    return pImpl->historyEnabled;
}

std::vector<ConfigChange> Config::getHistory() const {
    return pImpl->history;
}

void Config::clearHistory() {
    pImpl->history.clear();
}

std::vector<ConfigChange> Config::getHistory(const std::string& key) const {
    std::vector<ConfigChange> keyHistory;
    for (const auto& change : pImpl->history) {
        if (change.key == key) {
            keyHistory.push_back(change);
        }
    }
    return keyHistory;
}

std::map<std::string, std::pair<std::any, std::any>> Config::diff(const Config& other) const {
    std::map<std::string, std::pair<std::any, std::any>> differences;
    
    // Get all keys from both configs
    std::set<std::string> allKeys;
    for (const auto& [category, entries] : pImpl->categoryEntries) {
        for (const auto& entry : entries) {
            allKeys.insert(category + ":" + entry.key);
        }
    }
    
    for (const auto& otherImpl : other.pImpl->categoryEntries) {
        for (const auto& entry : otherImpl.second) {
            allKeys.insert(otherImpl.first + ":" + entry.key);
        }
    }
    
    for (const auto& key : allKeys) {
        // Parse key to get category and parameter name
        size_t colonPos = key.find(':');
        if (colonPos == std::string::npos) continue;
        
        std::string category = key.substr(0, colonPos);
        std::string paramName = key.substr(colonPos + 1);
        
        // Get value from current config
        std::any currentValue;
        bool currentHasValue = false;
        
        auto currentCatIt = pImpl->categoryEntries.find(category);
        if (currentCatIt != pImpl->categoryEntries.end()) {
            for (const auto& entry : currentCatIt->second) {
                if (entry.key == paramName) {
                    currentValue = entry.value;
                    currentHasValue = true;
                    break;
                }
            }
        }
        
        // Get value from other config
        std::any otherValue;
        bool otherHasValue = false;
        
        auto otherCatIt = other.pImpl->categoryEntries.find(category);
        if (otherCatIt != other.pImpl->categoryEntries.end()) {
            for (const auto& entry : otherCatIt->second) {
                if (entry.key == paramName) {
                    otherValue = entry.value;
                    otherHasValue = true;
                    break;
                }
            }
        }
        
        // Check if values differ
        if (currentHasValue != otherHasValue || 
            (currentHasValue && otherHasValue && currentValue != otherValue)) {
            differences[key] = std::make_pair(currentValue, otherValue);
        }
    }
    
    return differences;
}

std::string Config::diffToString(const Config& other) const {
    std::map<std::string, std::pair<std::any, std::any>> differences = diff(other);
    
    std::ostringstream oss;
    oss << "Configuration differences (" << differences.size() << "):\n";
    
    if (differences.empty()) {
        oss << "  No differences found.\n";
    } else {
        for (const auto& [key, values] : differences) {
            oss << "  " << key << ":\n";
            oss << "    Current: ";
            if (values.first.has_value()) {
                oss << std::any_cast<std::string>(values.first);
            } else {
                oss << "<unset>";
            }
            oss << "\n";
            oss << "    Other: ";
            if (values.second.has_value()) {
                oss << std::any_cast<std::string>(values.second);
            } else {
                oss << "<unset>";
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::string Config::createSnapshot(const std::string& name) {
    std::string snapshotName = name.empty() ? "snapshot_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) : name;
    
    for (const auto& [category, entries] : pImpl->categoryEntries) {
        pImpl->snapshots[snapshotName][category] = entries;
    }
    
    return snapshotName;
}

bool Config::rollbackToSnapshot(const std::string& name) {
    auto it = pImpl->snapshots.find(name);
    if (it == pImpl->snapshots.end()) {
        return false;
    }
    
    // Clear current config and restore from snapshot
    clear();
    
    for (const auto& [category, entries] : it->second) {
        pImpl->categoryEntries[category] = entries;
    }
    
    return true;
}

std::vector<std::string> Config::getSnapshotNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : pImpl->snapshots) {
        names.push_back(name);
    }
    return names;
}

void Config::addMigration(const std::string& fromVersion, const std::string& toVersion,
                           const std::function<void(Config&)>& migrationFunc) {
    pImpl->migrations[fromVersion][toVersion] = migrationFunc;
}

bool Config::migrate(const std::string& fromVersion, const std::string& toVersion) {
    auto fromIt = pImpl->migrations.find(fromVersion);
    if (fromIt == pImpl->migrations.end()) {
        return false;
    }
    
    auto toIt = fromIt->second.find(toVersion);
    if (toIt == fromIt->second.end()) {
        return false;
    }
    
    toIt->second(*this);
    return true;
}

void Config::applyTemplate(const std::string& templateName, const std::string& category) {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    // For now, use the legacy entries for templates
    // In a full implementation, templates would be stored in the template system
    if (actualCategory == "legacy") {
        // Example template application
        if (templateName == "basic") {
            set("random_seed", 42, ConfigSource::Default);
            set("simulation_timestep", 0.001, ConfigSource::Default);
            set("neuron_count", 1000, ConfigSource::Default);
            set("region_count", 2, ConfigSource::Default);
        } else if (templateName == "debug") {
            set("log_level", "DEBUG", ConfigSource::Default);
            set("log_to_file", true, ConfigSource::Default);
            set("visualization_enabled", true, ConfigSource::Default);
        }
    }
}

void Config::addTemplate(const std::string& name, const std::map<std::string, std::any>& parameters,
                         const std::string& description, const std::string& category) {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    TemplateMetadata templateMeta;
    templateMeta.name = name;
    templateMeta.description = description;
    templateMeta.parameters = parameters;
    
    pImpl->templates[actualCategory][name] = templateMeta;
}

std::vector<std::string> Config::getAvailableTemplates(const std::string& category) const {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    std::vector<std::string> templates;
    auto it = pImpl->templates.find(actualCategory);
    if (it != pImpl->templates.end()) {
        for (const auto& [name, _] : it->second) {
            templates.push_back(name);
        }
    }
    
    return templates;
}

void Config::merge(const Config& other, const std::string& category) {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    const auto& otherEntries = other.pImpl->categoryEntries.find(actualCategory);
    if (otherEntries == other.pImpl->categoryEntries.end()) {
        return;
    }
    
    auto currentIt = pImpl->categoryEntries.find(actualCategory);
    if (currentIt == pImpl->categoryEntries.end()) {
        // Create category if it doesn't exist
        pImpl->categoryEntries[actualCategory] = otherEntries->second;
        return;
    }
    
    // Merge other entries into current
    auto& currentEntries = currentIt->second;
    
    for (const auto& otherEntry : otherEntries->second) {
        auto currentIt = std::find_if(currentEntries.begin(), currentEntries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });
        
        if (currentIt != currentEntries.end()) {
            // Update existing entry with source information
            currentIt->value = otherEntry.value;
            currentIt->source = otherEntry.source;
        } else {
            currentEntries.push_back(otherEntry);
        }
    }
}

void Config::mergeFromFile(const std::string& filepath, const std::string& category) {
    Config tempConfig;
    if (tempConfig.loadFromFile(filepath, category)) {
        merge(tempConfig, category);
    }
}

Config::FileFormat Config::detectFileFormat(const std::string& filepath) {
    std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") return FileFormat::JSON;
    if (extension == "yaml" || extension == "yml") return FileFormat::YAML;
    if (extension == "ini" || extension == "cfg" || extension == "conf") return FileFormat::INI;
    if (extension == "toml") return FileFormat::TOML;
    
    return FileFormat::Auto;
}

std::string Config::getCategory() const {
    return "legacy";
}

std::string Config::summary() const {
    return summary("legacy");
}

std::string Config::summary(const std::string& category) const {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    std::ostringstream oss;
    oss << "Configuration (" << actualCategory << ") (" << getKeys(actualCategory).size() << " entries):\n";
    
    for (const auto& entry : getAllEntries(actualCategory)) {
        oss << "  " << entry.first << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.second.value);
        oss << "] (" << static_cast<int>(entry.second.source) << ")\n";
    }
    
    // Show history if enabled
    if (pImpl->historyEnabled && !pImpl->history.empty()) {
        oss << "\nRecent changes (" << pImpl->history.size() << "):\n";
        int count = std::min(5, static_cast<int>(pImpl->history.size()));
        for (int i = 0; i < count; ++i) {
            const auto& change = pImpl->history[pImpl->history.size() - 1 - i];
            oss << "  " << change.timestamp << ": " << change.key << " -> " << change.reason << "\n";
        }
    }
    
    return oss.str();
}

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <iomanip>
#include <rapidjson/document.h>
#include <yaml-cpp/yaml.h>
#include <inih/ini.h>
#include <toml.hpp>

namespace nlm {

struct Config::Impl {
    // Schema management
    std::unordered_map<std::string, std::shared_ptr<ConfigSchema>> schemas;
    
    // Configuration entries by category
    std::unordered_map<std::string, std::vector<ConfigEntry>> categoryEntries;
    
    // History tracking
    std::vector<ConfigChange> history;
    bool historyEnabled = false;
    
    // Templates
    std::unordered_map<std::string, std::unordered_map<std::string, TemplateMetadata>> templates;
    
    // Snapshots
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<ConfigEntry>>> snapshots;
    
    // Migration functions
    std::unordered_map<std::string, std::unordered_map<std::string, std::function<void(Config&)>>> migrations;
    
    // Legacy support
    std::vector<ConfigEntry> legacyEntries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Create default schema for backward compatibility
    auto defaultSchema = std::make_shared<ConfigSchema>("legacy", "Default schema for backward compatibility");
    pImpl->schemas["legacy"] = defaultSchema;
    
    // Add legacy parameters based on default.cfg
    ParameterMetadata param1("random_seed", "int", "Random seed for reproducible experiments", false, std::any(42), "Random seed value (0-2147483647)");
    param1.hasMin = true; param1.minValue = 0;
    defaultSchema->addParameter(param1);
    
    ParameterMetadata param2("simulation_timestep", "double", "Simulation timestep in seconds", false, std::any(0.001), "Simulation timestep in seconds (0.0001-1.0)");
    param2.hasMin = true; param2.minValue = 0.0001;
    param2.hasMax = true; param2.maxValue = 1.0;
    defaultSchema->addParameter(param2);
    
    ParameterMetadata param3("neuron_count", "int", "Number of neurons in the brain", false, std::any(1000), "Number of neurons (1-1000000)");
    param3.hasMin = true; param3.minValue = 1;
    param3.hasMax = true; param3.maxValue = 1000000;
    defaultSchema->addParameter(param3);
    
    ParameterMetadata param4("region_count", "int", "Number of brain regions", false, std::any(2), "Number of brain regions (1-100)");
    param4.hasMin = true; param4.minValue = 1;
    param4.hasMax = true; param4.maxValue = 100;
    defaultSchema->addParameter(param4);
    
    ParameterMetadata param5("connection_probability", "double", "Connection probability for random connectivity", false, std::any(0.1), "Connection probability (0.0-1.0)");
    param5.hasMin = true; param5.minValue = 0.0;
    param5.hasMax = true; param5.maxValue = 1.0;
    defaultSchema->addParameter(param5);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void ConfigSchema::addParameter(const ParameterMetadata& param) {
    parameters[param.name] = param;
}

bool ConfigSchema::hasParameter(const std::string& name) const {
    return parameters.find(name) != parameters.end();
}

const ParameterMetadata* ConfigSchema::getParameter(const std::string& name) const {
    auto it = parameters.find(name);
    if (it != parameters.end()) {
        return &it->second;
    }
    return nullptr;
}

void ConfigSchema::setParent(const std::shared_ptr<ConfigSchema>& parentSchema) {
    parent = parentSchema;
}

void ConfigSchema::addChild(const std::shared_ptr<ConfigSchema>& childSchema) {
    children.push_back(childSchema);
    childSchema->setParent(shared_from_this());
}

ValidationResult ConfigSchema::validateParameter(const std::string& name, const std::any& value) const {
    const ParameterMetadata* param = getParameter(name);
    if (!param) {
        return ValidationResult(false, "Parameter not found in schema");
    }
    
    // Get all values including parent schema values
    std::map<std::string, std::any> allValues;
    if (parent) {
        // For simplicity, we're validating against schema parameters, not actual values
        // In a full implementation, we would get all values first
    }
    
    ValidationResult result(true, "Valid");
    
    // Type validation
    if (param->type == "int") {
        if (!std::any_cast<int>(&value)) {
            result = ValidationResult(false, "Invalid type: expected int");
        }
    } else if (param->type == "double") {
        if (!std::any_cast<double>(&value)) {
            result = ValidationResult(false, "Invalid type: expected double");
        }
    } else if (param->type == "bool") {
        if (!std::any_cast<bool>(&value)) {
            result = ValidationResult(false, "Invalid type: expected bool");
        }
    } else if (param->type == "string") {
        if (!std::any_cast<std::string>(&value)) {
            result = ValidationResult(false, "Invalid type: expected string");
        }
    } else if (param->type == "vector<int>") {
        if (!std::any_cast<std::vector<int>>(&value)) {
            result = ValidationResult(false, "Invalid type: expected vector<int>");
        }
    } else if (param->type == "vector<double>") {
        if (!std::any_cast<std::vector<double>>(&value)) {
            result = ValidationResult(false, "Invalid type: expected vector<double>");
        }
    } else if (param->type == "vector<string>") {
        if (!std::any_cast<std::vector<std::string>>(&value)) {
            result = ValidationResult(false, "Invalid type: expected vector<string>");
        }
    }
    
    // Range validation
    if (result.valid) {
        if (param->type == "int" || param->type == "double") {
            double val = param->type == "int" ? std::any_cast<int>(value) : std::any_cast<double>(value);
            if (param->hasMin && val < param->minValue) {
                result = ValidationResult(false, "Value below minimum: " + std::to_string(val) + " < " + std::to_string(param->minValue));
            }
            if (param->hasMax && val > param->maxValue) {
                result = ValidationResult(false, "Value above maximum: " + std::to_string(val) + " > " + std::to_string(param->maxValue));
            }
        }
    }
    
    // Format validation
    if (result.valid && !param->pattern.empty()) {
        std::string strValue;
        if (param->type == "int") strValue = std::to_string(std::any_cast<int>(value));
        else if (param->type == "double") strValue = std::to_string(std::any_cast<double>(value));
        else if (param->type == "bool") strValue = std::any_cast<bool>(value) ? "true" : "false";
        else if (param->type == "string") strValue = std::any_cast<std::string>(value);
        else {
            result = ValidationResult(false, "Cannot validate pattern for type: " + param->type);
        }
        
        if (result.valid) {
            std::regex pattern(param->pattern);
            if (!std::regex_match(strValue, pattern)) {
                result = ValidationResult(false, "Value does not match pattern: " + param->pattern);
            }
        }
    }
    
    return result;
}

SchemaValidationResult ConfigSchema::validate(const std::map<std::string, std::any>& values) const {
    SchemaValidationResult result;
    
    // Get all parameters from this schema and parent schemas
    std::map<std::string, ParameterMetadata> allParams = getAllParameters();
    
    for (const auto& [name, param] : allParams) {
        // Check if parameter exists in values
        if (values.find(name) == values.end()) {
            if (param.required) {
                result.addError("Required parameter \"" + name + "\" is missing");
            }
            continue;
        }
        
        const auto& value = values.at(name);
        
        // Validate parameter
        ValidationResult paramResult = validateParameter(name, value);
        if (!paramResult.valid) {
            result.addParameterError(name, paramResult.message);
        }
        
        // Check conditional dependencies
        for (const auto& dep : param.dependsOn) {
            if (values.find(dep) == values.end()) {
                result.addParameterError(name, "Parameter depends on missing parameter: " + dep);
            }
        }
    }
    
    // Apply cross-parameter validation
    for (const auto& [name, param] : allParams) {
        if (param.crossValidator) {
            ValidationResult crossResult = param.crossValidator(values);
            if (!crossResult.valid) {
                result.addParameterError(name, crossResult.message);
            }
        }
    }
    
    return result;
}

void ConfigSchema::addTemplate(const std::string& name, const std::map<std::string, std::any>& values) {
    templates[name] = values;
}

bool ConfigSchema::merge(const ConfigSchema& other) {
    // For simplicity, we'll just merge parameters, avoiding duplicates
    for (const auto& [name, param] : other.parameters) {
        if (parameters.find(name) == parameters.end()) {
            parameters[name] = param;
        }
    }
    return true;
}

std::map<std::string, ParameterMetadata> ConfigSchema::getAllParameters() const {
    std::map<std::string, ParameterMetadata> allParams = parameters;
    
    if (parent) {
        auto parentParams = parent->getAllParameters();
        for (const auto& [name, param] : parentParams) {
            if (allParams.find(name) == allParams.end()) {
                allParams[name] = param;
            }
        }
    }
    
    return allParams;
}

void Config::registerSchema(const std::string& category, const std::shared_ptr<ConfigSchema>& schema) {
    pImpl->schemas[category] = schema;
}

const std::shared_ptr<ConfigSchema>& Config::getSchema(const std::string& category) const {
    auto it = pImpl->schemas.find(category);
    if (it != pImpl->schemas.end()) {
        return it->second;
    }
    static std::shared_ptr<ConfigSchema> empty;
    return empty;
}

bool Config::hasSchema(const std::string& category) const {
    return pImpl->schemas.find(category) != pImpl->schemas.end();
}

std::vector<std::string> Config::getSchemaCategories() const {
    std::vector<std::string> categories;
    for (const auto& [category, schema] : pImpl->schemas) {
        categories.push_back(category);
    }
    return categories;
}

// JSON parsing implementation
std::map<std::string, std::any> Config::parseJSON(const std::string& content) {
    std::map<std::string, std::any> result;
    rapidjson::Document doc;
    
    if (doc.Parse(content.c_str()).HasParseError()) {
        throw std::runtime_error("Failed to parse JSON content");
    }
    
    // Convert JSON to std::map<std::string, std::any>
    if (doc.IsObject()) {
        for (auto it = doc.MemberBegin(); it != doc.MemberEnd(); ++it) {
            const std::string& key = it->name.GetString();
            
            if (it->value.IsInt()) {
                result[key] = it->value.GetInt();
            } else if (it->value.IsInt64()) {
                result[key] = it->value.GetInt64();
            } else if (it->value.IsDouble()) {
                result[key] = it->value.GetDouble();
            } else if (it->value.IsBool()) {
                result[key] = it->value.GetBool();
            } else if (it->value.IsString()) {
                result[key] = std::string(it->value.GetString());
            } else if (it->value.IsArray()) {
                // Handle arrays
                std::vector<int> intArray;
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                
                bool allInt = true, allDouble = true, allString = true;
                
                for (const auto& item : it->value.GetArray()) {
                    if (item.IsInt()) {
                        intArray.push_back(item.GetInt());
                        if (!allDouble && !allString) allInt = true;
                    } else if (item.IsDouble()) {
                        doubleArray.push_back(item.GetDouble());
                        if (!allInt && !allString) allDouble = true;
                    } else if (item.IsString()) {
                        stringArray.push_back(std::string(item.GetString()));
                        if (!allInt && !allDouble) allString = true;
                    } else {
                        // Mixed types or other types, fall back to vector<string>
                        allInt = allDouble = allString = false;
                        break;
                    }
                }
                
                if (allInt) {
                    result[key] = intArray;
                } else if (allDouble) {
                    result[key] = doubleArray;
                } else if (allString) {
                    result[key] = stringArray;
                } else {
                    // Mixed array, store as vector<string> (JSON doesn't support mixed-type arrays well)
                    std::vector<std::string> mixedArray;
                    for (const auto& item : it->value.GetArray()) {
                        if (item.IsString()) {
                            mixedArray.push_back(std::string(item.GetString()));
                        } else {
                            mixedArray.push_back(item.GetString());
                        }
                    }
                    result[key] = mixedArray;
                }
            }
        }
    }
    
    return result;
}

// YAML parsing implementation
std::map<std::string, std::any> Config::parseYAML(const std::string& content) {
    std::map<std::string, std::any> result;
    try {
        YAML::Node doc = YAML::Load(content);
        
        if (doc.IsMap()) {
            for (auto it = doc.begin(); it != doc.end(); ++it) {
                const std::string& key = it->first.as<std::string>();
                
                if (it->second.IsScalar()) {
                    const std::string& value = it->second.as<std::string>();
                    
                    // Try to convert to appropriate type
                    if (value == "true" || value == "false") {
                        result[key] = value == "true";
                    } else if (std::all_of(value.begin(), value.end(), ::isdigit)) {
                        result[key] = std::stoi(value);
                    } else if (std::all_of(value.begin(), value.end(), [](char c) {
                        return std::isdigit(c) || c == '.' || c == '-';
                    })) {
                        try {
                            result[key] = std::stod(value);
                        } catch (...) {
                            result[key] = value;
                        }
                    } else {
                        result[key] = value;
                    }
                } else if (it->second.IsSequence()) {
                    // Handle YAML sequences
                    std::vector<int> intArray;
                    std::vector<double> doubleArray;
                    std::vector<std::string> stringArray;
                    
                    bool allInt = true, allDouble = true, allString = true;
                    
                    for (const auto& item : it->second) {
                        if (item.IsScalar()) {
                            const std::string& value = item.as<std::string>();
                            if (allInt && std::all_of(value.begin(), value.end(), ::isdigit)) {
                                intArray.push_back(std::stoi(value));
                            } else if (allDouble && std::all_of(value.begin(), value.end(), [](char c) {
                                return std::isdigit(c) || c == '.' || c == '-';
                            })) {
                                try {
                                    doubleArray.push_back(std::stod(value));
                                } catch (...) {
                                    allDouble = false;
                                }
                            } else {
                                stringArray.push_back(value);
                                allInt = allDouble = false;
                            }
                        } else {
                            allInt = allDouble = allString = false;
                        }
                    }
                    
                    if (allInt) {
                        result[key] = intArray;
                    } else if (allDouble) {
                        result[key] = doubleArray;
                    } else if (!stringArray.empty()) {
                        result[key] = stringArray;
                    }
                }
            }
        }
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("Failed to parse YAML content: " + std::string(e.what()));
    }
    
    return result;
}

// INI parsing implementation
std::map<std::string, std::any> Config::parseINI(const std::string& content) {
    std::map<std::string, std::any> result;
    
    // For simplicity, we'll implement a basic INI parser
    // This would normally use inih or a similar library
    std::istringstream stream(content);
    std::string line;
    std::string currentSection;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
        } else if (line.find('=') != std::string::npos) {
            size_t pos = line.find('=');
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            std::string fullKey = currentSection.empty() ? key : currentSection + "." + key;
            
            // Try to convert to appropriate type
            if (value == "true" || value == "false") {
                result[fullKey] = value == "true";
            } else if (std::all_of(value.begin(), value.end(), ::isdigit)) {
                result[fullKey] = std::stoi(value);
            } else if (std::all_of(value.begin(), value.end(), [](char c) {
                return std::isdigit(c) || c == '.' || c == '-';
            })) {
                try {
                    result[fullKey] = std::stod(value);
                } catch (...) {
                    result[fullKey] = value;
                }
            } else {
                result[fullKey] = value;
            }
        }
    }
    
    return result;
}

// TOML parsing implementation
std::map<std::string, std::any> Config::parseTOML(const std::string& content) {
    std::map<std::string, std::any> result;
    try {
        auto doc = toml::parse(content);
        
        // Convert TOML to std::map<std::string, std::any>
        // This is a simplified implementation
        // For a full implementation, you would need to handle nested TOML structures
        
        for (const auto& [key, value] : doc) {
            // Handle different TOML value types
            result[key] = value;
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse TOML content: " + std::string(e.what()));
    }
    
    return result;
}

std::map<std::string, std::any> Config::parseFile(const std::string& filepath) {
    FileFormat format = detectFileFormat(filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filepath);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    switch (format) {
        case FileFormat::JSON:
            return parseJSON(content);
        case FileFormat::YAML:
            return parseYAML(content);
        case FileFormat::INI:
            return parseINI(content);
        case FileFormat::TOML:
            return parseTOML(content);
        case FileFormat::Auto:
            // Try to detect based on content
            if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
                return parseJSON(content);
            } else if (content.find('[') != std::string::npos && content.find(']') != std::string::npos) {
                return parseYAML(content);
            } else if (content.find('=') != std::string::npos) {
                return parseINI(content);
            }
            return parseJSON(content); // Default to JSON
    }
    
    throw std::runtime_error("Unsupported file format");
}

// File format conversion implementations
std::string Config::toJSON(const std::map<std::string, std::any>& data) {
    rapidjson::Document doc;
    doc.SetObject();
    
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    
    for (const auto& [key, value] : data) {
        rapidjson::Value keyValue(key.c_str(), allocator);
        
        if (value.type() == typeid(int)) {
            rapidjson::Value valValue(std::any_cast<int>(value), allocator);
            doc.AddMember(keyValue, valValue, allocator);
        } else if (value.type() == typeid(double)) {
            rapidjson::Value valValue(std::any_cast<double>(value), allocator);
            doc.AddMember(keyValue, valValue, allocator);
        } else if (value.type() == typeid(bool)) {
            rapidjson::Value valValue(std::any_cast<bool>(value), allocator);
            doc.AddMember(keyValue, valValue, allocator);
        } else if (value.type() == typeid(std::string)) {
            rapidjson::Value valValue(std::any_cast<std::string>(value).c_str(), allocator);
            doc.AddMember(keyValue, valValue, allocator);
        } else if (value.type() == typeid(std::vector<int>)) {
            rapidjson::Value arrValue(rapidjson::kArrayType);
            for (int val : std::any_cast<std::vector<int>>(value)) {
                arrValue.PushBack(val, allocator);
            }
            doc.AddMember(keyValue, arrValue, allocator);
        } else if (value.type() == typeid(std::vector<double>)) {
            rapidjson::Value arrValue(rapidjson::kArrayType);
            for (double val : std::any_cast<std::vector<double>>(value)) {
                arrValue.PushBack(val, allocator);
            }
            doc.AddMember(keyValue, arrValue, allocator);
        } else if (value.type() == typeid(std::vector<std::string>)) {
            rapidjson::Value arrValue(rapidjson::kArrayType);
            for (const auto& val : std::any_cast<std::vector<std::string>>(value)) {
                rapidjson::Value strValue(val.c_str(), allocator);
                arrValue.PushBack(strValue, allocator);
            }
            doc.AddMember(keyValue, arrValue, allocator);
        }
    }
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    
    return buffer.GetString();
}

std::string Config::toYAML(const std::map<std::string, std::any>& data) {
    YAML::Emitter emitter;
    emitter << YAML::BeginDoc;
    
    for (const auto& [key, value] : data) {
        emitter << YAML::Key << key;
        
        if (value.type() == typeid(int)) {
            emitter << std::any_cast<int>(value);
        } else if (value.type() == typeid(double)) {
            emitter << std::any_cast<double>(value);
        } else if (value.type() == typeid(bool)) {
            emitter << std::any_cast<bool>(value);
        } else if (value.type() == typeid(std::string)) {
            emitter << std::any_cast<std::string>(value);
        } else if (value.type() == typeid(std::vector<int>)) {
            emitter << YAML::Flow;
            emitter << YAML::BeginSeq;
            for (int val : std::any_cast<std::vector<int>>(value)) {
                emitter << val;
            }
            emitter << YAML::EndSeq;
        } else if (value.type() == typeid(std::vector<double>)) {
            emitter << YAML::Flow;
            emitter << YAML::BeginSeq;
            for (double val : std::any_cast<std::vector<double>>(value)) {
                emitter << val;
            }
            emitter << YAML::EndSeq;
        } else if (value.type() == typeid(std::vector<std::string>)) {
            emitter << YAML::Flow;
            emitter << YAML::BeginSeq;
            for (const auto& val : std::any_cast<std::vector<std::string>>(value)) {
                emitter << val;
            }
            emitter << YAML::EndSeq;
        } else {
            emitter << "unknown";
        }
    }
    
    emitter << YAML::EndDoc;
    
    return emitter.c_str();
}

std::string Config::toINI(const std::map<std::string, std::any>& data) {
    std::ostringstream oss;
    
    // Group keys by sections (everything before the first dot)
    std::map<std::string, std::map<std::string, std::any>> sections;
    
    for (const auto& [fullKey, value] : data) {
        size_t dotPos = fullKey.find('.');
        if (dotPos == std::string::npos) {
            sections[""][fullKey] = value;
        } else {
            std::string section = fullKey.substr(0, dotPos);
            std::string key = fullKey.substr(dotPos + 1);
            sections[section][key] = value;
        }
    }
    
    for (const auto& [section, keys] : sections) {
        if (!section.empty()) {
            oss << "[" << section << "]\n";
        }
        
        for (const auto& [key, value] : keys) {
            oss << key << " = ";
            
            if (value.type() == typeid(int)) {
                oss << std::any_cast<int>(value);
            } else if (value.type() == typeid(double)) {
                oss << std::any_cast<double>(value);
            } else if (value.type() == typeid(bool)) {
                oss << (std::any_cast<bool>(value) ? "true" : "false");
            } else if (value.type() == typeid(std::string)) {
                oss << "\"" << std::any_cast<std::string>(value) << "\"";
            } else {
                oss << std::any_cast<std::string>(value); // Fallback
            }
            oss << "\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::toTOML(const std::map<std::string, std::any>& data) {
    // For now, return a simplified TOML representation
    // Full TOML implementation would require more sophisticated parsing
    std::ostringstream oss;
    oss << "# TOML format (simplified)\n";
    
    for (const auto& [key, value] : data) {
        oss << key << " = ";
        
        if (value.type() == typeid(int)) {
            oss << std::any_cast<int>(value) << "\n";
        } else if (value.type() == typeid(double)) {
            oss << std::any_cast<double>(value) << "\n";
        } else if (value.type() == typeid(bool)) {
            oss << (std::any_cast<bool>(value) ? "true" : "false") << "\n";
        } else if (value.type() == typeid(std::string)) {
            oss << "\"" << std::any_cast<std::string>(value) << "\"\n";
        } else {
            oss << "# [unhandled type]" << "\n";
        }
    }
    
    return oss.str();
}

bool Config::loadFromFile(const std::string& filepath, const std::string& category) {
    ConfigSource source = ConfigSource::File;
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    // Detect file format
    FileFormat format = detectFileFormat(filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to convert to appropriate type
            ConfigValue configValue;
            try {
                // Try int first
                try {
                    configValue = std::stoi(value);
                } catch (...) {
                    // Try double
                    try {
                        configValue = std::stod(value);
                    } catch (...) {
                        // Try bool
                        if (value == "true") {
                            configValue = true;
                        } else if (value == "false") {
                            configValue = false;
                        } else {
                            configValue = value;
                        }
                    }
                }
            } catch (...) {
                configValue = value;
            }
            
            set(key, configValue, source, "Loaded from file: " + filepath, actualCategory);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath, const std::string& category) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    // Get entries for the specified category
    const auto& entries = pImpl->categoryEntries.find(actualCategory);
    if (entries == pImpl->categoryEntries.end()) {
        return false;
    }
    
    for (const auto& entry : entries->second) {
        file << "# " << entry.description << "\n";
        
        std::visit([&file, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_integral_v<T>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_floating_point_v<T>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n";
            }
        }, entry.value);
    }
    
    return true;
}

bool Config::loadFromSchemaFile(const std::string& filepath, const std::string& category) {
    // For now, use the same implementation as loadFromFile
    // TODO: Implement schema-aware file loading
    return loadFromFile(filepath, category);
}

bool Config::saveToSchemaFile(const std::string& filepath, const std::string& category) const {
    // For now, use the same implementation as saveToFile
    // TODO: Implement schema-aware file saving
    return saveToFile(filepath, category);
}

SchemaValidationResult Config::validateSchema(const std::string& category) const {
    std::string actualCategory = category.empty() ? "legacy" : category;
    
    // Get schema
    const auto& schema = getSchema(actualCategory);
    if (!schema) {
        return SchemaValidationResult(false, {"Schema not found: " + actualCategory});
    }
    
    // Convert current entries to map<std::string, std::any>
    std::map<std::string, std::any> values;
    const auto& entries = pImpl->categoryEntries.find(actualCategory);
    if (entries != pImpl->categoryEntries.end()) {
        for (const auto& entry : entries->second) {
            // Convert ConfigValue to std::any
            std::visit([&values, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                values[entry.key] = static_cast<std::any>(arg);
            }, entry.value);
        }
    }
    
    return schema->validate(values);
}