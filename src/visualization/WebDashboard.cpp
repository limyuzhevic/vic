#include "WebDashboard.hpp"
#include "VisualizationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <boost/beast/version.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <ctime>

using json = nlohmann::json;
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

namespace nlm {

struct WebDashboard::Impl {
    // Core configuration
    std::shared_ptr<VisualizationManager> visualizationManager;
    std::atomic<bool> running;
    uint16_t port;
    bool enableWebSocket;
    double updateInterval;
    double metricRefreshRate;
    size_t maxConnections;
    bool authenticationRequired;
    bool compressionEnabled;
    
    // Server components
    std::unique_ptr<asio::io_context> ioContext;
    std::unique_ptr<tcp::acceptor> acceptor;
    std::unique_ptr<asio::signal_set> signals;
    std::unique_ptr<beast::ssl_stream<tcp::socket>> sslStream;
    
    // WebSocket management
    std::unordered_map<std::string, WebSocketClient> webSocketClients;
    std::mutex clientsMutex;
    std::unordered_map<std::string, UpdateCallback> customEndpoints;
    
    // Metrics and data
    std::unordered_map<std::string, double> metrics;
    std::mutex metricsMutex;
    std::atomic<size_t> connectedClients;
    std::chrono::steady_clock::time_point lastUpdate;
    std::chrono::steady_clock::time_point lastHealthCheck;
    
    // Performance tracking
    std::atomic<uint64_t> totalRequests;
    std::atomic<double> averageResponseTime;
    std::chrono::steady_clock::time_point startTime;
    
    Impl(std::shared_ptr<VisualizationManager> manager)
        : visualizationManager(manager)
        , running(false)
        , port(8080)
        , enableWebSocket(true)
        , updateInterval(1.0) // 1 second
        , metricRefreshRate(0.5) // 500ms
        , maxConnections(100)
        , authenticationRequired(false)
        , compressionEnabled(true)
        , connectedClients(0)
        , totalRequests(0)
        , averageResponseTime(0.0)
        , startTime(std::chrono::steady_clock::now()) {
        
        // Initialize metrics
        metrics["cpu_usage"] = 0.0;
        metrics["memory_usage"] = 0.0;
        metrics["gpu_usage"] = 0.0;
        metrics["neuron_count"] = 0;
        metrics["spike_count"] = 0;
        metrics["simulation_fps"] = 0.0;
        metrics["visualization_fps"] = 0.0;
    }
};

WebDashboard::WebDashboard(std::shared_ptr<VisualizationManager> manager)
    : pImpl(std::make_unique<Impl>(manager)) {
    LOG_INFO("WebDashboard initialized");
}

WebDashboard::~WebDashboard() {
    stop();
    LOG_INFO("WebDashboard destroyed");
}

WebDashboard::WebDashboard(WebDashboard&& other) noexcept
    : pImpl(std::move(other.pImpl)) {
}

WebDashboard& WebDashboard::operator=(WebDashboard&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool WebDashboard::initialize(uint16_t port, bool enableWebSocket) {
    LOG_INFO("Initializing WebDashboard on port " + std::to_string(port));
    
    if (pImpl->running) {
        LOG_WARN("WebDashboard already running");
        return false;
    }
    
    pImpl->port = port;
    pImpl->enableWebSocket = enableWebSocket;
    
    try {
        // Initialize IO context
        pImpl->ioContext = std::make_unique<asio::io_context>(1);
        pImpl->acceptor = std::make_unique<tcp::acceptor>(*pImpl->ioContext);
        
        // Bind to port
        tcp::endpoint endpoint(tcp::v4(), port);
        pImpl->acceptor->open(endpoint.protocol());
        pImpl->acceptor->set_option(tcp::acceptor::reuse_address(true));
        pImpl->acceptor->bind(endpoint);
        pImpl->acceptor->listen();
        
        // Setup signal handling
        pImpl->signals = std::make_unique<asio::signal_set>(*pImpl->ioContext);
        pImpl->signals->add(SIGINT);
        pImpl->signals->add(SIGTERM);
        
        pImpl->running = true;
        pImpl->startTime = std::chrono::steady_clock::now();
        
        // Start accepting connections
        acceptConnections();
        
        LOG_INFO("WebDashboard initialized successfully on port " + std::to_string(port));
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to initialize WebDashboard: " + std::string(e.what()));
        stop();
        return false;
    }
}

void WebDashboard::start() {
    if (!pImpl->running) {
        LOG_ERROR("WebDashboard not initialized, call initialize() first");
        return;
    }
    
    LOG_INFO("Starting WebDashboard event loop");
    
    // Start signal handler
    std::thread signalHandler([this]() {
        asio::signal_set signals(*pImpl->ioContext, SIGINT, SIGTERM);
        signals.async_wait([this](const boost::system::error_code& error, int signal) {
            LOG_INFO("Received signal " + std::to_string(signal) + ", shutting down");
            stop();
        });
        pImpl->ioContext->run();
    });
    
    // Start I/O context
    std::thread ioThread([this]() {
        pImpl->ioContext->run();
    });
    
    // Store threads for cleanup
    pImpl->signalThread = std::move(signalHandler);
    pImpl->ioThread = std::move(ioThread);
    
    // Wait for stop signal
    std::unique_lock<std::mutex> lock(pImpl->stopMutex);
    pImpl->stopCondition.wait(lock, [this]() { return !pImpl->running; });
    
    // Cleanup
    if (pImpl->ioContext->stopped()) {
        pImpl->ioContext->restart();
    }
    
    if (pImpl->signalThread.joinable()) {
        pImpl->signalThread.join();
    }
    
    if (pImpl->ioThread.joinable()) {
        pImpl->ioThread.join();
    }
}

void WebDashboard::stop() {
    if (!pImpl->running) {
        return;
    }
    
    LOG_INFO("Stopping WebDashboard");
    
    pImpl->running = false;
    pImpl->stopCondition.notify_all();
    
    // Close acceptor
    if (pImpl->acceptor) {
        pImpl->acceptor->close();
    }
    
    // Close all WebSocket connections
    {
        std::lock_guard<std::mutex> lock(pImpl->clientsMutex);
        for (auto& pair : pImpl->webSocketClients) {
            try {
                pair.second.socket.close();
            } catch (const std::exception& e) {
                LOG_DEBUG("Error closing WebSocket: " + std::string(e.what()));
            }
        }
        pImpl->webSocketClients.clear();
        pImpl->connectedClients = 0;
    }
    
    // Stop IO context
    if (pImpl->ioContext) {
        pImpl->ioContext->stop();
    }
    
    LOG_INFO("WebDashboard stopped");
}

bool WebDashboard::isRunning() const {
    return pImpl->running;
}

std::string WebDashboard::handleHTTPGet(const std::string& path) {
    pImpl->totalRequests++;
    auto startTime = std::chrono::steady_clock::now();
    
    std::string response;
    
    if (path == "/" || path == "/dashboard") {
        response = getStaticHTML("dashboard");
    } else if (path == "/javascript" || path == "/app.js") {
        response = getJavaScript("dashboard");
    } else if (path == "/css" || path == "/styles.css") {
        response = getCSS("dashboard");
    } else if (path == "/api/metrics") {
        response = getMetricsJSON();
    } else if (path == "/api/health") {
        response = getHealthJSON();
    } else if (path == "/api/status") {
        response = getStatusJSON();
    } else {
        // Check for custom endpoints
        auto it = pImpl->customEndpoints.find(path);
        if (it != pImpl->customEndpoints.end()) {
            response = it->second("");
        } else {
            response = getNotFoundHTML();
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Update average response time
    double responseTime = static_cast<double>(duration.count()) / 1000.0; // Convert to milliseconds
    pImpl->averageResponseTime = (pImpl->averageResponseTime + responseTime) / 2.0;
    
    return response;
}

std::string WebDashboard::handleHTTPPost(const std::string& path, const std::string& body) {
    pImpl->totalRequests++;
    
    if (path == "/api/commands") {
        return handleCommand(body);
    }
    
    return getBadRequestHTML();
}

bool WebDashboard::handleWebSocketUpgrade(boost::asio::ip::tcp::socket&& socket) {
    if (!pImpl->enableWebSocket) {
        return false;
    }
    
    // Check connection limit
    if (pImpl->connectedClients >= pImpl->maxConnections) {
        LOG_WARN("Connection limit reached, rejecting WebSocket connection");
        return false;
    }
    
    // Create WebSocket session
    auto clientId = generateClientId();
    WebSocketClient client;
    client.socket = std::move(socket);
    client.clientId = clientId;
    client.lastPing = std::chrono::steady_clock::now();
    client.authenticated = !pImpl->authenticationRequired;
    
    // Upgrade to WebSocket
    auto ws = std::make_unique<websocket::stream<tcp::socket>>(std::move(client.socket));
    
    // Set WebSocket options
    ws->set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
    ws->set_option(websocket::stream_base::decorator([](websocket::response_type& res) {
        res.set(http::field::server, "NLM-WebDashboard");
    }));
    
    // Async accept
    ws->async_accept([this, clientId, ws = std::move(ws)](const boost::system::error_code& ec) {
        if (!ec) {
            // WebSocket accepted, start reading messages
            readWebSocketMessages(clientId, std::move(ws));
        } else {
            LOG_DEBUG("WebSocket accept error: " + ec.message());
        }
    });
    
    // Add client to map
    {
        std::lock_guard<std::mutex> lock(pImpl->clientsMutex);
        pImpl->webSocketClients[clientId] = std::move(client);
        pImpl->connectedClients++;
    }
    
    LOG_INFO("WebSocket client connected: " + clientId);
    return true;
}

bool WebDashboard::connectWebSocketClient(const std::string& clientId, const std::string& url) {
    // WebSocket client connection (for connecting to external services)
    LOG_INFO("WebSocket client connecting to: " + url);
    return true;
}

void WebDashboard::disconnectWebSocketClient(const std::string& clientId) {
    std::lock_guard<std::mutex> lock(pImpl->clientsMutex);
    auto it = pImpl->webSocketClients.find(clientId);
    if (it != pImpl->webSocketClients.end()) {
        try {
            it->second.socket.close();
        } catch (const std::exception& e) {
            LOG_DEBUG("Error closing WebSocket client: " + std::string(e.what()));
        }
        pImpl->webSocketClients.erase(it);
        pImpl->connectedClients--;
        LOG_INFO("WebSocket client disconnected: " + clientId);
    }
}

void WebDashboard::broadcastMessage(const std::string& message, const std::string& clientId) {
    std::lock_guard<std::mutex> lock(pImpl->clientsMutex);
    
    if (clientId.empty()) {
        // Broadcast to all clients
        for (auto& pair : pImpl->webSocketClients) {
            try {
                pair.second.socket.write_some(asio::buffer(message));
            } catch (const std::exception& e) {
                LOG_DEBUG("Error broadcasting to client " + pair.first + ": " + std::string(e.what()));
            }
        }
    } else {
        // Send to specific client
        auto it = pImpl->webSocketClients.find(clientId);
        if (it != pImpl->webSocketClients.end()) {
            try {
                it->second.socket.write_some(asio::buffer(message));
            } catch (const std::exception& e) {
                LOG_DEBUG("Error sending to client " + clientId + ": " + std::string(e.what()));
            }
        }
    }
}

void WebDashboard::sendRealTimeUpdate(const RealTimeUpdate& update) {
    // Create JSON message
    json message;
    message["type"] = update.updateType;
    message["data"] = update.data;
    message["timestamp"] = std::chrono::system_clock::to_time_t(update.timestamp);
    message["source"] = update.source;
    
    std::string messageStr = message.dump();
    broadcastMessage(messageStr);
}

void WebDashboard::setUpdateInterval(double interval) {
    if (interval <= 0.0) {
        LOG_WARN("Invalid update interval, using default 1.0");
        interval = 1.0;
    }
    pImpl->updateInterval = interval;
}

void WebDashboard::setMetricRefreshRate(double rate) {
    if (rate <= 0.0) {
        LOG_WARN("Invalid refresh rate, using default 0.5");
        rate = 0.5;
    }
    pImpl->metricRefreshRate = rate;
}

void WebDashboard::addMetric(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    pImpl->metrics[name] = value;
}

void WebDashboard::setMetric(const std::string& name, double value) {
    addMetric(name, value);
}

double WebDashboard::getMetric(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
    auto it = pImpl->metrics.find(name);
    return (it != pImpl->metrics.end()) ? it->second : 0.0;
}

void WebDashboard::enableRealTimeUpdates(bool enable) {
    // Enable/disable real-time updates
    // Implementation would set up periodic timers to fetch and send updates
    LOG_INFO("Real-time updates " + std::string(enable ? "enabled" : "disabled"));
}

void WebDashboard::setWebSocketCompression(bool enable) {
    pImpl->compressionEnabled = enable;
}

void WebDashboard::setMaxConnections(size_t maxConnections) {
    pImpl->maxConnections = maxConnections;
}

void WebDashboard::setAuthenticationRequired(bool required) {
    pImpl->authenticationRequired = required;
}

std::string WebDashboard::getStaticHTML(const std::string& page) {
    // Return HTML for the dashboard
    return R"(
<!DOCTYPE html>
<html>
<head>
    <title>NLM Neural Network Simulator - Dashboard</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background: #f5f5f5; }
        .container { max-width: 1200px; margin: 0 auto; }
        .header { background: #2c3e50; color: white; padding: 20px; border-radius: 5px; }
        .metrics { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin: 20px 0; }
        .metric-card { background: white; padding: 20px; border-radius: 5px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }
        .metric-value { font-size: 2em; font-weight: bold; color: #3498db; }
        .status { padding: 10px; border-radius: 3px; margin: 10px 0; }
        .status-healthy { background: #d5f4e6; color: #27ae60; }
        .status-warning { background: #fef5e7; color: #f39c12; }
        .status-error { background: #fadbd8; color: #e74c3c; }
        .update-time { font-size: 0.9em; color: #7f8c8d; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>NLM Neural Network Simulator</h1>
            <p>Enterprise-Grade Visualization and Monitoring System</p>
        </div>
        
        <div class="metrics">
            <div class="metric-card">
                <h3>CPU Usage</h3>
                <div class="metric-value" id="cpu-usage">--%</div>
            </div>
            <div class="metric-card">
                <h3>Memory Usage</h3>
                <div class="metric-value" id="memory-usage">--%</div>
            </div>
            <div class="metric-card">
                <h3>GPU Usage</h3>
                <div class="metric-value" id="gpu-usage">--%</div>
            </div>
            <div class="metric-card">
                <h3>Neurons</h3>
                <div class="metric-value" id="neuron-count">--</div>
            </div>
            <div class="metric-card">
                <h3>Spikes</h3>
                <div class="metric-value" id="spike-count">--</div>
            </div>
            <div class="metric-card">
                <h3>Simulation FPS</h3>
                <div class="metric-value" id="simulation-fps">--</div>
            </div>
        </div>
        
        <div class="status status-healthy">
            <strong>System Status: Healthy</strong>
            <span class="update-time">Last updated: <span id="last-update">--</span></span>
        </div>
        
        <script>
            function updateMetrics() {
                fetch('/api/metrics')
                    .then(response => response.json())
                    .then(data => {
                        document.getElementById('cpu-usage').textContent = data.cpu_usage + '%';
                        document.getElementById('memory-usage').textContent = data.memory_usage + '%';
                        document.getElementById('gpu-usage').textContent = data.gpu_usage + '%';
                        document.getElementById('neuron-count').textContent = data.neuron_count;
                        document.getElementById('spike-count').textContent = data.spike_count;
                        document.getElementById('simulation-fps').textContent = data.simulation_fps;
                        document.getElementById('last-update').textContent = new Date().toLocaleString();
                    })
                    .catch(error => {
                        console.error('Error fetching metrics:', error);
                    });
            }
            
            // Update metrics every 500ms
            setInterval(updateMetrics, 500);
            
            // Initial update
            updateMetrics();
        </script>
    </div>
</body>
</html>
")";
}

std::string WebDashboard::getJavaScript(const std::string& page) {
    return "// JavaScript for NLM Dashboard";
}

std::string WebDashboard::getCSS(const std::string& page) {
    return "/* CSS for NLM Dashboard */";
}

void WebDashboard::exportDashboardData(const std::string& filepath) {
    // Export dashboard data to file
    LOG_INFO("Exporting dashboard data to: " + filepath);
    
    json data;
    data["title"] = "NLM Dashboard Export";
    data["timestamp"] = std::chrono::system_clock::now();
    data["metrics"] = pImpl->metrics;
    data["connected_clients"] = pImpl->connectedClients;
    data["total_requests"] = pImpl->totalRequests;
    data["average_response_time"] = pImpl->averageResponseTime;
    
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << data.dump(2);
        file.close();
        LOG_INFO("Dashboard data exported successfully");
    } else {
        LOG_ERROR("Failed to export dashboard data");
    }
}

void WebDashboard::generateReport(const std::string& reportPath) {
    // Generate a comprehensive report
    LOG_INFO("Generating dashboard report: " + reportPath);
    
    json report;
    report["title"] = "NLM Dashboard Report";
    report["generated_at"] = std::chrono::system_clock::now();
    report["metrics"] = pImpl->metrics;
    report["performance"] = getPerformanceMetrics();
    report["health"] = getHealthStatus();
    
    std::ofstream file(reportPath + "/dashboard_report.json");
    if (file.is_open()) {
        file << report.dump(2);
        file.close();
        LOG_INFO("Dashboard report generated: " + reportPath + "/dashboard_report.json");
    } else {
        LOG_ERROR("Failed to generate dashboard report");
    }
}

void WebDashboard::loadConfiguration(const std::string& configPath) {
    // Load configuration from file
    LOG_INFO("Loading dashboard configuration from: " + configPath);
    
    try {
        std::ifstream file(configPath);
        if (file.is_open()) {
            json config = json::parse(file);
            
            if (config.contains("port")) {
                port = config["port"];
            }
            if (config.contains("enable_websocket")) {
                enableWebSocket = config["enable_websocket"];
            }
            if (config.contains("update_interval")) {
                updateInterval = config["update_interval"];
            }
            if (config.contains("max_connections")) {
                maxConnections = config["max_connections"];
            }
            
            file.close();
            LOG_INFO("Dashboard configuration loaded successfully");
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load dashboard configuration: " + std::string(e.what()));
    }
}

void WebDashboard::saveConfiguration() const {
    // Save configuration to file
    LOG_INFO("Saving dashboard configuration");
    
    json config;
    config["port"] = pImpl->port;
    config["enable_websocket"] = pImpl->enableWebSocket;
    config["update_interval"] = pImpl->updateInterval;
    config["max_connections"] = pImpl->maxConnections;
    config["authentication_required"] = pImpl->authenticationRequired;
    config["compression_enabled"] = pImpl->compressionEnabled;
    
    std::string configPath = "dashboard_config.json";
    std::ofstream file(configPath);
    if (file.is_open()) {
        file << config.dump(2);
        file.close();
        LOG_INFO("Dashboard configuration saved: " + configPath);
    } else {
        LOG_ERROR("Failed to save dashboard configuration");
    }
}

std::string WebDashboard::getConfigurationJSON() const {
    json config;
    config["port"] = pImpl->port;
    config["enable_websocket"] = pImpl->enableWebSocket;
    config["update_interval"] = pImpl->updateInterval;
    config["max_connections"] = pImpl->maxConnections;
    config["authentication_required"] = pImpl->authenticationRequired;
    config["compression_enabled"] = pImpl->compressionEnabled;
    
    return config.dump(2);
}

bool WebDashboard::isHealthy() const {
    auto now = std::chrono::steady_clock::now();
    auto timeSinceUpdate = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->lastUpdate).count();
    
    return pImpl->running && timeSinceUpdate < 60 && pImpl->connectedClients <= pImpl->maxConnections;
}

std::string WebDashboard::getHealthStatus() const {
    json health;
    health["status"] = isHealthy() ? "healthy" : "unhealthy";
    health["running"] = pImpl->running;
    health["port"] = pImpl->port;
    health["connected_clients"] = pImpl->connectedClients;
    health["max_connections"] = pImpl->maxConnections;
    health["total_requests"] = pImpl->totalRequests;
    
    return health.dump(2);
}

size_t WebDashboard::getConnectedClients() const {
    return pImpl->connectedClients;
}

std::chrono::steady_clock::time_point WebDashboard::getLastUpdate() const {
    return pImpl->lastUpdate;
}

void WebDashboard::setUpdateCallback(UpdateCallback callback) {
    pImpl->updateCallback = std::move(callback);
}

void WebDashboard::addCustomEndpoint(const std::string& path, std::function<std::string(const std::string&)> handler) {
    std::lock_guard<std::mutex> lock(pImpl->customEndpointsMutex);
    pImpl->customEndpoints[path] = std::move(handler);
}

// Private helper methods
void WebDashboard::acceptConnections() {
    // Start accepting new connections
    if (pImpl->acceptor) {
        pImpl->acceptor->async_accept([this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                if (enableWebSocketUpgrade(std::move(socket))) {
                    // Continue accepting more connections
                    acceptConnections();
                } else {
                    // Connection rejected, still accept next
                    acceptConnections();
                }
            } else {
                LOG_DEBUG("Accept error: " + ec.message());
            }
        });
    }
}

std::string WebDashboard::getMetricsJSON() {
    json metrics;
    
    // Copy metrics safely
    {
        std::lock_guard<std::mutex> lock(pImpl->metricsMutex);
        for (const auto& pair : pImpl->metrics) {
            metrics[pair.first] = pair.second;
        }
    }
    
    metrics["connected_clients"] = pImpl->connectedClients;
    metrics["total_requests"] = pImpl->totalRequests;
    metrics["average_response_time"] = pImpl->averageResponseTime;
    metrics["timestamp"] = std::chrono::system_clock::now();
    
    return metrics.dump(2);
}

std::string WebDashboard::getHealthJSON() {
    return getHealthStatus();
}

std::string WebDashboard::getStatusJSON() {
    json status;
    status["name"] = "NLM WebDashboard";
    status["version"] = "1.0.0";
    status["running"] = pImpl->running;
    status["port"] = pImpl->port;
    status["uptime"] = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - pImpl->startTime).count();
    status["clients"] = pImpl->connectedClients;
    
    return status.dump(2);
}

std::string WebDashboard::getNotFoundHTML() {
    return R"(
<!DOCTYPE html>
<html>
<head><title>404 Not Found</title></head>
<body><h1>404 Not Found</h1><p>The requested resource was not found on this server.</p></body>
</html>
)";
}

std::string WebDashboard::getBadRequestHTML() {
    return R"(
<!DOCTYPE html>
<html>
<head><title>400 Bad Request</title></head>
<body><h1>400 Bad Request</h1><p>The request could not be understood by the server.</p></body>
</html>
)";
}

std::string WebDashboard::getCommand(const std::string& body) {
    // Handle command requests
    try {
        json command = json::parse(body);
        
        std::string commandType = command.value("command", "");
        
        if (commandType == "export") {
            ExportOptions options;
            if (command.contains("output_path")) {
                options.outputPath = command["output_path"];
            }
            visualizationManager->exportData(options);
            return R"({"status": "success", "message": "Export initiated"})";
        } else if (commandType == "analyze") {
            AnalysisOptions options;
            visualizationManager->analyzeData(options);
            return R"({"status": "success", "message": "Analysis initiated"})";
        } else if (commandType == "restart") {
            stop();
            return R"({"status": "success", "message": "Restart initiated"})";
        }
        
        return R"({"status": "error", "message": "Unknown command"})";
        
    } catch (const std::exception& e) {
        return R"({"status": "error", "message": "Invalid command format"})";
    }
}

std::string WebDashboard::generateClientId() {
    // Generate unique client ID
    static std::atomic<int> counter{0};
    return "client_" + std::to_string(++counter);
}

json WebDashboard::getPerformanceMetrics() {
    json perf;
    perf["total_requests"] = pImpl->totalRequests;
    perf["average_response_time"] = pImpl->averageResponseTime;
    perf["connected_clients"] = pImpl->connectedClients;
    perf["max_connections"] = pImpl->maxConnections;
    
    return perf;
}

void WebDashboard::readWebSocketMessages(const std::string& clientId, std::unique_ptr<websocket::stream<tcp::socket>> ws) {
    // Read WebSocket messages
    ws->async_read(
        boost::asio::buffer(pImpl->readBuffer),
        [this, clientId, ws = std::move(ws)](const boost::system::error_code& ec, size_t bytesTransferred) {
            if (!ec) {
                // Process message
                std::string message(pImpl->readBuffer.data(), bytesTransferred);
                processWebSocketMessage(clientId, message);
                
                // Read more messages
                readWebSocketMessages(clientId, std::move(ws));
            } else {
                disconnectWebSocketClient(clientId);
            }
        }
    );
}

void WebDashboard::processWebSocketMessage(const std::string& clientId, const std::string& message) {
    try {
        json data = json::parse(message);
        
        std::string type = data.value("type", "");
        
        if (type == "ping") {
            // Respond to ping
            json response;
            response["type"] = "pong";
            response["timestamp"] = std::chrono::system_clock::now();
            
            std::string responseStr = response.dump();
            broadcastMessage(responseStr, clientId);
        } else if (type == "metric_update") {
            // Update metrics from WebSocket
            for (auto& pair : data.items()) {
                if (pair.first != "type" && pair.first != "timestamp") {
                    addMetric(pair.first, pair.value());
                }
            }
        }
        
    } catch (const std::exception& e) {
        LOG_DEBUG("Error processing WebSocket message: " + std::string(e.what()));
    }
}

// Static member definitions
std::mutex WebDashboard::Impl::clientsMutex;
std::mutex WebDashboard::Impl::customEndpointsMutex;

} // namespace nlm
