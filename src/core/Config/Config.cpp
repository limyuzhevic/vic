// Config.cpp - Enhanced implementation with runtime configuration customization

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>
#include <chrono>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ValidationFunction> customValidators;
    bool allowDynamicConfiguration;
    std::chrono::system_clock::time_point lastValidation;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->allowDynamicConfiguration = true;
    pImpl->lastValidation = std::chrono::system_clock::now();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Cannot open configuration file: " + filepath);
    }
    
    try {
        return loadFromStream(file, ConfigSource::File);
    } catch (const ConfigError& e) {
        std::cerr << "Warning: Failed to load config from file " << filepath 
                  << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    throw ConfigValidationError("Invalid command line argument format: " + arg);
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
    } catch (const ConfigError& e) {
        std::cerr << "Warning: Failed to load config from command line arguments: " 
                  << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromJsonString(const std::string& jsonString) {
    try {
        std::istringstream stream(jsonString);
        return loadFromStream(stream, ConfigSource::File);
    } catch (const ConfigError& e) {
        std::cerr << "Warning: Failed to load config from JSON string: " 
                  << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Cannot write configuration file: " + filepath);
    }
    
    try {
        return saveToStream(file);
    } catch (const ConfigError& e) {
        std::cerr << "Warning: Failed to save config to file " << filepath 
                  << ": " << e.what() << std::endl;
        return false;
    }
}

template<typename T>
std::optional<T> Config::getSafe(const std::string& key) const {
    if (!isValidKey(key)) {
        throw ConfigValidationError("Invalid configuration key format: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    // Validate the entry
    if (!it->isValid()) {
        throw ConfigValidationError("Invalid configuration entry for key: " + key);
    }
    
    // Check custom validators
    auto customValidatorIt = pImpl->customValidators.find(key);
    if (customValidatorIt != pImpl->customValidators.end() && !customValidatorIt->second(it->value)) {
        throw ConfigValidationError("Custom validation failed for key: " + key);
    }
    
    try {
        T value = std::get<T>(it->value);
        return value;
    } catch (const std::bad_variant_access&) {
        throw ConfigTypeError(key, typeid(T).name(), it->getValueTypeName());
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = getSafe<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source,
                 const std::string& description,
                 const std::string& validationRegex,
                 const ValidationFunction& validator) {
    if (!isValidKey(key)) {
        throw ConfigValidationError("Invalid configuration key format: " + key);
    }
    
    if (value.index() == static_cast<size_t>(-1)) {
        throw ConfigValidationError("Invalid configuration value for key: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigEntry entry(key, value, source, description);
    entry.validationRegex = validationRegex;
    entry.validator = validator;
    
    if (it != pImpl->entries.end()) {
        entry.timestamp = it->timestamp;
        it->key = entry.key;
        it->value = entry.value;
        it->source = entry.source;
        it->description = entry.description;
        it->validationRegex = entry.validationRegex;
        it->validator = entry.validator;
    } else {
        pImpl->entries.push_back(entry);
    }
    
    // Update last validation timestamp
    pImpl->lastValidation = std::chrono::system_clock::now();
}

void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source, "", "^.{1,100}$"); // Basic validation
}

void Config::setInt(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source, "", "^(-?\d+)$");
}

void Config::setDouble(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source, "", "^(-?\d+(\.\d+)?)$");
}

void Config::setBool(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source, "", "^(true|false)$");
}

bool Config::validateAll() const {
    bool allValid = true;
    for (const auto& entry : pImpl->entries) {
        if (!entry.isValid()) {
            std::cerr << "Invalid configuration entry: " << entry.key << std::endl;
            allValid = false;
        }
    }
    return allValid;
}

bool Config::validateKey(const std::string& key) const {
    return isValidKey(key);
}

bool Config::has(const std::string& key) const {
    if (!isValidKey(key)) {
        return false;
    }
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

std::string Config::getValueType(const std::string& key) const {
    if (!has(key)) {
        throw ConfigKeyError(key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    return it->getValueTypeName();
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

void Config::remove(const std::string& key, ConfigSource source) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key, &source](const ConfigEntry& e) { return e.key == key && e.source == source; }),
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

std::vector<std::string> Config::getKeys(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->lastValidation = std::chrono::system_clock::now();
}

void Config::clear(ConfigSource source) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&source](const ConfigEntry& e) { return e.source == source; }),
        pImpl->entries.end()
    );
    pImpl->lastValidation = std::chrono::system_clock::now();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        oss << entry.getFormattedValue() << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::detailedSummary() const {
    std::ostringstream oss;
    oss << "=== DETAILED CONFIGURATION SUMMARY ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "\nConfiguration breakdown by source:\n";
    
    std::map<ConfigSource, int> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    for (const auto& [source, count] : sourceCounts) {
        oss << "  " << static_cast<int>(source) << ": " << count << " entries\n";
    }
    
    // Show validation status
    oss << "\nValidation Status:\n";
    oss << "  Last validation: " << getFormattedTimestamp("system.last_validation") << "\n";
    oss << "  Allow dynamic configuration: " << (pImpl->allowDynamicConfiguration ? "YES" : "NO") << "\n";
    oss << "  Total custom validators: " << pImpl->customValidators.size() << "\n";
    
    oss << "\nEntry details:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  Key: " << entry.key << "\n";
        oss << "    Type: " << entry.getValueTypeName() << "\n";
        oss << "    Value: " << entry.getFormattedValue() << "\n";
        oss << "    Source: " << static_cast<int>(entry.source) << "\n";
        if (!entry.description.empty()) {
            oss << "    Description: " << entry.description << "\n";
        }
        if (entry.isValid()) {
            oss << "    Status: Valid\n";
        } else {
            oss << "    Status: INVALID\n";
        }
        oss << "    Timestamp: " << getFormattedTimestamp(entry.key) << "\n";
    }
    
    return oss.str();
}

std::chrono::system_clock::time_point Config::getTimestamp(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        throw ConfigKeyError(key);
    }
    return it->timestamp;
}

std::string Config::getFormattedTimestamp(const std::string& key) const {
    auto timestamp = getTimestamp(key);
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&source](const ConfigEntry& e) { return e.source == source; });
}

std::vector<ConfigSource> Config::getSourcesWithEntries() const {
    std::vector<ConfigSource> sources;
    std::set<ConfigSource> sourceSet;
    for (const auto& entry : pImpl->entries) {
        if (sourceSet.find(entry.source) == sourceSet.end()) {
            sourceSet.insert(entry.source);
            sources.push_back(entry.source);
        }
    }
    return sources;
}

// Runtime configuration customization functions
void Config::enableDynamicConfiguration(bool enable) {
    pImpl->allowDynamicConfiguration = enable;
}

bool Config::isDynamicConfigurationEnabled() const {
    return pImpl->allowDynamicConfiguration;
}

void Config::addCustomValidator(const std::string& key, const ValidationFunction& validator) {
    if (!isValidKey(key)) {
        throw ConfigValidationError("Invalid key for custom validator: " + key);
    }
    pImpl->customValidators[key] = validator;
}

void Config::removeCustomValidator(const std::string& key) {
    pImpl->customValidators.erase(key);
}

bool Config::hasCustomValidator(const std::string& key) const {
    return pImpl->customValidators.find(key) != pImpl->customValidators.end();
}

// Configuration presets
void Config::loadPreset(const std::string& presetName) {
    // Load common configuration presets
    if (presetName == "minimal") {
        // Minimal configuration with just essential settings
        setString("brain.neuronType", "Internal");
        setInt("brain.neuronCount", 100);
        setDouble("simulation.timestep", 0.001);
        setBool("plasticity.enabled", false);
    } else if (presetName == "balanced") {
        // Balanced configuration for general use
        setString("brain.neuronType", "Excitatory");
        setInt("brain.neuronCount", 1000);
        setDouble("simulation.timestep", 0.001);
        setBool("plasticity.enabled", true);
        setDouble("plasticity.learningRate", 0.01);
        setBool("memory.workingMemory", true);
        setBool("memory.episodicMemory", true);
    } else if (presetName == "advanced") {
        // Advanced configuration with all features
        setString("brain.neuronType", "Mixed");
        setInt("brain.neuronCount", 5000);
        setDouble("simulation.timestep", 0.0005);
        setBool("plasticity.enabled", true);
        setDouble("plasticity.learningRate", 0.005);
        setBool("memory.workingMemory", true);
        setBool("memory.episodicMemory", true);
        setBool("memory.associativeMemory", true);
        setBool("neuromodulation.dopamine", true);
        setBool("neuromodulation.curiosity", true);
        setBool("development.enabled", true);
    } else {
        throw ConfigValidationError("Unknown preset name: " + presetName);
    }
}

// Configuration comparison
bool Config::compare(const Config& other, double tolerance) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (otherIt == other.pImpl->entries.end()) {
            return false;
        }
        
        // Compare values with tolerance for floating point
        if (entry.getValueTypeName() == "double" && otherIt->getValueTypeName() == "double") {
            double val1, val2;
            std::visit([&](const auto& val) { val1 = val; }, entry.value);
            std::visit([&](const auto& val) { val2 = val; }, otherIt->value);
            if (std::abs(val1 - val2) > tolerance) {
                return false;
            }
        } else if (entry.value != otherIt->value) {
            return false;
        }
    }
    return true;
}

std::string Config::getConfigurationDiff(const Config& other) const {
    std::ostringstream oss;
    oss << "=== CONFIGURATION DIFFERENCES ===\n";
    
    // Find entries that exist in this config but not in other
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (otherIt == other.pImpl->entries.end()) {
            oss << "REMOVED: " << entry.key << " (was: " << entry.getFormattedValue() << ")\n";
        }
    }
    
    // Find entries that exist in other but not in this
    for (const auto& entry : other.pImpl->entries) {
        auto thisIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (thisIt == pImpl->entries.end()) {
            oss << "ADDED: " << entry.key << " (new: " << entry.getFormattedValue() << ")\n";
        }
    }
    
    // Find entries that differ
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (otherIt != other.pImpl->entries.end() && entry.value != otherIt->value) {
            oss << "MODIFIED: " << entry.key << "\n";
            oss << "  Old: " << otherIt->getFormattedValue() << "\n";
            oss << "  New: " << entry.getFormattedValue() << "\n";
        }
    }
    
    if (oss.str().empty()) {
        oss << "No differences found.\n";
    }
    
    return oss.str();
}

// Configuration history and rollback
void Config::saveConfiguration(const std::string& name) {
    // Save current configuration as a snapshot
    std::string timestamp = getFormattedTimestamp("system.current_time");
    std::string filename = "config_history_" + name + "_" + timestamp + ".bin";
    saveToFile(filename);
}

bool Config::loadConfiguration(const std::string& name) {
    // Load configuration from history
    std::string filename = "config_history_" + name + ".bin";
    return loadFromFile(filename);
}

std::vector<std::string> Config::getConfigurationHistory() const {
    std::vector<std::string> history;
    // Implementation would scan for config history files
    return history;
}

// Convenience configuration functions
void Config::configureForTask(const std::string& taskType) {
    if (taskType == "simple") {
        setInt("brain.neuronCount", 100);
        setBool("plasticity.enabled", false);
        setBool("memory.workingMemory", true);
    } else if (taskType == "complex") {
        setInt("brain.neuronCount", 5000);
        setBool("plasticity.enabled", true);
        setBool("memory.workingMemory", true);
        setBool("memory.episodicMemory", true);
        setBool("memory.associativeMemory", true);
        setBool("neuromodulation.dopamine", true);
        setBool("neuromodulation.curiosity", true);
    } else if (taskType == "developmental") {
        setBool("development.enabled", true);
        setInt("brain.neuronCount", 1000);
        setBool("plasticity.enabled", true);
        setBool("plasticity.structural", true);
    }
}

// Expert configuration
void Config::setExpertParameters(const std::string& expertLevel) {
    if (expertLevel == "beginner") {
        setInt("brain.neuronCount", 500);
        setDouble("simulation.timestep", 0.001);
        setBool("plasticity.enabled", false);
        setBool("memory.workingMemory", true);
    } else if (expertLevel == "intermediate") {
        setInt("brain.neuronCount", 1000);
        setDouble("simulation.timestep", 0.0005);
        setBool("plasticity.enabled", true);
        setBool("memory.workingMemory", true);
        setBool("memory.episodicMemory", true);
    } else if (expertLevel == "advanced") {
        setInt("brain.neuronCount", 2000);
        setDouble("simulation.timestep", 0.0001);
        setBool("plasticity.enabled", true);
        setBool("plasticity.structural", true);
        setBool("memory.workingMemory", true);
        setBool("memory.episodicMemory", true);
        setBool("memory.associativeMemory", true);
        setBool("neuromodulation.dopamine", true);
        setBool("neuromodulation.curiosity", true);
        setBool("development.enabled", true);
    }
}

// Configuration validation presets
bool Config::validateString(const ConfigValue& value) {
    return std::holds_alternative<std::string>(value);
}

bool Config::validateInt(const ConfigValue& value) {
    return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
}

bool Config::validateIntRange(int min, int max) {
    return [min, max](const ConfigValue& value) -> bool {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) >= min && std::get<int>(value) <= max;
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) >= min && std::get<int64_t>(value) <= max;
        }
        return false;
    };
}

bool Config::validateDouble(const ConfigValue& value) {
    return std::holds_alternative<double>(value);
}

bool Config::validateDoubleRange(double min, double max) {
    return [min, max](const ConfigValue& value) -> bool {
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) >= min && std::get<double>(value) <= max;
        }
        return false;
    };
}

bool Config::validateBool(const ConfigValue& value) {
    return std::holds_alternative<bool>(value);
}

bool Config::validatePositive(const ConfigValue& value) {
    return std::visit([](const auto& val) -> bool {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || std::is_same_v<T, double>) {
            return val > 0;
        }
        return false;
    }, value);
}

// Internal helper functions
bool Config::isValidKey(const std::string& key) {
    return !key.empty() && std::all_of(key.begin(), key.end(), [](char c) {
        return std::isalnum(c) || c == '_' || c == '-';
    });
}

bool Config::isValidDescription(const std::string& description) {
    return description.length() <= 500; // Reasonable limit
}

bool Config::loadFromStream(std::istream& stream, ConfigSource source) {
    std::string line;
    while (std::getline(stream, line)) {
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
            
            set(key, value, source);
        }
    }
    return true;
}

bool Config::saveToStream(std::ostream& stream) const {
    for (const auto& entry : pImpl->entries) {
        stream << "# " << entry.description << "\n";
        stream << entry.key << " = " << configValueToJsonString(entry.value) << "\n\n";
    }
    return true;
}

std::string Config::configValueToJsonString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](const auto& val) {
        oss << val;
    }, value);
    
    // Escape string values for JSON
    if (std::holds_alternative<std::string>(value)) {
        oss << "\"";
    }
    
    return oss.str();
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

// Explicit template instantiations
// Config::getSafe templates are declared in header, so we need to provide definitions
template std::optional<int> Config::getSafe<int>(const std::string&) const;
template std::optional<int64_t> Config::getSafe<int64_t>(const std::string&) const;
template std::optional<double> Config::getSafe<double>(const std::string&) const;
template std::optional<bool> Config::getSafe<bool>(const std::string&) const;
template std::optional<std::string> Config::getSafe<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::getSafe<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::getSafe<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::getSafe<std::vector<std::string>>(const std::string&) const;

// Config::getOr templates are declared in header, so we need to provide definitions
template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm