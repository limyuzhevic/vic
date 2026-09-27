#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <any>
#include <mutex>
#include <functional>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Forward declarations
class ExtensionManager;

// Event types for brain behavior extension
template<typename EventData>
struct Event {
    std::string eventName;
    std::string sourceExtension;
    EventData data;
    uint64_t timestamp;
    std::unordered_map<std::string, std::any> metadata;
    
    Event() : timestamp(0) {}
    Event(const std::string& name, const std::string& source, const EventData& data)
        : eventName(name), sourceExtension(source), data(data) {
        timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }
};

// Extension point handler
using ExtensionPointHandler = std::function<bool(const std::string&, const std::any&, const std::unordered_map<std::string, std::any>&)>;

// Extension system for integrating custom extensions
class ExtensionSystem {
public:
    // Event listener
    class EventListener {
    public:
        virtual ~EventListener() = default;
        
        // Handle event
        virtual bool onEvent(const Event<std::any>& event) = 0;
        
        // Get event types this listener handles
        virtual std::vector<std::string> getEventTypes() const = 0;
        
        // Get listener name
        virtual std::string getName() const = 0;
    };
    
    // Extension system configuration
    struct Config {
        bool enableExtensionPoints = true;
        bool enableEventSystem = true;
        bool enablePluginArchitecture = true;
        bool autoDiscoverExtensions = true;
        std::string extensionPath = "./extensions";
        bool enableLogging = true;
        bool enableDebugging = false;
        
        Config() = default;
    };
    
    // Get singleton instance
    static ExtensionSystem& getInstance() {
        static ExtensionSystem instance;
        return instance;
    }
    
    // Initialize extension system
    bool initialize(const Config& config, std::shared_ptr<Config> globalConfig, std::shared_ptr<Brain> brain);
    
    // Shutdown extension system
    void shutdown();
    
    // Add extension point
    bool addExtensionPoint(const std::string& name, const std::string& description = "",
                          const std::vector<std::string>& parameterTypes = {});
    
    // Remove extension point
    bool removeExtensionPoint(const std::string& name);
    
    // Register extension point handler
    bool registerExtensionPointHandler(const std::string& pointName,
                                       ExtensionPointHandler handler,
                                       const std::string& handlerName = "",
                                       int priority = 0);
    
    // Remove extension point handler
    bool removeExtensionPointHandler(const std::string& pointName, const std::string& handlerName);
    
    // Execute extension point
    std::vector<bool> executeExtensionPoint(const std::string& pointName,
                                           const std::any& data,
                                           const std::unordered_map<std::string, std::any>& parameters = {});
    
    // Add event listener
    bool addEventListener(std::unique_ptr<EventListener> listener);
    
    // Remove event listener
    bool removeEventListener(const std::string& listenerName);
    
    // Emit event
    bool emitEvent(const std::string& eventName, const std::any& data,
                  const std::string& sourceExtension = "");
    
    // Get extension points
    std::vector<std::string> getExtensionPoints() const;
    
    // Get extension point handlers
    std::vector<std::string> getExtensionPointHandlers(const std::string& pointName) const;
    
    // Get event listeners
    std::vector<std::string> getEventListenerNames() const;
    
    // Get events
    std::vector<std::string> getEvents() const;
    
    // Get extension system status
    std::string getStatus() const;
    
    // Load extensions from path
    bool loadExtensionsFromPath(const std::string& path);
    
    // Load extension by name
    bool loadExtension(const std::string& name);
    
    // Unload extension
    bool unloadExtension(const std::string& name);
    
    // Check if extension point exists
    bool hasExtensionPoint(const std::string& name) const;
    
    // Check if event exists
    bool hasEvent(const std::string& eventName) const;
    
    // Get extension point description
    std::string getExtensionPointDescription(const std::string& name) const;
    
    // Get event description
    std::string getEventDescription(const std::string& eventName) const;
    
private:
    ExtensionSystem() = default;
    ExtensionSystem(const ExtensionSystem&) = delete;
    ExtensionSystem& operator=(const ExtensionSystem&) = delete;
    
    // Internal methods
    template<typename EventData>
    void emitTypedEvent(const Event<std::any>& event);
    
    mutable std::mutex mutex_;
    
    // Configuration
    Config config_;
    
    // Extension manager
    std::shared_ptr<ExtensionManager> extensionManager_;
    
    // Global configuration and brain reference
    std::shared_ptr<Config> globalConfig_;
    std::shared_ptr<Brain> brain_;
    
    // Extension points
    std::unordered_map<std::string, std::vector<std::string>> extensionPoints_;
    std::unordered_map<std::string, std::vector<std::pair<ExtensionPointHandler, std::string, int>>> extensionPointHandlers_;
    
    // Event system
    std::vector<std::unique_ptr<EventListener>> eventListeners_;
    std::unordered_map<std::string, std::string> eventDescriptions_;
    std::vector<Event<std::any>> eventHistory_;
    
    // Statistics
    size_t extensionPointsExecuted_ = 0;
    size_t eventsEmitted_ = 0;
};

} // namespace nlm
