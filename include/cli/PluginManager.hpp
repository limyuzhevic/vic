#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include "../../../core/Types/Types.hpp"
#include "../../../core/Config/Config.hpp"
#include "../../../core/Logger/Logger.hpp"

namespace nlm {
namespace cli {

class Plugin;

class PluginManager {
public:
    PluginManager(std::shared_ptr<Config> config, std::shared_ptr<Logger> logger);
    ~PluginManager();
    
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool reloadPlugin(const std::string& pluginName);
    
    bool isPluginLoaded(const std::string& pluginName) const;
    const std::vector<std::string>& getLoadedPlugins() const;
    
    std::shared_ptr<Plugin> getPlugin(const std::string& pluginName);
    
    bool executePluginFunction(const std::string& pluginName, 
                              const std::string& functionName,
                              const std::vector<std::string>& args);
    
    std::string getPluginInfo(const std::string& pluginName) const;
    std::string listAvailablePlugins() const;
    
    bool validatePluginDependencies(const std::string& pluginName) const;
    
    void setPluginDirectory(const std::string& directory) { pluginDirectory_ = directory; }
    const std::string& getPluginDirectory() const { return pluginDirectory_; }
    
    void logStatus() const;
    
    static bool initializePlugins(const std::vector<std::string>& pluginNames, std::shared_ptr<PluginManager> manager);
    static bool shutdownPlugins(std::shared_ptr<PluginManager> manager);

private:
    struct Impl;
    Impl* pImpl;
    std::string pluginDirectory_ = "./plugins";
    std::shared_ptr<Config> config_;
    std::shared_ptr<Logger> logger_;
};

class Plugin {
public:
    virtual ~Plugin() = default;
    
    virtual const std::string& getName() const = 0;
    virtual const std::string& getVersion() const = 0;
    virtual const std::string& getAuthor() const = 0;
    virtual const std::string& getDescription() const = 0;
    
    virtual bool initialize(std::shared_ptr<PluginManager> manager, 
                           std::shared_ptr<Config> config) = 0;
    virtual void shutdown() = 0;
    virtual void update(TimestepDuration dt) = 0;
    
    virtual bool onPluginMessage(const std::string& message, 
                               const std::vector<std::string>& data) = 0;
    
    virtual std::string getPluginSpecificInfo() const = 0;
    
    virtual bool requires(const std::string& dependency) const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;
    
protected:
    std::shared_ptr<PluginManager> pluginManager_;
    std::shared_ptr<Config> config_;
};

} // namespace cli
} // namespace nlm
