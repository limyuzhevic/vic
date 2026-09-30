#pragma once

#include "VisualizationManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace nlm {

// Web-based dashboard with real-time updates
class WebDashboard {
public:
    struct WebSocketClient {
        boost::asio::ip::tcp::socket socket;
        std::string clientId;
        std::chrono::steady_clock::time_point lastPing;
        bool authenticated;
        std::unordered_map<std::string, std::string> subscriptions;
    };

    struct DashboardMetrics {
        double cpuUsage;
        double memoryUsage;
        double gpuUsage;
        uint64_t neuronCount;
        uint64_t spikeCount;
        double simulationFPS;
        double visualizationFPS;
        std::chrono::system_clock::time_point timestamp;
        std::unordered_map<std::string, double> customMetrics;
    };

    struct RealTimeUpdate {
        std::string updateType;
        std::string data;
        std::chrono::system_clock::time_point timestamp;
        std::string source;
    };

    WebDashboard(std::shared_ptr<VisualizationManager> manager);
    ~WebDashboard();

    // Disable copying, enable moving
    WebDashboard(const WebDashboard&) = delete;
    WebDashboard& operator=(const WebDashboard&) = delete;
    WebDashboard(WebDashboard&&) noexcept;
    WebDashboard& operator=(WebDashboard&&) noexcept;

    // Initialize web dashboard
    bool initialize(uint16_t port = 8080, bool enableWebSocket = true);

    // Web server management
    void start();
    void stop();
    bool isRunning() const;

    // HTTP endpoint handlers
    std::string handleHTTPGet(const std::string& path);
    std::string handleHTTPPost(const std::string& path, const std::string& body);
    bool handleWebSocketUpgrade(boost::asio::ip::tcp::socket&& socket);

    // WebSocket client management
    bool connectWebSocketClient(const std::string& clientId, const std::string& url);
    void disconnectWebSocketClient(const std::string& clientId);
    void broadcastMessage(const std::string& message, const std::string& clientId = "");
    void sendRealTimeUpdate(const RealTimeUpdate& update);

    // Dashboard configuration
    void setUpdateInterval(double interval);
    void setMetricRefreshRate(double rate);
    void addMetric(const std::string& name, double value);
    void setMetric(const std::string& name, double value);
    double getMetric(const std::string& name) const;

    // Dashboard features
    void enableRealTimeUpdates(bool enable);
    void setWebSocketCompression(bool enable);
    void setMaxConnections(size_t maxConnections);
    void setAuthenticationRequired(bool required);

    // Static web content
    static std::string getStaticHTML(const std::string& page = "dashboard");
    static std::string getJavaScript(const std::string& page = "dashboard");
    static std::string getCSS(const std::string& page = "dashboard");

    // Data export and analysis
    void exportDashboardData(const std::string& filepath);
    void generateReport(const std::string& reportPath);

    // Configuration
    void loadConfiguration(const std::string& configPath);
    void saveConfiguration() const;
    std::string getConfigurationJSON() const;

    // Health and monitoring
    bool isHealthy() const;
    std::string getHealthStatus() const;
    size_t getConnectedClients() const;
    std::chrono::steady_clock::time_point getLastUpdate() const;

    // Event handling
    using UpdateCallback = std::function<void(const RealTimeUpdate&)>;
    void setUpdateCallback(UpdateCallback callback);
    void addCustomEndpoint(const std::string& path, std::function<std::string(const std::string&)> handler);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
