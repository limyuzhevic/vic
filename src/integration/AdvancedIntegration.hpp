#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <unordered_set>
#include <eigen3/Eigen/Dense>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <sqlalchemy/sqlalchemy.hpp>  // Assuming SQLAlchemy-like C++ wrapper
#include <libcurl/curl.h>
#include <boost/algorithm/string.hpp>
#include <nlohmann/json.hpp>
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/PerformanceMonitor.hpp"

namespace nlm {

/**
 * External data integration result
 */
struct ExternalDataResult {
    std::string source;
    std::string format;
    size_t dataPoints;
    double processingTime;
    std::map<std::string, double> statistics;
    std::vector<std::vector<double>> dataMatrix;
    std::map<std::string, std::vector<double>> timeSeries;
    bool success;
    std::string errorMessage;
};

/**
 * WebSocket connection configuration
 */
struct WebSocketConfig {
    std::string endpoint;
    uint16_t port;
    std::string path;
    std::map<std::string, std::string> headers;
    bool secure;
    size_t pingInterval;
    size_t connectionTimeout;
};

/**
 * Database connection configuration
 */
struct DatabaseConfig {
    std::string connectionString;
    std::string driver;
    std::string databaseName;
    std::string username;
    std::string password;
    std::string host;
    uint16_t port;
    bool sslEnabled;
    size_t connectionPoolSize;
    std::map<std::string, std::string> extraOptions;
};

/**
 * API integration configuration
 */
struct ApiConfig {
    std::string baseUrl;
    std::string apiKey;
    std::map<std::string, std::string> defaultHeaders;
    size_t timeout;
    size_t retryCount;
    std::string rateLimitKey;
    double rateLimitPerSecond;
    bool authEnabled;
    std::map<std::string, std::string> authHeaders;
};

/**
 * Real-time data source configuration
 */
struct RealTimeSourceConfig {
    std::string sourceName;
    std::string dataFormat;
    std::string url;
    std::map<std::string, std::string> parameters;
    bool enabled;
    double refreshInterval;
    size_t maxBufferSize;
    std::function<std::vector<double>(const std::map<std::string, std::string>&)> dataFetcher;
};

/**
 * Performance monitoring configuration
 */
struct PerformanceConfig {
    bool gpuAcceleration;
    bool simdOptimization;
    bool multiThreading;
    size_t numThreads;
    size_t numGPUs;
    std::map<std::string, double> cpuAffinity;
    std::map<std::string, double> memoryLimits;
    std::string optimizationLevel;  // "aggressive", "balanced", "conservative"
};

/**
 * Security configuration
 */
struct SecurityConfig {
    bool authenticationEnabled;
    std::string authType;  // "jwt", "oauth2", "basic"
    std::string tokenEndpoint;
    std::string clientId;
    std::string clientSecret;
    std::string username;
    std::string password;
    bool sslEnabled;
    std::string certificatePath;
    std::string privateKeyPath;
    std::map<std::string, std::string> corsOrigins;
    std::map<std::string, std::string> ipWhitelist;
    bool auditLogging;
    std::string logFile;
    size_t maxFailedAttempts;
    std::chrono::minutes lockoutDuration;
};

/**
 * Cross-platform compatibility result
 */
struct PlatformCompatibilityResult {
    std::string platform;
    bool pythonSupport;
    bool matlabSupport;
    bool rSupport;
    bool cppSupport;
    std::map<std::string, bool> languageBindings;
    std::vector<std::string> availableAPIs;
    std::string apiVersion;
    double compatibilityScore;
};

/**
 * Advanced integration and interoperability engine
 */
class AdvancedIntegration {
public:
    AdvancedIntegration();
    ~AdvancedIntegration();
    
    // Initialize with brain reference and config
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    
    // External data integration
    ExternalDataResult importExperimentalData(const std::string& filepath, 
                                              const std::string& format = "auto");
    ExternalDataResult fetchDataFromAPI(const std::string& endpoint,
                                       const std::map<std::string, std::string>& parameters,
                                       const ApiConfig& apiConfig);
    ExternalDataResult importFromDatabase(const std::string& query,
                                         const DatabaseConfig& dbConfig,
                                         const std::map<std::string, std::string>& params = {});
    ExternalDataResult fetchRealTimeData(const RealTimeSourceConfig& sourceConfig);
    
    // WebSocket integration for real-time visualization
    bool startWebSocketServer(const WebSocketConfig& config,
                            const std::function<void(const std::string&)>& messageHandler);
    void stopWebSocketServer();
    bool sendWebSocketMessage(const std::string& message);
    void setWebSocketDataCallback(const std::function<std::string()>& dataCallback);
    
    // Cross-platform compatibility
    std::vector<PlatformCompatibilityResult> checkPlatformCompatibility() const;
    bool exportForPython(const std::string& filename, 
                        const std::vector<std::string>& includeModules = {});
    bool exportForMATLAB(const std::string& filename,
                        const std::vector<std::string>& includeModules = {});
    bool exportForR(const std::string& filename,
                   const std::vector<std::string>& includeModules = {});
    
    // Performance monitoring and hardware acceleration
    bool configurePerformance(const PerformanceConfig& perfConfig);
    bool enableGPUAcceleration(size_t gpuIndex = 0);
    bool enableSIMDInstructions();
    bool enableMultiThreading(size_t numThreads = 0);
    bool optimizeMemoryUsage();
    PerformanceMetrics getCurrentPerformanceMetrics() const;
    
    // Security and access control
    bool configureSecurity(const SecurityConfig& securityConfig);
    bool authenticate(const std::string& username, const std::string& password,
                     const std::string& tokenEndpoint = "");
    bool authorize(const std::string& resource, const std::string& action,
                   const std::map<std::string, std::string>& context = {});
    void logAccess(const std::string& userId, const std::string& action,
                   const std::string& resource, bool success,
                   const std::map<std::string, std::string>& details = {});
    std::vector<std::map<std::string, std::string>> getAccessLogs(
        const std::string& userId = "",
        const std::chrono::system_clock::time_point& startTime = std::chrono::system_clock::time_point::min(),
        const std::chrono::system_clock::time_point& endTime = std::chrono::system_clock::time_point::max());
    
    // API integration for cloud computing
    bool connectToCloudService(const ApiConfig& config);
    bool disconnectFromCloudService();
    std::string executeCloudFunction(const std::string& functionName,
                                    const std::map<std::string, std::string>& parameters,
                                    const ApiConfig& config);
    std::vector<std::string> listCloudFunctions(const ApiConfig& config);
    bool deployToCloud(const std::map<std::string, std::string>& deploymentConfig,
                      const ApiConfig& config);
    
    // Multi-scale simulation with asynchronous time stepping
    struct MultiScaleSimulationConfig {
        double baseTimeStep;
        std::vector<size_t> scaleFactors;  // For micro, meso, macro scales
        bool enableAsynchronous;  // Enable asynchronous time stepping
        std::string schedulingAlgorithm;  // "static", "dynamic", "adaptive"
        size_t maxAsyncTasks;
        double maxClockSkew;  // Maximum allowed skew between scales
        std::function<void(size_t scale, double time, double dt)> scaleUpdateCallback;
    };
    
    bool startMultiScaleSimulation(const MultiScaleSimulationConfig& config);
    void stopMultiScaleSimulation();
    bool addMultiScaleTask(size_t scale, double executionTime, double priority = 1.0);
    bool synchronizeScales();
    double getScaleTime(size_t scale) const;
    
    // External data preprocessing
    std::vector<std::vector<double>> normalizeData(const std::vector<std::vector<double>>& data,
                                                   const std::vector<double>& means,
                                                   const std::vector<double>& stdDevs);
    std::vector<std::vector<double>> applyFeatureScaling(const std::vector<std::vector<double>>& data,
                                                         const std::string& method = "standard");
    std::vector<std::vector<double>> reduceDimensionality(const std::vector<std::vector<double>>& data,
                                                         size_t targetDimensions,
                                                         const std::string& method = "pca");
    std::vector<std::vector<double>> filterOutliers(const std::vector<std::vector<double>>& data,
                                                    double threshold = 3.0);
    
    // Real-time data streaming
    struct DataStream {
        std::string streamId;
        std::string source;
        std::chrono::system_clock::time_point startTime;
        std::vector<std::vector<double>> buffer;
        size_t maxBufferSize;
        double samplingRate;
        bool active;
        std::function<void(const std::vector<std::vector<double>>&)> dataCallback;
    };
    
    std::string createDataStream(const RealTimeSourceConfig& config);
    bool destroyDataStream(const std::string& streamId);
    bool startDataStream(const std::string& streamId);
    bool stopDataStream(const std::string& streamId);
    std::vector<std::vector<double>> getStreamData(const std::string& streamId, 
                                                   size_t maxPoints = 1000);
    void setStreamDataCallback(const std::string& streamId,
                              const std::function<void(const std::vector<std::vector<double>>&)>& callback);
    
    // Configuration and utilities
    void configure(const std::map<std::string, std::string>& options);
    std::map<std::string, std::string> getConfiguration() const;
    
    // Performance monitoring
    double getIntegrationTime() const { return integrationTime; }
    
    // Error handling
    std::string getLastError() const { return lastError; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::shared_ptr<Brain> brainRef;
    std::shared_ptr<Config> configRef;
    double integrationTime;
    std::string lastError;
    
    // Parallel processing utilities
    std::mutex integrationMutex;
    std::condition_variable integrationCV;
    bool stopRequested;
    
    // WebSocket server
    struct WebSocketServer {
        std::unique_ptr<websocketpp::server<websocketpp::config::asio_no_tls>> server;
        std::function<void(const std::string&)> messageHandler;
        std::function<std::string()> dataCallback;
        bool running;
    } wsServer;
    
    // Database connection pool
    struct DatabaseConnection {
        std::string id;
        std::chrono::system_clock::time_point lastUsed;
        bool connected;
    };
    std::vector<DatabaseConnection> dbConnections;
    
    // HTTP client for API calls
    struct HttpClient {
        CURL* curl;
        std::string apiKey;
        size_t timeout;
        size_t retryCount;
    } httpClient;
};

} // namespace nlm
