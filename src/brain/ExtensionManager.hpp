#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <any>
#include <mutex>
#include "../core/Config/Config.hpp"
#include "factory/ComponentFactory.hpp"

namespace nlm {

// Forward declarations
class Brain;
class ExtensionPoint;
class EventSystem;
class CommandRegistry;
class PluginManager;

// Extension interface
class Extension {
public:
    virtual ~Extension() = default;
    
    // Get extension name
    virtual std::string getName() const = 0;
    
    // Get extension version
    virtual std::string getVersion() const = 0;
    
    // Get extension category
    virtual std::string getCategory() const = 0;
    
    // Get extension description
    virtual std::string getDescription() const = 0;
    
    // Initialize extension
    virtual bool initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) = 0;
    
    // Shutdown extension
    virtual void shutdown() = 0;
    
    // Get component types provided by this extension
    virtual std::vector<std::string> getComponentTypes() const = 0;
    
    // Get command names provided by this extension
    virtual std::vector<std::string> getCommands() const = 0;
    
    // Get extension point names
    virtual std::vector<std::string> getExtensionPoints() const = 0;
    
    // Get event names
    virtual std::vector<std::string> getEvents() const = 0;
};

// Extension manager for registering and managing custom extensions
class ExtensionManager {
public:
    // Extension lifecycle
    enum class LifecycleState {
        Unloaded,
        Loading,
        Loaded,
        Initializing,
        Initialized,
        ShuttingDown,
        Failed
    };
    
    // Extension info
    struct ExtensionInfo {
        std::string name;
        std::string version;
        std::string category;
        std::string description;
        LifecycleState state;
        std::shared_ptr<Extension> extension;
        std::unordered_map<std::string, std::any> metadata;
        std::chrono::steady_clock::time_point loadTime;
        std::chrono::steady_clock::time_point lastUpdate;
        
        ExtensionInfo() : state(LifecycleState::Unloaded), extension(nullptr) {}
    };
    
    // Configuration for extension loading
    struct LoadConfig {
        bool autoLoad = true;
        bool autoInitialize = true;
        std::vector<std::string> requiredComponents;
        std::vector<std::string> requiredCommands;
        std::unordered_map<std::string, std::any> parameters;
        bool enableEvents = true;
        bool enableCommands = true;
        bool enableExtensionPoints = true;
        
        LoadConfig() = default;
    };
    
    // Event callback
    using EventCallback = std::function<void(const std::string&, const std::any&)>;
    
    // Get singleton instance
    static ExtensionManager& getInstance() {
        static ExtensionManager instance;
        return instance;
    }
    
    // Load an extension from file
    bool loadExtension(const std::string& filepath, const LoadConfig& config = LoadConfig());
    
    // Load an extension from module
    bool loadExtension(const std::string& name, const std::string& modulePath, const LoadConfig& config = LoadConfig());
    
    // Load an extension from a factory function
    bool loadExtension(const std::string& name, 
                      std::function<std::unique_ptr<Extension>()> factory,
                      const LoadConfig& config = LoadConfig());
    
    // Unload an extension
    bool unloadExtension(const std::string& name);
    
    // Initialize an extension
    bool initializeExtension(const std::string& name);
    
    // Shutdown an extension
    bool shutdownExtension(const std::string& name);
    
    // Get extension by name
    ExtensionInfo* getExtension(const std::string& name);
    
    // Get all extensions
    std::vector<ExtensionInfo*> getAllExtensions() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<ExtensionInfo*> result;
        result.reserve(extensions_.size());
        for (auto& pair : extensions_) {
            result.push_back(&pair.second);
        }
        return result;
    }
    
    // Get extensions by category
    std::vector<ExtensionInfo*> getExtensionsByCategory(const std::string& category) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<ExtensionInfo*> result;
        for (auto& pair : extensions_) {
            if (pair.second.category == category) {
                result.push_back(&pair.second);
            }
        }
        return result;
    }
    
    // Get extensions by state
    std::vector<ExtensionInfo*> getExtensionsByState(LifecycleState state) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<ExtensionInfo*> result;
        for (auto& pair : extensions_) {
            if (pair.second.state == state) {
                result.push_back(&pair.second);
            }
        }
        return result;
    }
    
    // Register extension point
    bool registerExtensionPoint(const std::string& name, const std::string& description = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        extensionPoints_[name] = description;
        return true;
    }
    
    // Register event
    bool registerEvent(const std::string& name, const std::string& description = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        events_[name] = description;
        return true;
    }
    
    // Add event callback
    void addEventCallback(const std::string& eventName, EventCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        eventCallbacks_[eventName].push_back(callback);
    }
    
    // Emit event
    void emitEvent(const std::string& eventName, const std::any& data = {}) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = eventCallbacks_.find(eventName);
        if (it != eventCallbacks_.end()) {
            for (const auto& callback : it->second) {
                callback(eventName, data);
            }
        }
    }
    
    // Get all extension points
    std::vector<std::string> getExtensionPoints() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        names.reserve(extensionPoints_.size());
        for (const auto& pair : extensionPoints_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get all events
    std::vector<std::string> getEvents() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> names;
        names.reserve(events_.size());
        for (const auto& pair : events_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Update extension metadata
    void updateExtensionMetadata(const std::string& name, const std::unordered_map<std::string, std::any>& metadata) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = extensions_.find(name);
        if (it != extensions_.end()) {
            for (const auto& pair : metadata) {
                it->second.metadata[pair.first] = pair.second;
            }
            it->second.lastUpdate = std::chrono::steady_clock::now();
        }
    }
    
    // Get extension metadata
    std::unordered_map<std::string, std::any> getExtensionMetadata(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = extensions_.find(name);
        if (it != extensions_.end()) {
            return it->second.metadata;
        }
        return {};
    }
    
    // Save extension manager state
    bool saveState(const std::string& filepath) const;
    
    // Load extension manager state
    bool loadState(const std::string& filepath);
    
    // Clear all extensions
    void clearAllExtensions() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& pair : extensions_) {
            if (pair.second.state == LifecycleState::Loaded || pair.second.state == LifecycleState::Initialized) {
                if (pair.second.extension) {
                    pair.second.extension->shutdown();
                }
            }
        }
        extensions_.clear();
    }
    
    // Get extension count
    size_t getExtensionCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return extensions_.size();
    }
    
    // Check if extension exists
    bool hasExtension(const std::string& name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return extensions_.find(name) != extensions_.end();
    }
    
private:
    ExtensionManager() = default;
    ExtensionManager(const ExtensionManager&) = delete;
    ExtensionManager& operator=(const ExtensionManager&) = delete;
    
    // Internal methods
    bool loadDynamicExtension(const std::string& name, const std::string& modulePath, const LoadConfig& config);
    bool loadStaticExtension(std::unique_ptr<Extension> extension, const LoadConfig& config);
    
    std::unordered_map<std::string, ExtensionInfo> extensions_;
    std::unordered_map<std::string, std::string> extensionPoints_;
    std::unordered_map<std::string, std::string> events_;
    std::unordered_map<std::string, std::vector<EventCallback>> eventCallbacks_;
    mutable std::mutex mutex_;
};

} // namespace nlm
