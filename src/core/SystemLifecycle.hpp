/*
 * NLM System Lifecycle Management - Complete System Control
 * Provides comprehensive system lifecycle management for neural system
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
#include <algorithm>

namespace nlm {

// System Lifecycle Stages
enum class LifecycleStage {
    NotStarted,
    Initializing,
    Running,
    Paused,
    Stopping,
    Shutdown,
    Restarting,
    Error,
    Maintenance
};

// Lifecycle Event Types
enum class LifecycleEventType {
    SystemStart,
    SystemInitialized,
    SystemRunning,
    SystemPaused,
    SystemStopping,
    SystemShutdown,
    SystemRestarted,
    SystemError,
    SystemMaintenance,
    ConfigurationChanged,
    DependencyResolved,
    HealthCheck,
    Backup,
    Restore
};

// System Lifecycle Event
struct LifecycleEvent {
    LifecycleEventType type;
    std::string message;
    std::map<std::string, std::any> data;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
    bool error = false;
    
    LifecycleEvent(LifecycleEventType type, const std::string& message)
        : type(type), message(message), timestamp(std::chrono::system_clock::now()), error(false) {}
    
    LifecycleEvent(LifecycleEventType type, const std::string& message, 
                  const std::map<std::string, std::any>& data)
        : type(type), message(message), data(data), 
          timestamp(std::chrono::system_clock::now()), error(false) {}
    
    LifecycleEvent(LifecycleEventType type, const std::string& message, 
                  bool error, const std::map<std::string, std::any>& data = {})
        : type(type), message(message), data(data), 
          timestamp(std::chrono::system_clock::now()), error(error) {}
};

// System Lifecycle Listener Interface
class ILifecycleListener {
public:
    virtual ~ILifecycleListener() = default;
    
    virtual std::string getListenerName() const = 0;
    virtual std::vector<LifecycleEventType> getEventTypes() const = 0;
    
    virtual void onLifecycleEvent(const LifecycleEvent& event) = 0;
    virtual bool isAsync() const { return false; }
    
    virtual void setEventCallback(std::function<void(const LifecycleEvent&)> callback) {
        eventCallback_ = callback;
    }
    
    void notifyEvent(const LifecycleEvent& event) {
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
    std::function<void(const LifecycleEvent&)> eventCallback_;
};

// System Lifecycle Manager
class SystemLifecycleManager {
public:
    SystemLifecycleManager();
    ~SystemLifecycleManager();
    
    // Event listener management
    bool registerLifecycleListener(std::shared_ptr<ILifecycleListener> listener);
    bool unregisterLifecycleListener(const std::string& listenerName);
    bool unregisterLifecycleListener(std::type_index listenerType);
    
    // Event distribution
    void distributeLifecycleEvent(const LifecycleEvent& event);
    void distributeLifecycleEventAsync(const LifecycleEvent& event);
    
    // Event filtering
    std::vector<std::shared_ptr<ILifecycleListener>> getListenersForEvent(LifecycleEventType type) const;
    std::vector<std::shared_ptr<ILifecycleListener>> getAllListeners() const;
    
    // Event management
    void clearListeners();
    size_t getListenerCount() const { return listeners_.size(); }
    
    // Lifecycle operation utilities
    std::string getLifecycleStageString(LifecycleStage stage);
    std::string getLifecycleEventTypeString(LifecycleEventType type);
    
    // Global instance access
    static SystemLifecycleManager& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// System Lifecycle Configuration
struct SystemLifecycleConfig {
    std::string systemName;
    LifecycleStage initialStage;
    std::chrono::milliseconds timeout;
    std::vector<std::string> dependencies;
    std::map<std::string, std::any> metadata;
    
    SystemLifecycleConfig() = default;
    SystemLifecycleConfig(const std::string& name) : systemName(name), initialStage(LifecycleStage::NotStarted),
                                                       timeout(std::chrono::milliseconds(30000)) {}
};

// System Lifecycle Interface
class ISystemLifecycle {
public:
    virtual ~ISystemLifecycle() = default;
    
    virtual std::string getSystemName() const = 0;
    virtual LifecycleStage getLifecycleStage() const = 0;
    
    virtual bool start() = 0;
    virtual bool stop() = 0;
    virtual bool pause() = 0;
    virtual bool resume() = 0;
    virtual bool restart() = 0;
    virtual bool shutdown() = 0;
    
    virtual bool isRunning() const { return getLifecycleStage() == LifecycleStage::Running; }
    virtual bool isPaused() const { return getLifecycleStage() == LifecycleStage::Paused; }
    virtual bool isStopped() const { return getLifecycleStage() == LifecycleStage::Shutdown; }
    
    // Configuration
    virtual void configure(const SystemLifecycleConfig& config) {}
    virtual SystemLifecycleConfig getConfig() const { return SystemLifecycleConfig(); }
    
    // Event handling
    virtual bool handleLifecycleEvent(const LifecycleEvent& event) { return false; }
    
    // Statistics
    virtual std::map<std::string, std::any> getStatistics() const { return std::map<std::string, std::any>(); }
    
    // Validation
    virtual bool validate() const { return true; }
    virtual std::string validateAll() const { return "Lifecycle validation passed"; }
};

// Abstract System Lifecycle Base Class
class SystemLifecycle : public ISystemLifecycle {
public:
    SystemLifecycle(const std::string& name);
    virtual ~SystemLifecycle() override;
    
    std::string getSystemName() const override { return systemName_; }
    LifecycleStage getLifecycleStage() const override { return currentStage_; }
    
    bool start() override;
    bool stop() override;
    bool pause() override;
    bool resume() override;
    bool restart() override;
    bool shutdown() override;
    
    virtual bool internalStart() = 0;
    virtual bool internalStop() = 0;
    virtual bool internalPause() = 0;
    virtual bool internalResume() = 0;
    virtual bool internalRestart() = 0;
    
    // Event handling
    bool handleLifecycleEvent(const LifecycleEvent& event) override;
    
    // Configuration
    void configure(const SystemLifecycleConfig& config) override;
    SystemLifecycleConfig getConfig() const override;
    
    // Statistics
    std::map<std::string, std::any> getStatistics() const override;
    
    // Validation
    bool validate() const override;
    std::string validateAll() const override;
    
    // Lifecycle management
    virtual bool transitionTo(LifecycleStage newStage, const std::string& reason);
    virtual bool canTransitionTo(LifecycleStage newStage) const;
    
    // Error handling
    virtual bool handleError(const std::string& errorMessage);
    virtual bool recoverFromError();
    
protected:
    std::string systemName_;
    LifecycleStage currentStage_;
    SystemLifecycleConfig config_;
    
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point stopTime_;
    std::chrono::milliseconds totalRuntime_;
    
    std::vector<LifecycleEvent> lifecycleHistory_;
    mutable std::mutex historyMutex_;
    
    // Error handling
    std::vector<std::string> errorHistory_;
    mutable std::mutex errorMutex_;
    bool inErrorState_;
};

// System Lifecycle Manager Interface
class ISystemLifecycleManager {
public:
    virtual ~ISystemLifecycleManager() = default;
    
    // Lifecycle management
    virtual bool registerLifecycle(std::shared_ptr<ISystemLifecycle> lifecycle) = 0;
    virtual bool unregisterLifecycle(const std::string& systemName) = 0;
    
    // Lifecycle operations
    virtual bool startSystem(const std::string& systemName) = 0;
    virtual bool stopSystem(const std::string& systemName) = 0;
    virtual bool pauseSystem(const std::string& systemName) = 0;
    virtual bool resumeSystem(const std::string& systemName) = 0;
    virtual bool restartSystem(const std::string& systemName) = 0;
    virtual bool shutdownSystem(const std::string& systemName) = 0;
    
    // Lifecycle query
    virtual std::vector<std::string> getSystemNames() const = 0;
    virtual LifecycleStage getSystemLifecycleStage(const std::string& systemName) const = 0;
    virtual bool isSystemRunning(const std::string& systemName) const = 0;
    
    // Lifecycle configuration
    virtual bool configureSystemLifecycle(const std::string& systemName,
                                        const SystemLifecycleConfig& config) = 0;
    virtual SystemLifecycleConfig getSystemLifecycleConfig(const std::string& systemName) const = 0;
    
    // Dependency management
    virtual bool registerSystemDependency(const std::string& systemName,
                                        const std::string& dependencyName) = 0;
    virtual std::vector<std::string> getSystemDependencies(const std::string& systemName) const = 0;
    virtual bool checkSystemDependencies(const std::string& systemName) const = 0;
    
    // Lifecycle operations
    virtual bool startAllSystems() = 0;
    virtual bool stopAllSystems() = 0;
    virtual bool pauseAllSystems() = 0;
    virtual bool resumeAllSystems() = 0;
    
    // Lifecycle management
    virtual std::string getLifecycleStatus() const = 0;
    virtual std::map<std::string, std::any> getLifecycleStatistics() const = 0;
    
    // Event management
    virtual bool registerLifecycleListener(std::shared_ptr<ILifecycleListener> listener) = 0;
    virtual bool unregisterLifecycleListener(const std::string& listenerName) = 0;
    virtual void distributeLifecycleEvent(const LifecycleEvent& event) = 0;
    
    // Validation
    virtual bool validateAllSystems() const = 0;
    virtual std::string getLifecycleDiagnostics() const = 0;
};

// Default System Lifecycle Manager Implementation
class SystemLifecycleManagerImpl : public ISystemLifecycleManager {
public:
    SystemLifecycleManagerImpl();
    ~SystemLifecycleManagerImpl() override;
    
    // Lifecycle management
    bool registerLifecycle(std::shared_ptr<ISystemLifecycle> lifecycle) override;
    bool unregisterLifecycle(const std::string& systemName) override;
    
    // Lifecycle operations
    bool startSystem(const std::string& systemName) override;
    bool stopSystem(const std::string& systemName) override;
    bool pauseSystem(const std::string& systemName) override;
    bool resumeSystem(const std::string& systemName) override;
    bool restartSystem(const std::string& systemName) override;
    bool shutdownSystem(const std::string& systemName) override;
    
    // Lifecycle query
    std::vector<std::string> getSystemNames() const override;
    LifecycleStage getSystemLifecycleStage(const std::string& systemName) const override;
    bool isSystemRunning(const std::string& systemName) const override;
    
    // Lifecycle configuration
    bool configureSystemLifecycle(const std::string& systemName,
                                const SystemLifecycleConfig& config) override;
    SystemLifecycleConfig getSystemLifecycleConfig(const std::string& systemName) const override;
    
    // Dependency management
    bool registerSystemDependency(const std::string& systemName,
                                 const std::string& dependencyName) override;
    std::vector<std::string> getSystemDependencies(const std::string& systemName) const override;
    bool checkSystemDependencies(const std::string& systemName) const override;
    
    // Lifecycle operations
    bool startAllSystems() override;
    bool stopAllSystems() override;
    bool pauseAllSystems() override;
    bool resumeAllSystems() override;
    
    // Lifecycle management
    std::string getLifecycleStatus() const override;
    std::map<std::string, std::any> getLifecycleStatistics() const override;
    
    // Event management
    bool registerLifecycleListener(std::shared_ptr<ILifecycleListener> listener) override;
    bool unregisterLifecycleListener(const std::string& listenerName) override;
    void distributeLifecycleEvent(const LifecycleEvent& event) override;
    
    // Validation
    bool validateAllSystems() const override;
    std::string getLifecycleDiagnostics() const override;
    
    // Global instance access
    static SystemLifecycleManagerImpl& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// System Lifecycle Factory
class SystemLifecycleFactory {
public:
    template<typename T, typename... Args>
    static std::shared_ptr<T> createLifecycle(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    
    template<typename T>
    static std::shared_ptr<T> createLifecycle(const SystemLifecycleConfig& config) {
        // For templated lifecycles, configuration would be handled differently
        throw std::runtime_error("Configuration-based lifecycle creation not implemented for type " + 
                               std::string(typeid(T).name());
    }
};

// System Lifecycle Utilities
class SystemLifecycleUtils {
public:
    // Configuration template management
    static SystemLifecycleConfig createLifecycleConfig(const std::string& systemName,
                                                    LifecycleStage initialStage = LifecycleStage::NotStarted);
    
    // Lifecycle validation
    static bool validateLifecycleStage(LifecycleStage stage, const std::string& systemName);
    static std::string getLifecycleStageDescription(LifecycleStage stage);
    
    // Event utilities
    static LifecycleEvent createLifecycleEvent(LifecycleEventType type, const std::string& message,
                                            const std::map<std::string, std::any>& data = {});
    static bool isLifecycleEventError(LifecycleEventType type);
    
    // Statistics utilities
    static std::string formatLifecycleStatistics(const std::map<std::string, std::any>& stats);
    static double calculateLifecycleUptime(const std::chrono::steady_clock::time_point& startTime,
                                         const std::chrono::steady_clock::time_point& endTime);
};

// System Lifecycle Framework
class SystemLifecycleFramework {
public:
    SystemLifecycleFramework();
    ~SystemLifecycleFramework();
    
    // Initialize the lifecycle framework
    bool initialize();
    
    // Shutdown the lifecycle framework
    void shutdown();
    
    // Get the global lifecycle manager
    ISystemLifecycleManager& getManager() { return manager_; }
    
    // Register core lifecycles
    void registerCoreLifecycles();
    
    // Lifecycle management
    bool startAllSystems();
    bool stopAllSystems();
    bool pauseAllSystems();
    bool resumeAllSystems();
    
    // Lifecycle validation
    bool validateSystemLifecycles();
    bool performSystemHealthChecks();
    
    // Get system status
    std::string getLifecycleStatus() const;
    std::map<std::string, std::any> getLifecycleStatistics() const;
    
    // Event handling
    void notifyLifecycleEvent(const LifecycleEvent& event);
    
    // Global instance access
    static SystemLifecycleFramework& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ISystemLifecycleManager& manager_;
};

} // namespace nlm