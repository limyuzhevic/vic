#ifndef NLM_PLUGIN_PLUGINBASE_HPP
#define NLM_PLUGIN_PLUGINBASE_HPP

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include <chrono>
#include <type_traits>

namespace nlm {
namespace plugin {

enum class PluginType {
    COMPONENT,
    PROCESSING,
    INTEGRATION,
    ANALYSIS,
    CONFIGURATION,
    PERFORMANCE
};

struct PluginVersion {
    int major = 0;
    int minor = 0;
    int patch = 0;
    std::string prerelease;
    std::string build;
    
    std::string toString() const {
        std::string result = std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
        if (!prerelease.empty()) {
            result += "-" + prerelease;
        }
        if (!build.empty()) {
            result += "+" + build;
        }
        return result;
    }
    
    bool operator==(const PluginVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
};

struct PluginMetadata {
    std::string name;
    PluginVersion version;
    PluginType type;
    std::string description;
    std::string author;
    std::string license;
    std::vector<std::string> dependencies;
    PluginVersion minCompatibleVersion;
    std::unordered_map<std::string, std::string> configuration;
    std::chrono::system_clock::time_point creationTime;
    std::chrono::system_clock::time_point lastModified;
};

enum class PluginState {
    UNLOADED,
    LOADING,
    LOADED,
    INITIALIZING,
    INITIALIZED,
    ACTIVATING,
    ACTIVATED,
    DEACTIVATING,
    DEACTIVATED,
    SHUTDOWN,
    ERROR
};

class PluginError : public std::runtime_error {
public:
    PluginError(const std::string& message, PluginState state = PluginState::ERROR)
        : std::runtime_error(message), state_(state) {}
    
    PluginState getState() const { return state_; }
    
private:
    PluginState state_;
};

class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginType getType() const = 0;
    virtual PluginMetadata getMetadata() const = 0;
    
    virtual bool initialize(const PluginMetadata& metadata) = 0;
    virtual void shutdown() = 0;
    virtual bool activate() = 0;
    virtual bool deactivate() = 0;
    virtual bool loadDependencies() = 0;
    virtual bool checkConflicts() = 0;
    
    virtual PluginState getState() const = 0;
    virtual std::string getError() const = 0;
    
    virtual void* getExtension(const std::string& extensionName) = 0;
    virtual bool registerExtension(const std::string& extensionName, void* extension) = 0;
    
    virtual void setConfig(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual std::unordered_map<std::string, std::string> getConfig() const = 0;
};

template<typename TPlugin>
class PluginWrapper : public IPlugin {
public:
    template<typename... Args>
    PluginWrapper(Args&&... args) : plugin_(std::make_unique<TPlugin>(std::forward<Args>(args)...)) {}
    
    std::string getName() const override { return plugin_->getName(); }
    std::string getVersion() const override { return plugin_->getVersion(); }
    std::string getDescription() const override { return plugin_->getDescription(); }
    PluginType getType() const override { return plugin_->getType(); }
    PluginMetadata getMetadata() const override { return plugin_->getMetadata(); }
    
    bool initialize(const PluginMetadata& metadata) override { return plugin_->initialize(metadata); }
    void shutdown() override { plugin_->shutdown(); }
    bool activate() override { return plugin_->activate(); }
    bool deactivate() override { return plugin_->deactivate(); }
    bool loadDependencies() override { return plugin_->loadDependencies(); }
    bool checkConflicts() override { return plugin_->checkConflicts(); }
    
    PluginState getState() const override { return plugin_->getState(); }
    std::string getError() const override { return plugin_->getError(); }
    
    void* getExtension(const std::string& extensionName) override { return plugin_->getExtension(extensionName); }
    bool registerExtension(const std::string& extensionName, void* extension) override { return plugin_->registerExtension(extensionName, extension); }
    
    void setConfig(const std::unordered_map<std::string, std::string>& config) override { return plugin_->setConfig(config); }
    std::unordered_map<std::string, std::string> getConfig() const override { return plugin_->getConfig(); }
    
    TPlugin* getRawPlugin() { return plugin_.get(); }
    const TPlugin* getRawPlugin() const { return plugin_.get(); }
    
private:
    std::unique_ptr<TPlugin> plugin_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_PLUGINBASE_HPP