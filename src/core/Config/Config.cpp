#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <iomanip>
#include <regex>
#include <cstring>  // for getenv

namespace nlm {

std::string Config::lastError = "";

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, SchemaEntry> schema;
    std::vector<ConfigChangeCallback> changeCallbacks;
    size_t modificationCount = 0;
    
    Impl() : modificationCount(0) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    clear();
    clearLastError();
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            setLastError("Cannot open configuration file: " + filepath);
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        bool success = true;
        
        while (std::getline(file, line)) {
            lineNumber++;
            line = trim(line);
            
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // FIXED: Proper validation for std::string::npos to prevent buffer overread
            size_t pos = line.find('=');
            if (pos != std::string::npos && pos < line.size() - 1) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                setLastError("Invalid configuration format at line " + 
                           std::to_string(lineNumber) + ": " + line);
                success = false;
            }
        }
        
        if (!success) {
            clear();
        }
        
        return success;
        
    } catch (const std::exception& e) {
        setLastError("Error loading configuration file " + filepath + ": " + e.what());
        clear();
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    clear();
    clearLastError();
    
    try {
        bool success = true;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos && pos < arg.size() - 1) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                }
            }
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        }
        
        return success;
        
    } catch (const std::exception& e) {
        setLastError("Error loading from command line: " + e.what());
        clear();
        return false;
    }
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    clear();
    clearLastError();
    
    try {
        bool success = true;
        
        // FIXED: Proper environment variable loading instead of filesystem iteration
        const char* envPtr = nullptr;
        for (size_t i = 0; ; ++i) {
            std::string envVar;
            if (prefix.empty()) {
                envVar = "NLM_" + std::to_string(i);
            } else {
                envVar = prefix + std::to_string(i);
            }
            
            envPtr = std::getenv(envVar.c_str());
            if (!envPtr) break;
            
            std::string key = envVarToKey(envVar);
            std::string value = envPtr;
            set(key, value, ConfigSource::Environment);
        }
        
        // Also check common environment variables without numeric suffixes
        const char* commonVars[] = {
            "NLM_RANDOM_SEED",
            "NLM_SIMULATION_TIMESTEP", 
            "NLM_NEURON_COUNT",
            "NLM_CONNECTION_PROBABILITY",
            "NLM_STDP_ENABLE",
            "NLM_HEBBIAN_ENABLE",
            "NLM_DEVELOPMENT_ENABLE"
        };
        
        for (const char* varName : commonVars) {
            envPtr = std::getenv(varName);
            if (envPtr) {
                std::string key = envVarToKey(varName);
                std::string value = envPtr;
                set(key, value, ConfigSource::Environment);
            }
        }
        
        return success;
        
    } catch (const std::exception& e) {
        setLastError("Error loading from environment: " + e.what());
        clear();
        return false;
    }
}

bool Config::loadFromJson(const std::string& jsonStr) {
    clear();
    clearLastError();
    
    try {
        json jsonData = json::parse(jsonStr);
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            auto valueOpt = jsonToConfigValue(*it);
            
            if (valueOpt.has_value()) {
                set(key, valueOpt.value(), ConfigSource::File);
            } else {
                setLastError("Failed to parse JSON value for key: " + key);
                return false;
            }
        }
        
        return true;
        
    } catch (const json::exception& e) {
        setLastError("JSON parsing error: " + std::string(e.what()));
        clear();
        return false;
    } catch (const std::exception& e) {
        setLastError("Error loading from JSON: " + e.what());
        clear();
        return false;
    } catch (...) {
        setLastError("Unknown error loading from JSON");
        clear();
        return false;
    }
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    clear();
    clearLastError();
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            setLastError("Cannot open JSON configuration file: " + filepath);
            return false;
        }
        
        json jsonData = json::parse(file);
        return loadFromJson(jsonData.dump());
        
    } catch (const json::exception& e) {
        setLastError("JSON parsing error in file " + filepath + ": " + std::string(e.what()));
        clear();
        return false;
    } catch (const std::exception& e) {
        setLastError("Error loading JSON file " + filepath + ": " + e.what());
        clear();
        return false;
    } catch (...) {
        setLastError("Unknown error loading JSON file " + filepath);
        clear();
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    clearLastError();
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            setLastError("Cannot open configuration file for writing: " + filepath);
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated on " << __TIMESTAMP__ << "\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << "# Source: " << getSourceName(entry.source) << "\n";
            if (!entry.validationKey.empty()) {
                file << "# Schema key: " << entry.validationKey << "\n";
            }
            file << entry.key << " = ";
            
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << escapeJson(arg) << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << escapeJson(arg[i]) << "\"";
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << (arg[i] ? "true" : "false");
                    }
                    file << "]";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        return true;
        
    } catch (const std::exception& e) {
        setLastError("Error saving configuration to file " + filepath + ": " + e.what());
        return false;
    } catch (...) {
        setLastError("Unknown error saving configuration to file " + filepath);
        return false;
    }
}

bool Config::saveToJson(const std::string& filepath) const {
    clearLastError();
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            setLastError("Cannot open JSON configuration file for writing: " + filepath);
            return false;
        }
        
        json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = configValueToJson(entry.value);
        }
        
        file << std::setw(2) << jsonData << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        setLastError("Error saving JSON configuration to file " + filepath + ": " + e.what());
        return false;
    } catch (...) {
        setLastError("Unknown error saving JSON configuration to file " + filepath);
        return false;
    }
}

std::string Config::toJsonString() const {
    json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = configValueToJson(entry.value);
    }
    
    return jsonData.dump(2);
}

std::string Config::toPrettyString() const {
    std::ostringstream oss;
    oss << "NLM Configuration\n";
    oss << "================\n\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Sources:\n";
    
    for (int i = 0; i < static_cast<int>(ConfigSource::Schema); ++i) {
        ConfigSource source = static_cast<ConfigSource>(i);
        size_t count = getEntryCount(source);
        if (count > 0) {
            oss << "  " << getSourceName(source) << ": " << count << " entries\n";
        }
    }
    
    oss << "\nEntries:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << (arg[i] ? "true" : "false");
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << " (" << getSourceName(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        if (entry.modified) {
            oss << " [MODIFIED]";
        }
        oss << "\n";
    }
    
    return oss.str();
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = findEntry(key);
    if (!it) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        setLastError("Type mismatch for key: " + key);
        return std::nullopt;
    } catch (...) {
        setLastError("Unknown error accessing key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value,
                 ConfigSource source) {
    auto entry = findEntry(key);
    ConfigValue oldValue;
    bool existed = false;
    
    if (entry) {
        oldValue = entry->value;
        existed = true;
    }
    
    setEntry(*entry, value, source);
    
    if (existed && oldValue != value) {
        entry->modified = true;
        pImpl->modificationCount++;
        notifyChange(key, oldValue, value);
    }
}

void Config::set(const std::string& key, const std::string& value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<bool>& value,
                 ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return findEntry(key) != nullptr;
}

void Config::remove(const std::string& key) {
    auto entry = findEntry(key);
    if (entry) {
        ConfigValue oldValue = entry->value;
        pImpl->entries.erase(findEntry(key));
        pImpl->modificationCount++;
        notifyChange(key, oldValue, ConfigValue());
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

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:\n";
    oss << "===================\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Sources:\n";
    
    for (int i = 0; i < static_cast<int>(ConfigSource::Schema); ++i) {
        ConfigSource source = static_cast<ConfigSource>(i);
        size_t count = getEntryCount(source);
        if (count > 0) {
            oss << "  " << getSourceName(source) << ": " << count << " entries\n";
        }
    }
    
    oss << "\nEntry Details:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (" << getSourceName(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        if (entry.modified) {
            oss << " [MODIFIED]";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string Config::getSourceName(ConfigSource source) const {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "Command Line";
        case ConfigSource::Runtime: return "Runtime";
        case ConfigSource::Environment: return "Environment";
        case ConfigSource::Schema: return "Schema";
        default: return "Unknown";
    }
}

void Config::clear() {
    clearLastError();
    
    for (auto& entry : pImpl->entries) {
        ConfigValue oldValue = entry.value;
        entry.value = ConfigValue();
        entry.source = ConfigSource::Default;
        entry.modified = false;
        notifyChange(entry.key, oldValue, entry.value);
    }
    
    pImpl->entries.clear();
    pImpl->modificationCount = 0;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, entry.source);
    }
}

bool Config::isModified() const {
    return pImpl->modificationCount > 0;
}

void Config::addSchemaEntry(const SchemaEntry& entry) {
    pImpl->schema[entry.key] = entry;
    updateSchemaCache();
}

bool Config::validate() const {
    ValidationResult result = validateAgainstSchema();
    setLastError(result.toString());
    return result.valid;
}

ValidationResult Config::validateAgainstSchema() const {
    return validateAgainstSchema(pImpl->entries);
}

SchemaEntry* Config::getSchemaEntry(const std::string& key) {
    auto it = pImpl->schema.find(key);
    return it != pImpl->schema.end() ? &it->second : nullptr;
}

const SchemaEntry* Config::getSchemaEntry(const std::string& key) const {
    auto it = pImpl->schema.find(key);
    return it != pImpl->schema.end() ? &it->second : nullptr;
}

void Config::addChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.push_back(callback);
}

void Config::removeChangeCallback(const ConfigChangeCallback& callback) {
    pImpl->changeCallbacks.erase(
        std::remove(pImpl->changeCallbacks.begin(), pImpl->changeCallbacks.end(), callback),
        pImpl->changeCallbacks.end()
    );
}

void Config::notifyChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    for (const auto& callback : pImpl->changeCallbacks) {
        try {
            callback(key, newValue);
        } catch (const std::exception& e) {
            setLastError("Error in config change callback: " + std::string(e.what()));
        } catch (...) {
            setLastError("Unknown error in config change callback");
        }
    }
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

std::string Config::getStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "======================\n";
    oss << "Total entries: " << getEntryCount() << "\n";
    oss << "Modified entries: " << std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.modified; }) << "\n";
    
    oss << "\nEntries by source:\n";
    for (int i = 0; i < static_cast<int>(ConfigSource::Schema); ++i) {
        ConfigSource source = static_cast<ConfigSource>(i);
        size_t count = getEntryCount(source);
        if (count > 0) {
            oss << "  " << getSourceName(source) << ": " << count << "\n";
        }
    }
    
    if (!pImpl->schema.empty()) {
        oss << "\nSchema entries: " << pImpl->schema.size() << "\n";
    }
    
    return oss.str();
}

std::string Config::getLastError() {
    return lastError;
}

void Config::clearLastError() {
    lastError.clear();
}

ConfigEntry* Config::findEntry(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end() ? &(*it) : nullptr;
}

const ConfigEntry* Config::findEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return it != pImpl->entries.end() ? &(*it) : nullptr;
}

void Config::setEntry(ConfigEntry& entry, const ConfigValue& value, ConfigSource source) {
    ConfigValue oldValue = entry.value;
    entry.value = value;
    entry.source = source;
    entry.modified = true;
    pImpl->modificationCount++;
}

void Config::updateSchemaCache() {
    for (auto& entry : pImpl->entries) {
        auto schemaIt = pImpl->schema.find(entry.key);
        if (schemaIt != pImpl->schema.end()) {
            entry.validationKey = schemaIt->second.key;
        }
    }
}

bool Config::isValidType(const std::string& key, const ConfigValue& value) const {
    const SchemaEntry* schema = getSchemaEntry(key);
    if (!schema) {
        return true; // No schema, accept any type
    }
    
    // Check type compatibility
    std::string typeName;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) typeName = "int";
        else if constexpr (std::is_same_v<T, int64_t>) typeName = "int64";
        else if constexpr (std::is_same_v<T, double>) typeName = "double";
        else if constexpr (std::is_same_v<T, bool>) typeName = "bool";
        else if constexpr (std::is_same_v<T, std::string>) typeName = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) typeName = "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) typeName = "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) typeName = "vector<string>";
        else if constexpr (std::is_same_v<T, std::vector<bool>>) typeName = "vector<bool>";
        else typeName = "unknown";
    }, value);
    
    // Check if type matches schema
    if (schema->type == "vector" && typeName.find("vector") != 0) {
        return false; // Schema expects vector but got scalar
    } else if (schema->type != "vector" && typeName.find("vector") != std::string::npos) {
        return false; // Schema expects scalar but got vector
    }
    
    // Check if specific numeric type matches
    if (schema->isNumeric() && typeName != "vector") {
        if ((schema->type == "int" && typeName != "int" && typeName != "int64" && typeName != "double") ||
            (schema->type == "int64" && typeName != "int64" && typeName != "double") ||
            (schema->type == "double" && typeName != "int" && typeName != "int64" && typeName != "double")) {
            return false;
        }
    }
    
    return true;
}

ValidationResult Config::validateAgainstSchema(const std::vector<ConfigEntry>& entries) const {
    ValidationResult result;
    
    for (const auto& entry : entries) {
        // Check if key exists in schema
        auto schemaIt = pImpl->schema.find(entry.key);
        if (schemaIt == pImpl->schema.end()) {
            result.addWarning("Entry '" + entry.key + "' has no schema definition");
            continue;
        }
        
        const SchemaEntry& schema = schemaIt->second;
        
        // Check if required
        if (schema.required && !has(entry.key)) {
            result.addError("Required schema entry missing: '" + entry.key + "'");
        }
        
        // Check type validity
        if (!isValidType(entry.key, entry.value)) {
            result.addError("Invalid type for entry '" + entry.key + "'. " +
                           "Expected type: '" + schema.type + "', " +
                           "actual type: '" + getValueTypeName(entry.value) + "'");
        }
        
        // Check range for numeric types
        if (schema.isNumeric()) {
            double value;
            if (getValueDouble(entry.value, value)) {
                if (value < schema.range.first || value > schema.range.second) {
                    result.addError("Value for entry '" + entry.key + "' (" + 
                                   std::to_string(value) + ") " +
                                   "is out of range [" + std::to_string(schema.range.first) +
                                   ", " + std::to_string(schema.range.second) + "]");
                }
            }
        }
        
        // Check allowed values
        if (!schema.allowedValues.empty()) {
            std::string valueStr;
            if (getValueString(entry.value, valueStr)) {
                if (std::find(schema.allowedValues.begin(), schema.allowedValues.end(), valueStr) ==
                    schema.allowedValues.end()) {
                    result.addError("Invalid value for entry '" + entry.key + "': '" +
                                   valueStr + "'. " +
                                   "Allowed values: " + formatAllowedValues(schema.allowedValues));
                }
            }
        }
    }
    
    return result;
}

std::string Config::getValueTypeName(const ConfigValue& value) const {
    std::string typeName;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) typeName = "int";
        else if constexpr (std::is_same_v<T, int64_t>) typeName = "int64";
        else if constexpr (std::is_same_v<T, double>) typeName = "double";
        else if constexpr (std::is_same_v<T, bool>) typeName = "bool";
        else if constexpr (std::is_same_v<T, std::string>) typeName = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) typeName = "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) typeName = "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) typeName = "vector<string>";
        else if constexpr (std::is_same_v<T, std::vector<bool>>) typeName = "vector<bool>";
        else typeName = "unknown";
    }, value);
    return typeName;
}

bool Config::getValueDouble(const ConfigValue& value, double& result) const {
    try {
        if (std::holds_alternative<int>(value)) {
            result = static_cast<double>(std::get<int>(value));
            return true;
        } else if (std::holds_alternative<int64_t>(value)) {
            result = static_cast<double>(std::get<int64_t>(value));
            return true;
        } else if (std::holds_alternative<double>(value)) {
            result = std::get<double>(value);
            return true;
        }
    } catch (const std::bad_variant_access&) {}
    return false;
}

bool Config::getValueString(const ConfigValue& value, std::string& result) const {
    try {
        if (std::holds_alternative<std::string>(value)) {
            result = std::get<std::string>(value);
            return true;
        }
    } catch (const std::bad_variant_access&) {}
    return false;
}

std::string Config::formatAllowedValues(const std::vector<std::string>& values) const {
    std::ostringstream oss;
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << "\"" << values[i] << "\"";
    }
    return oss.str();
}

json Config::configValueToJson(const ConfigValue& value) {
    json result;
    
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            result = arg;
        }
    }, value);
    
    return result;
}

std::optional<ConfigValue> Config::jsonToConfigValue(const json& j) {
    std::optional<ConfigValue> result;
    
    if (j.is_number_integer()) {
        result = j.get<int>();
    } else if (j.is_number_float()) {
        result = j.get<double>();
    } else if (j.is_boolean()) {
        result = j.get<bool>();
    } else if (j.is_string()) {
        result = j.get<std::string>();
    } else if (j.is_array()) {
        if (!j.empty()) {
            auto firstType = j[0].type();
            if (firstType == json::value_t::number_integer) {
                std::vector<int> vec = j.get<std::vector<int>>();
                result = vec;
            } else if (firstType == json::value_t::number_float) {
                std::vector<double> vec = j.get<std::vector<double>>();
                result = vec;
            } else if (firstType == json::value_t::boolean) {
                std::vector<bool> vec = j.get<std::vector<bool>>();
                result = vec;
            } else if (firstType == json::value_t::string) {
                std::vector<std::string> vec = j.get<std::vector<std::string>>();
                result = vec;
            }
        }
    }
    
    return result;
}

std::optional<ConfigValue> Config::jsonToConfigValue(const json& j, const std::string& expectedType) {
    auto valueOpt = jsonToConfigValue(j);
    if (!valueOpt.has_value()) {
        return std::nullopt;
    }
    
    ConfigValue value = valueOpt.value();
    
    // Check if type matches expected
    std::string actualType = getValueTypeName(value);
    if (expectedType == "int" && actualType != "int" && actualType != "int64" && actualType != "double") {
        return std::nullopt;
    } else if (expectedType == "double" && actualType != "int" && actualType != "int64" && actualType != "double") {
        return std::nullopt;
    } else if (expectedType == "bool" && actualType != "bool") {
        return std::nullopt;
    } else if (expectedType == "string" && actualType != "string") {
        return std::nullopt;
    } else if (expectedType == "vector" && actualType.find("vector") == std::string::npos) {
        return std::nullopt;
    }
    
    return value;
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

std::string Config::escapeJson(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '"': oss << "\\\""; break;
            case '/': oss << "\\/"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string Config::unescapeJson(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '"': result += '"'; i++; break;
                case '\\': result += '\\'; i++; break;
                case '/': result += '/'; i++; break;
                case 'b': result += '\b'; i++; break;
                case 'f': result += '\f'; i++; break;
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                case 't': result += '\t'; i++; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    
    return result;
}

std::string Config::envVarToKey(const std::string& envVar) {
    // Convert environment variable name to configuration key
    std::string key = envVar;
    // Remove prefix
    if (key.rfind("NLM_", 0) == 0) {
        key = key.substr(4);
    } else if (key.rfind("NLM", 0) == 0 && key.size() > 3) {
        // Remove NLM prefix if it's at the start
        key = key.substr(3);
    }
    
    // Convert to lowercase and replace underscores with dots
    key = toLower(key);
    std::replace(key.begin(), key.end(), '_', '.');
    
    return key;
}

// Explicit template instantiations
namespace nlm {
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
template std::optional<std::vector<bool>> Config::get<std::vector<bool>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
} // namespace nlm