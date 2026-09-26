/*
 * NLM Configuration-Driven System Loading - Automated Configuration
 * Provides configuration-driven system loading for neural system
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
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

// Configuration-Driven System Loading Type
enum class LoadingType {
    FileSystem,
    Memory,
    Network,
    Database,
    Dynamic,
    Composite
};

// Configuration-Driven System Error
enum class LoadingError {
    FileNotFound,
    ParseError,
    ValidationError,
    DependencyError,
    InitializationError,
    Timeout,
    PermissionError
};

// Configuration-Driven System Event
struct LoadingEvent {
    LoadingType type;
    std::string message;
    std::map<std::string, std::any> data;
    std::chrono::system_clock::time_point timestamp;
    std::string source;
    bool error = false;
    
    LoadingEvent(LoadingType type, const std::string& message)
        : type(type), message(message), timestamp(std::chrono::system_clock::now()), error(false) {}
    
    LoadingEvent(LoadingType type, const std::string& message, 
                const std::map<std::string, std::any>& data)
        : type(type), message(message), data(data), 
          timestamp(std::chrono::system_clock::now()), error(false) {}
    
    LoadingEvent(LoadingType type, const std::string& message, 
                bool error, const std::map<std::string, std::any>& data = {})
        : type(type), message(message), data(data), 
          timestamp(std::chrono::system_clock::now()), error(error) {}
};

// Configuration-Driven System Event Listener Interface
class ILoadingEventListener {
public:
    virtual ~ILoadingEventListener() = default;
    
    virtual std::string getListenerName() const = 0;
    virtual std::vector<LoadingType> getEventTypes() const = 0;
    
    virtual void onLoadingEvent(const LoadingEvent& event) = 0;
    virtual bool isAsync() const { return false; }
    
    virtual void setEventCallback(std::function<void(const LoadingEvent&)> callback) {
        eventCallback_ = callback;
    }
    
    void notifyEvent(const LoadingEvent& event) {
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
    std::function<void(const LoadingEvent&)> eventCallback_;
};

// System Configuration
struct SystemConfig {
    std::string name;
    std::string description;
    std::map<std::string, std::any> settings;
    std::vector<std::string> dependencies;
    std::map<std::string, std::any> metadata;
    
    SystemConfig() = default;
    SystemConfig(const std::string& name, const std::string& desc) 
        : name(name), description(desc) {}
};

// Component Configuration
struct ComponentConfig {
    std::string name;
    std::string type;
    std::map<std::string, std::any> config;
    std::vector<std::string> dependencies;
    bool enabled = true;
    
    ComponentConfig() = default;
    ComponentConfig(const std::string& name, const std::string& type) 
        : name(name), type(type), enabled(true) {}
};

// Service Configuration
struct ServiceConfig {
    std::string name;
    std::string type;
    std::map<std::string, std::any> config;
    std::vector<std::string> dependencies;
    bool singleton = false;
    
    ServiceConfig() = default;
    ServiceConfig(const std::string& name, const std::string& type) 
        : name(name), type(type), singleton(false) {}
};

// Configuration-Driven System Loader
class ISystemLoader {
public:
    virtual ~ISystemLoader() = default;
    
    virtual LoadingType getLoaderType() const = 0;
    virtual bool canLoad(const std::string& configFile) const = 0;
    
    virtual bool loadConfig(const std::string& configFile, 
                          std::map<std::string, SystemConfig>& systems,
                          std::map<std::string, ComponentConfig>& components,
                          std::map<std::string, ServiceConfig>& services) = 0;
    
    virtual bool saveConfig(const std::string& configFile,
                          const std::map<std::string, SystemConfig>& systems,
                          const std::map<std::string, ComponentConfig>& components,
                          const std::map<std::string, ServiceConfig>& services) = 0;
};

// File System Loader
class FileSystemLoader : public ISystemLoader {
public:
    FileSystemLoader();
    ~FileSystemLoader() override;
    
    LoadingType getLoaderType() const override { return LoadingType::FileSystem; }
    bool canLoad(const std::string& configFile) const override;
    
    bool loadConfig(const std::string& configFile,
                   std::map<std::string, SystemConfig>& systems,
                   std::map<std::string, ComponentConfig>& components,
                   std::map<std::string, ServiceConfig>& services) override;
    
    bool saveConfig(const std::string& configFile,
                   const std::map<std::string, SystemConfig>& systems,
                   const std::map<std::string, ComponentConfig>& components,
                   const std::map<std::string, ServiceConfig>& services) override;
    
    // Set configuration directory
    void setConfigDirectory(const std::string& directory) { configDirectory_ = directory; }
    
    // Get configuration directory
    std::string getConfigDirectory() const { return configDirectory_; }
    
private:
    std::string configDirectory_;
    
    bool loadFromJSON(const std::string& configFile,
                     std::map<std::string, SystemConfig>& systems,
                     std::map<std::string, ComponentConfig>& components,
                     std::map<std::string, ServiceConfig>& services);
    
    bool saveToJSON(const std::string& configFile,
                   const std::map<std::string, SystemConfig>& systems,
                   const std::map<std::string, ComponentConfig>& components,
                   const std::map<std::string, ServiceConfig>& services);
};

// Configuration-Driven System Configuration
struct ConfigurationDrivenSystemConfig {
    std::string configFile;
    LoadingType defaultLoader;
    std::map<std::string, std::any> loaderConfig;
    std::vector<std::string> requiredSystems;
    std::vector<std::string> optionalSystems;
    std::chrono::milliseconds timeout;
    
    ConfigurationDrivenSystemConfig() = default;
    ConfigurationDrivenSystemConfig(const std::string& file) 
        : configFile(file), defaultLoader(LoadingType::FileSystem),
          timeout(std::chrono::milliseconds(30000)) {}
};

// Configuration-Driven System Event Manager
class LoadingEventManager {
public:
    LoadingEventManager();
    ~LoadingEventManager();
    
    // Event listener management
    bool registerLoadingEventListener(std::shared_ptr<ILoadingEventListener> listener);
    bool unregisterLoadingEventListener(const std::string& listenerName);
    bool unregisterLoadingEventListener(std::type_index listenerType);
    
    // Event distribution
    void distributeLoadingEvent(const LoadingEvent& event);
    void distributeLoadingEventAsync(const LoadingEvent& event);
    
    // Event filtering
    std::vector<std::shared_ptr<ILoadingEventListener>> getListenersForEvent(LoadingType type) const;
    std::vector<std::shared_ptr<ILoadingEventListener>> getAllListeners() const;
    
    // Event management
    void clearListeners();
    size_t getListenerCount() const { return listeners_.size(); }
    
    // Global instance access
    static LoadingEventManager& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration-Driven System Loader Manager
class SystemLoaderManager {
public:
    SystemLoaderManager();
    ~SystemLoaderManager();
    
    // Loader management
    bool registerLoader(std::unique_ptr<ISystemLoader> loader);
    bool unregisterLoader(LoadingType type);
    
    // Loader query
    std::vector<std::unique_ptr<ISystemLoader>> getAllLoaders() const;
    ISystemLoader* getLoader(LoadingType type) const;
    bool hasLoader(LoadingType type) const;
    
    // Loader selection
    ISystemLoader* selectLoader(const std::string& configFile) const;
    
    // Global instance access
    static SystemLoaderManager& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration-Driven System Interface
class IConfigurationDrivenSystem {
public:
    virtual ~IConfigurationDrivenSystem() = default;
    
    virtual std::string getSystemName() const = 0;
    virtual std::string getSystemVersion() const = 0;
    
    virtual bool loadFromConfig(const std::map<std::string, std::any>& config) = 0;
    virtual std::map<std::string, std::any> getConfig() const = 0;
    
    virtual bool validateConfig() const = 0;
    virtual std::string validateConfigAll() const = 0;
    
    virtual std::map<std::string, std::any> getLoadedConfig() const { return std::map<std::string, std::any>(); }
    
    virtual bool configureFromFile(const std::string& configFile) = 0;
    virtual bool saveToFile(const std::string& configFile) const = 0;
};

// Default Configuration-Driven System Base Class
class ConfigurationDrivenSystem : public IConfigurationDrivenSystem {
public:
    ConfigurationDrivenSystem(const std::string& name, const std::string& version);
    virtual ~ConfigurationDrivenSystem() override;
    
    std::string getSystemName() const override { return systemName_; }
    std::string getSystemVersion() const override { return systemVersion_; }
    
    virtual bool internalLoadFromConfig(const std::map<std::string, std::any>& config) = 0;
    
    bool loadFromConfig(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfig() const override;
    
    bool validateConfig() const override;
    std::string validateConfigAll() const override;
    
    bool configureFromFile(const std::string& configFile) override;
    bool saveToFile(const std::string& configFile) const override;
    
protected:
    std::string systemName_;
    std::string systemVersion_;
    std::map<std::string, std::any> loadedConfig_;
    std::map<std::string, std::any> currentConfig_;
    
    std::vector<LoadingEvent> loadingEvents_;
    mutable std::mutex eventsMutex_;
};

// Configuration-Driven System Manager
class ConfigurationDrivenSystemManager {
public:
    ConfigurationDrivenSystemManager();
    ~ConfigurationDrivenSystemManager();
    
    // Initialize the configuration-driven system manager
    bool initialize();
    
    // Shutdown the configuration-driven system manager
    void shutdown();
    
    // System configuration management
    bool configureSystem(const std::string& systemName,
                       const ConfigurationDrivenSystemConfig& config);
    bool loadSystemConfig(const std::string& systemName,
                        const std::string& configFile);
    bool saveSystemConfig(const std::string& systemName,
                        const std::string& configFile) const;
    
    // System lifecycle management
    bool startSystem(const std::string& systemName);
    bool stopSystem(const std::string& systemName);
    bool restartSystem(const std::string& systemName);
    
    // System query
    std::vector<std::string> getSystemNames() const;
    ConfigurationDrivenSystemConfig getSystemConfig(const std::string& systemName) const;
    std::map<std::string, std::any> getSystemLoadedConfig(const std::string& systemName) const;
    
    // Configuration management
    bool loadAllSystemsFromConfig(const std::string& configDirectory);
    bool saveAllSystemsToConfig(const std::string& configDirectory) const;
    
    // System validation
    bool validateSystemConfig(const std::string& systemName) const;
    std::string validateAllSystemConfigs() const;
    
    // Get manager status
    std::string getSystemManagerStatus() const;
    std::map<std::string, std::any> getManagerStatistics() const;
    
    // Event handling
    bool registerLoadingEventListener(std::shared_ptr<ILoadingEventListener> listener);
    bool unregisterLoadingEventListener(const std::string& listenerName);
    void distributeLoadingEvent(const LoadingEvent& event);
    
    // Global instance access
    static ConfigurationDrivenSystemManager& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration-Driven System Framework
class ConfigurationDrivenSystemFramework {
public:
    ConfigurationDrivenSystemFramework();
    ~ConfigurationDrivenSystemFramework();
    
    // Initialize the configuration-driven system framework
    bool initialize();
    
    // Shutdown the configuration-driven system framework
    void shutdown();
    
    // Get the global configuration-driven system manager
    ConfigurationDrivenSystemManager& getManager() { return manager_; }
    
    // Register core systems
    void registerCoreSystems();
    
    // System lifecycle management
    bool loadAllSystems();
    bool saveAllSystems();
    
    // System validation
    bool validateSystemConfiguration();
    bool performSystemConfigurationHealthCheck();
    
    // Get framework status
    std::string getFrameworkStatus() const;
    std::map<std::string, std::any> getFrameworkStatistics() const;
    
    // Event handling
    void notifyLoadingEvent(const LoadingEvent& event);
    
    // Global instance access
    static ConfigurationDrivenSystemFramework& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigurationDrivenSystemManager& manager_;
};

} // namespace nlm