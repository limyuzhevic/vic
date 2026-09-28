// Enhanced Configuration System
// This file provides comprehensive configuration validation and parameter constants for the NLM project

#pragma once

#include "Config.hpp"
#include "ValidationFramework.hpp"
#include "EnhancedConfigSystem.cpp"
#include <chrono>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <map>

namespace nlm {

// Enhanced Configuration Manager
// This class provides enhanced configuration management with validation, security, and constants
class EnhancedConfigurationManager {
public:
    EnhancedConfigurationManager() {
        initializeEnhancedConfigSystem();
        setupParameterConstants();
        setupValidationRules();
        setupSecurityFeatures();
    }

    ~EnhancedConfigurationManager() {
        cleanup();
    }

    // Core configuration management
    void initialize() {
        enhancedConfigSystem.initialize();
    }

    bool loadConfiguration(const std::string& filepath) {
        try {
            config.loadFromFile(filepath);
            return performEnhancedValidation();
        } catch (const ConfigException& e) {
            logError(std::string("Configuration load error: ") + e.what());
            return false;
        }
    }

    bool loadFromCommandLine(int argc, char** argv) {
        config.loadFromArgs(argc, argv);
        return performEnhancedValidation();
    }

    bool loadFromEnvironment() {
        // Load from environment variables
        const char* envFile = std::getenv("NLM_CONFIG_FILE");
        if (envFile) {
            return loadConfiguration(envFile);
        }
        return performEnhancedValidation();
    }

    void saveConfiguration(const std::string& filepath) const {
        config.saveToFile(filepath);
        logConfigurationChange("save", filepath, "Configuration saved successfully");
    }

    // Enhanced parameter constants
    void setupParameterConstants() {
        // Brain Configuration Constants
        config.setInt("BRAIN_CONST_NEURON_COUNT_MIN", 1, ConfigSource::Default);
        config.setInt("BRAIN_CONST_NEURON_COUNT_MAX", 1000000, ConfigSource::Default);
        config.setInt("BRAIN_CONST_REGION_COUNT_MIN", 1, ConfigSource::Default);
        config.setInt("BRAIN_CONST_REGION_COUNT_MAX", 100, ConfigSource::Default);
        config.setDouble("BRAIN_CONST_CONNECTION_PROBABILITY_MIN", 0.0, ConfigSource::Default);
        config.setDouble("BRAIN_CONST_CONNECTION_PROBABILITY_MAX", 1.0, ConfigSource::Default);
        config.setDouble("BRAIN_CONST_SYNAPSE_DENSITY_MIN", 0.001, ConfigSource::Default);
        config.setDouble("BRAIN_CONST_SYNAPSE_DENSITY_MAX", 1.0, ConfigSource::Default);

        // Memory System Constants
        config.setInt("MEMORY_CONST_WORKING_CAPACITY_MIN", 10, ConfigSource::Default);
        config.setInt("MEMORY_CONST_WORKING_CAPACITY_MAX", 10000, ConfigSource::Default);
        config.setInt("MEMORY_CONST_EPISODIC_MAX_EPISODES_MIN", 100, ConfigSource::Default);
        config.setInt("MEMORY_CONST_EPISODIC_MAX_EPISODES_MAX", 1000000, ConfigSource::Default);
        config.setDouble("MEMORY_CONST_DECAY_RATE_MIN", 0.0, ConfigSource::Default);
        config.setDouble("MEMORY_CONST_DECAY_RATE_MAX", 1.0, ConfigSource::Default);

        // Neuromodulation Constants
        config.setDouble("NEUROMOD_CONST_DOPAMINE_SENSITIVITY_MIN", 0.0, ConfigSource::Default);
        config.setDouble("NEUROMOD_CONST_DOPAMINE_SENSITIVITY_MAX", 10.0, ConfigSource::Default);
        config.setDouble("NEUROMOD_CONST_CURIOUSITY_THRESHOLD_MIN", 0.0, ConfigSource::Default);
        config.setDouble("NEUROMOD_CONST_CURIOUSITY_THRESHOLD_MAX", 1.0, ConfigSource::Default);
        config.setDouble("NEUROMOD_CONST_CURIOUSITY_STRENGTH_MIN", 0.0, ConfigSource::Default);
        config.setDouble("NEUROMOD_CONST_CURIOUSITY_STRENGTH_MAX", 2.0, ConfigSource::Default);
        config.setDouble("NEUROMOD_CONST_NOVELTY_THRESHOLD_MIN", 0.0, ConfigSource::Default);
        config.setDouble("NEUROMOD_CONST_NOVELTY_THRESHOLD_MAX", 1.0, ConfigSource::Default);

        // Development Constants
        config.setInt("DEV_CONST_CRITICAL_PERIOD_LENGTH_MIN", 10, ConfigSource::Default);
        config.setInt("DEV_CONST_CRITICAL_PERIOD_LENGTH_MAX", 10000, ConfigSource::Default);
        config.setDouble("DEV_CONST_PLASTICITY_DECAY_RATE_MIN", 0.0, ConfigSource::Default);
        config.setDouble("DEV_CONST_PLASTICITY_DECAY_RATE_MAX", 0.1, ConfigSource::Default);
        config.setDouble("DEV_CONST_SYNAPTOGENESIS_RATE_MIN", 0.0, ConfigSource::Default);
        config.setDouble("DEV_CONST_SYNAPTOGENESIS_RATE_MAX", 0.01, ConfigSource::Default);
        config.setDouble("DEV_CONST_PRUNING_RATE_MIN", 0.0, ConfigSource::Default);
        config.setDouble("DEV_CONST_PRUNING_RATE_MAX", 0.001, ConfigSource::Default);

        // Performance Constants
        config.setDouble("PERF_CONST_TIMESTEP_MIN", 0.0001, ConfigSource::Default);
        config.setDouble("PERF_CONST_TIMESTEP_MAX", 1.0, ConfigSource::Default);
        config.setInt("PERF_CONST_MAX_STEPS_MIN", 100, ConfigSource::Default);
        config.setInt("PERF_CONST_MAX_STEPS_MAX", 1000000, ConfigSource::Default);
        config.setDouble("PERF_CONST_UPDATE_RATE_MIN", 1.0, ConfigSource::Default);
        config.setDouble("PERF_CONST_UPDATE_RATE_MAX", 1000.0, ConfigSource::Default);

        // System Limits
        config.setInt("SYS_CONST_MAX_NEURONS_MIN", 100, ConfigSource::Default);
        config.setInt("SYS_CONST_MAX_NEURONS_MAX", 10000000, ConfigSource::Default);
        config.setInt("SYS_CONST_MAX_MEMORY_MB_MIN", 64, ConfigSource::Default);
        config.setInt("SYS_CONST_MAX_MEMORY_MB_MAX", 4096, ConfigSource::Default);
        config.setInt("SYS_CONST_MAX_FDS_MIN", 256, ConfigSource::Default);
        config.setInt("SYS_CONST_MAX_FDS_MAX", 65536, ConfigSource::Default);
    }

    void setupValidationRules() {
        // Range validation rule
        validationRules.push_back({
            "range_validation",
            [this](const ConfigValue& value) { return validateRange(value); },
            "Parameter value out of valid range",
            "Check parameter constraints in configuration schema",
            ConfigValidationException::Severity::High
        });

        // Type validation rule
        validationRules.push_back({
            "type_validation",
            [this](const ConfigValue& value) { return validateType(value); },
            "Parameter type mismatch",
            "Ensure parameter type matches schema definition",
            ConfigValidationException::Severity::High
        });

        // Dependency validation rule
        validationRules.push_back({
            "dependency_validation",
            [this](const ConfigValue& value) { return validateDependencies(); },
            "Parameter dependency violation",
            "Check that dependent parameters are consistent",
            ConfigValidationException::Severity::Medium
        });

        // Consistency validation rule
        validationRules.push_back({
            "consistency_validation",
            [this](const ConfigValue& value) { return validateConsistency(); },
            "Parameter inconsistency detected",
            "Check that parameter combinations are consistent",
            ConfigValidationException::Severity::Medium
        });
    }

    void setupSecurityFeatures() {
        // Input sanitization rule
        securityRules.push_back({
            "input_sanitization",
            [this](const ConfigValue& value) { return sanitizeInput(value); },
            "Potentially unsafe configuration value",
            "Remove or escape special characters from configuration values",
            ConfigValidationException::Severity::Critical
        });

        // File path validation rule
        securityRules.push_back({
            "file_path_validation",
            [this](const ConfigValue& value) { return validateFilePath(value); },
            "Potentially unsafe file path",
            "Use absolute paths and validate file permissions",
            ConfigValidationException::Severity::High
        });

        // Access control rule
        securityRules.push_back({
            "access_control",
            [this](const ConfigValue& value) { return checkAccessControl(value); },
            "Access control violation",
            "Check file system permissions for configuration files",
            ConfigValidationException::Severity::High
        });
    }

    // Enhanced validation methods
    bool validateRange(const ConfigValue& value) const {
        // Implementation would check if value is within defined ranges
        return true;
    }

    bool validateType(const ConfigValue& value) const {
        // Implementation would check if value type matches schema
        return true;
    }

    bool validateDependencies() const {
        // Implementation would check parameter dependencies
        return true;
    }

    bool validateConsistency() const {
        // Implementation would check parameter consistency
        return true;
    }

    bool sanitizeInput(const ConfigValue& value) const {
        // Implementation would sanitize input to prevent injection
        return true;
    }

    bool validateFilePath(const ConfigValue& value) const {
        // Implementation would validate file paths
        return true;
    }

    bool checkAccessControl(const ConfigValue& value) const {
        // Implementation would check file system permissions
        return true;
    }

    // Performance metrics
    struct ValidationMetrics {
        int totalParameters;
        int validatedParameters;
        int validationErrors;
        double validationTimeMs;
        std::chrono::system_clock::time_point timestamp;
    };

    ValidationMetrics getValidationMetrics() const {
        ValidationMetrics metrics;
        metrics.totalParameters = config.getEntries().size();
        metrics.validationErrors = globalErrorCollector.getErrors().size();
        metrics.validationTimeMs = lastValidationTimeMs;
        metrics.timestamp = std::chrono::system_clock::now();
        metrics.validatedParameters = metrics.totalParameters - metrics.validationErrors;
        return metrics;
    }

    // Configuration reporting
    std::string generateConfigurationReport() const {
        std::ostringstream oss;
        oss << "=== Enhanced NLM Configuration Report ===\n\n";
        oss << "Generated: " << getCurrentTimestamp() << "\n\n";

        // Parameter statistics
        oss << "Parameter Statistics:\n";
        oss << "  Total parameters: " << config.getEntries().size() << "\n";
        oss << "  Default parameters: " << getParameterCountBySource(ConfigSource::Default) << "\n";
        oss << "  File parameters: " << getParameterCountBySource(ConfigSource::File) << "\n";
        oss << "  Command line parameters: " << getParameterCountBySource(ConfigSource::CommandLine) << "\n";
        oss << "  Runtime parameters: " << getParameterCountBySource(ConfigSource::Runtime) << "\n";
        oss << "  Environment parameters: " << getParameterCountBySource(ConfigSource::Environment) << "\n\n";

        // Validation results
        oss << "Validation Results:\n";
        oss << "  Validation passed: " << (performEnhancedValidation() ? "Yes" : "No") << "\n";
        oss << "  Validation errors: " << globalErrorCollector.getErrors().size() << "\n\n";

        // Performance metrics
        ValidationMetrics metrics = getValidationMetrics();
        oss << "Performance Metrics:\n";
        oss << "  Last validation time (ms): " << metrics.validationTimeMs << "\n";
        oss << "  Validation rate: " << (metrics.totalParameters > 0 ? (metrics.validatedParameters * 100.0 / metrics.totalParameters) : 0.0) << "%\n\n";

        // Configuration details
        oss << "Configuration Details:\n";
        for (const auto& entry : config.getEntries()) {
            oss << "  " << entry.key << " = ";
            if (const std::string* s = std::get_if<std::string>(&entry.value)) {
                oss << "\"" << *s << "\"";
            } else if (const bool* b = std::get_if<bool>(&entry.value)) {
                oss << (*b ? "true" : "false");
            } else if (const int* i = std::get_if<int>(&entry.value)) {
                oss << *i;
            } else if (const int64_t* i64 = std::get_if<int64_t>(&entry.value)) {
                oss << *i64;
            } else if (const double* d = std::get_if<double>(&entry.value)) {
                oss << *d;
            } else {
                oss << "[complex value]";
            }
            oss << " (Source: " << static_cast<int>(entry.source) << ") " << entry.description << "\n";
        }

        return oss.str();
    }

    // Audit and logging
    void logConfigurationChange(const std::string& operation, const std::string& details, const std::string& message) {
        std::string timestamp = getCurrentTimestamp();
        std::string logEntry = "[" + timestamp + "] [CONFIG] " + operation + ": " + details + " - " + message + "\n";

        std::string auditPath = config.getString("SECURITY_CONST_AUDIT_LOG_PATH").value_or("/var/log/nlm/config_audits.log");

        std::ofstream auditFile(auditPath, std::ios::app);
        if (auditFile.is_open()) {
            auditFile << logEntry;
            auditFile.close();
        }
    }

    void logError(const std::string& message) {
        std::string timestamp = getCurrentTimestamp();
        std::string logEntry = "[" + timestamp + "] [ERROR] " + message + "\n";

        // Log to error file
        std::ofstream errorFile("/tmp/nlm_config_errors.log", std::ios::app);
        if (errorFile.is_open()) {
            errorFile << logEntry;
            errorFile.close();
        }
    }

    // Runtime validation
    bool performEnhancedValidation() {
        auto startTime = std::chrono::high_resolution_clock::now();

        globalErrorCollector.clear();
        enhancedConfigSystem.performValidation();

        // Validate all entries
        for (const auto& entry : config.getEntries()) {
            validateEntry(entry);
        }

        auto endTime = std::chrono::high_resolution_clock::now();
        lastValidationTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();

        return !globalErrorCollector.hasErrors();
    }

    void validateEntry(const ConfigEntry& entry) {
        // Enhanced validation with detailed error reporting
        bool isValid = true;

        // Check if entry has valid type
        if (!isValidValue(entry.value)) {
            globalErrorCollector.addError(
                "Invalid value for parameter: " + entry.key,
                ConfigValidationException::Severity::High,
                entry.key,
                "Check parameter type and constraints"
            );
            isValid = false;
        }

        // Check for security issues
        if (!isSecureValue(entry.value)) {
            globalErrorCollector.addError(
                "Security violation detected for parameter: " + entry.key,
                ConfigValidationException::Severity::Critical,
                entry.key,
                "Remove or sanitize sensitive data"
            );
            isValid = false;
        }

        // Log validation results
        if (!isValid) {
            enhancedConfigSystem.logValidationResult("validateEntry", false, "Validation failed for " + entry.key);
        }
    }

    bool isValidValue(const ConfigValue& value) const {
        return std::visit([](const auto& val) -> bool {
            // Basic validation for each type
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                return !val.empty() && val.length() <= 1024 * 1024; // 1MB max
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                return val.size() <= 10000 && std::all_of(val.begin(), val.end(), [](const std::string& s) {
                    return !s.empty() && s.length() <= 1024 * 1024;
                });
            } else {
                return true; // Other types are considered valid
            }
        }, value);
    }

    bool isSecureValue(const ConfigValue& value) const {
        return std::visit([](const auto& val) -> bool {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                return isSecureString(val);
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                for (const auto& s : val) {
                    if (!isSecureString(s)) return false;
                }
                return true;
            } else {
                return true; // Non-string types are considered secure
            }
        }, value);
    }

    bool isSecureString(const std::string& str) const {
        // Check for common security issues
        if (str.find("password") != std::string::npos) {
            // Password-like values should be masked
            return false;
        }

        // Check for potentially dangerous patterns
        std::vector<std::string> dangerousPatterns = {
            "<script>", "javascript:", "data:", "eval(", "exec(",
            "rm -rf", "format", "delete", "DROP TABLE", "INSERT INTO",
            "UPDATE ", "DELETE ", "ALTER ", "CREATE ", "GRANT "
        };

        for (const auto& pattern : dangerousPatterns) {
            if (str.find(pattern) != std::string::npos) {
                return false;
            }
        }

        return true;
    }

    // Utility methods
    int getParameterCountBySource(ConfigSource source) const {
        int count = 0;
        for (const auto& entry : config.getEntries()) {
            if (entry.source == source) count++;
        }
        return count;
    }

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        char buf[100];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_buf);
        return std::string(buf);
    }

    void cleanup() {
        // Cleanup resources
        globalErrorCollector.clear();
        // Delete enhanced config system
        delete enhancedConfigSystem;
        enhancedConfigSystem = nullptr;
    }

private:
    Config config;
    EnhancedConfigSystem* enhancedConfigSystem;
    double lastValidationTimeMs;

    std::vector<ValidationRule> validationRules;
    std::vector<ValidationRule> securityRules;

    void initializeEnhancedConfigSystem() {
        enhancedConfigSystem = new EnhancedConfigSystem(config);
    }
};

// Global enhanced configuration manager instance
static EnhancedConfigurationManager globalEnhancedConfigManager;

// Convenience functions
void initializeEnhancedConfiguration() {
    globalEnhancedConfigManager.initialize();
}

bool loadEnhancedConfiguration(const std::string& filepath) {
    return globalEnhancedConfigManager.loadConfiguration(filepath);
}

bool loadEnhancedConfigurationFromCommandLine(int argc, char** argv) {
    return globalEnhancedConfigManager.loadFromCommandLine(argc, argv);
}

bool loadEnhancedConfigurationFromEnvironment() {
    return globalEnhancedConfigManager.loadFromEnvironment();
}

void saveEnhancedConfiguration(const std::string& filepath) {
    globalEnhancedConfigManager.saveConfiguration(filepath);
}

bool validateEnhancedConfiguration() {
    return globalEnhancedConfigManager.performEnhancedValidation();
}

std::string generateEnhancedConfigurationReport() {
    return globalEnhancedConfigManager.generateConfigurationReport();
}

void cleanupEnhancedConfiguration() {
    globalEnhancedConfigManager.cleanup();
}

} // namespace nlm

// Usage example:
// #include "EnhancedConfiguration.hpp"
//
// int main() {
//     // Initialize enhanced configuration system
//     initializeEnhancedConfiguration();
//
//     // Load configuration from file (recommended)
//     if (!loadEnhancedConfiguration("config.cfg")) {
//         std::cerr << "Configuration validation failed!\n";
//         std::cerr << generateEnhancedConfigurationReport();
//         return 1;
//     }
//
//     // Load from command line (alternative)
//     // if (!loadEnhancedConfigurationFromCommandLine(argc, argv)) {
//     //     std::cerr << "Command line configuration validation failed!\n";
//     //     return 1;
//     // }
//
//     // Save enhanced configuration
//     saveEnhancedConfiguration("enhanced_config.cfg");
//
//     // Generate and display configuration report
//     std::cout << generateEnhancedConfigurationReport();
//
//     return 0;
// }