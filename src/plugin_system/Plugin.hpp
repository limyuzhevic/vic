#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <typeindex>
#include <any>
#include <mutex>
#include <shared_mutex>

namespace nlm {

// Plugin system version
constexpr int PLUGIN_SYSTEM_VERSION = 1;

// Plugin types
enum class PluginType {
    Core,           // Essential functionality
    Component,      // Neural component extension
    Algorithm,      // Learning algorithm extension
    Visualization,  // Visualization plugin
    Integration,    // External system integration
    Utility,        // Utility functions
    Custom          // User-defined plugin
};

// Plugin capability flags
enum class PluginCapability {
    MultiThread,          // Supports multi-threaded operation
    RealTime,             // Real-time processing required
    GPUAccelerated,       // Requires GPU acceleration
    Memory intensive,     // High memory usage
    Network Dependent,    // Requires network connectivity
    External Resources,   // Requires external data/files
    User Input Required,  // Interactive plugin
    Hot Reloadable,       // Supports hot reloading
    Sandboxed,            // Isolated execution environment
    Debuggable,          // Supports debugging
    Testable,            // Has unit tests
    Documented,          // Has documentation
    Licensed,            // Has explicit license
    Version Controlled,   // Integrates with version control
    Monitoring Supported, // Provides monitoring data
    Configurable,        // Has configuration options
    Event Driven,        // Uses event-driven architecture
    Callback Enabled     // Supports callbacks
};

// Plugin lifecycle states
enum class PluginState {
    Unloaded,
    Loading,
    Loaded,
    Initializing,
    Initialized,
    Configuring,
    Configured,
    Activating,
    Active,
    Running,
    ShuttingDown,
    Failed,
    Disabled
};

// Plugin event types
enum class PluginEventType {
    PluginLoad,
    PluginUnload,
    PluginInitialize,
    PluginConfigure,
    PluginActivate,
    PluginDeactivate,
    PluginError,
    PluginStatusChange,
    ConfigurationChange,
    DependencyResolved,
    DependencyFailed,
    HotReload,
    MemoryUsageChanged,
    PerformanceMetric
};

// Plugin event base class
class PluginEvent {
public:
    virtual ~PluginEvent() = default;
    virtual PluginEventType getType() const = 0;
    virtual std::string getSource() const = 0;
    virtual std::chrono::system_clock::time_point getTimestamp() const = 0;
    virtual std::string toString() const = 0;
};

// Plugin metadata
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string website;
    PluginType type;
    std::vector<PluginCapability> capabilities;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> metadata;
    
    // File information
    std::filesystem::path filePath;
    std::filesystem::path directory;
    std::string checksum;
    std::chrono::system_clock::time_point lastModified;
    
    // Requirements
    std::string minimumSystemVersion;
    std::map<std::string, std::string> systemRequirements;
    std::vector<std::string> optionalDependencies;
    
    // Configuration
    std::string configSchema;
    bool requiresConfiguration = false;
    std::string defaultConfigFile;
    
    // Safety and security
    bool isTrusted = false;
    bool sandboxed = false;
    std::vector<std::string> allowedActions;
    std::vector<std::string> blockedActions;
    
    // Performance
    double estimatedMemoryUsage = 0.0;
    double estimatedCPUUsage = 0.0;
    size_t estimatedThreadCount = 1;
    
    // Documentation
    std::string documentationPath;
    std::vector<std::string> examples;
    std::vector<std::string> tutorials;
    
    PluginMetadata() : lastModified(std::chrono::system_clock::now()) {}
};

// Plugin event data
struct PluginEventData {
    PluginEventType type;
    std::string source;
    std::chrono::system_clock::time_point timestamp;
    std::string message;
    std::unordered_map<std::string, std::any> data;
    std::shared_ptr<PluginEvent> event;
    
    template<typename T>
    T getData(const std::string& key) const {
        auto it = data.find(key);
        if (it != data.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                throw std::runtime_error("Data type mismatch for key: " + key);
            }
        }
        throw std::runtime_error("Key not found: " + key);
    }
    
    template<typename T>
    void setData(const std::string& key, const T& value) {
        data[key] = value;
    }
};

// Plugin interface
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin identification
    virtual const PluginMetadata& getMetadata() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    
    // Plugin lifecycle
    virtual bool load() = 0;
    virtual bool unload() = 0;
    virtual bool initialize() = 0;
    virtual bool configure(const std::string& configData = "") = 0;
    virtual bool activate() = 0;
    virtual bool deactivate() = 0;
    virtual bool shutdown() = 0;
    
    // State management
    virtual PluginState getState() const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;
    virtual bool isDependencyResolved(const std::string& dependencyName) const = 0;
    
    // Core functionality
    virtual std::vector<std::string> getCapabilities() const = 0;
    virtual bool hasCapability(PluginCapability capability) const = 0;
    
    // Event handling
    virtual bool handleEvent(const PluginEventData& event) = 0;
    virtual void addEventListener(PluginEventType eventType,
                                 std::function<bool(const PluginEventData&)> callback) = 0;
    virtual void removeEventListener(PluginEventType eventType,
                                    std::function<bool(const PluginEventData&)> callback) = 0;
    
    // Configuration management
    virtual std::string getConfiguration() const = 0;
    virtual bool setConfiguration(const std::string& configData) = 0;
    virtual bool validateConfiguration(const std::string& configData) const = 0;
    
    // Performance and monitoring
    virtual double getCPUUsage() const = 0;
    virtual size_t getMemoryUsage() const = 0;
    virtual size_t getThreadCount() const = 0;
    virtual std::chrono::nanoseconds getUptime() const = 0;
    
    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getError() const = 0;
    virtual void clearError() = 0;
    
    // Extension points
    virtual std::vector<std::any> getExtensions() const = 0;
    virtual bool addExtension(const std::any& extension) = 0;
    virtual bool removeExtension(const std::any& extension) = 0;
    
    // Security and sandboxing
    virtual bool isSandboxed() const = 0;
    virtual bool executeAction(const std::string& action, const std::vector<std::string>& parameters) = 0;
    virtual bool canExecuteAction(const std::string& action) const = 0;
    
    // Configuration
    virtual void setPluginManager(void* manager) = 0;
    virtual void* getPluginManager() const = 0;
};

// Plugin manager interface
class IPluginManager {
public:
    virtual ~IPluginManager() = default;
    
    // Plugin discovery
    virtual std::vector<PluginMetadata> discoverPlugins(const std::filesystem::path& directory) = 0;
    virtual bool loadPlugin(const std::string& pluginName) = 0;
    virtual bool unloadPlugin(const std::string& pluginName) = 0;
    virtual bool loadPluginFromFile(const std::filesystem::path& filepath) = 0;
    
    // Plugin lifecycle management
    virtual bool initializePlugin(const std::string& pluginName) = 0;
    virtual bool configurePlugin(const std::string& pluginName, const std::string& configData) = 0;
    virtual bool activatePlugin(const std::string& pluginName) = 0;
    virtual bool deactivatePlugin(const std::string& pluginName) = 0;
    virtual bool shutdownPlugin(const std::string& pluginName) = 0;
    
    // Plugin state management
    virtual PluginState getPluginState(const std::string& pluginName) const = 0;
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
    virtual bool isPluginLoaded(const std::string& pluginName) const = 0;
    virtual bool isPluginActive(const std::string& pluginName) const = 0;
    
    // Plugin information
    virtual const PluginMetadata& getPluginMetadata(const std::string& pluginName) const = 0;
    virtual std::vector<std::string> getPluginDependencies(const std::string& pluginName) const = 0;
    virtual bool areDependenciesSatisfied(const std::string& pluginName) const = 0;
    
    // Event system
    virtual bool addEventListener(PluginEventType eventType,
                                 std::function<bool(const PluginEventData&)> callback) = 0;
    virtual bool removeEventListener(PluginEventType eventType,
                                    std::function<bool(const PluginEventData&)> callback) = 0;
    virtual void emitEvent(const PluginEventData& event) = 0;
    
    // Configuration management
    virtual bool setGlobalConfiguration(const std::string& configData) = 0;
    virtual std::string getGlobalConfiguration() const = 0;
    virtual bool validateGlobalConfiguration(const std::string& configData) const = 0;
    
    // Performance monitoring
    virtual double getSystemCPUUsage() const = 0;
    virtual size_t getSystemMemoryUsage() const = 0;
    virtual size_t getActivePluginCount() const = 0;
    virtual std::chrono::nanoseconds getSystemUptime() const = 0;
    
    // Plugin management
    virtual bool disablePlugin(const std::string& pluginName) = 0;
    virtual bool enablePlugin(const std::string& pluginName) = 0;
    virtual bool restartPlugin(const std::string& pluginName) = 0;
    virtual bool updatePlugin(const std::string& pluginName, const std::filesystem::path& newFilepath) = 0;
    
    // Batch operations
    virtual bool loadPlugins(const std::vector<std::string>& pluginNames) = 0;
    virtual bool unloadPlugins(const std::vector<std::string>& pluginNames) = 0;
    virtual bool activatePlugins(const std::vector<std::string>& pluginNames) = 0;
    virtual bool deactivatePlugins(const std::vector<std::string>& pluginNames) = 0;
    
    // Plugin repository and marketplace
    virtual bool addPluginRepository(const std::string& name, const std::filesystem::path& path) = 0;
    virtual bool removePluginRepository(const std::string& name) = 0;
    virtual std::vector<PluginMetadata> searchPlugins(const std::string& query) = 0;
    virtual bool installPlugin(const std::string& pluginName) = 0;
    virtual bool uninstallPlugin(const std::string& pluginName) = 0;
    
    // Security and sandboxing
    virtual bool setPluginSecurityPolicy(const std::string& pluginName, const std::vector<std::string>& allowedActions) = 0;
    virtual std::vector<std::string> getPluginSecurityPolicy(const std::string& pluginName) const = 0;
    virtual bool isPluginSandboxed(const std::string& pluginName) const = 0;
    
    // Hot reloading
    virtual bool enableHotReloading(bool enabled) = 0;
    virtual bool isHotReloadingEnabled() const = 0;
    virtual bool checkForPluginUpdates() = 0;
    virtual bool reloadPlugin(const std::string& pluginName) = 0;
    
    // Diagnostics and debugging
    virtual std::string getSystemStatus() const = 0;
    virtual std::string getPluginStatus(const std::string& pluginName) const = 0;
    virtual std::vector<std::string> getPluginErrors(const std::string& pluginName) const = 0;
    virtual void clearPluginErrors(const std::string& pluginName) = 0;
    
    // Statistics and analytics
    virtual size_t getPluginLoadCount(const std::string& pluginName) const = 0;
    virtual size_t getPluginUnloadCount(const std::string& pluginName) const = 0;
    virtual double getPluginSuccessRate(const std::string& pluginName) const = 0;
    virtual std::chrono::nanoseconds getAveragePluginLoadTime(const std::string& pluginName) const = 0;
    
    // Lifecycle callbacks
    virtual void setLoadCallback(std::function<void(const std::string&)> callback) = 0;
    virtual void setUnloadCallback(std::function<void(const std::string&)> callback) = 0;
    virtual void setErrorCallback(std::function<void(const std::string&, const std::string&)> callback) = 0;
    virtual void setStatusChangeCallback(std::function<void(const std::string&, PluginState, PluginState)> callback) = 0;
};

// Plugin registration macro
#define REGISTER_PLUGIN(PluginClass) \
    extern "C" IPlugin* createPlugin() { \
        return new PluginClass(); \
    } \
    extern "C" void destroyPlugin(IPlugin* plugin) { \
        delete plugin; \
    } \
    extern "C" int getPluginVersion() { \
        return PLUGIN_SYSTEM_VERSION; \
    } \
    extern "C" const char* getPluginName() { \
        return #PluginClass; \
    }

// Plugin metadata registration macro
#define PLUGIN_METADATA(name, version, description, author, license, type) \
    PluginMetadata getDefaultMetadata() { \
        PluginMetadata metadata; \
        metadata.name = name; \
        metadata.version = version; \
        metadata.description = description; \
        metadata.author = author; \
        metadata.license = license; \
        metadata.type = type; \
        return metadata; \
    }

// Plugin capability registration macro
#define PLUGIN_CAPABILITIES(...) \
    std::vector<PluginCapability> getCapabilities() { \
        std::vector<PluginCapability> caps = {__VA_ARGS__}; \
        return caps; \
    }

// Plugin dependency registration macro
#define PLUGIN_DEPENDENCIES(...) \
    std::vector<std::string> getDependencies() { \
        std::vector<std::string> deps = {__VA_ARGS__}; \
        return deps; \
    }

// Base plugin implementation
class BasePlugin : public IPlugin {
protected:
    PluginMetadata metadata_;
    PluginState state_;
    IPluginManager* pluginManager_;
    std::mutex mutex_;
    std::unordered_map<PluginEventType, std::vector<std::function<bool(const PluginEventData&)>>> eventListeners_;
    std::vector<std::any> extensions_;
    std::string error_;
    
public:
    BasePlugin() : state_(PluginState::Unloaded), pluginManager_(nullptr), error_("") {}
    
    virtual ~BasePlugin() = default;
    
    // Plugin identification
    const PluginMetadata& getMetadata() const override { return metadata_; }
    std::string getName() const override { return metadata_.name; }
    std::string getVersion() const override { return metadata_.version; }
    
    // Plugin lifecycle
    bool load() override {
        std::lock_guard<std::mutex> lock(mutex_);
        state_ = PluginState::Loading;
        
        try {
            if (!onLoad()) {
                state_ = PluginState::Failed;
                error_ = "Plugin load failed in onLoad";
                return false;
            }
            
            // Check dependencies
            if (!checkDependencies()) {
                state_ = PluginState::Failed;
                error_ = "Plugin dependencies not satisfied";
                return false;
            }
            
            state_ = PluginState::Loaded;
            return true;
        } catch (const std::exception& e) {
            state_ = PluginState::Failed;
            error_ = "Plugin load exception: " + std::string(e.what());
            return false;
        }
    }
    
    bool unload() override {
        std::lock_guard<std::mutex> lock(mutex_);
        state_ = PluginState::ShuttingDown;
        
        try {
            onShutdown();
            state_ = PluginState::Unloaded;
            return true;
        } catch (const std::exception& e) {
            state_ = PluginState::Failed;
            error_ = "Plugin unload exception: " + std::string(e.what());
            return false;
        }
    }
    
    bool initialize() override {
        std::lock_guard<std::mutex> lock(mutex_);
        state_ = PluginState::Initializing;
        
        try {
            if (!onInitialize()) {
                state_ = PluginState::Failed;
                error_ = "Plugin initialize failed in onInitialize";
                return false;
            }
            
            state_ = PluginState::Initialized;
            return true;
        } catch (const std::exception& e) {
            state_ = PluginState::Failed;
            error_ = "Plugin initialize exception: " + std::string(e.what());
            return false;
        }
    }
    
    bool configure(const std::string& configData = "") override {
        std::lock_guard<std::mutex> lock(mutex_);
        state_ = PluginState::Configuring;
        
        try {
            if (!onConfigure(configData)) {
                state_ = PluginState::Failed;
                error_ = "Plugin configure failed in onConfigure";
                return false;
            }
            
            state_ = PluginState::Configured;
            return true;
        } catch (const std::exception& e) {
            state_ = PluginState::Failed;
            error_ = "Plugin configure exception: " + std::string(e.what());
            return false;
        }
    }
    
    bool activate() override {
        std::lock_guard<std::mutex> lock(mutex_);
        state_ = PluginState::Activating;
        
        try {
            if (!onActivate()) {
                state_ = PluginState::Failed;
                error_ = "Plugin activate failed in onActivate";
                return false;
            }
            
            state_ = PluginState::Active;
            return true;
        } catch (const std::exception& e) {
            state_ = PluginState::Failed;
            error_ = "Plugin activate exception: " + std::string(e.what());
            return false;
        }
    }
    
    bool deactivate() override {
        std::lock_guard<std::mutex> lock(mutex_);
        state_ = PluginState::ShuttingDown;
        
        try {
            onDeactivate();
            state_ = PluginState::Configured;
            return true;
        } catch (const std::exception& e) {
            state_ = PluginState::Failed;
            error_ = "Plugin deactivate exception: " + std::string(e.what());
            return false;
        }
    }
    
    bool shutdown() override {
        std::lock_guard<std::mutex> lock(mutex_);
        state_ = PluginState::ShuttingDown;
        
        try {
            onShutdown();
            state_ = PluginState::Unloaded;
            return true;
        } catch (const std::exception& e) {
            state_ = PluginState::Failed;
            error_ = "Plugin shutdown exception: " + std::string(e.what());
            return false;
        }
    }
    
    // State management
    PluginState getState() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return state_;
    }
    
    std::vector<std::string> getDependencies() const override {
        return metadata_.dependencies;
    }
    
    bool isDependencyResolved(const std::string& dependencyName) const override {
        // Implementation depends on plugin manager
        return true;
    }
    
    // Core functionality
    std::vector<std::string> getCapabilities() const override {
        return {};
    }
    
    bool hasCapability(PluginCapability capability) const override {
        return std::find(metadata_.capabilities.begin(), metadata_.capabilities.end(), capability) != metadata_.capabilities.end();
    }
    
    // Event handling
    bool handleEvent(const PluginEventData& event) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = eventListeners_.find(event.type);
        if (it != eventListeners_.end()) {
            for (auto& callback : it->second) {
                if (callback(event)) {
                    return true;
                }
            }
        }
        
        return onEvent(event);
    }
    
    void addEventListener(PluginEventType eventType,
                         std::function<bool(const PluginEventData&)> callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        eventListeners_[eventType].push_back(callback);
    }
    
    void removeEventListener(PluginEventType eventType,
                            std::function<bool(const PluginEventData&)> callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = eventListeners_.find(eventType);
        if (it != eventListeners_.end()) {
            it->second.erase(
                std::remove(it->second.begin(), it->second.end(), callback),
                it->second.end()
            );
        }
    }
    
    // Configuration management
    virtual std::string getConfiguration() const override {
        return "";
    }
    
    virtual bool setConfiguration(const std::string& configData) override {
        return onConfigure(configData);
    }
    
    virtual bool validateConfiguration(const std::string& configData) const override {
        return true;
    }
    
    // Performance and monitoring
    virtual double getCPUUsage() const override {
        return 0.0;
    }
    
    virtual size_t getMemoryUsage() const override {
        return 0;
    }
    
    virtual size_t getThreadCount() const override {
        return 1;
    }
    
    virtual std::chrono::nanoseconds getUptime() const override {
        return std::chrono::nanoseconds(0);
    }
    
    // Error handling
    bool hasError() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return !error_.empty();
    }
    
    std::string getError() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return error_;
    }
    
    void clearError() override {
        std::lock_guard<std::mutex> lock(mutex_);
        error_.clear();
    }
    
    // Extension points
    std::vector<std::any> getExtensions() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return extensions_;
    }
    
    bool addExtension(const std::any& extension) override {
        std::lock_guard<std::mutex> lock(mutex_);
        extensions_.push_back(extension);
        return true;
    }
    
    bool removeExtension(const std::any& extension) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = std::find(extensions_.begin(), extensions_.end(), extension);
        if (it != extensions_.end()) {
            extensions_.erase(it);
            return true;
        }
        return false;
    }
    
    // Security and sandboxing
    virtual bool isSandboxed() const override {
        return metadata_.sandboxed;
    }
    
    virtual bool executeAction(const std::string& action, const std::vector<std::string>& parameters) override {
        if (!canExecuteAction(action)) {
            return false;
        }
        
        return onExecuteAction(action, parameters);
    }
    
    virtual bool canExecuteAction(const std::string& action) const override {
        return std::find(metadata_.allowedActions.begin(), metadata_.allowedActions.end(), action) != metadata_.allowedActions.end();
    }
    
    // Configuration
    void setPluginManager(void* manager) override {
        std::lock_guard<std::mutex> lock(mutex_);
        pluginManager_ = static_cast<IPluginManager*>(manager);
    }
    
    void* getPluginManager() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return pluginManager_;
    }
    
protected:
    // Virtual methods for plugin-specific implementation
    virtual bool onLoad() {
        return true;
    }
    
    virtual void onShutdown() {}
    
    virtual bool onInitialize() {
        return true;
    }
    
    virtual bool onConfigure(const std::string& configData) {
        return true;
    }
    
    virtual bool onActivate() {
        return true;
    }
    
    virtual void onDeactivate() {}
    
    virtual bool onEvent(const PluginEventData& event) {
        return false;
    }
    
    virtual bool onExecuteAction(const std::string& action, const std::vector<std::string>& parameters) {
        return false;
    }
    
    bool checkDependencies() const {
        // Implementation depends on plugin manager
        return true;
    }
};

// Plugin event implementation
class PluginEventImpl : public PluginEvent {
private:
    PluginEventType type_;
    std::string source_;
    std::chrono::system_clock::time_point timestamp_;
    std::unordered_map<std::string, std::any> data_;
    
public:
    PluginEventImpl(PluginEventType type, const std::string& source, 
                   const std::unordered_map<std::string, std::any>& data = {})
        : type_(type), source_(source), timestamp_(std::chrono::system_clock::now()), data_(data) {}
    
    PluginEventType getType() const override {
        return type_;
    }
    
    std::string getSource() const override {
        return source_;
    }
    
    std::chrono::system_clock::time_point getTimestamp() const override {
        return timestamp_;
    }
    
    std::string toString() const override {
        std::ostringstream oss;
        oss << "PluginEvent(type=" << static_cast<int>(type_) 
            << ", source=" << source_ 
            << ", timestamp=" << std::chrono::system_clock::to_time_t(timestamp_)
            << ")";
        return oss.str();
    }
    
    template<typename T>
    T getData(const std::string& key) const {
        auto it = data_.find(key);
        if (it != data_.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                throw std::runtime_error("Data type mismatch for key: " + key);
            }
        }
        throw std::runtime_error("Key not found: " + key);
    }
    
    template<typename T>
    void setData(const std::string& key, const T& value) {
        data_[key] = value;
    }
};

// Default plugin event factory
inline std::shared_ptr<PluginEvent> createPluginEvent(PluginEventType type, const std::string& source,
                                                       const std::unordered_map<std::string, std::any>& data = {}) {
    return std::make_shared<PluginEventImpl>(type, source, data);
}

} // namespace nlm
