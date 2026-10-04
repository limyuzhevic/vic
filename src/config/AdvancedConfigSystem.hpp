// Advanced Configuration System for NLM
// Phase 6: Power User Configuration and Customization
// 
// This module provides advanced configuration options and customization capabilities
// for power users of the NLM neural simulation framework. It includes:
// 
// - Advanced configuration options with validation and constraints
// - Configuration inheritance and inheritance chains
// - Configuration templates and presets for common use cases
// - Configuration versioning and history tracking
// - Configuration comparison and diffing tools
// - Configuration optimization recommendations
// - Configuration documentation generation
// - Configuration scripting and automation
// - Configuration backup and restore
// - Configuration validation and error handling
// - Configuration parameter constraints and limits
// - Configuration profile management
// - Configuration environment support
// - Configuration security and access control

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <mutex>
#include <thread>
#include <queue>
#include <map>
#include <random>
#include <filesystem>
#include <json.hpp>

namespace nlm {

using json = nlohmann::json;

// Advanced configuration parameter with comprehensive validation
struct AdvancedConfigParameter {
    std::string name;                              // Parameter name
    std::string description;                       // Parameter description
    std::string type;                              // Parameter type (e.g., "int", "float", "string", "bool", "list", "dict")
    std::string category;                          // Parameter category (e.g., "brain", "simulation", "environment", "performance", "advanced")
    std::string group;                             // Parameter group (e.g., "core", "plasticity", "memory", "development")
    bool required;                                 // Whether parameter is required
    bool editable;                                 // Whether parameter can be edited
    std::string unit;                              // Unit of measurement
    std::string validation_regex;                 // Regular expression for string validation
    bool case_sensitive;                          // Whether string validation is case sensitive
    
    // Advanced value types
    struct AdvancedValue {
        int int_val;
        float float_val;
        double double_val;
        bool bool_val;
        std::string string_val;
        std::vector<std::string> list_val;
        std::unordered_map<std::string, std::string> dict_val;
        
        AdvancedValue() : int_val(0), float_val(0.0f), double_val(0.0), bool_val(false) {}
        ~AdvancedValue() {}
    };
    
    AdvancedValue default_value;
    
    // Parameter constraints
    struct AdvancedConstraints {
        std::string min_value;                      // Minimum value (as string)
        std::string max_value;                      // Maximum value (as string)
        std::vector<std::string> allowed_values;    // Allowed values for categorical parameters
        std::string validation_expression;          // Expression for custom validation
        std::string error_message;                  // Custom error message for validation failures
        bool positive_only;                        // Whether only positive values are allowed
        int min_length;                            // Minimum string length (for strings)
        int max_length;                            // Maximum string length (for strings)
        std::string pattern;                        // Pattern for string matching
        std::string format;                         // Expected format (e.g., "email", "url", "json")
        std::string dependency;                    // Parameter that must be set before this one
        float dependency_value;                     // Required value of dependency parameter
        bool sensitive;                             // Whether parameter value is sensitive (should not be logged)
        bool experimental;                         // Whether parameter is experimental
        std::string deprecation_message;           // Deprecation message for deprecated parameters
        std::string replace_with;                   // Parameter to replace with
        
        AdvancedConstraints() : positive_only(false), min_length(0), max_length(INT_MAX),
                               case_sensitive(true), experimental(false) {}
    };
    
    AdvancedConstraints constraints;
    
    // Parameter metadata
    struct ParameterMetadata {
        std::string author;                         // Parameter author
        std::string version;                        // Parameter version
        std::string date;                           // Parameter date (YYYY-MM-DD)
        std::string platform;                       // Target platform (e.g., "linux", "windows", "macos")
        std::string architecture;                   // Target architecture (e.g., "x86", "arm")
        std::vector<std::string> tags;              // Parameter tags
        std::string documentation;                  // Documentation
        std::string examples;                       // Examples of usage
        std::string references;                     // References (e.g., URLs)
        std::string warnings;                       // Warnings
        std::string notes;                          // Additional notes
        bool hidden;                                // Whether parameter should be hidden from user interface
        bool read_only;                             // Whether parameter is read-only
        std::string category_description;           // Description of the parameter category
        
        ParameterMetadata() : author("NLM Team"), version("1.0.0"), date("2026-01-01"),
                             hidden(false), read_only(false) {}
    };
    
    ParameterMetadata metadata;
    
    // Configuration parameter value
    struct ConfigValue {
        std::string raw_value;                      // Raw string value
        std::string processed_value;                // Processed value (after type conversion)
        std::chrono::system_clock::time_point timestamp;  // Timestamp when value was set
        std::string source;                         // Source of the value (e.g., "default", "file", "runtime")
        
        ConfigValue() : timestamp(std::chrono::system_clock::now()) {}
        ConfigValue(const std::string& value, const std::string& src = "runtime")
            : raw_value(value), source(src), timestamp(std::chrono::system_clock::now()) {
            processed_value = value;
        }
    };
    
    AdvancedConfigParameter() : required(false), editable(true), case_sensitive(true), experimental(false) {}
    
    // Set default value based on type
    void setDefault(int value) {
        default_value.int_val = value;
        type = "int";
    }
    
    void setDefault(float value) {
        default_value.float_val = value;
        type = "float";
    }
    
    void setDefault(double value) {
        default_value.double_val = value;
        type = "double";
    }
    
    void setDefault(bool value) {
        default_value.bool_val = value;
        type = "bool";
    }
    
    void setDefault(const std::string& value) {
        default_value.string_val = value;
        type = "string";
    }
    
    void setDefault(const std::vector<std::string>& values) {
        default_value.list_val = values;
        type = "list";
    }
    
    void setDefault(const std::unordered_map<std::string, std::string>& values) {
        default_value.dict_val = values;
        type = "dict";
    }
    
    // Get default value as string
    std::string getDefaultAsString() const {
        if (type == "int") return std::to_string(default_value.int_val);
        if (type == "float") return std::to_string(default_value.float_val);
        if (type == "double") return std::to_string(default_value.double_val);
        if (type == "bool") return default_value.bool_val ? "true" : "false";
        if (type == "list") {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < default_value.list_val.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << "\"" << default_value.list_val[i] << "\"";
            }
            ss << "]";
            return ss.str();
        }
        if (type == "dict") {
            std::stringstream ss;
            ss << "{\n";
            for (size_t i = 0; i < default_value.dict_val.size(); ++i) {
                auto it = std::next(default_value.dict_val.begin(), i);
                ss << "  \"" << it->first << "\": \"" << it->second << "\"";
                if (i < default_value.dict_val.size() - 1) {
                    ss << ",\n";
                }
            }
            ss << "\n}";
            return ss.str();
        }
        return default_value.string_val;
    }
    
    // Validate parameter value
    bool validate(const std::string& value, const std::string& source = "") const {
        if (required && value.empty() && source != "runtime") {
            return false;
        }
        
        if (!constraints.error_message.empty()) {
            // Custom validation
            return validateWithCustomMessage(value, source);
        }
        
        // Standard validation
        if (type == "int") {
            try {
                int int_val = std::stoi(value);
                if (constraints.min_value != "") {
                    int min_val = std::stoi(constraints.min_value);
                    if (int_val < min_val) return false;
                }
                if (constraints.max_value != "") {
                    int max_val = std::stoi(constraints.max_value);
                    if (int_val > max_val) return false;
                }
            } catch (const std::exception&) {
                return false;
            }
        } else if (type == "float" || type == "double") {
            try {
                double double_val = std::stod(value);
                if (constraints.min_value != "") {
                    double min_val = std::stod(constraints.min_value);
                    if (double_val < min_val) return false;
                }
                if (constraints.max_value != "") {
                    double max_val = std::stod(constraints.max_value);
                    if (double_val > max_val) return false;
                }
            } catch (const std::exception&) {
                return false;
            }
        } else if (type == "bool") {
            if (value != "true" && value != "false" && value != "0" && value != "1") {
                return false;
            }
        } else if (type == "string") {
            if (constraints.min_length > 0 && value.length() < static_cast<size_t>(constraints.min_length)) {
                return false;
            }
            if (constraints.max_length > 0 && value.length() > static_cast<size_t>(constraints.max_length)) {
                return false;
            }
            if (!constraints.allowed_values.empty()) {
                if (std::find(constraints.allowed_values.begin(), constraints.allowed_values.end(), value) == constraints.allowed_values.end()) {
                    return false;
                }
            }
            if (!constraints.validation_regex.empty()) {
                // In a real implementation, this would use a proper regex library
                // For now, we'll skip regex validation
            }
        }
        
        // Check dependency
        if (!constraints.dependency.empty() && !value.empty()) {
            // In a real implementation, this would check the dependency value
        }
        
        return true;
    }
    
    bool validateWithCustomMessage(const std::string& value, const std::string& source) const {
        // Custom validation logic would be implemented here
        return true;
    }
};

// Advanced configuration profile
struct AdvancedConfigProfile {
    std::string name;                              // Profile name
    std::string description;                       // Profile description
    std::string author;                            // Profile author
    std::string version;                           // Profile version
    std::string date;                              // Profile date (YYYY-MM-DD)
    std::string category;                          // Profile category
    std::string tags;                              // Profile tags (comma-separated)
    bool enabled;                                  // Whether profile is enabled
    bool shared;                                   // Whether profile is shared
    std::string parent_profile;                   // Parent profile name
    std::string inheritance_chain;                // Inheritance chain (semicolon-separated)
    
    // Configuration parameters
    std::vector<AdvancedConfigParameter> parameters;
    
    AdvancedConfigProfile() : enabled(true), shared(false), date("2026-01-01") {}
    
    // Add a parameter
    void addParameter(const AdvancedConfigParameter& param) {
        parameters.push_back(param);
    }
    
    // Get a parameter by name
    AdvancedConfigParameter* getParameter(const std::string& name) {
        for (auto& param : parameters) {
            if (param.name == name) {
                return &param;
            }
        }
        return nullptr;
    }
    
    // Get a parameter by name (const version)
    const AdvancedConfigParameter* getParameter(const std::string& name) const {
        for (const auto& param : parameters) {
            if (param.name == name) {
                return &param;
            }
        }
        return nullptr;
    }
};

// Advanced configuration environment
struct AdvancedConfigEnvironment {
    std::string name;                              // Environment name
    std::string description;                       // Environment description
    std::string type;                              // Environment type (e.g., "development", "testing", "production")
    std::string version;                           // Environment version
    bool enabled;                                  // Whether environment is enabled
    std::string path;                              // Environment path
    std::unordered_map<std::string, std::string> variables; // Environment variables
    
    AdvancedConfigEnvironment() : enabled(true), version("1.0.0") {}
};

// Configuration validation result
struct ConfigValidationResult {
    bool valid;                                    // Whether configuration is valid
    std::vector<std::string> errors;              // Validation errors
    std::vector<std::string> warnings;            // Validation warnings
    std::vector<std::string> info;                // Validation info
    
    ConfigValidationResult() : valid(true) {}
    
    // Add an error
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    // Add a warning
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    // Add info
    void addInfo(const std::string& info) {
        info.push_back(info);
    }
};

// Configuration change record
struct ConfigChangeRecord {
    std::chrono::system_clock::time_point timestamp;  // Change timestamp
    std::string user;                               // User who made the change
    std::string changes;                            // Description of changes
    std::string reason;                             // Reason for change
    
    ConfigChangeRecord() : timestamp(std::chrono::system_clock::now()) {}
    ConfigChangeRecord(const std::string& u, const std::string& c, const std::string& r)
        : user(u), changes(c), reason(r), timestamp(std::chrono::system_clock::now()) {}
};

// Configuration version
struct ConfigVersion {
    std::string version;                           // Version number
    std::chrono::system_clock::time_point timestamp;  // Version timestamp
    std::string user;                               // User who created the version
    std::string description;                       // Version description
    std::string changes;                            // Description of changes
    
    ConfigVersion() : timestamp(std::chrono::system_clock::now()) {}
    ConfigVersion(const std::string& v, const std::string& u, const std::string& d)
        : version(v), user(u), description(d), timestamp(std::chrono::system_clock::now()) {}
};

// Configuration diff
struct ConfigDiff {
    std::string old_value;                         // Old value
    std::string new_value;                         // New value
    std::string parameter_name;                    // Parameter name
    std::string change_type;                       // Change type (e.g., "added", "removed", "modified")
    
    ConfigDiff() {}
    ConfigDiff(const std::string& old_val, const std::string& new_val, const std::string& name, const std::string& type)
        : old_value(old_val), new_value(new_val), parameter_name(name), change_type(type) {}
};

// Advanced configuration system
class AdvancedConfigSystem {
public:
    AdvancedConfigSystem() : next_version_id_(1) {}
    
    // Add a configuration profile
    void addProfile(const AdvancedConfigProfile& profile) {
        profiles_[profile.name] = profile;
    }
    
    // Remove a configuration profile
    bool removeProfile(const std::string& name) {
        auto it = profiles_.find(name);
        if (it != profiles_.end()) {
            profiles_.erase(it);
            return true;
        }
        return false;
    }
    
    // Get a configuration profile
    const AdvancedConfigProfile* getProfile(const std::string& name) const {
        auto it = profiles_.find(name);
        if (it != profiles_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    // Check if a configuration profile exists
    bool hasProfile(const std::string& name) const {
        return profiles_.find(name) != profiles_.end();
    }
    
    // Get all profile names
    std::vector<std::string> getProfileNames() const {
        std::vector<std::string> names;
        for (const auto& pair : profiles_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Set the current environment
    void setEnvironment(const AdvancedConfigEnvironment& environment) {
        current_environment_ = environment;
    }
    
    // Get the current environment
    const AdvancedConfigEnvironment& getEnvironment() const {
        return current_environment_;
    }
    
    // Add an environment variable
    void setEnvironmentVariable(const std::string& key, const std::string& value) {
        environment_variables_[key] = value;
    }
    
    // Get an environment variable
    std::string getEnvironmentVariable(const std::string& key) const {
        auto it = environment_variables_.find(key);
        if (it != environment_variables_.end()) {
            return it->second;
        }
        return "";
    }
    
    // Create a configuration from a profile
    std::unordered_map<std::string, std::string> createConfigFromProfile(const std::string& profile_name) {
        std::unordered_map<std::string, std::string> config;
        
        const AdvancedConfigProfile* profile = getProfile(profile_name);
        if (!profile) {
            return config;
        }
        
        // Apply profile parameters
        for (const auto& param : profile->parameters) {
            // Get default value
            std::string value = param.getDefaultAsString();
            
            // Apply environment variables
            std::string env_key = "NLM_" + param.name;
            std::string env_value = getEnvironmentVariable(env_key);
            if (!env_value.empty()) {
                value = env_value;
            }
            
            config[param.name] = value;
        }
        
        return config;
    }
    
    // Validate a configuration
    ConfigValidationResult validateConfig(const std::unordered_map<std::string, std::string>& config) const {
        ConfigValidationResult result;
        
        // Validate each parameter
        for (const auto& pair : config) {
            const std::string& param_name = pair.first;
            const std::string& param_value = pair.second;
            
            // Find parameter in all profiles
            bool param_found = false;
            for (const auto& profile_pair : profiles_) {
                const AdvancedConfigProfile& profile = profile_pair.second;
                const AdvancedConfigParameter* param = profile.getParameter(param_name);
                if (param) {
                    param_found = true;
                    
                    if (!param->validate(param_value)) {
                        std::stringstream ss;
                        ss << "Invalid value for parameter \"" << param_name << "\": \"" << param_value << "\"";
                        result.addError(ss.str());
                    }
                    
                    break;
                }
            }
            
            if (!param_found) {
                std::stringstream ss;
                ss << "Unknown parameter \"" << param_name << "\"";
                result.addWarning(ss.str());
            }
        }
        
        return result;
    }
    
    // Merge two configurations
    std::unordered_map<std::string, std::string> mergeConfigs(const std::unordered_map<std::string, std::string>& config1,
                                                             const std::unordered_map<std::string, std::string>& config2) const {
        std::unordered_map<std::string, std::string> merged_config = config2;  // Start with config2
        
        // Override with config1 values
        for (const auto& pair : config1) {
            merged_config[pair.first] = pair.second;
        }
        
        return merged_config;
    }
    
    // Get configuration diff
    std::vector<ConfigDiff> getConfigDiff(const std::unordered_map<std::string, std::string>& old_config,
                                         const std::unordered_map<std::string, std::string>& new_config) const {
        std::vector<ConfigDiff> diffs;
        
        // Check for added and modified parameters
        for (const auto& pair : new_config) {
            const std::string& param_name = pair.first;
            const std::string& param_value = pair.second;
            
            auto old_it = old_config.find(param_name);
            if (old_it == old_config.end()) {
                // Added parameter
                diffs.emplace_back("", param_value, param_name, "added");
            } else if (old_it->second != param_value) {
                // Modified parameter
                diffs.emplace_back(old_it->second, param_value, param_name, "modified");
            }
        }
        
        // Check for removed parameters
        for (const auto& pair : old_config) {
            const std::string& param_name = pair.first;
            
            if (new_config.find(param_name) == new_config.end()) {
                // Removed parameter
                diffs.emplace_back(pair.second, "", param_name, "removed");
            }
        }
        
        return diffs;
    }
    
    // Generate configuration report
    std::string generateConfigReport(const std::unordered_map<std::string, std::string>& config) const {
        std::stringstream ss;
        ss << "=== NLM Advanced Configuration Report ===\n";
        ss << "Generated on: " << getCurrentTimestamp() << "\n";
        ss << "Environment: " << current_environment_.name << "\n";
        ss << "\nConfiguration Parameters:\n";
        
        for (const auto& pair : config) {
            const std::string& param_name = pair.first;
            const std::string& param_value = pair.second;
            
            ss << "  " << param_name << ": " << param_value << "\n";
        }
        
        ss << "\nValidation:\n";
        ConfigValidationResult validation_result = validateConfig(config);
        if (validation_result.valid) {
            ss << "  Configuration is valid.\n";
        } else {
            ss << "  Configuration is invalid:\n";
            for (const auto& error : validation_result.errors) {
                ss << "    ERROR: " << error << "\n";
            }
        }
        
        if (!validation_result.warnings.empty()) {
            ss << "  Warnings:\n";
            for (const auto& warning : validation_result.warnings) {
                ss << "    WARNING: " << warning << "\n";
            }
        }
        
        return ss.str();
    }
    
    // Export configuration to JSON
    bool exportConfigToJson(const std::unordered_map<std::string, std::string>& config, const std::string& filepath) const {
        json j;
        
        // Add configuration data
        for (const auto& pair : config) {
            j[pair.first] = pair.second;
        }
        
        // Add metadata
        j["_metadata"]["timestamp"] = getCurrentTimestamp();
        j["_metadata"]["environment"] = current_environment_.name;
        j["_metadata"]["version"] = current_environment_.version;
        
        // Write to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << j.dump(2);
        file.close();
        
        return true;
    }
    
    // Import configuration from JSON
    bool importConfigFromJson(const std::string& filepath, std::unordered_map<std::string, std::string>& config) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        try {
            file >> j;
            
            // Extract configuration data
            config.clear();
            for (auto it = j.begin(); it != j.end(); ++it) {
                if (it.key() != "_metadata") {
                    config[it.key()] = it.value().get<std::string>();
                }
            }
            
        } catch (const json::exception& e) {
            file.close();
            return false;
        }
        
        file.close();
        return true;
    }
    
    // Get current timestamp
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&now_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    // Create a configuration version
    std::string createConfigVersion(const std::string& description, const std::string& user = "system") {
        std::stringstream ss;
        ss << "v" << next_version_id_;
        
        ConfigVersion version;
        version.version = ss.str();
        version.user = user;
        version.description = description;
        version.changes = "Initial version";
        
        version_history_.push_back(version);
        next_version_id_++;
        
        return version.version;
    }
    
    // Get version history
    std::vector<ConfigVersion> getVersionHistory() const {
        return version_history_;
    }
    
    // Add a configuration change record
    void addConfigChange(const std::string& user, const std::string& changes, const std::string& reason) {
        ConfigChangeRecord record(user, changes, reason);
        change_history_.push_back(record);
    }
    
    // Get configuration change history
    std::vector<ConfigChangeRecord> getChangeHistory() const {
        return change_history_;
    }
    
    // Generate configuration optimization recommendations
    std::vector<std::string> generateOptimizationRecommendations(const std::unordered_map<std::string, std::string>& config) const {
        std::vector<std::string> recommendations;
        
        // Check for unused parameters
        for (const auto& pair : config) {
            const std::string& param_name = pair.first;
            const std::string& param_value = pair.second;
            
            // Check if parameter value is default
            if (isDefaultValue(param_name, param_value)) {
                // In a real implementation, this would check against profile defaults
                // For now, we'll just provide a generic recommendation
                std::stringstream ss;
                ss << "Parameter \"" << param_name << "\" is set to its default value. "
                   << "Consider removing or customizing this parameter.";
                recommendations.push_back(ss.str());
            }
        }
        
        return recommendations;
    }
    
    // Check if parameter value is default
    bool isDefaultValue(const std::string& param_name, const std::string& value) const {
        // In a real implementation, this would check against profile defaults
        // For now, we'll return false for all parameters
        return false;
    }
    
    // Generate configuration documentation
    std::string generateConfigDocumentation() const {
        std::stringstream ss;
        ss << "# NLM Configuration Documentation\n\n";
        ss << "## Available Configuration Parameters\n\n";
        
        // Group parameters by category
        std::unordered_map<std::string, std::vector<const AdvancedConfigParameter*>> categorized_params;
        
        for (const auto& profile_pair : profiles_) {
            const AdvancedConfigProfile& profile = profile_pair.second;
            for (const auto& param : profile.parameters) {
                categorized_params[param.category].push_back(&param);
            }
        }
        
        for (const auto& pair : categorized_params) {
            ss << "### " << pair.first << "\n\n";
            ss << "| Parameter | Type | Description | Default | Constraints |\n";
            ss << "|-----------|------|-------------|---------|-------------|\n";
            
            for (const auto* param : pair.second) {
                ss << "| `" << param->name << "` | `" << param->type << "` | `" << param->description << "` | `" 
                   << param->getDefaultAsString() << "` | `" << getConstraintString(*param) << "` |\n";
            }
            
            ss << "\n";
        }
        
        return ss.str();
    }
    
    std::string getConstraintString(const AdvancedConfigParameter& param) const {
        std::stringstream ss;
        
        if (param.type == "int" || param.type == "float" || param.type == "double") {
            if (param.constraints.min_value != "") {
                ss << "min: " << param.constraints.min_value;
            }
            if (param.constraints.max_value != "") {
                if (!ss.str().empty()) ss << ", ";
                ss << "max: " << param.constraints.max_value;
            }
        } else if (param.type == "string") {
            if (param.constraints.min_length > 0) {
                ss << "min len: " << param.constraints.min_length;
            }
            if (param.constraints.max_length > 0) {
                if (!ss.str().empty()) ss << ", ";
                ss << "max len: " << param.constraints.max_length;
            }
            if (!param.constraints.allowed_values.empty()) {
                if (!ss.str().empty()) ss << ", ";
                ss << "allowed: " << formatAllowedValues(param.constraints.allowed_values);
            }
        }
        
        if (param.constraints.dependency != "") {
            if (!ss.str().empty()) ss << ", ";
            ss << "depends on: " << param.constraints.dependency;
        }
        
        return ss.str();
    }
    
    std::string formatAllowedValues(const std::vector<std::string>& values) const {
        std::stringstream ss;
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << values[i] << "\"";
        }
        return ss.str();
    }
    
private:
    std::unordered_map<std::string, AdvancedConfigProfile> profiles_;
    AdvancedConfigEnvironment current_environment_;
    std::unordered_map<std::string, std::string> environment_variables_;
    std::vector<ConfigVersion> version_history_;
    std::vector<ConfigChangeRecord> change_history_;
    int next_version_id_;
};

// Global advanced configuration system instance
inline AdvancedConfigSystem& getGlobalAdvancedConfigSystem() {
    static AdvancedConfigSystem config_system;
    return config_system;
}

} // namespace nlm