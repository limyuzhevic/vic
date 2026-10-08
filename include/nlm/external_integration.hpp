// Integration interfaces for external systems

#ifndef NLM_EXTERNAL_INTEGRATION_HPP
#define NLM_EXTERNAL_INTEGRATION_HPP

#include "core/Logger/Logger.hpp"
#include "error_handling.hpp"
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <chrono>
#include <atomic>
#include <json/json.h>

namespace nlm {

// External system interface
class IExternalSystem {
public:
    virtual ~IExternalSystem() = default;
    
    // System information
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Connection management
    virtual bool connect() = 0;
    virtual bool disconnect() = 0;
    virtual bool isConnected() const = 0;
    
    // Configuration management
    virtual bool configure(const Json::Value& config) = 0;
    virtual Json::Value getConfiguration() const = 0;
    
    // Data exchange
    virtual bool sendData(const std::string& data) = 0;
    virtual std::string receiveData() = 0;
    
    // Error handling
    virtual bool handleError(const NLMException& error) = 0;
    
    // Status monitoring
    virtual Json::Value getStatus() const = 0;
    virtual bool isHealthy() const = 0;
    
    // Event handling
    virtual void setEventCallback(std::function<void(const std::string&, const Json::Value&)> callback) = 0;
};

// Database integration interface
class IDatabase : public IExternalSystem {
public:
    virtual ~IDatabase() = default;
    
    // Connection management
    virtual bool connect(const std::string& connectionString) = 0;
    virtual bool disconnect() = 0;
    
    // Data operations
    virtual bool insert(const std::string& collection, const Json::Value& document) = 0;
    virtual bool update(const std::string& collection, const std::string& id, const Json::Value& document) = 0;
    virtual bool remove(const std::string& collection, const std::string& id) = 0;
    virtual Json::Value findOne(const std::string& collection, const Json::Value& query) = 0;
    virtual std::vector<Json::Value> find(const std::string& collection, const Json::Value& query) = 0;
    
    // Aggregation and analytics
    virtual Json::Value aggregate(const std::string& collection, const Json::Value& pipeline) = 0;
    virtual Json::Value computeStats(const std::string& collection, const Json::Value& statsQuery) = 0;
};

// Message queue interface
class IMessageQueue : public IExternalSystem {
public:
    virtual ~IMessageQueue() = default;
    
    // Queue management
    virtual bool createQueue(const std::string& queueName) = 0;
    virtual bool deleteQueue(const std::string& queueName) = 0;
    virtual std::vector<std::string> listQueues() = 0;
    
    // Message operations
    virtual bool sendMessage(const std::string& queueName, const std::string& message,
                           const Json::Value& headers = Json::Value()) = 0;
    virtual std::string receiveMessage(const std::string& queueName,
                                     const std::string& consumer = "",
                                     int timeout = 0) = 0;
    virtual bool ackMessage(const std::string& queueName, const std::string& messageId) = 0;
    
    // Queue attributes
    virtual Json::Value getQueueAttributes(const std::string& queueName) = 0;
    virtual bool setQueueAttributes(const std::string& queueName, const Json::Value& attributes) = 0;
};

// REST API client interface
class IRestAPI : public IExternalSystem {
public:
    virtual ~IRestAPI() = default;
    
    // HTTP methods
    virtual Json::Value get(const std::string& path, const Json::Value& headers = Json::Value()) = 0;
    virtual Json::Value post(const std::string& path, const Json::Value& body, const Json::Value& headers = Json::Value()) = 0;
    virtual Json::Value put(const std::string& path, const Json::Value& body, const Json::Value& headers = Json::Value()) = 0;
    virtual Json::Value delete(const std::string& path, const Json::Value& headers = Json::Value()) = 0;
    
    // Authentication
    virtual bool authenticate(const std::string& username, const std::string& password) = 0;
    virtual bool authenticateWithToken(const std::string& token) = 0;
    
    // API discovery
    virtual Json::Value discoverAPI(const std::string& baseUrl) = 0;
    virtual std::vector<std::string> getAvailableEndpoints() = 0;
};

// External data source interface
class IDataSource : public IExternalSystem {
public:
    virtual ~IDataSource() = default;
    
    // Data stream management
    virtual bool startStreaming() = 0;
    virtual bool stopStreaming() = 0;
    virtual bool isStreaming() const = 0;
    
    // Data retrieval
    virtual std::vector<Json::Value> fetchData(const Json::Value& query = Json::Value()) = 0;
    virtual Json::Value fetchLatest(const std::string& entity) = 0;
    virtual bool fetchHistorical(const std::string& entity, const std::string& startTime,
                                const std::string& endTime, Json::Value& data) = 0;
    
    // Data source metadata
    virtual Json::Value getMetadata() const = 0;
    virtual std::vector<std::string> getEntities() const = 0;
};

// External monitoring interface
class IMonitoring : public IExternalSystem {
public:
    virtual ~IMonitoring() = default;
    
    // Metric collection
    virtual bool recordMetric(const std::string& name, double value, const Json::Value& tags = Json::Value()) = 0;
    virtual bool recordGauge(const std::string& name, double value, const Json::Value& tags = Json::Value()) = 0;
    virtual bool recordCounter(const std::string& name, int64_t value, const Json::Value& tags = Json::Value()) = 0;
    virtual bool recordTimer(const std::string& name, double duration, const Json::Value& tags = Json::Value()) = 0;
    
    // Alert management
    virtual bool createAlert(const std::string& name, const Json::Value& condition,
                           const std::string& message, const Json::Value& metadata = Json::Value()) = 0;
    virtual bool disableAlert(const std::string& name) = 0;
    virtual bool enableAlert(const std::string& name) = 0;
    
    // Monitoring status
    virtual Json::Value getMetrics(const std::string& query = "") = 0;
    virtual Json::Value getAlerts(const std::string& filter = "") = 0;
    virtual std::string getHealthStatus() = 0;
};

// Integration manager
class IntegrationManager {
public:
    IntegrationManager() : nextId_(1), healthy_(true) {
        startMetricsThread();
    }
    
    ~IntegrationManager() {
        stopMetricsThread();
        for (auto& system : systems_) {
            if (system.second) {
                system.second->disconnect();
            }
        }
    }
    
    // System registration
    std::string registerSystem(std::unique_ptr<IExternalSystem> system);
    bool unregisterSystem(const std::string& id);
    bool isSystemRegistered(const std::string& id) const;
    
    // System access
    IExternalSystem* getSystem(const std::string& id) const;
    std::vector<std::string> getSystemIds() const;
    std::vector<IExternalSystem*> getSystemsByType(const std::string& type) const;
    
    // Configuration management
    bool configureSystem(const std::string& id, const Json::Value& config);
    Json::Value getSystemConfig(const std::string& id) const;
    bool setSystemConfig(const std::string& id, const Json::Value& config);
    
    // Operation management
    bool connectSystem(const std::string& id);
    bool disconnectSystem(const std::string& id);
    bool startAllSystems();
    bool stopAllSystems();
    
    // Data operations
    bool sendDataToSystem(const std::string& id, const std::string& data);
    std::string receiveDataFromSystem(const std::string& id);
    
    // Monitoring and health
    bool isSystemHealthy(const std::string& id) const;
    Json::Value getSystemStatus(const std::string& id) const;
    Json::Value getOverallStatus() const;
    
    // Error handling
    bool handleSystemError(const std::string& id, const NLMException& error);
    bool reportErrorToAll(const NLMException& error);
    
    // Metrics collection
    struct SystemMetrics {
        std::string systemId;
        std::string systemName;
        bool isConnected;
        bool isHealthy;
        int totalOperations;
        int failedOperations;
        double avgResponseTime;
        std::chrono::system_clock::time_point lastUpdate;
    };
    
    std::vector<SystemMetrics> getSystemMetrics() const;
    
    // Configuration
    void setConfig(const Json::Value& config) { config_ = config; }
    Json::Value getConfig() const { return config_; }
    
    // Event handling
    void setEventCallback(std::function<void(const std::string&, const Json::Value&)> callback) {
        eventCallback_ = callback;
    }
    
private:
    void startMetricsThread();
    void stopMetricsThread();
    void metricsCollector();
    std::string generateSystemId();
    
    std::map<std::string, std::unique_ptr<IExternalSystem>> systems_;
    Json::Value config_;
    std::atomic<int> nextId_;
    std::atomic<bool> healthy_;
    std::vector<SystemMetrics> metrics_;
    mutable std::mutex mutex_;
    bool running_ = true;
    std::thread metricsThread_;
    std::function<void(const std::string&, const Json::Value&)> eventCallback_;
};

// Concrete implementations

// Database implementation
class Database : public IDatabase {
public:
    Database() : connected_(false) {}
    ~Database() { disconnect(); }
    
    std::string getName() const override { return "Database"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "NLM Database Integration"; }
    
    bool connect() override { return connect(""); }
    bool disconnect() override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (connected_) {
            EnhancedLogger::logWithContext(LogLevel::Info, "Disconnecting from database", "database", "connection");
            connected_ = false;
        }
        return true;
    }
    
    bool isConnected() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return connected_;
    }
    
    bool configure(const Json::Value& config) override;
    Json::Value getConfiguration() const override;
    
    bool sendData(const std::string& data) override;
    std::string receiveData() override;
    
    bool handleError(const NLMException& error) override;
    
    Json::Value getStatus() const override;
    bool isHealthy() const override;
    
    // IDatabase interface
    bool connect(const std::string& connectionString) override;
    bool insert(const std::string& collection, const Json::Value& document) override;
    bool update(const std::string& collection, const std::string& id, const Json::Value& document) override;
    bool remove(const std::string& collection, const std::string& id) override;
    Json::Value findOne(const std::string& collection, const Json::Value& query) override;
    std::vector<Json::Value> find(const std::string& collection, const Json::Value& query) override;
    Json::Value aggregate(const std::string& collection, const Json::Value& pipeline) override;
    Json::Value computeStats(const std::string& collection, const Json::Value& statsQuery) override;
    
    // IExternalSystem interface
    void setEventCallback(std::function<void(const std::string&, const Json::Value&)> callback) override {
        eventCallback_ = callback;
    }
    
private:
    bool connected_;
    std::string connectionString_;
    mutable std::mutex mutex_;
    std::function<void(const std::string&, const Json::Value&)> eventCallback_;
};

// Message queue implementation
class MessageQueue : public IMessageQueue {
public:
    MessageQueue() : connected_(false) {}
    ~MessageQueue() { disconnect(); }
    
    std::string getName() const override { return "MessageQueue"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "NLM Message Queue Integration"; }
    
    bool connect() override;
    bool disconnect() override;
    bool isConnected() const override;
    
    bool configure(const Json::Value& config) override;
    Json::Value getConfiguration() const override;
    
    bool sendData(const std::string& data) override;
    std::string receiveData() override;
    
    bool handleError(const NLMException& error) override;
    
    Json::Value getStatus() const override;
    bool isHealthy() const override;
    
    // IMessageQueue interface
    bool createQueue(const std::string& queueName) override;
    bool deleteQueue(const std::string& queueName) override;
    std::vector<std::string> listQueues() override;
    
    bool sendMessage(const std::string& queueName, const std::string& message,
                    const Json::Value& headers = Json::Value()) override;
    std::string receiveMessage(const std::string& queueName,
                              const std::string& consumer = "",
                              int timeout = 0) override;
    bool ackMessage(const std::string& queueName, const std::string& messageId) override;
    
    Json::Value getQueueAttributes(const std::string& queueName) override;
    bool setQueueAttributes(const std::string& queueName, const Json::Value& attributes) override;
    
    void setEventCallback(std::function<void(const std::string&, const Json::Value&)> callback) override {
        eventCallback_ = callback;
    }
    
private:
    bool connected_;
    std::map<std::string, std::string> queueAttributes_;
    mutable std::mutex mutex_;
    std::function<void(const std::string&, const Json::Value&)> eventCallback_;
};

// REST API implementation
class RestAPI : public IRestAPI {
public:
    RestAPI() : connected_(false) {}
    ~RestAPI() { disconnect(); }
    
    std::string getName() const override { return "RestAPI"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "NLM REST API Client"; }
    
    bool connect() override;
    bool disconnect() override;
    bool isConnected() const override;
    
    bool configure(const Json::Value& config) override;
    Json::Value getConfiguration() const override;
    
    bool sendData(const std::string& data) override;
    std::string receiveData() override;
    
    bool handleError(const NLMException& error) override;
    
    Json::Value getStatus() const override;
    bool isHealthy() const override;
    
    // IRestAPI interface
    Json::Value get(const std::string& path, const Json::Value& headers = Json::Value()) override;
    Json::Value post(const std::string& path, const Json::Value& body, const Json::Value& headers = Json::Value()) override;
    Json::Value put(const std::string& path, const Json::Value& body, const Json::Value& headers = Json::Value()) override;
    Json::Value delete(const std::string& path, const Json::Value& headers = Json::Value()) override;
    
    bool authenticate(const std::string& username, const std::string& password) override;
    bool authenticateWithToken(const std::string& token) override;
    Json::Value discoverAPI(const std::string& baseUrl) override;
    std::vector<std::string> getAvailableEndpoints() override;
    
    void setEventCallback(std::function<void(const std::string&, const Json::Value&)> callback) override {
        eventCallback_ = callback;
    }
    
private:
    bool connected_;
    std::string baseUrl_;
    std::string authenticationToken_;
    mutable std::mutex mutex_;
    std::function<void(const std::string&, const Json::Value&)> eventCallback_;
};

// Data source implementation
class DataSource : public IDataSource {
public:
    DataSource() : connected_(false), streaming_(false) {}
    ~DataSource() { stopStreaming(); disconnect(); }
    
    std::string getName() const override { return "DataSource"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "NLM Data Source Integration"; }
    
    bool connect() override;
    bool disconnect() override;
    bool isConnected() const override;
    
    bool configure(const Json::Value& config) override;
    Json::Value getConfiguration() const override;
    
    bool sendData(const std::string& data) override;
    std::string receiveData() override;
    
    bool handleError(const NLMException& error) override;
    
    Json::Value getStatus() const override;
    bool isHealthy() const override;
    
    // IDataSource interface
    bool startStreaming() override;
    bool stopStreaming() override;
    bool isStreaming() const override;
    
    std::vector<Json::Value> fetchData(const Json::Value& query = Json::Value()) override;
    Json::Value fetchLatest(const std::string& entity) override;
    bool fetchHistorical(const std::string& entity, const std::string& startTime,
                        const std::string& endTime, Json::Value& data) override;
    
    Json::Value getMetadata() const override;
    std::vector<std::string> getEntities() const override;
    
    void setEventCallback(std::function<void(const std::string&, const Json::Value&)> callback) override {
        eventCallback_ = callback;
    }
    
private:
    bool connected_;
    bool streaming_;
    std::map<std::string, std::vector<Json::Value>> dataStore_;
    mutable std::mutex mutex_;
    std::thread streamingThread_;
    std::function<void(const std::string&, const Json::Value&)> eventCallback_;
};

// Monitoring implementation
class Monitoring : public IMonitoring {
public:
    Monitoring() : connected_(false) {}
    ~Monitoring() { disconnect(); }
    
    std::string getName() const override { return "Monitoring"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "NLM Monitoring Integration"; }
    
    bool connect() override;
    bool disconnect() override;
    bool isConnected() const override;
    
    bool configure(const Json::Value& config) override;
    Json::Value getConfiguration() const override;
    
    bool sendData(const std::string& data) override;
    std::string receiveData() override;
    
    bool handleError(const NLMException& error) override;
    
    Json::Value getStatus() const override;
    bool isHealthy() const override;
    
    // IMonitoring interface
    bool recordMetric(const std::string& name, double value, const Json::Value& tags = Json::Value()) override;
    bool recordGauge(const std::string& name, double value, const Json::Value& tags = Json::Value()) override;
    bool recordCounter(const std::string& name, int64_t value, const Json::Value& tags = Json::Value()) override;
    bool recordTimer(const std::string& name, double duration, const Json::Value& tags = Json::Value()) override;
    
    bool createAlert(const std::string& name, const Json::Value& condition,
                    const std::string& message, const Json::Value& metadata = Json::Value()) override;
    bool disableAlert(const std::string& name) override;
    bool enableAlert(const std::string& name) override;
    
    Json::Value getMetrics(const std::string& query = "") override;
    Json::Value getAlerts(const std::string& filter = "") override;
    std::string getHealthStatus() override;
    
    void setEventCallback(std::function<void(const std::string&, const Json::Value&)> callback) override {
        eventCallback_ = callback;
    }
    
private:
    bool connected_;
    std::map<std::string, Json::Value> metrics_;
    std::map<std::string, Json::Value> alerts_;
    mutable std::mutex mutex_;
    std::function<void(const std::string&, const Json::Value&)> eventCallback_;
};

// Convenience functions
inline std::unique_ptr<IntegrationManager> createIntegrationManager(const Json::Value& config = Json::Value()) {
    auto manager = std::make_unique<IntegrationManager>();
    manager->setConfig(config);
    return manager;
}

inline std::unique_ptr<Database> createDatabase(const Json::Value& config = Json::Value()) {
    auto db = std::make_unique<Database>();
    db->configure(config);
    return db;
}

inline std::unique_ptr<MessageQueue> createMessageQueue(const Json::Value& config = Json::Value()) {
    auto mq = std::make_unique<MessageQueue>();
    mq->configure(config);
    return mq;
}

inline std::unique_ptr<RestAPI> createRestAPI(const Json::Value& config = Json::Value()) {
    auto api = std::make_unique<RestAPI>();
    api->configure(config);
    return api;
}

inline std::unique_ptr<DataSource> createDataSource(const Json::Value& config = Json::Value()) {
    auto source = std::make_unique<DataSource>();
    source->configure(config);
    return source;
}

inline std::unique_ptr<Monitoring> createMonitoring(const Json::Value& config = Json::Value()) {
    auto monitoring = std::make_unique<Monitoring>();
    monitoring->configure(config);
    return monitoring;
}

} // namespace nlm

#endif // NLM_EXTERNAL_INTEGRATION_HPP
