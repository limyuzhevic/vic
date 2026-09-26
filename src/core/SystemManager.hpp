/*
 * NLM System Manager - Central System Management Framework
 * Provides system management, coordination, and control for neural system
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <any>
#include <stdexcept>
#include <mutex>
#include <chrono>
#include <thread>

namespace nlm {

// System Event Types
enum class SystemEventType {
    SystemStart,
    SystemStop,
    SystemError,
    SystemWarning,
    ConfigurationChanged,
    ModuleLoaded,
    ModuleUnloaded,
    ModuleError,
    IntegrationComplete,
    ValidationComplete
};

// System Event
struct SystemEvent {
    SystemEventType type;
    std::string message;
    std::map<std::string, std::any> data;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
    
    SystemEvent(SystemEventType type, const std::string& message)
        : type(type), message(message), timestamp(std::chrono::system_clock::now()) {}
    
    SystemEvent(SystemEventType type, const std::string& message, 
               const std::map<std::string, std::any>& data)
        : type(type), message(message), data(data), 
          timestamp(std::chrono::system_clock::now()) {}
};

// System Event Listener Interface
class ISystemEventListener {
public:
    virtual ~ISystemEventListener() = default;
    
    virtual std::string getListenerName() const = 0;
    virtual std::vector<SystemEventType> getEventTypes() const = 0;
    
    virtual void onEvent(const SystemEvent& event) = 0;
    virtual bool isAsync() const { return false; }
    
    virtual void setEventCallback(std::function<void(const SystemEvent&)> callback) {
        eventCallback_ = callback;
    }
    
    void notifyEvent(const SystemEvent& event) {
        if (eventCallback_) {
            if (isAsync()) {
                std::thread([this, event]() {
                    eventCallback_(event);
                }).detach();
            } else {
                eventCallback_(event);
            }
        }
    }
    
protected:
    std::function<void(const SystemEvent&)> eventCallback_;
};

// System Event Listener Manager
class SystemEventListenerManager {
public:
    SystemEventListenerManager();
    ~SystemEventListenerManager();
    
    // Event listener management
    bool registerListener(std::shared_ptr<ISystemEventListener> listener);
    bool unregisterListener(const std::string& listenerName);
    bool unregisterListener(std::type_index listenerType);
    
    // Event distribution
    void distributeEvent(const SystemEvent& event);
    void distributeEventAsync(const SystemEvent& event);
    
    // Event filtering
    std::vector<std::shared_ptr<ISystemEventListener>> getListenersForEvent(SystemEventType type) const;
    std::vector<std::shared_ptr<ISystemEventListener>> getAllListeners() const;
    
    // Event management
    void clearListeners();
    size_t getListenerCount() const { return listeners_.size(); }
    
    // Global instance access
    static SystemEventListenerManager& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// System State
enum class SystemState {
    NotInitialized,
    Initializing,
    Initialized,
    Running,
    ShuttingDown,
    Shutdown,
    Error
};

// System Interface
class ISystem {
public:
    virtual ~ISystem() = default;
    
    virtual std::string getSystemName() const = 0;
    virtual std::string getSystemVersion() const = 0;
    virtual SystemState getSystemState() const = 0;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    virtual bool isInitialized() const { return getSystemState() == SystemState::Initialized; }
    virtual bool isRunning() const { return getSystemState() == SystemState::Running; }
    
    // System configuration
    virtual void configure(const std::map<std::string, std::any>& config) {}
    virtual std::map<std::string, std::any> getConfig() const { return std::map<std::string, std::any>(); }
    
    // System statistics
    virtual std::map<std::string, std::any> getStatistics() const { return std::map<std::string, std::any>(); }
    
    // System information
    virtual std::string getSystemInfo() const { return "System: " + getSystemName(); }
    
    // Event handling
    virtual void handleEvent(const SystemEvent& event) {}
};

// Abstract System Base Class
class System : public ISystem {
public:
    System(const std::string& name, const std::string& version);
    virtual ~System() override;
    
    std::string getSystemName() const override { return systemName_; }
    std::string getSystemVersion() const override { return systemVersion_; }
    SystemState getSystemState() const override { return state_; }
    
    bool initialize() override;
    void shutdown() override;
    
    virtual bool internalInitialize() = 0;
    virtual void internalShutdown() = 0;
    
    // Event handling
    void registerEventListener(std::shared_ptr<ISystemEventListener> listener);
    void unregisterEventListener(const std::string& listenerName);
    void notifyEvent(const SystemEvent& event);
    
    // Configuration
    void configure(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfig() const override;
    
    // Statistics
    std::map<std::string, std::any> getStatistics() const override;
    
    // System management
    virtual bool start() { state_ = SystemState::Running; return true; }
    virtual bool stop() { state_ = SystemState::ShuttingDown; return true; }
    
protected:
    std::string systemName_;
    std::string systemVersion_;
    SystemState state_;
    std::map<std::string, std::any> config_;
    
    std::vector<std::shared_ptr<ISystemEventListener>> eventListeners_;
    mutable std::mutex eventListenersMutex_;
};

// System Manager Interface
class ISystemManager {
public:
    virtual ~ISystemManager() = default;
    
    // System lifecycle management
    virtual bool registerSystem(std::shared_ptr<ISystem> system) = 0;
    virtual bool unregisterSystem(const std::string& systemName) = 0;
    virtual bool initializeSystem(const std::string& systemName) = 0;
    virtual bool shutdownSystem(const std::string& systemName) = 0;
    
    // System query
    virtual std::vector<std::string> getSystemNames() const = 0;
    virtual std::vector<std::string> getSystemNames(const std::string& category) const = 0;
    virtual std::shared_ptr<ISystem> getSystem(const std::string& systemName) = 0;
    virtual std::shared_ptr<ISystem> getSystem(const std::string& category, const std::string& name) = 0;
    
    // System state management
    virtual SystemState getSystemState(const std::string& systemName) const = 0;
    virtual bool isSystemInitialized(const std::string& systemName) const = 0;
    virtual bool isSystemRunning(const std::string& systemName) const = 0;
    
    // System configuration
    virtual bool configureSystem(const std::string& systemName,
                               const std::map<std::string, std::any>& config) = 0;
    virtual std::map<std::string, std::any> getSystemConfig(const std::string& systemName) const = 0;
    
    // System dependency management
    virtual bool registerSystemDependency(const std::string& systemName,
                                         const std::string& dependencyName) = 0;
    virtual std::vector<std::string> getSystemDependencies(const std::string& systemName) const = 0;
    virtual bool checkSystemDependencies(const std::string& systemName) const = 0;
    
    // System lifecycle
    virtual bool initializeAllSystems() = 0;
    virtual void shutdownAllSystems() = 0;
    virtual bool startAllSystems() = 0;
    virtual bool stopAllSystems() = 0;
    
    // System management
    virtual std::string getSystemStatus() const = 0;
    virtual std::map<std::string, std::any> getSystemStatistics() const = 0;
    
    // Event management
    virtual bool registerEventListener(std::shared_ptr<ISystemEventListener> listener) = 0;
    virtual bool unregisterEventListener(const std::string& listenerName) = 0;
    virtual void distributeEvent(const SystemEvent& event) = 0;
    
    // System integration
    virtual bool validateSystemIntegration() const = 0;
    virtual bool performSystemDiagnostics() const = 0;
    virtual std::string getSystemDiagnostics() const = 0;
};

// Default System Manager Implementation
class SystemManager : public ISystemManager {
public:
    SystemManager();
    ~SystemManager() override;
    
    // System lifecycle management
    bool registerSystem(std::shared_ptr<ISystem> system) override;
    bool unregisterSystem(const std::string& systemName) override;
    bool initializeSystem(const std::string& systemName) override;
    bool shutdownSystem(const std::string& systemName) override;
    
    // System query
    std::vector<std::string> getSystemNames() const override;
    std::vector<std::string> getSystemNames(const std::string& category) const override;
    std::shared_ptr<ISystem> getSystem(const std::string& systemName) override;
    std::shared_ptr<ISystem> getSystem(const std::string& category, const std::string& name) override;
    
    // System state management
    SystemState getSystemState(const std::string& systemName) const override;
    bool isSystemInitialized(const std::string& systemName) const override;
    bool isSystemRunning(const std::string& systemName) const override;
    
    // System configuration
    bool configureSystem(const std::string& systemName,
                       const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getSystemConfig(const std::string& systemName) const override;
    
    // System dependency management
    bool registerSystemDependency(const std::string& systemName,
                                 const std::string& dependencyName) override;
    std::vector<std::string> getSystemDependencies(const std::string& systemName) const override;
    bool checkSystemDependencies(const std::string& systemName) const override;
    
    // System lifecycle
    bool initializeAllSystems() override;
    void shutdownAllSystems() override;
    bool startAllSystems() override;
    bool stopAllSystems() override;
    
    // System management
    std::string getSystemStatus() const override;
    std::map<std::string, std::any> getSystemStatistics() const override;
    
    // Event management
    bool registerEventListener(std::shared_ptr<ISystemEventListener> listener) override;
    bool unregisterEventListener(const std::string& listenerName) override;
    void distributeEvent(const SystemEvent& event) override;
    
    // System integration
    bool validateSystemIntegration() const override;
    bool performSystemDiagnostics() const override;
    std::string getSystemDiagnostics() const override;
    
    // Global instance access
    static SystemManager& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// System Category
enum class SystemCategory {
    Core,
    Brain,
    Memory,
    Plasticity,
    Neuromodulation,
    Prediction,
    Cognition,
    Sensory,
    Motor,
    World,
    Development,
    Visualization,
    Configuration,
    Performance,
    Testing,
    Debug
};

// System Registrar Macros
#define REGISTER_SYSTEM(systemClass, category) \
    namespace { \
        struct SystemRegistration { \
            SystemRegistration() { \
                SystemManager& manager = SystemManager::getInstance(); \
                manager.registerSystem(std::make_shared<systemClass>()); \
            } \
        }; \
        static SystemRegistration systemReg; \
    }

#define REGISTER_SYSTEM_WITH_NAME(systemClass, name, category) \
    namespace { \
        struct SystemRegistration { \
            SystemRegistration() { \
                SystemManager& manager = SystemManager::getInstance(); \
                auto system = std::make_shared<systemClass>(); \
                // Note: This is a simplified registration - actual implementation \
                // would need to handle categories properly \
                manager.registerSystem(system); \
            } \
        }; \
        static SystemRegistration systemReg; \
    }

// System Integration Utilities
class SystemIntegrationUtils {
public:
    // Configuration template management
    static std::map<std::string, std::any> createSystemConfig(const std::string& systemName,
                                                           const std::map<std::string, std::any>& defaults);
    
    // Dependency resolution
    static std::vector<std::string> resolveDependencies(const std::vector<std::string>& dependencies,
                                                      const std::map<std::string, bool>& systemStates);
    
    // System health checking
    static bool checkSystemHealth(const std::shared_ptr<ISystem>& system);
    static std::string getSystemHealthReport(const std::vector<std::shared_ptr<ISystem>>& systems);
    
    // Configuration validation
    static bool validateConfiguration(const std::map<std::string, std::any>& config);
    
    // Event utilities
    static std::string getEventTypeString(SystemEventType type);
    static SystemEvent createSystemEvent(SystemEventType type, const std::string& message,
                                        const std::map<std::string, std::any>& data = {});
    
    // Performance utilities
    static double measureSystemPerformance(std::function<void()> operation);
    static std::map<std::string, double> benchmarkSystemOperations(
        const std::vector<std::pair<std::string, std::function<void()>>>& operations);
};

// System Integration Framework
class SystemIntegrationFramework {
public:
    SystemIntegrationFramework();
    ~SystemIntegrationFramework();
    
    // Initialize the integration framework
    bool initialize();
    
    // Shutdown the integration framework
    void shutdown();
    
    // Get the global system manager
    ISystemManager& getManager() { return manager_; }
    
    // Register core systems
    void registerCoreSystems();
    
    // System lifecycle management
    bool initializeAllSystems();
    bool shutdownAllSystems();
    bool startAllSystems();
    bool stopAllSystems();
    
    // System validation
    bool validateSystemIntegration();
    bool performSystemDiagnostics();
    
    // Get system status
    std::string getSystemStatus() const;
    std::map<std::string, std::any> getSystemStatistics() const;
    
    // Event handling
    void notifySystemEvent(const SystemEvent& event);
    
    // Global instance access
    static SystemIntegrationFramework& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ISystemManager& manager_;
};

} // namespace nlm