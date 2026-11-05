#include "ConfigurationValidation.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {
namespace ConfigValidation {

    // Validation statistics helper functions
    std::string formatPercentage(double value) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << value << "%";
        return ss.str();
    }

    std::string formatCount(size_t count) {
        return std::to_string(count);
    }

    std::string formatValidationResult(const std::vector<std::string>& errors) {
        if (errors.empty()) {
            return "PASS";
        } else {
            return "FAIL (" + std::to_string(errors.size()) + " errors)";
        }
    }

    // Additional utility functions for validation
    namespace ValidationUtils {

        // Check if configuration key is in required list
        bool isKeyRequired(const std::string& key, const std::vector<std::string>& requiredKeys) {
            return std::find(requiredKeys.begin(), requiredKeys.end(), key) != requiredKeys.end();
        }

        // Get data type from string representation
        std::string getDataTypeFromString(const std::string& value) {
            if (value.empty()) {
                return "empty";
            }
            
            // Check if it's a boolean
            if (value == "true" || value == "false" || value == "1" || value == "0") {
                return "bool";
            }
            
            // Check if it's an integer
            try {
                size_t pos;
                int64_t intVal = std::stoll(value, &pos);
                if (pos == value.length()) {
                    return "int";
                }
            } catch (...) {
                // Not an integer
            }
            
            // Check if it's a floating-point number
            try {
                size_t pos;
                double doubleVal = std::stod(value, &pos);
                if (pos == value.length()) {
                    return "double";
                }
            } catch (...) {
                // Not a double
            }
            
            // Check if it's a vector (e.g., [1,2,3])
            if (value.size() >= 2 && value.front() == '[' && value.back() == ']') {
                // Count commas to estimate vector size
                size_t commaCount = std::count(value.begin() + 1, value.end() - 1, ',');
                if (commaCount > 0) {
                    return "vector";
                }
            }
            
            // Default to string
            return "string";
        }

        // Validate configuration key naming conventions
        bool isKeyNameValid(const std::string& key) {
            if (key.empty() || key.find(' ') != std::string::npos) {
                return false;
            }
            
            // Check for valid characters (alphanumeric, underscore, dot)
            for (char c : key) {
                if (!std::isalnum(c) && c != '_' && c != '.') {
                    return false;
                }
            }
            
            return true;
        }

        // Check for potential security issues in configuration
        std::vector<std::string> checkSecurityIssues(const Config& config) {
            std::vector<std::string> issues;
            
            // Check for suspicious values in critical keys
            if (config.has("random_seed")) {
                auto seed = config.getOptional<int64_t>("random_seed");
                if (seed && (*seed == 0 || *seed == 1 || *seed == 42)) {
                    issues.push_back("Unusual random seed value: " + std::to_string(*seed));
                }
            }
            
            if (config.has("log_filename")) {
                auto filename = config.getOptional<std::string>("log_filename");
                if (filename && (*filename == "nlm.log" || *filename == "log.txt")) {
                    issues.push_back("Generic log filename may cause file conflicts");
                }
            }
            
            // Check for potential performance issues
            if (config.has("neuron_count") && config.has("connection_probability")) {
                auto count = config.getOptional<size_t>("neuron_count");
                auto prob = config.getOptional<double>("connection_probability");
                if (count && prob && *count > 50000 && *prob > 0.2) {
                    issues.push_back("Large network with high connection probability may cause performance issues");
                }
            }
            
            return issues;
        }

        // Generate configuration validation recommendations
        std::vector<std::string> generateRecommendations(const Config& config) {
            std::vector<std::string> recommendations;
            
            // Check if logging is configured for production
            if (config.has("log_level") && config.has("log_to_file")) {
                auto level = config.getOptional<std::string>("log_level");
                auto toFile = config.getOptional<bool>("log_to_file");
                if (level && toFile && *toFile) {
                    std::string lowerLevel = ConfigUtils::toLower(*level);
                    if (lowerLevel == "debug") {
                        recommendations.push_back("Consider using INFO or WARN level for production to reduce log volume");
                    }
                }
            }
            
            // Check if visualization is enabled for headless environments
            if (config.has("visualization_enabled")) {
                auto enabled = config.getOptional<bool>("visualization_enabled");
                if (enabled && *enabled) {
                    recommendations.push_back("Consider disabling visualization for headless environments");
                }
            }
            
            // Check if development features are enabled for production
            if (config.has("enable_development")) {
                auto devEnabled = config.getOptional<bool>("enable_development");
                if (devEnabled && *devEnabled) {
                    recommendations.push_back("Consider disabling development features for production");
                }
            }
            
            return recommendations;
        }

    } // namespace ValidationUtils

} // namespace ConfigValidation

// Implementation of global validation functions
namespace {

    // Global validation report generator
    class ValidationReport {
    public:
        static std::string generateFullReport(const Config& config) {
            std::stringstream report;
            
            report << "=== NLM Configuration Validation Report ===\n\n";
            report << "Configuration Overview:\n";
            report << "  Total keys: " << config.getKeys().size() << "\n";
            report << "  Configuration source: Unknown\n\n";
            
            // Validation results
            report << "=== Validation Results ===\n";
            
            auto errors = ConfigValidation::validateConfiguration(config);
            
            if (errors.empty()) {
                report << "✅ Configuration validation passed successfully.\n\n";
            } else {
                report << "❌ Configuration validation failed with " << errors.size() << " error(s):\n\n";
                for (size_t i = 0; i < errors.size(); ++i) {
                    report << "  " << (i + 1) << ". " << errors[i] << "\n";
                }
                report << "\n";
            }
            
            // Status summary
            report << "=== Status Summary ===\n";
            report << "  Overall status: " << (errors.empty() ? "PASS" : "FAIL") << "\n";
            report << "  Configuration completeness: " << 
                (ConfigValidation::isConfigurationReady(config) ? "COMPLETE" : "INCOMPLETE") << "\n";
            
            // Get validation statistics
            auto stats = SchemaValidation::getValidationStats(config);
            
            report << "  Validation score: " << stats.validationScore << "%\n";
            report << "  Valid keys: " << stats.validKeys << "\n";
            report << "  Invalid keys: " << stats.invalidKeys << "\n";
            
            // Recommendations
            auto recommendations = ValidationUtils::generateRecommendations(config);
            if (!recommendations.empty()) {
                report << "\n=== Recommendations ===\n";
                for (const auto& rec : recommendations) {
                    report << "  • " << rec << "\n";
                }
            }
            
            // Security checks
            auto securityIssues = ValidationUtils::checkSecurityIssues(config);
            if (!securityIssues.empty()) {
                report << "\n=== Security Warnings ===\n";
                for (const auto& issue : securityIssues) {
                    report << "  ⚠️  " << issue << "\n";
                }
            }
            
            report << "\n=== End of Report ===\n";
            
            return report.str();
        }

    };

} // anonymous namespace

// Public validation functions
namespace ConfigValidation {

    // Validate configuration against all available schemas
    std::vector<std::string> validateConfiguration(const Config& config) {
        std::vector<std::string> errors;
        
        // Validate against JSON schema
        auto jsonErrors = SchemaValidation::validateWithJSONSchema(config);
        for (const auto& error : jsonErrors) {
            errors.push_back("JSON Schema: " + error.message);
        }
        
        // Validate against INI rules
        auto iniErrors = SchemaValidation::validateWithINIRules(config);
        for (const auto& error : iniErrors) {
            errors.push_back("INI Format: " + error.message);
        }
        
        // Get validation statistics
        auto stats = SchemaValidation::getValidationStats(config);
        
        // Add warnings to errors if score is low
        if (stats.validationScore < 50.0) {
            errors.push_back("Configuration validation score is low (" + 
                           std::to_string(stats.validationScore) + "%)");
        }
        
        return errors;
    }

    // Create configuration from YAML string
    bool createConfigFromYAMLString(Config& config, const std::string& yamlString) {
        try {
            YAML::Node doc = YAML::Load(yamlString);
            if (!doc.IsMap() || !doc["nlm"].IsMap()) {
                NLM_LOG_ERROR("Invalid YAML configuration format", 
                             "ConfigValidation::createConfigFromYAMLString");
                return false;
            }
            
            config.clear();
            auto nlmNode = doc["nlm"];
            
            for (const auto& it : nlmNode) {
                std::string key = it.first.as<std::string>();
                std::string value = it.second.as<std::string>();
                config.set(key, value, ConfigSource::File);
            }
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error parsing YAML string: ") + e.what(), 
                         "ConfigValidation::createConfigFromYAMLString");
            return false;
        }
    }

    // Create configuration from JSON string
    bool createConfigFromJSONString(Config& config, const std::string& jsonString) {
        try {
            nlohmann::json jsonConfig = nlohmann::json::parse(jsonString);
            
            if (!jsonConfig.contains("nlm") || !jsonConfig["nlm"].is_object()) {
                NLM_LOG_ERROR("Invalid JSON configuration format", 
                             "ConfigValidation::createConfigFromJSONString");
                return false;
            }
            
            config.clear();
            auto nlmNode = jsonConfig["nlm"];
            
            for (const auto& it : nlmNode.items()) {
                std::string key = it.key();
                std::string value = it.value().dump();
                
                // Remove JSON-specific quotes
                if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.size() - 2);
                }
                
                config.set(key, value, ConfigSource::File);
            }
            
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Error parsing JSON string: ") + e.what(), 
                         "ConfigValidation::createConfigFromJSONString");
            return false;
        }
    }

    // Get configuration validation summary
    std::string getConfigurationSummary(const Config& config) {
        auto stats = SchemaValidation::getValidationStats(config);
        std::stringstream summary;
        
        summary << "Configuration Summary:\n";
        summary << "  Total keys: " << stats.totalKeys << "\n";
        summary << "  Valid keys: " << stats.validKeys << "\n";
        summary << "  Invalid keys: " << stats.invalidKeys << "\n";
        summary << "  Validation score: " << std::fixed << std::setprecision(1) 
                << stats.validationScore << "%\n";
        
        if (!stats.warnings.empty()) {
            summary << "  Warnings:\n";
            for (const auto& warning : stats.warnings) {
                summary << "    - " << warning << "\n";
            }
        }
        
        return summary.str();
    }

    // Check if configuration is ready for use
    bool isConfigurationReady(const Config& config) {
        auto errors = validateConfiguration(config);
        
        // Check for critical missing keys
        std::vector<std::string> criticalKeys = {
            "random_seed",
            "simulation_timestep",
            "neuron_count",
            "region_count",
            "connection_probability"
        };
        
        for (const auto& key : criticalKeys) {
            if (!config.has(key)) {
                errors.push_back("Missing critical configuration key: " + key);
            }
        }
        
        return errors.empty();
    }

    // Get detailed configuration validation report
    std::string getDetailedValidationReport(const Config& config) {
        return ValidationReport::generateFullReport(config);
    }

} // namespace ConfigValidation
