#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <unordered_set>
#include <unordered_map>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigParameter> registeredParameters;
    std::vector<ConfigChangeCallback> changeCallbacks;
    std::unordered_map<std::string, ConfigValue> previousValues;
    bool validateOnSet;
    size_t maxValueHistory;
    
    Impl() : validateOnSet(true), maxValueHistory(10) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, ConfigSource source) {
    return loadFromFile(filepath, pImpl->validateOnSet);
}

bool Config::loadFromFile(const std::string& filepath, bool validateOnLoad) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        if (line.find('=') != std::string::npos) {
            std::string key = trim(line.substr(0, line.find('=')));
            std::string value = trim(line.substr(line.find('=') + 1));
            
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                     (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            ConfigValue configValue;
            
            if (value.find('.') != std::string::npos) {
                try {
                    configValue = std::stod(value);
                } catch (...) {
                    configValue = value;
                }
            } else {
                try {
                    if (value.find('e') != std::string::npos || value.find('E') != std::string::npos) {
                        configValue = std::stod(value);
                    } else {
                        size_t pos;
                        long long_val = std::stoll(value, &pos);
                        if (pos == value.size()) {
                            configValue = long_val;
                        } else {
                            configValue = value;
                        }
                    }
                } catch (...) {
                    bool bool_val;
                    if (value == "true" || value == "1") {
                        bool_val = true;
                        configValue = bool_val;
                    } else if (value == "false" || value == "0") {
                        bool_val = false;
                        configValue = bool_val;
                    } else {
                        configValue = value;
                    }
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return loadFromArgs(args);
}

bool Config::loadFromArgs(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && arg.size() > 1) {
            std::string key = arg.substr(1);
            // Handle cases like -key value
            // In this simple implementation, we assume key=value format
            // For dash arguments with values, we need to check if the next arg is not another option
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (prettyPrint) {
        file << "# NLM Configuration File\n";
        file << "# Generated on " << std::time(nullptr) << "\n\n";
        file << "# Configuration entries with descriptions:\n\n";
    }
    
    for (const auto& entry : pImpl->entries) {
        if (prettyPrint) {
            file << "# " << entry.key << " - " << entry.description;
            if (!entry.validationPattern.empty()) {
                file << " [pattern: " << entry.validationPattern << "]";
            }
            file << "\n";
        }
        
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        if (prettyPrint) {
            file << "# Source: " << static_cast<int>(entry.source) << "\n\n";
        }
    }
    
    return true;
}

bool Config::exportConfig(const std::string& filepath, const std::vector<std::string>& filterKeys) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NLM Configuration Export\n";
    file << "Generated on: " << std::time(nullptr) << "\n\n";
    
    auto entries = pImpl->entries;
    if (!filterKeys.empty()) {
        std::unordered_set<std::string> filterSet(filterKeys.begin(), filterKeys.end());
        entries.erase(std::remove_if(entries.begin(), entries.end(),
            [&filterSet](const ConfigEntry& e) { return filterSet.find(e.key) == filterSet.end(); }),
            entries.end());
    }
    
    file << "Total entries: " << entries.size() << "\n\n";
    
    for (const auto& entry : entries) {
        file << "Key: " << entry.key << "\n";
        file << "Value: ";
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n";
        file << "Source: " << static_cast<int>(entry.source) << "\n";
        if (!entry.description.empty()) {
            file << "Description: " << entry.description << "\n";
        }
        file << "\n";
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    bool changed = false;
    ConfigValue oldValue;
    
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
        if (oldValue == value) {
            it->source = source;
            return;
        }
        it->value = value;
        it->source = source;
        changed = true;
    } else {
        ConfigEntry newEntry(key, value, source);
        pImpl->entries.push_back(newEntry);
        changed = true;
        if (pImpl->validateOnSet && !newEntry.validate()) {
            pImpl->entries.pop_back();
            return;
        }
    }
    
    if (changed) {
        if (pImpl->maxValueHistory > 0) {
            if (pImpl->previousValues.find(key) != pImpl->previousValues.end()) {
                if (pImpl->previousValues.size() >= pImpl->maxValueHistory) {
                    auto itOldest = pImpl->previousValues.begin();
                    pImpl->previousValues.erase(itOldest);
                }
            }
            pImpl->previousValues[key] = oldValue;
        }
        
        for (const auto& callback : pImpl->changeCallbacks) {
            callback(key, oldValue, value);
        }
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

void Config::registerParameter(const ConfigParameter& param) {
    pImpl->registeredParameters[param.key] = param;
}

void Config::unregisterParameter(const std::string& key) {
    pImpl->registeredParameters.erase(key);
}

bool Config::validate(const std::string& key) const {
    if (key.empty()) {
        for (const auto& entry : pImpl->entries) {
            if (!validateEntry(entry)) {
                return false;
            }
        }
        return true;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    return (it != pImpl->entries.end()) && validateEntry(*it);
}

std::vector<std::string> Config::getValidationErrors(const std::string& key) const {
    std::vector<std::string> errors;
    
    auto validateSingle = [&errors](const ConfigEntry& entry) {
        if (!entry.validate()) {
            errors.push_back("Validation failed for key: " + entry.key);
        }
        auto valErrors = validateValue(entry.value, nullptr);
        errors.insert(errors.end(), valErrors.begin(), valErrors.end());
    };
    
    if (key.empty()) {
        for (const auto& entry : pImpl->entries) {
            validateSingle(entry);
        }
    } else {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            validateSingle(*it);
        }
    }
    
    return errors;
}

std::vector<std::string> Config::getMissingRequiredKeys() const {
    std::vector<std::string> missing;
    
    for (const auto& param : pImpl->registeredParameters) {
        if (param.second.required && !has(param.first)) {
            missing.push_back(param.first);
        }
    }
    
    return missing;
}

bool Config::saveTemplate(const std::string& name, const std::vector<std::string>& includeKeys) const {
    std::string filepath = getDefaultConfigPath(name + ".template");
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration Template: " << name << "\n";
    file << "# Generated automatically\n\n";
    
    auto entries = pImpl->entries;
    if (!includeKeys.empty()) {
        std::unordered_set<std::string> filterSet(includeKeys.begin(), includeKeys.end());
        entries.erase(std::remove_if(entries.begin(), entries.end(),
            [&filterSet](const ConfigEntry& e) { return filterSet.find(e.key) == filterSet.end(); }),
            entries.end());
    }
    
    for (const auto& entry : entries) {
        file << "# " << entry.key << ": " << entry.description << "\n";
        file << "# Required: " << (entry.required ? "yes" : "no") << "\n";
        if (!entry.validationPattern.empty()) {
            file << "# Pattern: " << entry.validationPattern << "\n";
        }
        file << "# Default source: " << static_cast<int>(entry.source) << "\n";
        file << "\n";
    }
    
    file << "# Instructions for use:\n";
    file << "# 1. Copy this file to your project's config directory\n";
    file << "# 2. Modify values as needed\n";
    file << "# 3. Load with --config=filename.template\n";
    
    return true;
}

bool Config::loadTemplate(const std::string& name) {
    std::string filepath = getDefaultConfigPath(name);
    if (filepath.empty()) {
        return false;
    }
    
    return loadFromFile(filepath);
}

std::vector<std::string> Config::getAvailableTemplates() const {
    std::vector<std::string> templates;
    std::string configDir = getConfigDirectory();
    
    if (std::filesystem::exists(configDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(configDir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.size() >= 9 && filename.substr(filename.size() - 9) == ".template") {
                    templates.push_back(filename.substr(0, filename.size() - 9));
                }
            }
        }
    }
    
    return templates;
}

void Config::set(const std::string& key, const std::string& value, const std::string& unit) {
    std::string valueWithUnit = value;
    if (!unit.empty()) {
        valueWithUnit += " " + unit;
    }
    set(key, ConfigValue(valueWithUnit), ConfigSource::Runtime);
}

void Config::increment(const std::string& key, double amount) {
    auto current = get<double>(key);
    if (current.has_value()) {
        set(key, current.value() + amount, ConfigSource::Runtime);
    } else {
        set(key, amount, ConfigSource::Runtime);
    }
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        auto existing = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (existing == pImpl->entries.end()) {
            pImpl->entries.push_back(entry);
        } else {
            existing->value = entry.value;
            existing->source = source;
        }
    }
}

std::string Config::diff(const Config& other) const {
    std::ostringstream oss;
    oss << "Configuration Differences:\n";
    oss << "==========================\n\n";
    
    std::unordered_map<std::string, const ConfigEntry*> thisMap;
    for (const auto& entry : pImpl->entries) {
        thisMap[entry.key] = &entry;
    }
    
    std::unordered_map<std::string, const ConfigEntry*> otherMap;
    for (const auto& entry : other.pImpl->entries) {
        otherMap[entry.key] = &entry;
    }
    
    bool hasDiff = false;
    
    for (const auto& pair : thisMap) {
        const std::string& key = pair.first;
        const ConfigEntry* thisEntry = pair.second;
        
        auto otherIt = otherMap.find(key);
        if (otherIt == otherMap.end()) {
            oss << "Key \"" << key << "\" exists in this config but not in other\n";
            hasDiff = true;
            continue;
        }
        
        const ConfigEntry* otherEntry = otherIt->second;
        
        if (thisEntry->value != otherEntry->value || thisEntry->source != otherEntry->source) {
            oss << "Difference for key \"" << key << "\":\n";
            oss << "  This config: ";
            std::visit([&oss](const auto& arg) { oss << arg; }, thisEntry->value);
            oss << " (source: " << static_cast<int>(thisEntry->source) << ")\n";
            oss << "  Other config: ";
            std::visit([&oss](const auto& arg) { oss << arg; }, otherEntry->value);
            oss << " (source: " << static_cast<int>(otherEntry->source) << ")\n";
            hasDiff = true;
        }
    }
    
    for (const auto& pair : otherMap) {
        if (thisMap.find(pair.first) == thisMap.end()) {
            oss << "Key \"" << pair.first << "\" exists in other config but not in this\n";
            hasDiff = true;
        }
    }
    
    if (!hasDiff) {
        oss << "No differences found.\n";
    }
    
    return oss.str();
}

bool Config::isIdentical(const Config& other) const {
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

std::string Config::getStats() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "======================\n\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Registered parameters: " << pImpl->registeredParameters.size() << "\n";
    oss << "Change callbacks: " << pImpl->changeCallbacks.size() << "\n";
    oss << "Value history size: " << pImpl->previousValues.size() << "\n";
    oss << "Validation on set: " << (pImpl->validateOnSet ? "enabled" : "disabled") << "\n";
    
    std::unordered_map<ConfigSource, int> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    oss << "\nEntries by source:\n";
    for (const auto& pair : sourceCounts) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "\nMemory usage: ~" << (sizeof(Impl) + pImpl->entries.size() * sizeof(ConfigEntry)) << " bytes\n";
    
    return oss.str();
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

double Config::getMemoryUsage() const {
    return sizeof(Impl) + pImpl->entries.size() * sizeof(ConfigEntry);
}

void Config::addChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.push_back(callback);
}

void Config::removeChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.erase(std::remove(pImpl->changeCallbacks.begin(), pImpl->changeCallbacks.end(), callback),
        pImpl->changeCallbacks.end());
}

void Config::clearChangeCallbacks() {
    pImpl->changeCallbacks.clear();
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
    pImpl->previousValues.erase(key);
}

void Config::resetToDefault(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    if (it != pImpl->entries.end()) {
        auto registeredIt = pImpl->registeredParameters.find(key);
        if (registeredIt != pImpl->registeredParameters.end()) {
            it->value = registeredIt->second.defaultValue;
            it->source = ConfigSource::Default;
        } else {
            remove(key);
        }
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

std::vector<ConfigEntry> Config::getEntries() const {
    return pImpl->entries;
}

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
}

std::vector<ConfigEntry> Config::getEntriesByPattern(const std::string& pattern) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (matchesPattern(entry.key, pattern)) {
            result.push_back(entry);
        }
    }
    return result;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->previousValues.clear();
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

std::string Config::debugInfo() const {
    std::ostringstream oss;
    oss << "=== Debug Information ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Registered parameters: " << pImpl->registeredParameters.size() << "\n";
    oss << "Change callbacks: " << pImpl->changeCallbacks.size() << "\n";
    oss << "Value history size: " << pImpl->previousValues.size() << "\n";
    oss << "Validation on set: " << (pImpl->validateOnSet ? "enabled" : "disabled") << "\n\n";
    
    oss << "Recent entries:\n";
    for (size_t i = 0; i < std::min(pImpl->entries.size(), size_t(5)); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  " << entry.key << " = ";
        std::visit([&oss](const auto& arg) { 
            using T = std::decay_t<decltype(arg)>; 
            if constexpr (std::is_same_v<T, std::string>) oss << "\"" << arg << "\""; 
            else oss << arg; 
        }, entry.value);
        oss << " (source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    if (!pImpl->previousValues.empty()) {
        oss << "\nPrevious values:\n";
        for (const auto& pair : pImpl->previousValues) {
            oss << "  " << pair.first << " -> ";
            std::visit([&oss](const auto& arg) {
                using T = std::decay_t<decltype(arg)>; 
                if constexpr (std::is_same_v<T, std::string>) oss << "\"" << arg << "\""; 
                else oss << arg; 
            }, pair.second);
            oss << "\n";
        }
    }
    
    return oss.str();
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

bool Config::validateEntry(const ConfigEntry& entry) const {
    if (!entry.validate()) {
        return false;
    }
    
    auto valErrors = validateValue(entry.value, nullptr);
    return valErrors.empty();
}

std::vector<std::string> Config::validateValue(const ConfigValue& value, const ConfigParameter* param) {
    std::vector<std::string> errors;
    
    std::visit([&errors, param](const auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if (param) {
            if (param->validationPattern.size() > 0 && !matchesPattern(
                std::get<std::string>(value), param->validationPattern)) {
                errors.push_back("Value does not match pattern: " + param->validationPattern);
            }
        }
        
        if constexpr (std::is_same_v<T, std::string>) {
            if (arg.empty()) {
                errors.push_back("String value cannot be empty");
            }
            if (arg.find('\n') != std::string::npos || arg.find('\r') != std::string::npos) {
                errors.push_back("String value contains newline characters");
            }
        }
        
        if constexpr (std::is_same_v<T, int>) {
            if (arg < 0) {
                errors.push_back("Integer value cannot be negative");
            }
        }
        
        if constexpr (std::is_same_v<T, double>) {
            if (std::isnan(arg) || std::isinf(arg)) {
                errors.push_back("Double value is NaN or infinity");
            }
        }
    }, value);
    
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool Config::matchesPattern(const std::string& str, const std::string& pattern) {
    if (pattern.empty()) return true;
    if (pattern == "*" || pattern == "ALL") return true;
    if (pattern[0] == '*') {
        size_t pos = str.find(pattern.substr(1));
        return pos != std::string::npos;
    }
    return str.find(pattern) != std::string::npos;
}

std::string Config::parseConfigKey(const std::string& arg) {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        return arg.substr(0, pos);
    }
    return arg;
}

std::string Config::parseConfigValue(const std::string& arg) {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        return arg.substr(pos + 1);
    }
    return "";
}

std::string Config::getConfigDirectory() {
    const char* home = std::getenv("HOME");
    if (home) {
        return std::string(home) + "/.nlm/config";
    }
    return ".";
}

std::string Config::getDefaultConfigPath(const std::string& name) {
    std::string configDir = getConfigDirectory();
    std::string extension = name.size() >= 9 && name.substr(name.size() - 9) == ".template" ? "" : ".cfg";
    return configDir + "/" + name + extension;
}