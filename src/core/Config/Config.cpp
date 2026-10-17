#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <set>
#include <chrono>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> presets;
    ConfigStatistics statistics;
    bool presetsLoaded = false;
    
    Impl() {
        if (!presetsLoaded) {
            loadPresetsFromDirectory();
        }
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validate) {
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
            
            if (key.empty() || value.empty()) {
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                     (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to detect value type
            ConfigValue configValue;
            if (value == "true" || value == "false") {
                configValue = (value == "true");
            } else {
                // Try to parse as number
                try {
                    if (value.find('.') != std::string::npos) {
                        configValue = std::stod(value);
                    } else {
                        configValue = std::stoll(value);
                    }
                } catch (...) {
                    // Try as vector if contains brackets
                    if (value.front() == '[' && value.back() == ']') {
                        // Parse simple vector
                        std::string inner = value.substr(1, value.size() - 2);
                        if (inner.empty()) {
                            configValue = std::vector<int>{};
                        } else {
                            // Simple parsing - could be enhanced for complex types
                            configValue = std::vector<std::string>{};
                        }
                    } else {
                        configValue = value;
                    }
                }
            }
            
            // Extract description from comments
            std::string description = "";
            // Look for comment before this line (simplified)
            
            set(key, configValue, description, ConfigSource::File);
        }
    }
    
    pImpl->statistics.update(pImpl->entries);
    return true;
}

bool Config::loadFromFileWithPresets(const std::string& filepath) {
    // Load preset file first if exists
    std::string presetFile = filepath + ".presets";
    if (std::filesystem::exists(presetFile)) {
        std::ifstream presetStream(presetFile);
        if (presetStream.is_open()) {
            std::string line;
            while (std::getline(presetStream, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#') continue;
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string presetName = trim(line.substr(0, pos));
                    std::string presetPath = trim(line.substr(pos + 1));
                    pImpl->presets[presetName] = presetPath;
                }
            }
        }
    }
    
    return loadFromFile(filepath);
}

bool Config::loadFromArgs(int argc, char** argv, bool validate) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Try to detect value type
                ConfigValue configValue;
                if (value == "true" || value == "false") {
                    configValue = (value == "true");
                } else {
                    try {
                        if (value.find('.') != std::string::npos) {
                            configValue = std::stod(value);
                        } else {
                            configValue = std::stoll(value);
                        }
                    } catch (...) {
                        if (value.front() == '[' && value.back() == ']') {
                            configValue = std::vector<std::string>{};
                        } else {
                            configValue = value;
                        }
                    }
                }
                
                set(key, configValue, "Command line argument", ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            ConfigValue configValue;
            if (value == "true" || value == "false") {
                configValue = (value == "true");
            } else {
                try {
                    if (value.find('.') != std::string::npos) {
                        configValue = std::stod(value);
                    } else {
                        configValue = std::stoll(value);
                    }
                } catch (...) {
                    configValue = value;
                }
            }
            
            set(key, configValue, "Command line argument", ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    // Simple environment loading - could be enhanced
    return true;
}

bool Config::loadFromString(const std::string& configString) {
    std::istringstream stream(configString);
    std::string line;
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (key.empty() || value.empty()) continue;
            
            ConfigValue configValue;
            if (value == "true" || value == "false") {
                configValue = (value == "true");
            } else {
                try {
                    if (value.find('.') != std::string::npos) {
                        configValue = std::stod(value);
                    } else {
                        configValue = std::stoll(value);
                    }
                } catch (...) {
                    configValue = value;
                }
            }
            
            set(key, configValue, ConfigSource::Runtime);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimeString() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        if (!entry.validationRules.empty()) {
            file << "# Validation: " << entry.validationRules << "\n";
        }
        if (entry.isRequired) {
            file << "# Required: Yes\n";
        }
        if (entry.isReadOnly) {
            file << "# Read-only: Yes\n";
        }
        file << entry.key << " = " << entry.getStringValue() << "\n\n";
    }
    
    // Save presets
    if (!pImpl->presets.empty()) {
        file << "# Presets\n";
        for (const auto& preset : pImpl->presets) {
            file << preset.first << " = " << preset.second << "\n";
        }
        file << "\n";
    }
    
    return true;
}

std::string Config::saveToString() const {
    std::ostringstream oss;
    oss << "# NLM Configuration\n";
    oss << "# Generated on " << getCurrentTimeString() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << " = " << entry.getStringValue() << "\n";
    }
    
    return oss.str();
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
    set(key, value, "", source, "", "", false, false);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<bool>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const ConfigValue& value, const std::string& description,
                 ConfigSource source, const std::string& validationRules,
                 const std::string& defaultValue, bool isRequired, bool isReadOnly) {
    if (!isValidKey(key)) {
        throw std::invalid_argument("Invalid configuration key: " + key);
    }
    
    if (!validateValue(key, value)) {
        throw std::invalid_argument("Invalid configuration value for key: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
        it->description = description;
        it->validationRules = validationRules;
        it->defaultValue = defaultValue;
        it->isRequired = isRequired;
        it->isReadOnly = isReadOnly;
        it->timestamp = std::chrono::system_clock::now();
    } else {
        // Add new entry
        ConfigEntry newEntry(key, value, source, description, validationRules, defaultValue, isRequired, isReadOnly);
        pImpl->entries.push_back(newEntry);
    }
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        // Validate key format
        if (!validateKey(entry.key)) {
            result.addError("Invalid key format: " + entry.key);
        }
        
        // Validate value type
        if (!validateValue(entry.key, entry.value)) {
            result.addError("Invalid value for key: " + entry.key);
        }
        
        // Validate required keys
        if (entry.isRequired && !has(entry.key)) {
            result.addError("Required key is missing: " + entry.key);
        }
        
        // Validate ranges if specified
        if (!entry.validationRules.empty() && !validateRange(entry.key, entry.value)) {
            result.addWarning("Range validation failed for key: " + entry.key);
        }
    }
    
    // Check for duplicate keys
    std::set<std::string> keySet;
    for (const auto& entry : pImpl->entries) {
        if (keySet.count(entry.key) > 0) {
            result.addError("Duplicate key found: " + entry.key);
        }
        keySet.insert(entry.key);
    }
    
    return result;
}

bool Config::validateKey(const std::string& key) const {
    if (key.empty()) return false;
    if (key.find('=') != std::string::npos) return false;
    if (key.find(' ') != std::string::npos) return false;
    if (key.find('\t') != std::string::npos) return false;
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Basic type checking
    if (std::holds_alternative<int>(value)) {
        auto intVal = std::get<int>(value);
        return intVal >= -1000000 && intVal <= 1000000;
    } else if (std::holds_alternative<int64_t>(value)) {
        auto int64Val = std::get<int64_t>(value);
        return int64Val >= -1000000000000 && int64Val <= 1000000000000;
    } else if (std::holds_alternative<double>(value)) {
        auto doubleVal = std::get<double>(value);
        return std::isfinite(doubleVal) && doubleVal >= -1e9 && doubleVal <= 1e9;
    } else if (std::holds_alternative<bool>(value)) {
        return true;
    } else if (std::holds_alternative<std::string>(value)) {
        auto& strVal = std::get<std::string>(value);
        return strVal.length() <= 1000;
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        auto& vecVal = std::get<std::vector<int>>(value);
        return vecVal.size() <= 1000;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        auto& vecVal = std::get<std::vector<double>>(value);
        return vecVal.size() <= 1000;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        auto& vecVal = std::get<std::vector<std::string>>(value);
        return vecVal.size() <= 1000;
    } else if (std::holds_alternative<std::vector<bool>>(value)) {
        auto& vecVal = std::get<std::vector<bool>>(value);
        return vecVal.size() <= 1000;
    }
    
    return false;
}

ValidationResult Config::validateSchema(const std::vector<std::pair<std::string, std::string>>& schema) const {
    ValidationResult result;
    
    for (const auto& schemaItem : schema) {
        const std::string& key = schemaItem.first;
        const std::string& expectedType = schemaItem.second;
        
        if (!has(key)) {
            if (expectedType.find("required") != std::string::npos) {
                result.addError("Required key not found in schema: " + key);
            }
            continue;
        }
        
        auto val = get<std::string>(key);
        if (!val.has_value()) {
            result.addError("Key not found in schema: " + key);
            continue;
        }
        
        // Check type compatibility
        if (expectedType == "int" && !std::holds_alternative<int>(getEntry(key)->value)) {
            result.addError("Key " + key + " should be int type, got " + typeName(val.value()));
        } else if (expectedType == "double" && !std::holds_alternative<double>(getEntry(key)->value)) {
            result.addError("Key " + key + " should be double type, got " + typeName(val.value()));
        } else if (expectedType == "string" && !std::holds_alternative<std::string>(getEntry(key)->value)) {
            result.addError("Key " + key + " should be string type, got " + typeName(val.value()));
        } else if (expectedType == "bool" && !std::holds_alternative<bool>(getEntry(key)->value)) {
            result.addError("Key " + key + " should be bool type, got " + typeName(val.value()));
        }
    }
    
    return result;
}

bool Config::validateRange(const std::string& key, const ConfigValue& value) const {
    auto entry = getEntry(key);
    if (!entry || entry->validationRules.empty()) {
        return true;
    }
    
    std::string rules = entry->validationRules;
    
    if (rules.find("range:") == 0) {
        std::string rangePart = rules.substr(7);
        size_t commaPos = rangePart.find(',');
        if (commaPos != std::string::npos) {
            std::string minStr = rangePart.substr(0, commaPos);
            std::string maxStr = rangePart.substr(commaPos + 1);
            
            try {
                double minVal = std::stod(minStr);
                double maxVal = std::stod(maxStr);
                
                if (std::holds_alternative<double>(value)) {
                    double val = std::get<double>(value);
                    return val >= minVal && val <= maxVal;
                } else if (std::holds_alternative<int>(value)) {
                    int val = std::get<int>(value);
                    return val >= static_cast<int>(minVal) && val <= static_cast<int>(maxVal);
                }
            } catch (...) {
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::string> Config::getAvailablePresets() {
    loadPresetsFromDirectory();
    std::vector<std::string> presetNames;
    presetNames.reserve(pImpl->presets.size());
    
    for (const auto& preset : pImpl->presets) {
        presetNames.push_back(preset.first);
    }
    
    return presetNames;
}

bool Config::loadPreset(const std::string& presetName) {
    if (pImpl->presets.find(presetName) == pImpl->presets.end()) {
        return false;
    }
    
    std::string presetPath = pImpl->presets[presetName];
    return loadFromFile(presetPath);
}

void Config::saveAsPreset(const std::string& presetName) const {
    std::string presetDir = getPresetDirectory();
    if (!std::filesystem::exists(presetDir)) {
        std::filesystem::create_directories(presetDir);
    }
    
    std::string presetPath = getPresetFilePath(presetName);
    saveToFile(presetPath);
    
    // Update preset index
    std::string indexPath = presetDir + "/presets.index";
    std::ofstream indexFile(indexPath, std::ios::app);
    if (indexFile.is_open()) {
        indexFile << presetName << " = " << presetPath << "\n";
        indexFile.close();
    }
    
    pImpl->presets[presetName] = presetPath;
}

void Config::removePreset(const std::string& presetName) {
    std::string presetPath = getPresetFilePath(presetName);
    if (std::filesystem::exists(presetPath)) {
        std::filesystem::remove(presetPath);
    }
    
    pImpl->presets.erase(presetName);
    
    // Update index file
    std::string indexPath = getPresetDirectory() + "/presets.index";
    if (std::filesystem::exists(indexPath)) {
        std::ifstream indexFile(indexPath);
        std::string line;
        std::vector<std::string> remainingLines;
        while (std::getline(indexFile, line)) {
            if (line.find(presetName + " =") == std::string::npos) {
                remainingLines.push_back(line);
            }
        }
        indexFile.close();
        
        std::ofstream indexFileOut(indexPath);
        for (const auto& remainingLine : remainingLines) {
            indexFileOut << remainingLine << "\n";
        }
        indexFileOut.close();
    }
}

void Config::createDefaultPreset() {
    Config config;
    config.set("neuron_count", 1000, "Default neuron count", ConfigSource::Default);
    config.set("region_count", 1, "Default region count", ConfigSource::Default);
    config.set("connection_probability", 0.1f, "Default connection probability", ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.01f, "STDP LTP weight", ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.012f, "STDP LTD weight", ConfigSource::Default);
    config.set("simulation_timestep", 0.001, "Simulation timestep", ConfigSource::Default);
    
    config.saveAsPreset("default");
}

void Config::createPerformancePreset() {
    Config config;
    config.set("neuron_count", 500, "Performance neuron count", ConfigSource::Default);
    config.set("region_count", 1, "Performance region count", ConfigSource::Default);
    config.set("connection_probability", 0.05f, "Performance connection probability", ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.005f, "Performance STDP LTP weight", ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.006f, "Performance STDP LTD weight", ConfigSource::Default);
    config.set("simulation_timestep", 0.002, "Performance simulation timestep", ConfigSource::Default);
    
    config.saveAsPreset("performance");
}

void Config::createAccuracyPreset() {
    Config config;
    config.set("neuron_count", 5000, "Accuracy neuron count", ConfigSource::Default);
    config.set("region_count", 2, "Accuracy region count", ConfigSource::Default);
    config.set("connection_probability", 0.2f, "Accuracy connection probability", ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.02f, "Accuracy STDP LTP weight", ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.018f, "Accuracy STDP LTD weight", ConfigSource::Default);
    config.set("simulation_timestep", 0.0005, "Accuracy simulation timestep", ConfigSource::Default);
    
    config.saveAsPreset("accuracy");
}

void Config::createMinimalPreset() {
    Config config;
    config.set("neuron_count", 100, "Minimal neuron count", ConfigSource::Default);
    config.set("region_count", 1, "Minimal region count", ConfigSource::Default);
    config.set("connection_probability", 0.01f, "Minimal connection probability", ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.01f, "Minimal STDP LTP weight", ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.012f, "Minimal STDP LTD weight", ConfigSource::Default);
    config.set("simulation_timestep", 0.001, "Minimal simulation timestep", ConfigSource::Default);
    
    config.saveAsPreset("minimal");
}

std::vector<std::string> Config::findKeysContaining(const std::string& substring) const {
    std::vector<std::string> keys;
    std::string lowerSubstring = toLower(substring);
    
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key).find(lowerSubstring) != std::string::npos) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

std::vector<std::string> Config::findKeysMatching(const std::string& pattern) const {
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        if (patternMatch(entry.key, pattern)) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

std::vector<std::string> Config::getKeysByType(const std::string& type) const {
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        if (getTypeName(entry.value) == type) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

std::vector<ConfigEntry> Config::filterBySource(ConfigSource source) const {
    std::vector<ConfigEntry> filtered;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            filtered.push_back(entry);
        }
    }
    
    return filtered;
}

std::vector<ConfigEntry> Config::filterByType(const std::string& type) const {
    std::vector<ConfigEntry> filtered;
    
    for (const auto& entry : pImpl->entries) {
        if (getTypeName(entry.value) == type) {
            filtered.push_back(entry);
        }
    }
    
    return filtered;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.getAllEntries()) {
        set(entry.key, entry.value, entry.description, source, 
            entry.validationRules, entry.defaultValue, entry.isRequired, entry.isReadOnly);
    }
}

bool Config::isIdentical(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherEntry = other.getEntry(entry.key);
        if (!otherEntry || entry.value != otherEntry->value) {
            return false;
        }
    }
    
    return true;
}

double Config::similarity(const Config& other) const {
    if (pImpl->entries.empty() || other.pImpl->entries.empty()) {
        return 0.0;
    }
    
    int matches = 0;
    int total = std::max(pImpl->entries.size(), other.pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        auto otherEntry = other.getEntry(entry.key);
        if (otherEntry && entry.value == otherEntry->value) {
            matches++;
        }
    }
    
    return static_cast<double>(matches) / total;
}

std::vector<std::string> Config::getDifferences(const Config& other) const {
    std::vector<std::string> differences;
    
    std::set<std::string> thisKeys, otherKeys;
    for (const auto& entry : pImpl->entries) {
        thisKeys.insert(entry.key);
    }
    for (const auto& entry : other.pImpl->entries) {
        otherKeys.insert(entry.key);
    }
    
    // Keys in this but not in other
    for (const auto& key : thisKeys) {
        if (otherKeys.find(key) == otherKeys.end()) {
            differences.push_back("Missing in other config: " + key);
        }
    }
    
    // Keys in other but not in this
    for (const auto& key : otherKeys) {
        if (thisKeys.find(key) == thisKeys.end()) {
            differences.push_back("Extra in other config: " + key);
        }
    }
    
    // Compare common keys
    for (const auto& key : thisKeys) {
        if (otherKeys.find(key) != otherKeys.end()) {
            auto thisEntry = getEntry(key);
            auto otherEntry = other.getEntry(key);
            if (thisEntry && otherEntry && thisEntry->value != otherEntry->value) {
                differences.push_back("Different value for " + key + ": " + 
                                    getValueString(thisEntry->value) + " vs " + 
                                    getValueString(otherEntry->value));
            }
        }
    }
    
    return differences;
}

ConfigStatistics Config::getStatistics() const {
    ConfigStatistics stats;
    stats.update(pImpl->entries);
    return stats;
}

std::chrono::system_clock::time_point Config::getLastModified() const {
    if (pImpl->entries.empty()) {
        return std::chrono::system_clock::now();
    }
    
    std::chrono::system_clock::time_point latest = pImpl->entries[0].timestamp;
    for (const auto& entry : pImpl->entries) {
        if (entry.timestamp > latest) {
            latest = entry.timestamp;
        }
    }
    
    return latest;
}

bool Config::isEmpty() const {
    return pImpl->entries.empty();
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
    pImpl->statistics.update(pImpl->entries);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::optional<ConfigEntry> Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return *it;
}

std::vector<ConfigEntry> Config::getAllEntries() const {
    return pImpl->entries;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->statistics.update(pImpl->entries);
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "=== Configuration Summary ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Unique keys: " << getUniqueKeyCount() << "\n";
    oss << "Memory usage: " << pImpl->statistics.memoryUsage << " bytes\n";
    oss << "Last modified: " << getTimeString(getLastModified()) << "\n\n";
    
    oss << "Entries by source:\n";
    for (const auto& pair : pImpl->statistics.entriesBySource) {
        oss << "  " << getSourceString(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "\nEntries by type:\n";
    for (const auto& pair : pImpl->statistics.keysByType) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

std::string Config::getFormattedEntryList() const {
    std::ostringstream oss;
    oss << std::left << std::setw(20) << "Key" 
        << std::setw(15) << "Type" 
        << std::setw(12) << "Source" 
        << std::setw(10) << "Required" 
        << std::setw(10) << "ReadOnly" 
        << "Description" << std::endl;
    oss << std::string(80, '-') << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        oss << std::left << std::setw(20) << entry.key
            << std::setw(15) << getTypeName(entry.value)
            << std::setw(12) << getSourceString(entry.source)
            << std::setw(10) << (entry.isRequired ? "Yes" : "No")
            << std::setw(10) << (entry.isReadOnly ? "Yes" : "No")
            << entry.description << std::endl;
    }
    
    return oss.str();
}

std::string Config::getSourceString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        case ConfigSource::Preset: return "Preset";
        case ConfigSource::Environment: return "Environment";
        case ConfigSource::Compilation: return "Compilation";
        default: return "Unknown";
    }
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

bool Config::isValidKey(const std::string& key) {
    if (key.empty() || key.length() > 100) return false;
    if (key.find(' ') != std::string::npos) return false;
    if (key.find('\t') != std::string::npos) return false;
    if (key.find('\n') != std::string::npos) return false;
    if (key.find('=') != std::string::npos) return false;
    return true;
}

bool Config::isValidValue(const ConfigValue& value) {
    // Already validated in the variant, but check for specific cases
    if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (const auto& str : vec) {
            if (str.length() > 1000) return false;
        }
    }
    return true;
}

bool Config::validateIntRange(const ConfigValue& value, const std::string& rules) {
    if (!std::holds_alternative<int>(value)) return false;
    // Parse rules and validate
    return true;
}

bool Config::validateDoubleRange(const ConfigValue& value, const std::string& rules) {
    if (!std::holds_alternative<double>(value)) return false;
    // Parse rules and validate
    return true;
}

bool Config::validateStringLength(const ConfigValue& value, const std::string& rules) {
    if (!std::holds_alternative<std::string>(value)) return false;
    // Parse rules and validate
    return true;
}

bool Config::validateEnum(const ConfigValue& value, const std::string& rules) {
    // Parse rules and validate
    return true;
}

bool Config::validateArraySize(const ConfigValue& value, const std::string& rules) {
    // Parse rules and validate
    return true;
}

std::string Config::getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Config::getTimeString(std::chrono::system_clock::time_point timePoint) {
    auto timeT = std::chrono::system_clock::to_time_t(timePoint);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Config::getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    if (std::holds_alternative<std::vector<bool>>(value)) return "vector<bool>";
    return "unknown";
}

std::string Config::getValueString(const ConfigValue& value) {
    std::ostringstream oss;
    if (std::holds_alternative<int>(value)) {
        oss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        oss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        oss << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        oss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        oss << "\"" << std::get<std::string>(value) << "\"";
    } else {
        oss << "(complex value)";
    }
    return oss.str();
}

size_t Config::getUniqueKeyCount() const {
    std::set<std::string> uniqueKeys;
    for (const auto& entry : pImpl->entries) {
        uniqueKeys.insert(entry.key);
    }
    return uniqueKeys.size();
}

bool Config::patternMatch(const std::string& key, const std::string& pattern) const {
    // Simple glob pattern matching
    bool inWildcard = false;
    for (size_t i = 0; i < key.length(); ++i) {
        if (pattern[i] == '*') {
            inWildcard = true;
            i++;
            continue;
        } else if (pattern[i] == '?') {
            if (i >= key.length()) return false;
            i++;
            continue;
        } else {
            if (inWildcard) {
                if (i < key.length() && key[i] != pattern[i]) {
                    return false;
                }
            } else if (key[i] != pattern[i]) {
                return false;
            }
        }
    }
    return true;
}

std::string Config::getPresetDirectory() {
    std::string homeDir = std::getenv("HOME");
    if (!homeDir.empty()) {
        return homeDir + "/.nlm/presets";
    }
    return "./presets";
}

std::string Config::getPresetFilePath(const std::string& presetName) {
    std::string presetDir = getPresetDirectory();
    return presetDir + "/" + presetName + ".cfg";
}

bool Config::loadPresetsFromDirectory() {
    std::string presetDir = getPresetDirectory();
    if (!std::filesystem::exists(presetDir)) {
        return false;
    }
    
    std::string indexFile = presetDir + "/presets.index";
    if (!std::filesystem::exists(indexFile)) {
        return false;
    }
    
    std::ifstream indexStream(indexFile);
    if (!indexStream.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(indexStream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string presetName = trim(line.substr(0, pos));
            std::string presetPath = trim(line.substr(pos + 1));
            pImpl->presets[presetName] = presetPath;
        }
    }
    
    pImpl->presetsLoaded = true;
    return true;
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

} // namespace nlm