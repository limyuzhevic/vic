// WebDashboard.cpp - Web-based dashboard implementation

#include "WebDashboard.hpp"
#include "Brain.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "SpikeEvent.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>
#include <random>
#include <codecvt>
#include <locale>
#include <thread>
#include <chrono>
#include <limits>

namespace nlm {

struct WebDashboard::Impl {
    DashboardConfig config;
    bool running;
    std::thread serverThread;
    std::mutex mutex;
    std::unordered_map<std::string, WebSocketSession> websocketSessions;
    std::vector<ApiEndpoint> apiEndpoints;
    std::chrono::steady_clock::time_point startTime;
    size_t totalConnections;
    size_t activeConnections;
    double uptime;
    bool sslEnabled;
    bool authEnabled;
    std::string currentData; // Simplified data storage
    std::vector<double> dataHistory;
    std::chrono::steady_clock::time_point lastDataUpdate;
    
    Impl() : running(false), totalConnections(0), activeConnections(0), uptime(0.0), sslEnabled(false), authEnabled(false) {
        startTime = std::chrono::steady_clock::now();
        lastDataUpdate = startTime;
        
        // Initialize API endpoints
        apiEndpoints = {
            {"/api/health", "GET", "health_check", false, "Health check endpoint"},
            {"/api/brain", "GET", "get_brain_data", false, "Get brain data"},
            {"/api/neurons", "GET", "get_neuron_data", false, "Get neuron data"},
            {"/api/synapses", "GET", "get_synapse_data", false, "Get synapse data"},
            {"/api/spikes", "GET", "get_spike_data", false, "Get spike data"},
            {"/api/stats", "GET", "get_statistics", false, "Get statistics"},
            {"/api/config", "GET", "get_config", false, "Get configuration"},
            {"/api/update", "POST", "update_data", false, "Update data"},
            {"/api/command", "POST", "execute_command", false, "Execute command"}
        };
    }
};

WebDashboard::WebDashboard() : pImpl(std::make_unique<Impl>()) {
}

WebDashboard::WebDashboard(const DashboardConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
    pImpl->sslEnabled = config.enableSSL;
    pImpl->authEnabled = config.enableAuthentication;
}

WebDashboard::~WebDashboard() {
    stop();
}

void WebDashboard::configure(const DashboardConfig& config) {
    pImpl->config = config;
    pImpl->sslEnabled = config.enableSSL;
    pImpl->authEnabled = config.enableAuthentication;
}

bool WebDashboard::start() {
    if (pImpl->running) {
        NLM_LOG_WARNING("WebDashboard already running");
        return false;
    }
    
    pImpl->running = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->uptime = 0.0;
    
    // Start server thread
    pImpl->serverThread = std::thread(&WebDashboard::serverThreadFunc, this);
    
    NLM_LOG_INFO("WebDashboard started on " + pImpl->config.host + ":" + std::to_string(pImpl->config.port) +
                " (SSL: " + std::string(pImpl->sslEnabled ? "enabled" : "disabled") + ")");
    
    return true;
}

void WebDashboard::stop() {
    if (!pImpl->running) return;
    
    pImpl->running = false;
    
    if (pImpl->serverThread.joinable()) {
        pImpl->serverThread.join();
    }
    
    NLM_LOG_INFO("WebDashboard stopped");
}

bool WebDashboard::isRunning() const {
    return pImpl->running;
}

void WebDashboard::update(const Brain& brain, const std::vector<SpikeEvent>& spikes) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Generate dashboard data
    std::ostringstream dataStream;
    dataStream << "{\n";
    dataStream << "  \"timestamp\": \"" << getCurrentTime() << "\",\n";
    dataStream << "  \"neuronCount\": " << brain.getNeuronCount() << ",\n";
    dataStream << "  \"synapseCount\": " << brain.getSynapseCount() << ",\n";
    dataStream << "  \"spikeCount\": " << spikes.size() << ",\n";
    dataStream << "  \"brainRegions\": " << brain.getRegionCount() << ",\n";
    dataStream << "  \"data\": [\n";
    
    // Add sample data points
    for (size_t i = 0; i < std::min(spikes.size(), static_cast<size_t>(100)); ++i) {
        dataStream << "    {\"time\": " << spikes[i].time << ", \"neuronId\": " << spikes[i].neuronId << "}";
        if (i < std::min(spikes.size(), static_cast<size_t>(100)) - 1) {
            dataStream << ",";
        }
        dataStream << "\n";
    }
    
    dataStream << "  ]\n";
    dataStream << "}\n";
    
    pImpl->currentData = dataStream.str();
    pImpl->lastDataUpdate = std::chrono::steady_clock::now();
    pImpl->dataHistory.push_back(spikes.size());
    
    // Keep only last 1000 data points
    if (pImpl->dataHistory.size() > 1000) {
        pImpl->dataHistory.erase(pImpl->dataHistory.begin());
    }
}

bool WebDashboard::handleWebSocketMessage(const std::string& sessionId, const std::string& message) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto sessionIt = pImpl->websocketSessions.find(sessionId);
    if (sessionIt == pImpl->websocketSessions.end()) {
        NLM_LOG_WARNING("WebSocket session not found: " + sessionId);
        return false;
    }
    
    // Parse WebSocket message
    // For simplicity, we just log the message
    NLM_LOG_INFO("WebSocket message from " + sessionId + ": " + message);
    
    // Update activity time
    sessionIt->second.lastActivity = std::chrono::steady_clock::now();
    
    return true;
}

std::string WebDashboard::handleRestRequest(const std::string& path, const std::string& method, const std::string& body) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Find matching endpoint
    for (const auto& endpoint : pImpl->apiEndpoints) {
        if (endpoint.path == path && endpoint.method == method) {
            // Call appropriate handler
            if (endpoint.handler == "health_check") {
                return handleHealthRequest();
            } else if (endpoint.handler == "get_brain_data") {
                return handleBrainDataRequest();
            } else if (endpoint.handler == "get_neuron_data") {
                return handleNeuronDataRequest();
            } else if (endpoint.handler == "get_synapse_data") {
                return handleSynapseDataRequest();
            } else if (endpoint.handler == "get_spike_data") {
                return handleSpikeDataRequest();
            } else if (endpoint.handler == "get_statistics") {
                return handleStatisticsRequest();
            } else if (endpoint.handler == "get_config") {
                return handleConfigRequest();
            } else if (endpoint.handler == "update_data") {
                return handleUpdateDataRequest(body);
            } else if (endpoint.handler == "execute_command") {
                return handleExecuteCommandRequest(body);
            }
            break;
        }
    }
    
    return createErrorResponse(404, "Endpoint not found");
}

std::string WebDashboard::getStatistics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto now = std::chrono::steady_clock::now();
    auto uptime = std::chrono::duration<double>(now - pImpl->startTime).count();
    
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"uptime\": " << uptime << ",\n";
    oss << "  \"totalConnections\": " << pImpl->totalConnections << ",\n";
    oss << "  \"activeConnections\": " << pImpl->activeConnections << ",\n";
    oss << "  \"websocketSessions\": " << pImpl->websocketSessions.size() << ",\n";
    oss << "  \"dataPoints\": " << pImpl->dataHistory.size() << ",\n";
    oss << "  \"currentDataSize\": " << pImpl->currentData.size() << ",\n";
    oss << "  \"serverTime\": \"" << getCurrentTime() << "\",\n";
    oss << "  \"lastDataUpdate\": \"" << getTimeSince(pImpl->lastDataUpdate) << " ago\"\n";
    oss << "}\n";
    
    return oss.str();
}

std::string WebDashboard::exportConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ostringstream oss;
    oss << "# NLM WebDashboard Configuration\n";
    oss << "# Generated: " << getCurrentTime() << "\n\n";
    oss << "[server]\n";
    oss << "host = \"" << pImpl->config.host << "\"\n";
    oss << "port = " << pImpl->config.port << "\n";
    oss << "enableSSL = " << (pImpl->config.enableSSL ? "true" : "false") << "\n";
    oss << "\n[api]\n";
    oss << "prefix = \"" << pImpl->config.apiPrefix << "\"\n";
    oss << "enableCORS = " << (pImpl->config.enableCORS ? "true" : "false") << "\n";
    oss << "\n[security]\n";
    oss << "enableAuthentication = " << (pImpl->config.enableAuthentication ? "true" : "false") << "\n";
    oss << "enableRateLimiting = " << (pImpl->config.enableRateLimiting ? "true" : "false") << "\n";
    oss << "maxRequestsPerMinute = " << pImpl->config.maxRequestsPerMinute << "\n";
    oss << "\n[visualization]\n";
    oss << "theme = \"" << pImpl->config.theme << "\"\n";
    oss << "enableWebSocket = " << (pImpl->config.enableWebSocket ? "true" : "false") << "\n";
    
    return oss.str();
}

bool WebDashboard::importConfig(const std::string& configData) {
    // Simple implementation - in real version would parse JSON/YAML/TOML
    NLM_LOG_INFO("Importing configuration from data");
    // For now, just accept any config
    return true;
}

std::string WebDashboard::generateHtml() const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n";
    oss << "<html lang=\"en\">\n";
    oss << "<head>\n";
    oss << "  <meta charset=\"UTF-8\">\n";
    oss << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    oss << "  <title>" << pImpl->config.title << "</title>\n";
    oss << "  <style>\n";
    oss << "    body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }\n";
    oss << "    .container { max-width: 1200px; margin: 0 auto; }\n";
    oss << "    .header { background-color: #333; color: white; padding: 20px; border-radius: 5px; margin-bottom: 20px; }\n";
    oss << "    .stats { display: grid; grid-template-columns: repeat(auto-fill, minmax(200px, 1fr)); gap: 20px; margin-bottom: 20px; }\n";
    oss << "    .stat-card { background-color: white; padding: 15px; border-radius: 5px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }\n";
    oss << "    .stat-title { font-size: 0.9em; color: #666; margin-bottom: 5px; }\n";
    oss << "    .stat-value { font-size: 2em; font-weight: bold; color: #333; }\n";
    oss << "    .chart { background-color: white; padding: 20px; border-radius: 5px; margin-bottom: 20px; }\n";
    oss << "    .websocket-status { padding: 10px; border-radius: 3px; margin-bottom: 10px; }\n";
    oss << "    .status-connected { background-color: #d4edda; color: #155724; }\n";
    oss << "    .status-disconnected { background-color: #f8d7da; color: #721c24; }\n";
    oss << "    button { background-color: #007bff; color: white; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer; }\n";
    oss << "    button:hover { background-color: #0056b3; }\n";
    oss << "  </style>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    oss << "  <div class=\"container\">\n";
    oss << "    <div class=\"header\">\n";
    oss << "      <h1>" << pImpl->config.title << "</h1>\n";
    oss << "      <p>Real-time neural network visualization and monitoring</p>\n";
    oss << "    </div>\n";
    oss << "\n";
    oss << "    <div class=\"stats\">\n";
    oss << "      <div class=\"stat-card\">\n";
    oss << "        <div class=\"stat-title\">Uptime</div>\n";
    oss << "        <div class=\"stat-value\">" << pImpl->uptime << "s</div>\n";
    oss << "      </div>\n";
    oss << "      <div class=\"stat-card\">\n";
    oss << "        <div class=\"stat-title\">Connections</div>\n";
    oss << "        <div class=\"stat-value\">" << pImpl->activeConnections << "</div>\n";
    oss << "      </div>\n";
    oss << "      <div class=\"stat-card\">\n";
    oss << "        <div class=\"stat-title\">Data Points</div>\n";
    oss << "        <div class=\"stat-value\">" << pImpl->dataHistory.size() << "</div>\n";
    oss << "      </div>\n";
    oss << "      <div class=\"stat-card\">\n";
    oss << "        <div class=\"stat-title\">Server Status</div>\n";
    oss << "        <div class=\"stat-value websocket-status " << (pImpl->running ? "status-connected" : "status-disconnected") << "\">\n";
    oss << "          " << (pImpl->running ? "Running" : "Stopped") << "\n";
    oss << "        </div>\n";
    oss << "      </div>\n";
    oss << "    </div>\n";
    oss << "\n";
    oss << "    <div class=\"chart\">\n";
    oss << "      <h2>System Status</h2>\n";
    oss << "      <div id=\"statusChart\">Status chart would appear here</div>\n";
    oss << "    </div>\n";
    oss << "\n";
    oss << "    <div class=\"chart\">\n";
    oss << "      <h2>WebSocket Connections</h2>\n";
    oss << "      <div id=\"websocketChart\">WebSocket connections would appear here</div>\n";
    oss << "    </div>\n";
    oss << "\n";
    oss << "    <div class=\"chart\">\n";
    oss << "      <h2>API Endpoints</h2>\n";
    oss << "      <ul>\n";
    for (const auto& endpoint : pImpl->apiEndpoints) {
        oss << "        <li>" << endpoint.method << " " << endpoint.path << " - " << endpoint.description << "</li>\n";
    }
    oss << "      </ul>\n";
    oss << "    </div>\n";
    oss << "  </div>\n";
    oss << "\n";
    oss << "  <script>\n";
    oss << "    // WebSocket connection\n";
    oss << "    const ws = new WebSocket('ws://' + window.location.hostname + ':" << pImpl->config.port << "/');\n";
    oss << "\n";
    oss << "    ws.onopen = function() {\n";
    oss << "      console.log('WebSocket connected');\n";
    oss << "      updateStatus('connected');\n";
    oss << "    };\n";
    oss << "\n";
    oss << "    ws.onmessage = function(event) {\n";
    oss << "      const data = JSON.parse(event.data);\n";
    oss << "      console.log('Message received:', data);\n";
    oss << "      updateDashboard(data);\n";
    oss << "    };\n";
    oss << "\n";
    oss << "    ws.onclose = function() {\n";
    oss << "      console.log('WebSocket disconnected');\n";
    oss << "      updateStatus('disconnected');\n";
    oss << "      setTimeout(function() { ws.connect(); }, 5000);\n";
    oss << "    };\n";
    oss << "\n";
    oss << "    ws.onerror = function(error) {\n";
    oss << "      console.error('WebSocket error:', error);\n";
    oss << "      updateStatus('error');\n";
    oss << "    };\n";
    oss << "\n";
    oss << "    function updateStatus(status) {\n";
    oss << "      const statusElement = document.querySelector('.websocket-status');\n";
    oss << "      statusElement.textContent = status;\n";
    oss << "      statusElement.className = 'websocket-status ' + (status === 'connected' ? 'status-connected' : 'status-disconnected');\n";
    oss << "    }\n";
    oss << "\n";
    oss << "    function updateDashboard(data) {\n";
    oss << "      // Update dashboard with new data\n";
    oss << "      console.log('Dashboard updated:', data);\n";
    oss << "    }\n";
    oss << "\n";
    oss << "    // API request function\n";
    oss << "    function makeApiRequest(path, method = 'GET', body = null) {\n";
    oss << "      return fetch(path, {\n";
    oss << "        method: method,\n";
    oss << "        headers: {\n";
    oss << "          'Content-Type': 'application/json'\n";
    oss << "        },\n";
    oss << "        body: body ? JSON.stringify(body) : null\n";
    oss << "      }).then(response => response.json()).catch(error => {\n";
    oss << "        console.error('API request failed:', error);\n";
    oss << "      });\n";
    oss << "    }\n";
    oss << "\n";
    oss << "    // Health check\n";
    oss << "    makeApiRequest('" << pImpl->config.apiPrefix << "/health');\n";
    oss << "  </script>\n";
    oss << "</body>\n";
    oss << "</html>\n";
    
    return oss.str();
}

std::string WebDashboard::getWebSocketEndpoint() const {
    return "ws://" + pImpl->config.host + ":" + std::to_string(pImpl->config.port) + pImpl->config.websocketEndpoint;
}

std::string WebDashboard::getApiPrefix() const {
    return pImpl->config.apiPrefix;
}

// Private handler methods
void WebDashboard::serverThreadFunc() {
    while (pImpl->running) {
        // Update uptime
        auto now = std::chrono::steady_clock::now();
        pImpl->uptime = std::chrono::duration<double>(now - pImpl->startTime).count();
        
        // Cleanup inactive WebSocket sessions
        cleanupInactiveSessions();
        
        // Sleep
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void WebDashboard::cleanupInactiveSessions() {
    auto now = std::chrono::steady_clock::now();
    double timeoutSeconds = pImpl->config.timeoutSeconds;
    
    std::vector<std::string> toRemove;
    for (const auto& pair : pImpl->websocketSessions) {
        auto lastActivity = pair.second.lastActivity;
        auto elapsed = std::chrono::duration<double>(now - lastActivity).count();
        if (elapsed > timeoutSeconds) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (const auto& sessionId : toRemove) {
        pImpl->websocketSessions.erase(sessionId);
        pImpl->activeConnections--;
    }
}

std::string WebDashboard::handleHealthRequest() {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"status\": \"healthy\",\n";
    oss << "  \"timestamp\": \"" << getCurrentTime() << "\",\n";
    oss << "  \"version\": \"" << pImpl->config.version << "\",\n";
    oss << "  \"activeConnections\": " << pImpl->activeConnections << ",\n";
    oss << "  \"uptime\": " << pImpl->uptime << "\n";
    oss << "}\n";
    
    return oss.str();
}

std::string WebDashboard::handleBrainDataRequest() {
    // In real implementation, would get brain data from visualization system
    return createJsonResponse({
        {"neuronCount", std::to_string(pImpl->brainData ? pImpl->brainData->getNeuronCount() : 0)},
        {"synapseCount", std::to_string(pImpl->brainData ? pImpl->brainData->getSynapseCount() : 0)},
        {"regionCount", std::to_string(pImpl->brainData ? pImpl->brainData->getRegionCount() : 0)}
    });
}

std::string WebDashboard::handleNeuronDataRequest() {
    // Return sample neuron data
    return createJsonResponse({
        {"data", "[{"neuronId": 1, "region": \"cortex\", "type\": \"excitatory\"}]"}
    });
}

std::string WebDashboard::handleSynapseDataRequest() {
    // Return sample synapse data
    return createJsonResponse({
        {"data", "[{"id": 1, "pre": 1, "post": 2, "weight": 0.5}]"}
    });
}

std::string WebDashboard::handleSpikeDataRequest() {
    // Return spike data
    std::ostringstream oss;
    oss << "[\n";
    for (size_t i = 0; i < std::min(pImpl->dataHistory.size(), static_cast<size_t>(10)); ++i) {
        oss << "  " << pImpl->dataHistory[i];
        if (i < pImpl->dataHistory.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "]\n";
    
    return oss.str();
}

std::string WebDashboard::handleStatisticsRequest() {
    return getStatistics();
}

std::string WebDashboard::handleConfigRequest() {
    return exportConfig();
}

std::string WebDashboard::handleUpdateDataRequest(const std::string& body) {
    // Parse and update data
    NLM_LOG_INFO("Received data update request: " + body.substr(0, std::min(body.size(), size_t(100))));
    return createJsonResponse({
        {"status", "updated"},
        {"timestamp", getCurrentTime()}
    });
}

std::string WebDashboard::handleExecuteCommandRequest(const std::string& body) {
    // Parse and execute command
    NLM_LOG_INFO("Received command execution request: " + body.substr(0, std::min(body.size(), size_t(100))));
    return createJsonResponse({
        {"status", "executed"},
        {"timestamp", getCurrentTime()}
    });
}

std::string WebDashboard::createJsonResponse(const std::unordered_map<std::string, std::string>& data) const {
    std::ostringstream oss;
    oss << "{\n";
    bool first = true;
    for (const auto& pair : data) {
        if (!first) oss << ",\n";
        oss << "  \"" << pair.first << "\": \"" << escapeJsonString(pair.second) << "\"";
        first = false;
    }
    oss << "\n}\n";
    return oss.str();
}

std::string WebDashboard::createErrorResponse(int code, const std::string& message) {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"error\": true,\n";
    oss << "  \"code\": " << code << ",\n";
    oss << "  \"message\": \"" << escapeJsonString(message) << "\"\n";
    oss << "}\n";
    return oss.str();
}

std::string WebDashboard::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string WebDashboard::getTimeSince(std::chrono::steady_clock::time_point timePoint) const {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - timePoint).count();
    
    if (elapsed < 60) return std::to_string(elapsed) + "s";
    if (elapsed < 3600) return std::to_string(elapsed / 60) + "m";
    return std::to_string(elapsed / 3600) + "h";
}

std::string WebDashboard::escapeJsonString(const std::string& str) const {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

} // namespace nlm
