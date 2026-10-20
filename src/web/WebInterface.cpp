#include "WebInterface.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>
#include <regex>
#include <ctime>

namespace nlm {

namespace {
    // Default web interface configuration
    const WebInterfaceConfig defaultWebConfig = {
        .enableWebInterface = true,
        .host = "localhost",
        .port = 8080,
        .basePath = "/",
        .staticPath = "./web/static",
        .templatePath = "./web/templates",
        .enableSSE = true,
        .enableWebSocket = false,
        .enableCORS = true,
        .allowedOrigins = {"*"},
        .requestTimeout = std::chrono::milliseconds(30000),
        .maxConcurrentConnections = 100,
        .sslCertificate = "",
        .sslKey = "",
        .enableAuthentication = false,
        .authUsername = "admin",
        .authPasswordHash = "",
        .apiKey = "",
        .enableLogging = true,
        .logLevel = "info"
    };
}

struct WebInterface::Impl {
    std::vector<std::shared_ptr<WebClientConnection>> connections;
    std::mutex connectionsMutex;
    std::unordered_map<std::string, RequestHandler> requestHandlers;
    std::unordered_map<std::string, std::pair<std::string, RequestHandler>> methodPathHandlers;
    WebSocketHandler webSocketHandler;
    std::vector<EventHandler> eventHandlers;
    std::chrono::high_resolution_clock::time_point lastCleanup;
    int serverSocketFD;
    bool serverRunning;
    bool acceptConnections;
    std::thread acceptThread;
    std::vector<std::thread> clientThreads;
    std::atomic<bool> shouldStop;
    std::chrono::milliseconds requestTimeout;
    std::string host;
    int port;
    std::string basePath;
    
    Impl() : serverSocketFD(-1), serverRunning(false), acceptConnections(true),
             shouldStop(false), requestTimeout(30000), host("localhost"), port(8080) {
        lastCleanup = std::chrono::high_resolution_clock::now();
    }
};

WebInterface::WebInterface() : running_(false), acceptConnections_(true) {
    pImpl = std::make_unique<Impl>();
}

WebInterface::~WebInterface() {
    stop();
}

bool WebInterface::initialize(const WebInterfaceConfig& config) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    config_ = config;
    
    // Validate configuration
    if (config_.port < 1 || config_.port > 65535) {
        NLM_LOG_ERROR("Invalid port number: " + std::to_string(config_.port));
        return false;
    }
    
    if (config_.maxConcurrentConnections == 0) {
        NLM_LOG_ERROR("Max concurrent connections must be greater than 0");
        return false;
    }
    
    // Store configuration
    pImpl->host = config_.host;
    pImpl->port = config_.port;
    pImpl->basePath = config_.basePath;
    pImpl->requestTimeout = config_.requestTimeout;
    
    // Register default API endpoints
    registerMonitoringAPI();
    registerVisualizationAPI();
    registerExportAPI();
    registerSystemAPI();
    
    // Create static file directory
    serveStaticFiles(config_.staticPath);
    
    NLM_LOG_INFO("WebInterface initialized on " + config_.host + ":" + std::to_string(config_.port));
    return true;
}

void WebInterface::start() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (!running_) {
        running_ = true;
        pImpl->shouldStop = false;
        pImpl->serverRunning = true;
        
        // Start accept thread
        pImpl->acceptThread = std::thread(&WebInterface::acceptLoop, this);
        
        // Set thread name for debugging
        pthread_setname_np(pImpl->acceptThread.native_handle(), "web_accept");
        
        NLM_LOG_INFO("WebInterface started");
    }
}

void WebInterface::stop() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    if (running_) {
        running_ = false;
        pImpl->shouldStop = true;
        pImpl->serverRunning = false;
        acceptConnections_ = false;
        
        // Signal accept thread to stop
        if (pImpl->serverSocketFD != -1) {
            char signal = 'x';
            send(pImpl->serverSocketFD, &signal, 1, MSG_DONTWAIT);
        }
        
        // Wait for accept thread to finish
        if (pImpl->acceptThread.joinable()) {
            pImpl->acceptThread.join();
        }
        
        // Stop client threads
        for (auto& clientThread : pImpl->clientThreads) {
            if (clientThread.joinable()) {
                clientThread.join();
            }
        }
        
        pImpl->clientThreads.clear();
        pImpl->connections.clear();
        
        NLM_LOG_INFO("WebInterface stopped");
    }
}

void WebInterface::configure(const WebInterfaceConfig& config) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    config_ = config;
    
    // Update stored configuration
    pImpl->host = config_.host;
    pImpl->port = config_.port;
    pImpl->basePath = config_.basePath;
    pImpl->requestTimeout = config_.requestTimeout;
    
    NLM_LOG_INFO("WebInterface configured");
}

void WebInterface::addRequestHandler(const std::string& path, const RequestHandler& handler) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    pImpl->requestHandlers[path] = handler;
    NLM_LOG_INFO("Added request handler for path: " + path);
}

void WebInterface::addRequestHandler(const std::string& method, const std::string& path,
                                     const RequestHandler& handler) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    std::string key = method + " " + path;
    pImpl->methodPathHandlers[key] = std::make_pair(method, handler);
    NLM_LOG_INFO("Added request handler for " + method + " " + path);
}

void WebInterface::addWebSocketHandler(const WebSocketHandler& handler) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    pImpl->webSocketHandler = handler;
    NLM_LOG_INFO("Added WebSocket handler");
}

void WebInterface::addEventHandler(const EventHandler& handler) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    pImpl->eventHandlers.push_back(handler);
    NLM_LOG_INFO("Added event handler");
}

void WebInterface::registerMonitoringAPI() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    // GET /api/metrics
    addRequestHandler("GET", APIEndpoints::Monitoring::GET_METRICS,
        [this](const WebRequest& request, WebResponse& response) {
            response.setContentType("application/json");
            response.body = generateMetricsResponse();
            return true;
        });
    
    // GET /api/performance
    addRequestHandler("GET", APIEndpoints::Monitoring::GET_PERFORMANCE,
        [this](const WebRequest& request, WebResponse& response) {
            response.setContentType("application/json");
            response.body = generatePerformanceResponse();
            return true;
        });
    
    // GET /api/realtime
    addRequestHandler("GET", APIEndpoints::Monitoring::GET_REAL_TIME,
        [this](const WebRequest& request, WebResponse& response) {
            response.setContentType("application/json");
            response.body = generateRealTimeResponse();
            return true;
        });
    
    // GET /api/history
    addRequestHandler("GET", APIEndpoints::Monitoring::GET_HISTORY,
        [this](const WebRequest& request, WebResponse& response) {
            response.setContentType("application/json");
            response.body = generateHistoryResponse();
            return true;
        });
}

void WebInterface::registerVisualizationAPI() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    // GET /api/visualizations
    addRequestHandler("GET", APIEndpoints::Visualization::GET_VISUALIZATIONS,
        [this](const WebRequest& request, WebResponse& response) {
            response.setContentType("application/json");
            response.body = "{\"visualizations\": []}"; // Placeholder
            return true;
        });
    
    // GET /api/visualizations/{name}/data
    // Note: This is a simplified pattern matching - real implementation would use regex
    addRequestHandler("GET", APIEndpoints::Visualization::GET_VIZ_DATA,
        [this](const WebRequest& request, WebResponse& response) {
            response.setContentType("application/json");
            response.body = "{\"data\": []}"; // Placeholder
            return true;
        });
}

void WebInterface::registerExportAPI() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    // GET /api/exports
    addRequestHandler("GET", APIEndpoints::Export::GET_EXPORTS,
        [this](const WebRequest& request, WebResponse& response) {
            response.setContentType("application/json");
            response.body = "{\"exports\": []}"; // Placeholder
            return true;
        });
}

void WebInterface::registerSystemAPI() {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    // GET /api/status
    addRequestHandler("GET", APIEndpoints::System::GET_STATUS,
        [this](const WebRequest& request, WebResponse& response) {
            response.setContentType("application/json");
            response.body = getServerStatus();
            return true;
        });
    
    // GET /api/health
    addRequestHandler("GET", APIEndpoints::System::GET_HEALTH,
        [this](const WebRequest& request, WebResponse& response) {
            response.setContentType("application/json");
            nlohmann::json health;
            health["status"] = "healthy";
            health["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            health["version"] = "1.0.0";
            response.body = health.dump();
            return true;
        });
}

void WebInterface::serveStaticFiles(const std::string& path) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
        // Create a handler for static files
        addRequestHandler("GET", APIEndpoints::Static::STATIC,
            [this, path](const WebRequest& request, WebResponse& response) {
                // Extract path from request path_params
                std::string filePath = request.path_params["path"];
                return serveStaticFile(path + "/" + filePath, response);
            });
    }
}

void WebInterface::broadcastMessage(const WebSocketMessage& message) {
    std::lock_guard<std::mutex> lock(pImpl->connectionsMutex);
    
    std::string jsonData = message.toJSON().dump();
    
    for (auto& connection : pImpl->connections) {
        if (connection->authenticated) {
            sendToConnection(connection, message);
        }
    }
}

void WebInterface::sendToConnection(std::shared_ptr<WebClientConnection> connection,
                                    const WebSocketMessage& message) {
    // Placeholder for WebSocket message sending
    // In a real implementation, this would use the actual socket
}

void WebInterface::fireEvent(const WebEvent& event) {
    std::lock_guard<std::mutex> lock(pImpl->connectionsMutex);
    
    for (const auto& handler : pImpl->eventHandlers) {
        try {
            handler(event);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Event handler exception: " + std::string(e.what()));
        }
    }
}

bool WebInterface::serveStaticFile(const std::string& path, WebResponse& response) const {
    if (!std::filesystem::exists(path) || !std::filesystem::is_regular_file(path)) {
        response.setStatus(404, "File not found");
        response.body = "File not found: " + path;
        response.setContentType("text/plain");
        return false;
    }
    
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        response.setStatus(500, "Cannot open file");
        response.body = "Cannot open file: " + path;
        response.setContentType("text/plain");
        return false;
    }
    
    // Read file content
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
        response.setStatus(500, "Cannot read file");
        response.body = "Cannot read file: " + path;
        response.setContentType("text/plain");
        return false;
    }
    
    response.body = std::string(buffer.begin(), buffer.end());
    
    // Determine content type based on file extension
    size_t dotPos = path.find_last_of('.');
    if (dotPos != std::string::npos) {
        std::string extension = path.substr(dotPos + 1);
        if (extension == "html" || extension == "htm") {
            response.setContentType("text/html");
        } else if (extension == "css") {
            response.setContentType("text/css");
        } else if (extension == "js") {
            response.setContentType("application/javascript");
        } else if (extension == "json") {
            response.setContentType("application/json");
        } else if (extension == "png") {
            response.setContentType("image/png");
        } else if (extension == "jpg" || extension == "jpeg") {
            response.setContentType("image/jpeg");
        } else if (extension == "svg") {
            response.setContentType("image/svg+xml");
        } else {
            response.setContentType("application/octet-stream");
        }
    }
    
    NLM_LOG_INFO("Served static file: " + path);
    return true;
}

std::string WebInterface::getServerStatus() const {
    nlohmann::json status;
    status["status"] = "running";
    status["host"] = pImpl->host;
    status["port"] = pImpl->port;
    status["connections"] = getConnectionCount();
    status["uptime"] = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - startTime_).count();
    status["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    return status.dump();
}

size_t WebInterface::getConnectionCount() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return pImpl->connections.size();
}

std::vector<std::string> WebInterface::getConnectedClients() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    std::vector<std::string> clients;
    
    for (const auto& connection : pImpl->connections) {
        clients.push_back(connection->clientIP + ":" + std::to_string(connection->clientPort));
    }
    
    return clients;
}

bool WebInterface::exportWebInterface(const std::string& filename, const std::string& format) {
    std::lock_guard<std::shared_mutex> lock(mutex_);
    
    std::string extension = ".html"; // Default to HTML
    if (format == "json") {
        extension = ".json";
    } else if (format == "css") {
        extension = ".css";
    } else if (format == "js") {
        extension = ".js";
    }
    
    std::string actualFilename = filename;
    if (actualFilename.find(extension) == std::string::npos) {
        actualFilename += extension;
    }
    
    std::ofstream file(actualFilename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for export: " + actualFilename);
        return false;
    }
    
    if (format == "json") {
        nlohmann::json jsonData;
        jsonData["host"] = config_.host;
        jsonData["port"] = config_.port;
        jsonData["apiEndpoints"] = nlohmann::json::array();
        jsonData["apiEndpoints"].push_back(APIEndpoints::Monitoring::GET_METRICS);
        jsonData["apiEndpoints"].push_back(APIEndpoints::Monitoring::GET_PERFORMANCE);
        jsonData["apiEndpoints"].push_back(APIEndpoints::Monitoring::GET_REAL_TIME);
        
        file << jsonData.dump(2);
    } else {
        file << "<!DOCTYPE html>\n<html>\n<head>\n<title>NLM Web Interface</title>\n";
        file << "<meta charset=\"UTF-8\">\n";
        file << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        file << "<title>NLM Monitoring Dashboard</title>\n";
        file << "<style>\nbody { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }\n";
        file << "h1 { color: #333; }\n";
        file << ".metric { background: white; padding: 15px; margin: 10px 0; border-radius: 5px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }\n";
        file << "</style>\n";
        file << "</head>\n<body>\n";
        file << "<h1>NLM Neural Network Monitoring</h1>\n";
        file << "<div class=\"metric\">\n";
        file << "<h2>System Status</h2>\n";
        file << "<p>Status: Running</p>\n";
        file << "<p>Host: " << config_.host << "</p>\n";
        file << "<p>Port: " << config_.port << "</p>\n";
        file << "<p>Connections: " << getConnectionCount() << "</p>\n";
        file << "</div>\n";
        file << "<div class=\"metric\">\n";
        file << "<h2>API Endpoints</h2>\n";
        file << "<ul>\n";
        file << "<li><a href=\"" << APIEndpoints::Monitoring::GET_METRICS << "\">Metrics API</a></li>\n";
        file << "<li><a href=\"" << APIEndpoints::Monitoring::GET_PERFORMANCE << ">Performance API</a></li>\n";
        file << "<li><a href=\"" << APIEndpoints::Monitoring::GET_REAL_TIME << ">Real-time API</a></li>\n";
        file << "<li><a href=\"" << APIEndpoints::Monitoring::GET_HISTORY << ">History API</a></li>\n";
        file << "</ul>\n";
        file << "</div>\n";
        file << "</body>\n</html>\n";
    }
    
    file.close();
    NLM_LOG_INFO("Web interface exported to " + actualFilename);
    return true;
}

void WebInterface::startGlobal(const WebInterfaceConfig& config) {
    getInstance().start();
}

void WebInterface::stopGlobal() {
    getInstance().stop();
}

bool WebInterface::isGlobalRunning() {
    return getInstance().isRunning();
}

} // namespace nlm
