#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace nlm {

/**
 * NLM Plugin Interface
 * 
 * Base class for all NLM plugins
 */
class NLMPlugin {
public:
    virtual ~NLMPlugin() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getAuthor() const = 0;
    
    // Dependencies
    virtual std::vector<std::string> getDependencies() const = 0;
    
    // Lifecycle
    virtual bool initialize(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Capabilities
    virtual std::vector<std::string> getCapabilities() const = 0;
    
    // Actions
    virtual bool execute(const std::string& action, const std::vector<std::string>& args) = 0;
    
    // Configuration
    virtual std::unordered_map<std::string, std::string> getConfigSchema() const = 0;
    virtual bool validateConfig(const std::unordered_map<std::string, std::string>& config) const = 0;
};

/**
 * Plugin Factory
 * 
 * Creates plugin instances from dynamic libraries
 */
class PluginFactory {
public:
    virtual ~PluginFactory() = default;
    
    virtual bool loadPlugin(const std::string& filename) = 0;
    virtual std::shared_ptr<NLMPlugin> createPlugin(const std::string& name) = 0;
    virtual bool unloadPlugin(const std::string& name) = 0;
    virtual bool hasPlugin(const std::string& name) const = 0;
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
};

/**
 * Default Plugin Factory Implementation
 */
class DefaultPluginFactory : public PluginFactory {
public:
    DefaultPluginFactory();
    ~DefaultPluginFactory() override;
    
    // PluginFactory interface
    bool loadPlugin(const std::string& filename) override;
    std::shared_ptr<NLMPlugin> createPlugin(const std::string& name) override;
    bool unloadPlugin(const std::string& name) override;
    bool hasPlugin(const std::string& name) const override;
    std::vector<std::string> getLoadedPlugins() const override;
    
    // Configuration
    void setPluginDirectory(const std::string& directory);
    std::string getPluginDirectory() const;
    
    // Scanning
    std::vector<std::string> scanPlugins(const std::string& directory) const;
    bool isPluginFile(const std::string& filename) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Plugin Manager
 * 
 * Manages plugin lifecycle and execution
 */
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Disable copying, enable moving
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager(PluginManager&&) noexcept;
    PluginManager& operator=(PluginManager)&& = delete;

    /**
     * Set plugin factory
     */
    void setPluginFactory(std::shared_ptr<PluginFactory> factory);
    
    /**
     * Load a plugin
     */
    bool loadPlugin(const std::string& filename);
    
    /**
     * Load a plugin by name
     */
    bool loadPluginByName(const std::string& name);
    
    /**
     * Unload a plugin
     */
    bool unloadPlugin(const std::string& name);
    
    /**
     * Get loaded plugins
     */
    std::vector<std::string> getLoadedPlugins() const;
    
    /**
     * Get plugin instance
     */
    std::shared_ptr<NLMPlugin> getPlugin(const std::string& name) const;
    
    /**
     * Initialize all plugins
     */
    bool initializeAll(const std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& configs);
    
    /**
     * Shutdown all plugins
     */
    void shutdownAll();
    
    /**
     * Execute plugin action
     */
    bool executePluginAction(const std::string& pluginName,
                           const std::string& action,
                           const std::vector<std::string>& args);
    
    /**
     * Check if plugin is initialized
     */
    bool isPluginInitialized(const std::string& name) const;
    
    /**
     * Get plugin information
     */
    std::string getPluginInfo(const std::string& name) const;
    std::string getAllPluginsInfo() const;
    
    /**
     * Set plugin directory
     */
    void setPluginDirectory(const std::string& directory);
    
    /**
     * Get plugin directory
     */
    std::string getPluginDirectory() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Basic Plugin Implementation
 * 
 * Template class for simple plugins
 */
template<typename T>
class BasicPlugin : public NLMPlugin {
public:
    BasicPlugin(const std::string& name, const std::string& version,
                const std::string& description, const std::string& author);
    
    std::string getName() const override { return name_; }
    std::string getVersion() const override { return version_; }
    std::string getDescription() const override { return description_; }
    std::string getAuthor() const override { return author_; }
    std::vector<std::string> getDependencies() const override { return dependencies_; }
    
    bool initialize(const std::unordered_map<std::string, std::string>& config) override;
    void shutdown() override;
    bool isInitialized() const override { return initialized_; }
    
    std::vector<std::string> getCapabilities() const override { return capabilities_; }
    
    bool execute(const std::string& action, const std::vector<std::string>& args) override;
    
    std::unordered_map<std::string, std::string> getConfigSchema() const override;
    bool validateConfig(const std::unordered_map<std::string, std::string>& config) const override;
    
protected:
    // Override these methods in derived classes
    virtual bool initializeImpl(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual void shutdownImpl() = 0;
    virtual bool executeImpl(const std::string& action, const std::vector<std::string>& args) = 0;
    virtual std::unordered_map<std::string, std::string> getConfigSchemaImpl() const = 0;
    virtual bool validateConfigImpl(const std::unordered_map<std::string, std::string>& config) const = 0;
    
    void addCapability(const std::string& capability);
    void addDependency(const std::string& dependency);

private:
    std::string name_;
    std::string version_;
    std::string description_;
    std::string author_;
    std::vector<std::string> dependencies_;
    std::vector<std::string> capabilities_;
    bool initialized_;
    std::unordered_map<std::string, std::string> config_;
};

} // namespace nlm
