#ifndef NLM_PLUGIN_IPLUGIN_H
#define NLM_PLUGIN_IPLUGIN_H

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace nlm {
namespace plugin {

enum class PluginState {
    UNLOADED,
    LOADING,
    LOADED,
    INITIALIZED,
    RUNNING,
    SHUTDOWN,
    FAILED,
    UNLOADING
};

enum class PluginCapability {
    NEURAL,
    SENSORY,
    MOTOR,
    MEMORY,
    NEUROMODULATION,
    ENVIRONMENT,
    EXPERIMENT,
    VISUALIZATION
};

class IPlugin {
public:
    virtual ~IPlugin() = default;

    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getAuthor() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getLicense() const = 0;
    virtual std::vector<PluginCapability> getCapabilities() const = 0;
    virtual std::vector<std::string> getDependencies() const = 0;

    // Lifecycle management
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    virtual bool shutdown() = 0;
    virtual bool update(double deltaTime) = 0;

    // State management
    virtual PluginState getState() const = 0;
    virtual bool isEnabled() const = 0;
    virtual bool enable() = 0;
    virtual bool disable() = 0;

    // Plugin introspection
    virtual std::string getType() const = 0;
    virtual std::string getID() const = 0;

    // Event handling
    virtual void onEvent(const std::string& eventName, const std::map<std::string, std::string>& data) = 0;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_IPLUGIN_H
