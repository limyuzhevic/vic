#include "IPlugin.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <chrono>
#include <unordered_set>
#include <atomic>
#include <regex>

namespace nlm {

// Plugin implementation
class Plugin : public IPlugin {
public:
    Plugin(const PluginMetadata& metadata, PluginFactoryFunc factory)
        : metadata_(metadata), factory_(factory), state_(PluginState::UNLOADED) {}
    
    virtual ~Plugin() {
        if (state_ == PluginState::LOADED) {
            onUnload();
        }
    }
    
    const std::string& getName() const override {
        return metadata_.name;
    }
    
    const std::string& getVersion() const override {
        return metadata_.version;
    }
    
    const std::string& getDescription() const override {
        return metadata_.description;
    }
    
    PluginType getType() const override {
        return metadata_.type;
    }
    
    const std::string& getAuthor() const override {
        return metadata_.author;
    }
    
    const std::string& getContact() const override {
        return metadata_.contact;
    }
    
    std::vector<std::string> getDependencies() const override {
        return metadata_.dependencies;
    }
    
    std::vector<std::string> getOptionalDependencies() const override {
        return metadata_.optionalDependencies;
    }
    
    std::string getMinSystemVersion() const override {
        return metadata_.minSystemVersion;
    }
    
    PluginLoadResult onLoad() override {
        if (state_ != PluginState::UNLOADED) {
            return PluginLoadResult::ALREADY_LOADED;
        }
        
        state_ = PluginState::LOADING;
        
        try {
            instance_ = factory_();
            if (!instance_) {
                return PluginLoadResult::FAILED;
            }
            
            // Apply configuration
            instance_->configure(metadata_.configuration);
            
            // Call plugin-specific load logic
            loadResult = instance_->onLoad();
            
            if (loadResult == PluginLoadResult::SUCCESS) {
                state_ = PluginState::LOADED;
                metadata_.state = state_;
                
                // Record load time
                auto now = std::chrono::high_resolution_clock::now();
                metadata_.loadTime = std::chrono::duration<double>(now - loadStartTime_).count();
                
                return PluginLoadResult::SUCCESS;
            } else {
                state_ = PluginState::ERROR;
                return loadResult;
            }
        } catch (const std::exception& e) {
            errorMessage_ = e.what();
            state_ = PluginState::ERROR;
            return PluginLoadResult::FAILED;
        }
    }
    
    void onUnload() override {
        if (state_ == PluginState::LOADED && instance_) {
            instance_->onUnload();
            instance_.reset();
        }
        state_ = PluginState::UNLOADED;
        metadata_.state = state_;
    }
    
    void onInitialize() override {
        if (instance_ && state_ == PluginState::LOADED) {
            instance_->onInitialize();
        }
    }
    
    void onShutdown() override {
        if (instance_ && state_ == PluginState::LOADED) {
            instance_->onShutdown();
        }
    }
    
    PluginState getState() const override {
        return state_;
    }
    
    void configure(const std::unordered_map<std::string, std::string>& config) override {
        metadata_.configuration = config;
        
        // Validate configuration
        if (instance_) {
            instance_->configure(config);
        }
    }
    
    bool validateConfiguration(const std::unordered_map<std::string, std::string>& config) const override {
        if (instance_) {
            return instance_->validateConfiguration(config);
        }
        return true; // Built-in plugins always valid
    }
    
    bool isHealthy() const override {
        if (state_ != PluginState::LOADED || !instance_) {
            return false;
        }
        return instance_->isHealthy();
    }
    
    std::string getHealthStatus() const override {
        if (state_ != PluginState::LOADED || !instance_) {
            return "Plugin not loaded or instance not created";
        }
        return instance_->getHealthStatus();
    }
    
    std::unordered_map<std::string, double> getMetrics() const override {
        std::unordered_map<std::string, double> metrics;
        
        // Add plugin-level metrics
        metrics["load_time"] = metadata_.loadTime;
        metrics["state"] = static_cast<double>(static_cast<int>(state_));
        
        // Add component-level metrics
        if (instance_) {
            auto componentMetrics = instance_->getMetrics();
            metrics.insert(componentMetrics.begin(), componentMetrics.end());
        }
        
        return metrics;
    }
    
    void onEvent(const std::string& eventName, const std::unordered_map<std::string, std::string>& parameters) override {
        if (instance_) {
            instance_->onEvent(eventName, parameters);
        }
    }
    
    // Internal methods for plugin manager
    void setLoadStartTime() {
        loadStartTime_ = std::chrono::high_resolution_clock::now();
    }
    
    std::unique_ptr<IPlugin>& getInstance() {
        return instance_;
    }
    
    const PluginMetadata& getMetadata() const {
        return metadata_;
    }
    
    std::string getErrorMessage() const {
        return errorMessage_;
    }
    
private:
    PluginMetadata metadata_;
    PluginFactoryFunc factory_;
    PluginState state_;
    std::unique_ptr<IPlugin> instance_;
    std::chrono::high_resolution_clock::time_point loadStartTime_;
    std::string errorMessage_;
    PluginLoadResult loadResult_ = PluginLoadResult::SUCCESS;
};

// Component implementation
class Component : public IComponent {
public:
    Component(const ComponentInfo& info) : info_(info), initialized_(false) {}
    
    const std::string& getName() const override {
        return info_.name;
    }
    
    const std::string& getCategory() const override {
        return info_.category;
    }
    
    std::type_index getComponentType() const override {
        return info_.type;
    }
    
    bool initialize() override {
        if (initialized_) {
            return true;
        }
        
        try {
            instance_ = info_.factory();
            if (!instance_) {
                return false;
            }
            
            // Initialize component
            if (instance_->initialize()) {
                initialized_ = true;
                return true;
            }
            
            return false;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    void shutdown() override {
        if (instance_) {
            instance_->shutdown();
            instance_.reset();
        }
        initialized_ = false;
    }
    
    bool isInitialized() const override {
        return initialized_;
    }
    
    void configure(const std::unordered_map<std::string, std::string>& config) override {
        if (instance_) {
            instance_->configure(config);
        }
        currentConfig_ = config;
    }
    
    bool validateConfiguration(const std::unordered_map<std::string, std::string>& config) const override {
        if (instance_) {
            return instance_->validateConfiguration(config);
        }
        return true;
    }
    
    std::unordered_map<std::string, std::string> getDefaultConfiguration() const override {
        return info_.defaultConfig;
    }
    
    std::unordered_map<std::string, double> getMetrics() const override {
        std::unordered_map<std::string, double> metrics;
        
        metrics["initialized"] = initialized_ ? 1.0 : 0.0;
        metrics["is_builtin"] = info_.isBuiltIn ? 1.0 : 0.0;
        
        if (instance_) {
            auto componentMetrics = instance_->getMetrics();
            metrics.insert(componentMetrics.begin(), componentMetrics.end());
        }
        
        return metrics;
    }
    
    bool isHealthy() const override {
        if (!initialized_ || !instance_) {
            return false;
        }
        return instance_->isHealthy();
    }
    
    std::string getHealthStatus() const override {
        if (!initialized_ || !instance_) {
            return "Component not initialized or instance not created";
        }
        return instance_->getHealthStatus();
    }
    
    // Internal methods
    std::unique_ptr<IComponent>& getInstance() {
        return instance_;
    }
    
    const ComponentInfo& getInfo() const {
        return info_;
    }
    
private:
    ComponentInfo info_;
    std::unique_ptr<IComponent> instance_;
    bool initialized_;
    std::unordered_map<std::string, std::string> currentConfig_;
};

// Default plugin loader implementation
class DefaultPluginLoader : public IPluginLoader {
public:
    DefaultPluginLoader(std::shared_ptr<IPluginRegistry> registry)
        : registry_(registry) {}
    
    PluginLoadResult loadPlugin(const std::string& filePath, const PluginLoadOptions& options) override {
        try {
            // Check if plugin is already loaded
            if (options.skipAlreadyLoaded && isPluginLoaded(getPluginNameFromPath(filePath))) {
                return PluginLoadResult::ALREADY_LOADED;
            }
            
            // Load plugin metadata
            PluginMetadata metadata = loadPluginMetadata(filePath);
            if (metadata.name.empty()) {
                return PluginLoadResult::FAILED;
            }
            
            // Check dependencies
            if (options.validateDependencies) {
                PluginLoadResult depResult = checkDependencies(metadata.dependencies);
                if (depResult != PluginLoadResult::SUCCESS) {
                    return depResult;
                }
            }
            
            // Create plugin instance
            PluginFactoryFunc factory = createPluginFactory(filePath, metadata.className);
            if (!factory) {
                return PluginLoadResult::FAILED;
            }
            
            // Create and load plugin
            auto plugin = std::make_shared<Plugin>(metadata, factory);
            plugin->setLoadStartTime();
            
            PluginLoadResult loadResult = plugin->onLoad();
            if (loadResult == PluginLoadResult::SUCCESS) {
                // Add to loaded plugins
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    loadedPlugins_[metadata.name] = plugin;
                    plugin->configure(options.pluginConfiguration);
                    
                    if (options.autoInitialize) {
                        plugin->onInitialize();
                    }
                }
                
                // Add to registry
                registry_->registerBuiltInPlugin(metadata, factory);
                
                // Fire event
                firePluginEvent(PluginEventType::PLUGIN_LOADED, metadata.name);
                
                return PluginLoadResult::SUCCESS;
            } else {
                return loadResult;
            }
        } catch (const std::exception& e) {
            errorMessage_ = e.what();
            return PluginLoadResult::FAILED;
        }
    }
    
    bool unloadPlugin(const std::string& pluginName) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = loadedPlugins_.find(pluginName);
        if (it == loadedPlugins_.end()) {
            return false;
        }
        
        auto plugin = it->second;
        
        // Call plugin unload
        plugin->onUnload();
        
        // Remove from loaded plugins
        loadedPlugins_.erase(it);
        
        // Remove from registry
        registry_->getPluginMetadata(pluginName); // This will remove the plugin from registry
        
        // Fire event
        firePluginEvent(PluginEventType::PLUGIN_UNLOADED, pluginName);
        
        return true;
    }
    
    std::optional<PluginInfo> getPluginInfo(const std::string& pluginName) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = loadedPlugins_.find(pluginName);
        if (it == loadedPlugins_.end()) {
            return std::nullopt;
        }
        
        auto& plugin = it->second;
        PluginInfo info;
        
        const auto& metadata = plugin->getMetadata();
        info.name = metadata.name;
        info.version = metadata.version;
        info.description = metadata.description;
        info.type = metadata.type;
        info.author = metadata.author;
        info.contact = metadata.contact;
        info.dependencies = metadata.dependencies;
        info.optionalDependencies = metadata.optionalDependencies;
        info.minSystemVersion = metadata.minSystemVersion;
        info.state = metadata.state;
        info.configuration = metadata.configuration;
        info.loadTime = metadata.loadTime;
        info.metrics = plugin->getMetrics();
        
        return info;
    }
    
    bool isPluginLoaded(const std::string& pluginName) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return loadedPlugins_.find(pluginName) != loadedPlugins_.end();
    }
    
    std::vector<std::string> getLoadedPlugins() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> plugins;
        plugins.reserve(loadedPlugins_.size());
        for (const auto& pair : loadedPlugins_) {
            plugins.push_back(pair.first);
        }
        return plugins;
    }
    
    std::vector<ComponentInfo> getAllComponents() const override {
        return registry_->getAllComponents();
    }
    
    std::optional<ComponentInfo> getComponent(const std::string& componentName) const override {
        return registry_->getComponentInfo(componentName);
    }
    
    std::unique_ptr<IComponent> createComponent(const std::string& componentName, const std::string& config) override {
        auto componentInfo = getComponent(componentName);
        if (!componentInfo) {
            return nullptr;
        }
        
        std::unique_ptr<Component> component = std::make_unique<Component>(*componentInfo);
        component->configure(parseConfigString(config));
        
        if (!component->initialize()) {
            return nullptr;
        }
        
        return std::move(component->getInstance());
    }
    
    // Internal methods
    std::string getPluginNameFromPath(const std::string& filePath) {
        std::filesystem::path path(filePath);
        return path.stem().string();
    }
    
    PluginMetadata loadPluginMetadata(const std::string& filePath) {
        PluginMetadata metadata;
        
        // Try to load metadata from JSON file
        std::string metadataPath = filePath + ".metadata";
        if (std::filesystem::exists(metadataPath)) {
            // Load metadata from JSON file
            // For simplicity, return default metadata
            metadata.name = getPluginNameFromPath(filePath);
            metadata.version = "1.0.0";
            metadata.type = PluginType::COMPONENT;
            metadata.description = "Plugin loaded from " + filePath;
            metadata.author = "Unknown";
            metadata.contact = "";
            metadata.minSystemVersion = "1.0.0";
            metadata.state = PluginState::UNLOADED;
            metadata.isBuiltIn = false;
        } else {
            // Default metadata for file-based plugins
            metadata.name = getPluginNameFromPath(filePath);
            metadata.version = "1.0.0";
            metadata.type = PluginType::COMPONENT;
            metadata.description = "Plugin loaded from " + filePath;
            metadata.author = "Unknown";
            metadata.contact = "";
            metadata.minSystemVersion = "1.0.0";
            metadata.state = PluginState::UNLOADED;
            metadata.isBuiltIn = false;
        }
        
        return metadata;
    }
    
    PluginLoadResult checkDependencies(const std::vector<std::string>& dependencies) {
        for (const auto& dep : dependencies) {
            if (!isPluginLoaded(dep)) {
                return PluginLoadResult::DEPENDENCY_ERROR;
            }
        }
        return PluginLoadResult::SUCCESS;
    }
    
    PluginFactoryFunc createPluginFactory(const std::string& filePath, const std::string& className) {
        // For simplicity, return a lambda that creates a simple test plugin
        // In a real implementation, this would dynamically load the plugin class
        return [filePath]() -> std::unique_ptr<IPlugin> {
            // Create a simple test plugin
            PluginMetadata metadata;
            metadata.name = "test_plugin";
            metadata.version = "1.0.0";
            metadata.type = PluginType::COMPONENT;
            metadata.description = "Test plugin created dynamically";
            metadata.author = "NLM";
            metadata.contact = "";
            metadata.minSystemVersion = "1.0.0";
            metadata.state = PluginState::UNLOADED;
            metadata.isBuiltIn = false;
            
            return std::make_unique<Plugin>(metadata, []( ) -> std::unique_ptr<IPlugin> {
                return nullptr; // Would create actual plugin instance
            });
        };
    }
    
    std::unordered_map<std::string, std::string> parseConfigString(const std::string& config) {
        std::unordered_map<std::string, std::string> parsedConfig;
        
        // Simple key=value parsing
        std::regex kvPattern(R"((\w+)\s*=\s*([^;]+))");
        std::string::const_iterator begin(config.begin()), end(config.end());
        std::regex_search(begin, end, std::match_results<decltype(begin)>(), kvPattern);
        
        return parsedConfig;
    }
    
    void firePluginEvent(PluginEventType eventType, const std::string& pluginName) {
        PluginEvent event;
        event.type = eventType;
        event.pluginName = pluginName;
        event.timestamp = std::chrono::system_clock::now();
        
        // Notify all registered callbacks
        // In a real implementation, this would call all registered callbacks
    }
    
private:
    std::shared_ptr<IPluginRegistry> registry_;
    std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<Plugin>> loadedPlugins_;
    std::string errorMessage_;
};

// Default plugin registry implementation
class DefaultPluginRegistry : public IPluginRegistry {
public:
    DefaultPluginRegistry() {
        // Register built-in components
        registerBuiltInComponents();
    }
    
    bool registerBuiltInPlugin(const PluginMetadata& metadata, PluginFactoryFunc factory) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (registeredPlugins_.find(metadata.name) != registeredPlugins_.end()) {
            return false; // Already registered
        }
        
        registeredPlugins_[metadata.name] = metadata;
        pluginFactories_[metadata.name] = factory;
        
        // Fire event
        firePluginEvent(PluginEventType::PLUGIN_LOADED, metadata.name);
        
        return true;
    }
    
    bool registerBuiltInComponent(const ComponentInfo& info) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (registeredComponents_.find(info.name) != registeredComponents_.end()) {
            return false; // Already registered
        }
        
        registeredComponents_[info.name] = info;
        
        // Fire event
        firePluginEvent(PluginEventType::COMPONENT_REGISTERED, info.name);
        
        return true;
    }
    
    bool discoverAndRegisterPlugin(const std::string& directory) override {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    std::string filePath = entry.path().string();
                    
                    // Check if file has plugin extension
                    std::string extension = entry.path().extension().string();
                    if (extension == ".plugin" || extension == ".dll" || extension == ".so") {
                        // Try to load metadata
                        PluginMetadata metadata;
                        // For simplicity, skip actual plugin discovery
                        // In a real implementation, this would parse plugin metadata
                    }
                }
            }
        } catch (const std::exception&) {
            // Directory doesn't exist or other error
            return false;
        }
        
        return true;
    }
    
    std::vector<PluginMetadata> getAllPlugins() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<PluginMetadata> plugins;
        plugins.reserve(registeredPlugins_.size());
        
        for (const auto& pair : registeredPlugins_) {
            plugins.push_back(pair.second);
        }
        
        return plugins;
    }
    
    std::vector<ComponentInfo> getAllComponents() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<ComponentInfo> components;
        components.reserve(registeredComponents_.size());
        
        for (const auto& pair : registeredComponents_) {
            components.push_back(pair.second);
        }
        
        return components;
    }
    
    std::optional<PluginMetadata> getPluginMetadata(const std::string& pluginName) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = registeredPlugins_.find(pluginName);
        if (it == registeredPlugins_.end()) {
            return std::nullopt;
        }
        
        return it->second;
    }
    
    std::optional<ComponentInfo> getComponentInfo(const std::string& componentName) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = registeredComponents_.find(componentName);
        if (it == registeredComponents_.end()) {
            return std::nullopt;
        }
        
        return it->second;
    }
    
    bool isPluginRegistered(const std::string& pluginName) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return registeredPlugins_.find(pluginName) != registeredPlugins_.end();
    }
    
    bool isComponentRegistered(const std::string& componentName) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return registeredComponents_.find(componentName) != registeredComponents_.end();
    }
    
    void addEventCallback(PluginEventCallback callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        eventCallbacks_.push_back(callback);
    }
    
    void removeEventCallback(PluginEventCallback callback) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = std::find(eventCallbacks_.begin(), eventCallbacks_.end(), callback);
        if (it != eventCallbacks_.end()) {
            eventCallbacks_.erase(it);
        }
    }
    
    PluginSystemStats getSystemStats() const override {
        PluginSystemStats stats;
        
        stats.totalPlugins = registeredPlugins_.size();
        stats.loadedPlugins = loadedPlugins_.size();
        stats.failedPlugins = 0; // Would need to track this
        stats.totalLoadTime = 0.0; // Would need to track this
        stats.activeComponents = 0;
        stats.totalComponents = registeredComponents_.size();
        
        // Calculate metrics
        for (const auto& pair : registeredComponents_) {
            auto metrics = pair.second.factory ? pair.second.factory()->getMetrics() : std::unordered_map<std::string, double>{};
            stats.componentMetrics.insert(metrics.begin(), metrics.end());
        }
        
        return stats;
    }
    
    // Internal methods
    void firePluginEvent(PluginEventType eventType, const std::string& pluginName) {
        PluginEvent event;
        event.type = eventType;
        event.pluginName = pluginName;
        event.timestamp = std::chrono::system_clock::now();
        
        for (const auto& callback : eventCallbacks_) {
            callback(event);
        }
    }
    
    void registerBuiltInComponents() {
        // Register built-in components for the NLM system
        // This includes all the core components from the existing codebase
        
        // Example: Register Neuron component
        ComponentInfo neuronInfo;
        neuronInfo.name = "neuron";
        neuronInfo.category = "brain";
        neuronInfo.type = std::type_index(typeid(class Neuron));
        neuronInfo.isBuiltIn = true;
        
        // In a real implementation, this would create a factory function
        // that creates Neuron instances
        registerBuiltInComponent(neuronInfo);
    }
    
private:
    std::mutex mutex_;
    std::unordered_map<std::string, PluginMetadata> registeredPlugins_;
    std::unordered_map<std::string, ComponentInfo> registeredComponents_;
    std::vector<PluginEventCallback> eventCallbacks_;
    std::unordered_map<std::string, std::shared_ptr<Plugin>> loadedPlugins_;
};

// Default plugin manager implementation
class DefaultPluginManager : public IPluginManager {
public:
    DefaultPluginManager() {
        registry_ = std::make_shared<DefaultPluginRegistry>();
        loader_ = std::make_shared<DefaultPluginLoader>(registry_);
    }
    
    PluginLoadResult loadPlugin(const std::string& filePath, const PluginLoadOptions& options) override {
        return loader_->loadPlugin(filePath, options);
    }
    
    bool unloadPlugin(const std::string& pluginName) override {
        return loader_->unloadPlugin(pluginName);
    }
    
    bool initializeAll() override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        bool allInitialized = true;
        for (const auto& pair : loader_->getLoadedPlugins()) {
            auto pluginInfo = loader_->getPluginInfo(pair);
            if (pluginInfo && pluginInfo->state == PluginState::LOADED) {
                if (!initializePlugin(pair)) {
                    allInitialized = false;
                }
            }
        }
        
        return allInitialized;
    }
    
    void shutdownAll() override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& pair : loader_->getLoadedPlugins()) {
            unloadPlugin(pair);
        }
    }
    
    PluginState getPluginState(const std::string& pluginName) const override {
        auto pluginInfo = loader_->getPluginInfo(pluginName);
        return pluginInfo ? pluginInfo->state : PluginState::UNLOADED;
    }
    
    std::optional<PluginInfo> getPluginInfo(const std::string& pluginName) const override {
        return loader_->getPluginInfo(pluginName);
    }
    
    std::vector<std::string> getLoadedPlugins() const override {
        return loader_->getLoadedPlugins();
    }
    
    std::unique_ptr<IComponent> createComponent(const std::string& componentName, const std::string& config) override {
        return loader_->createComponent(componentName, config);
    }
    
    std::unique_ptr<IComponent> createComponent(const std::type_index& type, const std::string& config) override {
        auto componentInfo = loader_->getComponent(type);
        if (!componentInfo) {
            return nullptr;
        }
        
        return createComponent(componentInfo->name, config);
    }
    
    std::optional<ComponentInfo> getComponentInfo(const std::string& componentName) const override {
        return loader_->getComponent(componentName);
    }
    
    std::vector<ComponentInfo> getAvailableComponents() const override {
        return loader_->getAllComponents();
    }
    
    PluginSystemStats getSystemStats() const override {
        return registry_->getSystemStats();
    }
    
    bool saveConfiguration(const std::string& filePath) const override {
        // Save plugin configuration to file
        // In a real implementation, this would serialize plugin metadata and configuration
        return true;
    }
    
    bool loadConfiguration(const std::string& filePath) override {
        // Load plugin configuration from file
        // In a real implementation, this would deserialize plugin metadata and configuration
        return true;
    }
    
    // Internal methods
    bool initializePlugin(const std::string& pluginName) {
        auto pluginInfo = loader_->getPluginInfo(pluginName);
        if (!pluginInfo || pluginInfo->state != PluginState::LOADED) {
            return false;
        }
        
        // In a real implementation, this would call plugin initialization
        return true;
    }
    
private:
    std::shared_ptr<IPluginRegistry> registry_;
    std::shared_ptr<IPluginLoader> loader_;
    std::mutex mutex_;
};

// Global plugin manager instance
std::shared_ptr<IPluginManager> globalPluginManager_ = nullptr;

// Plugin system initialization
void initializePluginSystem() {
    if (!globalPluginManager_) {
        globalPluginManager_ = std::make_shared<DefaultPluginManager>();
    }
}

// Get global plugin manager
IPluginManager& getPluginManager() {
    if (!globalPluginManager_) {
        initializePluginSystem();
    }
    return *globalPluginManager_;
}

// Create plugin manager
IPluginManager& createPluginManager() {
    return getPluginManager();
}

} // namespace nlm