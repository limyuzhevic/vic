#ifndef NLM_PLUGIN_ABSTRACTCOMPONENTPLUGIN_HPP
#define NLM_PLUGIN_ABSTRACTCOMPONENTPLUGIN_HPP

#include "PluginBase.hpp"
#include <memory>
#include <string>
#include <unordered_map>

namespace nlm {
namespace plugin {

class AbstractComponentPlugin : public virtual IPlugin {
public:
    virtual ~AbstractComponentPlugin() = default;
    
    virtual std::string getComponentType() const = 0;
    virtual std::string getComponentName() const = 0;
    
    // Component-specific configuration
    virtual bool configureComponent(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual std::unordered_map<std::string, std::string> getComponentConfig() const = 0;
    
    // Component lifecycle
    virtual bool preInitialize() = 0;
    virtual bool postInitialize() = 0;
    virtual bool preShutdown() = 0;
    virtual bool postShutdown() = 0;
    
    // Component state
    virtual ComponentState getComponentState() const = 0;
    virtual std::string getComponentStatus() const = 0;
    
    // Component extension points
    virtual void* getComponentExtension(const std::string& extensionName) = 0;
    virtual bool registerComponentExtension(const std::string& extensionName, void* extension) = 0;
    
    // Component interaction
    virtual bool connectToComponent(const std::string& componentName) = 0;
    virtual bool disconnectFromComponent(const std::string& componentName) = 0;
    virtual bool isConnectedToComponent(const std::string& componentName) const = 0;
    
    // Component validation
    virtual bool validateComponentConfiguration() const = 0;
    virtual bool validateComponentDependencies() const = 0;
    
protected:
    std::unordered_map<std::string, void*> extensions_;
    std::unordered_map<std::string, std::string> config_;
    std::unordered_map<std::string, std::string> componentConnections_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_ABSTRACTCOMPONENTPLUGIN_HPP