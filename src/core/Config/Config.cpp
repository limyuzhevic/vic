#pragma once

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <random>
#include <chrono>
#include <ctime>
#include <map>
#include <unordered_map>
#include <numeric>

namespace nlm {

// Validation result implementation
std::string ValidationResult::getErrorSummary() const {
    if (errors.empty()) return "Configuration is valid";
    std::ostringstream oss;
    oss << "Configuration has " << errors.size() << " error(s):\n";
    for (const auto& error : errors) {
        oss << "  - " << error << "\n";
    }
    return oss.str();
}

// Schema element validation implementation
bool SchemaElement::validate(const ConfigValue& value) const {
    std::string error = validateAndGetError(value);
    return error.empty();
}

std::string SchemaElement::validateAndGetError(const ConfigValue& value) const {
    // Type validation
    std::string typeName;
    if (std::holds_alternative<int>(value)) typeName = "int";
    else if (std::holds_alternative<int64_t>(value)) typeName = "int64_t";
    else if (std::holds_alternative<double>(value)) typeName = "double";
    else if (std::holds_alternative<bool>(value)) typeName = "bool";
    else if (std::holds_alternative<std::string>(value)) typeName = "string";
    else if (std::holds_alternative<std::vector<int>>(value)) typeName = "vector<int>";
    else if (std::holds_alternative<std::vector<double>>(value)) typeName = "vector<double>";
    else if (std::holds_alternative<std::vector<std::string>>(value)) typeName = "vector<string>";
    else typeName = "unknown";
    
    // Check type match
    if (typeName != type) {
        return "Type mismatch: expected '" + type + "', got '" + typeName + "'";
    }
    
    // Range validation for numeric types
    if (isNumericType()) {
        if (type == "int" || type == "int64_t") {
            int64_t val = std::holds_alternative<int>(value) ? std::get<int>(value) : std::get<int64_t>(value);
            if (minInt.has_value() && val < minInt.value()) {
                return "Value " + std::to_string(val) + " is less than minimum " + std::to_string(minInt.value());
            }
            if (maxInt.has_value() && val > maxInt.value()) {
                return "Value " + std::to_string(val) + " exceeds maximum " + std::to_string(maxInt.value());
            }
        } else if (type == "double") {
            double val = std::get<double>(value);
            if (minDouble.has_value() && val < minDouble.value()) {
                return "Value " + std::to_string(val) + " is less than minimum " + std::to_string(minDouble.value());
            }
            if (maxDouble.has_value() && val > maxDouble.value()) {
                return "Value " + std::to_string(val) + " exceeds maximum " + std::to_string(maxDouble.value());
            }
        }
    }
    
    // Enum validation
    if (isEnumType() && enumValues.has_value()) {
        std::string valStr;
        if (type == "int") valStr = std::to_string(std::get<int>(value));
        else if (type == "int64_t") valStr = std::to_string(std::get<int64_t>(value));
        else if (type == "double") valStr = std::to_string(std::get<double>(value));
        else if (type == "bool") valStr = std::get<bool>(value) ? "true" : "false";
        else if (type == "string") valStr = std::get<std::string>(value);
        
        bool found = false;
        for (const auto& enumVal : enumValues.value()) {
            if (enumVal == valStr) {
                found = true;
                break;
            }
        }
        if (!found) {
            return "Value '" + valStr + "' is not in allowed enum values";
        }
    }
    
    return "";
}

bool SchemaElement::isNumericType() const {
    return type == "int" || type == "int64_t" || type == "double";
}

bool SchemaElement::isEnumType() const {
    return enumValues.has_value() && !enumValues.value().empty();
}

bool SchemaElement::isVectorType() const {
    return type == "vector<int>" || type == "vector<double>" || type == "vector<string>";
}

// Helper functions
static std::string configValueToString(const ConfigValue& value) {
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
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        const auto& vec = std::get<std::vector<int>>(value);
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ",";
            oss << vec[i];
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& vec = std::get<std::vector<double>>(value);
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ",";
            oss << vec[i];
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        oss << "[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << ",";
            oss << "\"" << vec[i] << "\"";
        }
        oss << "]";
    }
    return oss.str();
}

static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

static std::string currentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timestamp), "%Y%m%d_%H%M%S");
    return ss.str();
}

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string version;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point modified;
    size_t backupCount = 0;
    std::string backupDir;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->created = std::chrono::system_clock::now();
    pImpl->modified = pImpl->created;
    pImpl->version = "1.0.0";
    pImpl->backupDir = "/tmp/nlm_backups"; // Default backup directory
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Core configuration methods
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear(); // Clear existing config before loading
    
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
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

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

void Config::loadFromDict(const std::map<std::string, ConfigValue>& config) {
    clear();
    for (const auto& pair : config) {
        set(pair.first, pair.second, ConfigSource::Runtime);
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::filesystem::create_directories(std::filesystem::path(filepath).parent_path());
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << configValueToString(entry.value) << "\n";
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
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
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    pImpl->modified = std::chrono::system_clock::now();
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
    pImpl->modified = std::chrono::system_clock::now();
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
    pImpl->modified = std::chrono::system_clock::now();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, version: " << pImpl->version << "):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        oss << configValueToString(entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

std::string Config::getDetailedSummary() const {
    std::ostringstream oss;
    auto now = std::chrono::system_clock::now();
    auto createdDuration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->created);
    auto modifiedDuration = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->modified);
    
    time_t createdTime = std::chrono::system_clock::to_time_t(pImpl->created);
    time_t modifiedTime = std::chrono::system_clock::to_time_t(pImpl->modified);
    
    oss << "=== Configuration Details ===\n";
    oss << "Version: " << pImpl->version << "\n";
    oss << "Created: " << std::asctime(std::localtime(&createdTime))
        << "(" << createdDuration.count() << "s ago)\n";
    oss << "Modified: " << std::asctime(std::localtime(&modifiedTime))
        << "(" << modifiedDuration.count() << "s ago)\n";
    oss << "Backup count: " << pImpl->backupCount << "\n";
    oss << "Backup directory: " << pImpl->backupDir << "\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "\n=== Entries ===\n";
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << "\n";
        oss << "  Value: " << configValueToString(entry.value) << "\n";
        oss << "  Source: " << static_cast<int>(entry.source) << "\n";
        oss << "  Description: " << entry.description << "\n";
    }
    return oss.str();
}

std::map<std::string, std::string> Config::exportAsDict() const {
    std::map<std::string, std::string> result;
    for (const auto& entry : pImpl->entries) {
        result[entry.key] = configValueToString(entry.value);
    }
    return result;
}

void Config::importFromDict(const std::map<std::string, std::string>& config) {
    clear();
    for (const auto& pair : config) {
        ConfigValue value;
        if (deserializeValue(pair.second, value)) {
            set(pair.first, value, ConfigSource::Runtime);
        } else {
            set(pair.first, pair.second, ConfigSource::Runtime);
        }
    }
}

void Config::merge(const Config& other, ConfigSource priority) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, priority);
    }
}

std::map<std::string, ConfigValue> Config::filterByPrefix(const std::string& prefix) const {
    std::map<std::string, ConfigValue> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(prefix) == 0) {
            result[entry.key] = entry.value;
        }
    }
    return result;
}

std::vector<std::string> Config::getKeysWithPrefix(const std::string& prefix) const {
    std::vector<std::string> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find(prefix) == 0) {
            result.push_back(entry.key);
        }
    }
    return result;
}

ValidationResult Config::validateSchema(const std::map<std::string, SchemaElement>& schema) const {
    ValidationResult result;
    
    // Check required fields
    for (const auto& pair : schema) {
        if (pair.second.required && !has(pair.first)) {
            result.addError("Required field '" + pair.first + "' is missing");
        }
    }
    
    // Validate all entries
    for (const auto& entry : pImpl->entries) {
        auto it = schema.find(entry.key);
        if (it != schema.end()) {
            ValidationResult entryResult = validateKey(entry.key, it->second);
            if (entryResult.hasErrors()) {
                for (const auto& error : entryResult.errors) {
                    result.addError("Key '" + entry.key + "': " + error);
                }
            }
            if (entryResult.hasWarnings()) {
                for (const auto& warning : entryResult.warnings) {
                    result.addWarning("Key '" + entry.key + "': " + warning);
                }
            }
        }
    }
    
    return result;
}

ValidationResult Config::validateKey(const std::string& key, const SchemaElement& schema) const {
    ValidationResult result;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        if (schema.required) {
            result.addError("Required field is missing");
        }
        return result;
    }
    
    std::string error = schema.validateAndGetError(it->value);
    if (!error.empty()) {
        result.addError(error);
    }
    
    return result;
}

// Static helper methods
bool Config::matchesSchemaType(const std::string& type, const ConfigValue& value) {
    std::string typeName;
    if (std::holds_alternative<int>(value)) typeName = "int";
    else if (std::holds_alternative<int64_t>(value)) typeName = "int64_t";
    else if (std::holds_alternative<double>(value)) typeName = "double";
    else if (std::holds_alternative<bool>(value)) typeName = "bool";
    else if (std::holds_alternative<std::string>(value)) typeName = "string";
    else if (std::holds_alternative<std::vector<int>>(value)) typeName = "vector<int>";
    else if (std::holds_alternative<std::vector<double>>(value)) typeName = "vector<double>";
    else if (std::holds_alternative<std::vector<std::string>>(value)) typeName = "vector<string>";
    else typeName = "unknown";
    
    return typeName == type;
}

std::map<std::string, SchemaElement> Config::getBuiltInSchema(const std::string& presetName) {
    std::map<std::string, SchemaElement> schema;
    
    if (presetName == "debug") {
        schema["debug.enabled"] = SchemaElement{"bool", {}, {}, {}, {}, {}, true, "Enable debug mode", "Debug flag"};
        schema["debug.level"] = SchemaElement{"int", 0, 3, {}, {}, {}, false, "1", "Debug verbosity level"};
        schema["debug.log_file"] = SchemaElement{"string", {}, {}, {}, {}, {}, false, "", "Debug log file path"};
        
    } else if (presetName == "production") {
        schema["performance.threads"] = SchemaElement{"int", 1, 32, {}, {}, {}, true, "4", "Number of worker threads"};
        schema["performance.timeout"] = SchemaElement{"double", 0.0, 3600.0, {}, {}, {}, true, "30.0", "Timeout in seconds"};
        schema["performance.cache_size"] = SchemaElement{"int", 0, 1000000, {}, {}, {}, true, "1000", "Cache size in MB"};
        schema["logging.level"] = SchemaElement{"string", {}, {}, {}, {}, {}, true, "INFO", "Log level"};
        schema["logging.level"].enumValues = {"DEBUG", "INFO", "WARNING", "ERROR"};
        
    } else { // basic
        schema["brain.neuron_count"] = SchemaElement{"int", 1, 100000, {}, {}, {}, true, "1000", "Number of neurons"};
        schema["brain.synapse_density"] = SchemaElement{"double", 0.0, 1.0, {}, {}, {}, true, "0.1", "Synapse density (0.0-1.0)"};
        schema["plasticity.stdp.enable"] = SchemaElement{"bool", {}, {}, {}, {}, {}, true, "true", "Enable STDP plasticity"};
        schema["plasticity.hebbian.enable"] = SchemaElement{"bool", {}, {}, {}, {}, {}, true, "true", "Enable Hebbian plasticity"};
        schema["neuromod.curiosity.enable"] = SchemaElement{"bool", {}, {}, {}, {}, {}, true, "true", "Enable curiosity modulation"};
        schema["neuromod.novelty.enable"] = SchemaElement{"bool", {}, {}, {}, {}, {}, true, "true", "Enable novelty detection"};
    }
    
    return schema;
}

bool Config::applyPreset(const Preset& preset) {
    bool success = true;
    for (const auto& pair : preset.values) {
        set(pair.first, pair.second, preset.source);
    }
    return success;
}

std::map<std::string, Preset> Config::getBuiltInPresets() {
    std::map<std::string, Preset> presets;
    
    // Experimental preset
    Preset experimental("experimental", "Experimental configuration with advanced features enabled");
    experimental.add("brain.neuron_count", 5000);
    experimental.add("brain.synapse_density", 0.15);
    experimental.add("plasticity.stdp.enable", true);
    experimental.add("plasticity.hebbian.enable", true);
    experimental.add("plasticity.structural.enable", true);
    experimental.add("neuromod.curiosity.enable", true);
    experimental.add("neuromod.novelty.enable", true);
    experimental.add("neuromod.dopamine.scale", 2.0);
    experimental.add("debug.enabled", true);
    experimental.add("debug.level", 2);
    experimental.add("performance.threads", 8);
    
    // Production preset
    Preset production("production", "Production-ready configuration with stability optimizations");
    production.add("brain.neuron_count", 10000);
    production.add("brain.synapse_density", 0.12);
    production.add("plasticity.stdp.enable", true);
    production.add("plasticity.hebbian.enable", true);
    production.add("neuromod.curiosity.enable", true);
    production.add("neuromod.novelty.enable", true);
    production.add("performance.threads", 16);
    production.add("performance.timeout", 300.0);
    production.add("performance.cache_size", 2048);
    production.add("logging.level", std::string("INFO"));
    production.add("debug.enabled", false);
    
    // Debug preset
    Preset debug("debug", "Debug configuration with detailed logging and lower performance");
    debug.add("brain.neuron_count", 100);
    debug.add("brain.synapse_density", 0.05);
    debug.add("plasticity.stdp.enable", true);
    debug.add("plasticity.hebbian.enable", true);
    debug.add("neuromod.curiosity.enable", false);
    debug.add("neuromod.novelty.enable", false);
    debug.add("debug.enabled", true);
    debug.add("debug.level", 3);
    debug.add("debug.log_file", std::string("debug.log"));
    
    presets["experimental"] = experimental;
    presets["production"] = production;
    presets["debug"] = debug;
    
    return presets;
}

Preset Config::createPreset(const std::string& name, const std::string& description, 
                            const std::map<std::string, ConfigValue>& values) {
    Preset preset(name, description);
    for (const auto& pair : values) {
        preset.add(pair.first, pair.second);
    }
    return preset;
}

std::vector<ConfigDiff> Config::compare(const Config& other) const {
    std::vector<ConfigDiff> diff;
    
    // Check entries in this config
    for (const auto& entry : pImpl->entries) {
        auto it = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it == other.pImpl->entries.end()) {
            // Entry exists in this but not in other
            diff.emplace_back(entry.key, std::make_optional(entry.value), std::nullopt, entry.source);
        } else if (it->value != entry.value) {
            // Entry differs
            diff.emplace_back(entry.key, std::make_optional(it->value), std::make_optional(entry.value), entry.source);
        }
    }
    
    // Check entries in other config
    for (const auto& entry : other.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it == pImpl->entries.end()) {
            // Entry exists in other but not in this
            diff.emplace_back(entry.key, std::nullopt, std::make_optional(entry.value), entry.source);
        }
    }
    
    return diff;
}

size_t Config::getDiffCount(const Config& other) const {
    return compare(other).size();
}

bool Config::equals(const Config& other) const {
    return pImpl->entries.size() == other.pImpl->entries.size() &&
           std::equal(pImpl->entries.begin(), pImpl->entries.end(), other.pImpl->entries.begin(),
               [](const ConfigEntry& a, const ConfigEntry& b) {
                   return a.key == b.key && a.value == b.value && a.source == b.source;
               });
}

std::string Config::getDiffSummary(const Config& other) const {
    std::vector<ConfigDiff> diff = compare(other);
    
    std::ostringstream oss;
    oss << "Configuration Comparison Summary:\n";
    oss << "  Total differences: " << diff.size() << "\n";
    
    size_t additions = 0, modifications = 0, removals = 0;
    for (const auto& d : diff) {
        if (!d.oldValue.has_value() && d.newValue.has_value()) additions++;
        else if (d.oldValue.has_value() && d.newValue.has_value()) modifications++;
        else if (d.oldValue.has_value() && !d.newValue.has_value()) removals++;
    }
    
    oss << "  Additions: " << additions << "\n";
    oss << "  Modifications: " << modifications << "\n";
    oss << "  Removals: " << removals << "\n";
    
    if (!diff.empty()) {
        oss << "\nDetailed differences:\n";
        for (const auto& d : diff) {
            oss << "  " << d.key;
            if (d.oldValue.has_value() && d.newValue.has_value()) {
                oss << ": [" << configValueToString(d.oldValue.value()) << "] -> [" << configValueToString(d.newValue.value()) << "]";
            } else if (!d.oldValue.has_value()) {
                oss << ": <- [" << configValueToString(d.newValue.value()) << "]";
            } else {
                oss << ": -> [" << configValueToString(d.oldValue.value()) << "]";
            }
            oss << " (" << static_cast<int>(d.source) << ")\n";
        }
    }
    
    return oss.str();
}

void Config::applyDiff(const std::vector<ConfigDiff>& diff) {
    for (const auto& d : diff) {
        if (d.newValue.has_value()) {
            set(d.key, d.newValue.value(), ConfigSource::Runtime);
        } else {
            remove(d.key);
        }
    }
}

void Config::optimize(const std::map<std::string, SchemaElement>& constraints) {
    std::map<std::string, ConfigValue> defaults;
    
    // Collect default values from schema
    for (const auto& pair : constraints) {
        if (pair.second.defaultValue.empty()) continue;
        ConfigValue defaultValue = fromString(pair.second.defaultValue, pair.second.type);
        defaults[pair.first] = defaultValue;
    }
    
    removeUnusedDefaults(defaults);
}

void Config::removeUnusedDefaults(const std::map<std::string, ConfigValue>& defaults) {
    for (const auto& pair : defaults) {
        if (!has(pair.first)) {
            remove(pair.first);
        }
    }
}

void Config::setVersion(const std::string& version) {
    pImpl->version = version;
    pImpl->modified = std::chrono::system_clock::now();
}

std::string Config::getVersion() const {
    return pImpl->version;
}

bool Config::createBackup(const std::string& filepath) const {
    try {
        std::filesystem::create_directories(std::filesystem::path(filepath).parent_path());
        
        // Create backup filename with timestamp
        std::string backupPath = filepath;
        if (!backupPath.empty() && backupPath.back() != '/') {
            backupPath += "/";
        }
        backupPath += "config_backup_" + currentTimeString() + ".txt";
        
        bool success = saveToFile(backupPath);
        if (success) {
            const_cast<Impl*>(pImpl.get())->backupCount++;
        }
        return success;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::restoreFromBackup(const std::string& filepath) {
    clear();
    return loadFromFile(filepath);
}

std::map<std::string, size_t> Config::getStatistics() const {
    std::map<std::string, size_t> stats;
    
    stats["total_entries"] = pImpl->entries.size();
    
    // Count by source
    std::map<ConfigSource, size_t> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    for (const auto& pair : sourceCount) {
        stats["by_source_" + std::to_string(static_cast<int>(pair.first))] = pair.second;
    }
    
    // Count by type
    std::map<std::string, size_t> typeCount;
    for (const auto& entry : pImpl->entries) {
        std::string typeName;
        if (std::holds_alternative<int>(entry.value)) typeName = "int";
        else if (std::holds_alternative<int64_t>(entry.value)) typeName = "int64_t";
        else if (std::holds_alternative<double>(entry.value)) typeName = "double";
        else if (std::holds_alternative<bool>(entry.value)) typeName = "bool";
        else if (std::holds_alternative<std::string>(entry.value)) typeName = "string";
        else if (std::holds_alternative<std::vector<int>>(entry.value)) typeName = "vector<int>";
        else if (std::holds_alternative<std::vector<double>>(entry.value)) typeName = "vector<double>";
        else if (std::holds_alternative<std::vector<std::string>>(entry.value)) typeName = "vector<string>";
        
        typeCount[typeName]++;
    }
    
    for (const auto& pair : typeCount) {
        stats["by_type_" + pair.first] = pair.second;
    }
    
    return stats;
}

std::map<ConfigSource, std::vector<std::string>> Config::getKeysBySource() const {
    std::map<ConfigSource, std::vector<std::string>> result;
    
    for (const auto& entry : pImpl->entries) {
        result[entry.source].push_back(entry.key);
    }
    
    return result;
}

double Config::getEntropy() const {
    if (pImpl->entries.empty()) return 0.0;
    
    // Simple entropy calculation based on source diversity
    std::map<ConfigSource, size_t> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    double total = static_cast<double>(pImpl->entries.size());
    double entropy = 0.0;
    for (const auto& pair : sourceCounts) {
        double p = static_cast<double>(pair.second) / total;
        if (p > 0) {
            entropy -= p * std::log2(p);
        }
    }
    
    return entropy;
}

ValidationResult Config::validateAll(const std::map<std::string, SchemaElement>& schema) const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        auto it = schema.find(entry.key);
        if (it != schema.end()) {
            ValidationResult entryResult = validateKey(entry.key, it->second);
            if (entryResult.hasErrors()) {
                for (const auto& error : entryResult.errors) {
                    result.addError("Key '" + entry.key + "': " + error);
                }
            }
            if (entryResult.hasWarnings()) {
                for (const auto& warning : entryResult.warnings) {
                    result.addWarning("Key '" + entry.key + "': " + warning);
                }
            }
        }
    }
    
    return result;
}

void Config::batchSet(const std::vector<std::pair<std::string, ConfigValue>>& values, ConfigSource source) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, source);
    }
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

// ConfigValue serialization helpers
std::string Config::serializeValue(const ConfigValue& value) {
    return configValueToString(value);
}

bool Config::deserializeValue(const std::string& str, ConfigValue& value) {
    if (str.empty()) return false;
    
    // Try int
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            value = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try int64_t
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) {
            value = int64Val;
            return true;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            value = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // Try bool
    if (str == "true" || str == "false" || str == "1" || str == "0") {
        value = (str == "true" || str == "1");
        return true;
    }
    
    // Try vector types
    if (str.find(',') != std::string::npos) {
        try {
            // Try vector<int>
            std::vector<int> intVec;
            size_t pos = 0;
            while (pos < str.size()) {
                size_t end = str.find(',', pos);
                if (end == std::string::npos) end = str.size();
                intVec.push_back(std::stoi(str.substr(pos, end - pos)));
                pos = end + 1;
            }
            value = intVec;
            return true;
        } catch (...) {}
        
        try {
            // Try vector<double>
            std::vector<double> doubleVec;
            size_t pos = 0;
            while (pos < str.size()) {
                size_t end = str.find(',', pos);
                if (end == std::string::npos) end = str.size();
                doubleVec.push_back(std::stod(str.substr(pos, end - pos)));
                pos = end + 1;
            }
            value = doubleVec;
            return true;
        } catch (...) {}
        
        try {
            // Try vector<string>
            std::vector<std::string> stringVec;
            size_t pos = 0;
            while (pos < str.size()) {
                size_t end = str.find(',', pos);
                if (end == std::string::npos) end = str.size();
                std::string item = str.substr(pos, end - pos);
                if (item.size() >= 2 && 
                    ((item.front() == '"' && item.back() == '"') ||
                     (item.front() == '\'' && item.back() == '\''))) {
                    item = item.substr(1, item.size() - 2);
                }
                stringVec.push_back(item);
                pos = end + 1;
            }
            value = stringVec;
            return true;
        } catch (...) {}
    }
    
    // Assume string
    value = str;
    return true;
}

} // namespace nlm
