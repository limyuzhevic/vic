#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace nlm {

// Error categories
enum class ErrorCategory {
    Configuration,
    FileIO,
    Memory,
    Neural,
    Plasticity,
    Development,
    Neuromodulation,
    MemorySystem,
    Prediction,
    Cognition,
    System,
    UserInput
};

// Severity levels
enum class ErrorSeverity {
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

// Enhanced error information
struct ErrorInfo {
    ErrorCategory category;
    ErrorSeverity severity;
    std::string message;
    std::string context;
    std::string file;
    int line;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> suggestions;
    std::map<std::string, std::string> details;
    
    ErrorInfo(ErrorCategory cat, ErrorSeverity sev, const std::string& msg,
             const std::string& ctx = "", const std::string& file = "",
             int line = 0)
        : category(cat), severity(sev), message(msg), context(ctx),
          file(file), line(line), timestamp(std::chrono::system_clock::now()) {}
    
    std::string getFormattedMessage() const {
        std::ostringstream oss;
        oss << "[" << static_cast<int>(severity) << "] ";
        oss << "[" << static_cast<int>(category) << "] ";
        if (!file.empty()) oss << file << ":" << line << " ";
        oss << message;
        if (!context.empty()) oss << " (Context: " << context << ")";
        return oss.str();
    }
};

// Error reporting system
class ErrorReporter {
public:
    ErrorReporter();
    ~ErrorReporter();
    
    // Add error
    void addError(ErrorCategory category, ErrorSeverity severity,
                  const std::string& message, const std::string& context = "",
                  const std::string& file = "", int line = 0);
    
    // Add suggestion
    void addSuggestion(const std::string& suggestion);
    
    // Add detail
    void addDetail(const std::string& key, const std::string& value);
    
    // Get all errors
    const std::vector<ErrorInfo>& getErrors() const { return errors_; }
    
    // Get errors by category
    std::vector<ErrorInfo> getErrorsByCategory(ErrorCategory category) const;
    
    // Get errors by severity
    std::vector<ErrorInfo> getErrorsBySeverity(ErrorSeverity severity) const;
    
    // Check if there are any errors
    bool hasErrors() const { return !errors_.empty(); }
    
    // Get error count by severity
    size_t getErrorCount(ErrorSeverity severity) const;
    
    // Clear all errors
    void clear() { errors_.clear(); }
    
    // Generate report
    std::string generateReport() const;
    std::string generateHTMLReport() const;
    std::string generateJSONReport() const;
    
    // Save report to file
    bool saveReport(const std::string& filename, const std::string& format = "text") const;
    
    // Get summary
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<ErrorInfo> errors_;
    std::vector<std::string> suggestions_;
    std::map<std::string, std::string> details_;
};

// Enhanced exception handling
class NLMException : public std::runtime_error {
public:
    NLMException(ErrorCategory category, ErrorSeverity severity,
                 const std::string& message, const std::string& context = "",
                 const std::string& file = "", int line = 0)
        : std::runtime_error(formatMessage(message, context)),
          category_(category), severity_(severity), file_(file), line_(line) {}
    
    ErrorCategory getCategory() const { return category_; }
    ErrorSeverity getSeverity() const { return severity_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }
    const std::string& getContext() const { return context_; }
    
    std::string getFormattedMessage() const {
        return std::runtime_error::what();
    }
    
private:
    ErrorCategory category_;
    ErrorSeverity severity_;
    std::string file_;
    int line_;
    std::string context_;
    
    static std::string formatMessage(const std::string& message,
                                    const std::string& context);
};

// Enhanced configuration error handling
class ConfigErrorHandler {
public:
    static bool validateConfigFile(const std::string& filepath);
    static std::vector<std::string> validateConfigFileSyntax(const std::string& content);
    static std::vector<std::string> validateConfigValues(const Config& config);
    static std::string getConfigFileErrorSuggestion(const std::string& error);
};

// Neural computation error handling
class NeuralErrorHandler {
public:
    static bool validateNeuronParameters(double v, double thresh, double leakConductance);
    static std::string getNeuronErrorSuggestion(double v, double thresh, double leakConductance);
    
    static bool validateSynapseParameters(double weight, double delay);
    static std::string getSynapseErrorSuggestion(double weight, double delay);
    
    static bool validateSpikeParameters(const class Neuron& neuron);
    static std::string getSpikeErrorSuggestion(const class Neuron& neuron);
};

// Enhanced logging with error handling
class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger();
    
    void log(ErrorCategory category, ErrorSeverity severity,
             const std::string& message, const std::string& context = "",
             const std::string& file = "", int line = 0);
    
    void logInfo(const std::string& message, const std::string& context = "");
    void logWarning(const std::string& message, const std::string& context = "");
    void logError(const std::string& message, const std::string& context = "");
    void logCritical(const std::string& message, const std::string& context = "");
    
    void logNeural(const std::string& message, const std::string& context = "");
    void logPlasticity(const std::string& message, const std::string& context = "");
    void logMemory(const std::string& message, const std::string& context = "");
    void logSystem(const std::string& message, const std::string& context = "");
    
    // Get error reporter
    const ErrorReporter& getErrorReporter() const { return error_reporter_; }
    
    // Clear errors
    void clearErrors() { error_reporter_.clear(); }
    
    // Get error count
    size_t getErrorCount() const { return error_reporter_.getErrors().size(); }
    
    // Check if system is healthy
    bool isHealthy() const { return !error_reporter_.hasErrors(); }
    
    // Generate system health report
    std::string generateHealthReport() const;
    
    // Print error summary
    void printErrorSummary() const;
    
private:
    ErrorReporter error_reporter_;
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global error reporter instance
extern EnhancedLogger& getGlobalLogger();

// Error handling macros
#define NLM_LOG_ERROR_WITH_CATEGORY(category, message, context) \
    do { \
        auto& logger = getGlobalLogger(); \
        logger.log(category, ErrorSeverity::Error, message, context, __FILE__, __LINE__); \
    } while(0)

#define NLM_LOG_WARNING_WITH_CATEGORY(category, message, context) \
    do { \
        auto& logger = getGlobalLogger(); \
        logger.log(category, ErrorSeverity::Warning, message, context, __FILE__, __LINE__); \
    } while(0)

#define NLM_LOG_INFO_WITH_CATEGORY(category, message, context) \
    do { \
        auto& logger = getGlobalLogger(); \
        logger.log(category, ErrorSeverity::Info, message, context, __FILE__, __LINE__); \
    } while(0)

#define NLM_THROW_ERROR(category, severity, message, context) \
    do { \
        throw NLMException(category, severity, message, context, __FILE__, __LINE__); \
    } while(0)

#define NLM_CHECK(condition, category, severity, message, context) \
    do { \
        if (!(condition)) { \
            NLM_THROW_ERROR(category, severity, message, context); \
        } \
    } while(0)

} // namespace nlm