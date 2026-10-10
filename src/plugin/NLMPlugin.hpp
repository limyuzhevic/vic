// NLM Plugin and Module Headers
#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace nlm {

/**
 * Plugin configuration
 */
struct PluginConfig {
    std::map<std::string, std::string> settings;
    
    std::string getString(const std::string& key) const {
        auto it = settings.find(key);
        return (it != settings.end()) ? it->second : "";
    }
    
    int getInt(const std::string& key, int defaultValue = 0) const {
        auto it = settings.find(key);
        if (it != settings.end()) {
            try {
                return std::stoi(it->second);
            } catch (const std::exception& e) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    double getDouble(const std::string& key, double defaultValue = 0.0) const {
        auto it = settings.find(key);
        if (it != settings.end()) {
            try {
                return std::stod(it->second);
            } catch (const std::exception& e) {
                return defaultValue;
            }
        }
        return defaultValue;
    }
    
    bool getBool(const std::string& key, bool defaultValue = false) const {
        auto it = settings.find(key);
        if (it != settings.end()) {
            return it->second == "true" || it->second == "1" || it->second == "yes";
        }
        return defaultValue;
    }
};

/**
 * NLM Plugin base class
 */
class NLMPlugin {
public:
    NLMPlugin(const std::string& name, const std::string& version, const std::string& description);
    virtual ~NLMPlugin();
    
    /**
     * Get plugin name
     */
    const std::string& getName() const;
    
    /**
     * Get plugin version
     */
    const std::string& getVersion() const;
    
    /**
     * Get plugin description
     */
    const std::string& getDescription() const;
    
    /**
     * Check if plugin is initialized
     */
    bool isInitialized() const;
    
    /**
     * Initialize plugin
     */
    bool initialize(const PluginConfig& config);
    
    /**
     * Shutdown plugin
     */
    void shutdown();
    
    /**
     * Register extension
     */
    bool registerExtension(const std::string& name, std::function<void()> extension);
    
    /**
     * Execute extension
     */
    void executeExtension(const std::string& name);
    
    /**
     * Get plugin configuration
     */
    PluginConfig getConfig() const;
    
    /**
     * Set plugin configuration
     */
    void setConfig(const PluginConfig& config);
    
protected:
    virtual bool onInitialize(const PluginConfig& config) {
        return true;
    }
    
    virtual void onShutdown() {
    }
    
private:
    std::string _name;
    std::string _version;
    std::string _description;
    bool _initialized;
    PluginConfig _config;
    std::map<std::string, std::function<void()>> _extensions;
};

/**
 * NLM Module base class
 */
class NLMModule {
public:
    NLMModule(const std::string& name, const std::string& type, const std::string& version);
    virtual ~NLMModule();
    
    /**
     * Get module name
     */
    const std::string& getName() const;
    
    /**
     * Get module type
     */
    const std::string& getType() const;
    
    /**
     * Get module version
     */
    const std::string& getVersion() const;
    
    /**
     * Check if module is enabled
     */
    bool isEnabled() const;
    
    /**
     * Check if module is initialized
     */
    bool isInitialized() const;
    
    /**
     * Enable module
     */
    bool enable();
    
    /**
     * Disable module
     */
    bool disable();
    
    /**
     * Initialize module
     */
    bool initialize();
    
    /**
     * Shutdown module
     */
    void shutdown();
    
    /**
     * Set dependency
     */
    void setDependency(const std::string& dependency);
    
    /**
     * Get dependencies
     */
    const std::vector<std::string>& getDependencies() const;
    
    /**
     * Check dependencies
     */
    bool checkDependencies() const;
    
protected:
    virtual bool onEnable() {
        return true;
    }
    
    virtual bool onDisable() {
        return true;
    }
    
    virtual bool onInitialize() {
        return true;
    }
    
    virtual bool onShutdown() {
        return true;
    }
    
private:
    std::string _name;
    std::string _type;
    std::string _version;
    bool _enabled;
    bool _initialized;
    std::vector<std::string> _dependencies;
};

/**
 * NLM Plugin Manager
 */
class NLMPluginManager {
public:
    NLMPluginManager();
    ~NLMPluginManager();
    
    /**
     * Load plugin from file
     */
    bool loadPlugin(const std::string& filePath);
    
    /**
     * Unload plugin
     */
    bool unloadPlugin(const std::string& name);
    
    /**
     * Enable plugin
     */
    bool enablePlugin(const std::string& name);
    
    /**
     * Disable plugin
     */
    bool disablePlugin(const std::string& name);
    
    /**
     * Get plugin
     */
    NLMPlugin* getPlugin(const std::string& name);
    
    /**
     * Get all plugins
     */
    const std::map<std::string, std::unique_ptr<NLMPlugin>>& getAllPlugins() const;
    
    /**
     * Execute plugin extension
     */
    void executePluginExtension(const std::string& name, const std::string& extension);
    
    /**
     * Save plugin configuration
     */
    void savePluginConfig(const std::string& name, const PluginConfig& config);
    
private:
    std::map<std::string, std::unique_ptr<NLMPlugin>> _plugins;
};

/**
 * NLM Module Manager
 */
class NLMModuleManager {
public:
    NLMModuleManager();
    ~NLMModuleManager();
    
    /**
     * Load module from file
     */
    bool loadModule(const std::string& filePath);
    
    /**
     * Unload module
     */
    bool unloadModule(const std::string& name);
    
    /**
     * Enable module
     */
    bool enableModule(const std::string& name);
    
    /**
     * Disable module
     */
    bool disableModule(const std::string& name);
    
    /**
     * Get module
     */
    NLMModule* getModule(const std::string& name);
    
    /**
     * Get all modules
     */
    const std::map<std::string, std::unique_ptr<NLMModule>>& getAllModules() const;
    
    /**
     * Save module configuration
     */
    void saveModuleConfig(const std::string& name, const ModuleConfig& config);
    
private:
    std::map<std::string, std::unique_ptr<NLMModule>> _modules;
};

/**
 * NLM Implementation
 */
class NLMImplementation {
public:
    NLMImplementation();
    ~NLMImplementation();
    
    /**
     * Register plugin
     */
    void registerPlugin(std::unique_ptr<NLMPlugin> plugin);
    
    /**
     * Register module
     */
    void registerModule(std::unique_ptr<NLMModule> module);
    
    /**
     * Get plugin manager
     */
    NLMPluginManager& getPluginManager();
    
    /**
     * Get module manager
     */
    NLMModuleManager& getModuleManager();
    
    /**
     * Initialize all plugins and modules
     */
    void initializeAll();
    
    /**
     * Shutdown all plugins and modules
     */
    void shutdownAll();
    
private:
    NLMPluginManager _pluginManager;
    NLMModuleManager _moduleManager;
};

/**
 * Plugin interface for plugin authors
 */
class PluginInterface {
public:
    PluginInterface();
    virtual ~PluginInterface();
    
    /**
     * Called when plugin is loaded
     */
    virtual void onPluginLoad();
    
    /**
     * Called when plugin is unloaded
     */
    virtual void onPluginUnload();
    
    /**
     * Called when plugin is enabled
     */
    virtual void onPluginEnable();
    
    /**
     * Called when plugin is disabled
     */
    virtual void onPluginDisable();
};

/**
 * Module configuration
 */
struct ModuleConfig {
    std::map<std::string, std::string> settings;
};

} // namespace nlm