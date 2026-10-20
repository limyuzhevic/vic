#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <thread>
#include <chrono>
#include <functional>
#include <nlohmann/json.hpp>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Web interface configuration
struct WebInterfaceConfig {
    bool enableWebInterface = false;
    std::string host = "localhost";
    int port = 8080;
    std::string basePath = "/";
    std::string staticPath = "./web/static";
    std::string templatePath = "./web/templates";
    bool enableSSE = true; // Server-Sent Events for real-time updates
    bool enableWebSocket = false; // WebSocket for bidirectional communication
    bool enableCORS = true;
    std::vector<std::string> allowedOrigins = {"*"};
    std::chrono::milliseconds requestTimeout = std::chrono::milliseconds(30000);
    size_t maxConcurrentConnections = 100;
    std::string sslCertificate = "";
    std::string sslKey = "";
    bool enableAuthentication = false;
    std::string authUsername = "admin";
    std::string authPasswordHash = "";
    std::string apiKey = "";
    bool enableLogging = true;
    std::string logLevel = "info";
};

// Web interface request/response types
struct WebRequest {
    std::string method;
    std::string path;
    std::string queryString;
    std::string fragment;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    std::unordered_map<std::string, std::string> pathParams;
    std::unordered_map<std::string, std::string> queryParams;
    std::unordered_map<std::string, std::string> formData;
    std::unordered_map<std::string, std::string> jsonData;
    std::chrono::high_resolution_clock::time_point receivedTime;
    std::string clientIP;
    int clientPort;
    
    WebRequest() {
        receivedTime = std::chrono::high_resolution_clock::now();
    }
};

struct WebResponse {
    int statusCode = 200;
    std::string statusText = "OK";
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    std::string contentType = "text/html";
    bool connectionKeepAlive = false;
    std::function<void()> callback;
    
    void setContentType(const std::string& type) {
        contentType = type;
        headers["Content-Type"] = type;
    }
    
    void setStatus(int code, const std::string& text = "") {
        statusCode = code;
        statusText = text.empty() ? getDefaultStatusText(code) : text;
    }
    
    std::string getDefaultStatusText(int code) {
        switch (code) {
            case 200: return "OK";
            case 201: return "Created";
            case 204: return "No Content";
            case 400: return "Bad Request";
            case 401: return "Unauthorized";
            case 403: return "Forbidden";
            case 404: return "Not Found";
            case 405: return "Method Not Allowed";
            case 500: return "Internal Server Error";
            case 502: return "Bad Gateway";
            case 503: return "Service Unavailable";
            default: return "Unknown";
        }
    }
};

// Web interface API endpoints
struct APIEndpoints {
    // Monitoring API
    struct Monitoring {
        static constexpr const char* GET_METRICS = "/api/metrics";
        static constexpr const char* GET_PERFORMANCE = "/api/performance";
        static constexpr const char* GET_REAL_TIME = "/api/realtime";
        static constexpr const char* GET_HISTORY = "/api/history";
        static constexpr const char* POST_METRIC = "/api/metric";
        static constexpr const char* DELETE_METRIC = "/api/metric/{name}";
    };
    
    // Visualization API
    struct Visualization {
        static constexpr const char* GET_VISUALIZATIONS = "/api/visualizations";
        static constexpr const char* GET_VIZ_DATA = "/api/visualizations/{name}/data";
        static constexpr const char* GET_VIZ_IMAGE = "/api/visualizations/{name}/image";
        static constexpr const char* POST_VIZ_CONFIG = "/api/visualizations/config";
        static constexpr const char* EXPORT_VIZ = "/api/visualizations/export/{format}";
    };
    
    // Export API
    struct Export {
        static constexpr const char* GET_EXPORTS = "/api/exports";
        static constexpr const char* GET_EXPORT = "/api/exports/{name}";
        static constexpr const char* POST_EXPORT = "/api/exports";
        static constexpr const char* DELETE_EXPORT = "/api/exports/{name}";
    };
    
    // System API
    struct System {
        static constexpr const char* GET_STATUS = "/api/status";
        static constexpr const char* GET_CONFIG = "/api/config";
        static constexpr const char* GET_HEALTH = "/api/health";
        static constexpr const char* POST_SHUTDOWN = "/api/shutdown";
        static constexpr const char* POST_RELOAD = "/api/reload";
    };
    
    // Static files
    struct Static {
        static constexpr const char* STATIC = "/static/{path}";
        static constexpr const char* INDEX_HTML = "/";
        static constexpr const char* DASHBOARD_HTML = "/dashboard";
        static constexpr const char* API_DOCS_HTML = "/api-docs";
    };
};

// Web interface WebSocket message types
enum class WebSocketMessageType {
    PING,
    PONG,
    METRIC_UPDATE,
    VISUALIZATION_UPDATE,
    SYSTEM_STATUS,
    COMMAND,
    RESPONSE,
    ERROR
};

struct WebSocketMessage {
    WebSocketMessageType type;
    std::string data;
    std::string timestamp;
    std::string sender;
    
    nlohmann::json toJSON() const {
        nlohmann::json j;
        j["type"] = static_cast<int>(type);
        j["data"] = data;
        j["timestamp"] = timestamp;
        j["sender"] = sender;
        return j;
    }
    
    static WebSocketMessage fromJSON(const nlohmann::json& j) {
        WebSocketMessage msg;
        msg.type = static_cast<WebSocketMessageType>(j["type"].get<int>());
        msg.data = j["data"].get<std::string>();
        msg.timestamp = j["timestamp"].get<std::string>();
        msg.sender = j["sender"].get<std::string>();
        return msg;
    }
};

// Web interface client connection
struct WebClientConnection {
    int socketFD;
    std::string clientIP;
    int clientPort;
    std::string sessionID;
    std::chrono::high_resolution_clock::time_point lastActivity;
    bool authenticated;
    std::string userAgent;
    std::unordered_map<std::string, std::string> headers;
    std::chrono::milliseconds requestTimeout;
    
    WebClientConnection(int fd, const std::string& ip, int port)
        : socketFD(fd), clientIP(ip), clientPort(port), authenticated(false),
          lastActivity(std::chrono::high_resolution_clock::now()),
          requestTimeout(30000) {}
};

// Web interface event types
enum class WebEventType {
    CONNECTION_ESTABLISHED,
    CONNECTION_CLOSED,
    MESSAGE_RECEIVED,
    REQUEST_PROCESSED,
    ERROR_OCCURRED
};

struct WebEvent {
    WebEventType type;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string message;
    std::shared_ptr<WebClientConnection> connection;
    std::shared_ptr<WebRequest> request;
    std::shared_ptr<WebResponse> response;
    
    WebEvent(WebEventType t, const std::string& msg)
        : type(t), timestamp(std::chrono::high_resolution_clock::now()), message(msg) {}
};

// Web interface request handler
using RequestHandler = std::function<bool(const WebRequest&, WebResponse&)>;
using WebSocketHandler = std::function<void(std::shared_ptr<WebClientConnection>, const WebSocketMessage&)>;
using EventHandler = std::function<void(const WebEvent&)>;

// Web interface manager for web-based monitoring dashboard
class WebInterface {
public:
    WebInterface();
    ~WebInterface();
    
    // Disable copying
    WebInterface(const WebInterface&) = delete;
    WebInterface& operator=(const WebInterface&) = delete;
    
    // Initialize web interface
    bool initialize(const WebInterfaceConfig& config = WebInterfaceConfig());
    
    // Start/stop web interface
    void start();
    void stop();
    bool isRunning() const { return running_.load(); }
    
    // Configure web interface
    void configure(const WebInterfaceConfig& config);
    WebInterfaceConfig getConfig() const { return config_; }
    
    // Connection management
    void acceptConnections(bool accept) { acceptConnections_ = accept; }
    bool areConnectionsAccepted() const { return acceptConnections_; }
    
    // Handler management
    void addRequestHandler(const std::string& path, const RequestHandler& handler);
    void addRequestHandler(const std::string& method, const std::string& path, 
                          const RequestHandler& handler);
    void addWebSocketHandler(const WebSocketHandler& handler);
    void addEventHandler(const EventHandler& handler);
    
    // API endpoint registration
    void registerMonitoringAPI();
    void registerVisualizationAPI();
    void registerExportAPI();
    void registerSystemAPI();
    
    // Static file serving
    void serveStaticFiles(const std::string& path);
    
    // WebSocket management
    void broadcastMessage(const WebSocketMessage& message);
    void sendToConnection(std::shared_ptr<WebClientConnection> connection, 
                         const WebSocketMessage& message);
    
    // Event system
    void fireEvent(const WebEvent& event);
    
    std::vector<std::string> getConnectedClients() const;
    
    // Status and monitoring
    size_t getConnectionCount() const;
    std::string getServerStatus() const;
    
    // Export functionality
    bool exportWebInterface(const std::string& filename, const std::string& format = "html");
    
    // Get singleton instance
    static WebInterface& getInstance() {
        static WebInterface instance;
        return instance;
    }
    
    // Global web interface functions
    static void startGlobal(const WebInterfaceConfig& config = WebInterfaceConfig());
    static void stopGlobal();
    static bool isGlobalRunning();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::shared_mutex mutex_;
    std::atomic<bool> running_;
    std::atomic<bool> acceptConnections_;
    WebInterfaceConfig config_;
    std::chrono::high_resolution_clock::time_point startTime_;
    
    // Internal helper methods
    bool createSocket() const;
    bool bindSocket() const;
    bool listen() const;
    void acceptLoop();
    void handleConnection(int clientSocket);
    void handleRequest(std::shared_ptr<WebClientConnection> connection, 
                     const WebRequest& request);
    void handleWebSocket(std::shared_ptr<WebClientConnection> connection,
                        const WebSocketMessage& message);
    void processRequest(const WebRequest& request, WebResponse& response);
    
    // Request parsing
    WebRequest parseRequest(const std::string& rawRequest) const;
    WebResponse createResponse(const std::string& body, int statusCode = 200) const;
    
    // URL parsing
    std::string extractPath(const std::string& requestLine) const;
    std::string extractQueryString(const std::string& requestLine) const;
    std::unordered_map<std::string, std::string> parseQueryString(const std::string& queryString) const;
    std::unordered_map<std::string, std::string> parsePathParams(const std::string& path, 
                                                              const std::string& pattern) const;
    
    // Response generation
    std::string generateHTMLResponse(const std::string& templateName,
                                     const std::unordered_map<std::string, std::string>& variables) const;
    std::string generateJSONResponse(const nlohmann::json& data) const;
    std::string generateErrorResponse(int statusCode, const std::string& message) const;
    
    // API response generators
    std::string generateMetricsResponse() const;
    std::string generatePerformanceResponse() const;
    std::string generateRealTimeResponse() const;
    std::string generateHistoryResponse() const;
    
    // Authentication and security
    bool authenticateRequest(const WebRequest& request) const;
    bool validateOrigin(const std::string& origin) const;
    
    // File serving
    bool serveStaticFile(const std::string& path, WebResponse& response) const;
    
    // Export generation
    bool generateExportPage(WebResponse& response, const std::string& format) const;
    bool generateDashboardPage(WebResponse& response) const;
    bool generateAPIDocsPage(WebResponse& response) const;
};

} // namespace nlm
