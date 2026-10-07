// NLM Web Interface
// Web server for real-time monitoring and visualization of NLM simulations

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <nlohmann/json.hpp>

namespace nlm {

using json = nlohmann::json;

class WebSocketConnection {
public:
    virtual ~WebSocketConnection() = default;
    virtual void send(const std::string& message) = 0;
    virtual bool isConnected() const = 0;
};

class WebSocketServer {
public:
    WebSocketServer(int port, int backlog = 10);
    ~WebSocketServer();
    
    void start();
    void stop();
    void setMessageHandler(std::function<void(const std::string&, WebSocketConnection*)> handler);
    
    void broadcast(const std::string& message);
    void addConnection(std::shared_ptr<WebSocketConnection> connection);
    void removeConnection(WebSocketConnection* connection);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

class SimulationMonitor {
public:
    SimulationMonitor();
    ~SimulationMonitor() = default;
    
    void update(const std::string& simulationId, const json& metrics);
    json getMetrics(const std::string& simulationId) const;
    json getAllMetrics() const;
    std::vector<std::string> getActiveSimulations() const;
    
    void startMonitoring(const std::string& simulationId);
    void stopMonitoring(const std::string& simulationId);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

class VisualizationServer {
public:
    VisualizationServer(int port, const std::string& rootPath = "public");
    ~VisualizationServer();
    
    void start();
    void stop();
    void setDataProvider(std::function<json(const std::string&)> provider);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

class NLMWebInterface {
public:
    NLMWebInterface(int websocketPort = 8080, int httpPort = 8081);
    ~NLMWebInterface();
    
    void start();
    void stop();
    void setSimulationManager(std::shared_ptr<void> simulationManager);
    
    void broadcastSimulationUpdate(const std::string& simulationId, const json& metrics);
    json getSystemStats() const;
    json getActiveSessions() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

// Command line interface for web server
class WebServerCLI {
public:
    static int run(int argc, char** argv);
    
private:
    static void printHelp();
    static void runServer(int port, const std::string& mode, const std::string& configFile);
    static void runInteractiveServer(int port);
};

} // namespace nlm
