#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "ExperimentProtocol.hpp"
#include "ExperimentStateManager.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// API endpoint
struct APIEndpoint {
    std::string endpointId;
    std::string method; // "GET", "POST", "PUT", "DELETE", "PATCH"
    std::string path;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queryParams;
    std::string requestBody;
    std::map<std::string, std::string> responseHeaders;
    std::string responseBody;
    int statusCode;
    std::chrono::system_clock::time_point timestamp;
    
    APIEndpoint() : statusCode(0), timestamp(std::chrono::system_clock::now()) {}
    APIEndpoint(const std::string& endpointId, const std::string& method, const std::string& path)
        : endpointId(endpointId), method(method), path(path), statusCode(0),
          timestamp(std::chrono::system_clock::now()) {}
};

// API authentication
struct APIAuthentication {
    std::string authType; // "none", "basic", "bearer", "oauth", "api_key"
    std::string authToken;
    std::map<std::string, std::string> authParameters;
    std::chrono::system_clock::time_point tokenExpiry;
    
    APIAuthentication() : tokenExpiry(std::chrono::system_clock::now()) {}
};

// API request
struct APIRequest {
    std::string requestId;
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> queryParams;
    std::string body;
    std::string contentType;
    std::chrono::system_clock::time_point timestamp;
    
    APIRequest() : timestamp(std::chrono::system_clock::now()) {}
    APIRequest(const std::string& requestId, const std::string& method, const std::string& path)
        : requestId(requestId), method(method), path(path), timestamp(std::chrono::system_clock::now()) {}
};

// API response
struct APIResponse {
    std::string responseId;
    int statusCode;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string contentType;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, std::string> metadata;
    
    APIResponse() : statusCode(0), timestamp(std::chrono::system_clock::now()) {}
    APIResponse(const std::string& responseId, int statusCode, const std::string& body)
        : responseId(responseId), statusCode(statusCode), body(body),
          timestamp(std::chrono::system_clock::now()) {}
};

// API rate limit
struct APIRateLimit {
    std::string rateLimitId;
    std::string endpoint;
    int requestsPerSecond;
    int requestsPerMinute;
    int requestsPerHour;
    int requestsPerDay;
    std::chrono::system_clock::time_point resetTime;
    int currentRequests;
    
    APIRateLimit() : requestsPerSecond(0), requestsPerMinute(0), requestsPerHour(0), requestsPerDay(0),
                    currentRequests(0), resetTime(std::chrono::system_clock::now() + std::chrono::hours(1)) {}
};

class ExperimentAPI {
public:
    ExperimentAPI();
    ~ExperimentAPI();
    
    // Disable copying, enable moving
    ExperimentAPI(const ExperimentAPI&) = delete;
    ExperimentAPI& operator=(const ExperimentAPI&) = delete;
    ExperimentAPI(ExperimentAPI&&) noexcept;
    ExperimentAPI& operator=(ExperimentAPI&&) noexcept;
    
    // API management
    bool initializeAPI(const std::string& apiConfigPath = "");
    bool shutdownAPI();
    bool isAPIInitialized() const;
    
    // Endpoint management
    bool addAPIEndpoint(const std::string& endpointId, const APIEndpoint& endpoint);
    bool removeAPIEndpoint(const std::string& endpointId);
    bool updateAPIEndpoint(const std::string& endpointId, const APIEndpoint& endpoint);
    APIEndpoint getAPIEndpoint(const std::string& endpointId) const;
    std::vector<APIEndpoint> getAllAPIEndpoints() const;
    std::vector<APIEndpoint> getAPIEndpointsByMethod(const std::string& method) const;
    
    // API authentication
    bool configureAuthentication(const APIAuthentication& auth);
    APIAuthentication getAPIAuthentication() const;
    bool validateAPIToken(const std::string& token) const;
    bool refreshAPIToken();
    
    // API requests
    APIResponse makeAPIRequest(const APIRequest& request);
    bool makeAsyncAPIRequest(const APIRequest& request, 
                           std::function<void(const APIResponse&)> callback);
    APIResponse makeAuthenticatedRequest(const APIRequest& request, const std::string& token);
    
    // Rate limiting
    bool configureRateLimit(const APIRateLimit& rateLimit);
    bool checkRateLimit(const std::string& endpointId);
    std::vector<APIRateLimit> getRateLimits() const;
    
    // API monitoring
    bool enableAPIMonitoring(const std::string& endpointId, bool enable);
    bool isAPIMonitoringEnabled(const std::string& endpointId) const;
    bool enableRealTimeAPIMonitoring(bool enable);
    bool isRealTimeAPIMonitoringEnabled() const;
    
    // API analytics
    std::vector<APIRequest> getAPIRequestHistory(const std::string& endpointId,
                                                const std::chrono::system_clock::time_point& fromTime,
                                                const std::chrono::system_clock::time_point& toTime) const;
    std::vector<APIResponse> getAPIResponseHistory(const std::string& endpointId,
                                                  const std::chrono::system_clock::time_point& fromTime,
                                                  const std::chrono::system_clock::time_point& toTime) const;
    std::map<std::string, double> calculateAPIPerformance(const std::string& endpointId) const;
    
    // API backup and recovery
    bool backupAPIConfig(const std::string& backupPath) const;
    bool restoreAPIConfig(const std::string& backupPath);
    bool exportAPIToJSON(const std::string& filepath) const;
    bool importAPIFromJSON(const std::string& filepath);
    
    // API cleanup
    bool cleanupAPIHistory(const std::string& endpointId, 
                         const std::chrono::system_clock::time_point& cutoffTime);
    bool resetAPI(const std::string& endpointId);
    bool clearAllAPIHistory();
    
    // System management
    bool startAPIServer();
    bool stopAPIServer();
    bool isAPIServerRunning() const;
    
    // API utilities
    static std::string generateEndpointId(const std::string& method, const std::string& path) const;
    static bool isValidAPIEndpoint(const APIEndpoint& endpoint) const;
    static bool isValidMethod(const std::string& method) const;
    static std::string generateToken() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm