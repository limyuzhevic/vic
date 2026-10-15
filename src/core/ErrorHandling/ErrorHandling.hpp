// NLM Error Handling and Validation System
// 
// This provides comprehensive error handling and validation for NLM:
// - Error types and severity levels
// - Validation functions for parameters
// - Error recovery mechanisms
// - Detailed error reporting
// - Performance monitoring
// 
// Usage:
//   #include "ErrorHandling.hpp"
//   
//   if (!validateConfiguration(config)) {
//       printErrors();
//       return 1;
//   }
//   
//   try {
//       brain->initialize();
//   } catch (const NLMException& e) {
//       logError(e);
//       return 1;
//   }

#ifndef NLM_ERROR_HANDLING_HPP
#define NLM_ERROR_HANDLING_HPP

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <map>
#include <algorithm>
#include <chrono>
#include <set>
#include <unordered_map>

namespace nlm {

// Error severity levels
enum class ErrorSeverity {
    Info = 0,
    Warning = 1,
    Error = 2,
    Critical = 3,
    Fatal = 4
};

// Error categories
enum class ErrorCategory {
    Configuration = 0,
    Memory = 1,
    Simulation = 2,
    Plasticity = 3,
    Development = 4,
    Neuromodulation = 5,
    Cognition = 6,
    I_O = 7,
    Checkpoint = 8,
    System = 9
};

// Custom exception class
class NLMException : public std::runtime_error {
public:
    ErrorSeverity severity;
    ErrorCategory category;
    std::string component;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> context;
    mutable std::string formattedMessage;
    
    NLMException(ErrorSeverity sev, ErrorCategory cat, const std::string& msg,
                 const std::string& comp = "Unknown")
        : std::runtime_error(msg), severity(sev), category(cat), component(comp) {
        timestamp = std::chrono::system_clock::now();
        formattedMessage = msg;
    }
    
    NLMException& addContext(const std::string& ctx) {
        context.push_back(ctx);
        return *this;
    }
    
    std::string getFullMessage() const {
        std::ostringstream oss;
        oss << "[NLM " << static_cast<int>(category) << "] " << component << ": ";
        oss << what();
        if (!context.empty()) {
            oss << " (Context: ";
            for (size_t i = 0; i < context.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << context[i];
            }
            oss << "; Time: " << std::chrono::system_clock::to_time_t(timestamp)
                << ")";
        }
        return oss.str();
    }
};

// Validation result structure
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::chrono::system_clock::time_point validationTime;
    
    ValidationResult() : isValid(true), validationTime(std::chrono::system_clock::now()) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        isValid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    std::string getSummary() const {
        std::ostringstream oss;
        if (isValid) {
            oss << "Validation PASSED";
            if (!warnings.empty()) {
                oss << " (" << warnings.size() << " warnings)";
            }
        } else {
            oss << "Validation FAILED";
            if (!errors.empty()) {
                oss << " (" << errors.size() << " errors, " << warnings.size() << " warnings)";
            }
        }
        return oss.str();
    }
};

// Configuration validator
class ConfigurationValidator {
public:
    struct ParameterRules {
        std::string type; // int, float, string, bool
        std::string range;
        std::string description;
        bool required;
        std::string defaultValue;
        std::function<bool(const std::string&)> customValidator;
    };
    
private:
    std::map<std::string, ParameterRules> parameterRules;
    std::vector<std::string> requiredSections;
    std::map<std::string, std::set<std::string>> sectionDependencies;
    
public:
    ConfigurationValidator() {
        initializeDefaultRules();
    }
    
    void initializeDefaultRules() {
        // Basic configuration
        parameterRules["random_seed"] = {"int", "0-2147483647", "Random seed for reproducible experiments", true, "42"};
        parameterRules["simulation_timestep"] = {"float", "0.0001-1.0", "Simulation timestep in seconds", true, "0.001"};
        parameterRules["neuron_count"] = {"int", "10-100000", "Number of neurons in the brain", true, "1000"};
        parameterRules["region_count"] = {"int", "1-1000", "Number of brain regions", true, "1"};
        parameterRules["connection_probability"] = {"float", "0.0-1.0", "Connection probability for random connectivity", true, "0.1"};
        
        // Plasticity
        parameterRules["plasticity_learning_rate"] = {"float", "0.0-1.0", "Plasticity learning rate", false, "0.01"};
        parameterRules["stdp_ltp_weight"] = {"float", "0.0-1.0", "STDP LTP weight", false, "0.01"};
        parameterRules["stdp_ltd_weight"] = {"float", "0.0-1.0", "STDP LTD weight", false, "0.012"};
        parameterRules["stdp_time_constant"] = {"float", "1.0-1000.0", "STDP time constant in ms", false, "20.0"};
        
        // Development
        parameterRules["synaptogenesis_rate"] = {"float", "0.000001-0.1", "Synaptogenesis rate", false, "0.001"};
        parameterRules["pruning_rate"] = {"float", "0.00000001-0.01", "Pruning rate", false, "0.00001"};
        
        // Neuromodulation
        parameterRules["dopamine_scale"] = {"float", "0.0-10.0", "Dopamine modulation scale", false, "1.0"};
        parameterRules["reward_discount_factor"] = {"float", "0.0-1.0", "Reward discount factor", false, "0.99"};
        
        // Environment
        parameterRules["environment_width"] = {"int", "1-1000", "Environment width", false, "10"};
        parameterRules["environment_height"] = {"int", "1-1000", "Environment height", false, "10"};
        
        // Logging
        parameterRules["log_level"] = {"string", "DEBUG|INFO|WARNING|ERROR|FATAL", "Log level", true, "INFO"};
        
        // Simulation
        parameterRules["max_simulation_steps"] = {"int", "1-1000000", "Maximum simulation steps", false, "10000"};
        parameterRules["simulation_time_limit"] = {"float", "0.0-1000000.0", "Simulation time limit in seconds", false, "0.0"};
        
        requiredSections = {"basic", "plasticity", "development", "neuromodulation", "environment", "logging", "simulation"};
    }
    
    void addParameter(const std::string& key, const ParameterRules& rules) {
        parameterRules[key] = rules;
    }
    
    ValidationResult validate(const std::shared_ptr<Config>& config) const {
        ValidationResult result;
        
        // Validate all registered parameters
        for (const auto& pair : parameterRules) {
            const std::string& key = pair.first;
            const ParameterRules& rules = pair.second;
            
            if (rules.required && !config->has(key)) {
                result.addError("Missing required parameter: " + key);
            }
            
            if (config->has(key)) {
                std::string value;
                if (rules.type == "int") {
                    auto val = config->getOr<int>(key, std::stoi(rules.defaultValue));
                    value = std::to_string(val);
                } else if (rules.type == "float") {
                    auto val = config->getOr<float>(key, std::stof(rules.defaultValue));
                    value = std::to_string(val);
                } else if (rules.type == "string") {
                    auto val = config->getOr<std::string>(key, rules.defaultValue);
                    value = val;
                } else if (rules.type == "bool") {
                    auto val = config->getOr<bool>(key, rules.defaultValue == "true");
                    value = rules.defaultValue == "true" ? "true" : "false";
                }
                
                // Validate type
                if (!validateType(key, value, rules.type)) {
                    result.addError("Invalid type for parameter " + key + ": " + value + " (expected " + rules.type + ")");
                }
                
                // Validate range
                if (!rules.range.empty() && !validateRange(key, value, rules.range)) {
                    result.addError("Parameter " + key + " value " + value + " outside range " + rules.range);
                }
                
                // Validate string values
                if (rules.type == "string") {
                    if (!validateString(key, value, rules.range)) {
                        result.addError("Invalid value for string parameter " + key + ": " + value);
                    }
                }
                
                // Apply custom validator
                if (rules.customValidator && !rules.customValidator(value)) {
                    result.addError("Custom validation failed for parameter " + key);
                }
            }
        }
        
        // Validate section consistency
        validateSectionConsistency(config, result);
        
        return result;
    }
    
private:
    bool validateType(const std::string& key, const std::string& value, const std::string& type) const {
        if (type == "int") {
            return std::all_of(value.begin(), value.end(), ::isdigit) ||
                   (value[0] == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit));
        } else if (type == "float") {
            try {
                std::stod(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (type == "bool") {
            return value == "true" || value == "false";
        }
        return !value.empty();
    }
    
    bool validateRange(const std::string& key, const std::string& value, const std::string& range) const {
        if (range.empty()) return true;
        
        if (range.find('-') != std::string::npos) {
            // Numeric range: min-max
            size_t dashPos = range.find('-');
            std::string minStr = range.substr(0, dashPos);
            std::string maxStr = range.substr(dashPos + 1);
            
            double valueNum = std::stod(value);
            double min = std::stod(minStr);
            double max = std::stod(maxStr);
            
            return valueNum >= min && valueNum <= max;
        } else if (range.find('|') != std::string::npos) {
            // Discrete values: val1|val2|val3
            std::string allowedValues = range;
            return std::any_of(allowedValues.begin(), allowedValues.end(), 
                             [&value](char c) { return c == '|'; });
        }
        return true;
    }
    
    bool validateString(const std::string& key, const std::string& value, const std::string& allowedValues) const {
        if (allowedValues.empty()) return true;
        
        if (allowedValues.find('|') != std::string::npos) {
            // Check if value is in allowed list
            std::string pattern = "|" + value + "|";
            return allowedValues.find(pattern) != std::string::npos;
        }
        return true;
    }
    
    void validateSectionConsistency(const std::shared_ptr<Config>& config, ValidationResult& result) const {
        // Check neuron_count >= region_count
        if (config->has("neuron_count") && config->has("region_count")) {
            auto neurons = config->getOr<size_t>("neuron_count", 0);
            auto regions = config->getOr<size_t>("region_count", 1);
            if (neurons < regions) {
                result.addWarning("neuron_count (" + std::to_string(neurons) + ") is less than region_count (" + std::to_string(regions) + ")");
            }
        }
        
        // Check connection_probability in valid range
        if (config->has("connection_probability")) {
            auto prob = config->getOr<float>("connection_probability", 0.0f);
            if (prob < 0.0f || prob > 1.0f) {
                result.addError("connection_probability (" + std::to_string(prob) + ") must be between 0.0 and 1.0");
            }
        }
        
        // Check memory allocation
        if (config->has("neuron_count") && config->has("memory_capacity")) {
            auto neurons = config->getOr<size_t>("neuron_count", 0);
            auto memory = config->getOr<float>("memory_capacity", 0.0f);
            if (memory < neurons * 0.01f) {
                result.addWarning("memory_capacity (" + std::to_string(memory) + ") may be insufficient for neuron_count (" + std::to_string(neurons) + ")");
            }
        }
    }
};

// Error reporter for comprehensive error management
class ErrorReporter {
public:
    struct ErrorReport {
        std::string timestamp;
        std::vector<std::string> infoMessages;
        std::vector<std::string> warningMessages;
        std::vector<std::string> errorMessages;
        std::vector<std::string> criticalMessages;
        std::chrono::milliseconds duration;
        std::string sessionId;
        bool saveToFile;
        std::string outputFile;
    };
    
private:
    std::vector<ErrorReport> errorHistory;
    std::mutex mutex;
    bool autoSave;
    std::string defaultOutputDir;
    std::map<ErrorSeverity, std::string> severityColors;
    std::map<ErrorCategory, std::string> categorySymbols;
    
public:
    ErrorReporter() : autoSave(true), defaultOutputDir("./error_reports") {
        initializeSymbolsAndColors();
    }
    
    void initializeSymbolsAndColors() {
        // Severity colors (ANSI escape codes)
        severityColors[ErrorSeverity::Info] = "\033[32m";      // Green
        severityColors[ErrorSeverity::Warning] = "\033[33m";  // Yellow
        severityColors[ErrorSeverity::Error] = "\033[31m";    // Red
        severityColors[ErrorSeverity::Critical] = "\033[35m"; // Magenta
        severityColors[ErrorSeverity::Fatal] = "\033[41m\033[37m"; // White on red background
        
        // Category symbols
        categorySymbols[ErrorCategory::Configuration] = "⚙️";
        categorySymbols[ErrorCategory::Memory] = "💾";
        categorySymbols[ErrorCategory::Simulation] = "⏱️";
        categorySymbols[ErrorCategory::Plasticity] = "🧠";
        categorySymbols[ErrorCategory::Development] = "📈";
        categorySymbols[ErrorCategory::Neuromodulation] = "🧬";
        categorySymbols[ErrorCategory::Cognition] = "🧩";
        categorySymbols[ErrorCategory::I_O] = "🔄";
        categorySymbols[ErrorCategory::Checkpoint] = "💾";
        categorySymbols[ErrorCategory::System] = "💻";
    }
    
    void setAutoSave(bool enabled) {
        std::lock_guard<std::mutex> lock(mutex);
        autoSave = enabled;
    }
    
    void setOutputDirectory(const std::string& dir) {
        std::lock_guard<std::mutex> lock(mutex);
        defaultOutputDir = dir;
    }
    
    void reportError(const NLMException& error) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Create error report entry
        ErrorReport report;
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        report.timestamp = std::string(std::ctime(&time_t_now));
        report.timestamp.erase(report.timestamp.find_last_not_of("\n\r") + 1);
        
        // Add error based on severity
        switch (error.severity) {
            case ErrorSeverity::Info:
                report.infoMessages.push_back(error.getFullMessage());
                break;
            case ErrorSeverity::Warning:
                report.warningMessages.push_back(error.getFullMessage());
                break;
            case ErrorSeverity::Error:
                report.errorMessages.push_back(error.getFullMessage());
                break;
            case ErrorSeverity::Critical:
                report.criticalMessages.push_back(error.getFullMessage());
                break;
            case ErrorSeverity::Fatal:
                report.criticalMessages.push_back(error.getFullMessage());
                break;
        }
        
        // Add to history
        errorHistory.push_back(report);
        
        // Auto-save if enabled
        if (autoSave) {
            saveToFile("error_report_" + std::to_string(errorHistory.size()) + ".json");
        }
        
        // Print to console
        printError(error);
    }
    
    void logValidationResult(const ValidationResult& result, const std::string& context = "") {
        std::lock_guard<std::mutex> lock(mutex);
        
        ErrorReport report;
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        report.timestamp = std::string(std::ctime(&time_t_now));
        report.timestamp.erase(report.timestamp.find_last_not_of("\n\r") + 1);
        
        std::string summary = "Validation " + result.getSummary();
        if (!context.empty()) {
            summary += " (Context: " + context + ")";
        }
        
        if (result.hasErrors()) {
            report.errorMessages.push_back(summary);
            for (const auto& error : result.errors) {
                report.errorMessages.push_back("  ERROR: " + error);
            }
        }
        
        if (result.hasWarnings()) {
            for (const auto& warning : result.warnings) {
                report.warningMessages.push_back("  WARNING: " + warning);
            }
        }
        
        errorHistory.push_back(report);
        
        // Print to console
        std::cout << "Validation " << result.getSummary() << std::endl;
        if (result.hasErrors()) {
            std::cout << "Errors:" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "  ERROR: " << error << std::endl;
            }
        }
        if (result.hasWarnings()) {
            std::cout << "Warnings:" << std::endl;
            for (const auto& warning : result.warnings) {
                std::cout << "  WARNING: " << warning << std::endl;
            }
        }
    }
    
    void saveToFile(const std::string& filename = "") {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::string filepath = filename;
        if (filepath.empty()) {
            filepath = defaultOutputDir + "/error_report_" + 
                      std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".json";
        }
        
        // Ensure directory exists
        std::string dir = defaultOutputDir;
        size_t lastSlash = dir.find_last_of("/");
        if (lastSlash != std::string::npos) {
            system(("mkdir -p " + dir).c_str());
        }
        
        // Write error report to file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open error report file: " << filepath << std::endl;
            return;
        }
        
        file << "{\n";
        file << "  \"timestamp\": \"" << errorHistory.back().timestamp << "\",\n";
        file << "  \"info_messages\": [";
        for (size_t i = 0; i < errorHistory.back().infoMessages.size(); ++i) {
            if (i > 0) file << ", ";
            file << "\"" << escapeJson(errorHistory.back().infoMessages[i]) << "\"";
        }
        file << "],\n";
        
        file << "  \"warning_messages\": [";
        for (size_t i = 0; i < errorHistory.back().warningMessages.size(); ++i) {
            if (i > 0) file << ", ";
            file << "\"" << escapeJson(errorHistory.back().warningMessages[i]) << "\"";
        }
        file << "],\n";
        
        file << "  \"error_messages\": [";
        for (size_t i = 0; i < errorHistory.back().errorMessages.size(); ++i) {
            if (i > 0) file << ", ";
            file << "\"" << escapeJson(errorHistory.back().errorMessages[i]) << "\"";
        }
        file << "],\n";
        
        file << "  \"critical_messages\": [";
        for (size_t i = 0; i < errorHistory.back().criticalMessages.size(); ++i) {
            if (i > 0) file << ", ";
            file << "\"" << escapeJson(errorHistory.back().criticalMessages[i]) << "\"";
        }
        file << "],\n";
        
        file << "  \"session_id\": \"session_" << errorHistory.size() << "\",\n";
        file << "  \"auto_saved\": " << (autoSave ? "true" : "false") << "\n";
        file << "}\n";
        
        file.close();
    }
    
    void printError(const NLMException& error) {
        std::string symbol = categorySymbols[error.category];
        std::string color = severityColors[error.severity];
        std::string reset = "\033[0m";
        
        std::cout << color << "[" << symbol << "] " << static_cast<int>(error.severity) 
                  << " " << static_cast<int>(error.category) << " " << error.component << ": " 
                  << error.what() << reset << std::endl;
        
        if (!error.context.empty()) {
            std::cout << "  Context:" << std::endl;
            for (const auto& ctx : error.context) {
                std::cout << "    - " << ctx << std::endl;
            }
        }
    }
    
    std::vector<ErrorReport> getErrorHistory() const {
        std::lock_guard<std::mutex> lock(mutex);
        return errorHistory;
    }
    
    void clearHistory() {
        std::lock_guard<std::mutex> lock(mutex);
        errorHistory.clear();
    }
    
private:
    std::string escapeJson(const std::string& input) const {
        std::ostringstream oss;
        for (char c : input) {
            switch (c) {
                case '\"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if (c < ' ' || c == '\x7f') {
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)(unsigned char)c;
                    } else {
                        oss << c;
                    }
            }
        }
        return oss.str();
    }
};

// Global error reporter instance
ErrorReporter& getErrorReporter() {
    static ErrorReporter reporter;
    return reporter;
}

// Convenience functions
inline void reportNLMError(ErrorSeverity severity, ErrorCategory category, 
                           const std::string& message, const std::string& component = "Unknown") {
    NLMException error(severity, category, message, component);
    getErrorReporter().reportError(error);
}

inline void reportValidationError(const ValidationResult& result, const std::string& context = "") {
    getErrorReporter().logValidationResult(result, context);
}

// Macro for easy error reporting
#define NLM_VALIDATE(config, validator) \
    do { \
        ConfigurationValidator validator; \
        ValidationResult validationResult = validator.validate(config); \
        if (validationResult.hasErrors()) { \
            reportValidationError(validationResult, #validator); \
            throw NLMException(ErrorSeverity::Error, ErrorCategory::Configuration, \
                               "Configuration validation failed", "ConfigurationValidator"); \
        } \
        if (validationResult.hasWarnings()) { \
            reportValidationError(validationResult, #validator); \
        } \
    } while (0)

// Macro for error reporting in specific components
#define NLM_THROW_ERROR(severity, category, component, message) \
    do { \
        NLMException error(severity, category, message, component); \
        getErrorReporter().reportError(error); \
        throw error; \
    } while (0)

// Macro for logging warnings
#define NLM_WARN(category, component, message) \
    do { \
        NLMException error(ErrorSeverity::Warning, category, message, component); \
        getErrorReporter().reportError(error); \
    } while (0)

// Macro for logging info
#define NLM_INFO(category, component, message) \
    do { \
        NLMException error(ErrorSeverity::Info, category, message, component); \
        getErrorReporter().reportError(error); \
    } while (0)

} // namespace nlm

#endif // NLM_ERROR_HANDLING_HPP