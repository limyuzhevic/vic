// External integration implementation

#include "external_integration.hpp"
#include "error_handling.hpp"
#include <json/json.h>
#include <fstream>
#include <ctime>
#include <thread>
#include <chrono>

namespace nlm {

// IntegrationManager implementation
void IntegrationManager::startMetricsThread() {
    running_ = true;
    metricsThread_ = std::thread(&IntegrationManager::metricsCollector, this);
}

void IntegrationManager::stopMetricsThread() {
    running_ = false;
    if (metricsThread_.joinable()) {
        metricsThread_.join();
    }
}

void IntegrationManager::metricsCollector() {
    while (running_) {
        std::vector<SystemMetrics> currentMetrics;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            
            for (const auto& pair : systems_) {
                IExternalSystem* system = pair.second.get();
                SystemMetrics metric;
                metric.systemId = pair.first;
                metric.systemName = system->getName();
                metric.isConnected = system->isConnected();
                metric.isHealthy = system->isHealthy();
                metric.totalOperations = 0; // Would be tracked from operations
                metric.failedOperations = 0; // Would be tracked from operations
                metric.avgResponseTime = 0.0; // Would be tracked from operations
                metric.lastUpdate = std::chrono::system_clock::now();
                
                currentMetrics.push_back(metric);
            }
            
            metrics_ = currentMetrics;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

std::string IntegrationManager::registerSystem(std::unique_ptr<IExternalSystem> system) {
    if (!system) {
        throw SystemException(ErrorCode::InvalidParameter, "Cannot register null system");
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string id = generateSystemId();
    systems_[id] = std::move(system);
    
    EnhancedLogger::logWithContext(LogLevel::Info, 
                                 "Registered external system: " + id + " (" + systems_[id]->getName() + ")",
                                 "integration_manager", "system_registration");
    
    return id;
}

bool IntegrationManager::unregisterSystem(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = systems_.find(id);
    if (it == systems_.end()) {
        EnhancedLogger::logWithContext(LogLevel::Warning,
                                     "Attempted to unregister non-existent system: " + id,
                                     "integration_manager", "system_unregistration");
        return false;
    }
    
    EnhancedLogger::logWithContext(LogLevel::Info,
                                 "Unregistering external system: " + id,
                                 "integration_manager", "system_unregistration");
    
    systems_.erase(it);
    return true;
}

bool IntegrationManager::isSystemRegistered(const std::string& id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return systems_.find(id) != systems_.end();
}

IExternalSystem* IntegrationManager::getSystem(const std::string& id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = systems_.find(id);
    if (it == systems_.end()) {
        return nullptr;
    }
    
    return it->second.get();
}

std::vector<std::string> IntegrationManager::getSystemIds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> ids;
    for (const auto& pair : systems_) {
        ids.push_back(pair.first);
    }
    
    return ids;
}

std::vector<IExternalSystem*> IntegrationManager::getSystemsByType(const std::string& type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<IExternalSystem*> result;
    for (const auto& pair : systems_) {
        IExternalSystem* system = pair.second.get();
        std::string name = system->getName();
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        
        if (name.find(type) != std::string::npos) {
            result.push_back(system);
        }
    }
    
    return result;
}

bool IntegrationManager::configureSystem(const std::string& id, const Json::Value& config) {
    IExternalSystem* system = getSystem(id);
    if (!system) {
        EnhancedLogger::logWithContext(LogLevel::Error,
                                     "Cannot configure non-existent system: " + id,
                                     "integration_manager", "system_configuration");
        return false;
    }
    
    bool success = system->configure(config);
    
    if (success) {
        EnhancedLogger::logWithContext(LogLevel::Info,
                                     "Configured system: " + id,
                                     "integration_manager", "system_configuration");
    } else {
        EnhancedLogger::logWithContext(LogLevel::Error,
                                     "Failed to configure system: " + id,
                                     "integration_manager", "system_configuration");
    }
    
    return success;
}

Json::Value IntegrationManager::getSystemConfig(const std::string& id) const {
    IExternalSystem* system = getSystem(id);
    if (!system) {
        EnhancedLogger::logWithContext(LogLevel::Error,
                                     "Cannot get config for non-existent system: " + id,
                                     "integration_manager", "system_config");
        return Json::Value();
    }
    
    return system->getConfiguration();
}

bool IntegrationManager::setSystemConfig(const std::string& id, const Json::Value& config) {
    // Most systems use configure() for both initial and subsequent configuration
    return configureSystem(id, config);
}

bool IntegrationManager::connectSystem(const std::string& id) {
    IExternalSystem* system = getSystem(id);
    if (!system) {
        EnhancedLogger::logWithContext(LogLevel::Error,
                                     "Cannot connect non-existent system: " + id,
                                     "integration_manager", "system_connection");
        return false;
    }
    
    bool success = system->connect();
    
    if (success) {
        EnhancedLogger::logWithContext(LogLevel::Info,
                                     "Connected system: " + id,
                                     "integration_manager", "system_connection");
    } else {
        EnhancedLogger::logWithContext(LogLevel::Error,
                                     "Failed to connect system: " + id,
                                     "integration_manager", "system_connection");
    }
    
    return success;
}

bool IntegrationManager::disconnectSystem(const std::string& id) {
    IExternalSystem* system = getSystem(id);
    if (!system) {
        EnhancedLogger::logWithContext(LogLevel::Warning,
                                     "Cannot disconnect non-existent system: " + id,
                                     "integration_manager", "system_disconnection");
        return false;
    }
    
    bool success = system->disconnect();
    
    if (success) {
        EnhancedLogger::logWithContext(LogLevel::Info,
                                     "Disconnected system: " + id,
                                     "integration_manager", "system_disconnection");
    } else {
        EnhancedLogger::logWithContext(LogLevel::Error,
                                     "Failed to disconnect system: " + id,
                                     "integration_manager", "system_disconnection");
    }
    
    return success;
}

bool IntegrationManager::startAllSystems() {
    bool allSuccess = true;
    
    for (const auto& pair : systems_) {
        if (!connectSystem(pair.first)) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

bool IntegrationManager::stopAllSystems() {
    bool allSuccess = true;
    
    for (const auto& pair : systems_) {
        if (!disconnectSystem(pair.first)) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

bool IntegrationManager::sendDataToSystem(const std::string& id, const std::string& data) {
    IExternalSystem* system = getSystem(id);
    if (!system) {
        EnhancedLogger::logWithContext(LogLevel::Error,
                                     "Cannot send data to non-existent system: " + id,
                                     "integration_manager", "data_send");
        return false;
    }
    
    bool success = system->sendData(data);
    
    if (success) {
        EnhancedLogger::logWithContext(LogLevel::Info,
                                     "Sent data to system: " + id,
                                     "integration_manager", "data_send");
    } else {
        EnhancedLogger::logWithContext(LogLevel::Error,
                                     "Failed to send data to system: " + id,
                                     "integration_manager", "data_send");
    }
    
    return success;
}

std::string IntegrationManager::receiveDataFromSystem(const std::string& id) {
    IExternalSystem* system = getSystem(id);
    if (!system) {
        EnhancedLogger::logWithContext(LogLevel::Error,
                                     "Cannot receive data from non-existent system: " + id,
                                     "integration_manager", "data_receive");
        return "";
    }
    
    std::string data = system->receiveData();
    
    if (!data.empty()) {
        EnhancedLogger::logWithContext(LogLevel::Info,
                                     "Received data from system: " + id,
                                     "integration_manager", "data_receive");
    }
    
    return data;
}

bool IntegrationManager::isSystemHealthy(const std::string& id) const {
    IExternalSystem* system = getSystem(id);
    if (!system) {
        return false;
    }
    
    return system->isHealthy();
}

Json::Value IntegrationManager::getSystemStatus(const std::string& id) const {
    IExternalSystem* system = getSystem(id);
    if (!system) {
        return Json::Value();
    }
    
    return system->getStatus();
}

Json::Value IntegrationManager::getOverallStatus() const {
    Json::Value status;
    status["healthy"] = healthy_;
    status["total_systems"] = static_cast<int>(systems_.size());
    
    int connectedCount = 0;
    int healthyCount = 0;
    
    for (const auto& pair : systems_) {
        if (pair.second->isConnected()) {
            connectedCount++;
        }
        if (pair.second->isHealthy()) {
            healthyCount++;
        }
    }
    
    status["connected_systems"] = connectedCount;
    status["healthy_systems"] = healthyCount;
    
    return status;
}

bool IntegrationManager::handleSystemError(const std::string& id, const NLMException& error) {
    IExternalSystem* system = getSystem(id);
    if (!system) {
        EnhancedLogger::logWithContext(LogLevel::Error,
                                     "Cannot handle error for non-existent system: " + id,
                                     "integration_manager", "error_handling");
        return false;
    }
    
    return system->handleError(error);
}

bool IntegrationManager::reportErrorToAll(const NLMException& error) {
    bool allHandled = true;
    
    for (const auto& pair : systems_) {
        if (!handleSystemError(pair.first, error)) {
            allHandled = false;
        }
    }
    
    return allHandled;
}

std::vector<IntegrationManager::SystemMetrics> IntegrationManager::getSystemMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return metrics_;
}

std::string IntegrationManager::generateSystemId() {
    std::ostringstream oss;
    oss << "sys_" << std::hex << std::setw(8) << std::setfill('0') << nextId_++;
    return oss.str();
}

// Database implementation
bool Database::configure(const Json::Value& config) {
    try {
        // Extract database configuration
        if (config.isMember("connection_string")) {
            connectionString_ = config["connection_string"].asString();
        }
        
        // Test connection
        connected_ = !connectionString_.empty();
        
        EnhancedLogger::logWithContext(LogLevel::Info,
                                     "Database configuration completed",
                                     "database", "configuration");
        
        return connected_;
        
    } catch (const std::exception& e) {
        EnhancedLogger::logWithContext(LogLevel::Error,
                                     "Database configuration failed: " + std::string(e.what()),
                                     "database", "configuration");
        return false;
    }
}

Json::Value Database::getConfiguration() const {
    Json::Value config;
    config["connection_string"] = connectionString_;
    config["connected"] = connected_;
    return config;
}

bool Database::sendData(const std::string& data) {
    if (!connected_) {
        return false;
    }
    
    // In a real implementation, this would serialize the data and send to database
    EnhancedLogger::logWithContext(LogLevel::Info,
                                 "Data sent to database",
                                 "database", "data_send");
    
    return true;
}

std::string Database::receiveData() {
    if (!connected_) {
        return "";
    }
    
    // In a real implementation, this would retrieve data from database
    EnhancedLogger::logWithContext(LogLevel::Info,
                                 "Data retrieved from database",
                                 "database", "data_receive");
    
    return "sample_data";
}

bool Database::handleError(const NLMException& error) {
    EnhancedLogger::logWithContext(LogLevel::Error,
                                 "Database error: " + error.getFullMessage(),
                                 "database", "error_handling");
    
    // Database-specific error handling
    if (error.getCategory() == ErrorCategory::Memory) {
        // Memory errors are usually fatal for database operations
        connected_ = false;
    }
    
    return true;
}

Json::Value Database::getStatus() const {
    Json::Value status;
    status["name"] = getName();
    status["version"] = getVersion();
    status["description"] = getDescription();
    status["connected"] = connected_;
    status["connection_string_length"] = static_cast<int>(connectionString_.size());
    status["last_updated"] = std::chrono::system_clock::now();
    
    return status;
}

bool Database::isHealthy() const {
    return connected_ && !connectionString_.empty();
}

bool Database::connect(const std::string& connectionString) {
    try {
        if (connectionString.empty() && !this->connectionString_.empty()) {
            connectionString_ = connectionString;
        }
        
        connected_ = !connectionString_.empty();
        
        if (connected_) {
            EnhancedLogger::logWithContext(LogLevel::Info,
                                         "Connected to database",
                                         "database", "connection");
        } else {
            EnhancedLogger::logWithContext(LogLevel::Error,
                                         "Failed to connect to database",
                                         "database", "connection");
        }
        
        return connected_;
        
    } catch (const std::exception& e) {
        EnhancedLogger::logWithContext(LogLevel::Error,
                                     "Database connection failed: " + std::string(e.what()),
                                     "database", "connection");
        connected_ = false;
        return false;
    }
}

// Other concrete implementations would follow similar patterns...

} // namespace nlm
