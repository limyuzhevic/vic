#include "ExtensionSystem.hpp"
#include "../ExtensionManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

namespace {
    // Helper function to normalize path
    std::string normalizePath(const std::string& path) {
        std::filesystem::path fsPath(path);
        return fsPath.lexically_normal().string();
    }
    
    // Check if directory exists
    bool directoryExists(const std::string& path) {
        std::filesystem::path fsPath(normalizePath(path));
        return std::filesystem::is_directory(fsPath);
    }
    
    // Get all files in directory
    std::vector<std::string> getFilesInDirectory(const std::string& path) {
        std::vector<std::string> files;
        std::filesystem::path fsPath(normalizePath(path));
        
        if (!std::filesystem::exists(fsPath)) {
            return files;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(fsPath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
        
        return files;
    }
}

bool ExtensionSystem::initialize(const Config& config, std::shared_ptr<Config> globalConfig, std::shared_ptr<Brain> brain) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    config_ = config;
    globalConfig_ = globalConfig;
    brain_ = brain;
    
    // Initialize extension manager
    extensionManager_ = std::make_shared<ExtensionManager>();
    
    NLM_LOG_INFO("Initialized ExtensionSystem");
    
    // Register default extension points
    addExtensionPoint("brain_start", "Called when brain starts");
    addExtensionPoint("brain_step", "Called on each brain step", {"step", "time"});
    addExtensionPoint("brain_stop", "Called when brain stops");
    addExtensionPoint("neuron_added", "Called when a neuron is added", {"neuron_id", "region"});
    addExtensionPoint("neuron_spike", "Called when a neuron spikes", {"neuron_id", "time"});
    addExtensionPoint("synapse_formed", "Called when a synapse is formed", {"synapse_id", "pre_id", "post_id"});
    addExtensionPoint("plasticity_applied", "Called when plasticity is applied", {"weight_change", "location"});
    
    // Register default events
    addExtensionPoint("event_brain_initialized", "Brain initialization event");
    addExtensionPoint("event_brain_reset", "Brain reset event");
    addExtensionPoint("event_memory_updated", "Memory update event");
    addExtensionPoint("event_prediction_computed", "Prediction computation event");
    addExtensionPoint("event_cognition_executed", "Cognition execution event");
    addExtensionPoint("event_neuromodulation_updated", "Neuromodulation update event");
    
    // Auto-discover extensions if enabled
    if (config_.autoDiscoverExtensions && directoryExists(config_.extensionPath)) {
        loadExtensionsFromPath(config_.extensionPath);
    }
    
    NLM_LOG_INFO("ExtensionSystem initialization complete");
    return true;
}

void ExtensionSystem::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Shutdown extension manager
    if (extensionManager_) {
        extensionManager_->clearAllExtensions();
        extensionManager_.reset();
    }
    
    // Clear listeners
    eventListeners_.clear();
    
    NLM_LOG_INFO("Shutdown ExtensionSystem");
}

bool ExtensionSystem::addExtensionPoint(const std::string& name, const std::string& description,
                                       const std::vector<std::string>& parameterTypes) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (hasExtensionPoint(name)) {
        NLM_LOG_WARNING("Extension point already exists: " + name);
        return false;
    }
    
    extensionPoints_[name] = parameterTypes;
    NLM_LOG_INFO("Added extension point: " + name);
    
    return true;
}

bool ExtensionSystem::removeExtensionPoint(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!hasExtensionPoint(name)) {
        return false;
    }
    
    // Remove handlers for this extension point
    extensionPoints_.erase(name);
    extensionPointHandlers_.erase(name);
    
    NLM_LOG_INFO("Removed extension point: " + name);
    
    return true;
}

bool ExtensionSystem::registerExtensionPointHandler(const std::string& pointName,
                                                   ExtensionPointHandler handler,
                                                   const std::string& handlerName,
                                                   int priority) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!hasExtensionPoint(pointName)) {
        NLM_LOG_ERROR("Extension point not found: " + pointName);
        return false;
    }
    
    // Check if handler already registered with this name
    auto it = extensionPointHandlers_.find(pointName);
    if (it != extensionPointHandlers_.end()) {
        for (const auto& pair : it->second) {
            if (pair.second == handlerName) {
                NLM_LOG_WARNING("Handler already registered: " + handlerName + " for point: " + pointName);
                return false;
            }
        }
    }
    
    extensionPointHandlers_[pointName].push_back({handler, handlerName, priority});
    
    // Sort by priority (higher priority first)
    auto& handlers = extensionPointHandlers_[pointName];
    std::sort(handlers.begin(), handlers.end(),
              [](const auto& a, const auto& b) { return a.priority > b.priority; });
    
    NLM_LOG_INFO("Registered extension point handler: " + handlerName + " for point: " + pointName);
    
    return true;
}

bool ExtensionSystem::removeExtensionPointHandler(const std::string& pointName, const std::string& handlerName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = extensionPointHandlers_.find(pointName);
    if (it == extensionPointHandlers_.end()) {
        return false;
    }
    
    auto& handlers = it->second;
    auto handlerIt = std::find_if(handlers.begin(), handlers.end(),
                                 [&handlerName](const auto& pair) { return pair.second == handlerName; });
    
    if (handlerIt != handlers.end()) {
        handlers.erase(handlerIt);
        NLM_LOG_INFO("Removed extension point handler: " + handlerName + " for point: " + pointName);
        return true;
    }
    
    return false;
}

std::vector<bool> ExtensionSystem::executeExtensionPoint(const std::string& pointName,
                                                       const std::any& data,
                                                       const std::unordered_map<std::string, std::any>& parameters) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!hasExtensionPoint(pointName)) {
        NLM_LOG_ERROR("Extension point not found: " + pointName);
        return {};
    }
    
    auto it = extensionPointHandlers_.find(pointName);
    if (it == extensionPointHandlers_.end() || it->second.empty()) {
        return {};
    }
    
    std::vector<bool> results;
    results.reserve(it->second.size());
    
    for (const auto& pair : it->second) {
        const auto& handler = pair.first;
        try {
            bool result = handler(pointName, data, parameters);
            results.push_back(result);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in extension point handler " + pair.second + ": " + e.what());
            results.push_back(false);
        }
    }
    
    extensionPointsExecuted_++;
    
    return results;
}

bool ExtensionSystem::addEventListener(std::unique_ptr<EventListener> listener) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!listener) {
        NLM_LOG_ERROR("Cannot add null event listener");
        return false;
    }
    
    eventListeners_.push_back(std::move(listener));
    
    NLM_LOG_INFO("Added event listener: " + eventListeners_.back()->getName());
    
    return true;
}

bool ExtensionSystem::removeEventListener(const std::string& listenerName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::remove_if(eventListeners_.begin(), eventListeners_.end(),
                             [&listenerName](const auto& listener) {
                                 return listener->getName() == listenerName;
                             });
    
    if (it != eventListeners_.end()) {
        NLM_LOG_INFO("Removed event listener: " + listenerName);
        eventListeners_.erase(it, eventListeners_.end());
        return true;
    }
    
    return false;
}

bool ExtensionSystem::emitEvent(const std::string& eventName, const std::any& data,
                               const std::string& sourceExtension) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!hasEvent(eventName)) {
        NLM_LOG_ERROR("Event not found: " + eventName);
        return false;
    }
    
    Event<std::any> event;
    event.eventName = eventName;
    event.sourceExtension = sourceExtension;
    event.data = data;
    
    emitTypedEvent(event);
    eventHistory_.push_back(event);
    
    eventsEmitted_++;
    
    NLM_LOG_INFO("Emitted event: " + eventName + " from extension: " + sourceExtension);
    
    return true;
}

std::vector<std::string> ExtensionSystem::getExtensionPoints() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(extensionPoints_.size());
    for (const auto& pair : extensionPoints_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> ExtensionSystem::getExtensionPointHandlers(const std::string& pointName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    
    auto it = extensionPointHandlers_.find(pointName);
    if (it != extensionPointHandlers_.end()) {
        names.reserve(it->second.size());
        for (const auto& pair : it->second) {
            names.push_back(pair.second);
        }
    }
    
    return names;
}

std::vector<std::string> ExtensionSystem::getEventListenerNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(eventListeners_.size());
    for (const auto& listener : eventListeners_) {
        names.push_back(listener->getName());
    }
    return names;
}

std::vector<std::string> ExtensionSystem::getEvents() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> names;
    names.reserve(eventDescriptions_.size());
    for (const auto& pair : eventDescriptions_) {
        names.push_back(pair.first);
    }
    return names;
}

std::string ExtensionSystem::getStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream status;
    status << "ExtensionSystem Status:\n";
    status << "  Extension Points: " << extensionPoints_.size() << "\n";
    status << "  Event Listeners: " << eventListeners_.size() << "\n";
    status << "  Events: " << eventsEmitted_ << " emitted\n";
    status << "  Extension Points Executed: " << extensionPointsExecuted_ << "\n";
    
    if (extensionManager_) {
        status << "  Extensions Loaded: " << extensionManager_->getExtensionCount() << "\n";
    }
    
    return status.str();
}

bool ExtensionSystem::loadExtensionsFromPath(const std::string& path) {
    if (!directoryExists(path)) {
        NLM_LOG_WARNING("Extension path does not exist: " + path);
        return false;
    }
    
    std::vector<std::string> files = getFilesInDirectory(path);
    int loadedCount = 0;
    
    for (const auto& file : files) {
        // Check if file extension suggests it's an extension
        std::string ext = getFileExtension(file);
        if (ext == "nlmext" || ext == "so" || ext == "dll") {
            std::string name = file;
            size_t lastSlash = name.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                name = name.substr(lastSlash + 1);
            }
            size_t lastDot = name.find_last_of('.');
            if (lastDot != std::string::npos) {
                name = name.substr(0, lastDot);
            }
            
            if (loadExtension(name)) {
                loadedCount++;
            }
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(loadedCount) + " extensions from path: " + path);
    
    return loadedCount > 0;
}

bool ExtensionSystem::loadExtension(const std::string& name) {
    if (!extensionManager_) {
        NLM_LOG_ERROR("Extension manager not initialized");
        return false;
    }
    
    // Check if already loaded
    if (extensionManager_->hasExtension(name)) {
        NLM_LOG_WARNING("Extension already loaded: " + name);
        return false;
    }
    
    // Try to load extension via extension manager
    // This is a simplified implementation - in production, this would use
    // dynamic loading based on the extension system configuration
    NLM_LOG_INFO("Attempting to load extension: " + name + " (simplified implementation)");
    
    // For demonstration, just return false - in a real implementation,
    // this would use the extension manager to load the extension
    return false;
}

bool ExtensionSystem::unloadExtension(const std::string& name) {
    if (!extensionManager_) {
        NLM_LOG_ERROR("Extension manager not initialized");
        return false;
    }
    
    return extensionManager_->unloadExtension(name);
}

bool ExtensionSystem::hasExtensionPoint(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return extensionPoints_.find(name) != extensionPoints_.end();
}

bool ExtensionSystem::hasEvent(const std::string& eventName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return eventDescriptions_.find(eventName) != eventDescriptions_.end();
}

std::string ExtensionSystem::getExtensionPointDescription(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = extensionPoints_.find(name);
    if (it != extensionPoints_.end()) {
        return "Extension point with parameters: " + std::to_string(it->second.size()) + " parameters";
    }
    return "";
}

std::string ExtensionSystem::getEventDescription(const std::string& eventName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = eventDescriptions_.find(eventName);
    if (it != eventDescriptions_.end()) {
        return it->second;
    }
    return "";
}

template<typename EventData>
void ExtensionSystem::emitTypedEvent(const Event<std::any>& event) {
    for (const auto& listener : eventListeners_) {
        try {
            listener->onEvent(event);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception in event listener " + listener->getName() + ": " + e.what());
        }
    }
}

} // namespace nlm
