#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <chrono>
#include <functional>
#include <websocketpp/config/asio.hpp>
#include <websocketpp/server.hpp>
#include <asio.hpp>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class AdvancedLogger;
class PerformanceMonitor;
class RuntimeMetricsCollector;
class LogFormatter;

// Dashboard configuration
struct DashboardConfig {
    std::string name;
    std::string version;
    std::string host;
    uint16_t port;
    bool enableWebSocket;
    bool enableHTTP;
    std::string staticPath;
    std::string templatePath;
    std::chrono::seconds updateInterval;
    std::chrono::seconds connectionTimeout;
    size_t maxConnections;
    std::map<std::string, std::string> headers;
    
    DashboardConfig() : version("1.0.0"), host("localhost"), port(8080), 
                       enableWebSocket(true), enableHTTP(true),
                       updateInterval(std::chrono::seconds(1)),
                       connectionTimeout(std::chrono::seconds(30)),
                       maxConnections(100) {}
};

// WebSocket connection
struct WebSocketConnection {
    std::string id;
    websocketpp::connection_hdl handle;
    std::chrono::system_clock::time_point connectedTime;
    std::chrono::system_clock::time_point lastPing;
    std::map<std::string, std::string> metadata;
    
    WebSocketConnection() : connectedTime(std::chrono::system_clock::now()),
                          lastPing(std::chrono::system_clock::now()) {}
};

// HTTP request
struct HTTPRequest {
    std::string method;
    std::string path;
    std::string query;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string remoteAddress;
    uint16_t remotePort;
    std::chrono::system_clock::time_point timestamp;
    
    HTTPRequest() : remotePort(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// HTTP response
struct HTTPResponse {
    int statusCode;
    std::map<std::string, std::string> headers;
    std::string body;
    std::chrono::system_clock::time_point timestamp;
    
    HTTPResponse() : statusCode(200) {
        timestamp = std::chrono::system_clock::now();
        headers["Content-Type"] = "application/json";
    }
};

// Dashboard metrics
struct DashboardMetrics {
    std::chrono::system_clock::time_point timestamp;
    double cpuUsage;
    double memoryUsage;
    uint64_t activeConnections;
    size_t logQueueSize;
    uint64_t totalLogCount;
    uint64_t errorCount;
    std::map<std::string, double> customMetrics;
    
    DashboardMetrics() : cpuUsage(0.0), memoryUsage(0.0), activeConnections(0),
                        logQueueSize(0), totalLogCount(0), errorCount(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Alert state
struct AlertState {
    std::string id;
    std::string name;
    LogLevel level;
    std::string condition;
    std::string message;
    double threshold;
    std::chrono::system_clock::time_point firstOccurred;
    std::chrono::system_clock::time_point lastOccurred;
    size_t occurrenceCount;
    bool resolved;
    std::chrono::system_clock::time_point resolvedTime;
    
    AlertState() : level(LogLevel::Warning), threshold(0.0), occurrenceCount(0),
                  resolved(false) {
        firstOccurred = std::chrono::system_clock::now();
        lastOccurred = firstOccurred;
    }
};

// REST API endpoint
struct RESTEndpoint {
    std::string path;
    std::string method;
    std::function<HTTPResponse(const HTTPRequest&)> handler;
    bool requireAuth;
    std::vector<std::string> allowedMethods;
    
    RESTEndpoint() : requireAuth(false) {}
    RESTEndpoint(const std::string& p, const std::string& m,
                 const std::function<HTTPResponse(const HTTPRequest&)>& h,
                 bool auth = false)
        : path(p), method(m), handler(h), requireAuth(auth) {}
};

// WebSocket message
struct WebSocketMessage {
    std::string type;
    std::string data;
    std::chrono::system_clock::time_point timestamp;
    std::string sender;
    
    WebSocketMessage() {
        timestamp = std::chrono::system_clock::now();
    }
    
    WebSocketMessage(const std::string& t, const std::string& d, const std::string& s = "")
        : type(t), data(d), sender(s) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Main MonitorDashboard class
class MonitorDashboard {
public:
    MonitorDashboard();
    ~MonitorDashboard();
    
    // Disable copying, enable moving
    MonitorDashboard(const MonitorDashboard&) = delete;
    MonitorDashboard& operator=(const MonitorDashboard&) = delete;
    MonitorDashboard(MonitorDashboard&&) noexcept;
    MonitorDashboard& operator=(MonitorDashboard&&) noexcept;
    
    // Configuration
    void configure(const DashboardConfig& config);
    DashboardConfig getConfig() const;
    
    // Component integration
    void setLogger(std::shared_ptr<AdvancedLogger> logger);
    void setPerformanceMonitor(std::shared_ptr<PerformanceMonitor> monitor);
    void setRuntimeMetricsCollector(std::shared_ptr<RuntimeMetricsCollector> collector);
    void setLogFormatter(std::shared_ptr<LogFormatter> formatter);
    
    // REST API management
    void addEndpoint(const std::string& path, const std::string& method,
                    const std::function<HTTPResponse(const HTTPRequest&)>& handler,
                    bool requireAuth = false);
    void removeEndpoint(const std::string& path, const std::string& method);
    void clearEndpoints();
    
    // WebSocket management
    void startWebSocketServer();
    void stopWebSocketServer();
    bool isWebSocketServerRunning() const;
    void broadcastMessage(const WebSocketMessage& message);
    void sendToConnection(const std::string& connectionId, const WebSocketMessage& message);
    
    // HTTP server management
    void startHTTPServer();
    void stopHTTPServer();
    bool isHTTPServerRunning() const;
    
    // Dashboard operations
    void start();
    void stop();
    bool isRunning() const;
    
    // Metrics and data
    DashboardMetrics getCurrentMetrics() const;
    nlohmann::json getSystemInfo() const;
    nlohmann::json getPerformanceData(const std::string& timeframe = "1h") const;
    nlohmann::json getLogData(const std::string& category = "", const std::string& level = "") const;
    nlohmann::json getAlertData() const;
    
    // Alert management
    void addAlert(const AlertState& alert);
    void resolveAlert(const std::string& alertId);
    void removeAlert(const std::string& alertId);
    void clearAlerts();
    std::vector<AlertState> getAlerts() const;
    std::vector<AlertState> getActiveAlerts() const;
    
    // Client management
    std::string getConnectionId(const websocketpp::connection_hdl& handle) const;
    void removeConnection(const std::string& connectionId);
    std::vector<std::string> getActiveConnections() const;
    
    // Configuration files
    bool loadConfiguration(const std::string& filepath);
    bool saveConfiguration() const;
    
    // Static instance management
    static MonitorDashboard& getInstance();
    static void setGlobal(std::shared_ptr<MonitorDashboard> dashboard);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void updateMetrics();
    void checkAlerts();
    void generateGraphs();
    void updateWebSocketClients();
    void cleanupConnections();
    std::string generateConnectionId() const;
};

// WebSocket server handler
class WebSocketServerHandler {
public:
    WebSocketServerHandler();
    ~WebSocketServerHandler();
    
    void onMessage(websocketpp::connection_hdl hdl, WebSocketMessage message);
    void onOpen(websocketpp::connection_hdl hdl);
    void onClose(websocketpp::connection_hdl hdl);
    void onError(websocketpp::connection_hdl hdl);
    
    void setDashboard(std::shared_ptr<MonitorDashboard> dashboard);
    std::shared_ptr<MonitorDashboard> getDashboard() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::shared_ptr<MonitorDashboard> dashboard_;
};

// HTTP server handler
class HTTPServerHandler {
public:
    HTTPServerHandler();
    ~HTTPServerHandler();
    
    void onRequest(const HTTPRequest& request, HTTPResponse& response);
    void setDashboard(std::shared_ptr<MonitorDashboard> dashboard);
    std::shared_ptr<MonitorDashboard> getDashboard() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::shared_ptr<MonitorDashboard> dashboard_;
    
    HTTPResponse handleGET(const HTTPRequest& request);
    HTTPResponse handlePOST(const HTTPRequest& request);
    HTTPResponse handlePUT(const HTTPRequest& request);
    HTTPResponse handleDELETE(const HTTPRequest& request);
    HTTPResponse handleStatic(const HTTPRequest& request);
};

// Dashboard utility functions
namespace Dashboard {
    // Configuration file operations
    bool saveConfig(const DashboardConfig& config, const std::string& filepath);
    bool loadConfig(DashboardConfig& config, const std::string& filepath);
    
    // Data formatting
    nlohmann::json formatMetrics(const DashboardMetrics& metrics);
    nlohmann::json formatAlert(const AlertState& alert);
    nlohmann::json formatSystemInfo();
    
    // Visualization
    std::string generateGraphHTML(const nlohmann::json& data, const std::string& type = "line");
    std::string generateMetricChart(const nlohmann::json& metrics, const std::string& title);
    std::string generateLogTimeline(const nlohmann::json& logData);
    
    // Utility
    std::string getCurrentTimestamp();
    std::string generateUUID();
    std::vector<std::string> splitString(const std::string& str, char delimiter);
    bool startsWith(const std::string& str, const std::string& prefix);
}

} // namespace nlm