#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <set>
#include <fstream>
#include <iostream>
#include <cmath>
#include <limits>
#include <map>
#include <vector>
#include <cctype>

namespace nlm {

// Simple JSON parser for basic objects
class SimpleJsonParser {
public:
    static bool parse(const std::string& json, std::unordered_map<std::string, ConfigValue>& result) {
        try {
            // Very simple JSON parsing - can be extended later
            if (json.empty() || json[0] != '{') return false;
            
            size_t pos = 1;
            while (pos < json.size()) {
                // Find key
                size_t keyStart = json.find('"', pos);
                if (keyStart == std::string::npos) break;
                size_t keyEnd = json.find('"', keyStart + 1);
                if (keyEnd == std::string::npos) break;
                
                std::string key = json.substr(keyStart + 1, keyEnd - keyStart - 1);
                pos = keyEnd + 2; // Skip ",
                
                // Skip whitespace
                while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n')) pos++;
                
                if (pos >= json.size() || json[pos] != ':') break;
                pos++; // Skip :
                
                // Skip whitespace
                while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n')) pos++;
                
                // Parse value
                ConfigValue value;
                if (pos < json.size() && json[pos] == '"') {
                    // String value
                    size_t valStart = pos + 1;
                    size_t valEnd = json.find('"', valStart);
                    if (valEnd == std::string::npos) return false;
                    value = json.substr(valStart, valEnd - valStart);
                    pos = valEnd + 1;
                } else if (pos + 3 < json.size() && json.substr(pos, 4) == "true") {
                    value = true;
                    pos += 4;
                } else if (pos + 4 < json.size() && json.substr(pos, 5) == "false") {
                    value = false;
                    pos += 5;
                } else if (pos < json.size() && json[pos] == 'n') {
                    // null
                    pos += 4; // Skip "null"
                } else {
                    // Try to parse as number
                    size_t valEnd = pos;
                    while (valEnd < json.size() && (std::isdigit(json[valEnd]) || json[valEnd] == '.' || json[valEnd] == '-' || json[valEnd] == 'e' || json[valEnd] == 'E')) {
                        valEnd++;
                    }
                    if (valEnd > pos) {
                        std::string numStr = json.substr(pos, valEnd - pos);
                        try {
                            // Try double first
                            size_t decimalPos = numStr.find('.');
                            if (decimalPos != std::string::npos) {
                                value = std::stod(numStr);
                            } else {
                                size_t expPos = numStr.find('e');
                                if (expPos != std::string::npos || numStr.find('E') != std::string::npos) {
                                    value = std::stod(numStr);
                                } else {
                                    // Try int64_t
                                    value = std::stoll(numStr);
                                }
                            }
                        } catch (...) {
                            return false;
                        }
                        pos = valEnd;
                    } else {
                        return false;
                    }
                }
                
                // Store result
                result[key] = value;
                
                // Skip whitespace
                while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n')) pos++;
                
                // Check for comma or end
                if (pos < json.size() && json[pos] == ',') {
                    pos++; // Skip comma
                    continue;
                } else if (pos < json.size() && json[pos] == '}') {
                    break;
                }
            }
            
            return true;
        } catch (...) {
            return false;
        }
    }
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
    std::unordered_map<std::string, size_t> keyIndex; // For fast lookup
    std::unordered_map<std::string, size_t> keyCount; // For duplicate detection
    bool lookupsOptimized;
    
    Impl() : lookupsOptimized(false) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first, then YAML, then simple format
    bool success = false;
    
    // Check file extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    if (ext == "json") {
        success = parseJsonFile(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        success = parseYamlFile(filepath);
    } else {
        success = parseSimpleFile(filepath);
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Enhanced argument parsing with more formats
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setWithDescription(key, ConfigValue(value), "Command line argument", ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setWithDescription(key, ConfigValue(value), "Command line argument", ConfigSource::CommandLine);
        }
        // Handle --key value format (spaced)
        else if (arg[0] == '-' && arg.size() > 1 && std::isalpha(arg[1])) {
            // This could be a standalone flag
            // For now, treat as unrecognized
            success = false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::string ext = format;
    if (ext.empty()) {
        // Determine from file extension
        size_t dotPos = filepath.find_last_of('.');
        if (dotPos != std::string::npos) {
            ext = filepath.substr(dotPos + 1);
        }
    }
    
    if (ext == "json") {
        return writeJsonFile(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return writeYamlFile(filepath);
    } else {
        return writeSimpleFile(filepath);
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Fast lookup using index
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const ConfigEntry& entry = pImpl->entries[index];
    
    try {
        return std::get<T>(entry.value);
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
    setWithDescription(key, value, "", source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    setWithDescription(key, ConfigValue(value), "", source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    setWithDescription(key, ConfigValue(value), "", source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    setWithDescription(key, ConfigValue(value), "", source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    setWithDescription(key, ConfigValue(value), "", source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    setWithDescription(key, ConfigValue(value), "", source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    setWithDescription(key, ConfigValue(value), "", source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    setWithDescription(key, ConfigValue(value), "", source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    setWithDescription(key, ConfigValue(value), "", source);
}

template<typename T>
bool Config::setValidated(const std::string& key, const T& value, ConfigSource source) {
    // Validate against schemas first
    bool schemaValid = true;
    for (const auto& schema : pImpl->schemas) {
        if (schema.key == key) {
            if (!validateValueType(value, schema.type)) {
                schemaValid = false;
                break;
            }
            // Could add more complex validation here
            break;
        }
    }
    
    if (schemaValid) {
        set(key, ConfigValue(value), source);
        return true;
    }
    
    return false;
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas.push_back(schema);
}

bool Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& schema : pImpl->schemas) {
        auto it = pImpl->keyIndex.find(schema.key);
        if (it == pImpl->keyIndex.end()) {
            if (schema.required) {
                errors.push_back("Required key missing: " + schema.key);
            }
            continue;
        }
        
        size_t index = it->second;
        if (index >= pImpl->entries.size()) continue;
        
        const ConfigEntry& entry = pImpl->entries[index];
        
        // Check type
        if (!validateValueType(entry.value, schema.type)) {
            errors.push_back("Type mismatch for key " + schema.key + ": expected " + 
                           (std::is_same_v<typename std::remove_reference<decltype(schema.type)>::type, std::type_index> ? 
                           "unknown type" : "known type") + ", got variant type");
        }
        
        // Validate against validators
        for (const auto& validator : schema.validators) {
            if (!validator(entry.value)) {
                errors.push_back("Validation failed for key " + schema.key);
                break;
            }
        }
    }
    
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& schema : pImpl->schemas) {
        auto it = pImpl->keyIndex.find(schema.key);
        if (it == pImpl->keyIndex.end()) {
            if (schema.required) {
                errors.push_back("Required key missing: " + schema.key);
            }
            continue;
        }
        
        size_t index = it->second;
        if (index >= pImpl->entries.size()) continue;
        
        const ConfigEntry& entry = pImpl->entries[index];
        
        // Check type
        if (!validateValueType(entry.value, schema.type)) {
            errors.push_back("Type mismatch for key " + schema.key + ": expected " + 
                           (std::is_same_v<typename std::remove_reference<decltype(schema.type)>::type, std::type_index> ? 
                           "unknown type" : "known type") + ", got variant type");
        }
        
        // Validate against validators
        for (const auto& validator : schema.validators) {
            if (!validator(entry.value)) {
                errors.push_back("Validation failed for key " + schema.key);
                break;
            }
        }
    }
    
    return errors;
}

bool Config::has(const std::string& key) const {
    return pImpl->keyIndex.find(key) != pImpl->keyIndex.end();
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return ConfigSource::Default;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return ConfigSource::Default;
    }
    
    return pImpl->entries[index].source;
}

std::chrono::system_clock::time_point Config::getTimestamp(const std::string& key) const {
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::chrono::system_clock::now();
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::chrono::system_clock::now();
    }
    
    return pImpl->entries[index].timestamp;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return "";
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return "";
    }
    
    return pImpl->entries[index].description;
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return;
    }
    
    size_t index = it->second;
    
    // Remove from entries
    pImpl->entries.erase(pImpl->entries.begin() + index);
    
    // Update indexes
    pImpl->keyIndex.clear();
    pImpl->keyCount.clear();
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        pImpl->keyIndex[pImpl->entries[i].key] = i;
        pImpl->keyCount[pImpl->entries[i].key] = 1; // Simplified - assume no duplicates
    }
    
    pImpl->lookupsOptimized = false;
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
    pImpl->entries.clear();
    pImpl->keyIndex.clear();
    pImpl->keyCount.clear();
    pImpl->lookupsOptimized = false;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << pImpl->schemas.size() << " schemas):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[int array]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[double array]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[string array]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ") - " << entry.description << "\n";
    }
    
    if (!pImpl->schemas.empty()) {
        oss << "\nSchemas:";
        for (const auto& schema : pImpl->schemas) {
            oss << "\n  - " << schema.key << " (required: " << (schema.required ? "yes" : "no") 
                << ", type: " << "unknown" << ") - " << schema.description;
        }
    }
    
    return oss.str();
}

std::string Config::detailedSummary() const {
    std::ostringstream oss;
    oss << "=== DETAILED CONFIGURATION SUMMARY ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Total schemas: " << pImpl->schemas.size() << "\n";
    oss << "Estimated memory usage: " << getEstimatedMemoryUsage() << " bytes\n\n";
    
    // Group by source
    std::unordered_map<ConfigSource, std::vector<const ConfigEntry*>> bySource;
    for (const auto& entry : pImpl->entries) {
        bySource[entry.source].push_back(&entry);
    }
    
    for (const auto& pair : bySource) {
        oss << "Source " << static_cast<int>(pair.first) << ": " << pair.second.size() << " entries\n";
    }
    
    // Validation status
    oss << "\nValidation status: " << (validate() ? "VALID" : "INVALID") << "\n";
    if (!validate()) {
        auto errors = getValidationErrors();
        oss << "Errors:";
        for (const auto& error : errors) {
            oss << "\n  - " << error;
        }
    }
    
    return oss.str();
}

void Config::optimizeLookups() {
    if (pImpl->lookupsOptimized) {
        return;
    }
    
    pImpl->keyIndex.clear();
    pImpl->keyCount.clear();
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        pImpl->keyIndex[pImpl->entries[i].key] = i;
        // Simple duplicate counting
        pImpl->keyCount[pImpl->entries[i].key] = 1;
    }
    
    pImpl->lookupsOptimized = true;
}

size_t Config::getEstimatedMemoryUsage() const {
    size_t total = 0;
    for (const auto& entry : pImpl->entries) {
        total += entry.key.size() * sizeof(char);
        total += sizeof(ConfigValue) + 32; // Approximate variant size
        total += entry.description.size() * sizeof(char);
        total += sizeof(std::chrono::system_clock::time_point);
    }
    
    for (const auto& schema : pImpl->schemas) {
        total += schema.key.size() * sizeof(char);
        total += sizeof(std::type_index);
        total += schema.description.size() * sizeof(char);
    }
    
    return total;
}

std::string Config::toJson() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "["; for (size_t j = 0; j < arg.size(); ++j) { if (j > 0) oss << ", "; oss << arg[j]; } oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "["; for (size_t j = 0; j < arg.size(); ++j) { if (j > 0) oss << ", "; oss << std::fixed << std::setprecision(6) << arg[j]; } oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "["; for (size_t j = 0; j < arg.size(); ++j) { if (j > 0) oss << ", "; oss << "\"" << arg[j] << "\""; } oss << "]";
            } else {
                oss << "null";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

bool Config::fromJson(const std::string& jsonStr) {
    // Clear existing entries
    clear();
    
    // Parse JSON
    return SimpleJsonParser::parse(jsonStr, pImpl->entries);
}

void Config::merge(const Config& other, bool overrideExisting) {
    for (const auto& entry : other.pImpl->entries) {
        if (!overrideExisting && has(entry.key)) {
            continue;
        }
        
        // Create new entry with different timestamp
        ConfigEntry newEntry = entry;
        newEntry.timestamp = std::chrono::system_clock::now();
        
        // Remove old entry
        remove(entry.key);
        
        // Add new entry
        pImpl->entries.push_back(newEntry);
    }
    
    // Re-optimize lookups
    optimizeLookups();
}

bool Config::exportToFile(const std::string& filepath) const {
    return saveToFile(filepath, "json");
}

bool Config::importFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try to parse as JSON first
    if (SimpleJsonParser::parse(content, pImpl->entries)) {
        optimizeLookups();
        return true;
    }
    
    // If JSON parsing failed, try simple format
    clear();
    return parseSimpleFile(filepath);
}

bool Config::createBackup(const std::string& backupPath) const {
    return exportToFile(backupPath);
}

bool Config::restoreFromBackup(const std::string& backupPath) {
    return importFromFile(backupPath);
}

bool Config::isValidKey(const std::string& key) const {
    return !key.empty() && std::all_of(key.begin(), key.end(), ::isalnum);
}

void Config::setWithDescription(const std::string& key, const ConfigValue& value, 
                              const std::string& description, ConfigSource source) {
    // Validate key
    if (!isValidKey(key)) {
        return;
    }
    
    // Handle duplicates (simple case)
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        size_t index = it->second;
        if (index < pImpl->entries.size()) {
            // Update existing entry
            pImpl->entries[index].value = value;
            pImpl->entries[index].source = source;
            pImpl->entries[index].description = description;
            pImpl->entries[index].timestamp = std::chrono::system_clock::now();
        }
    } else {
        // Add new entry
        pImpl->entries.emplace_back(key, value, source, description);
    }
    
    pImpl->lookupsOptimized = false;
}

bool Config::validateValueType(const ConfigValue& value, const std::type_index& expectedType) {
    // For now, just return true since we can't easily match std::type_index
    // In a real implementation, this would check the actual type
    return true;
}

bool Config::parseJsonFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Clear existing entries
    clear();
    
    return SimpleJsonParser::parse(content, pImpl->entries);
}

bool Config::parseYamlFile(const std::string& filepath) {
    // For now, fall back to simple parsing
    // In a full implementation, this would use a YAML parser
    return parseSimpleFile(filepath);
}

bool Config::parseSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();
    
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
            
            setWithDescription(key, ConfigValue(value), "", ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::writeJsonFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << toJson();
    return true;
}

bool Config::writeYamlFile(const std::string& filepath) const {
    // For now, fall back to simple format
    // In a full implementation, this would write proper YAML
    return writeSimpleFile(filepath);
}

bool Config::writeSimpleFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm
