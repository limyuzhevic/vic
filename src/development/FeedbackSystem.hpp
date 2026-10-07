// Enhanced Error Handling and User Feedback System
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// Error categories for better classification
enum class ErrorCategory {
    ConfigurationError,
    InitializationError,
    SimulationError,
    NetworkError,
    PlasticityError,
    MemoryError,
    InputError,
    OutputError,
    SystemError,
    UserError
};

// Detailed error information
struct ErrorInfo {
    ErrorCategory category;
    std::string message;
    std::string details;
    std::string suggestion;
    std::chrono::system_clock::time_point timestamp;
    int errorCode;
    bool isRecoverable;
    
    ErrorInfo() : category(ErrorCategory::SystemError), message(""), details(""), 
                  suggestions(""), timestamp(std::chrono::system_clock::now()),
                  errorCode(0), isRecoverable(false) {}
};

class ErrorHandler {
public:
    ErrorHandler();
    ~ErrorHandler();
    
    // Report an error with automatic context
    void reportError(ErrorCategory category, const std::string& message, 
                    const std::string& details = "", int errorCode = 0);
    
    // User-friendly error reporting
    void reportUserError(const std::string& message, const std::string& suggestion = "");
    void reportSystemError(const std::string& message, const std::string& details = "");
    
    // Error recovery
    bool recoverFromError(size_t errorIndex);
    void clearErrors();
    bool hasErrors() const;
    size_t getErrorCount() const;
    
    // Get formatted error messages
    std::string getErrorSummary() const;
    std::string getDetailedErrorReport() const;
    std::vector<ErrorInfo> getErrors() const;
    
    // Error classification
    ErrorCategory classifyError(const std::string& message) const;
    std::string getErrorCategoryName(ErrorCategory category) const;
    
    // Configuration validation
    bool validateConfiguration(const std::string& key, const std::string& value) const;
    std::string getConfigurationSuggestions(const std::string& key) const;
    
    // User feedback
    void showProgress(const std::string& message, bool showPercentage = false, 
                     double percentage = 0.0);
    void showSuccess(const std::string& message);
    void showWarning(const std::string& message);
    void showInfo(const std::string& message);
    
    // Interactive user feedback
    bool askYesNo(const std::string& question);
    std::string askForInput(const std::string& prompt);
    void showHelp(const std::string& topic = "");
    
    // Performance feedback
    void startPerformanceTimer(const std::string& operation);
    void endPerformanceTimer(const std::string& operation);
    void showPerformanceReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class UserFeedback {
public:
    UserFeedback();
    ~UserFeedback();
    
    void initialize(bool verbose = false, bool interactive = true);
    
    // Status reporting
    void reportOperationStart(const std::string& operation);
    void reportOperationProgress(const std::string& operation, double progress);
    void reportOperationComplete(const std::string& operation, bool success);
    
    // Network state feedback
    void reportNetworkStatus(const class Brain& brain);
    void reportLearningProgress(const class Brain& brain);
    void reportPlasticityChanges(const class Brain& brain);
    
    // Experiment feedback
    void reportExperimentStart(const std::string& experimentName);
    void reportExperimentProgress(const std::string& experimentName, 
                                 size_t currentStep, size_t totalSteps);
    void reportExperimentComplete(const std::string& experimentName,
                                 bool success, const std::string& results);
    
    // Configuration feedback
    void reportConfigurationChange(const std::string& key, const std::string& oldValue, 
                                  const std::string& newValue);
    void reportConfigurationValidation(const std::string& key, bool isValid);
    
    // Error feedback
    void reportError(const ErrorInfo& error);
    void reportWarning(const std::string& message);
    
    // Output formatting
    void setOutputLevel(LogLevel level);
    void enableColoredOutput(bool enable);
    void setOutputFormat(const std::string& format); // "plain", "json", "detailed"
    
    // File output
    void enableFileOutput(const std::string& directory);
    void disableFileOutput();
    bool saveSessionReport(const std::string& filename);
    
    // Session management
    void startSession(const std::string& sessionName);
    void endSession();
    std::string getCurrentSessionName() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

class ProgressReporter {
public:
    ProgressReporter();
    ~ProgressReporter();
    
    void startProgress(const std::string& description, size_t totalSteps);
    void advanceProgress(size_t steps = 1);
    void completeProgress(const std::string& finalMessage = "");
    void cancelProgress();
    
    bool isActive() const;
    double getProgressPercentage() const;
    std::string getCurrentStatus() const;
    
    // Progress styles
    void setProgressStyle(const std::string& style); // "bar", "spinner", "percentage"
    void setProgressWidth(size_t width);
    
    // Output
    void displayProgress();
    std::string getProgressDisplay() const;
    
    // Callbacks
    void setProgressCallback(std::function<void(double)> callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Utility functions for error handling and feedback
namespace FeedbackUtils {
    // Error formatting
    std::string formatError(const ErrorInfo& error);
    std::string formatErrorCategory(ErrorCategory category);
    std::string getErrorSuggestion(ErrorCategory category);
    
    // Progress formatting
    std::string formatProgress(double percentage, const std::string& description = "");
    std::string formatTime(double seconds);
    std::string formatBytes(size_t bytes);
    
    // Status reporting
    std::string getNetworkStatusString(const class Brain& brain);
    std::string getPlasticityStatusString(const class Brain& brain);
    std::string getLearningStatusString(const class Brain& brain);
    
    // Output formatting
    std::string escapeForJSON(const std::string& text);
    std::string escapeForHTML(const std::string& text);
    
    // File operations
    bool ensureDirectoryExists(const std::string& path);
    std::string getTimestampString(const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
}

} // namespace nlm
