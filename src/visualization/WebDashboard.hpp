#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <thread>
#include <mutex>
#include "Visualizer.hpp"

namespace nlm {

class Brain;
class Neuron;
class Synapse;
class SpikeEvent;

/**
 * Web-based dashboard system for NLM
 * 
 * Provides comprehensive web-based visualization with:
 * - Real-time web interface
 * - RESTful API for data access
 * - WebSocket support for live updates
 * - Interactive web-based controls
 * - Remote visualization capabilities
 * - Multi-user support
 * - Cross-platform compatibility
 * - Mobile device support
 * - Browser-based visualization
 * - Server-side rendering
 * - Client-side JavaScript
 * - Data streaming for large datasets
 * - Visualization presets
 * - Shareable visualizations
 * 
 * Features:
 * - Built-in web server
 * - REST API endpoints
 * - WebSocket connections
 * - Real-time data updates
 * - Interactive visualizations
 * - Export to web formats
 * - Responsive design
 * - Authentication support
 * - Authorization controls
 * - Rate limiting
 * - Compression
 * 
 * Integration with:
 * - All visualization systems
 * - Data processing pipeline
 * - Export systems
 * - Experiment management
 * 
 * @see Visualizer
 * @see InteractiveVisualizer
 * @see DataExporter
 * @see AnimationGenerator
 */
class WebDashboard {
public:
    /**
     * Dashboard configuration
     */
    struct DashboardConfig {
        std::string host = "0.0.0.0";
        int port = 8080;
        bool enableSSL = false;
        std::string sslCertPath;
        std::string sslKeyPath;
        std::string documentRoot = "web_root";
        bool enableWebSocket = true;
        bool enableREST = true;
        bool enableCORS = true;
        std::vector<std::string> allowedOrigins;
        std::string apiPrefix = "/api/v1";
        std::string staticPrefix = "/static";
        int maxConnections = 100;
        int timeoutSeconds = 30;
        bool enableAuthentication = false;
        std::string authToken;
        bool enableRateLimiting = true;
        int maxRequestsPerMinute = 60;
        std::string compression = "gzip";
        bool enableGzip = true;
        bool enableBrotli = false;
        bool enableCache = true;
        int cacheMaxAge = 3600; // seconds
        std::string title = "NLM Web Dashboard";
        std::string theme = "light"; // light, dark, auto
        bool enablePushNotifications = false;
        std::string websocketEndpoint = "/ws";
        bool enableBrowserSync = false;
        bool enableCollaborativeEditing = false;
        std::string version = "1.0.0";
        std::string nodeVersion = "node16.0.0";
        std::string dependencies = "npm6.0.0";
        std::unordered_map<std::string, std::string> metadata;
    };

    /**
     * WebSocket message types
     */
    enum class WebSocketMessageType {
        CONNECT,
        DISCONNECT,
        DATA_UPDATE,
        VISUALIZATION_UPDATE,
        COMMAND,
        RESPONSE,
        ERROR,
        PING,
        PONG
    };

    /**
     * REST API endpoint
     */
    struct ApiEndpoint {
        std::string path;
        std::string method; // GET, POST, PUT, DELETE, OPTIONS
        std::string handler;
        bool authRequired = false;
        std::string description;
        bool enableCORS = true;
        bool enableCache = false;
    };

    /**
     * WebSocket session information
     */
    struct WebSocketSession {
        std::string sessionId;
        std::string clientId;
        std::chrono::steady_clock::time_point connectedAt;
        std::chrono::steady_clock::time_point lastActivity;
        std::unordered_map<std::string, std::string> metadata;
        bool authenticated = false;
    };

    /**
     * WebDashboard constructor
     */
    WebDashboard();
    
    /**
     * WebDashboard constructor with configuration
     */
    explicit WebDashboard(const DashboardConfig& config);
    
    /**
     * WebDashboard destructor
     */
    ~WebDashboard();

    /**
     * Configure web dashboard
     */
    void configure(const DashboardConfig& config);
    
    /**
     * Start web dashboard
     */
    bool start();
    
    /**
     * Stop web dashboard
     */
    void stop();
    
    /**
     * Check if web dashboard is running
     */
    bool isRunning() const;
    
    /**
     * Update dashboard with new data
     */
    void update(const Brain& brain, const std::vector<SpikeEvent>& spikes);
    
    /**
     * Handle WebSocket message
     */
    bool handleWebSocketMessage(const std::string& sessionId, const std::string& message);
    
    /**
     * Handle REST API request
     */
    std::string handleRestRequest(const std::string& path, const std::string& method, const std::string& body);
    
    /**
     * Get dashboard statistics
     */
    std::string getStatistics() const;
    
    /**
     * Export dashboard configuration
     */
    std::string exportConfig() const;
    
    /**
     * Import dashboard configuration
     */
    bool importConfig(const std::string& configData);
    
    /**
     * Generate dashboard HTML
     */
    std::string generateHtml() const;
    
    /**
     * Get WebSocket endpoint
     */
    std::string getWebSocketEndpoint() const;
    
    /**
     * Get API prefix
     */
    std::string getApiPrefix() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Suppress warnings about unused private members
    WebDashboard(const WebDashboard&) = delete;
    WebDashboard& operator=(const WebDashboard&) = delete;
};

} // namespace nlm
