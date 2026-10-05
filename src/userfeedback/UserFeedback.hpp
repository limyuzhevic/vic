// Enhanced error handling and user feedback framework
// Provides comprehensive error management, user feedback, and progress reporting

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <fstream>
#include <sstream>

namespace nlm {

// Error types for different levels of severity
enum class ErrorLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL,
    SUCCESS
};

// Detailed error information
struct ErrorInfo {
    ErrorLevel level;
    std::string category;
    std::string message;
    std::string context;
    std::string timestamp;
    std::string suggestedAction;
    bool hasDetails = false;
    std::string technicalDetails;
    std::string recoveryInstructions;
    
    ErrorInfo() : level(ErrorLevel::INFO), hasDetails(false) {}
    
    ErrorInfo(ErrorLevel lvl, const std::string& msg, const std::string& ctx = "")
        : level(lvl), message(msg), context(ctx), hasDetails(false) {
        timestamp = getCurrentTimestamp();
    }
};

// User feedback system
class UserFeedback {
public:
    struct FeedbackOptions {
        bool showProgress = true;
        bool showErrors = true;
        bool showWarnings = true;
        bool showSuccess = true;
        bool useColorOutput = true;
        bool enableSound = false;
        std::string outputFormat = "detailed";
        bool enableInteractive = true;
        std::string theme = "default";
    };
    
    UserFeedback(const FeedbackOptions& options = FeedbackOptions()) : options_(options) {}
    
    // Display different types of feedback
    void showProgress(const std::string& message, float progress = -1.0f, bool force = false);
    void showInfo(const std::string& message, const std::string& context = "");
    void showWarning(const std::string& message, const std::string& context = "", const std::string& action = "");
    void showError(const std::string& message, const std::string& context = "", const std::string& action = "");
    void showFatal(const std::string& message, const std::string& context = "");
    void showSuccess(const std::string& message, const std::string& context = "");
    
    // Error handling
    bool handleError(const ErrorInfo& error);
    void registerErrorHandler(const std::function<void(const ErrorInfo&)>& handler);
    std::vector<ErrorInfo> getRecentErrors() const { return recentErrors_; }
    void clearRecentErrors() { recentErrors_.clear(); }
    
    // Configuration
    void setOptions(const FeedbackOptions& options) { options_ = options; }
    FeedbackOptions getOptions() const { return options_; }
    
    // Interaction
    bool askQuestion(const std::string& question, const std::vector<std::string>& options = {});
    std::string getInput(const std::string& prompt);
    
    // State management
    void startSession(const std::string& sessionName);
    void endSession();
    std::string getSessionInfo() const;
    
private:
    FeedbackOptions options_;
    std::vector<ErrorInfo> recentErrors_;
    std::vector<std::function<void(const ErrorInfo&)>> errorHandlers_;
    std::string currentSession_;
    std::chrono::system_clock::time_point sessionStart_;
    
    std::string getCurrentTimestamp() const;
    void formatMessage(std::stringstream& output, const ErrorInfo& error) const;
};

// Enhanced logging system
class EnhancedLogger {
public:
    struct LogConfig {
        std::string logFile = "nlm.log";
        bool enableConsoleOutput = true;
        bool enableFileOutput = true;
        bool enableStructuredLogging = false;
        bool enableLogRotation = true;
        size_t maxLogFileSize = 10 * 1024 * 1024; // 10 MB
        int maxLogFiles = 5;
        std::string logLevel = "INFO";
        bool includeTimestamps = true;
        bool includeThreadInfo = false;
        bool includeSourceInfo = true;
        bool enablePerformanceLogging = false;
    };
    
    EnhancedLogger(const LogConfig& config = LogConfig()) : config_(config) {}
    
    // Core logging functions
    void log(ErrorLevel level, const std::string& category, const std::string& message, const std::string& context = "");
    
    // Convenience functions
    void debug(const std::string& message, const std::string& category = "general", const std::string& context = "");
    void info(const std::string& message, const std::string& category = "general", const std::string& context = "");
    void warning(const std::string& message, const std::string& category = "general", const std::string& context = "", const std::string& action = "");
    void error(const std::string& message, const std::string& category = "general", const std::string& context = "", const std::string& action = "");
    void fatal(const std::string& message, const std::string& category = "general", const std::string& context = "");
    void success(const std::string& message, const std::string& category = "general", const std::string& context = "");
    
    // Log management
    void flush();
    void rotateLogs();
    void clearLogs();
    bool backupLogs(const std::string& backupDir);
    
    // Configuration
    void configure(const LogConfig& config);
    LogConfig getConfig() const { return config_; }
    
    // Query functions
    std::vector<std::string> getRecentMessages(ErrorLevel level = ErrorLevel::INFO, const std::string& category = "") const;
    size_t getMessageCount(ErrorLevel level = ErrorLevel::INFO) const;
    bool hasErrors() const;
    std::string getLogSummary() const;
    
    // File operations
    bool saveLog(const std::string& filename);
    bool loadLog(const std::string& filename);
    
private:
    LogConfig config_;
    std::vector<ErrorInfo> logBuffer_;
    std::ofstream logFile_;
    std::chrono::system_clock::time_point lastRotation_;
    
    void writeToFile(const ErrorInfo& error);
    void writeToConsole(const ErrorInfo& error);
    std::string formatErrorMessage(const ErrorInfo& error) const;
    bool shouldLogLevel(ErrorLevel level) const;
};

// Progress tracking system
class ProgressTracker {
public:
    struct ProgressOptions {
        bool showPercentage = true;
        bool showElapsedTime = true;
        bool showEstimatedTime = true;
        bool showSpeed = true;
        bool showProgressBar = true;
        int barWidth = 50;
        std::string updateFrequency = "normal"; // "frequent", "normal", "rare"
        bool enableSoundNotifications = false;
        std::string progressFormat = "detailed";
    };
    
    ProgressTracker(const ProgressOptions& options = ProgressOptions()) : options_(options) {}
    
    // Start/stop progress tracking
    void start(const std::string& description, int totalSteps = -1);
    void stop();
    
    // Update progress
    void increment(int steps = 1, const std::string& message = "");
    void update(int currentStep, int totalSteps, const std::string& message = "");
    void setMessage(const std::string& message);
    
    // Progress state
    bool isActive() const { return active_; }
    int getCurrentStep() const { return currentStep_; }
    int getTotalSteps() const { return totalSteps_; }
    float getProgressPercentage() const;
    std::string getElapsedTime() const;
    std::string getEstimatedTimeRemaining() const;
    std::string getCurrentMessage() const { return currentMessage_; }
    
    // Configuration
    void setOptions(const ProgressOptions& options) { options_ = options; }
    ProgressOptions getOptions() const { return options_; }
    
    // Callbacks
    void setProgressCallback(const std::function<void(float)>& callback);
    void setCompletionCallback(const std::function<void()>& callback);
    void setErrorCallback(const std::function<void(const std::string&)>& callback);
    
private:
    ProgressOptions options_;
    bool active_ = false;
    int currentStep_ = 0;
    int totalSteps_ = 0;
    std::string description_;
    std::string currentMessage_;
    std::chrono::system_clock::time_point startTime_;
    std::function<void(float)> progressCallback_;
    std::function<void()> completionCallback_;
    std::function<void(const std::string&)> errorCallback_;
    
    void updateProgressDisplay();
    std::string formatTime(std::chrono::system_clock::duration duration) const;
};

// Status reporting system
class StatusReporter {
public:
    struct StatusOptions {
        bool enableSystemStatus = true;
        bool enableBrainStatus = true;
        bool enableMemoryStatus = true;
        bool enablePerformanceStatus = true;
        bool enableNetworkStatus = true;
        bool enableResourceStatus = true;
        std::string statusFormat = "detailed";
        bool enableRealTimeUpdates = true;
        double updateInterval = 5.0; // seconds
        bool showWarnings = true;
        bool showErrors = true;
        bool enableAlerts = true;
    };
    
    StatusReporter(const StatusOptions& options = StatusOptions()) : options_(options) {}
    
    // Update status
    void update();
    void refresh() { update(); }
    
    // Get status information
    std::string getSystemStatus() const;
    std::string getBrainStatus() const;
    std::string getMemoryStatus() const;
    std::string getPerformanceStatus() const;
    std::string getNetworkStatus() const;
    std::string getResourceStatus() const;
    
    // Status queries
    bool hasSystemIssues() const;
    bool hasBrainIssues() const;
    bool hasMemoryIssues() const;
    bool hasPerformanceIssues() const;
    
    // Configuration
    void setOptions(const StatusOptions& options) { options_ = options; }
    StatusOptions getOptions() const { return options_; }
    
    // Callbacks
    void setStatusChangeCallback(const std::function<void(const std::string&)>& callback);
    
    // Export
    bool exportStatus(const std::string& filename, const std::string& format = "json");
    std::string getStatusReport() const;
    
private:
    StatusOptions options_;
    mutable std::chrono::system_clock::time_point lastUpdate_;
    mutable std::string cachedSystemStatus_;
    mutable std::string cachedBrainStatus_;
    mutable std::string cachedMemoryStatus_;
    mutable std::string cachedPerformanceStatus_;
    mutable std::string cachedNetworkStatus_;
    mutable std::string cachedResourceStatus_;
    
    std::function<void(const std::string&)>& statusChangeCallback_;
    
    void generateSystemStatus();
    void generateBrainStatus();
    void generateMemoryStatus();
    void generatePerformanceStatus();
    void generateNetworkStatus();
    void generateResourceStatus();
};

} // namespace nlm