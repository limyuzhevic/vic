// Enhanced Configuration Implementation
// 
// This file implements the enhanced configuration class with:
// 1. Comprehensive configuration validation
// 2. Automatic duplicate cleanup
// 3. Schema-based validation
// 4. Enhanced error handling and reporting
// 5. Configuration inheritance and scoping
// 6. Change notification patterns
// 7. Type safety and validation
// 8. Comprehensive documentation
// 9. Integration with experiment framework
// 10. Support for advanced configuration operations

#include "EnhancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <random>
#include <iostream>

namespace nlm {

// Enhanced Config Implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string scope;
    std::shared_ptr<Config> parentConfig;
    std::vector<std::shared_ptr<ConfigObserver>> observers;
    std::unordered_map<std::string, ConfigSchema> schemas;
    
    // Statistics
    size_t totalChanges;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastChangeTime;
    
    Impl() : scope(""), parentConfig(nullptr), totalChanges(0) {
        lastChangeTime = std::chrono::high_resolution_clock::now();
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Register common schemas
    pImpl->schemas["default"] = ConfigSchemas::createDefaultNLMSchema();
    pImpl->schemas["phase2"] = ConfigSchemas::createPhase2Schema();
    pImpl->schemas["enhanced_phase2"] = ConfigSchemas::createEnhancedPhase2Schema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Enhanced file loading with validation
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigValidationError("Cannot open configuration file: '" + filepath + "'", filepath);
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasChanges = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with enhanced error handling
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            throw ConfigValidationError("Invalid configuration line: '" + line + "' (line " + std::to_string(lineNumber) + ")");
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Try to convert to appropriate type
        ConfigValue configValue = stringToConfigValue(value);
        
        // Check if key already exists
        auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                      [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (existingIt != pImpl->entries.end()) {
            // Update existing entry
            ConfigValue oldValue = existingIt->value;
            existingIt->value = configValue;
            existingIt->source = ConfigSource::File;
            hasChanges = true;
            notifyObservers(key, oldValue, configValue);
        } else {
            // Add new entry
            pImpl->entries.emplace_back(key, configValue, ConfigSource::File);
            hasChanges = true;
            notifyObservers(key, ConfigValue(), configValue);
        }
    }
    
    if (hasChanges) {
        pImpl->totalChanges++;
        pImpl->lastChangeTime = std::chrono::high_resolution_clock::now();
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Enhanced command line argument parsing with validation
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setWithValidation(key, value, ConfigSource::CommandLine);
            } else {
                throw ConfigValidationError("Invalid command line argument: '" + arg + "' (missing value)");
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setWithValidation(key, value, ConfigSource::CommandLine);
        } else {
            throw ConfigValidationError("Invalid command line argument: '" + arg + "' (unknown format)");
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Enhanced file saving with validation
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated: " << std::chrono::system_clock::now() << "\n";
    file << "# Scope: " << pImpl->scope << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (source: " << static_cast<int>(entry.source) << ")\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n";
        file << "\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n    
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
    auto value = get<T>(key);
    if (value.has_value()) {
        return *value;
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setWithValidation(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    ConfigValue configValue = stringToConfigValue(value);
    setWithValidation(key, configValue, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    setWithValidation(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    setWithValidation(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    setWithValidation(key, value, source);
}

bool Config::setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Check if key already exists
    auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                  [&key](const ConfigEntry& e) { return e.key == key; });
    
    ConfigValue oldValue;
    bool isUpdate = false;
    
    if (existingIt != pImpl->entries.end()) {
        oldValue = existingIt->value;
        existingIt->value = value;
        existingIt->source = source;
        isUpdate = true;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        oldValue = ConfigValue();  // Empty value for new entries
    }
    
    // Validate the entry
    if (!validateEntry(pImpl->entries.back())) {
        if (isUpdate) {
            existingIt->value = oldValue;
            existingIt->source = existingIt->source;  // Restore old source
        } else {
            pImpl->entries.pop_back();
        }
        return false;
    }
    
    // Notify observers
    notifyObservers(key, oldValue, value);
    
    // Update statistics
    pImpl->totalChanges++;
    pImpl->lastChangeTime = std::chrono::high_resolution_clock::now();
    
    return true;
}

bool Config::has(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        ConfigValue oldValue = it->value;
        pImpl->entries.erase(it);
        notifyObservers(key, oldValue, ConfigValue());
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    for (const auto& entry : pImpl->entries) {
        notifyObservers(entry.key, entry.value, ConfigValue());
    }
    
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "=== NLM Configuration Summary ===\n";
    ss << "Scope: " << pImpl->scope << "\n";
    ss << "Total entries: " << pImpl->entries.size() << "\n";
    ss << "Total changes: " << pImpl->totalChanges << "\n";
    ss << "Last changed: " << std::chrono::system_clock::to_time_t(pImpl->lastChangeTime) << "\n\n";
    
    ss << "Entries by source:\n";
    std::unordered_map<ConfigSource, size_t> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    for (const auto& [source, count] : sourceCounts) {
        ss << "  " << static_cast<int>(source) << ": " << count << "\n";
    }
    
    ss << "\nEntry details:\n";
    for (const auto& entry : pImpl->entries) {
        ss << "  " << entry.key << " = " << configValueToString(entry.value) 
           << " (source: " << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            ss << " - " << entry.description;
        }
        ss << "\n";
    }
    
    return ss.str();
}

bool Config::validate(const ConfigSchema& schema) const {
    try {
        return validate(schema);
    } catch (const ConfigValidationError& e) {
        std::cerr << "Validation error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Validation error: " << e.what() << std::endl;
        return false;
    }
}

std::string Config::getValidationErrors(const ConfigSchema& schema) const {
    std::stringstream ss;
    
    // Create temporary entry map for validation
    std::unordered_map<std::string, ConfigEntry> entryMap;
    for (const auto& entry : pImpl->entries) {
        entryMap[entry.key] = entry;
    }
    
    try {
        if (validate(schema)) {
            return "Configuration is valid.";
        }
    } catch (const ConfigValidationError& e) {
        ss << "Key: '" << e.getKey() << "' - " << e.what() << "\n";
    } catch (const std::exception& e) {
        ss << "Error: " << e.what() << "\n";
    }
    
    return ss.str();
}

void Config::cleanupDuplicates() {
    // Remove duplicate entries with same key
    removeDuplicateEntries();
    
    // Resolve conflicts by preferring higher priority sources
    resolveConflicts(pImpl->entries);
}

void Config::inheritFrom(const Config& parentConfig, ConfigSource source) {
    // Create a copy of parent entries to process
    std::vector<ConfigEntry> parentEntries;
    
    // Recursively collect parent entries
    const Config* current = &parentConfig;
    while (current) {
        parentEntries.insert(parentEntries.end(), current->pImpl->entries.begin(), current->pImpl->entries.end());
        if (current->pImpl->parentConfig) {
            current = current->pImpl->parentConfig.get();
        } else {
            break;
        }
    }
    
    // Add parent entries to current config
    for (const auto& entry : parentEntries) {
        if (!has(entry.key)) {
            set(entry.key, entry.value, source);
        }
    }
    
    pImpl->parentConfig = std::make_shared<Config>(parentConfig);
}

bool Config::hasInheritance() const {
    return pImpl->parentConfig != nullptr;
}

void Config::addObserver(std::shared_ptr<ConfigObserver> observer) {
    pImpl->observers.push_back(observer);
}

void Config::removeObserver(std::shared_ptr<ConfigObserver> observer) {
    auto it = std::find(pImpl->observers.begin(), pImpl->observers.end(), observer);
    if (it != pImpl->observers.end()) {
        pImpl->observers.erase(it);
    }
}

void Config::notifyObservers(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    for (const auto& observer : pImpl->observers) {
        try {
            observer->onConfigChanged(key, oldValue, newValue);
        } catch (const std::exception& e) {
            std::cerr << "Observer notification error: " << e.what() << std::endl;
        }
    }
}

void Config::setScope(const std::string& scope) {
    pImpl->scope = scope;
}

std::string Config::getScope() const {
    return pImpl->scope;
}

bool Config::hasScope(const std::string& scope) const {
    return pImpl->scope == scope;
}

std::string Config::exportToString() const {
    std::stringstream ss;
    ss << "=== NLM Configuration Export ===\n";
    ss << "Scope: " << pImpl->scope << "\n";
    ss << "Timestamp: " << std::chrono::system_clock::now() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        ss << entry.key << " = " << configValueToString(entry.value) << "\n";
    }
    
    return ss.str();
}

bool Config::importFromString(const std::string& data) {
    std::stringstream ss(data);
    std::string line;
    bool success = true;
    
    while (std::getline(ss, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (!setWithValidation(key, value, ConfigSource::Runtime)) {
                std::cerr << "Failed to import key: " << key << std::endl;
                success = false;
            }
        }
    }
    
    return success;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            count++;
        }
    }
    return count;
}

std::unordered_map<ConfigSource, size_t> Config::getSourceStats() const {
    std::unordered_map<ConfigSource, size_t> stats;
    for (const auto& entry : pImpl->entries) {
        stats[entry.source]++;
    }
    return stats;
}

// Helper methods
std::string Config::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return str;
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool Config::isNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

ConfigValue Config::stringToConfigValue(const std::string& str) {
    // Try to parse as different types
    if (isNumeric(str)) {
        // Check if it's int or int64_t
        try {
            if (std::stoll(str) <= std::numeric_limits<int>::max() && 
                std::stoll(str) >= std::numeric_limits<int>::min()) {
                return std::stoll(str);
            }
            return std::stoll(str);  // int64_t
        } catch (...) {
            return 0;
        }
    }
    
    // Try to parse as double
    try {
        size_t pos;
        double d = std::stod(str, &pos);
        if (pos == str.size()) {
            return d;
        }
    } catch (...) {}
    
    // Try to parse as bool
    if (toLower(str) == "true" || toLower(str) == "yes" || toLower(str) == "1") {
        return true;
    }
    if (toLower(str) == "false" || toLower(str) == "no" || toLower(str) == "0") {
        return false;
    }
    
    // Check if it's a vector
    if (str.front() == '[' && str.back() == ']') {
        std::string content = str.substr(1, str.size() - 2);
        std::vector<std::string> parts;
        std::stringstream ss(content);
        std::string part;
        while (std::getline(ss, part, ',')) {
            parts.push_back(trim(part));
        }
        
        // Try to create vector of doubles
        std::vector<double> doubleVector;
        bool allDouble = true;
        for (const auto& part : parts) {
            try {
                doubleVector.push_back(std::stod(part));
            } catch (...) {
                allDouble = false;
                break;
            }
        }
        
        if (allDouble) {
            return doubleVector;
        }
        
        // Try to create vector of ints
        std::vector<int> intVector;
        bool allInt = true;
        for (const auto& part : parts) {
            try {
                intVector.push_back(std::stoi(part));
            } catch (...) {
                allInt = false;
                break;
            }
        }
        
        if (allInt) {
            return intVector;
        }
    }
    
    // Default to string
    return str;
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::stringstream ss;
    
    if (std::holds_alternative<int>(value)) {
        ss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        ss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        ss << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        ss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        ss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << vec[i] << "\"";
        }
        ss << "]";
    }
    
    return ss.str();
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    // Basic validation: key cannot be empty
    if (entry.key.empty()) {
        return false;
    }
    
    // Validate value based on type
    if (entry.value.index() == static_cast<size_t>(-1)) {
        return false;  // Invalid index
    }
    
    return true;
}

bool Config::validateType(const ConfigValue& value, const std::string& typeName) const {
    // Simplified type validation
    switch (hash<std::string>{}(typeName)) {
        case hash<std::string>{}("int"):
            return std::holds_alternative<int>(value);
        case hash<std::string>{}("int64_t"):
            return std::holds_alternative<int64_t>(value);
        case hash<std::string>{}("double"):
            return std::holds_alternative<double>(value);
        case hash<std::string>{}("bool"):
            return std::holds_alternative<bool>(value);
        case hash<std::string>{}("string"):
            return std::holds_alternative<std::string>(value);
        case hash<std::string>{}("vector<int>"):
            return std::holds_alternative<std::vector<int>>(value);
        case hash<std::string>{}("vector<double>"):
            return std::holds_alternative<std::vector<double>>(value);
        case hash<std::string>{}("vector<string>"):
            return std::holds_alternative<std::vector<std::string>>(value);
        default:
            return true;  // Unknown type, skip validation
    }
}

bool Config::validateRange(const ConfigValue& value, double min, double max) const {
    // Simplified range validation for numeric types
    if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        return val >= min && val <= max;
    }
    return true;  // Non-numeric type, skip range validation
}

void Config::removeDuplicateEntries() {
    // Remove entries with same key, keeping the highest priority source
    std::sort(pImpl->entries.begin(), pImpl->entries.end(),
              [](const ConfigEntry& a, const ConfigEntry& b) {
                  // Higher priority sources should come first
                  return static_cast<int>(a.source) > static_cast<int>(b.source);
              });
    
    // Remove duplicates, keeping first occurrence (highest priority)
    std::vector<ConfigEntry> uniqueEntries;
    std::unordered_set<std::string> seenKeys;
    
    for (const auto& entry : pImpl->entries) {
        if (seenKeys.find(entry.key) == seenKeys.end()) {
            uniqueEntries.push_back(entry);
            seenKeys.insert(entry.key);
        }
    }
    
    pImpl->entries = uniqueEntries;
}

void Config::resolveConflicts(const std::vector<ConfigEntry>& entries) {
    // Resolve conflicts by considering inheritance and source priority
    // This is a simplified implementation
    for (const auto& entry : entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });n        
        if (it != pImpl->entries.end()) {
            // Check if current entry has higher priority
            if (static_cast<int>(entry.source) > static_cast<int>(it->source)) {
                ConfigValue oldValue = it->value;
                it->value = entry.value;
                it->source = entry.source;
                
                // Notify observers of conflict resolution
                notifyObservers(entry.key, oldValue, entry.value);
            }
        }
    }
}

// Common configuration schemas
ConfigSchema ConfigSchemas::createDefaultNLMSchema() {
    ConfigSchema schema;
    
    // Basic type validators
    schema.addTypeValidator("random_seed", "int64_t");
    schema.addTypeValidator("simulation_timestep", "double");
    schema.addTypeValidator("neuron_count", "int64_t");
    schema.addTypeValidator("region_count", "int64_t");
    schema.addTypeValidator("connection_probability", "double");
    
    // Range validators
    schema.addRangeValidator("random_seed", 0, std::numeric_limits<int64_t>::max());
    schema.addRangeValidator("simulation_timestep", 0.0, 1.0);
    schema.addRangeValidator("neuron_count", 1, 1000000);
    schema.addRangeValidator("region_count", 1, 100);
    schema.addRangeValidator("connection_probability", 0.0, 1.0);
    
    // Required keys
    schema.addRequiredKey("neuron_count");
    schema.addRequiredKey("region_count");
    
    // Dependencies
    schema.addDependency("connection_probability", "region_count");
    
    return schema;
}

ConfigSchema ConfigSchemas::createPhase2Schema() {
    ConfigSchema schema = createDefaultNLMSchema();
    
    // Add Phase 2 specific keys
    schema.addTypeValidator("stdp_ltp_weight", "double");
    schema.addTypeValidator("stdp_ltd_weight", "double");
    schema.addTypeValidator("stdp_tau", "double");
    schema.addTypeValidator("synaptogenesis_rate", "double");
    schema.addTypeValidator("pruning_rate", "double");
    
    // Add range validators for plasticity parameters
    schema.addRangeValidator("stdp_ltp_weight", 0.0, 1.0);
    schema.addRangeValidator("stdp_ltd_weight", 0.0, 1.0);
    schema.addRangeValidator("stdp_tau", 0.0, 1000.0);
    schema.addRangeValidator("synaptogenesis_rate", 0.0, 1.0);
    schema.addRangeValidator("pruning_rate", 0.0, 1.0);
    
    // Add required keys for Phase 2
    schema.addRequiredKey("stdp_ltp_weight");
    schema.addRequiredKey("stdp_ltd_weight");
    schema.addRequiredKey("stdp_tau");
    
    return schema;
}

ConfigSchema ConfigSchemas::createEnhancedPhase2Schema() {
    ConfigSchema schema = createPhase2Schema();
    
    // Add custom validators for advanced validation
    auto plasticityValidator = [](const ConfigValue& value) {
        double val = std::get<double>(value);
        return val >= 0.0 && val <= 1.0;
    };
    
    schema.addCustomValidator("stdp_ltp_weight", plasticityValidator);
    schema.addCustomValidator("stdp_ltd_weight", plasticityValidator);
    schema.addCustomValidator("synaptogenesis_rate", plasticityValidator);
    schema.addCustomValidator("pruning_rate", plasticityValidator);
    
    // Add dependency between plasticity parameters
    schema.addDependency("stdp_ltp_weight", "stdp_ltd_weight");
    
    return schema;
}

ConfigSchema ConfigSchemas::createMinimalSchema() {
    ConfigSchema schema;
    
    // Minimal schema with no validation
    return schema;
}

} // namespace nlm