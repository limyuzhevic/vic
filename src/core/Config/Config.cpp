#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <chrono>
#include <regex>
#include <cmath>
#include <iomanip>
#include <map>
#include <set>
#include <numeric>
#include <unordered_set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigParameter> parameters;
    std::vector<std::shared_ptr<ConfigChangeListener>> changeListeners;
    
    // Change tracking
    std::set<std::string> changedKeys;
    std::chrono::system_clock::time_point lastModified;
    
    // Statistics
    size_t totalChanges = 0;
    
    // Import/export cache
    std::string lastJSONExport;
    std::string lastINIExport;
    std::string lastYAMLExport;
    
    // Snapshot data
    std::vector<std::string> snapshotHistory;
    size_t currentSnapshotIndex = 0;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastModified = std::chrono::system_clock::now();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Clear existing entries
    clear();
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
            continue;
        }
        
        // Parse different file formats
        if (filepath.find(".json") != std::string::npos) {
            // JSON parsing (simplified)
            if (line.find("{") != std::string::npos) {
                // Parse JSON object (simplified)
                size_t keyStart = line.find('"') + 1;
                size_t keyEnd = line.find('"', keyStart);
                if (keyStart != std::string::npos && keyEnd != std::string::npos) {
                    std::string key = line.substr(keyStart, keyEnd - keyStart);
                    size_t colonPos = line.find(':', keyEnd);
                    if (colonPos != std::string::npos) {
                        std::string valueStr = line.substr(colonPos + 1);
                        // Remove quotes and trailing characters
                        if (valueStr.front() == '"' && valueStr.back() == '"') {
                            valueStr = valueStr.substr(1, valueStr.size() - 2);
                        }
                        set(key, valueStr, ConfigSource::File);
                    }
                }
            }
        } else if (filepath.find(".ini") != std::string::npos || filepath.find(".cfg") != std::string::npos) {
            // INI format parsing
            if (line.find('[') != std::string::npos && line.find(']') != std::string::npos) {
                // Section header (skip for now)
                continue;
            }
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                set(key, value, ConfigSource::File);
            }
        } else if (filepath.find(".yaml") != std::string::npos || filepath.find(".yml") != std::string::npos) {
            // YAML parsing (simplified)
            if (line.find('- ') == 0) {
                // List item (skip for now)
                continue;
            } else if (line.find(": ") != std::string::npos) {
                size_t pos = line.find(": ");
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 2));
                set(key, value, ConfigSource::File);
            }
        } else {
            // Default simple format
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
    
    if (validate) {
        validate();
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv, bool validate) {
    // Clear existing command line entries but preserve file and default entries
    std::vector<ConfigEntry> nonCommandLineEntries;
    for (const auto& entry : pImpl->entries) {
        if (entry.source != ConfigSource::CommandLine) {
            nonCommandLineEntries.push_back(entry);
        }
    }
    pImpl->entries = std::move(nonCommandLineEntries);
    
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
        // Handle boolean flags
        else if (arg == "--enable-debug" || arg == "--verbose" || arg == "--benchmark") {
            set(arg.substr(2), true, ConfigSource::CommandLine);
        }
    }
    
    if (validate) {
        validate();
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
    return true;
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        file << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            file << "  \"" << entry.key << "\": \"" << valueToString(entry.value) << "\"";
            if (i < pImpl->entries.size() - 1) file << ",";
            file << "\n";
        }
        file << "}\n";
    } else if (format == "ini") {
        file << "[GENERAL]\n";
        for (const auto& entry : pImpl->entries) {
            file << entry.key << " = " << valueToString(entry.value) << "\n";
        }
    } else if (format == "yaml") {
        file << "---\n";
        for (const auto& entry : pImpl->entries) {
            file << entry.key << ": " << valueToString(entry.value) << "\n";
        }
    } else {
        // Default simple format
        for (const auto& entry : pImpl->entries) {
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            file << entry.key << " = " << valueToString(entry.value) << "\n";
        }
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check if parameter exists and get old value for listeners
    ConfigValue oldValue;
    bool exists = false;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
        exists = true;
    }
    
    // Set the value
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Update change tracking
    if (exists && !(oldValue == value)) {
        pImpl->changedKeys.insert(key);
        pImpl->totalChanges++;
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
    
    // Notify listeners
    if (exists && !(oldValue == value)) {
        notifyChangeListeners(key, oldValue, value);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    setWithType(key, value, source);
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

bool Config::setWithType(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue newValue;
    if (!stringToValue(value, newValue)) {
        return false;
    }
    
    // Check for parameter constraints
    auto paramIt = pImpl->parameters.find(key);
    if (paramIt != pImpl->parameters.end()) {
        const auto& param = paramIt->second;
        // Validate the value against parameter constraints
        std::string validationError = validateParameter(param, newValue);
        if (!validationError.empty()) {
            throw ConfigValidationError(validationError, key);
        }
    }
    
    set(key, newValue, source);
    return true;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    // Notify before removal
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        ConfigValue oldValue = it->value;
        pImpl->entries.erase(it);
        pImpl->changedKeys.insert(key);
        notifyChangeListeners(key, oldValue, ConfigValue());
    }
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
    // Notify listeners before clearing
    for (const auto& entry : pImpl->entries) {
        notifyChangeListeners(entry.key, entry.value, ConfigValue());
    }
    
    pImpl->entries.clear();
    pImpl->changedKeys.clear();
    pImpl->totalChanges = 0;
    pImpl->lastModified = std::chrono::system_clock::now();
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

std::string Config::summaryDetailed() const {
    std::ostringstream oss;
    oss << "=== Detailed Configuration Summary ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Changed entries: " << pImpl->changedKeys.size() << "\n";
    oss << "Total changes: " << pImpl->totalChanges << "\n";
    oss << "Last modified: " << std::chrono::system_clock::to_time_t(pImpl->lastModified) << "\n\n";
    
    oss << "=== Entry Details ===\n";
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << "\n";
        oss << "  Value: " << valueToString(entry.value) << "\n";
        oss << "  Source: " << static_cast<int>(entry.source) << "\n";
        oss << "  Description: " << entry.description << "\n";
        
        // Find parameter metadata
        auto paramIt = pImpl->parameters.find(entry.key);
        if (paramIt != pImpl->parameters.end()) {
            const auto& param = paramIt->second;
            oss << "  Category: " << param.category << "\n";
            oss << "  Type: " << param.dataType << "\n";
            oss << "  Required: " << (param.required ? "Yes" : "No") << "\n";
            oss << "  Editable: " << (param.editable ? "Yes" : "No") << "\n";
        }
        
        oss << "\n";
    }
    
    oss << "=== Validation Status ===\n";
    auto errors = getValidationErrors();
    if (errors.empty()) {
        oss << "✓ Configuration is valid\n";
    } else {
        oss << "✗ Configuration has validation errors:\n";
        for (const auto& error : errors) {
            oss << "  - " << error << "\n";
        }
    }
    
    return oss.str();
}

bool Config::validate() const {
    pImpl->lastModified = std::chrono::system_clock::now();
    return getValidationErrors().empty();
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    auto paramIt = pImpl->parameters.find(key);
    if (paramIt == pImpl->parameters.end()) {
        return true;  // No parameter constraints, assume valid
    }
    
    const auto& param = paramIt->second;
    std::string error = validateParameter(param, value);
    return error.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Validate each entry against its parameter
    for (const auto& entry : pImpl->entries) {
        auto paramIt = pImpl->parameters.find(entry.key);
        if (paramIt != pImpl->parameters.end()) {
            const auto& param = paramIt->second;
            std::string error = validateParameter(param, entry.value);
            if (!error.empty()) {
                errors.push_back("Key \"" + entry.key + "\": " + error);
            }
        }
        
        // Check for missing required parameters
        if (paramIt == pImpl->parameters.end()) {
            auto requiredParamIt = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
                [](const auto& pair) { return pair.second.required; });
            if (requiredParamIt != pImpl->parameters.end()) {
                errors.push_back("Required parameter \"" + requiredParamIt->second.key + "\" is missing");
            }
        }
    }
    
    // Validate parameter consistency
    std::vector<std::string> consistencyErrors = getConsistencyErrors();
    errors.insert(errors.end(), consistencyErrors.begin(), consistencyErrors.end());
    
    return errors;
}

void Config::registerParameter(const ConfigParameter& param) {
    if (!validateParameter(param)) {
        throw ConfigValidationError("Invalid parameter definition", param.key);
    }
    
    pImpl->parameters[param.key] = param;
}

bool Config::hasParameter(const std::string& key) const {
    return pImpl->parameters.find(key) != pImpl->parameters.end();
}

std::optional<ConfigParameter> Config::getParameter(const std::string& key) const {
    auto it = pImpl->parameters.find(key);
    if (it != pImpl->parameters.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<ConfigParameter> Config::getAllParameters() const {
    std::vector<ConfigParameter> params;
    params.reserve(pImpl->parameters.size());
    for (const auto& pair : pImpl->parameters) {
        params.push_back(pair.second);
    }
    return params;
}

std::vector<std::string> Config::getParameterCategories() const {
    std::vector<std::string> categories;
    std::set<std::string> uniqueCategories;
    
    for (const auto& param : pImpl->parameters) {
        if (uniqueCategories.find(param.second.category) == uniqueCategories.end()) {
            uniqueCategories.insert(param.second.category);
            categories.push_back(param.second.category);
        }
    }
    
    return categories;
}

bool Config::validateParameter(const ConfigParameter& param) const {
    std::string error = validateParameter(param, param.defaultValue);
    return error.empty();
}

std::string Config::validateParameter(const ConfigParameter& param, const ConfigValue& value) const {
    // Check data type compatibility
    if (!valueCompatibleWithType(value, param.dataType)) {
        return "Value type incompatible with parameter type \"" + param.dataType + "\"";
    }
    
    // Validate against validation rules
    for (const auto& rule : param.validationRules) {
        std::string ruleError = validateRule(rule, value);
        if (!ruleError.empty()) {
            return rule.errorMessage.empty() ? ruleError : rule.errorMessage;
        }
    }
    
    // Check dependencies
    for (const auto& dependency : param.dependencies) {
        if (!has(dependency)) {
            return "Missing dependency parameter: \"" + dependency + "\"";
        }
    }
    
    // Check dependency validator
    if (param.dependencyValidator && !param.dependencyValidator(value)) {
        return "Value does not satisfy dependency validation";
    }
    
    return "";
}

std::unordered_map<std::string, std::shared_ptr<Config>> Config::getPresets() {
    static std::unordered_map<std::string, std::shared_ptr<Config>> presets;
    if (presets.empty()) {
        presets = initializePresets();
    }
    return presets;
}

bool Config::applyPreset(const std::string& presetName) {
    auto presets = getPresets();
    auto it = presets.find(presetName);
    if (it == presets.end()) {
        return false;
    }
    
    // Merge preset configuration
    merge(*it->second, true);
    return true;
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << '"' << arg << '"';
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i];
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i];
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << '"' << arg[i] << '"';
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::stringToValue(const std::string& str, ConfigValue& value, const std::string& targetType) {
    if (str.empty()) {
        value = "";
        return true;
    }
    
    // Parse boolean
    if (str == "true" || str == "false") {
        value = (str == "true");
        return true;
    }
    
    // Parse integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            value = intVal;
            return true;
        }
    } catch (...) {}
    
    // Parse int64
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) {
            value = int64Val;
            return true;
        }
    } catch (...) {}
    
    // Parse double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Parse as string (remove quotes)
    std::string cleaned = str;
    if (cleaned.size() >= 2 && 
        ((cleaned.front() == '"' && cleaned.back() == '"') ||
         (cleaned.front() == '\'' && cleaned.back() == '\''))) {
        cleaned = cleaned.substr(1, cleaned.size() - 2);
    }
    
    value = cleaned;
    return true;
}

bool Config::valueCompatibleWithType(const ConfigValue& value, const std::string& type) const {
    if (type == "auto") {
        return true;
    }
    
    if (type == "int") {
        return std::holds_alternative<int>(value);
    } else if (type == "int64") {
        return std::holds_alternative<int64_t>(value);
    } else if (type == "double" || type == "float") {
        return std::holds_alternative<double>(value);
    } else if (type == "bool") {
        return std::holds_alternative<bool>(value);
    } else if (type == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (type == "int_vector") {
        return std::holds_alternative<std::vector<int>>(value);
    } else if (type == "double_vector") {
        return std::holds_alternative<std::vector<double>>(value);
    } else if (type == "string_vector") {
        return std::holds_alternative<std::vector<std::string>>(value);
    }
    
    return false;
}

std::string Config::validateRule(const Config::ConfigParameter::ValidationRule& rule, const ConfigValue& value) const {
    switch (rule.type) {
        case ConfigParameter::ValidationRule::Type::Range:
            if (!validateRange(value, rule.data.range.min, rule.data.range.max, rule.data.range.inclusive)) {
                return rule.errorMessage.empty() ? "Value is out of range" : rule.errorMessage;
            }
            break;
            
        case ConfigParameter::ValidationRule::Type::Length:
            if (!validateLength(value, rule.data.length.minLength, rule.data.length.maxLength, rule.data.length.inclusive)) {
                return rule.errorMessage.empty() ? "Value length is out of range" : rule.errorMessage;
            }
            break;
            
        case ConfigParameter::ValidationRule::Type::Pattern:
            if (!validatePattern(value, rule.data.pattern.pattern, rule.data.pattern.flags)) {
                return rule.errorMessage.empty() ? "Value does not match pattern" : rule.errorMessage;
            }
            break;
            
        case ConfigParameter::ValidationRule::Type::OneOf:
            if (!validateOneOf(value, rule.data.oneOf.allowedValues)) {
                return rule.errorMessage.empty() ? "Value is not one of the allowed values" : rule.errorMessage;
            }
            break;
            
        case ConfigParameter::ValidationRule::Type::Custom:
            if (rule.data.custom.validator && !rule.data.custom.validator(value)) {
                return rule.errorMessage.empty() ? "Value does not satisfy custom validation" : rule.errorMessage;
            }
            break;
    }
    
    return "";
}

void Config::addChangeListener(std::shared_ptr<ConfigChangeListener> listener) {
    if (listener) {
        pImpl->changeListeners.push_back(listener);
    }
}

void Config::removeChangeListener(std::shared_ptr<ConfigChangeListener> listener) {
    pImpl->changeListeners.erase(
        std::remove(pImpl->changeListeners.begin(), pImpl->changeListeners.end(), listener),
        pImpl->changeListeners.end()
    );
}

void Config::notifyChangeListeners(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    for (const auto& listener : pImpl->changeListeners) {
        try {
            listener->onConfigChanged(key, oldValue, newValue);
        } catch (const std::exception& e) {
            // Log error but continue with other listeners
        }
    }
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getChangedEntryCount() const {
    return static_cast<size_t>(pImpl->changedKeys.size());
}

std::vector<std::string> Config::getChangedKeys() const {
    return std::vector<std::string>(pImpl->changedKeys.begin(), pImpl->changedKeys.end());
}

std::chrono::system_clock::time_point Config::getLastModified() const {
    return pImpl->lastModified;
}

std::vector<std::string> Config::findKeysByPattern(const std::string& pattern) const {
    std::vector<std::string> matches;
    std::regex regex(pattern, std::regex::ECMAScript);
    
    for (const auto& entry : pImpl->entries) {
        if (std::regex_search(entry.key, regex)) {
            matches.push_back(entry.key);
        }
    }
    
    return matches;
}

std::vector<std::string> Config::findKeysByCategory(const std::string& category) const {
    std::vector<std::string> matches;
    
    for (const auto& param : pImpl->parameters) {
        if (param.second.category == category) {
            if (has(param.second.key)) {
                matches.push_back(param.second.key);
            }
        }
    }
    
    return matches;
}

std::unordered_map<std::string, ConfigValue> Config::findByPrefix(const std::string& prefix) const {
    std::unordered_map<std::string, ConfigValue> result;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(prefix) == 0) {
            result[entry.key] = entry.value;
        }
    }
    
    return result;
}

std::string Config::exportToString(const std::string& format) const {
    if (format == "summary") {
        return summary();
    } else if (format == "detailed") {
        return summaryDetailed();
    } else if (format == "json") {
        return exportToJSON();
    } else if (format == "ini") {
        return exportToINI();
    } else if (format == "yaml") {
        return exportToYAML();
    }
    return summary();
}

bool Config::importFromString(const std::string& str, const std::string& format) {
    if (format == "json") {
        auto imported = importFromJSON(str);
        if (imported) {
            merge(*imported, true);
            return true;
        }
    } else if (format == "ini") {
        auto imported = importFromINI(str);
        if (imported) {
            merge(*imported, true);
            return true;
        }
    } else if (format == "yaml") {
        auto imported = importFromYAML(str);
        if (imported) {
            merge(*imported, true);
            return true;
        }
    } else {
        // Simple format parsing
        std::istringstream iss(str);
        std::string line;
        while (std::getline(iss, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
    return false;
}

void Config::batchSet(const std::unordered_map<std::string, ConfigValue>& values) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, ConfigSource::Runtime);
    }
}

void Config::batchSet(const std::vector<std::pair<std::string, ConfigValue>>& values) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, ConfigSource::Runtime);
    }
}

std::unordered_map<std::string, ConfigValue> Config::getBatch(const std::vector<std::string>& keys) const {
    std::unordered_map<std::string, ConfigValue> result;
    
    for (const auto& key : keys) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            result[key] = it->value;
        }
    }
    
    return result;
}

bool Config::checkConsistency() const {
    return getConsistencyErrors().empty();
}

std::vector<std::string> Config::getConsistencyErrors() const {
    std::vector<std::string> errors;
    
    // Check parameter dependencies
    errors = checkParameterDependencies();
    
    // Check value ranges
    std::vector<std::string> rangeErrors = checkValueRanges();
    errors.insert(errors.end(), rangeErrors.begin(), rangeErrors.end());
    
    // Check value formats
    std::vector<std::string> formatErrors = checkValueFormats();
    errors.insert(errors.end(), formatErrors.begin(), formatErrors.end());
    
    return errors;
}

void Config::merge(const Config& other, bool overwrite) {
    for (const auto& entry : other.pImpl->entries) {
        if (overwrite || !has(entry.key)) {
            set(entry.key, entry.value, entry.source);
        }
    }
}

void Config::mergeFromString(const std::string& str, const std::string& format) {
    if (format == "json") {
        auto imported = importFromJSON(str);
        if (imported) {
            merge(*imported, true);
        }
    } else if (format == "ini") {
        auto imported = importFromINI(str);
        if (imported) {
            merge(*imported, true);
        }
    } else if (format == "yaml") {
        auto imported = importFromYAML(str);
        if (imported) {
            merge(*imported, true);
        }
    } else {
        loadFromString(str, format);
    }
}

std::string Config::createSnapshot() const {
    std::ostringstream oss;
    oss << "CONFIG_SNAPSHOT\n";
    oss << "Version: 1.0\n";
    oss << "Timestamp: " << std::chrono::system_clock::to_time_t(pImpl->lastModified) << "\n";
    oss << "Entries: " << pImpl->entries.size() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << "\n";
        oss << "Value: " << valueToString(entry.value) << "\n";
        oss << "Source: " << static_cast<int>(entry.source) << "\n";
        oss << "Description: " << entry.description << "\n";
        oss << "---\n";
    }
    
    return oss.str();
}

void Config::restoreFromSnapshot(const std::string& snapshot) {
    // Clear current config
    clear();
    
    // Parse snapshot (simplified)
    std::istringstream iss(snapshot);
    std::string line;
    std::string currentKey;
    std::string currentValue;
    std::string currentSource;
    std::string currentDescription;
    
    while (std::getline(iss, line)) {
        if (line.empty()) continue;
        
        if (line.find("Key: ") == 0) {
            if (!currentKey.empty()) {
                set(currentKey, currentValue, static_cast<ConfigSource>(std::stoi(currentSource)));
            }
            currentKey = line.substr(5);
            currentValue.clear();
            currentSource.clear();
            currentDescription.clear();
        } else if (line.find("Value: ") == 0) {
            currentValue = line.substr(7);
        } else if (line.find("Source: ") == 0) {
            currentSource = line.substr(8);
        } else if (line.find("Description: ") == 0) {
            currentDescription = line.substr(13);
        } else if (line == "---") {
            if (!currentKey.empty()) {
                set(currentKey, currentValue, static_cast<ConfigSource>(std::stoi(currentSource)));
                currentKey.clear();
            }
        }
    }
    
    if (!currentKey.empty()) {
        set(currentKey, currentValue, static_cast<ConfigSource>(std::stoi(currentSource)));
    }
}

std::string Config::diff(const Config& other) const {
    std::ostringstream oss;
    oss << "=== Configuration Differences ===\n\n";
    
    // Find keys in this config but not in other
    std::set<std::string> thisKeys, otherKeys;
    for (const auto& entry : pImpl->entries) thisKeys.insert(entry.key);
    for (const auto& entry : other.pImpl->entries) otherKeys.insert(entry.key);
    
    std::set<std::string> onlyInThis, onlyInOther;
    std::set_difference(thisKeys.begin(), thisKeys.end(), otherKeys.begin(), otherKeys.end(), std::inserter(onlyInThis, onlyInThis.begin()));
    std::set_difference(otherKeys.begin(), otherKeys.end(), thisKeys.begin(), thisKeys.end(), std::inserter(onlyInOther, onlyInOther.begin()));
    
    if (!onlyInThis.empty()) {
        oss << "Keys only in this config:\n";
        for (const auto& key : onlyInThis) {
            auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            if (it != pImpl->entries.end()) {
                oss << "  " << key << " = " << valueToString(it->value) << "\n";
            }
        }
        oss << "\n";
    }
    
    if (!onlyInOther.empty()) {
        oss << "Keys only in other config:\n";
        for (const auto& key : onlyInOther) {
            auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; });
            if (it != other.pImpl->entries.end()) {
                oss << "  " << key << " = " << valueToString(it->value) << "\n";
            }
        }
        oss << "\n";
    }
    
    // Find differing values in both configs
    oss << "Differing values:\n";
    for (const auto& key : onlyInThis) {
        auto thisIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (thisIt != pImpl->entries.end() && otherIt != other.pImpl->entries.end() &&
            !(thisIt->value == otherIt->value)) {
            oss << "  Key: " << key << "\n";
            oss << "    This: " << valueToString(thisIt->value) << "\n";
            oss << "    Other: " << valueToString(otherIt->value) << "\n";
        }
    }
    
    return oss.str();
}

// Private helper methods
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

bool Config::stringToInt(const std::string& str, int& value) {
    try {
        size_t pos;
        value = std::stoi(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool Config::stringToInt64(const std::string& str, int64_t& value) {
    try {
        size_t pos;
        value = std::stoll(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool Config::stringToDouble(const std::string& str, double& value) {
    try {
        size_t pos;
        value = std::stod(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool Config::stringToBool(const std::string& str, bool& value) {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "yes" || lower == "1") {
        value = true;
        return true;
    } else if (lower == "false" || lower == "no" || lower == "0") {
        value = false;
        return true;
    }
    return false;
}

bool Config::validateRange(const ConfigValue& value, double min, double max, bool inclusive) {
    double val = config_utils::toDouble(value);
    
    if (inclusive) {
        return val >= min && val <= max;
    } else {
        return val > min && val < max;
    }
}

bool Config::validateLength(const ConfigValue& value, size_t minLen, size_t maxLen, bool inclusive) {
    size_t len = config_utils::getVectorSize(value);
    if (std::holds_alternative<std::string>(value)) {
        len = std::get<std::string>(value).size();
    }
    
    if (inclusive) {
        return len >= minLen && len <= maxLen;
    } else {
        return len > minLen && len < maxLen;
    }
}

bool Config::validatePattern(const ConfigValue& value, const std::string& pattern, std::regex::flags_type flags) {
    if (!std::holds_alternative<std::string>(value)) {
        return false;
    }
    
    std::string str = std::get<std::string>(value);
    std::regex regex(pattern, flags);
    return std::regex_match(str, regex);
}

bool Config::validateOneOf(const ConfigValue& value, const std::vector<ConfigValue>& allowed) {
    return std::find(allowed.begin(), allowed.end(), value) != allowed.end();
}

void Config::notifyChangeListeners(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    for (const auto& listener : pImpl->changeListeners) {
        try {
            listener->onConfigChanged(key, oldValue, newValue);
        } catch (const std::exception& e) {
            // Log error but continue with other listeners
        }
    }
}

bool Config::validateAgainstSchema() const {
    return getConsistencyErrors().empty();
}

bool Config::validateParameterConsistency() const {
    for (const auto& param : pImpl->parameters) {
        if (param.second.required && !has(param.second.key)) {
            return false;
        }
        
        // Check dependencies
        for (const auto& dependency : param.second.dependencies) {
            if (!has(dependency)) {
                return false;
            }
        }
    }
    return true;
}

std::unordered_map<std::string, std::shared_ptr<Config>> Config::initializePresets() {
    std::unordered_map<std::string, std::shared_ptr<Config>> presets;
    
    // Default preset
    auto defaultConfig = createDefaultConfig();
    presets["default"] = defaultConfig;
    
    // Development preset
    auto devConfig = createDevelopmentConfig();
    presets["development"] = devConfig;
    
    // Production preset
    auto prodConfig = createProductionConfig();
    presets["production"] = prodConfig;
    
    // Research preset
    auto researchConfig = createResearchConfig();
    presets["research"] = researchConfig;
    
    return presets;
}

size_t Config::calculateChangedEntryCount() const {
    return static_cast<size_t>(pImpl->changedKeys.size());
}

std::chrono::system_clock::time_point Config::calculateLastModified() const {
    return pImpl->lastModified;
}

bool Config::compareValues(const ConfigValue& val1, const ConfigValue& val2) const {
    return val1 == val2;
}

void Config::applyBatchSet(const std::unordered_map<std::string, ConfigValue>& values, ConfigSource source) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, source);
    }
}

std::string Config::createDiffEntry(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) const {
    std::ostringstream oss;
    oss << "Key: " << key << "\n";
    oss << "  Old value: " << valueToString(oldValue) << "\n";
    oss << "  New value: " << valueToString(newValue) << "\n";
    return oss.str();
}

std::string Config::escapeString(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            case '\\': oss << "\\\\"; break;
            case '"': oss << "\\\""; break;
            case '\'': oss << "\\'"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string Config::unescapeString(const std::string& str) {
    std::ostringstream oss;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case 'n': oss << '\n'; break;
                case 'r': oss << '\r'; break;
                case 't': oss << '\t'; break;
                case '\\': oss << '\\'; break;
                case '"': oss << '"'; break;
                case '\'': oss << '\''; break;
                default: oss << str[i + 1]; break;
            }
            i++;  // Skip escaped character
        } else {
            oss << str[i];
        }
    }
    return oss.str();
}

std::string Config::createSnapshotContent(const Config& config) {
    std::ostringstream oss;
    oss << "CONFIG_SNAPSHOT\n";
    oss << "Version: 1.0\n";
    oss << "Timestamp: " << std::chrono::system_clock::to_time_t(config.pImpl->lastModified) << "\n";
    oss << "Entries: " << config.pImpl->entries.size() << "\n\n";
    
    for (const auto& entry : config.pImpl->entries) {
        oss << "Key: " << entry.key << "\n";
        oss << "Value: " << valueToString(entry.value) << "\n";
        oss << "Source: " << static_cast<int>(entry.source) << "\n";
        oss << "Description: " << entry.description << "\n";
        oss << "---\n";
    }
    
    return oss.str();
}

bool Config::restoreFromSnapshotContent(Config& config, const std::string& content) {
    // Clear current config
    config.clear();
    
    // Parse snapshot (simplified)
    std::istringstream iss(content);
    std::string line;
    std::string currentKey;
    std::string currentValue;
    std::string currentSource;
    std::string currentDescription;
    
    while (std::getline(iss, line)) {
        if (line.empty()) continue;
        
        if (line.find("Key: ") == 0) {
            if (!currentKey.empty()) {
                config.set(currentKey, currentValue, static_cast<ConfigSource>(std::stoi(currentSource)));
            }
            currentKey = line.substr(5);
            currentValue.clear();
            currentSource.clear();
            currentDescription.clear();
        } else if (line.find("Value: ") == 0) {
            currentValue = line.substr(7);
        } else if (line.find("Source: ") == 0) {
            currentSource = line.substr(8);
        } else if (line.find("Description: ") == 0) {
            currentDescription = line.substr(13);
        } else if (line == "---") {
            if (!currentKey.empty()) {
                config.set(currentKey, currentValue, static_cast<ConfigSource>(std::stoi(currentSource)));
                currentKey.clear();
            }
        }
    }
    
    if (!currentKey.empty()) {
        config.set(currentKey, currentValue, static_cast<ConfigSource>(std::stoi(currentSource)));
    }
    
    return true;
}

std::vector<std::string> Config::checkParameterDependencies() const {
    std::vector<std::string> errors;
    
    for (const auto& param : pImpl->parameters) {
        for (const auto& dependency : param.second.dependencies) {
            if (!has(dependency)) {
                errors.push_back("Parameter \"" + param.second.key + "\" depends on missing parameter \"" + dependency + "\"");
            }
        }
    }
    
    return errors;
}

std::vector<std::string> Config::checkValueRanges() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        auto paramIt = pImpl->parameters.find(entry.key);
        if (paramIt != pImpl->parameters.end()) {
            const auto& param = paramIt->second;
            std::string error = validateParameter(param, entry.value);
            if (!error.empty()) {
                errors.push_back(error);
            }
        }
    }
    
    return errors;
}

std::vector<std::string> Config::checkValueFormats() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (std::holds_alternative<std::string>(entry.value)) {
            const std::string& str = std::get<std::string>(entry.value);
            // Check for potential format issues
            if (str.empty()) {
                // Empty string might be valid for some parameters
                continue;
            }
        }
    }
    
    return errors;
}

// Explicit template instantiations
namespace nlm {
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
}

} // namespace nlm

// Type checking and conversion utilities
namespace config_utils {
    inline bool isNumberType(const ConfigValue& value) {
        return std::holds_alternative<int>(value) ||
               std::holds_alternative<int64_t>(value) ||
               std::holds_alternative<double>(value);
    }
    
    inline bool isStringType(const ConfigValue& value) {
        return std::holds_alternative<std::string>(value);
    }
    
    inline bool isBoolType(const ConfigValue& value) {
        return std::holds_alternative<bool>(value);
    }
    
    inline bool isVectorType(const ConfigValue& value) {
        return std::holds_alternative<std::vector<int>>(value) ||
               std::holds_alternative<std::vector<double>>(value) ||
               std::holds_alternative<std::vector<std::string>>(value);
    }
    
    inline double toDouble(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<double>(std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        return 0.0;
    }
    
    inline std::string toString(const ConfigValue& value) {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
        return std::to_string(toDouble(value));
    }
    
    inline bool toBool(const ConfigValue& value) {
        if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        }
        return toDouble(value) > 0.5;
    }
    
    inline size_t getVectorSize(const ConfigValue& value) {
        if (std::holds_alternative<std::vector<int>>(value)) {
            return std::get<std::vector<int>>(value).size();
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value).size();
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return std::get<std::vector<std::string>>(value).size();
        }
        return 0;
    }
}

// Configuration comparison operators
namespace nlm {
    inline bool operator==(const Config& a, const Config& b) {
        return a.equals(b);
    }
    
    inline bool operator!=(const Config& a, const Config& b) {
        return !(a == b);
    }
}

// Hash function for Config
namespace std {
    template<typename T>
    struct hash<std::reference_wrapper<const T>> {
        size_t operator()(const std::reference_wrapper<const T>& wrapper) const {
            return hash<const T*>{}(&wrapper.get());
        }
    };
}

// Convenience functions for parameter validation
namespace nlm {
    inline bool validateNeuronCount(const ConfigValue& value) {
        double count = config_utils::toDouble(value);
        return count > 0 && count <= 100000;  // Reasonable limits
    }
    
    inline bool validateSynapseDensity(const ConfigValue& value) {
        double density = config_utils::toDouble(value);
        return density >= 0.0 && density <= 1.0;  // Probability range
    }
    
    inline bool validateVoltage(const ConfigValue& value) {
        double voltage = config_utils::toDouble(value);
        return voltage >= -100.0 && voltage <= 50.0;  // Reasonable voltage range
    }
    
    inline bool validateTimeConstant(const ConfigValue& value) {
        double tau = config_utils::toDouble(value);
        return tau > 0 && tau <= 1000.0;  // Time constant range
    }
    
    inline bool validateLearningRate(const ConfigValue& value) {
        double rate = config_utils::toDouble(value);
        return rate >= 0.0 && rate <= 1.0;  // Learning rate range
    }
}

} // namespace nlm