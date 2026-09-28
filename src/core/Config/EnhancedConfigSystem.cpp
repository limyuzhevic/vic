#include "Config.hpp"
#include "ValidationFramework.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>

namespace nlm {

// Global instances
static ValidationErrorCollector globalErrorCollector;
static ConfigurationSchema globalSchema;

// EnhancedConfigSystem implementation
class EnhancedConfigSystem {
public:
    EnhancedConfigSystem(Config& config) : config(config) {
        initializeParameterConstants();
        initializeSecuritySettings();
        setupSecurityAuditTrail();
    }

    // Parameter Constants Definition
    void initializeParameterConstants() {
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

    void initializeSecuritySettings() {
        config.setBool("SECURITY_CONST_INPUT_SANITIZATION_ENABLED", true, ConfigSource::Default);
        config.setBool("SECURITY_CONST_AUDIT_TRAIL_ENABLED", true, ConfigSource::Default);
        config.setBool("SECURITY_CONST_FILE_PERMISSIONS_ENFORCED", true, ConfigSource::Default);
        config.setBool("SECURITY_CONST_SENSITIVE_DATA_MASKING_ENABLED", true, ConfigSource::Default);
        config.setString("SECURITY_CONST_AUDIT_LOG_PATH", "/var/log/nlm/config_audits.log", ConfigSource::Default);
        config.setInt("SECURITY_CONST_MAX_AUDIT_ENTRIES", 10000, ConfigSource::Default);
    }

    void setupSecurityAuditTrail() {
        std::time_t now = std::time(nullptr);
        std::string timestamp = std::ctime(&now);
        // Initialize audit log if needed
        if (config.getBool("SECURITY_CONST_AUDIT_TRAIL_ENABLED").value_or(true)) {
            std::string auditPath = config.getString("SECURITY_CONST_AUDIT_LOG_PATH").value_or("/var/log/nlm/config_audits.log");
            // Ensure audit log directory exists
            std::string dir = auditPath.substr(0, auditPath.find_last_of('/'));
            if (!dir.empty()) {
                mkdir(dir.c_str(), 0755);
            }
        }
    }

    // Enhanced configuration validation
    bool validateConfiguration() {
        globalErrorCollector.clear();

        // Validate range constraints
        validateRangeConstraints();

        // Validate type constraints
        validateTypeConstraints();

        // Validate dependency constraints
        validateDependencyConstraints();

        // Validate consistency constraints
        validateConsistencyConstraints();

        // Validate security constraints
        validateSecurityConstraints();

        return !globalErrorCollector.hasErrors();
    }

    void validateRangeConstraints() {
        for (const auto& param : globalSchema.getParameters()) {
            auto valueOpt = config.getValue(param.name);
            if (!valueOpt) {
                continue; // Skip if parameter not set
            }

            const ConfigValue& value = *valueOpt;

            // Check if parameter has constraints
            if (param.constraints.find("min") != param.constraints.end()) {
                std::string minStr = param.constraints.at("min");
                if (!validateValue(value, minStr, "min")) {
                    globalErrorCollector.addError(
                        "Parameter '" + param.name + "' is below minimum value",
                        ConfigValidationException::Severity::High,
                        param.name,
                        "Minimum allowed value: " + minStr
                    );
                }
            }

            if (param.constraints.find("max") != param.constraints.end()) {
                std::string maxStr = param.constraints.at("max");
                if (!validateValue(value, maxStr, "max")) {
                    globalErrorCollector.addError(
                        "Parameter '" + param.name + "' is above maximum value",
                        ConfigValidationException::Severity::High,
                        param.name,
                        "Maximum allowed value: " + maxStr
                    );
                }
            }
        }
    }

    void validateTypeConstraints() {
        for (const auto& param : globalSchema.getParameters()) {
            auto valueOpt = config.getValue(param.name);
            if (!valueOpt) {
                continue; // Skip if parameter not set
            }

            // Basic type validation (can be enhanced)
            ConfigValueType valueType = getConfigValueType(*valueOpt);
            if (!isTypeCompatible(param.type, valueType)) {
                globalErrorCollector.addError(
                    "Parameter '" + param.name + "' has incompatible type",
                    ConfigValidationException::Severity::High,
                    param.name,
                    "Expected type: " + param.type + ", got: " + valueTypeToString(valueType)
                );
            }
        }
    }

    void validateDependencyConstraints() {
        // Validate parameter dependencies
        // Examples: working_memory_capacity must be >= working_memory_min
        //           simulation_timestep > 0

        if (config.has("working_memory_capacity")) {
            int capacity = config.getInt("working_memory_capacity").value_or(0);
            if (capacity < 10) {
                globalErrorCollector.addError(
                    "working_memory_capacity must be at least 10",
                    ConfigValidationException::Severity::Medium,
                    "working_memory_capacity",
                    "Consider increasing to 10 or more"
                );
            }
        }

        if (config.has("connection_probability")) {
            double prob = config.getDouble("connection_probability").value_or(-1.0);
            if (prob < 0.0 || prob > 1.0) {
                globalErrorCollector.addError(
                    "connection_probability must be between 0.0 and 1.0",
                    ConfigValidationException::Severity::High,
                    "connection_probability",
                    "Set value between 0.0 and 1.0"
                );
            }
        }

        if (config.has("timestep")) {
            double timestep = config.getDouble("timestep").value_or(-1.0);
            if (timestep <= 0.0) {
                globalErrorCollector.addError(
                    "timestep must be greater than 0.0",
                    ConfigValidationException::Severity::High,
                    "timestep",
                    "Set timestep to a positive value"
                );
            }
        }
    }

    void validateConsistencyConstraints() {
        // Validate consistency between related parameters

        if (config.has("neuron_count") && config.has("region_count")) {
            int neuronCount = config.getInt("neuron_count").value_or(0);
            int regionCount = config.getInt("region_count").value_or(0);

            if (neuronCount < regionCount * 100) {
                globalErrorCollector.addError(
                    "Neuron count must be at least 100 times region count",
                    ConfigValidationException::Severity::Medium,
                    "neuron_count, region_count",
                    "Consider increasing neuron_count or decreasing region_count"
                );
            }
        }

        if (config.has("plasticity_learning_rate") && config.has("learning_rate")) {
            double plasticityRate = config.getDouble("plasticity_learning_rate").value_or(-1.0);
            double globalRate = config.getDouble("learning_rate").value_or(-1.0);

            if (plasticityRate > globalRate * 10.0) {
                globalErrorCollector.addError(
                    "plasticity_learning_rate should not be more than 10x learning_rate",
                    ConfigValidationException::Severity::Medium,
                    "plasticity_learning_rate, learning_rate",
                    "Consider reducing plasticity_learning_rate"
                );
            }
        }
    }

    void validateSecurityConstraints() {
        for (const auto& rule : globalSchema.getSecurityRules()) {
            bool rulePassed = true;
            std::string ruleContext;

            if (rule.parameterName == "input_sanitization") {
                rulePassed = validateInputSanitization();
                ruleContext = "input_sanitization";
            }
            else if (rule.parameterName == "file_path_validation") {
                rulePassed = validateFilePathSecurity();
                ruleContext = "file_path_validation";
            }
            else if (rule.parameterName == "access_control") {
                rulePassed = validateAccessControl();
                ruleContext = "access_control";
            }

            if (!rulePassed) {
                globalErrorCollector.addError(
                    rule.errorMessage,
                    rule.severity,
                    ruleContext,
                    rule.suggestion
                );
            }
        }
    }

    // Security validation methods
    bool validateInputSanitization() {
        for (const auto& entry : config.getEntries()) {
            if (!isSafeValue(entry.value)) {
                logSecurityEvent("UNSAFE_INPUT", entry.key, "Potentially unsafe input detected");
                return false;
            }
        }
        return true;
    }

    bool validateFilePathSecurity() {
        for (const auto& entry : config.getEntries()) {
            if (isFilePath(entry.value)) {
                if (!isSafeFilePath(entry.value)) {
                    logSecurityEvent("UNSAFE_FILEPATH", entry.key, "Potentially unsafe file path");
                    return false;
                }
            }
        }
        return true;
    }

    bool validateAccessControl() {
        for (const auto& entry : config.getEntries()) {
            if (isFilePath(entry.value)) {
                if (!checkFilePermissions(entry.value)) {
                    logSecurityEvent("ACCESS_VIOLATION", entry.key, "File permission violation");
                    return false;
                }
            }
        }
        return true;
    }

    // Enhanced validation helper methods
    bool validateValue(const ConfigValue& value, const std::string& constraint, const std::string& type) {
        try {
            if (type == "min") {
                double minVal = std::stod(constraint);
                if (std::holds_alternative<double>(value)) {
                    return std::get<double>(value) >= minVal;
                } else if (std::holds_alternative<int>(value)) {
                    return static_cast<double>(std::get<int>(value)) >= minVal;
                } else if (std::holds_alternative<int64_t>(value)) {
                    return static_cast<double>(std::get<int64_t>(value)) >= minVal;
                }
            } else if (type == "max") {
                double maxVal = std::stod(constraint);
                if (std::holds_alternative<double>(value)) {
                    return std::get<double>(value) <= maxVal;
                } else if (std::holds_alternative<int>(value)) {
                    return static_cast<double>(std::get<int>(value)) <= maxVal;
                } else if (std::holds_alternative<int64_t>(value)) {
                    return static_cast<double>(std::get<int64_t>(value)) <= maxVal;
                }
            }
        } catch (...) {
            return false;
        }
        return false;
    }

    ConfigValueType getConfigValueType(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return ConfigValueType::Int;
        if (std::holds_alternative<int64_t>(value)) return ConfigValueType::Int64;
        if (std::holds_alternative<double>(value)) return ConfigValueType::Double;
        if (std::holds_alternative<bool>(value)) return ConfigValueType::Bool;
        if (std::holds_alternative<std::string>(value)) return ConfigValueType::String;
        if (std::holds_alternative<std::vector<int>>(value)) return ConfigValueType::IntVector;
        if (std::holds_alternative<std::vector<double>>(value)) return ConfigValueType::DoubleVector;
        if (std::holds_alternative<std::vector<std::string>>(value)) return ConfigValueType::StringVector;
        return ConfigValueType::Unknown;
    }

    bool isTypeCompatible(const std::string& expectedType, ConfigValueType actualType) {
        if (expectedType == "int64_t" && actualType == ConfigValueType::Int64) return true;
        if (expectedType == "double" && actualType == ConfigValueType::Double) return true;
        if (expectedType == "bool" && actualType == ConfigValueType::Bool) return true;
        if (expectedType == "string" && actualType == ConfigValueType::String) return true;
        if (expectedType == "int" && actualType == ConfigValueType::Int) return true;
        if (expectedType == "vector<int>" && actualType == ConfigValueType::IntVector) return true;
        if (expectedType == "vector<double>" && actualType == ConfigValueType::DoubleVector) return true;
        if (expectedType == "vector<string>" && actualType == ConfigValueType::StringVector) return true;
        return false;
    }

    std::string valueTypeToString(ConfigValueType type) {
        switch (type) {
            case ConfigValueType::Int: return "int";
            case ConfigValueType::Int64: return "int64_t";
            case ConfigValueType::Double: return "double";
            case ConfigValueType::Bool: return "bool";
            case ConfigValueType::String: return "string";
            case ConfigValueType::IntVector: return "vector<int>";
            case ConfigValueType::DoubleVector: return "vector<double>";
            case ConfigValueType::StringVector: return "vector<string>";
            default: return "unknown";
        }
    }

    // Security helper methods
    bool isSafeValue(const ConfigValue& value) {
        return std::visit([](const auto& val) -> bool {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                return isSafeString(val);
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                for (const auto& s : val) {
                    if (!isSafeString(s)) return false;
                }
                return true;
            } else {
                return true; // Non-string types are considered safe
            }
        }, value);
    }

    bool isSafeString(const std::string& str) {
        // Check for potentially dangerous patterns
        if (str.find("<script>") != std::string::npos) return false;
        if (str.find("javascript:") != std::string::npos) return false;
        if (str.find("data:") != std::string::npos) return false;
        if (str.find("eval(") != std::string::npos) return false;
        if (str.find("exec(") != std::string::npos) return false;

        // Check for directory traversal attempts
        if (str.find("..") != std::string::npos && str.find("/") != std::string::npos) return false;

        return true;
    }

    bool isFilePath(const ConfigValue& value) {
        return std::visit([](const auto& val) -> bool {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                return isPathString(val);
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                for (const auto& s : val) {
                    if (isPathString(s)) return true;
                }
                return false;
            } else {
                return false;
            }
        }, value);
    }

    bool isPathString(const std::string& str) {
        // Simple heuristic for detecting file paths
        return str.find('/') != std::string::npos ||
               str.find('.') != std::string::npos ||
               str.find('\\') != std::string::npos;
    }

    bool isSafeFilePath(const ConfigValue& value) {
        return std::visit([](const auto& val) -> bool {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                return isSafePathString(val);
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                for (const auto& s : val) {
                    if (!isSafePathString(s)) return false;
                }
                return true;
            } else {
                return false;
            }
        }, value);
    }

    bool isSafePathString(const std::string& path) {
        // Check for directory traversal attempts
        if (path.find("..") != std::string::npos) return false;

        // Check for absolute paths (can be customized based on security requirements)
        if (path.find("/") == 0 || (path.length() >= 2 && path[1] == ':' && path.find('\\') != std::string::npos)) {
            // Absolute paths are generally allowed but should be validated
            return validateAbsolutePath(path);
        }

        // Relative paths are allowed
        return true;
    }

    bool validateAbsolutePath(const std::string& path) {
        struct stat path_stat;
        if (stat(path.c_str(), &path_stat) == 0) {
            // Path exists, check if it's a directory
            if (S_ISDIR(path_stat.st_mode)) {
                // Directory access permissions
                return access(path.c_str(), R_OK | X_OK) == 0;
            } else {
                // File access permissions
                return access(path.c_str(), R_OK) == 0;
            }
        }
        // Path doesn't exist, check if parent directory is accessible
        size_t last_slash = path.find_last_of("/");
        if (last_slash != std::string::npos) {
            std::string parent_dir = path.substr(0, last_slash);
            return access(parent_dir.c_str(), W_OK) == 0;
        }
        return false;
    }

    bool checkFilePermissions(const ConfigValue& value) {
        return std::visit([](const auto& val) -> bool {
            if constexpr (std::is_same_v<decltype(val), std::string>) {
                return checkFilePermissions(val);
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                for (const auto& s : val) {
                    if (!checkFilePermissions(s)) return false;
                }
                return true;
            } else {
                return true;
            }
        }, value);
    }

    bool checkFilePermissions(const std::string& path) {
        struct stat path_stat;
        if (stat(path.c_str(), &path_stat) != 0) {
            return false; // Path doesn't exist
        }

        // Check file permissions based on user role
        mode_t mode = path_stat.st_mode;

        // For configuration files, we typically want read-only access for non-admin users
        if (!isAdminUser()) {
            if (mode & (S_IWUSR | S_IWGRP | S_IWOTH)) {
                return false; // File is writable by others
            }
        }

        return true;
    }

    bool isAdminUser() {
        // Simple check - can be enhanced based on system requirements
        return geteuid() == 0; // Check if running as root
    }

    // Configuration security methods
    void logSecurityEvent(const std::string& eventType, const std::string& details, const std::string& message) {
        std::string timestamp = getCurrentTimestamp();
        std::string logEntry = "[" + timestamp + "] [SECURITY] " + eventType + ": " + details + " - " + message + "\n";

        std::string auditPath = config.getString("SECURITY_CONST_AUDIT_LOG_PATH").value_or("/var/log/nlm/config_audits.log");

        std::ofstream auditFile(auditPath, std::ios::app);
        if (auditFile.is_open()) {
            auditFile << logEntry;
            auditFile.close();
        }
    }

    // Configuration statistics and reporting
    std::string generateValidationReport() {
        std::ostringstream oss;
        oss << "=== Enhanced NLM Configuration Validation Report ===\n\n";
        oss << "Generated: " << getCurrentTimestamp() << "\n\n";

        // Parameter statistics
        oss << "Parameter Statistics:\n";
        oss << "  Total parameters: " << globalSchema.getParameters().size() << "\n";
        oss << "  Configured parameters: " << config.getEntries().size() << "\n";
        oss << "  Missing parameters: " << (globalSchema.getParameters().size() - config.getEntries().size()) << "\n\n";

        // Validation results
        oss << "Validation Results:\n";
        oss << "  Validation passed: " << (validateConfiguration() ? "Yes" : "No") << "\n";
        oss << "  Validation errors: " << globalErrorCollector.getErrors().size() << "\n\n";

        // Security status
        oss << "Security Status:\n";
        oss << "  Input sanitization: " << (validateInputSanitization() ? "Enabled" : "Disabled") << "\n";
        oss << "  File permission checks: " << (validateFilePathSecurity() ? "Enabled" : "Disabled") << "\n";
        oss << "  Access control: " << (validateAccessControl() ? "Enabled" : "Disabled") << "\n\n";

        // Configuration source distribution
        oss << "Configuration Source Distribution:\n";
        for (const auto& source : {ConfigSource::Default, ConfigSource::File, ConfigSource::CommandLine, ConfigSource::Runtime, ConfigSource::Environment}) {
            std::string sourceName;
            switch (source) {
                case ConfigSource::Default: sourceName = "Default"; break;
                case ConfigSource::File: sourceName = "File"; break;
                case ConfigSource::CommandLine: sourceName = "Command Line"; break;
                case ConfigSource::Runtime: sourceName = "Runtime"; break;
                case ConfigSource::Environment: sourceName = "Environment"; break;
            }

            int count = 0;
            for (const auto& entry : config.getEntries()) {
                if (entry.source == source) count++;
            }

            oss << "  " << sourceName << ": " << count << " parameters\n";
        }

        oss << "\n" << globalErrorCollector.getReport() << "\n";

        return oss.str();
    }

    // Public interface for enhanced configuration system
    void initialize() {
        // Initialize all constant parameters
        initializeParameterConstants();

        // Setup security infrastructure
        initializeSecuritySettings();
        setupSecurityAuditTrail();

        // Perform initial validation
        validateConfiguration();
    }

    bool performValidation() {
        return validateConfiguration();
    }

    std::string getValidationReport() {
        return generateValidationReport();
    }

    void logValidationResult(const std::string& operation, bool success, const std::string& details) {
        std::string timestamp = getCurrentTimestamp();
        std::string logEntry = "[" + timestamp + "] [VALIDATION] " + operation + ": " + (success ? "PASS" : "FAIL") + " - " + details + "\n";

        // Log to internal validation log
        std::string validationLogPath = "/tmp/nlm_validation.log";
        std::ofstream validationFile(validationLogPath, std::ios::app);
        if (validationFile.is_open()) {
            validationFile << logEntry;
            validationFile.close();
        }
    }

private:
    Config& config;

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        char buf[100];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_buf);
        return std::string(buf);
    }

    const std::vector<ConfigEntry>& getConfigEntries() const {
        return config.getEntries();
    }
};

} // namespace nlm