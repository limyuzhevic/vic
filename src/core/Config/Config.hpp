#pragma once

#include "Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <set>

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Configuration validation
    struct ValidationRule {
        std::string key;
        std::string type; // "int", "double", "bool", "string", "vector"
        std::string validation; // "range", "enum", "pattern", "required"
        std::string min; // For range validation
        std::string max; // For range validation
        std::string defaultValue;
        bool required;
        std::set<std::string> enumValues;
        std::string pattern; // For string pattern validation
        std::string description;
        
        ValidationRule() : required(false), min(""), max(""), defaultValue(""), 
                          enumValues(), pattern(""), description("") {}
    };
    
    std::vector<ValidationRule> validationRules;
    
    // Validation results
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ValidationResult() : valid(true) {}
        
        void addError(const std::string& error) {
            errors.push_back(error);
            valid = false;
        }
        
        void addWarning(const std::string& warning) {
            warnings.push_back(warning);
        }
    };
    
    // Configuration schema
    struct ConfigSchema {
        std::string name;
        std::string version;
        std::vector<ValidationRule> rules;
        std::string description;
        
        ConfigSchema() : version("1.0"), description("") {}
    };
    
    ConfigSchema schema;
    
    // Statistics
    size_t totalChanges;
    size_t validationFailures;
    
    Impl() : totalChanges(0), validationFailures(0) {
        // Initialize with default validation rules
        initializeDefaultSchema();
    }
    
    void initializeDefaultSchema() {
        schema.name = "NLM Configuration";
        schema.version = "1.0";
        schema.description = "Default NLM configuration schema";
        
        // Add common validation rules
        ValidationRule seedRule;
        seedRule.key = "random_seed";
        seedRule.type = "int";
        seedRule.validation = "range";
        seedRule.min = "0";
        seedRule.max = "2147483647";
        seedRule.defaultValue = "42";
        seedRule.required = false;
        seedRule.description = "Random seed for reproducible simulations";
        schema.rules.push_back(seedRule);
        
        ValidationRule timestepRule;
        timestepRule.key = "simulation_timestep";
        timestepRule.type = "double";
        timestepRule.validation = "range";
        timestepRule.min = "0.0001";
        timestepRule.max = "1.0";
        timestepRule.defaultValue = "0.001";
        timestepRule.required = true;
        timestepRule.description = "Simulation time step in seconds";
        schema.rules.push_back(timestepRule);
        
        ValidationRule neuronCountRule;
        neuronCountRule.key = "neuron_count";
        neuronCountRule.type = "int";
        neuronCountRule.validation = "range";
        neuronCountRule.min = "1";
        neuronCountRule.max = "1000000";
        neuronCountRule.defaultValue = "1000";
        neuronCountRule.required = true;
        neuronCountRule.description = "Number of neurons in the simulation";
        schema.rules.push_back(neuronCountRule);
        
        ValidationRule connectionProbRule;
        connectionProbRule.key = "connection_probability";
        connectionProbRule.type = "double";
        connectionProbRule.validation = "range";
        connectionProbRule.min = "0.0";
        connectionProbRule.max = "1.0";
        connectionProbRule.defaultValue = "0.1";
        connectionProbRule.required = true;
        connectionProbRule.description = "Probability of connections between neurons";
        schema.rules.push_back(connectionProbRule);
        
        ValidationRule regionCountRule;
        regionCountRule.key = "region_count";
        regionCountRule.type = "int";
        regionCountRule.validation = "range";
        regionCountRule.min = "1";
        regionCountRule.max = "100";
        regionCountRule.defaultValue = "1";
        regionCountRule.required = true;
        regionCountRule.description = "Number of neural regions";
        schema.rules.push_back(regionCountRule);
        
        ValidationRule stdpLtpRule;
        stdpLtpRule.key = "stdp_ltp_weight";
        stdpLtpRule.type = "double";
        stdpLtpRule.validation = "range";
        stdpLtpRule.min = "0.0";
        stdpLtpRule.max = "1.0";
        stdpLtpRule.defaultValue = "0.02";
        stdpLtpRule.required = true;
        stdpLtpRule.description = "STDP Long-Term Potentiation weight factor";
        schema.rules.push_back(stdpLtpRule);
        
        ValidationRule stdpLtdRule;
        stdpLtdRule.key = "stdp_ltd_weight";
        stdpLtdRule.type = "double";
        stdpLtdRule.validation = "range";
        stdpLtdRule.min = "0.0";
        stdpLtdRule.max = "1.0";
        stdpLtdRule.defaultValue = "0.015";
        stdpLtdRule.required = true;
        stdpLtdRule.description = "STDP Long-Term Depression weight factor";
        schema.rules.push_back(stdpLtdRule);
        
        ValidationRule stdpTauRule;
        stdpTauRule.key = "stdp_tau";
        stdpTauRule.type = "double";
        stdpTauRule.validation = "range";
        stdpTauRule.min = "0.1";
        stdpTauRule.max = "1000.0";
        stdpTauRule.defaultValue = "20.0";
        stdpTauRule.required = true;
        stdpTauRule.description = "STDP time constant (ms)";
        schema.rules.push_back(stdpTauRule);
        
        ValidationRule synaptogenesisRule;
        synaptogenesisRule.key = "synaptogenesis_rate";
        synaptogenesisRule.type = "double";
        synaptogenesisRule.validation = "range";
        synaptogenesisRule.min = "0.0";
        synaptogenesisRule.max = "1.0";
        synaptogenesisRule.defaultValue = "0.0001";
        synaptogenesisRule.required = true;
        synaptogenesisRule.description = "Rate of new synapse formation";
        schema.rules.push_back(synaptogenesisRule);
        
        ValidationRule pruningRule;
        pruningRule.key = "pruning_rate";
        pruningRule.type = "double";
        pruningRule.validation = "range";
        pruningRule.min = "0.0";
        pruningRule.max = "1.0";
        pruningRule.defaultValue = "0.00001";
        pruningRule.required = true;
        pruningRule.description = "Rate of synapse removal";
        schema.rules.push_back(pruningRule);
        
        ValidationRule replayIntervalRule;
        replayIntervalRule.key = "replay_interval";
        replayIntervalRule.type = "int";
        replayIntervalRule.validation = "range";
        replayIntervalRule.min = "1";
        replayIntervalRule.max = "100000";
        replayIntervalRule.defaultValue = "100";
        replayIntervalRule.required = false;
        replayIntervalRule.description = "Steps between memory replay events";
        schema.rules.push_back(replayIntervalRule);
        
        ValidationRule consolidationIntervalRule;
        consolidationIntervalRule.key = "consolidation_interval";
        consolidationIntervalRule.type = "int";
        consolidationIntervalRule.validation = "range";
        consolidationIntervalRule.min = "1";
        consolidationIntervalRule.max = "1000000";
        consolidationIntervalRule.defaultValue = "1000";
        consolidationIntervalRule.required = false;
        consolidationIntervalRule.description = "Steps between memory consolidation events";
        schema.rules.push_back(consolidationIntervalRule);
        
        ValidationRule checkpointDirRule;
        checkpointDirRule.key = "checkpoint_dir";
        checkpointDirRule.type = "string";
        checkpointDirRule.validation = "pattern";
        checkpointDirRule.pattern = ".*";
        checkpointDirRule.defaultValue = "./checkpoints";
        checkpointDirRule.required = false;
        checkpointDirRule.description = "Directory for checkpoint files";
        schema.rules.push_back(checkpointDirRule);
        
        ValidationRule compressionLevelRule;
        compressionLevelRule.key = "compression_level";
        compressionLevelRule.type = "int";
        compressionLevelRule.validation = "range";
        compressionLevelRule.min = "0";
        compressionLevelRule.max = "9";
        compressionLevelRule.defaultValue = "5";
        compressionLevelRule.required = false;
        compressionLevelRule.description = "ZIP compression level (0-9)";
        schema.rules.push_back(compressionLevelRule);
    }
    
    // Validate configuration
    ValidationResult validate() const {
        ValidationResult result;
        
        // Check required rules first
        for (const auto& rule : schema.rules) {
            if (rule.required) {
                if (!has(rule.key)) {
                    result.addError("Required configuration key missing: " + rule.key);
                }
            }
        }
        
        // Validate each entry against rules
        for (const auto& entry : entries) {
            // Find matching validation rule
            for (const auto& rule : schema.rules) {
                if (rule.key == entry.key) {
                    if (!validateEntry(entry, rule, result)) {
                        result.addError("Invalid configuration value for key: " + entry.key);
                    }
                    break;
                }
            }
            
            // Check for unknown keys (optional warning)
            bool isKnown = false;
            for (const auto& rule : schema.rules) {
                if (rule.key == entry.key) {
                    isKnown = true;
                    break;
                }
            }
            if (!isKnown) {
                result.addWarning("Unknown configuration key: " + entry.key);
            }
        }
        
        return result;
    }
    
    bool validateEntry(const ConfigEntry& entry, const ValidationRule& rule, ValidationResult& result) const {
        try {
            if (rule.type == "int") {
                if (!std::holds_alternative<int>(entry.value)) {
                    result.addError("Configuration key '" + entry.key + "' expects int type, got " + getTypeName(entry.value));
                    return false;
                }
                
                int value = std::get<int>(entry.value);
                if (rule.validation == "range") {
                    try {
                        int minVal = std::stoi(rule.min);
                        int maxVal = std::stoi(rule.max);
                        if (value < minVal || value > maxVal) {
                            result.addError("Configuration key '" + entry.key + "' value " + std::to_string(value) + " outside range [" + rule.min + ", " + rule.max + "]");
                            return false;
                        }
                    } catch (const std::exception&) {
                        result.addError("Invalid range specification for key '" + entry.key + "'");
                        return false;
                    }
                }
            } else if (rule.type == "double") {
                if (!std::holds_alternative<double>(entry.value)) {
                    result.addError("Configuration key '" + entry.key + "' expects double type, got " + getTypeName(entry.value));
                    return false;
                }
                
                double value = std::get<double>(entry.value);
                if (rule.validation == "range") {
                    try {
                        double minVal = std::stod(rule.min);
                        double maxVal = std::stod(rule.max);
                        if (value < minVal || value > maxVal) {
                            result.addError("Configuration key '" + entry.key + "' value " + std::to_string(value) + " outside range [" + rule.min + ", " + rule.max + "]");
                            return false;
                        }
                    } catch (const std::exception&) {
                        result.addError("Invalid range specification for key '" + entry.key + "'");
                        return false;
                    }
                }
            } else if (rule.type == "bool") {
                if (!std::holds_alternative<bool>(entry.value)) {
                    result.addError("Configuration key '" + entry.key + "' expects bool type, got " + getTypeName(entry.value));
                    return false;
                }
            } else if (rule.type == "string") {
                if (!std::holds_alternative<std::string>(entry.value)) {
                    result.addError("Configuration key '" + entry.key + "' expects string type, got " + getTypeName(entry.value));
                    return false;
                }
                
                if (rule.validation == "pattern") {
                    std::string value = std::get<std::string>(entry.value);
                    std::regex pattern(rule.pattern);
                    if (!std::regex_match(value, pattern)) {
                        result.addError("Configuration key '" + entry.key + "' value '" + value + "' does not match pattern '" + rule.pattern + "'");
                        return false;
                    }
                }
            }
            
            // Check enum values if specified
            if (!rule.enumValues.empty()) {
                if (rule.type == "int") {
                    int value = std::get<int>(entry.value);
                    if (rule.enumValues.find(std::to_string(value)) == rule.enumValues.end()) {
                        result.addError("Configuration key '" + entry.key + "' value " + std::to_string(value) + " not in allowed enum values");
                        return false;
                    }
                } else if (rule.type == "string") {
                    std::string value = std::get<std::string>(entry.value);
                    if (rule.enumValues.find(value) == rule.enumValues.end()) {
                        result.addError("Configuration key '" + entry.key + "' value '" + value + "' not in allowed enum values");
                        return false;
                    }
                }
            }
            
            return true;
        } catch (const std::exception& e) {
            result.addError("Exception during validation of key '" + entry.key + "': " + e.what());
            return false;
        }
    }
    
    std::string getTypeName(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return "int";
        if (std::holds_alternative<int64_t>(value)) return "int64_t";
        if (std::holds_alternative<double>(value)) return "double";
        if (std::holds_alternative<bool>(value)) return "bool";
        if (std::holds_alternative<std::string>(value)) return "string";
        if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
        if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
        if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
        return "unknown";
    }
    
    // Add validation rule
    void addValidationRule(const ValidationRule& rule) {
        // Check if rule already exists
        for (auto it = schema.rules.begin(); it != schema.rules.end(); ++it) {
            if (it->key == rule.key) {
                *it = rule;
                return;
            }
        }
        schema.rules.push_back(rule);
    }
    
    // Get configuration statistics
    std::string getStatistics() const {
        std::ostringstream oss;
        oss << "Configuration Statistics:\n";
        oss << "  Total entries: " << entries.size() << "\n";
        oss << "  Total validation rules: " << schema.rules.size() << "\n";
        oss << "  Required entries: " << getRequiredCount() << "\n";
        oss << "  Missing required: " << getMissingRequiredCount() << "\n";
        oss << "  Unknown keys: " << getUnknownKeyCount() << "\n";
        oss << "  Total changes: " << totalChanges << "\n";
        oss << "  Validation failures: " << validationFailures << "\n";
        return oss.str();
    }
    
    size_t getRequiredCount() const {
        size_t required = 0;
        for (const auto& rule : schema.rules) {
            if (rule.required) required++;
        }
        return required;
    }
    
    size_t getMissingRequiredCount() const {
        size_t missing = 0;
        for (const auto& rule : schema.rules) {
            if (rule.required && !has(rule.key)) {
                missing++;
            }
        }
        return missing;
    }
    
    size_t getUnknownKeyCount() const {
        size_t unknown = 0;
        for (const auto& entry : entries) {
            bool isKnown = false;
            for (const auto& rule : schema.rules) {
                if (rule.key == entry.key) {
                    isKnown = true;
                    break;
                }
            }
            if (!isKnown) unknown++;
        }
        return unknown;
    }
    
    // Export configuration to JSON
    std::string toJSON() const {
        json j;
        j["name"] = schema.name;
        j["version"] = schema.version;
        j["description"] = schema.description;
        
        json entriesJson = json::array();
        for (const auto& entry : entries) {
            json entryJson;
            entryJson["key"] = entry.key;
            entryJson["value"] = configValueToJson(entry.value);
            entryJson["source"] = static_cast<int>(entry.source);
            entryJson["description"] = entry.description;
            entriesJson.push_back(entryJson);
        }
        j["entries"] = entriesJson;
        
        return j.dump(2);
    }
    
    // Import configuration from JSON
    bool fromJSON(const std::string& jsonStr) {
        try {
            json j = json::parse(jsonStr);
            
            if (j.contains("name")) {
                schema.name = j["name"].get<std::string>();
            }
            if (j.contains("version")) {
                schema.version = j["version"].get<std::string>();
            }
            if (j.contains("description")) {
                schema.description = j["description"].get<std::string>();
            }
            
            // Clear existing entries
            entries.clear();
            
            // Import entries
            if (j.contains("entries")) {
                for (const auto& entryJson : j["entries"]) {
                    ConfigEntry entry;
                    entry.key = entryJson["key"].get<std::string>();
                    entry.value = jsonToConfigValue(entryJson["value"]);
                    entry.source = static_cast<ConfigSource>(entryJson["source"].get<int>());
                    if (entryJson.contains("description")) {
                        entry.description = entryJson["description"].get<std::string>();
                    }
                    entries.push_back(entry);
                }
            }
            
            // Validate imported configuration
            ValidationResult result = validate();
            if (!result.valid) {
                validationFailures++;
                NLM_LOG_ERROR("Imported configuration validation failed:");
                for (const auto& error : result.errors) {
                    NLM_LOG_ERROR("  " + error);
                }
                return false;
            }
            
            return true;
        } catch (const json::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to parse JSON configuration: ") + e.what());
            return false;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to import configuration: ") + e.what());
            return false;
        }
    }
    
    // Convert ConfigValue to JSON
    json configValueToJson(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            return std::get<std::vector<int>>(value);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value);
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return std::get<std::vector<std::string>>(value);
        } else {
            return nullptr;
        }
    }
    
    // Convert JSON to ConfigValue
    ConfigValue jsonToConfigValue(const json& j) const {
        if (j.is_number_integer()) {
            return j.get<int>();
        } else if (j.is_number_unsigned()) {
            return static_cast<int64_t>(j.get<uint64_t>());
        } else if (j.is_number()) {
            return j.get<double>();
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_array()) {
            // Determine array element type
            if (!j.empty()) {
                if (j[0].is_number_integer()) {
                    std::vector<int> vec = j.get<std::vector<int>>();
                    return vec;
                } else if (j[0].is_number()) {
                    std::vector<double> vec = j.get<std::vector<double>>();
                    return vec;
                } else if (j[0].is_string()) {
                    std::vector<std::string> vec = j.get<std::vector<std::string>>();
                    return vec;
                }
            }
            return std::vector<std::string>{};
        }
        return "";
    }
    
    // Apply configuration changes safely
    bool applyChanges(const std::vector<std::pair<std::string, ConfigValue>>& changes) {
        bool success = true;
        
        for (const auto& change : changes) {
            try {
                // Validate change before applying
                ValidationResult changeResult = validateSingleChange(change.first, change.second);
                if (!changeResult.valid) {
                    validationFailures++;
                    NLM_LOG_ERROR("Invalid configuration change for key '" + change.first + "':");
                    for (const auto& error : changeResult.errors) {
                        NLM_LOG_ERROR("  " + error);
                    }
                    success = false;
                    continue;
                }
                
                // Apply change
                set(change.first, change.second, ConfigSource::Runtime);
                totalChanges++;
                
                NLM_LOG_INFO("Applied configuration change: " + change.first + " = [value]");
                
            } catch (const std::exception& e) {
                validationFailures++;
                NLM_LOG_ERROR(std::string("Exception applying configuration change for key '" + change.first + "': ") + e.what());
                success = false;
            }
        }
        
        return success;
    }
    
    ValidationResult validateSingleChange(const std::string& key, const ConfigValue& value) const {
        ValidationResult result;
        
        // Find validation rule for this key
        for (const auto& rule : schema.rules) {
            if (rule.key == key) {
                // Create temporary entry for validation
                ConfigEntry tempEntry(key, value, ConfigSource::Runtime);
                return validateEntry(tempEntry, rule, result);
            }
        }
        
        // No rule found - warn but don't fail
        result.addWarning("Unknown configuration key: " + key);
        return result;
    }
    
    // Set default values for all required rules
    void applyDefaultValues() {
        for (const auto& rule : schema.rules) {
            if (!has(rule.key) && !rule.defaultValue.empty()) {
                // Parse default value based on type
                ConfigValue defaultVal;
                if (rule.type == "int") {
                    try {
                        int val = std::stoi(rule.defaultValue);
                        defaultVal = val;
                    } catch (...) {
                        NLM_LOG_WARNING(std::string("Failed to parse default int value for key '" + rule.key + "'"));
                        continue;
                    }
                } else if (rule.type == "double") {
                    try {
                        double val = std::stod(rule.defaultValue);
                        defaultVal = val;
                    } catch (...) {
                        NLM_LOG_WARNING(std::string("Failed to parse default double value for key '" + rule.key + "'"));
                        continue;
                    }
                } else if (rule.type == "bool") {
                    if (rule.defaultValue == "true" || rule.defaultValue == "1") {
                        defaultVal = true;
                    } else {
                        defaultVal = false;
                    }
                } else if (rule.type == "string") {
                    defaultVal = rule.defaultValue;
                } else {
                    continue;
                }
                
                set(rule.key, defaultVal, ConfigSource::Default);
                NLM_LOG_INFO("Applied default value for key '" + rule.key + "': " + rule.defaultValue);
            }
        }
    }
    
    // Get validation errors as string
    std::string getValidationErrors() const {
        ValidationResult result = validate();
        if (result.errors.empty()) {
            return "";
        }
        
        std::ostringstream oss;
        for (const auto& error : result.errors) {
            oss << error << "\n";
        }
        return oss.str();
    }
    
    // Get validation warnings as string
    std::string getValidationWarnings() const {
        ValidationResult result = validate();
        if (result.warnings.empty()) {
            return "";
        }
        
        std::ostringstream oss;
        for (const auto& warning : result.warnings) {
            oss << warning << "\n";
        }
        return oss.str();
    }
    
    // Export configuration to file
    bool exportToFile(const std::string& filepath) const {
        try {
            // Ensure directory exists
            std::filesystem::path path(filepath);
            if (path.has_parent_path()) {
                std::filesystem::create_directories(path.parent_path());
            }
            
            // Write JSON configuration
            std::ofstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
                return false;
            }
            
            file << toJSON();
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to export configuration to file: ") + e.what());
            return false;
        }
    }
    
    // Import configuration from file
    bool importFromFile(const std::string& filepath) {
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open configuration file for reading: " + filepath);
                return false;
            }
            
            std::stringstream buffer;
            buffer << file.rdbuf();
            
            return fromJSON(buffer.str());
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to import configuration from file: ") + e.what());
            return false;
        }
    }
    
    // Clear command-line arguments from entries
    bool clearCommandLineArgs() {
        for (auto it = entries.begin(); it != entries.end(); ) {
            if (it->source == ConfigSource::CommandLine) {
                it = entries.erase(it);
            } else {
                ++it;
            }
        }
        return true;
    }
    
    bool has(const std::string& key) const {
        return std::any_of(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    }
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source) {
        auto it = std::find_if(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            entries.emplace_back(key, value, source);
        }
    }
    
    // Helper function for has() method
    static bool has(const std::vector<ConfigEntry>& entries, const std::string& key) {
        return std::any_of(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    }
    
    // Helper function for trim
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
    
    // Implementation of has() method for the actual object
    bool has(const std::string& key) const {
        return has(this->entries, key);
    }
    
    // Remove key
    void remove(const std::string& key) {
        entries.erase(
            std::remove_if(entries.begin(), entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            entries.end()
        );
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        keys.reserve(entries.size());
        for (const auto& entry : entries) {
            keys.push_back(entry.key);
        }
        return keys;
    }
    
    // Clear all
    void clear() {
        entries.clear();
    }
    
    // Get configuration summary
    std::string summary() const {
        std::ostringstream oss;
        oss << "Configuration Summary:" << std::endl;
        oss << "  Total entries: " << entries.size() << std::endl;
        oss << "  Total validation rules: " << schema.rules.size() << std::endl;
        
        // Validation status
        ValidationResult validationResult = validate();
        if (validationResult.valid) {
            oss << "  Validation: PASS" << std::endl;
        } else {
            oss << "  Validation: FAIL" << std::endl;
            oss << "  Errors:" << std::endl;
            for (const auto& error : validationResult.errors) {
                oss << "    - " << error << std::endl;
            }
        }
        
        oss << "  Statistics:" << std::endl;
        oss << getStatistics();
        
        return oss.str();
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    try {
        // Apply default values immediately
        pImpl->applyDefaultValues();
        
        NLM_LOG_INFO("Configuration initialized with default values");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to initialize configuration: ") + e.what());
        throw;
    }
}

Config::~Config() {
    try {
        // Clear entries and rules
        if (pImpl) {
            pImpl->entries.clear();
            pImpl->schema.rules.clear();
            pImpl->validationRules.clear();
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during configuration cleanup: ") + e.what());
    }
}

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        NLM_LOG_INFO("Loading configuration from file: " + filepath);
        
        // Check if file exists
        if (!std::filesystem::exists(filepath)) {
            NLM_LOG_ERROR("Configuration file not found: " + filepath);
            return false;
        }
        
        // Parse file based on extension
        std::filesystem::path path(filepath);
        std::string extension = path.extension().string();
        
        if (extension == ".json" || extension == ".JSON") {
            return pImpl->importFromFile(filepath);
        } else {
            // Parse as key=value format (current implementation)
            return loadFromFileImpl(filepath);
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load configuration from file: ") + e.what());
        return false;
    }
}

bool Config::loadFromFileImpl(const std::string& filepath) {
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
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        NLM_LOG_INFO("Loading configuration from command line arguments");
        
        // First, clear any command-line arguments from previous loads
        clearCommandLineArgs();
        
        // Parse arguments
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load configuration from command line arguments: ") + e.what());
        return false;
    }
}

void Config::clearCommandLineArgs() {
    for (auto it = pImpl->entries.begin(); it != pImpl->entries.end(); ) {
        if (it->source == ConfigSource::CommandLine) {
            it = pImpl->entries.erase(it);
        } else {
            ++it;
        }
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        NLM_LOG_INFO("Saving configuration to file: " + filepath);
        
        // Export as JSON
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
            return false;
        }
        
        file << pImpl->toJSON();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save configuration to file: ") + e.what());
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
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
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting configuration value for key '" + key + "': ") + e.what());
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    try {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    } catch (const std::exception& e) {
        NLM_LOG_WARNING(std::string("Exception gettingOr configuration value for key '" + key + "': ") + e.what());
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        NLM_LOG_INFO("Setting configuration: " + key + " = [value] (source: " + std::to_string(static_cast<int>(source)) + ")");
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
        
        // Validate the new value
        Config::Impl::ValidationResult validationResult = pImpl->validateSingleChange(key, value);
        if (!validationResult.valid) {
            pImpl->validationFailures++;
            for (const auto& error : validationResult.errors) {
                NLM_LOG_WARNING("Configuration validation warning: " + error);
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception setting configuration key '" + key + "': ") + e.what());
        throw;
    }
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
    try {
        return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception checking configuration key '" + key + "': ") + e.what());
        return false;
    }
}

void Config::remove(const std::string& key) {
    try {
        NLM_LOG_INFO("Removing configuration key: " + key);
        remove(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception removing configuration key '" + key + "': ") + e.what());
        throw;
    }
}

std::vector<std::string> Config::getKeys() const {
    try {
        std::vector<std::string> keys;
        keys.reserve(pImpl->entries.size());
        for (const auto& entry : pImpl->entries) {
            keys.push_back(entry.key);
        }
        return keys;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting configuration keys: ") + e.what());
        return {};
    }
}

void Config::clear() {
    try {
        NLM_LOG_INFO("Clearing all configuration entries");
        pImpl->entries.clear();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception clearing configuration: ") + e.what());
        throw;
    }
}

std::string Config::summary() const {
    try {
        std::ostringstream oss;
        oss << "Configuration Summary:" << std::endl;
        oss << "  Total entries: " << pImpl->entries.size() << std::endl;
        oss << "  Total validation rules: " << pImpl->schema.rules.size() << std::endl;
        
        // Validation status
        Config::Impl::ValidationResult validationResult = pImpl->validate();
        if (validationResult.valid) {
            oss << "  Validation: PASS" << std::endl;
        } else {
            oss << "  Validation: FAIL" << std::endl;
            oss << "  Errors:" << std::endl;
            for (const auto& error : validationResult.errors) {
                oss << "    - " << error << std::endl;
            }
        }
        
        oss << "  Statistics:" << std::endl;
        oss << pImpl->getStatistics();
        
        return oss.str();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception generating configuration summary: ") + e.what());
        return "Error generating summary: " + std::string(e.what());
    }
}

// Export validation rules to JSON
bool Config::exportValidationRules(const std::string& filepath) const {
    try {
        NLM_LOG_INFO("Exporting validation rules to file: " + filepath);
        
        json j;
        j["name"] = pImpl->schema.name;
        j["version"] = pImpl->schema.version;
        j["description"] = pImpl->schema.description;
        
        json rulesJson = json::array();
        for (const auto& rule : pImpl->schema.rules) {
            json ruleJson;
            ruleJson["key"] = rule.key;
            ruleJson["type"] = rule.type;
            ruleJson["validation"] = rule.validation;
            ruleJson["description"] = rule.description;
            
            if (!rule.min.empty()) ruleJson["min"] = rule.min;
            if (!rule.max.empty()) ruleJson["max"] = rule.max;
            if (!rule.defaultValue.empty()) ruleJson["defaultValue"] = rule.defaultValue;
            if (rule.required) ruleJson["required"] = rule.required;
            if (!rule.enumValues.empty()) {
                json enumJson = json::array();
                for (const auto& value : rule.enumValues) {
                    enumJson.push_back(value);
                }
                ruleJson["enumValues"] = enumJson;
            }
            if (!rule.pattern.empty()) ruleJson["pattern"] = rule.pattern;
            
            rulesJson.push_back(ruleJson);
        }
        j["rules"] = rulesJson;
        
        // Ensure directory exists
        std::filesystem::path path(filepath);
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing validation rules: " + filepath);
            return false;
        }
        
        file << j.dump(2);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to export validation rules: ") + e.what());
        return false;
    }
}

// Import validation rules from JSON
bool Config::importValidationRules(const std::string& filepath) {
    try {
        NLM_LOG_INFO("Importing validation rules from file: " + filepath);
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for reading validation rules: " + filepath);
            return false;
        }
        
        json j = json::parse(file);
        
        pImpl->schema.name = j.value("name", pImpl->schema.name);
        pImpl->schema.version = j.value("version", pImpl->schema.version);
        pImpl->schema.description = j.value("description", pImpl->schema.description);
        
        // Clear existing rules
        pImpl->schema.rules.clear();
        
        if (j.contains("rules")) {
            for (const auto& ruleJson : j["rules"]) {
                Config::Impl::ValidationRule rule;
                rule.key = ruleJson.value("key", std::string());
                rule.type = ruleJson.value("type", std::string());
                rule.validation = ruleJson.value("validation", std::string());
                rule.description = ruleJson.value("description", std::string());
                rule.min = ruleJson.value("min", std::string());
                rule.max = ruleJson.value("max", std::string());
                rule.defaultValue = ruleJson.value("defaultValue", std::string());
                rule.required = ruleJson.value("required", false);
                
                if (ruleJson.contains("enumValues")) {
                    for (const auto& value : ruleJson["enumValues"]) {
                        rule.enumValues.insert(value.get<std::string>());
                    }
                }
                
                rule.pattern = ruleJson.value("pattern", std::string());
                
                if (!rule.key.empty()) {
                    pImpl->schema.rules.push_back(rule);
                }
            }
        }
        
        return true;
    } catch (const json::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to parse validation rules JSON: ") + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to import validation rules: ") + e.what());
        return false;
    }
}

// Add validation rule
void Config::addValidationRule(const std::string& key, const std::string& type, 
    const std::string& validation, const std::string& description, 
    const std::string& min, const std::string& max, 
    const std::string& defaultValue, bool required,
    const std::set<std::string>& enumValues, const std::string& pattern) {
    
    Config::Impl::ValidationRule rule;
    rule.key = key;
    rule.type = type;
    rule.validation = validation;
    rule.description = description;
    rule.min = min;
    rule.max = max;
    rule.defaultValue = defaultValue;
    rule.required = required;
    rule.enumValues = enumValues;
    rule.pattern = pattern;
    
    pImpl->addValidationRule(rule);
}

// Validate all configuration
bool Config::validate() const {
    try {
        Config::Impl::ValidationResult result = pImpl->validate();
        
        if (!result.valid) {
            NLM_LOG_ERROR("Configuration validation failed:");
            for (const auto& error : result.errors) {
                NLM_LOG_ERROR("  " + error);
            }
            return false;
        } else if (!result.warnings.empty()) {
            NLM_LOG_WARNING("Configuration validation warnings:");
            for (const auto& warning : result.warnings) {
                NLM_LOG_WARNING("  " + warning);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during configuration validation: ") + e.what());
        return false;
    }
}

// Get validation errors
std::string Config::getValidationErrors() const {
    try {
        return pImpl->getValidationErrors();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting validation errors: ") + e.what());
        return "Error getting validation errors: " + std::string(e.what());
    }
}

// Get validation warnings
std::string Config::getValidationWarnings() const {
    try {
        return pImpl->getValidationWarnings();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting validation warnings: ") + e.what());
        return "Error getting validation warnings: " + std::string(e.what());
    }
}

// Export configuration to JSON file
bool Config::exportToJSON(const std::string& filepath) const {
    try {
        NLM_LOG_INFO("Exporting configuration to JSON file: " + filepath);
        
        // Ensure directory exists
        std::filesystem::path path(filepath);
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for writing JSON: " + filepath);
            return false;
        }
        
        file << pImpl->toJSON();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to export configuration to JSON: ") + e.what());
        return false;
    }
}

// Import configuration from JSON file
bool Config::importFromJSON(const std::string& filepath) {
    try {
        NLM_LOG_INFO("Importing configuration from JSON file: " + filepath);
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for reading JSON: " + filepath);
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        return pImpl->fromJSON(buffer.str());
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to import configuration from JSON: ") + e.what());
        return false;
    }
}

// Apply configuration changes with validation
bool Config::applyChanges(const std::vector<std::pair<std::string, ConfigValue>>& changes) {
    try {
        NLM_LOG_INFO("Applying " + std::to_string(changes.size()) + " configuration changes");
        
        return pImpl->applyChanges(changes);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception applying configuration changes: ") + e.what());
        return false;
    }
}

// Get configuration statistics
std::string Config::getStatistics() const {
    try {
        return pImpl->getStatistics();
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception getting configuration statistics: ") + e.what());
        return "Error getting statistics: " + std::string(e.what());
    }
}

// Add validation rule helper
void Config::addValidationRule(const std::string& key, const std::string& type,
    const std::string& validation, const std::string& description,
    const std::string& defaultValue, bool required) {
    addValidationRule(key, type, validation, description, "", "", defaultValue, required, {}, "");
}

void Config::addValidationRule(const std::string& key, const std::string& type,
    const std::string& validation, const std::string& description,
    const std::string& min, const std::string& max, const std::string& defaultValue) {
    addValidationRule(key, type, validation, description, min, max, defaultValue, false, {}, "");
}

void Config::addValidationRule(const std::string& key, const std::string& type,
    const std::string& description, const std::set<std::string>& enumValues) {
    addValidationRule(key, type, "enum", description, "", "", "", false, enumValues, "");
}

// Explicit template instantiations
#include "Config.cpp"
#include "../core/Logger/Logger.cpp"

// Update Config.cpp with proper error handling
#include <algorithm>
#include <stdexcept>
#include <cstring>

namespace nlm {

// Config::Impl helper functions
bool Config::Impl::has(const std::vector<ConfigEntry>& entries, const std::string& key) {
    return std::any_of(entries.begin(), entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

std::string Config::Impl::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::Impl::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Helper function for remove
void Config::remove(std::vector<ConfigEntry>::iterator begin, std::vector<ConfigEntry>::iterator end,
    const std::function<bool(const ConfigEntry&)>& pred) {
    entries.erase(std::remove_if(begin, end, pred), end);
}

} // namespace nlm

// Add JSON library dependency
#include <nlohmann/json.hpp>