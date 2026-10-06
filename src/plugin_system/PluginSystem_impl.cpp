#pragma once

#include "PluginSystem_impl.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include <dlfcn.h>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace nlm {

class PluginSystem::Impl {
public:
    explicit Impl(std::shared_ptr<Config> config)
        : config_(config)
        , brain_(nullptr)
        , initialized_(false) {
        
        // Create plugin loader instance
        pluginLoader_ = std::make_unique<PluginLoader>();
        serviceRegistry_ = std::make_unique<ServiceRegistry>();
        executionEngine_ = std::make_unique<ExecutionEngine>();
    }
    
    ~Impl() {
        shutdown();
    }
    
    bool initialize() {
        if (initialized_) {
            return true;
        }
        
        initialized_ = true;
        NLM_LOG_INFO("PluginSystem initialized");
        return true;
    }
    
    void shutdown() {
        if (!initialized_) {
            return;
        }
        
        // Shutdown all plugins
        for (const auto& pluginName : enabledPlugins_) {
            disablePlugin(pluginName);
        }
        
        // Unload all plugins
        for (const auto& pluginName : loadedPlugins_) {
            unloadPlugin(pluginName);
        }
        
        // Clear registries
        serviceRegistry_->clear();
        pluginLoader_->clearCache();
        
        initialized_ = false;
        NLM_LOG_INFO("PluginSystem shutdown");
    }
    
    bool loadPlugin(const std::string& pluginPath) {
        if (!initialized_) {
            NLM_LOG_ERROR("PluginSystem not initialized");
            return false;
        }
        
        std::string pluginName = extractPluginName(pluginPath);
        if (loadedPlugins_.count(pluginName) > 0) {
            NLM_LOG_WARNING(std::string("Plugin already loaded: ") + pluginName);
            return true;
        }
        
        // Validate plugin signature
        if (!pluginLoader_->validatePluginSignature(pluginPath)) {
            NLM_LOG_ERROR(std::string("Plugin signature validation failed: ") + pluginPath);
            return false;
        }
        
        // Load plugin library
        void* handle = nullptr;
        if (!pluginLoader_->loadPluginLibrary(pluginPath, handle)) {
            NLM_LOG_ERROR(std::string("Failed to load plugin library: ") + pluginPath);
            return false;
        }
        
        // Get create plugin function
        using CreatePluginFunc = std::unique_ptr<Plugin>(*)();
        auto createPlugin = reinterpret_cast<CreatePluginFunc>(
            pluginLoader_->getPluginSymbol(handle, "createPlugin")
        );
        
        if (!createPlugin) {
            pluginLoader_->closePluginLibrary(handle);
            NLM_LOG_ERROR(std::string("Could not find createPlugin function in: ") + pluginPath);
            return false;
        }
        
        // Create plugin instance
        auto plugin = createPlugin();
        if (!plugin) {
            pluginLoader_->closePluginLibrary(handle);
            NLM_LOG_ERROR(std::string("Failed to create plugin instance from: ") + pluginPath);
            return false;
        }
        
        // Get plugin metadata
        const PluginMetadata& metadata = plugin->getMetadata();
        pluginName = metadata.name;
        
        // Store metadata
        pluginMetadata_[pluginName] = metadata;
        
        // Initialize plugin
        if (!plugin->initialize()) {
            pluginLoader_->closePluginLibrary(handle);
            NLM_LOG_ERROR(std::string("Plugin initialization failed: ") + metadata.name);
            return false;
        }
        
        // Get provided services
        auto services = plugin->getProvidedServices();
        for (const auto& serviceInfo : services) {
            if (serviceInfo.isSingleton) {
                // Check if service with same name already exists
                if (serviceRegistry_->hasService(serviceInfo.name)) {
                    NLM_LOG_ERROR(std::string("Service already registered: ") + serviceInfo.name);
                    continue;
                }
            }
            
            // Create service wrapper
            auto serviceImpl = std::make_unique<ServiceImpl>(createService(serviceInfo, handle));
            if (!serviceImpl->initialize()) {
                NLM_LOG_ERROR(std::string("Service initialization failed: ") + serviceInfo.name);
                continue;
            }
            
            // Register service
            serviceRegistry_->registerService(serviceInfo.name, std::move(serviceImpl), pluginName);
        }
        
        // Store plugin
        auto pluginImpl = std::make_unique<PluginImpl>(std::move(plugin));
        if (!pluginImpl->initialize()) {
            NLM_LOG_ERROR(std::string("PluginImpl initialization failed: ") + metadata.name);
            pluginLoader_->closePluginLibrary(handle);
            return false;
        }
        
        loadedPlugins_[pluginName] = std::move(pluginImpl);
        pluginHandles_[pluginName] = handle;
        
        // Update state
        updatePluginState(pluginName, PluginState::Loaded);
        
        // Auto-enable if configured
        if (metadata.autoEnable) {
            enablePlugin(pluginName);
        }
        
        NLM_LOG_INFO(std::string("Plugin loaded: ") + metadata.name + " v" + metadata.version);
        return true;
    }
    
    bool unloadPlugin(const std::string& pluginName) {
        if (loadedPlugins_.count(pluginName) == 0) {
            NLM_LOG_WARNING(std::string("Plugin not loaded: ") + pluginName);
            return true;
        }
        
        // Disable if enabled
        if (enabledPlugins_.count(pluginName) > 0) {
            disablePlugin(pluginName);
        }
        
        // Shutdown plugin
        auto& pluginImpl = loadedPlugins_[pluginName];
        pluginImpl->shutdown();
        
        // Unregister services
        serviceRegistry_->unregisterServicesByPlugin(pluginName);
        
        // Close library
        if (pluginHandles_.count(pluginName) > 0) {
            pluginLoader_->closePluginLibrary(pluginHandles_[pluginName]);
            pluginHandles_.erase(pluginName);
        }
        
        // Remove from loaded plugins
        loadedPlugins_.erase(pluginName);
        
        // Update metadata
        pluginMetadata_.erase(pluginName);
        
        // Update state
        updatePluginState(pluginName, PluginState::Unloaded);
        
        NLM_LOG_INFO(std::string("Plugin unloaded: ") + pluginName);
        return true;
    }
    
    bool enablePlugin(const std::string& pluginName) {
        if (loadedPlugins_.count(pluginName) == 0) {
            NLM_LOG_ERROR(std::string("Plugin not loaded: ") + pluginName);
            return false;
        }
        
        if (enabledPlugins_.count(pluginName) > 0) {
            NLM_LOG_WARNING(std::string("Plugin already enabled: ") + pluginName);
            return true;
        }
        
        auto& pluginImpl = loadedPlugins_[pluginName];
        
        // Check dependencies
        const auto& metadata = pluginMetadata_[pluginName];
        for (const auto& dependency : metadata.dependencies) {
            if (!isPluginEnabled(dependency)) {
                NLM_LOG_ERROR(std::string("Plugin dependency not enabled: ") + dependency);
                return false;
            }
        }
        
        // Update state
        updatePluginState(pluginName, PluginState::Enabling);
        
        // Enable plugin
        enabledPlugins_.insert(pluginName);
        
        // Call enable handler
        updatePluginState(pluginName, PluginState::Enabled);
        
        // Broadcast event
        PluginEvent event;
        event.type = PluginEventType::Enable;
        event.pluginName = pluginName;
        broadcastPluginEvent(event);
        
        NLM_LOG_INFO(std::string("Plugin enabled: ") + pluginName);
        return true;
    }
    
    bool disablePlugin(const std::string& pluginName) {
        if (loadedPlugins_.count(pluginName) == 0) {
            NLM_LOG_WARNING(std::string("Plugin not loaded: ") + pluginName);
            return true;
        }
        
        if (enabledPlugins_.count(pluginName) == 0) {
            NLM_LOG_WARNING(std::string("Plugin not enabled: ") + pluginName);
            return true;
        }
        
        // Call disable handler
        updatePluginState(pluginName, PluginState::Disabling);
        
        // Remove from enabled plugins
        enabledPlugins_.erase(pluginName);
        
        // Update state
        updatePluginState(pluginName, PluginState::Disabled);
        
        // Broadcast event
        PluginEvent event;
        event.type = PluginEventType::Disable;
        event.pluginName = pluginName;
        broadcastPluginEvent(event);
        
        NLM_LOG_INFO(std::string("Plugin disabled: ") + pluginName);
        return true;
    }
    
    bool isPluginLoaded(const std::string& pluginName) const {
        return loadedPlugins_.count(pluginName) > 0;
    }
    
    bool isPluginEnabled(const std::string& pluginName) const {
        return enabledPlugins_.count(pluginName) > 0;
    }
    
    std::vector<std::string> getLoadedPlugins() const {
        std::vector<std::string> pluginNames;
        pluginNames.reserve(loadedPlugins_.size());
        for (const auto& pair : loadedPlugins_) {
            pluginNames.push_back(pair.first);
        }
        return pluginNames;
    }
    
    std::vector<std::string> getEnabledPlugins() const {
        std::vector<std::string> pluginNames;
        pluginNames.reserve(enabledPlugins_.size());
        for (const auto& pluginName : enabledPlugins_) {
            pluginNames.push_back(pluginName);
        }
        return pluginNames;
    }
    
    std::vector<ServiceInfo> getAvailableServices() const {
        return serviceRegistry_->getAllServices();
    }
    
    template<typename T>
    std::shared_ptr<T> getService(const std::string& serviceName) {
        auto serviceImpl = serviceRegistry_->getService(serviceName);
        if (!serviceImpl) {
            throw ServiceNotFoundException(serviceName);
        }
        
        // In a real implementation, we would cast to the specific type
        // For now, return as generic service
        return std::dynamic_pointer_cast<T>(serviceImpl);
    }
    
    template<typename T>
    bool callService(const std::string& serviceName, const T& request, T& response) {
        auto serviceImpl = serviceRegistry_->getService(serviceName);
        if (!serviceImpl) {
            NLM_LOG_ERROR(std::string("Service not found: ") + serviceName);
            return false;
        }
        
        // Convert request/response types
        ServiceRequest genericRequest;
        ServiceResponse genericResponse;
        
        // In a real implementation, we would convert between types
        // For now, use generic interface
        if (serviceImpl->handleRequest(genericRequest, genericResponse)) {
            response.success = genericResponse.success;
            response.errorMessage = genericResponse.errorMessage;
            return true;
        }
        
        response.success = false;
        response.errorMessage = "Service call failed";
        return false;
    }
    
    bool executePlugin(const std::string& pluginName, const ExecutionContext& context) {
        if (loadedPlugins_.count(pluginName) == 0) {
            NLM_LOG_ERROR(std::string("Plugin not loaded: ") + pluginName);
            return false;
        }
        
        auto& pluginImpl = loadedPlugins_[pluginName];
        
        try {
            bool result = pluginImpl->execute(context);
            if (result) {
                NLM_LOG_INFO(std::string("Plugin executed: ") + pluginName + " operation: " + context.operation);
            } else {
                NLM_LOG_ERROR(std::string("Plugin execution failed: ") + pluginName + " operation: " + context.operation);
            }
            return result;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Plugin execution exception for ") + pluginName + ": " + e.what());
            return false;
        }
    }
    
    bool generatePluginTemplate(const std::string& templateType, const std::string& outputPath) {
        // In a real implementation, this would generate code templates
        NLM_LOG_INFO(std::string("Generating plugin template: ") + templateType + " at " + outputPath);
        return true;
    }
    
    void setBrainReference(Brain* brain) {
        brain_ = brain;
    }
    
    void addLoadListener(const PluginLoadListener& listener) {
        loadListeners_.push_back(listener);
    }
    
    void addEventListener(const PluginEventListener& listener) {
        eventListeners_.push_back(listener);
    }
    
    void logStatus() const {
        NLM_LOG_INFO("=== PluginSystem Status ===");
        NLM_LOG_INFO(std::string("Initialized: ") + (initialized_ ? "Yes" : "No"));
        NLM_LOG_INFO(std::string("Loaded plugins: ") + std::to_string(loadedPlugins_.size()));
        NLM_LOG_INFO(std::string("Enabled plugins: ") + std::to_string(enabledPlugins_.size()));
        NLM_LOG_INFO(std::string("Available services: ") + std::to_string(serviceRegistry_->getAllServices().size()));
        
        NLM_LOG_INFO("Loaded plugin names:");
        for (const auto& pluginName : getLoadedPlugins()) {
            NLM_LOG_INFO(std::string("  - ") + pluginName);
        }
        
        NLM_LOG_INFO("Enabled plugin names:");
        for (const auto& pluginName : getEnabledPlugins()) {
            NLM_LOG_INFO(std::string("  - ") + pluginName);
        }
    }
    
private:
    std::string extractPluginName(const std::string& pluginPath) const {
        size_t lastSlash = pluginPath.find_last_of("/");
        if (lastSlash != std::string::npos) {
            return pluginPath.substr(lastSlash + 1);
        }
        return pluginPath;
    }
    
    std::unique_ptr<Service> createService(const ServiceInfo& serviceInfo, void* handle) {
        // In a real implementation, this would create the actual service
        // For now, return a dummy implementation
        return nullptr;
    }
    
    void updatePluginState(const std::string& pluginName, PluginState newState) {
        pluginStates_[pluginName] = newState;
        
        PluginEvent event;
        event.type = PluginEventType::Custom;
        event.pluginName = pluginName;
        event.data["state"] = static_cast<int>(newState);
        
        broadcastPluginEvent(event);
    }
    
    void broadcastPluginEvent(const PluginEvent& event) {
        // Notify load listeners
        for (const auto& listener : loadListeners_) {
            listener(event);
        }
        
        // Notify event listeners
        for (const auto& listener : eventListeners_) {
            listener(event);
        }
    }
    
    // Member variables
    std::shared_ptr<Config> config_;
    Brain* brain_;
    bool initialized_;
    
    std::unique_ptr<PluginLoader> pluginLoader_;
    std::unique_ptr<ServiceRegistry> serviceRegistry_;
    std::unique_ptr<ExecutionEngine> executionEngine_;
    
    std::unordered_map<std::string, std::unique_ptr<class PluginImpl>> loadedPlugins_;
    std::unordered_set<std::string> enabledPlugins_;
    std::unordered_map<std::string, PluginMetadata> pluginMetadata_;
    std::unordered_map<std::string, PluginState> pluginStates_;
    std::unordered_map<std::string, void*> pluginHandles_;
    
    std::vector<PluginLoadListener> loadListeners_;
    std::vector<PluginEventListener> eventListeners_;
};

} // namespace nlm
