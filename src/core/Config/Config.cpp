// Config.cpp
#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <unordered_map>
#include <string>
#include <vector>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigEntry> defaultEntries;
    std::vector<ConfigEntry> fileEntries;
    std::vector<ConfigEntry> commandLineEntries;
    
    // Validation and error tracking
    mutable std::vector<ValidationError> validationErrors;
    
    // Performance optimization
    std::unordered_map<std::string, size_t> accessCount;
    std::chrono::steady_clock::time_point loadStartTime;
    double totalLoadTime = 0.0;
    size_t loadCount = 0;
    
    // Thread safety
    mutable std::mutex mutex;
    bool isLocked = false;
    
    // Presets
    std::unordered_map<std::string, std::vector<ConfigEntry>> presets;
    
    // Serialization
    std::unordered_map<std::string, std::string> cachedJson;
    std::unordered_map<std::string, std::string> cachedYaml;
    bool needsSerialization = false;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default entries
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void Config::initializeDefaults() {
    // Phase 6 default configuration
    pImpl->defaultEntries = {
        {"neuron_count", 1000i64, ConfigSource::Default, "Number of neurons in the brain", std::nullopt, std::nullopt, false, "int64_t"},
        {"region_count", 1i64, ConfigSource::Default, "Number of brain regions", std::nullopt, std::nullopt, false, "int64_t"},
        {"connection_probability", 0.1, ConfigSource::Default, "Probability of synaptic connections", std::nullopt, std::nullopt, false, "double"},
        {"stdp_ltp_weight", 0.01, ConfigSource::Default, "STDP Long-Term Potentiation weight", std::nullopt, std::nullopt, false, "double"},
        {"stdp_ltd_weight", 0.012, ConfigSource::Default, "STDP Long-Term Depression weight", std::nullopt, std::nullopt, false, "double"},
        {"stdp_tau", 20.0, ConfigSource::Default, "STDP time constant", std::nullopt, std::nullopt, false, "double"},
        {"synaptogenesis_rate", 0.0001, ConfigSource::Default, "Rate of new synapse formation", std::nullopt, std::nullopt, false, "double"},
        {"pruning_rate", 0.00001f, ConfigSource::Default, "Rate of synapse pruning", std::nullopt, std::nullopt, false, "double"},
        {"simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in seconds", std::nullopt, std::nullopt, false, "double"},
        {"replay_interval", 100i64, ConfigSource::Default, "Memory replay interval in steps", std::nullopt, std::nullopt, false, "int64_t"},
        {"consolidation_interval", 1000i64, ConfigSource::Default, "Memory consolidation interval in steps", std::nullopt, std::nullopt, false, "int64_t"},
        {"random_seed", 42i64, ConfigSource::Default, "Random seed for reproducibility", std::nullopt, std::nullopt, false, "int64_t"},
        {"checkpoint_dir", "./checkpoints", ConfigSource::Default, "Directory for checkpoint files", std::nullopt, std::nullopt, false, "std::string"},
        {"enable_replay", true, ConfigSource::Default, "Enable memory replay system", std::nullopt, std::nullopt, false, "bool"},
        {"enable_development", true, ConfigSource::Default, "Enable developmental processes", std::nullopt, std::nullopt, false, "bool"},
        {"checkpoint_compression", "balanced", ConfigSource::Default, "Checkpoint compression level", std::nullopt, std::nullopt, false, "std::string"},
    };
    
    // Add default entries to current entries
    for (const auto& entry : pImpl->defaultEntries) {
        pImpl->entries.push_back(entry);
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loadStartTime = std::chrono::steady_clock::now();
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        size_t lineNumber = 0;
        std::vector<ValidationError> fileErrors;
        
        while (std::getline(file, line)) {
            ++lineNumber;
            line = trim(line);
            
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string valueStr = trim(line.substr(pos + 1));
                
                if (valueStr.size() >= 2 && 
                    ((valueStr.front() == '"' && valueStr.back() == '"') ||
                     (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                    valueStr = valueStr.substr(1, valueStr.size() - 2);
                }
                
                ConfigValue value;
                std::string typeName;
                
                // Try to determine type from existing entry or try all types
                auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });
                
                if (existingIt != pImpl->entries.end()) {
                    typeName = existingIt->typeName;
                }
                
                // Try to parse based on type name or fallback to heuristics
                if (typeName.empty() || typeName == "int64_t") {
                    try {
                        int64_t intVal = std::stoll(valueStr);
                        value = intVal;
                        typeName = "int64_t";
                    } catch (...) {
                        // Not an integer
                    }
                }
                
                if (typeName.empty() || typeName == "double") {
                    try {
                        size_t pos2;
                        double doubleVal = std::stod(valueStr, &pos2);
                        if (pos2 == valueStr.size()) {
                            value = doubleVal;
                            typeName = "double";
                        }
                    } catch (...) {
                        // Not a double
                    }
                }
                
                if (typeName.empty() || typeName == "bool") {
                    if (valueStr == "true" || valueStr == "1") {
                        value = true;
                        typeName = "bool";
                    } else if (valueStr == "false" || valueStr == "0") {
                        value = false;
                        typeName = "bool";
                    }
                }
                
                if (typeName.empty() || typeName == "std::string") {
                    value = valueStr;
                    typeName = "std::string";
                }
                
                if (std::holds_alternative<ConfigValue>(value)) {
                    // Parsing failed
                    fileErrors.push_back({
                        key, "Line " + std::to_string(lineNumber) + ": Cannot parse value '" + valueStr + "' as " + typeName,
                        ValidationResult::InvalidType
                    });
                    continue;
                }
                
                ConfigEntry entry;
                entry.key = key;
                entry.value = value;
                entry.source = ConfigSource::File;
                entry.description = "Loaded from " + filepath + " line " + std::to_string(lineNumber);
                entry.required = false;
                entry.typeName = typeName;
                
                // Apply validation if type is known
                if (!typeName.empty() && entry.minValue.has_value() && entry.maxValue.has_value()) {
                    // Skip validation for now - would need type conversion
                }
                
                pImpl->entries.push_back(entry);
                pImpl->fileEntries.push_back(entry);
                pImpl->accessCount[key]++;
            }
        }
        
        file.close();
        
        // Validate all loaded entries
        pImpl->validationErrors.clear();
        validate();
        
        pImpl->loadCount++;
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double>(endTime - pImpl->loadStartTime).count();
        pImpl->totalLoadTime += duration;
        
        return fileErrors.empty();
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loadStartTime = std::chrono::steady_clock::now();
    
    bool success = true;
    std::vector<ValidationError> argErrors;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            } else {
                argErrors.push_back({arg, "Missing '=' in command line argument", ValidationResult::InvalidValue});
                success = false;
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    
    if (!argErrors.empty()) {
        pImpl->validationErrors.insert(pImpl->validationErrors.end(), argErrors.begin(), argErrors.end());
    }
    
    pImpl->loadCount++;
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - pImpl->loadStartTime).count();
    pImpl->totalLoadTime += duration;
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write entries in a consistent order
    std::vector<ConfigEntry> allEntries;
    
    // Start with defaults, then file, then command line (overrides)
    for (const auto& entry : pImpl->defaultEntries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key && e.source == ConfigSource::Default; });
        if (it != pImpl->entries.end()) {
            allEntries.push_back(*it);
        }
    }
    
    for (const auto& entry : pImpl->fileEntries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key && e.source == ConfigSource::File; });
        if (it != pImpl->entries.end()) {
            allEntries.push_back(*it);
        }
    }
    
    for (const auto& entry : pImpl->commandLineEntries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key && e.source == ConfigSource::CommandLine; });
        if (it != pImpl->entries.end()) {
            allEntries.push_back(*it);
        }
    }
    
    // Write to file
    for (const auto& entry : allEntries) {
        file << "# " << entry.description << "\n";
        
        // Convert value to string representation
        std::string valueStr;
        if (std::holds_alternative<int64_t>(entry.value)) {
            valueStr = std::to_string(std::get<int64_t>(entry.value));
        } else if (std::holds_alternative<double>(entry.value)) {
            valueStr = std::to_string(std::get<double>(entry.value));
        } else if (std::holds_alternative<bool>(entry.value)) {
            valueStr = std::get<bool>(entry.value) ? "true" : "false";
        } else if (std::holds_alternative<std::string>(entry.value)) {
            valueStr = "\"" + std::get<std::string>(entry.value) + "\"";
        }
        
        file << entry.key << " = " << valueStr << "\n\n";
    }
    
    file.close();
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        // Increment access count for statistics
        pImpl->accessCount[key]++;
        
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Validate value against existing type if present
    auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    std::string typeName;
    if (existingIt != pImpl->entries.end()) {
        typeName = existingIt->typeName;
    }
    
    // Determine type if not already known
    if (typeName.empty()) {
        if (std::holds_alternative<int64_t>(value)) {
            typeName = "int64_t";
        } else if (std::holds_alternative<double>(value)) {
            typeName = "double";
        } else if (std::holds_alternative<bool>(value)) {
            typeName = "bool";
        } else if (std::holds_alternative<std::string>(value)) {
            typeName = "std::string";
        }
    }
    
    ConfigEntry entry;
    entry.key = key;
    entry.value = value;
    entry.source = source;
    entry.description = "Set programmatically";
    entry.required = false;
    entry.typeName = typeName;
    
    if (source == ConfigSource::CommandLine) {
        pImpl->commandLineEntries.push_back(entry);
    } else if (source == ConfigSource::File) {
        pImpl->fileEntries.push_back(entry);
    }
    
    // Replace existing entry or add new one
    auto replaceIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (replaceIt != pImpl->entries.end()) {
        *replaceIt = entry;
    } else {
        pImpl->entries.push_back(entry);
    }
    
    // Mark serialization as needed
    pImpl->needsSerialization = true;
    pImpl->validationErrors.clear();
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Try to parse string value
    ConfigValue configValue;
    
    // Try integer first
    try {
        size_t pos;
        int64_t intVal = std::stoll(value, &pos);
        if (pos == value.size()) {
            configValue = intVal;
        }
    } catch (...) {}
    
    // Try double
    if (!std::holds_alternative<ConfigValue>(configValue)) {
        try {
            size_t pos;
            double doubleVal = std::stod(value, &pos);
            if (pos == value.size()) {
                configValue = doubleVal;
            }
        } catch (...) {}
    }
    
    // Try boolean
    if (!std::holds_alternative<ConfigValue>(configValue)) {
        if (value == "true" || value == "1") {
            configValue = true;
        } else if (value == "false" || value == "0") {
            configValue = false;
        }
    }
    
    // Use as string if still not set
    if (!std::holds_alternative<ConfigValue>(configValue)) {
        configValue = value;
    }
    
    set(key, configValue, source);
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

bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    pImpl->fileEntries.erase(
        std::remove_if(pImpl->fileEntries.begin(), pImpl->fileEntries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->fileEntries.end()
    );
    
    pImpl->commandLineEntries.erase(
        std::remove_if(pImpl->commandLineEntries.begin(), pImpl->commandLineEntries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->commandLineEntries.end()
    );
    
    pImpl->accessCount.erase(key);
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->entries.clear();
    pImpl->fileEntries.clear();
    pImpl->commandLineEntries.clear();
    pImpl->validationErrors.clear();
    pImpl->accessCount.clear();
    pImpl->needsSerialization = false;
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        if (std::find(keys.begin(), keys.end(), entry.key) == keys.end()) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> keys;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            if (std::find(keys.begin(), keys.end(), entry.key) == keys.end()) {
                keys.push_back(entry.key);
            }
        }
    }
    
    return keys;
}

ConfigSource Config::getSource(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    return (it != pImpl->entries.end()) ? it->source : ConfigSource::Default;
}

std::vector<ValidationError> Config::validate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->validationErrors.clear();
    
    for (const auto& entry : pImpl->entries) {
        std::vector<ValidationError> entryErrors;
        if (!validateKey(entry.key, entryErrors)) {
            pImpl->validationErrors.insert(pImpl->validationErrors.end(), entryErrors.begin(), entryErrors.end());
        }
    }
    
    return pImpl->validationErrors;
}

bool Config::validateKey(const std::string& key, std::vector<ValidationError>& errors) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        errors.push_back({key, "Key not found in configuration", ValidationResult::MissingRequired});
        return false;
    }
    
    const auto& entry = *it;
    
    // Type validation
    bool isValidType = false;
    if (entry.typeName == "int64_t" && std::holds_alternative<int64_t>(entry.value)) {
        isValidType = true;
    } else if (entry.typeName == "double" && std::holds_alternative<double>(entry.value)) {
        isValidType = true;
    } else if (entry.typeName == "bool" && std::holds_alternative<bool>(entry.value)) {
        isValidType = true;
    } else if (entry.typeName == "std::string" && std::holds_alternative<std::string>(entry.value)) {
        isValidType = true;
    }
    
    if (!isValidType) {
        errors.push_back({key, "Type mismatch: expected " + entry.typeName + ", got " + configValueToString(entry.value),
                         ValidationResult::InvalidType});
        return false;
    }
    
    // Range validation
    if (entry.minValue.has_value() && entry.maxValue.has_value()) {
        if (!isWithinRange(key, entry.minValue.value())) {
            errors.push_back({key, "Value is below minimum allowed", ValidationResult::OutOfRange});
            return false;
        }
        
        if (!isWithinRange(key, entry.maxValue.value())) {
            errors.push_back({key, "Value is above maximum allowed", ValidationResult::OutOfRange});
            return false;
        }
    }
    
    // Required validation
    if (entry.required) {
        // For non-primitive types, check if value exists
        // For primitive types, all values are valid
        // This is a simple check - in practice, you might want more sophisticated validation
    }
    
    return true;
}

bool Config::isValid() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    validate();
    return pImpl->validationErrors.empty();
}

void Config::setDescription(const std::string& key, const std::string& description) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        it->description = description;
    }
}

std::string Config::getDescription(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    return (it != pImpl->entries.end()) ? it->description : "";
}

void Config::setTypeValidation(const std::string& key, const std::string& typeName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        it->typeName = typeName;
    }
}

std::string Config::getTypeName(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    return (it != pImpl->entries.end()) ? it->typeName : "";
}

void Config::setRangeValidation(const std::string& key, const ConfigValue& minValue, const ConfigValue& maxValue) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->minValue = minValue;
        it->maxValue = maxValue;
    }
}

bool Config::isWithinRange(const std::string& key, const ConfigValue& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    // Simple range check - in practice, this would need type comparison
    // For now, just check if the value is less than or equal to the boundary
    if (std::holds_alternative<int64_t>(it->value) && std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(it->value) >= std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(it->value) && std::holds_alternative<double>(value)) {
        return std::get<double>(it->value) >= std::get<double>(value);
    } else if (std::holds_alternative<bool>(it->value) && std::holds_alternative<bool>(value)) {
        // Boolean comparison - true >= false is typically true
        return !(std::get<bool>(it->value) == false && std::get<bool>(value) == true);
    } else if (std::holds_alternative<std::string>(it->value) && std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(it->value).size() >= std::get<std::string>(value).size();
    }
    
    return false;
}

void Config::markRequired(const std::string& key, bool required) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        it->required = required;
    }
}

bool Config::isRequired(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    return (it != pImpl->entries.end()) ? it->required : false;
}

void Config::merge(const Config& other, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& otherEntry : other.pImpl->entries) {
        // Check if this key already exists
        auto existingIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });
        
        if (existingIt != pImpl->entries.end()) {
            // Replace with new value if higher priority source
            if (source >= existingIt->source) {
                *existingIt = otherEntry;
                existingIt->source = source;
            }
        } else {
            // Add new entry
            ConfigEntry entry = otherEntry;
            entry.source = source;
            pImpl->entries.push_back(entry);
            
            if (source == ConfigSource::File) {
                pImpl->fileEntries.push_back(entry);
            } else if (source == ConfigSource::CommandLine) {
                pImpl->commandLineEntries.push_back(entry);
            }
        }
    }
    
    pImpl->validationErrors.clear();
}

void Config::applyPreset(const std::string& presetName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto presetIt = pImpl->presets.find(presetName);
    if (presetIt == pImpl->presets.end()) {
        return;
    }
    
    merge(Config(presetIt->second), ConfigSource::Programmatic);
}

bool Config::hasPreset(const std::string& presetName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->presets.find(presetName) != pImpl->presets.end();
}

void Config::lock() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    isLocked = true;
}

void Config::unlock() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    isLocked = false;
}

bool Config::tryLock() const {
    std::unique_lock<std::mutex> lock(pImpl->mutex, std::try_to_lock);
    if (lock.owns_lock()) {
        isLocked = true;
        return true;
    }
    return false;
}

std::string Config::toString(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "";
    }
    
    return configValueToString(it->value);
}

std::string Config::configValueToString(const ConfigValue& value) {
    if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    return "";
}

std::optional<ConfigValue> Config::stringToConfigValue(const std::string& str, const std::string& typeName) {
    if (typeName == "int64_t") {
        try {
            return std::stoll(str);
        } catch (...) {
            return std::nullopt;
        }
    } else if (typeName == "double") {
        try {
            return std::stod(str);
        } catch (...) {
            return std::nullopt;
        }
    } else if (typeName == "bool") {
        if (str == "true" || str == "1") {
            return true;
        } else if (str == "false" || str == "0") {
            return false;
        }
        return std::nullopt;
    } else if (typeName == "std::string") {
        // Remove quotes if present
        std::string result = str;
        if (result.size() >= 2 && 
            ((result.front() == '"' && result.back() == '"') ||
             (result.front() == '\'' && result.back() == '\''))) {
            result = result.substr(1, result.size() - 2);
        }
        return result;
    }
    
    return std::nullopt;
}

size_t Config::getEntryCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->entries.size();
}

void Config::reserve(size_t capacity) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.reserve(capacity);
    pImpl->defaultEntries.reserve(capacity);
    pImpl->fileEntries.reserve(capacity);
    pImpl->commandLineEntries.reserve(capacity);
}

size_t Config::getLoadCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->loadCount;
}

double Config::getAverageLoadTime() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->loadCount == 0) {
        return 0.0;
    }
    
    return pImpl->totalLoadTime / pImpl->loadCount;
}

// Explicit template instantiations

// int64_t
template std::optional<int64_t> Config::get<int64_t>(const std::string& key) const;

// double
template std::optional<double> Config::get<double>(const std::string& key) const;

// bool
template std::optional<bool> Config::get<bool>(const std::string& key) const;

// std::string
template std::optional<std::string> Config::get<std::string>(const std::string& key) const;

// getOr instantiations

template int64_t Config::getOr<int64_t>(const std::string& key, const int64_t& defaultValue) const;
template double Config::getOr<double>(const std::string& key, const double& defaultValue) const;
template bool Config::getOr<bool>(const std::string& key, const bool& defaultValue) const;
template std::string Config::getOr<std::string>(const std::string& key, const std::string& defaultValue) const;

} // namespace nlm
