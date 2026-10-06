#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <limits>
#include <random>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::unordered_map<std::string, ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSource> keySources;
};

Config::Config() : pImpl(std::make_unique<Impl>()), currentSchema_() {
    // Initialize with some common configuration entries
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaults() {
    // Common simulation configuration
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", 1000, ConfigSource::Default);
    set("region_count", 1, ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Plasticity configuration
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_tau", 20.0f, ConfigSource::Default);
    set("hebbian_learning_rate", 0.01f, ConfigSource::Default);
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Memory configuration
    set("replay_interval", 100, ConfigSource::Default);
    set("consolidation_interval", 1000, ConfigSource::Default);
    
    // Neuromodulation configuration
    set("dopamine_peak", 1.0f, ConfigSource::Default);
    set("curiosity_threshold", 0.5f, ConfigSource::Default);
    set("novelty_decay", 0.95f, ConfigSource::Default);
    
    // Development configuration
    set("developmental_stage", static_cast<int>(DevelopmentalStage::Adult), ConfigSource::Default);
    
    // Logging configuration
    set("log_level", static_cast<int>(LogLevel::Info), ConfigSource::Default);
    set("enable_colors", true, ConfigSource::Default);
}

bool Config::loadFromFile(const std::string& filepath, bool validateSchema) {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return parseJSONFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return parseYAMLFile(filepath);
    } else {
        // Try JSON first, then YAML, then fallback to simple format
        if (parseJSONFile(filepath)) return true;
        if (parseYAMLFile(filepath)) return true;
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromYAML(const std::string& filepath, bool validateSchema) {
    // For now, implement basic YAML support using simple parsing
    // In a real implementation, you would use a YAML library like yaml-cpp
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromArgs(int argc, char** argv, const std::vector<std::string>& allowedKeys) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                if (allowedKeys.empty() || std::find(allowedKeys.begin(), allowedKeys.end(), key) != allowedKeys.end()) {
                    setFromString(key, value, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (allowedKeys.empty() || std::find(allowedKeys.begin(), allowedKeys.end(), key) != allowedKeys.end()) {
                setFromString(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    return success;
}

bool Config::loadFromEnvironment(const std::unordered_map<std::string, std::string>& envVars) {
    bool success = true;
    
    for (const auto& pair : envVars) {
        std::string key = normalizeKey(pair.first);
        std::string value = pair.second;
        
        if (validateKey(key)) {
            setFromString(key, value, ConfigSource::Environment);
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (prettyPrint) {
        // Write as JSON
        file << "{" << std::endl;
        bool first = true;
        for (const auto& pair : pImpl->entries) {
            if (!first) file << "," << std::endl;
            first = false;
            
            file << "  \"" << pair.first << "\": " << formatConfigValue(pair.second.value);
        }
        file << std::endl << "}" << std::endl;
    } else {
        // Write as simple key=value format
        for (const auto& pair : pImpl->entries) {
            file << pair.first << " = " << formatConfigValue(pair.second.value) << std::endl;
        }
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    
    auto it = pImpl->entries.find(normalizedKey);
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->second.value);
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
    std::string normalizedKey = normalizeKey(key);
    
    // Validate key
    if (!validateKey(normalizedKey)) {
        return; // Silently ignore invalid keys
    }
    
    // Create entry
    ConfigEntry entry;
    entry.key = normalizedKey;
    entry.value = value;
    entry.source = source;
    
    // Try to set validator based on key
    if (normalizedKey == "log_level") {
        entry.validator = [](const ConfigValue& v) -> bool {
            try {
                int level = std::get<int>(v);
                return level >= 0 && level <= 4;
            } catch (...) {
                return false;
            }
        };
    } else if (normalizedKey == "simulation_timestep") {
        entry.validator = [](const ConfigValue& v) -> bool {
            try {
                double dt = std::get<double>(v);
                return dt > 0.0 && dt < 1.0;
            } catch (...) {
                return false;
            }
        };
    }
    
    // Store the entry
    pImpl->entries[normalizedKey] = entry;
    pImpl->keySources[normalizedKey] = source;
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

bool Config::getBool(const std::string& key, bool defaultValue) const {
    return getOr<bool>(key, defaultValue);
}

int Config::getInt(const std::string& key, int defaultValue) const {
    return getOr<int>(key, defaultValue);
}

double Config::getDouble(const std::string& key, double defaultValue) const {
    return getOr<double>(key, defaultValue);
}

std::string Config::getString(const std::string& key, const std::string& defaultValue) const {
    return getOr<std::string>(key, defaultValue);
}

std::vector<int> Config::getIntVector(const std::string& key) const {
    auto val = get<std::vector<int>>(key);
    return val.has_value() ? val.value() : std::vector<int>{};
}

std::vector<double> Config::getDoubleVector(const std::string& key) const {
    auto val = get<std::vector<double>>(key);
    return val.has_value() ? val.value() : std::vector<double>{};
}

std::vector<std::string> Config::getStringVector(const std::string& key) const {
    auto val = get<std::vector<std::string>>(key);
    return val.has_value() ? val.value() : std::vector<std::string>{};
}

bool Config::has(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    return pImpl->entries.find(normalizedKey) != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    std::string normalizedKey = normalizeKey(key);
    pImpl->entries.erase(normalizedKey);
    pImpl->keySources.erase(normalizedKey);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& pair : pImpl->entries) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& pair : pImpl->entries) {
        if (pair.second.category == category) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->keySources.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):" << std::endl;
    
    for (const auto& pair : pImpl->entries) {
        oss << "  " << pair.first << " = [" << formatConfigValue(pair.second.value) << "] "
            << "(source: " << static_cast<int>(pair.second.source) << ")";
        if (!pair.second.description.empty()) {
            oss << " - " << pair.second.description;
        }
        if (!pair.second.category.empty()) {
            oss << " [" << pair.second.category << "]";
        }
        oss << std::endl;
    }
    
    return oss.str();
}

std::unordered_map<std::string, ConfigEntry> Config::getBySource(ConfigSource source) const {
    std::unordered_map<std::string, ConfigEntry> result;
    
    for (const auto& pair : pImpl->entries) {
        if (pair.second.source == source) {
            result[pair.first] = pair.second;
        }
    }
    
    return result;
}

std::vector<std::string> Config::validateAgainstSchema() const {
    std::vector<std::string> errors;
    
    if (!currentSchema_.validate()) {
        errors.push_back("Schema validation failed");
    }
    
    for (const auto& pair : currentSchema_.entries) {
        const auto& entry = pair.second;
        
        if (entry.required && !has(entry.key)) {
            errors.push_back("Required configuration key missing: " + entry.key);
        }
        
        if (has(entry.key)) {
            ConfigValue value;
            auto it = pImpl->entries.find(entry.key);
            if (it != pImpl->entries.end()) {
                value = it->second.value;
            }
            
            if (!entry.validate() || !Validation::isValidConfigurationValue(entry.key, value)) {
                errors.push_back("Invalid configuration value for key: " + entry.key);
            }
        }
    }
    
    return errors;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& pair : other.pImpl->entries) {
        std::string key = pair.first;
        ConfigSource otherSource = other.pImpl->keySources.at(key);
        
        // Use higher priority source if exists
        if (pImpl->keySources.find(key) != pImpl->keySources.end()) {
            if (getSourcePriority(source) > getSourcePriority(pImpl->keySources[key])) {
                pImpl->entries[key] = pair.second;
                pImpl->keySources[key] = source;
            }
        } else {
            pImpl->entries[key] = pair.second;
            pImpl->keySources[key] = source;
        }
    }
}

void Config::resetToDefaults() {
    clear();
    initializeDefaults();
}

ConfigSource Config::getSource(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    
    auto it = pImpl->keySources.find(normalizedKey);
    return it != pImpl->keySources.end() ? it->second : ConfigSource::Default;
}

bool Config::isOverridden(const std::string& key) const {
    ConfigSource source = getSource(key);
    return source == ConfigSource::CommandLine || source == ConfigSource::Environment || 
           source == ConfigSource::Runtime || source == ConfigSource::Override;
}

int Config::getSourcePriority(ConfigSource source) {
    switch (source) {
        case ConfigSource::Override: return 5;
        case ConfigSource::CommandLine: return 4;
        case ConfigSource::Environment: return 3;
        case ConfigSource::Runtime: return 2;
        case ConfigSource::File: return 1;
        case ConfigSource::Default: return 0;
        default: return 0;
    }
}

bool Config::parseJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        Json::CharReaderBuilder readerBuilder;
        Json::Value root;
        std::string errors;
        
        if (!Json::parseFromStream(readerBuilder, file, &root, &errors)) {
            return false;
        }
        
        // Parse JSON values
        for (const auto& member : root.getMemberNames()) {
            setFromJSONValue(member, root[member]);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::parseYAMLFile(const std::string& filepath) {
    // For now, use simple parsing - would use yaml-cpp in production
    return loadFromSimpleFormat(filepath);
}

std::string Config::normalizeKey(const std::string& key) const {
    std::string normalized = key;
    // Convert to lowercase
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    
    // Replace underscores and hyphens with single underscore
    normalized.erase(std::remove(normalized.begin(), normalized.end(), '-'), normalized.end());
    
    return normalized;
}

bool Config::validateKey(const std::string& key) const {
    // Basic key validation
    if (key.empty()) return false;
    if (key.size() > 100) return false; // Reasonable key length
    
    // Check for valid characters
    for (char c : key) {
        if (!(std::isalnum(c) || c == '_' || c == '-')) {
            return false;
        }
    }
    
    return true;
}

ConfigValue Config::jsonToConfigValue(const Json::Value& jsonValue) const {
    if (jsonValue.isInt()) {
        return jsonValue.asInt();
    } else if (jsonValue.isUInt()) {
        return jsonValue.asUInt();
    } else if (jsonValue.isInt64()) {
        return jsonValue.asInt64();
    } else if (jsonValue.isDouble()) {
        return jsonValue.asDouble();
    } else if (jsonValue.isBool()) {
        return jsonValue.asBool();
    } else if (jsonValue.isString()) {
        return jsonValue.asString().asStdString();
    } else if (jsonValue.isArray()) {
        std::vector<int> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        std::vector<bool> boolArray;
        
        for (const auto& element : jsonValue) {
            if (element.isInt()) {
                intArray.push_back(element.asInt());
            } else if (element.isDouble()) {
                doubleArray.push_back(element.asDouble());
            } else if (element.isString()) {
                stringArray.push_back(element.asString().asStdString());
            } else if (element.isBool()) {
                boolArray.push_back(element.asBool());
            }
        }
        
        // Prefer int array if all elements are integers
        if (!intArray.empty() && doubleArray.empty() && stringArray.empty() && boolArray.empty()) {
            return intArray;
        } else if (!doubleArray.empty() && stringArray.empty() && boolArray.empty()) {
            return doubleArray;
        } else if (!stringArray.empty() && boolArray.empty()) {
            return stringArray;
        } else if (!boolArray.empty()) {
            return boolArray;
        }
    }
    
    return std::string(); // Return empty string for unknown types
}

Json::Value Config::configValueToJSON(const ConfigValue& value) const {
    Json::Value jsonValue;
    
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = static_cast<Json::Int64>(val);
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (int item : val) {
                jsonValue.append(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (double item : val) {
                jsonValue.append(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const std::string& item : val) {
                jsonValue.append(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            for (bool item : val) {
                jsonValue.append(item);
            }
        }
    }, value);
    
    return jsonValue;
}

std::string Config::configValueToString(const ConfigValue& value) const {
    return formatConfigValue(value);
}

bool Config::stringMatchesPattern(const std::string& value, const std::string& pattern) {
    if (pattern.empty()) return true;
    
    try {
        std::regex regex(pattern);
        return std::regex_match(value, regex);
    } catch (const std::regex_error&) {
        return false;
    }
}

std::string Config::formatConfigValue(const ConfigValue& value) const {
    std::ostringstream oss;
    
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << val;
        } else if constexpr (std::is_same_v<T, double>) {
            // Format double to avoid scientific notation
            oss.precision(6);
            oss << std::fixed << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                           std::is_same_v<T, std::vector<double>> ||
                           std::is_same_v<T, std::vector<std::string>> ||
                           std::is_same_v<T, std::vector<bool>>) {
            oss << "[";
            bool first = true;
            std::visit([&](const auto& vec) {
                for (const auto& item : vec) {
                    if (!first) oss << ", ";
                    first = false;
                    
                    if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<int64_t>>) {
                        oss << item;
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        oss.precision(3);
                        oss << std::fixed << item;
                    } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                        oss << (item ? "true" : "false");
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        oss << "\"" << item << "\"";
                    }
                }
            }, val);
            oss << "]";
        }
    }, value);
    
    return oss.str();
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
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
            
            setFromString(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

void Config::setFromString(const std::string& key, const std::string& value, ConfigSource source) {
    std::string normalizedKey = normalizeKey(key);
    
    // Try to infer type from string
    ConfigValue configValue;
    
    if (value == "true" || value == "false") {
        configValue = (value == "true");
    } else {
        // Try to parse as number
        bool isNumber = true;
        double numValue;
        size_t pos;
        
        try {
            numValue = std::stod(value, &pos);
            if (pos == value.size() && !std::isnan(numValue)) {
                configValue = numValue;
            } else {
                configValue = value;
            }
        } catch (...) {
            configValue = value;
        }
    }
    
    set(normalizedKey, configValue, source);
}

void Config::setFromJSONValue(const std::string& key, const Json::Value& jsonValue) {
    ConfigValue configValue = jsonToConfigValue(jsonValue);
    set(key, configValue, ConfigSource::File);
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// ==================== VALIDATION IMPLEMENTATIONS ====================

bool Validation::isPositive(const ConfigValue& value) {
    try {
        if constexpr (std::is_same_v<int>) {
            int val = std::get<int>(value);
            return val > 0;
        } else if constexpr (std::is_same_v<double>) {
            double val = std::get<double>(value);
            return val > 0.0;
        }
    } catch (...) {
        return false;
    }
    return false;
}

bool Validation::isNonNegative(const ConfigValue& value) {
    try {
        if constexpr (std::is_same_v<int>) {
            int val = std::get<int>(value);
            return val >= 0;
        } else if constexpr (std::is_same_v<double>) {
            double val = std::get<double>(value);
            return val >= 0.0;
        }
    } catch (...) {
        return false;
    }
    return false;
}

bool Validation::isInRange(const ConfigValue& value, double min, double max) {
    try {
        if constexpr (std::is_same_v<int>) {
            int val = std::get<int>(value);
            return static_cast<double>(val) >= min && static_cast<double>(val) <= max;
        } else if constexpr (std::is_same_v<double>) {
            double val = std::get<double>(value);
            return val >= min && val <= max;
        }
    } catch (...) {
        return false;
    }
    return false;
}

bool Validation::matchesPattern(const ConfigValue& value, const std::string& pattern) {
    try {
        std::string val = std::get<std::string>(value);
        return Config::stringMatchesPattern(val, pattern);
    } catch (...) {
        return false;
    }
}

bool Validation::isStringLength(const ConfigValue& value, size_t minLen, size_t maxLen) {
    try {
        std::string val = std::get<std::string>(value);
        return val.length() >= minLen && val.length() <= maxLen;
    } catch (...) {
        return false;
    }
}

bool Validation::isValidNeuronType(const ConfigValue& value) {
    try {
        int val = std::get<int>(value);
        return val >= 0 && val <= 9; // NeuronType enum range
    } catch (...) {
        return false;
    }
}

bool Validation::isValidSynapseType(const ConfigValue& value) {
    try {
        int val = std::get<int>(value);
        return val >= 0 && val <= 7; // SynapseType enum range
    } catch (...) {
        return false;
    }
}

bool Validation::isValidPlasticityRule(const ConfigValue& value) {
    try {
        int val = std::get<int>(value);
        return val >= 0 && val <= 6; // PlasticityRule enum range
    } catch (...) {
        return false;
    }
}

bool Validation::isValidDevelopmentalStage(const ConfigValue& value) {
    try {
        int val = std::get<int>(value);
        return val >= 0 && val <= 5; // DevelopmentalStage enum range
    } catch (...) {
        return false;
    }
}

bool Validation::isValidFiringState(const ConfigValue& value) {
    try {
        int val = std::get<int>(value);
        return val >= 0 && val <= 7; // FiringState enum range
    } catch (...) {
        return false;
    }
}

bool Validation::isValidLogLevel(const ConfigValue& value) {
    try {
        int val = std::get<int>(value);
        return val >= 0 && val <= 4; // LogLevel enum range
    } catch (...) {
        return false;
    }
}

bool Validation::isValidConfigurationValue(const std::string& key, const ConfigValue& value) {
    // Basic validation based on key name
    if (key == "log_level") {
        return isValidLogLevel(value);
    } else if (key == "simulation_timestep") {
        return isPositive(value) && isInRange(value, 0.0, 1.0);
    } else if (key == "neuron_count") {
        return isNonNegative(value) && isInRange(value, 1, 10000);
    } else if (key == "region_count") {
        return isNonNegative(value) && isInRange(value, 1, 100);
    } else if (key == "connection_probability") {
        return isInRange(value, 0.0, 1.0);
    }
    
    return true; // Default to passing validation for unknown keys
}

// ==================== CONVENIENCE FUNCTION IMPLEMENTATIONS ====================

ConfigEntry createConfigEntry(const std::string& key, const ConfigValue& value,
                             ConfigSource source, const std::string& description,
                             const std::string& category,
                             std::function<bool(const ConfigValue&)> validator) {
    ConfigEntry entry(key, value, source, description, category);
    entry.validator = validator;
    return entry;
}

bool parseCommandLineArgs(int argc, char** argv, const ConfigurationSchema& schema,
                         Config& config, bool validateUnknownKeys) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Check if key exists in schema
                if (!validateUnknownKeys || schema.getEntry(key) != nullptr) {
                    config.setFromString(key, value, ConfigSource::CommandLine);
                }
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (!validateUnknownKeys || schema.getEntry(key) != nullptr) {
                config.setFromString(key, value, ConfigSource::CommandLine);
            }
        }
    }
    
    return true;
}

bool loadEnvironmentVariables(Config& config, const std::vector<std::string>& prefixList) {
    for (const std::string& prefix : prefixList) {
        // In a real implementation, this would iterate through environment variables
        // with the given prefix
        // For now, return false
    }
    return true;
}

} // namespace nlm

// Include the original Config implementation for backward compatibility
#include "core/Config/Config.cpp"