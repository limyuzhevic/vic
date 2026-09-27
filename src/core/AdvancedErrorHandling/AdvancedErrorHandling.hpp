#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <future>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <regex>

namespace nlm {

// Advanced error recovery system
class ErrorRecoverySystem {
public:
    ErrorRecoverySystem();
    ~ErrorRecoverySystem();

    // Recovery strategy interface
    class RecoveryStrategy {
    public:
        virtual ~RecoveryStrategy() = default;
        virtual bool canRecover(const std::shared_ptr<Error>& error) const = 0;
        virtual bool recover(std::shared_ptr<Error>& error, const std::map<std::string, std::string>& context) = 0;
        virtual std::string getName() const = 0;
        virtual ErrorSeverity getRecommendedSeverity() const = 0;
        virtual double getEstimatedRecoveryTime() const = 0;
    };

    // Specific recovery strategies
    class AutomaticRetryStrategy : public RecoveryStrategy {
    public:
        AutomaticRetryStrategy(size_t maxRetries = 3, double retryDelay = 1.0);
        bool canRecover(const std::shared_ptr<Error>& error) const override;
        bool recover(std::shared_ptr<Error>& error, const std::map<std::string, std::string>& context) override;
        std::string getName() const override { return "AutomaticRetry"; }
        ErrorSeverity getRecommendedSeverity() const override { return ErrorSeverity::Warning; }
        double getEstimatedRecoveryTime() const override { return 0.5; }
    
    private:
        size_t maxRetries_;
        double retryDelay_;
        std::map<std::string, size_t> retryCount_;
    };

    class FallbackValueStrategy : public RecoveryStrategy {
    public:
        FallbackValueStrategy();
        bool canRecover(const std::shared_ptr<Error>& error) const override;
        bool recover(std::shared_ptr<Error>& error, const std::map<std::string, std::string>& context) override;
        std::string getName() const override { return "FallbackValue"; }
        ErrorSeverity getRecommendedSeverity() const override { return ErrorSeverity::Error; }
        double getEstimatedRecoveryTime() const override { return 0.1; }
    };

    class CircuitBreakerStrategy : public RecoveryStrategy {
    public:
        CircuitBreakerStrategy(double failureThreshold = 5.0, double timeout = 60.0);
        bool canRecover(const std::shared_ptr<Error>& error) const override;
        bool recover(std::shared_ptr<Error>& error, const std::map<std::string, std::string>& context) override;
        std::string getName() const override { return "CircuitBreaker"; }
        ErrorSeverity getRecommendedSeverity() const override { return ErrorSeverity::Critical; }
        double getEstimatedRecoveryTime() const override { return 1.0; }
    
    private:
        double failureThreshold_;
        double timeout_;
        std::map<std::string, double> failureCounts_;
        std::map<std::string, double> lastFailureTimes_;
    };

    class GracefulDegradationStrategy : public RecoveryStrategy {
    public:
        GracefulDegradationStrategy();
        bool canRecover(const std::shared_ptr<Error>& error) const override;
        bool recover(std::shared_ptr<Error>& error, const std::map<std::string, std::string>& context) override;
        std::string getName() const override { return "GracefulDegradation"; }
        ErrorSeverity getRecommendedSeverity() const override { return ErrorSeverity::Error; }
        double getEstimatedRecoveryTime() const override { return 0.5; }
    };

    // Recovery orchestration
    void addStrategy(std::shared_ptr<RecoveryStrategy> strategy);
    void removeStrategy(const std::string& name);
    void clearStrategies();
    bool attemptRecovery(std::shared_ptr<Error>& error, const std::map<std::string, std::string>& context = std::map<std::string, std::string>());
    bool autoRecover(std::shared_ptr<Error>& error, const std::map<std::string, std::string>& context = std::map<std::string, std::string>());

    // Recovery statistics
    struct RecoveryStatistics {
        size_t totalRecoveryAttempts = 0;
        size_t successfulRecoveries = 0;
        size_t failedRecoveries = 0;
        double averageRecoveryTime = 0.0;
        std::map<std::string, size_t> strategyCounts;
        std::chrono::system_clock::time_point lastRecoveryTime;
        
        void recordRecovery(bool success, double recoveryTime, const std::string& strategyName);
        double getSuccessRate() const;
    };

    const RecoveryStatistics& getStatistics() const { return statistics_; }
    void resetStatistics();

    // Recovery configuration
    struct RecoveryConfig {
        bool enableAutoRecovery = true;
        double recoveryTimeout = 30.0;
        size_t maxConcurrentRecoveries = 5;
        bool enableExponentialBackoff = true;
        double baseRetryDelay = 1.0;
        double maxRetryDelay = 60.0;
        bool enableCircuitBreaker = true;
        bool enableGracefulDegradation = true;
        std::string defaultStrategy = "AutomaticRetry";
        
        void loadFromFile(const std::string& filepath);
        void saveToFile(const std::string& filepath) const;
    };

    void setConfig(const RecoveryConfig& config);
    const RecoveryConfig& getConfig() const { return config_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    RecoveryConfig config_;
    RecoveryStatistics statistics_;
};

// Error diagnostics system
class ErrorDiagnostics {
public:
    ErrorDiagnostics();
    ~ErrorDiagnostics();

    // Diagnostic information collection
    struct DiagnosticInfo {
        std::string timestamp;
        std::string errorType;
        std::string errorCategory;
        std::string severity;
        std::string sourceFile;
        int lineNumber;
        std::string functionName;
        std::string message;
        std::map<std::string, std::string> context;
        std::vector<std::string> stackTrace;
        double diagnosticTime;
        std::map<std::string, double> resourceUsage;
        std::map<std::string, std::string> systemState;
        
        std::string toJSON() const;
        std::string toXML() const;
        std::string toCSV() const;
        std::string toText() const;
    };

    // Diagnostic collection
    void collectDiagnosticInfo(std::shared_ptr<Error> error);
    DiagnosticInfo generateDiagnosticInfo(std::shared_ptr<Error> error) const;
    std::vector<DiagnosticInfo> getRecentDiagnostics(size_t count = 100) const;
    DiagnosticInfo getDiagnosticInfo(const std::string& errorId) const;

    // Diagnostic analysis
    std::string analyzeErrorPattern(const std::vector<DiagnosticInfo>& diagnostics) const;
    std::string getMostCommonErrorTypes(const std::vector<DiagnosticInfo>& diagnostics) const;
    double getAverageErrorSeverity(const std::vector<DiagnosticInfo>& diagnostics) const;
    std::map<std::string, size_t> getErrorFrequencyDistribution(const std::vector<DiagnosticInfo>& diagnostics) const;
    std::string getErrorCorrelationReport(const std::vector<DiagnosticInfo>& diagnostics) const;

    // Diagnostic export
    bool exportDiagnostics(const std::vector<DiagnosticInfo>& diagnostics, const std::string& filepath, const std::string& format = "json") const;
    bool importDiagnostics(std::vector<DiagnosticInfo>& diagnostics, const std::string& filepath);

    // Diagnostic configuration
    struct DiagnosticConfig {
        bool enableAutomaticCollection = true;
        size_t maxDiagnosticHistory = 1000;
        std::string diagnosticFormat = "detailed";
        bool includeStackTrace = true;
        bool includeResourceUsage = true;
        bool includeSystemState = true;
        double collectionInterval = 1.0;
        std::string outputDirectory = "./diagnostics";
        
        void loadFromFile(const std::string& filepath);
        void saveToFile(const std::string& filepath) const;
    };

    void setConfig(const DiagnosticConfig& config);
    const DiagnosticConfig& getConfig() const { return config_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    DiagnosticConfig config_;
    std::vector<DiagnosticInfo> diagnosticHistory_;
    std::mutex mutex_;
};

// Error analytics system
class ErrorAnalytics {
public:
    ErrorAnalytics();
    ~ErrorAnalytics();

    // Analytics data
    struct AnalyticsData {
        std::chrono::system_clock::time_point timePeriodStart;
        std::chrono::system_clock::time_point timePeriodEnd;
        size_t totalErrors;
        size_t totalRecoveries;
        double recoveryRate;
        std::map<std::string, size_t> errorTypeDistribution;
        std::map<std::string, size_t> errorSeverityDistribution;
        std::map<std::string, size_t> errorCategoryDistribution;
        std::map<std::string, double> errorFrequency;
        std::map<std::string, double> recoveryTimeDistribution;
        std::vector<std::pair<std::string, double>> topErrorSources;
        std::vector<std::pair<std::string, double>> errorTrends;
        
        std::string generateReport() const;
        std::string generateJSONReport() const;
        std::string generateHTMLReport() const;
        std::string generateChartData() const;
    };

    // Analytics collection
    void recordError(std::shared_ptr<Error> error, bool recovered = false, double recoveryTime = 0.0);
    AnalyticsData calculateAnalytics(const std::chrono::system_clock::time_point& startTime, const std::chrono::system_clock::time_point& endTime) const;
    AnalyticsData calculateAnalytics(const std::string& timeRange = "day") const;

    // Analytics queries
    size_t getErrorCount(const std::chrono::system_clock::time_point& startTime, const std::chrono::system_clock::time_point& endTime, ErrorSeverity severity = ErrorSeverity::Error) const;
    double getRecoveryRate(const std::chrono::system_clock::time_point& startTime, const std::chrono::system_clock::time_point& endTime) const;
    std::string getMostFrequentErrorType(const std::chrono::system_clock::time_point& startTime, const std::chrono::system_clock::time_point& endTime) const;
    std::string getErrorTrend(const std::chrono::system_clock::time_point& startTime, const std::chrono::system_clock::time_point& endTime) const;
    std::map<std::string, double> getErrorDistributionByTime(const std::chrono::system_clock::time_point& startTime, const std::chrono::system_clock::time_point& endTime) const;

    // Analytics export
    bool exportAnalytics(const AnalyticsData& data, const std::string& filepath, const std::string& format = "json") const;
    bool importAnalytics(AnalyticsData& data, const std::string& filepath);
    std::string generateAnalyticsReport(const AnalyticsData& data, const std::string& format = "text") const;

    // Analytics configuration
    struct AnalyticsConfig {
        bool enableRealTimeAnalytics = true;
        size_t analyticsWindowSize = 3600; // seconds
        bool trackRecoveryMetrics = true;
        bool trackErrorTrends = true;
        bool detectAnomalies = true;
        double anomalyThreshold = 3.0; // standard deviations
        std::string analyticsInterval = "1m";
        std::string outputFormat = "json";
        std::string outputDirectory = "./analytics";
        
        void loadFromFile(const std::string& filepath);
        void saveToFile(const std::string& filepath) const;
    };

    void setConfig(const AnalyticsConfig& config);
    const AnalyticsConfig& getConfig() const { return config_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    AnalyticsConfig config_;
    std::vector<std::pair<std::chrono::system_clock::time_point, std::shared_ptr<Error>>> errorHistory_;
    std::mutex mutex_;
};

// Advanced error management system
class AdvancedErrorManager {
public:
    AdvancedErrorManager();
    ~AdvancedErrorManager();

    // Integration with existing error handling systems
    void integrateWithErrorReporter(std::shared_ptr<ErrorReporter> errorReporter);
    void integrateWithValidationEngine(std::shared_ptr<ValidationEngine> validationEngine);
    void integrateWithLogger(std::shared_ptr<Logger> logger);

    // Advanced error processing
    void processError(std::shared_ptr<Error> error);
    bool handleError(std::shared_ptr<Error> error);
    void escalateError(std::shared_ptr<Error> error, ErrorSeverity newSeverity);

    // Error prediction and prevention
    bool predictErrors(const std::vector<std::shared_ptr<Error>>& recentErrors);
    bool preventErrors(const std::map<std::string, std::string>& riskFactors);
    std::vector<std::string> getErrorPredictions() const;

    // Error prevention
    bool enableErrorPrevention(bool enabled) { return errorPreventionEnabled_ = enabled; }
    bool isErrorPreventionEnabled() const { return errorPreventionEnabled_; }
    void addErrorPreventionRule(const std::string& name, std::function<bool()> rule);
    void removeErrorPreventionRule(const std::string& name);

    // System health monitoring
    struct SystemHealth {
        double cpuUsage;
        double memoryUsage;
        size_t activeConnections;
        size_t errorRate;
        double responseTime;
        bool isHealthy() const;
        std::string getHealthReport() const;
    };

    SystemHealth getSystemHealth() const;
    bool monitorSystemHealth();
    void setHealthThresholds(double maxCpu, double maxMemory, double maxErrorRate);

    // Advanced configuration
    struct AdvancedConfig {
        bool enableAdvancedErrorHandling = true;
        bool enableErrorPrediction = true;
        bool enableErrorPrevention = true;
        bool enableSystemHealthMonitoring = true;
        double errorThreshold = 10.0; // errors per minute
        double recoveryThreshold = 5.0; // recoveries per minute
        bool enableAutoEscalation = true;
        double escalationFactor = 1.5;
        std::string analyticsEndpoint = "";
        bool enableRemoteLogging = false;
        
        void loadFromFile(const std::string& filepath);
        void saveToFile(const std::string& filepath) const;
    };

    void setConfig(const AdvancedConfig& config);
    const AdvancedConfig& getConfig() const { return config_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    AdvancedConfig config_;
    bool errorPreventionEnabled_;
    std::vector<std::function<bool()>> preventionRules_;
    SystemHealth currentHealth_;
    std::chrono::system_clock::time_point lastHealthCheck_;
};

// Convenience functions for advanced error handling
namespace AdvancedErrorHandling {
    // Error correlation
    std::vector<std::shared_ptr<Error>> correlateErrors(const std::vector<std::shared_ptr<Error>>& errors);
    std::string analyzeErrorCorrelation(const std::vector<std::shared_ptr<Error>>& errors);

    // Error prediction
    std::vector<std::string> predictFutureErrors(const std::vector<std::shared_ptr<Error>>& recentErrors, size_t predictionHorizon = 10);
    bool detectErrorAnomaly(const std::vector<std::shared_ptr<Error>>& errors, double threshold = 3.0);

    // Error prevention
    bool applyPreventiveMeasures(const std::map<std::string, std::string>& riskFactors);
    std::vector<std::string> getPreventiveActions(const std::vector<std::shared_ptr<Error>>& errors);

    // Performance optimization
    void optimizeErrorHandling(std::shared_ptr<ErrorReporter> errorReporter, std::shared_ptr<ValidationEngine> validationEngine);
    bool monitorErrorHandlingPerformance(std::shared_ptr<ErrorReporter> errorReporter);

    // Integration utilities
    void registerAdvancedErrorHandlers(std::shared_ptr<AdvancedErrorManager> manager);
    void unregisterAdvancedErrorHandlers();
}

} // namespace nlm
