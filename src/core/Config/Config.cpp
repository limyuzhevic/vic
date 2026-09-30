#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <limits>
#include <regex>
#include <chrono>
#include <unordered_map>
#include <set>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ValidatedConfigEntry> entries;
    std::vector<ConfigError> errors;
    
    // Statistics
    size_t totalAccesses = 0;
    size_t successfulAccesses = 0;
    double memoryUsage = 0.0;
    
    // Monitoring data
    std::chrono::system_clock::time_point lastAccessTime;
    
    // Version control
    std::string version;
    
    // Usage patterns
    std::unordered_map<std::string, size_t> accessCount;
    
    // Snapshot data
    std::map<std::string, std::string> snapshots;
    
    // Recent changes
    std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> changes;
    
    Impl() : version("1.0") {
        lastAccessTime = std::chrono::system_clock::now();
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validateOnLoad) {
    pImpl->lastAccessTime = std::chrono::system_clock::now();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        logError("file", "Cannot open configuration file: " + filepath, ConfigSource::File);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == ';') {
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
            
            // Set the configuration value
            set(key, value, ConfigSource::File);
            
            if (validateOnLoad) {
                validate(key);
            }
        } else if (!line.empty()) {
            // Handle comments or malformed lines
            logError("line_" + std::to_string(lineNumber), 
                     "Malformed configuration line: " + line, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv, bool validateOnLoad) {
    pImpl->lastAccessTime = std::chrono::system_clock::now();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
                
                if (validateOnLoad) {
                    validate(key);
                }
            } else {
                logError("arg_" + std::to_string(i), 
                         "Missing value for argument: " + arg, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
            
            if (validateOnLoad) {
                validate(key);
            }
        } else {
            logError("arg_" + std::to_string(i), 
                     "Invalid argument format: " + arg, ConfigSource::CommandLine);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    pImpl->lastAccessTime = std::chrono::system_clock::now();
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logError("file", "Cannot save configuration to file: " + filepath, ConfigSource::Runtime);
        return false;
    }
    
    if (prettyPrint) {
        file << "# NLM Configuration File" << std::endl;
        file << "# Version: " << pImpl->version << std::endl;
        file << "# Generated: " << std::chrono::system_clock::now() << std::endl;
        file << std::endl;
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
            file << "# Last validated: " << (entry.isValidated ? "Yes" : "No") << std::endl;
            
            // Format value based on type
            std::string valueStr;
            if (std::holds_alternative<int>(entry.value)) {
                valueStr = std::to_string(std::get<int>(entry.value));
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                valueStr = std::to_string(std::get<int64_t>(entry.value));
            } else if (std::holds_alternative<double>(entry.value)) {
                valueStr = std::to_string(std::get<double>(entry.value));
            } else if (std::holds_alternative<bool>(entry.value)) {
                valueStr = std::get<bool>(entry.value) ? "true" : "false";
            } else if (std::holds_alternative<std::string>(entry.value)) {
                valueStr = "\"" + std::get<std::string>(entry.value) + "\"";
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                const auto& vec = std::get<std::vector<int>>(entry.value);
                valueStr = "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    valueStr += std::to_string(vec[i]);
                    if (i < vec.size() - 1) valueStr += ", ";
                }
                valueStr += "]";
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                const auto& vec = std::get<std::vector<double>>(entry.value);
                valueStr = "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    valueStr += std::to_string(vec[i]);
                    if (i < vec.size() - 1) valueStr += ", ";
                }
                valueStr += "]";
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                const auto& vec = std::get<std::vector<std::string>>(entry.value);
                valueStr = "[";
                for (size_t i = 0; i < vec.size(); ++i) {
                    valueStr += "\"" + vec[i] + "\"";
                    if (i < vec.size() - 1) valueStr += ", ";
                }
                valueStr += "]";
            }
            
            file << entry.key << " = " << valueStr << std::endl;
            file << std::endl;
        }
    } else {
        // Simple format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key, bool validate) const {
    pImpl->totalAccesses++;
    pImpl->lastAccessTime = std::chrono::system_clock::now();
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ValidatedConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        pImpl->successfulAccesses++;
        
        if (validate && !it->isValidated) {
            validate(key);
        }
        
        return value;
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    return getOr(key, defaultValue, true);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue, bool validate) const {
    auto val = get<T>(key, validate);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Find existing entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ValidatedConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        // Update existing entry
        it->value = value;
        it->source = source;
        it->timestamp = std::chrono::system_clock::now();
        it->isValidated = false;
        
        // Record change
        pImpl->changes.emplace_back(std::chrono::system_clock::now(), 
                                    "Updated " + key + " to " + configValueToString(value));
    } else {
        // Create new entry
        ValidatedConfigEntry newEntry(key, value, source, "", ValidationRules(ParameterType::String, ""));
        pImpl->entries.emplace_back(std::move(newEntry));
        
        // Record change
        pImpl->changes.emplace_back(std::chrono::system_clock::now(), 
                                    "Added new key " + key + " with value " + configValueToString(value));
    }
    
    pImpl->lastAccessTime = std::chrono::system_clock::now();
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

void Config::set(const std::string& key, const std::vector<int>& values, ConfigSource source) {
    set(key, ConfigValue(values), source);
}

void Config::set(const std::string& key, const std::vector<double>& values, ConfigSource source) {
    set(key, ConfigValue(values), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& values, ConfigSource source) {
    set(key, ConfigValue(values), source);
}

bool Config::has(const std::string& key) const {
    pImpl->lastAccessTime = std::chrono::system_clock::now();
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ValidatedConfigEntry& e) { return e.key == key; });
}

ParameterType Config::getParameterType(const std::string& key) const {
    pImpl->lastAccessTime = std::chrono::system_clock::now();
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ValidatedConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return ParameterType::String;
    }
    
    const ConfigValue& value = it->value;
    
    if (std::holds_alternative<int>(value)) return ParameterType::Integer;
    if (std::holds_alternative<int64_t>(value)) return ParameterType::Integer;
    if (std::holds_alternative<double>(value)) return ParameterType::Float;
    if (std::holds_alternative<bool>(value)) return ParameterType::Boolean;
    if (std::holds_alternative<std::string>(value)) return ParameterType::String;
    if (std::holds_alternative<std::vector<int>>(value)) return ParameterType::VectorInt;
    if (std::holds_alternative<std::vector<double>>(value)) return ParameterType::VectorFloat;
    if (std::holds_alternative<std::vector<std::string>>(value)) return ParameterType::VectorString;
    
    return ParameterType::String;
}

bool Config::isValid(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ValidatedConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return it->isValidated;
}

std::vector<int> Config::getVectorInt(const std::string& key, bool validate) const {
    auto val = get<std::vector<int>>(key, validate);
    return val.has_value() ? val.value() : std::vector<int>();
}

std::vector<double> Config::getVectorFloat(const std::string& key, bool validate) const {
    auto val = get<std::vector<double>>(key, validate);
    return val.has_value() ? val.value() : std::vector<double>();
}

std::vector<std::string> Config::getVectorString(const std::string& key, bool validate) const {
    auto val = get<std::vector<std::string>>(key, validate);
    return val.has_value() ? val.value() : std::vector<std::string>();
}

void Config::remove(const std::string& key) {
    remove(key, true);
}

void Config::remove(const std::string& key, bool validate) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ValidatedConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        pImpl->changes.emplace_back(std::chrono::system_clock::now(), 
                                    "Removed key " + key);
        pImpl->entries.erase(it);
    }
    
    pImpl->lastAccessTime = std::chrono::system_clock::now();
}

std::vector<std::string> Config::getKeys(bool includeDefaults) const {
    pImpl->lastAccessTime = std::chrono::system_clock::now();
    
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (includeDefaults || entry.source != ConfigSource::Default) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

std::vector<std::string> Config::getKeys(const std::string& prefix) const {
    pImpl->lastAccessTime = std::chrono::system_clock::now();
    
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(prefix) == 0) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

std::vector<std::string> Config::getKeys(ConfigSource source) const {
    pImpl->lastAccessTime = std::chrono::system_clock::now();
    
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
    pImpl->errors.clear();
    pImpl->changes.clear();
    pImpl->accessCount.clear();
    pImpl->lastAccessTime = std::chrono::system_clock::now();
}

std::string Config::summary() const {
    pImpl->lastAccessTime = std::chrono::system_clock::now();
    
    std::stringstream ss;
    ss << "NLM Configuration Summary" << std::endl;
    ss << "===========================" << std::endl;
    ss << "Total entries: " << pImpl->entries.size() << std::endl;
    ss << "Access count: " << pImpl->totalAccesses << std::endl;
    ss << "Successful accesses: " << pImpl->successfulAccesses << std::endl;
    ss << "Success rate: " << (pImpl->totalAccesses > 0 ? 
        (pImpl->successfulAccesses * 100.0 / pImpl->totalAccesses) : 0.0) << "%" << std::endl;
    ss << "Default entries: " << getKeys(ConfigSource::Default).size() << std::endl;
    ss << "File entries: " << getKeys(ConfigSource::File).size() << std::endl;
    ss << "Command line entries: " << getKeys(ConfigSource::CommandLine).size() << std::endl;
    ss << "Runtime entries: " << getKeys(ConfigSource::Runtime).size() << std::endl;
    
    return ss.str();
}

std::string Config::detailedSummary() const {
    pImpl->lastAccessTime = std::chrono::system_clock::now();
    
    std::stringstream ss;
    ss << summary() << std::endl;
    ss << std::endl;
    ss << "Recent changes:" << std::endl;
    
    size_t changesToShow = std::min(pImpl->changes.size(), size_t(10));n    for (size_t i = 0; i < changesToShow; ++i) {
        auto time = pImpl->changes[i].first;
        auto change = pImpl->changes[i].second;
        ss << "  " << i + 1 << ". [" << std::chrono::system_clock::to_time_t(time) << "] " << change << std::endl;
    }
    
    ss << std::endl;
    ss << "Parameter types:" << std::endl;
    
    std::map<std::string, ParameterType> typeCount;
    for (const auto& entry : pImpl->entries) {
        typeCount[entry.key] = getParameterType(entry.key);
    }
    
    for (const auto& pair : typeCount) {
        ss << "  " << pair.first << ": " << static_cast<int>(pair.second) << std::endl;
    }
    
    return ss.str();
}

bool Config::validateAll() const {
    bool allValid = true;
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key)) {
            allValid = false;
        }
    }
    
    if (!allValid) {
        return false;
    }
    
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.isValidated) {
            errors.push_back("Key \"" + entry.key + "\" is not validated");
        }
    }
    
    for (const auto& error : pImpl->errors) {
        errors.push_back(error.message);
    }
    
    return errors;
}

bool Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ValidatedConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        logError(key, "Cannot validate non-existent key", ConfigSource::Runtime);
        return false;
    }
    
    bool isValid = true;
    std::string validationMessage;
    
    // Basic type validation
    ParameterType paramType = getParameterType(key);
    isValid = validateType(it->value, paramType);
    validationMessage = "Basic type validation passed";
    
    if (!isValid) {
        logError(key, "Basic type validation failed for key: " + key, ConfigSource::Runtime);
        it->isValidated = false;
        return false;
    }
    
    // Check parameter schema if available
    if (hasParameterSchema(key)) {
        ParameterSchema schema = getParameterSchema(key);
        if (!validateConstraints(it->value, schema)) {
            isValid = false;
            validationMessage = "Schema validation failed";
        }
    }
    
    // Check validation rules
    if (validationRules.find(key) != validationRules.end()) {
        auto rule = validationRules.find(key)->second;
        if (!rule.first(it->value)) {
            isValid = false;
            validationMessage = rule.second;
        }
    }
    
    if (isValid) {
        it->isValidated = true;
        it->timestamp = std::chrono::system_clock::now();
    } else {
        logError(key, "Validation failed: " + validationMessage, ConfigSource::Runtime);
    }
    
    return isValid;
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validate(entry.key)) {
            errors.push_back("Validation failed for key: " + entry.key);
        }
    }
    
    return errors;
}

void Config::addParameterSchema(const ParameterSchema& schema) {
    parameterSchemas[schema.key] = schema;
}

bool Config::hasParameterSchema(const std::string& key) const {
    return parameterSchemas.find(key) != parameterSchemas.end();
}

ParameterSchema Config::getParameterSchema(const std::string& key) const {
    if (hasParameterSchema(key)) {
        return parameterSchemas.find(key)->second;
    }
    
    return ParameterSchema{key, ParameterType::String, "No schema defined", std::nullopt, false};
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ValidatedConfigEntry& e) { return e.key == entry.key; });n        
        if (it == other.pImpl->entries.end() || it->value != entry.value) {
            return false;
        }
    }
    
    return true;
}

bool Config::similar(const Config& other, double tolerance) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ValidatedConfigEntry& e) { return e.key == entry.key; });n        
        if (it == other.pImpl->entries.end()) {
            return false;
        }
        
        const ConfigValue& val1 = entry.value;
        const ConfigValue& val2 = it->value;
        
        if (std::holds_alternative<double>(val1) && std::holds_alternative<double>(val2)) {
            double diff = std::abs(std::get<double>(val1) - std::get<double>(val2));
            if (diff > tolerance) {
                return false;
            }
        } else if (val1 != val2) {
            return false;
        }
    }
    
    return true;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        // Find existing entry
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ValidatedConfigEntry& e) { return e.key == entry.key; });n        
        if (it == pImpl->entries.end()) {
            // Add new entry with different source
            ValidatedConfigEntry newEntry(entry.key, entry.value, source, entry.description, entry.validation);
            pImpl->entries.push_back(std::move(newEntry));
        } else {
            // Update existing entry
            it->value = entry.value;
            it->source = entry.source; // Keep original source priority
            it->timestamp = std::chrono::system_clock::now();
            it->isValidated = false;
        }
    }
}

void Config::replace(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Find existing entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ValidatedConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        // Replace existing entry
        it->value = value;
        it->source = source;
        it->timestamp = std::chrono::system_clock::now();
        it->isValidated = false;
        
        pImpl->changes.emplace_back(std::chrono::system_clock::now(), 
                                    "Replaced key " + key + " with new value");
    } else {
        // Add new entry
        ValidatedConfigEntry newEntry(key, value, source, "", ValidationRules(ParameterType::String, ""));
        pImpl->entries.push_back(std::move(newEntry));
        
        pImpl->changes.emplace_back(std::chrono::system_clock::now(), 
                                    "Added new key " + key + " via replace operation");
    }
}

std::vector<std::string> Config::getRecentChanges(size_t limit) const {
    std::vector<std::string> changes;
    
    size_t changesToShow = std::min(pImpl->changes.size(), limit);
    for (size_t i = 0; i < changesToShow; ++i) {
        changes.push_back(pImpl->changes[i].second);
    }
    
    return changes;
}

bool Config::hasChangedSince(const std::string& snapshotId) const {
    if (snapshots.find(snapshotId) == snapshots.end()) {
        return false;
    }
    
    std::string snapshotConfig = snapshots.find(snapshotId)->second;
    
    // Simple comparison - in practice would need proper snapshot comparison
    return snapshotConfig != exportToString();
}

std::string Config::getStatistics() const {
    std::stringstream ss;
    ss << "NLM Configuration Statistics" << std::endl;
    ss << "===========================" << std::endl;
    ss << "Total entries: " << pImpl->entries.size() << std::endl;
    ss << "Total accesses: " << pImpl->totalAccesses << std::endl;
    ss << "Successful accesses: " << pImpl->successfulAccesses << std::endl;
    if (pImpl->totalAccesses > 0) {
        ss << "Access success rate: " << (pImpl->successfulAccesses * 100.0 / pImpl->totalAccesses) << "%" << std::endl;
    }
    ss << "Default entries: " << getKeys(ConfigSource::Default).size() << std::endl;
    ss << "File entries: " << getKeys(ConfigSource::File).size() << std::endl;
    ss << "Command line entries: " << getKeys(ConfigSource::CommandLine).size() << std::endl;
    ss << "Runtime entries: " << getKeys(ConfigSource::Runtime).size() << std::endl;
    ss << "Generated entries: " << getKeys(ConfigSource::Generated).size() << std::endl;
    ss << "Inherited entries: " << getKeys(ConfigSource::Inherited).size() << std::endl;
    
    // Parameter type distribution
    std::map<ParameterType, size_t> typeCounts;
    for (const auto& entry : pImpl->entries) {
        typeCounts[getParameterType(entry.key)]++;
    }
    
    ss << std::endl;
    ss << "Parameter type distribution:" << std::endl;
    for (const auto& pair : typeCounts) {
        ss << "  " << static_cast<int>(pair.first) << ": " << pair.second << std::endl;
    }
    
    // Validation status
    size_t validatedCount = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.isValidated) {
            validatedCount++;
        }
    }
    
    ss << std::endl;
    ss << "Validation status:" << std::endl;
    ss << "  Validated entries: " << validatedCount << std::endl;
    ss << "  Unvalidated entries: " << (pImpl->entries.size() - validatedCount) << std::endl;
    if (pImpl->entries.size() > 0) {
        ss << "  Validation rate: " << (validatedCount * 100.0 / pImpl->entries.size()) << "%" << std::endl;
    }
    
    return ss.str();
}

std::map<std::string, double> Config::getPerformanceMetrics() const {
    std::map<std::string, double> metrics;
    
    metrics["total_accesses"] = pImpl->totalAccesses;
    metrics["successful_accesses"] = pImpl->successfulAccesses;
    metrics["success_rate"] = (pImpl->totalAccesses > 0 ? 
        (pImpl->successfulAccesses * 100.0 / pImpl->totalAccesses) : 0.0);
    metrics["validation_rate"] = (pImpl->entries.size() > 0 ? 
        (getKeys(ConfigSource::Default).size() * 100.0 / pImpl->entries.size()) : 0.0);
    metrics["recent_accesses_per_second"] = 0.0; // Would need implementation
    
    // Calculate memory usage
    double memoryPerEntry = sizeof(ValidatedConfigEntry) + 100.0; // Estimated
    metrics["estimated_memory_mb"] = (pImpl->entries.size() * memoryPerEntry) / (1024.0 * 1024.0);
    
    return metrics;
}

void Config::setVersion(const std::string& version) {
    pImpl->version = version;
    pImpl->changes.emplace_back(std::chrono::system_clock::now(), 
                                "Configuration version changed to: " + version);
}

std::string Config::getVersion() const {
    return pImpl->version;
}

bool Config::needsMigration(const std::string& targetVersion) const {
    // Simple version comparison - in practice would need proper version handling
    return pImpl->version != targetVersion;
}

bool Config::createBackup(const std::string& backupPath) const {
    std::ofstream file(backupPath);
    if (!file.is_open()) {
        logError("backup", "Cannot create backup file: " + backupPath, ConfigSource::Runtime);
        return false;
    }
    
    file << exportToString(true); // Pretty print backup
    
    pImpl->snapshots[backupPath] = exportToString();
    
    return true;
}

bool Config::restoreBackup(const std::string& backupPath) {
    std::ifstream file(backupPath);
    if (!file.is_open()) {
        logError("backup", "Cannot restore backup file: " + backupPath, ConfigSource::Runtime);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    if (!importFromString(buffer.str())) {
        logError("backup", "Failed to restore backup: " + backupPath, ConfigSource::Runtime);
        return false;
    }
    
    return true;
}

void Config::enableMonitoring(bool enable) {
    monitoringEnabled = enable;
    
    if (monitoringEnabled) {
        monitoringData["monitoring_enabled"] = "true";
    } else {
        monitoringData["monitoring_enabled"] = "false";
    }
}

bool Config::isMonitoringEnabled() const {
    return monitoringEnabled;
}

std::map<std::string, std::string> Config::getMonitoringData() const {
    std::map<std::string, std::string> data = monitoringData;
    
    data["total_entries"] = std::to_string(pImpl->entries.size());
    data["total_accesses"] = std::to_string(pImpl->totalAccesses);
    data["version"] = pImpl->version;
    
    if (!pImpl->changes.empty()) {
        auto lastChange = pImpl->changes.back();
        data["last_change"] = std::to_string(std::chrono::system_clock::to_time_t(lastChange.first));
        data["last_change_description"] = lastChange.second;
    }
    
    return data;
}

void Config::addValidationRule(const std::string& key,
                              const std::function<bool(const ConfigValue&)>& validator,
                              const std::string& errorMessage) {
    validationRules[key] = std::make_pair(validator, errorMessage);
}

void Config::removeValidationRule(const std::string& key) {
    validationRules.erase(key);
}

void Config::addParameterConstraint(const std::string& key,
                                   ParameterType expectedType,
                                   const std::string& constraintDescription,
                                   const std::map<std::string, std::string>& constraints) {
    parameterConstraints[key] = constraints;
}

void Config::removeParameterConstraint(const std::string& key) {
    parameterConstraints.erase(key);
}

std::string Config::getParameterConstraints(const std::string& key) const {
    if (parameterConstraints.find(key) != parameterConstraints.end()) {
        std::stringstream ss;
        ss << "Constraints for \"" << key << "\":" << std::endl;
        for (const auto& pair : parameterConstraints.find(key)->second) {
            ss << "  " << pair.first << ": " << pair.second << std::endl;
        }
        return ss.str();
    }
    
    return "No constraints defined for key: " + key;
}

void Config::batchSet(const std::map<std::string, ConfigValue>& keyValues,
                     ConfigSource source) {
    for (const auto& pair : keyValues) {
        set(pair.first, pair.second, source);
    }
}

void Config::batchRemove(const std::vector<std::string>& keys) {
    for (const auto& key : keys) {
        remove(key);
    }
}

std::map<std::string, ConfigValue> Config::batchGet(const std::vector<std::string>& keys) const {
    std::map<std::string, ConfigValue> results;
    
    for (const auto& key : keys) {
        auto val = get<std::string>(key); // Use string getter for simplicity
        if (val.has_value()) {
            results[key] = ConfigValue(val.value());
        }
    }
    
    return results;
}

bool Config::exportWithSchema(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "NLM Configuration with Schema" << std::endl;
    file << "Version: " << pImpl->version << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "Key: " << entry.key << std::endl;
        file << "Type: " << static_cast<int>(getParameterType(entry.key)) << std::endl;
        file << "Value: " << configValueToString(entry.value) << std::endl;
        file << "Source: " << static_cast<int>(entry.source) << std::endl;
        file << "Validated: " << (entry.isValidated ? "Yes" : "No") << std::endl;
        file << "Description: " << entry.description << std::endl;
        file << std::endl;
    }
    
    return true;
}

bool Config::importWithSchema(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentKey;
    std::string currentType;
    std::string currentValue;
    std::string currentSource;
    bool inEntry = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        
        if (line.empty()) {
            if (inEntry && !currentKey.empty()) {
                // Process completed entry
                try {
                    ConfigValue value;
                    if (currentType == "integer" && !currentValue.empty()) {
                        value = std::stoi(currentValue);
                    } else if (currentType == "float" && !currentValue.empty()) {
                        value = std::stod(currentValue);
                    } else if (currentType == "boolean" && !currentValue.empty()) {
                        value = (currentValue == "true" || currentValue == "1");
                    } else if (currentType == "string" && !currentValue.empty()) {
                        // Remove quotes if present
                        if (currentValue.size() >= 2 && 
                            ((currentValue.front() == '"' && currentValue.back() == '"') ||
                             (currentValue.front() == '\'' && currentValue.back() == '\''))) {
                            currentValue = currentValue.substr(1, currentValue.size() - 2);
                        }
                        value = currentValue;
                    } else {
                        value = currentValue;
                    }
                    
                    ConfigSource source = ConfigSource::Runtime;
                    if (currentSource == "file") source = ConfigSource::File;
                    else if (currentSource == "commandline") source = ConfigSource::CommandLine;
                    else if (currentSource == "default") source = ConfigSource::Default;
                    
                    set(currentKey, value, source);
                } catch (...) {
                    // Skip malformed entries
                }
                
                currentKey.clear();
                currentType.clear();
                currentValue.clear();
                currentSource.clear();
                inEntry = false;
            }
        } else if (line.find("Key: ") == 0) {
            currentKey = line.substr(5);
            inEntry = true;
        } else if (line.find("Type: ") == 0) {
            currentType = line.substr(6);
        } else if (line.find("Value: ") == 0) {
            currentValue = line.substr(7);
        } else if (line.find("Source: ") == 0) {
            currentSource = line.substr(8);
        }
    }
    
    return true;
}

std::string Config::createSnapshot() const {
    std::string snapshotId = "snapshot_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    std::string snapshotData = exportToString();
    
    pImpl->snapshots[snapshotId] = snapshotData;
    
    pImpl->changes.emplace_back(std::chrono::system_clock::now(), 
                                "Created snapshot: " + snapshotId);
    
    return snapshotId;
}

bool Config::restoreSnapshot(const std::string& snapshotId) {
    if (pImpl->snapshots.find(snapshotId) == pImpl->snapshots.end()) {
        return false;
    }
    
    std::string snapshotData = pImpl->snapshots.find(snapshotId)->second;
    
    // Clear current configuration
    clear();
    
    // Restore from snapshot
    if (!importFromString(snapshotData)) {
        return false;
    }
    
    pImpl->changes.emplace_back(std::chrono::system_clock::now(), 
                                "Restored snapshot: " + snapshotId);
    
    return true;
}

std::string Config::getUsagePatterns() const {
    std::stringstream ss;
    ss << "NLM Configuration Usage Patterns" << std::endl;
    ss << "===============================" << std::endl;
    ss << std::endl;
    
    if (pImpl->accessCount.empty()) {
        ss << "No usage patterns available (no accesses recorded)" << std::endl;
        return ss.str();
    }
    
    ss << "Most accessed keys:" << std::endl;
    
    std::vector<std::pair<std::string, size_t>> sortedAccesses;
    for (const auto& pair : pImpl->accessCount) {
        sortedAccesses.emplace_back(pair.first, pair.second);
    }
    
    std::sort(sortedAccesses.begin(), sortedAccesses.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });n    
    size_t limit = std::min(sortedAccesses.size(), size_t(10));n    for (size_t i = 0; i < limit; ++i) {
        ss << "  " << i + 1 << ". " << sortedAccesses[i].first 
           << " (accessed " << sortedAccesses[i].second << " times)" << std::endl;
    }
    
    return ss.str();
}

std::map<std::string, double> Config::getParameterUsageStats(const std::string& key) const {
    std::map<std::string, double> stats;
    
    if (pImpl->accessCount.find(key) != pImpl->accessCount.end()) {
        stats["access_count"] = pImpl->accessCount.find(key)->second;
    } else {
        stats["access_count"] = 0.0;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ValidatedConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        stats["validation_success_rate"] = it->isValidated ? 1.0 : 0.0;
    } else {
        stats["validation_success_rate"] = 0.0;
    }
    
    return stats;
}

void Config::logError(const std::string& key, const std::string& message, ConfigSource source) {
    ConfigError error(key, message, source);
    pImpl->errors.push_back(error);
}

void Config::validateEntry(const ValidatedConfigEntry& entry) const {
    bool isValid = true;
    std::string errorMessage;
    
    // Check parameter type
    ParameterType paramType = getParameterType(entry.key);
    isValid = validateType(entry.value, paramType);
    if (!isValid) {
        errorMessage = "Type validation failed for key: " + entry.key;
    }
    
    // Check constraints
    if (isValid && hasParameterSchema(entry.key)) {
        ParameterSchema schema = getParameterSchema(entry.key);
        if (!validateConstraints(entry.value, schema)) {
            isValid = false;
            errorMessage = "Schema validation failed for key: " + entry.key;
        }
    }
    
    // Check validation rules
    if (isValid && validationRules.find(entry.key) != validationRules.end()) {
        auto rule = validationRules.find(entry.key)->second;
        if (!rule.first(entry.value)) {
            isValid = false;
            errorMessage = rule.second;
        }
    }
    
    if (!isValid) {
        logError(entry.key, errorMessage, entry.source);
    }
}

bool Config::validateAgainstSchema(const std::string& key, const ConfigValue& value) const {
    if (!hasParameterSchema(key)) {
        return true; // No schema to validate against
    }
    
    ParameterSchema schema = getParameterSchema(key);
    return validateConstraints(value, schema);
}

// Static helper functions
bool Config::isValidInteger(const ConfigValue& value, int64_t min, int64_t max) {
    if (!std::holds_alternative<int>(value) && !std::holds_alternative<int64_t>(value)) {
        return false;
    }
    
    int64_t val = std::holds_alternative<int>(value) ? 
        std::get<int>(value) : std::get<int64_t>(value);
    
    return val >= min && val <= max;
}

bool Config::isValidFloat(const ConfigValue& value, double min, double max) {
    if (!std::holds_alternative<double>(value)) {
        return false;
    }
    
    double val = std::get<double>(value);
    
    return val >= min && val <= max;
}

bool Config::isValidString(const ConfigValue& value, size_t minLength, size_t maxLength) {
    if (!std::holds_alternative<std::string>(value)) {
        return false;
    }
    
    std::string val = std::get<std::string>(value);
    
    return val.length() >= minLength && val.length() <= maxLength;
}

bool Config::isValidVector(const ConfigValue& value, size_t minSize, size_t maxSize) {
    if (std::holds_alternative<std::vector<int>>(value) ||
        std::holds_alternative<std::vector<double>>(value) ||
        std::holds_alternative<std::vector<std::string>>(value)) {
        return true;
    }
    
    return false;
}

std::string Config::parameterTypeToString(ParameterType type) {
    switch (type) {
        case ParameterType::Integer: return "integer";
        case ParameterType::Float: return "float";
        case ParameterType::Boolean: return "boolean";
        case ParameterType::String: return "string";
        case ParameterType::VectorInt: return "vector_int";
        case ParameterType::VectorFloat: return "vector_float";
        case ParameterType::VectorString: return "vector_string";
        case ParameterType::NeuronType: return "neuron_type";
        case ParameterType::SynapseType: return "synapse_type";
        case ParameterType::DevelopmentalStage: return "developmental_stage";
        case ParameterType::ActionType: return "action_type";
        default: return "unknown";
    }
}

ParameterType Config::stringToParameterType(const std::string& typeStr) {
    std::string type = toLower(typeStr);
    
    if (type == "integer") return ParameterType::Integer;
    if (type == "float") return ParameterType::Float;
    if (type == "boolean") return ParameterType::Boolean;
    if (type == "string") return ParameterType::String;
    if (type == "vector_int") return ParameterType::VectorInt;
    if (type == "vector_float") return ParameterType::VectorFloat;
    if (type == "vector_string") return ParameterType::VectorString;
    if (type == "neuron_type") return ParameterType::NeuronType;
    if (type == "synapse_type") return ParameterType::SynapseType;
    if (type == "developmental_stage") return ParameterType::DevelopmentalStage;
    if (type == "action_type") return ParameterType::ActionType;
    
    return ParameterType::String;
}

std::string Config::configValueToString(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            ss << vec[i];
            if (i < vec.size() - 1) ss << ", ";
        }
        ss << "]";
        return ss.str();
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            ss << vec[i];
            if (i < vec.size() - 1) ss << ", ";
        }
        ss << "]";
        return ss.str();
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            ss << "\"" << vec[i] << "\"";
            if (i < vec.size() - 1) ss << ", ";
        }
        ss << "]";
        return ss.str();
    }
    
    return "";
}

std::optional<ConfigValue> Config::stringToConfigValue(const std::string& value, ParameterType type) {
    switch (type) {
        case ParameterType::Integer: {
            try {
                int val = std::stoi(value);
                return ConfigValue(val);
            } catch (...) {
                return std::nullopt;
            }
        }
        case ParameterType::Float: {
            try {
                double val = std::stod(value);
                return ConfigValue(val);
            } catch (...) {
                return std::nullopt;
            }
        }
        case ParameterType::Boolean: {
            std::string lower = toLower(value);
            if (lower == "true" || lower == "1" || lower == "yes") {
                return ConfigValue(true);
            } else if (lower == "false" || lower == "0" || lower == "no") {
                return ConfigValue(false);
            } else {
                return std::nullopt;
            }
        }
        case ParameterType::String: {
            // Remove quotes if present
            std::string val = value;
            if (val.size() >= 2 && 
                ((val.front() == '"' && val.back() == '"') ||
                 (val.front() == '\'' && val.back() == '\''))) {
                val = val.substr(1, val.size() - 2);
            }
            return ConfigValue(val);
        }
        default:
            return std::nullopt;
    }
}

// Template instantiations
template std::optional<int> Config::get<int>(const std::string& key, bool validate) const;
template int Config::getOr<int>(const std::string& key, const int& defaultValue) const;
template int Config::getOr<int>(const std::string& key, const int& defaultValue, bool validate) const;

} // namespace nlm
