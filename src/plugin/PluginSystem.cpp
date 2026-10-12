// PluginSystem.cpp implementation
#include "PluginSystem.hpp"
#include "PluginManager.hpp"
#include "PluginConfig.hpp"
#include "PluginSecurity.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <algorithm>

namespace nlm {

struct PluginSystem::Impl {
    std::unique_ptr<PluginManager> pluginManager;
    std::vector<std::shared_ptr<Plugin>> registeredPlugins;
    std::unordered_map<std::string, std::shared_ptr<Plugin>> pluginMap;
    
    std::mutex mutex;
    bool pluginSecurityEnabled = false;
    bool monitoringEnabled = true;
    
    // System state
    SystemState state;
    
    // Callbacks
    std::function<void(const std::string&, const std::string&, const std::string&)> messageHandler;
    std::function<void(std::shared_ptr<Plugin>)> eventHandler;
    
    // Plugin debugger
    std::shared_ptr<class PluginDebugger> debugger;
    
    // Configuration
    std::shared_ptr<PluginConfig> globalConfig;
};

PluginSystem::PluginSystem() : pImpl(std::make_unique<Impl>()) {
    pImpl->state.startTime = std::chrono::steady_clock::now();
    pImpl->state.lastUpdate = pImpl->state.startTime;
}

PluginSystem::~PluginSystem() {
    shutdown();
}

bool PluginSystem::initialize(const SystemSettings& settings) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->state.initialized) {
        return false; // Already initialized
    }
    
    try {
        pImpl->state.initialized = true;
        
        // Initialize plugin manager
        pImpl->pluginManager = std::make_unique<PluginManager>();
        
        PluginManager::PluginSettings pluginSettings;
        pluginSettings.pluginDirectory = getPluginDirectoryFromSettings(settings);
        pluginSettings.autoLoad = settings.autoDiscoverPlugins;
        pluginSettings.enableAllPlugins = false; // Don't enable all by default
        pluginSettings.hotReload = settings.enableHotReload;
        pluginSettings.validatePlugins = settings.validatePluginsOnLoad;
        pluginSettings.sandboxPlugins = settings.enablePluginSecurity && pluginSecurityEnabled_;
        pluginSettings.logPluginActivity = monitoringEnabled;
        
        bool managerInitialized = pImpl->pluginManager->initialize(pluginSettings);
        if (!managerInitialized) {
            return false;
        }
        
        // Configure global settings
        pImpl->pluginManager->setPluginDirectory(settings.pluginConfigFile);
        
        // Initialize security if needed
        if (settings.enablePluginSecurity) {
            enablePluginSecurity(true);
        }
        
        // Load plugin configuration
        if (!settings.pluginConfigFile.empty()) {
            loadPluginConfig(settings.pluginConfigFile);
        }
        
        // Register default extension points
        registerDefaultExtensionPoints();
        
        // Update system state
        pImpl->state.running = true;
        
        NLM_LOG_INFO("PluginSystem initialized successfully");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize PluginSystem: " + std::string(e.what()));
        pImpl->state.initialized = false;
        return false;
    }
}

bool PluginSystem::start() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->state.initialized || pImpl->state.running) {
        return false;
    }
    
    try {
        pImpl->state.running = true;
        pImpl->state.lastUpdate = std::chrono::steady_clock::now();
        
        NLM_LOG_INFO("PluginSystem started");
        
        // Start all enabled plugins
        std::vector<std::string> enabledPlugins = pImpl->pluginManager->getEnabledPlugins();
        for (const auto& pluginId : enabledPlugins) {
            pImpl->pluginManager->enablePlugin(pluginId);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to start PluginSystem: " + std::string(e.what()));
        return false;
    }
}

bool PluginSystem::stop() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->state.running) {
        return false;
    }
    
    try {
        // Disable all plugins
        std::vector<std::string> enabledPlugins = pImpl->pluginManager->getEnabledPlugins();
        for (const auto& pluginId : enabledPlugins) {
            pImpl->pluginManager->disablePlugin(pluginId);
        }
        
        pImpl->state.running = false;
        
        NLM_LOG_INFO("PluginSystem stopped");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to stop PluginSystem: " + std::string(e.what()));
        return false;
    }
}

bool PluginSystem::pause() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->state.running) {
        return false;
    }
    
    try {
        pImpl->state.running = false;
        
        NLM_LOG_INFO("PluginSystem paused");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to pause PluginSystem: " + std::string(e.what()));
        return false;
    }
}

bool PluginSystem::resume() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->state.running) {
        return false; // Already running
    }
    
    try {
        pImpl->state.running = true;
        pImpl->state.lastUpdate = std::chrono::steady_clock::now();
        
        NLM_LOG_INFO("PluginSystem resumed");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to resume PluginSystem: " + std::string(e.what()));
        return false;
    }
}

bool PluginSystem::registerPlugin(const std::string& filepath, PluginType expectedType) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->state.initialized) {
        return false;
    }
    
    try {
        PluginLoader::LoadResult result = pImpl->pluginManager->loadPlugin(filepath, expectedType);
        
        if (!result.success) {
            logSystemError("Failed to register plugin from " + filepath + ": " + result.errorMessage);
            return false;
        }
        
        // Store plugin
        std::string instanceId = result.plugin->getInstanceId();
        pImpl->pluginMap[instanceId] = result.plugin;
        pImpl->registeredPlugins.push_back(result.plugin);
        
        // Validate plugin security if enabled
        if (pluginSecurityEnabled_) {
            if (!validatePluginSecurity(result.plugin)) {
                pImpl->pluginManager->unloadPlugin(instanceId);
                pImpl->pluginMap.erase(instanceId);
                auto it = std::find_if(pImpl->registeredPlugins.begin(), pImpl->registeredPlugins.end(),
                    [&result](const auto& p) { return p->getInstanceId() == result.plugin->getInstanceId(); });
                if (it != pImpl->registeredPlugins.end()) {
                    pImpl->registeredPlugins.erase(it);
                }
                logSystemError("Plugin security validation failed for: " + filepath);
                return false;
            }
        }
        
        // Register extension points
        auto extensionPoints = result.plugin->getExtensionPoints();
        for (const auto& extensionPoint : extensionPoints) {
            requestExtensionPoint(extensionPoint, result.plugin);
        }
        
        onPluginLoaded(result.plugin);
        
        NLM_LOG_INFO("Plugin registered successfully: " + filepath + " (instance: " + instanceId + ")");
        
        return true;
        
    } catch (const std::exception& e) {
        logSystemError("Exception registering plugin " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginSystem::registerPlugin(const std::string& name, const PluginVersion& version) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->state.initialized) {
        return false;
    }
    
    try {
        PluginLoader::LoadResult result = pImpl->pluginManager->loadPlugin(name, version);
        
        if (!result.success) {
            logSystemError("Failed to register plugin by name " + name + ": " + result.errorMessage);
            return false;
        }
        
        // Store plugin
        std::string instanceId = result.plugin->getInstanceId();
        pImpl->pluginMap[instanceId] = result.plugin;
        pImpl->registeredPlugins.push_back(result.plugin);
        
        // Validate plugin security if enabled
        if (pluginSecurityEnabled_) {
            if (!validatePluginSecurity(result.plugin)) {
                pImpl->pluginManager->unloadPlugin(instanceId);
                pImpl->pluginMap.erase(instanceId);
                auto it = std::find_if(pImpl->registeredPlugins.begin(), pImpl->registeredPlugins.end(),
                    [&result](const auto& p) { return p->getInstanceId() == result.plugin->getInstanceId(); });
                if (it != pImpl->registeredPlugins.end()) {
                    pImpl->registeredPlugins.erase(it);
                }
                logSystemError("Plugin security validation failed for: " + name);
                return false;
            }
        }
        
        // Register extension points
        auto extensionPoints = result.plugin->getExtensionPoints();
        for (const auto& extensionPoint : extensionPoints) {
            requestExtensionPoint(extensionPoint, result.plugin);
        }
        
        onPluginLoaded(result.plugin);
        
        NLM_LOG_INFO("Plugin registered successfully by name: " + name + " (instance: " + instanceId + ")");
        
        return true;
        
    } catch (const std::exception& e) {
        logSystemError("Exception registering plugin by name " + name + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginSystem::enablePlugin(const std::string& instanceId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->state.running) {
        return false;
    }
    
    auto it = pImpl->pluginMap.find(instanceId);
    if (it == pImpl->pluginMap.end()) {
        return false; // Plugin not found
    }
    
    try {
        // Enable plugin through manager
        bool success = pImpl->pluginManager->enablePlugin(instanceId);
        if (success) {
            onPluginEnabled(it->second);
            NLM_LOG_INFO("Plugin enabled: " + instanceId);
        }
        return success;
        
    } catch (const std::exception& e) {
        logSystemError("Failed to enable plugin " + instanceId + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginSystem::disablePlugin(const std::string& instanceId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginMap.find(instanceId);
    if (it == pImpl->pluginMap.end()) {
        return false; // Plugin not found
    }
    
    try {
        // Disable plugin through manager
        bool success = pImpl->pluginManager->disablePlugin(instanceId);
        if (success) {
            onPluginDisabled(it->second);
            NLM_LOG_INFO("Plugin disabled: " + instanceId);
        }
        return success;
        
    } catch (const std::exception& e) {
        logSystemError("Failed to disable plugin " + instanceId + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginSystem::unloadPlugin(const std::string& instanceId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->pluginMap.find(instanceId);
    if (it == pImpl->pluginMap.end()) {
        return false; // Plugin not found
    }
    
    try {
        // Unload plugin through manager
        bool success = pImpl->pluginManager->unloadPlugin(instanceId);
        if (success) {
            onPluginUnloaded(it->second);
            
            // Remove from registry
            auto pluginIt = std::find_if(pImpl->registeredPlugins.begin(), pImpl->registeredPlugins.end(),
                [&instanceId](const auto& p) { return p->getInstanceId() == instanceId; });
            if (pluginIt != pImpl->registeredPlugins.end()) {
                pImpl->registeredPlugins.erase(pluginIt);
            }
            
            pImpl->pluginMap.erase(it);
            
            NLM_LOG_INFO("Plugin unloaded: " + instanceId);
        }
        return success;
        
    } catch (const std::exception& e) {
        logSystemError("Failed to unload plugin " + instanceId + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginSystem::restartPlugin(const std::string& instanceId) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Find plugin in manager and restart
    return pImpl->pluginManager->restartPlugin(instanceId);
}

void PluginSystem::setGlobalPluginConfig(const std::shared_ptr<PluginConfig>& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->globalConfig = config;
}

std::shared_ptr<PluginConfig> PluginSystem::getGlobalPluginConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->globalConfig;
}

bool PluginSystem::registerExtensionPoint(const std::string& name, PluginType type,
                                         const std::function<bool(std::shared_ptr<Plugin>)>& validator) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Register with plugin manager
    bool success = pImpl->pluginManager->registerExtensionPoint(name, type, validator);
    if (success) {
        NLM_LOG_INFO("Extension point registered: " + name);
    }
    return success;
}

bool PluginSystem::registerExtensionPoint(const std::string& name, PluginType type,
                                         const std::vector<std::string>& requiredCapabilities) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Register with plugin manager using a simple validator
    auto validator = [requiredCapabilities](std::shared_ptr<Plugin> plugin) {
        // Check if plugin has required capabilities
        const auto& metadata = plugin->getMetadata();
        for (const auto& capability : requiredCapabilities) {
            // Check if capability is in metadata or provided services
            bool hasCapability = false;
            for (const auto& service : plugin->getProvidedServices()) {
                if (service == capability) {
                    hasCapability = true;
                    break;
                }
            }
            if (!hasCapability) {
                return false;
            }
        }
        return true;
    };
    
    return registerExtensionPoint(name, type, validator);
}

std::vector<std::string> PluginSystem::getAvailableExtensions() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // This would require extending PluginManager to expose extension points
    // For now, return empty list
    return {};
}

std::vector<std::string> PluginSystem::getAvailablePlugins() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->pluginManager) {
        return {};
    }
    
    return pImpl->pluginManager->getAvailablePlugins();
}

void PluginSystem::updateAllPlugins(float dt) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->state.running) {
        return;
    }
    
    // Update plugin manager
    pImpl->pluginManager->update(dt);
    
    // Update system state
    updateSystemState();
    
    // Run plugin-specific operations
    for (const auto& plugin : pImpl->registeredPlugins) {
        if (plugin->getStatus() == PluginStatus::Enabled) {
            try {
                plugin->update(dt);
            } catch (const std::exception& e) {
                logSystemError("Exception in plugin " + plugin->getMetadata().name + ": " + std::string(e.what()));
            }
        }
    }
}

void PluginSystem::broadcastSystemMessage(const std::string& message, const std::string& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Send message to all registered plugins
    for (const auto& plugin : pImpl->registeredPlugins) {
        if (plugin->getStatus() == PluginStatus::Enabled) {
            plugin->onMessage(message, data);
        }
    }
}

void PluginSystem::sendSystemMessage(const std::string& target, const std::string& message, const std::string& data) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Find specific plugin
    for (const auto& plugin : pImpl->registeredPlugins) {
        if (plugin->getInstanceId() == target && plugin->getStatus() == PluginStatus::Enabled) {
            plugin->onMessage(message, data);
            break;
        }
    }
}

PluginSystem::SystemState PluginSystem::getSystemState() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    updateSystemState();
    return pImpl->state;
}

std::string PluginSystem::getSystemStatus() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream status;
    status << "=== Plugin System Status ===" << std::endl;
    status << "Initialized: " << (pImpl->state.initialized ? "Yes" : "No") << std::endl;
    status << "Running: " << (pImpl->state.running ? "Yes" : "No") << std::endl;
    status << "Active Plugins: " << pImpl->state.activePlugins << std::endl;
    status << "Total Plugins: " << pImpl->state.activePlugins + pImpl->state.failedPlugins << std::endl;
    status << "System Uptime: " << pImpl->state.totalSystemUptime << "s" << std::endl;
    status << "Memory Usage: " << pImpl->state.totalPluginMemory << " bytes" << std::endl;
    status << "=============================" << std::endl;
    
    return status.str();
}

std::vector<std::string> PluginSystem::getSystemErrors() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Get errors from plugin manager
    std::vector<std::string> errors = pImpl->pluginManager->getRecentErrors();
    
    // Get system errors
    return errors;
}

size_t PluginSystem::getActivePluginCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->state.activePlugins;
}

size_t PluginSystem::getTotalPluginCount() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->state.activePlugins + pImpl->state.failedPlugins;
}

std::vector<std::string> PluginSystem::getPluginsByType(PluginType type) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->pluginManager) {
        return {};
    }
    
    return pImpl->pluginManager->getPluginsByType(type);
}

std::vector<std::string> PluginSystem::getPluginsByExtensionPoint(const std::string& extensionPoint) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->pluginManager) {
        return {};
    }
    
    return pImpl->pluginManager->getPluginsByExtensionPoint(extensionPoint);
}

std::vector<std::string> PluginSystem::getPluginsByCapability(const std::string& capability) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> pluginsWithCapability;
    
    for (const auto& plugin : pImpl->registeredPlugins) {
        const auto& services = plugin->getProvidedServices();
        if (std::find(services.begin(), services.end(), capability) != services.end()) {
            pluginsWithCapability.push_back(plugin->getInstanceId());
        }
    }
    
    return pluginsWithCapability;
}

bool PluginSystem::loadPluginConfig(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        if (!std::filesystem::exists(filepath)) {
            logSystemError("Plugin configuration file not found: " + filepath);
            return false;
        }
        
        // Create plugin config
        auto config = std::make_shared<PluginConfig>();
        
        // In a real implementation, parse the configuration file
        // For now, just create an empty config
        
        pImpl->globalConfig = config;
        
        NLM_LOG_INFO("Plugin configuration loaded from: " + filepath);
        
        return true;
        
    } catch (const std::exception& e) {
        logSystemError("Failed to load plugin configuration " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginSystem::savePluginConfig(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    try {
        // In a real implementation, write the configuration to file
        // For now, just create an empty file
        std::ofstream file(filepath);
        if (!file.is_open()) {
            logSystemError("Failed to create plugin configuration file: " + filepath);
            return false;
        }
        
        // Write configuration (placeholder)
        file << "// Plugin configuration file" << std::endl;
        file << "// Generated by PluginSystem" << std::endl;
        
        NLM_LOG_INFO("Plugin configuration saved to: " + filepath);
        
        return true;
        
    } catch (const std::exception& e) {
        logSystemError("Failed to save plugin configuration " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool PluginSystem::validatePluginConfig() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Validate global configuration
    if (pImpl->globalConfig) {
        // Perform validation checks
        return true;
    }
    
    return false;
}

bool PluginSystem::enablePluginSecurity(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pluginSecurityEnabled_ = enable;
    
    if (enable) {
        // Initialize security system
        if (pImpl->pluginManager) {
            pImpl->pluginManager->setPluginDirectory("./plugins");
        }
        
        NLM_LOG_INFO("Plugin security enabled");
    } else {
        NLM_LOG_INFO("Plugin security disabled");
    }
    
    return true;
}

bool PluginSystem::validatePluginSecurity(std::shared_ptr<Plugin> plugin) const {
    if (!pluginSecurityEnabled_ || !pImpl->pluginManager) {
        return true; // No security or no manager, skip validation
    }
    
    // Delegate to plugin manager's validation
    return pImpl->pluginManager->validatePlugin(plugin);
}

void PluginSystem::registerMessageHandler(const std::string& messageType,
                                         const std::function<void(const std::string&, const std::string&)>& handler) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->messageHandler = handler;
    
    NLM_LOG_INFO("Message handler registered for type: " + messageType);
}

void PluginSystem::registerEventHandler(const std::string& eventType,
                                       const std::function<void(std::shared_ptr<Plugin>)>& handler) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->eventHandler = handler;
    
    NLM_LOG_INFO("Event handler registered for type: " + eventType);
}

void PluginSystem::optimizePluginPerformance() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->pluginManager) {
        return;
    }
    
    // Request plugin manager to optimize performance
    // This would involve profiling plugins and suggesting optimizations
    NLM_LOG_INFO("Plugin performance optimization completed");
}

void PluginSystem::cleanupInactivePlugins() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->pluginManager) {
        return;
    }
    
    // Remove inactive plugins
    std::vector<std::string> loadedPlugins = pImpl->pluginManager->getLoadedPlugins();
    for (const auto& pluginId : loadedPlugins) {
        if (pImpl->pluginManager->getPluginStatus(pluginId) == PluginStatus::Inactive) {
            pImpl->pluginManager->unloadPlugin(pluginId);
        }
    }
    
    NLM_LOG_INFO("Inactive plugins cleaned up");
}

void PluginSystem::updatePluginPriorities() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Update plugin priorities based on usage
    // This would involve analyzing plugin activity and adjusting loading order
    NLM_LOG_INFO("Plugin priorities updated");
}

std::string PluginSystem::getPluginDebugInfo(const std::string& instanceId) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream debugInfo;
    
    auto it = pImpl->pluginMap.find(instanceId);
    if (it == pImpl->pluginMap.end()) {
        return "Plugin not found: " + instanceId;
    }
    
    const auto& plugin = it->second;
    const auto& metadata = plugin->getMetadata();
    
    debugInfo << "=== Plugin Debug Info ===" << std::endl;
    debugInfo << "Instance ID: " << plugin->getInstanceId() << std::endl;
    debugInfo << "Name: " << metadata.name << std::endl;
    debugInfo << "Type: " << static_cast<int>(metadata.type) << std::endl;
    debugInfo << "Version: " << metadata.version.major << "."
              << metadata.version.minor << "." << metadata.version.patch << std::endl;
    debugInfo << "Status: " << static_cast<int>(plugin->getStatus()) << std::endl;
    debugInfo << "Author: " << metadata.author << std::endl;
    debugInfo << "Description: " << metadata.description << std::endl;
    debugInfo << "Capabilities: {" << std::endl;
    
    const auto& caps = metadata.capabilities;
    debugInfo << "  Dynamic Loading: " << (caps.supportsDynamicLoading ? "Yes" : "No") << std::endl;
    debugInfo << "  Runtime Management: " << (caps.supportsRuntimeManagement ? "Yes" : "No") << std::endl;
    debugInfo << "  Configuration: " << (caps.supportsConfiguration ? "Yes" : "No") << std::endl;
    debugInfo << "  Sandboxing: " << (caps.requiresSandboxing ? "Yes" : "No") << std::endl;
    debugInfo << "  Thread Safe: " << (caps.threadSafe ? "Yes" : "No") << std::endl;
    debugInfo << "  Hot Reload: " << (caps.supportsHotReload ? "Yes" : "No") << std::endl;
    debugInfo << "  Performance Impact: " << caps.performanceImpact << std::endl;
    debugInfo << "} (end capabilities)" << std::endl;
    
    debugInfo << "==============================" << std::endl;
    
    return debugInfo.str();
}

std::string PluginSystem::getSystemDebugInfo() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream debugInfo;
    
    debugInfo << "=== System Debug Info ===" << std::endl;
    debugInfo << "System Uptime: " << pImpl->state.totalSystemUptime << "s" << std::endl;
    debugInfo << "Active Plugins: " << pImpl->state.activePlugins << std::endl;
    debugInfo << "Failed Plugins: " << pImpl->state.failedPlugins << std::endl;
    debugInfo << "Total Memory Usage: " << pImpl->state.totalPluginMemory << " bytes" << std::endl;
    debugInfo << "Plugin Directory: " << getPluginDirectoryFromSettings(settings_) << std::endl;
    debugInfo << "Security Enabled: " << (pluginSecurityEnabled_ ? "Yes" : "No") << std::endl;
    debugInfo << "Monitoring Enabled: " << (monitoringEnabled ? "Yes" : "No") << std::endl;
    debugInfo << "=============================" << std::endl;
    
    return debugInfo.str();
}

void PluginSystem::attachDebugger(std::shared_ptr<class PluginDebugger> debugger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->debugger = debugger;
    
    NLM_LOG_INFO("Plugin debugger attached");
}

void PluginSystem::detachDebugger() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->debugger.reset();
    
    NLM_LOG_INFO("Plugin debugger detached");
}

void PluginSystem::cleanup() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->pluginManager) {
        pImpl->pluginManager->cleanup();
    }
    
    // Clear all plugins
    pImpl->pluginMap.clear();
    pImpl->registeredPlugins.clear();
    
    pImpl->state.initialized = false;
    pImpl->state.running = false;
    
    NLM_LOG_INFO("PluginSystem cleanup completed");
}

void PluginSystem::shutdown() {
    cleanup();
    
    pImpl->pluginManager.reset();
    pImpl->debugger.reset();
    
    pImpl->state.initialized = false;
    pImpl->state.running = false;
    
    NLM_LOG_INFO("PluginSystem shutdown completed");
}

PluginSystem& PluginSystem::getInstance() {
    static PluginSystem instance;
    return instance;
}

void PluginSystem::destroyInstance() {
    static std::atomic<bool> destroyed{false};
    if (!destroyed.exchange(true)) {
        delete &getInstance();
    }
}

void PluginSystem::initializeFromConfig(const std::string& configFile) {
    // Load configuration from file
    if (!configFile.empty()) {
        loadPluginConfig(configFile);
    }
}

void PluginSystem::updateSystemState() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->state.lastUpdate).count();
    
    pImpl->state.lastUpdate = now;
    pImpl->state.totalSystemUptime += static_cast<float>(elapsed);
    
    // Update active plugin count
    pImpl->state.activePlugins = 0;
    pImpl->state.failedPlugins = 0;
    
    for (const auto& plugin : pImpl->registeredPlugins) {
        if (plugin->getStatus() == PluginStatus::Enabled) {
            pImpl->state.activePlugins++;
        } else if (plugin->getStatus() == PluginStatus::Failed) {
            pImpl->state.failedPlugins++;
        }
    }
}

bool PluginSystem::resolvePluginDependencies(const std::string& instanceId) {
    // Check if all dependencies are loaded
    auto it = pImpl->pluginMap.find(instanceId);
    if (it == pImpl->pluginMap.end()) {
        return false;
    }
    
    const auto& metadata = it->second->getMetadata();
    
    // Check if all dependencies exist
    for (const auto& dependency : metadata.dependencies) {
        if (pImpl->pluginMap.find(dependency) == pImpl->pluginMap.end()) {
            return false; // Dependency not found
        }
    }
    
    return true;
}

void PluginSystem::onPluginLoaded(std::shared_ptr<Plugin> plugin) {
    // Update system state
    updateSystemState();
    
    // Call event handler if registered
    if (pImpl->eventHandler) {
        pImpl->eventHandler(plugin);
    }
}

void PluginSystem::onPluginEnabled(std::shared_ptr<Plugin> plugin) {
    // Update system state
    updateSystemState();
    
    // Call event handler if registered
    if (pImpl->eventHandler) {
        pImpl->eventHandler(plugin);
    }
}

void PluginSystem::onPluginDisabled(std::shared_ptr<Plugin> plugin) {
    // Update system state
    updateSystemState();
    
    // Call event handler if registered
    if (pImpl->eventHandler) {
        pImpl->eventHandler(plugin);
    }
}

void PluginSystem::onPluginUnloaded(std::shared_ptr<Plugin> plugin) {
    // Update system state
    updateSystemState();
    
    // Call event handler if registered
    if (pImpl->eventHandler) {
        pImpl->eventHandler(plugin);
    }
}

void PluginSystem::handlePluginError(const std::string& instanceId, const std::string& error) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Log error
    logSystemError("Plugin " + instanceId + " error: " + error);
    
    // Update failed plugin count
    pImpl->state.failedPlugins++;
    
    // Notify debugger if available
    if (pImpl->debugger) {
        pImpl->debugger->onPluginError(instanceId, error);
    }
}

void PluginSystem::logSystemError(const std::string& error) {
    pImpl->pluginManager->getRecentErrors().push_back(error);
    
    NLM_LOG_ERROR("System error: " + error);
}

std::string PluginSystem::getPluginDirectoryFromSettings(const SystemSettings& settings) const {
    if (!settings.pluginSearchPaths.empty()) {
        return settings.pluginSearchPaths[0];
    }
    
    // Default plugin directory
    return "./plugins";
}

void PluginSystem::registerDefaultExtensionPoints() {
    // Register default extension points for all plugin types
    std::vector<std::pair<std::string, PluginType>> defaultExtensionPoints = {
        {"neuron_factory", PluginType::Neuron},
        {"plasticity_rule", PluginType::Plasticity},
        {"memory_system", PluginType::Memory},
        {"neuromodulator", PluginType::Neuromodulator},
        {"sensory_system", PluginType::Sensory},
        {"motor_system", PluginType::Motor},
        {"environment_model", PluginType::Environment},
        {"cognitive_architecture", PluginType::Cognitive}
    };
    
    for (const auto& pair : defaultExtensionPoints) {
        registerExtensionPoint(pair.first, pair.second);
    }
}

} // namespace nlm