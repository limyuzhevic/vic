#pragma once

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::shared_ptr<Config> parentConfig;
    std::map<std::string, Config> templates;
    std::map<std::string, ConfigSchema> schemas;
    std::map<std::string, Preset> presets;
    std::map<std::string, std::vector<Constraint>> keyConstraints;
    std::map<std::string, std::string> keyMetadata;
    std::map<std::string, std::string> keyCategories;
    std::map<std::string, bool> keyRequired;
    std::map<std::string, std::string> keyDescriptions;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default presets and schemas
    initializeDefaultPresets();
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

// Enhanced configuration management
void Config::registerKey(const std::string& key, const ConfigValue& defaultValue, 
                        const std::string& description, const std::string& category,
                        bool required) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        pImpl->entries.emplace_back(key, defaultValue, ConfigSource::Default, description, category);
        pImpl->keyRequired[key] = required;
        pImpl->keyDescriptions[key] = description;
        pImpl->keyCategories[key] = category;
    }
}

void Config::registerKeyWithConstraints(const std::string& key, const ConfigValue& defaultValue,
                                       const std::string& description, const std::string& category,
                                       const std::vector<Constraint>& constraints) {
    registerKey(key, defaultValue, description, category);
    if (!constraints.empty()) {
        pImpl->keyConstraints[key] = constraints;
    }
}

// Enhanced file I/O with multiple formats
bool Config::loadFromFile(const std::string& filepath, const std::string& format) {
    if (format == "json") {
        return loadFromJSONFile(filepath);
    } else if (format == "yaml" || format == "yml") {
        return loadFromYAMLFile(filepath);
    } else if (format == "ini") {
        return loadFromINIFile(filepath);
    } else {
        // Default to JSON
        return loadFromJSONFile(filepath);
    }
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        auto data = json::parse(jsonStr);
        pImpl->entries.clear();
        
        for (auto& [key, value] : data.items()) {
            ConfigValue configValue;
            ConfigSource source = ConfigSource::File;
            
            if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_number_integer()) {
                configValue = value.get<int64_t>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
            } else if (value.is_number()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_array()) {
                // Handle arrays
                std::vector<std::string> strArray;
                for (const auto& item : value) {
                    strArray.push_back(item.get<std::string>());
                }
                configValue = strArray;
            }
            
            pImpl->entries.emplace_back(key, configValue, source, "", "");
        }
        
        return true;
    } catch (const json::parse_error& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlStr) {
    // TODO: Implement YAML parsing using a YAML library
    // For now, return false to indicate not implemented
    return false;
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string jsonStr((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    return loadFromJSON(jsonStr);
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string yamlStr((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    return loadFromYAML(yamlStr);
}

bool Config::loadFromINIFile(const std::string& filepath) {
    // TODO: Implement INI file parsing
    return false;
}

// Export/import enhanced formats
bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    if (format == "json") {
        return saveToJSONFile(filepath);
    } else if (format == "yaml" || format == "yml") {
        return saveToYAMLFile(filepath);
    } else {
        return saveToJSONFile(filepath);
    }
}

std::string Config::toJSON() const {
    json data;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&data, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                data[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                json arr = json::array();
                for (const auto& item : arg) {
                    arr.push_back(item);
                }
                data[entry.key] = arr;
            }
        }, entry.value);
    }
    
    return data.dump(2);
}

std::string Config::toYAML() const {
    // TODO: Implement YAML export using a YAML library
    return "";
}

bool Config::fromJSON(const std::string& jsonStr) {
    return loadFromJSON(jsonStr);
}

bool Config::fromYAML(const std::string& yamlStr) {
    return loadFromYAML(yamlStr);
}

bool Config::saveToJSONFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << toJSON();
    return true;
}

bool Config::saveToYAMLFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << toYAML();
    return true;
}

// Command line loading
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

// Get values
namespace {
template<typename T>
std::optional<T> getValue(const ConfigEntry& entry) {
    try {
        return std::get<T>(entry.value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}
} // namespace

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return getValue<T>(*it);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Enhanced set with validation
void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Check constraints if any
        auto constraintIt = pImpl->keyConstraints.find(key);
        if (constraintIt != pImpl->keyConstraints.end()) {
            ValidationResult result;
            for (const auto& constraint : constraintIt->second) {
                if (!checkConstraint(constraint, value, result)) {
                    // Validation failed - return without setting
                    return;
                }
            }
        }
        
        it->value = value;
        it->source = source;
    }
}

void Config::set(const std::string& key, const std::string& value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, 
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Configuration management
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
    pImpl->keyConstraints.erase(key);
    pImpl->keyMetadata.erase(key);
    pImpl->keyCategories.erase(key);
    pImpl->keyRequired.erase(key);
    pImpl->keyDescriptions.erase(key);
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
    pImpl->keyConstraints.clear();
    pImpl->keyMetadata.clear();
    pImpl->keyCategories.clear();
    pImpl->keyRequired.clear();
    pImpl->keyDescriptions.clear();
}

// Validation
ValidationResult Config::validate() const {
    ValidationResult result;
    
    // Check required keys
    for (const auto& entry : pImpl->entries) {
        auto requiredIt = pImpl->keyRequired.find(entry.key);
        if (requiredIt != pImpl->keyRequired.end() && requiredIt->second && !has(entry.key)) {
            result.addError("Required key not found: " + entry.key);
        }
        
        // Validate against constraints
        auto constraintIt = pImpl->keyConstraints.find(entry.key);
        if (constraintIt != pImpl->keyConstraints.end()) {
            for (const auto& constraint : constraintIt->second) {
                if (!checkConstraint(constraint, entry.value, result)) {
                    result.addError("Constraint violation for key: " + entry.key);
                }
            }
        }
    }
    
    return result;
}

bool Config::validateKey(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    ValidationResult result;
    auto constraintIt = pImpl->keyConstraints.find(key);
    if (constraintIt != pImpl->keyConstraints.end()) {
        for (const auto& constraint : constraintIt->second) {
            if (!checkConstraint(constraint, it->value, result)) {
                return false;
            }
        }
    }
    
    return true;
}

// Preset management
bool Config::addPreset(const Preset& preset) {
    pImpl->presets[preset.name] = preset;
    return true;
}

bool Config::applyPreset(const std::string& presetName) {
    auto it = pImpl->presets.find(presetName);
    if (it == pImpl->presets.end()) {
        return false;
    }
    
    const Preset& preset = it->second;
    
    // Apply dependencies first
    std::set<std::string> visited;
    for (const auto& dep : preset.dependencies) {
        if (!applyPresetRecursive(dep, visited)) {
            return false;
        }
    }
    
    // Apply preset values
    for (const auto& [key, value] : preset.values) {
        set(key, value, ConfigSource::Preset);
    }
    
    return true;
}

std::vector<std::string> Config::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& [name, preset] : pImpl->presets) {
        names.push_back(name);
    }
    return names;
}

// Hierarchical configuration
void Config::setParentConfig(std::shared_ptr<Config> parent) {
    pImpl->parentConfig = parent;
}

std::shared_ptr<Config> Config::getParentConfig() const {
    return pImpl->parentConfig;
}

bool Config::inheritsFrom(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key && e.isInherited; });
    return it != pImpl->entries.end();
}

// Template management
bool Config::addTemplate(const std::string& name, const Config& templateConfig) {
    pImpl->templates[name] = templateConfig;
    return true;
}

bool Config::applyTemplate(const std::string& templateName) {
    auto it = pImpl->templates.find(templateName);
    if (it == pImpl->templates.end()) {
        return false;
    }
    
    const Config& templateConfig = it->second;
    
    // Apply template values (overriding existing values)
    for (const auto& entry : templateConfig.pImpl->entries) {
        set(entry.key, entry.value, ConfigSource::Runtime);
    }
    
    return true;
}

std::vector<std::string> Config::getTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& [name, template] : pImpl->templates) {
        names.push_back(name);
    }
    return names;
}

// Schema management
bool Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas[schema.name] = schema;
    return true;
}

bool Config::validateAgainstSchema(const std::string& schemaName) const {
    auto it = pImpl->schemas.find(schemaName);
    if (it == pImpl->schemas.end()) {
        return false;
    }
    
    ValidationResult result = validateAgainstSchema(it->second);
    return result.valid;
}

std::vector<std::string> Config::getSchemaNames() const {
    std::vector<std::string> names;
    for (const auto& [name, schema] : pImpl->schemas) {
        names.push_back(name);
    }
    return names;
}

// Configuration summary
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

std::string Config::summaryByCategory() const {
    std::ostringstream oss;
    oss << "Configuration by Category:\n";
    
    std::map<std::string, std::vector<ConfigEntry>> entriesByCategory;
    for (const auto& entry : pImpl->entries) {
        entriesByCategory[entry.category].push_back(entry);
    }
    
    for (const auto& [category, entries] : entriesByCategory) {
        oss << "  " << category << " (" << entries.size() << "):\n";
        for (const auto& entry : entries) {
            oss << "    " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "]\n";
        }
    }
    
    return oss.str();
}

// File I/O helpers
bool Config::fileExists(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}

std::string Config::getConfigDirectory() {
    return "./configs/";
}

// Static preset management
void Config::initializeDefaultPresets() {
    // Performance preset
    Preset performancePreset;
    performancePreset.name = "performance";
    performancePreset.description = "High-performance configuration with minimal processing time";
    performancePreset.values = {
        {"neural_dynamics.max_iterations", 100},
        {"plasticity.plasticity_rate", 0.01},
        {"memory.capacity", 1000}
    };
    performancePreset.dependencies = {};
    addPreset(performancePreset);
    
    // Accuracy preset
    Preset accuracyPreset;
    accuracyPreset.name = "accuracy";
    accuracyPreset.description = "High-accuracy configuration with more computation time";
    accuracyPreset.values = {
        {"neural_dynamics.max_iterations", 1000},
        {"plasticity.plasticity_rate", 0.1},
        {"memory.capacity", 10000}
    };
    accuracyPreset.dependencies = {};
    addPreset(accuracyPreset);
    
    // Balanced preset
    Preset balancedPreset;
    balancedPreset.name = "balanced";
    balancedPreset.description = "Balanced configuration between performance and accuracy";
    balancedPreset.values = {
        {"neural_dynamics.max_iterations", 500},
        {"plasticity.plasticity_rate", 0.05},
        {"memory.capacity", 5000}
    };
    balancedPreset.dependencies = {"performance", "accuracy"};
    addPreset(balancedPreset);
}

void Config::initializeDefaultSchema() {
    ConfigSchema schema;
    schema.name = "default_schema";
    schema.description = "Default configuration schema for NLM";
    
    // Add some standard configuration entries
    registerKey("neural_dynamics.max_iterations", ConfigValue(100), 
                "Maximum iterations for neural dynamics", "Neural Dynamics", true);
    registerKeyWithConstraints("neural_dynamics.max_iterations", ConfigValue(100),
                               "Maximum iterations for neural dynamics", "Neural Dynamics",
                               {Constraint{ConstraintType::Range, "1", "10000"}});
    
    registerKey("plasticity.plasticity_rate", ConfigValue(0.05),
                "Plasticity learning rate", "Plasticity", true);
    registerKeyWithConstraints("plasticity.plasticity_rate", ConfigValue(0.05),
                               "Plasticity learning rate", "Plasticity",
                               {Constraint{ConstraintType::Range, "0.0", "1.0"}});
    
    registerKey("memory.capacity", ConfigValue(5000),
                "Memory capacity", "Memory", true);
    registerKeyWithConstraints("memory.capacity", ConfigValue(5000),
                               "Memory capacity", "Memory",
                               {Constraint{ConstraintType::Range, "100", "100000"}});
    
    schema.entries = pImpl->entries; // Simplified - in reality would be more complex
    addSchema(schema);
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

std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c;
        }
    }
    return result;
}

std::string Config::unescapeString(const std::string& str) {
    std::string result;
    size_t i = 0;
    while (i < str.size()) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '\"': result += '\"'; i += 2; break;
                case '\\': result += '\\'; i += 2; break;
                case 'n': result += '\n'; i += 2; break;
                case 'r': result += '\r'; i += 2; break;
                case 't': result += '\t'; i += 2; break;
                default: result += str[i]; i++;
            }
        } else {
            result += str[i];
            i++;
        }
    }
    return result;
}

bool Config::checkConstraint(const Constraint& constraint, const ConfigValue& value, 
                            ValidationResult& result) const {
    switch (constraint.type) {
        case ConstraintType::Range:
            return checkRangeConstraint(constraint, value);
        case ConstraintType::Enumeration:
            return checkEnumerationConstraint(constraint, value);
        case ConstraintType::Length:
            return checkLengthConstraint(constraint, value);
        case ConstraintType::Pattern:
            return checkPatternConstraint(constraint, value);
        case ConstraintType::Custom:
            return constraint.customValidator(value);
        default:
            return true;
    }
}

bool Config::checkRangeConstraint(const Constraint& constraint, const ConfigValue& value) const {
    try {
        if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
            int64_t val = std::holds_alternative<int>(value) ? 
                         std::get<int>(value) : std::get<int64_t>(value);
            int64_t min = std::stoll(constraint.param1);
            int64_t max = std::stoll(constraint.param2);
            return val >= min && val <= max;
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            double min = std::stod(constraint.param1);
            double max = std::stod(constraint.param2);
            return val >= min && val <= max;
        }
    } catch (...) {
        return false;
    }
    return false;
}

bool Config::checkEnumerationConstraint(const Constraint& constraint, const ConfigValue& value) const {
    if (std::holds_alternative<std::string>(value)) {
        const std::string& val = std::get<std::string>(value);
        // Simple enumeration check - in practice would be more sophisticated
        return constraint.param1 == val; // Just checking exact match for demo
    }
    return false;
}

bool Config::checkLengthConstraint(const Constraint& constraint, const ConfigValue& value) const {
    try {
        if (std::holds_alternative<std::string>(value)) {
            size_t len = std::get<std::string>(value).size();
            size_t min = std::stoull(constraint.param1);
            size_t max = std::stoull(constraint.param2);
            return len >= min && len <= max;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            size_t len = std::get<std::vector<std::string>>(value).size();
            size_t min = std::stoull(constraint.param1);
            size_t max = std::stoull(constraint.param2);
            return len >= min && len <= max;
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            size_t len = std::get<std::vector<int>>(value).size();
            size_t min = std::stoull(constraint.param1);
            size_t max = std::stoull(constraint.param2);
            return len >= min && len <= max;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            size_t len = std::get<std::vector<double>>(value).size();
            size_t min = std::stoull(constraint.param1);
            size_t max = std::stoull(constraint.param2);
            return len >= min && len <= max;
        } else if (std::holds_alternative<std::vector<bool>>(value)) {
            size_t len = std::get<std::vector<bool>>(value).size();
            size_t min = std::stoull(constraint.param1);
            size_t max = std::stoull(constraint.param2);
            return len >= min && len <= max;
        }
    } catch (...) {
        return false;
    }
    return false;
}

bool Config::checkPatternConstraint(const Constraint& constraint, const ConfigValue& value) const {
    if (std::holds_alternative<std::string>(value)) {
        const std::string& val = std::get<std::string>(value);
        // Simple regex check - would use actual regex library in practice
        // For demo, just check if pattern is substring
        return val.find(constraint.param1) != std::string::npos;
    }
    return false;
}

ValidationResult Config::validateAgainstSchema(const ConfigSchema& schema) const {
    ValidationResult result;
    
    // Check for required schema entries
    for (const auto& [key, entry] : schema.entries) {
        if (entry.required && !has(key)) {
            result.addError("Schema requires key not found: " + key);
        }
        
        // Validate against schema constraints
        auto constraintIt = schema.constraints.find(key);
        if (constraintIt != schema.constraints.end()) {
            for (const auto& constraint : constraintIt->second) {
                auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });
                if (it != pImpl->entries.end() && !checkConstraint(constraint, it->value, result)) {
                    result.addError("Schema constraint violation for key: " + key);
                }
            }
        }
    }
    
    return result;
}

bool Config::applyPresetRecursive(const std::string& presetName, std::set<std::string>& visited) {
    if (visited.find(presetName) != visited.end()) {
        return true; // Already visited, no circular dependency
    }
    
    visited.insert(presetName);
    
    auto it = pImpl->presets.find(presetName);
    if (it == pImpl->presets.end()) {
        return false; // Preset not found
    }
    
    const Preset& preset = it->second;
    
    // Apply dependencies first
    for (const auto& dep : preset.dependencies) {
        if (!applyPresetRecursive(dep, visited)) {
            return false;
        }
    }
    
    // Apply preset values
    for (const auto& [key, value] : preset.values) {
        set(key, value, ConfigSource::Preset);
    }
    
    return true;
}

} // namespace nlm
