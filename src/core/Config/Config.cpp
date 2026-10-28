#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchemaEntry> schema;
    size_t loadCount;
    size_t saveCount;
    time_t lastModified;
    
    Impl() : loadCount(0), saveCount(0), lastModified(0) {
        loadDefaults();
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with defaults
    loadDefaults();
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {
    schema_ = std::move(other.schema_);
    loadCount_ = other.loadCount_;
    saveCount_ = other.saveCount_;
    lastModified_ = other.lastModified_;
}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        schema_ = std::move(other.schema_);
        loadCount_ = other.loadCount_;
        saveCount_ = other.saveCount_;
        lastModified_ = other.lastModified_;
    }
    return *this;
}

void Config::loadDefaults() {
    // Load default configuration values
    setString("brain.neuron_count", "1000", ConfigSource::Default);
    setDouble("brain.synapse_density", 0.1, ConfigSource::Default);
    setDouble("brain.connection_probability", 0.05, ConfigSource::Default);
    setDouble("brain.v_thresh", -50.0, ConfigSource::Default);
    setDouble("brain.v_rest", -70.0, ConfigSource::Default);
    setDouble("brain.v_reset", -75.0, ConfigSource::Default);
    setDouble("brain.tau_mem", 20.0, ConfigSource::Default);
    setDouble("brain.tau_ref", 2.0, ConfigSource::Default);
    
    setBool("plasticity.stdp.enable", true, ConfigSource::Default);
    setDouble("plasticity.stdp.learning_rate", 0.001, ConfigSource::Default);
    setDouble("plasticity.stdp.tau_plus", 20.0, ConfigSource::Default);
    setDouble("plasticity.stdp.tau_minus", 20.0, ConfigSource::Default);
    
    setBool("plasticity.hebbian.enable", true, ConfigSource::Default);
    setBool("plasticity.structural.enable", true, ConfigSource::Default);
    
    setBool("neuromod.dopamine.enable", true, ConfigSource::Default);
    setDouble("neuromod.dopamine.scale", 1.0, ConfigSource::Default);
    
    setBool("neuromod.curiosity.enable", true, ConfigSource::Default);
    setBool("neuromod.novelty.enable", true, ConfigSource::Default);
    
    setDouble("simulation.timestep", 0.001, ConfigSource::Default);
    setInt64("random_seed", 42, ConfigSource::Default);
    
    setInt("brain.region_count", 1, ConfigSource::Default);
}

bool Config::loadFromJSON(const std::string& filepath, bool merge) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ConfigIOError("Cannot open configuration file: " + filepath);
        }
        
        nlohmann::json j;
        file >> j;
        
        return loadFromJSONInternal(j, merge);
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath, bool merge) {
    try {
        // Implementation depends on YAML library availability
        // For now, fall back to JSON
        return loadFromJSON(filepath, merge);
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv, const std::string& prefix) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Try to parse value as different types
                if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    setBool(key, boolValue, ConfigSource::CommandLine);
                } else {
                    // Try to parse as number
                    try {
                        size_t posEnd;
                        int intValue = std::stoi(value, &posEnd);
                        if (posEnd == value.size()) {
                            setInt(key, intValue, ConfigSource::CommandLine);
                        } else {
                            double doubleValue = std::stod(value, &posEnd);
                            if (posEnd == value.size()) {
                                setDouble(key, doubleValue, ConfigSource::CommandLine);
                            } else {
                                setString(key, value, ConfigSource::CommandLine);
                            }
                        }
                    } catch (...) {
                        setString(key, value, ConfigSource::CommandLine);
                    }
                }
            }
        }
        // Handle --key value format
        else if (arg.substr(0, 2) == "--" && i + 1 < argc) {
            std::string key = arg.substr(2);
            std::string value = argv[++i];
            
            // Try to parse value as different types (same logic as above)
            if (value == "true" || value == "false") {
                bool boolValue = (value == "true");
                setBool(key, boolValue, ConfigSource::CommandLine);
            } else {
                try {
                    size_t posEnd;
                    int intValue = std::stoi(value, &posEnd);
                    if (posEnd == value.size()) {
                        setInt(key, intValue, ConfigSource::CommandLine);
                    } else {
                        double doubleValue = std::stod(value, &posEnd);
                        if (posEnd == value.size()) {
                            setDouble(key, doubleValue, ConfigSource::CommandLine);
                        } else {
                            setString(key, value, ConfigSource::CommandLine);
                        }
                    }
                } catch (...) {
                    setString(key, value, ConfigSource::CommandLine);
                }
            }
        }
    }
    
    return true;
}

bool Config::saveToJSON(const std::string& filepath, bool pretty) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigIOError("Cannot open file for writing: " + filepath);
        }
        
        nlohmann::json j;
        
        for (const auto& entry : pImpl->entries) {
            std::string key = entry.key;
            
            // Handle nested keys (e.g., "brain.neuron_count")
            size_t dotPos = key.find('.');
            if (dotPos != std::string::npos) {
                std::string parentKey = key.substr(0, dotPos);
                std::string childKey = key.substr(dotPos + 1);
                
                if (!j.contains(parentKey)) {
                    j[parentKey] = nlohmann::json::object();
                }
                
                // Convert value based on type
                j[parentKey][childKey] = valueToJSON(entry.value);
            } else {
                j[key] = valueToJSON(entry.value);
            }
        }
        
        if (pretty) {
            file << std::setw(2) << j;
        } else {
            file << j;
        }
        
        pImpl->saveCount++;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToYAML(const std::string& filepath, bool pretty) const {
    // For now, save as JSON (YAML is a superset of JSON)
    return saveToJSON(filepath, pretty);
}

nlohmann::json Config::valueToJSON(const ConfigValue& value) const {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg;
        } else {
            return "";
        }
    }, value);
}

ConfigValidationResult Config::validate(const std::vector<ConfigSchemaEntry>& schema) const {
    ConfigValidationResult result;
    
    // Update schema
    pImpl->schema = schema;
    
    // Validate each entry against schema
    for (const auto& entry : pImpl->entries) {
        // Find corresponding schema entry
        auto schemaIt = std::find_if(schema.begin(), schema.end(),
            [&entry](const ConfigSchemaEntry& se) { return se.key == entry.key; });
        
        if (schemaIt != schema.end()) {
            ConfigValidationResult entryResult = validateEntry(entry, *schemaIt);
            if (!entryResult.valid) {
                result.valid = false;
                result.errors.insert(result.errors.end(), entryResult.errors.begin(), entryResult.errors.end());
            }
            result.warnings.insert(result.warnings.end(), entryResult.warnings.begin(), entryResult.warnings.end());
        }
    }
    
    // Check for required entries that are missing
    for (const auto& schemaEntry : schema) {
        if (schemaEntry.required && !has(schemaEntry.key)) {
            result.valid = false;
            result.addError("Required configuration key missing: " + schemaEntry.key);
        }
    }
    
    return result;
}

ConfigValidationResult Config::validateEntry(const ConfigEntry& entry, const ConfigSchemaEntry& schema) const {
    ConfigValidationResult result;
    
    // Check type
    std::string expectedType = schema.type;
    std::string actualType = ConfigUtils::getTypeNameFromValue(entry.value);
    
    if (expectedType != "auto" && expectedType != actualType) {
        result.addError("Type mismatch for key '" + entry.key + "': expected " + expectedType + ", got " + actualType);
    }
    
    // Check value ranges for numeric types
    if (expectedType == "int" || expectedType == "double") {
        double value = std::get<double>(entry.value);
        if (value < schema.minValue) {
            result.addError("Value for key '" + entry.key + "' is below minimum allowed: " + std::to_string(value) + " < " + std::to_string(schema.minValue));
        }
        if (value > schema.maxValue) {
            result.addError("Value for key '" + entry.key + "' exceeds maximum allowed: " + std::to_string(value) + " > " + std::to_string(schema.maxValue));
        }
    }
    
    // Check regex pattern for strings
    if (expectedType == "string" && !schema.validationRegex.empty()) {
        std::string value = std::get<std::string>(entry.value);
        // Simple regex check (could use more sophisticated regex library)
        if (!std::regex_match(value, std::regex(schema.validationRegex))) {
            result.addWarning("Value for key '" + entry.key + "' does not match expected pattern: " + value);
        }
    }
    
    return result;
}

std::string Config::getStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "  Total entries: " << pImpl->entries.size() << "\n";
    oss << "  Schema entries: " << pImpl->schema.size() << "\n";
    oss << "  Load operations: " << pImpl->loadCount << "\n";
    oss << "  Save operations: " << pImpl->saveCount << "\n";
    
    if (pImpl->lastModified > 0) {
        oss << "  Last modified: " << std::ctime(&pImpl->lastModified);
    }
    
    return oss.str();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Advanced Configuration Summary (Version " << getVersion() << ")\n";
    oss << "=" << std::string(50, '=') << "\n";
    oss << "Loaded from: " << (pImpl->lastModified > 0 ? std::ctime(&pImpl->lastModified) : "Not loaded") << "\n";
    oss << "Total parameters: " << pImpl->entries.size() << "\n";
    oss << "Loaded successfully: " << (pImpl->loadCount) << " times\n";
    oss << "Saved successfully: " << (pImpl->saveCount) << " times\n\n";
    
    // Show source breakdown
    std::unordered_map<ConfigSource, int> sourceCounts;
    for (const auto& entry : pImpl->entries) {
        sourceCounts[entry.source]++;
    }
    
    oss << "Source distribution:";
    for (const auto& pair : sourceCounts) {
        std::string sourceName;
        switch (pair.first) {
            case ConfigSource::Default: sourceName = "Default"; break;
            case ConfigSource::File: sourceName = "File"; break;
            case ConfigSource::CommandLine: sourceName = "CommandLine"; break;
            case ConfigSource::Runtime: sourceName = "Runtime"; break;
        }
        oss << "    " << sourceName << ": " << pair.second << "\n";
    }
    
    oss << "\nParameters:";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = " << ConfigUtils::valueToString(entry.value);
        oss << " (source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    if (!schema_.empty()) {
        oss << "\nSchema: " << schema_.size() << " entries defined\n";
    }
    
    return oss.str();
}

bool Config::isValid() const {
    // Check for required schema entries
    for (const auto& schemaEntry : schema_) {
        if (schemaEntry.required && !has(schemaEntry.key)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> Config::getErrors() const {
    // This would be populated during validation
    // For now, return empty vector
    return std::vector<std::string>();
}

std::vector<std::string> Config::getWarnings() const {
    // This would be populated during validation
    // For now, return empty vector
    return std::vector<std::string>();
}

void Config::merge(const Config& other, bool override) {
    for (const auto& otherEntry : other.pImpl->entries) {
        bool exists = has(otherEntry.key);
        
        if (exists && !override) {
            // Keep existing value if override is false
            continue;
        }
        
        // Update or add entry
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });
        
        if (it != pImpl->entries.end()) {
            it->value = otherEntry.value;
            it->source = otherEntry.source;
        } else {
            pImpl->entries.push_back(otherEntry);
        }
    }
}

std::vector<ConfigSchemaEntry> Config::getSchemaEntries() const {
    return pImpl->schema;
}

void Config::addSchemaEntry(const ConfigSchemaEntry& entry) {
    pImpl->schema.push_back(entry);
}

// Template instantiations for common types
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

// Convenience methods
void Config::setString(const std::string& key, const std::string& value, ConfigSource source) {
    set<std::string>(key, value, source);
}

void Config::setInt(const std::string& key, int value, ConfigSource source) {
    set<int>(key, value, source);
}

void Config::setDouble(const std::string& key, double value, ConfigSource source) {
    set<double>(key, value, source);
}

void Config::setBool(const std::string& key, bool value, ConfigSource source) {
    set<bool>(key, value, source);
}

// Implementation of template methods
// Note: These would be implemented in the .cpp file

} // namespace nlm