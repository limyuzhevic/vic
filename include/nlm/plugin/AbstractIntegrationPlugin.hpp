#ifndef NLM_PLUGIN_ABSTRACTINTEGRATIONPLUGIN_HPP
#define NLM_PLUGIN_ABSTRACTINTEGRATIONPLUGIN_HPP

#include "PluginBase.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {
namespace plugin {

struct IntegrationEvent {
    std::string pluginName;
    std::string eventType;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> data;
    bool handled;
};

class AbstractIntegrationPlugin : public virtual IPlugin {
public:
    virtual ~AbstractIntegrationPlugin() = default;
    
    virtual std::string getIntegrationType() const = 0;
    virtual std::vector<std::string> getIntegratedSystems() const = 0;
    
    // Integration configuration
    virtual bool configureIntegration(const std::unordered_map<std::string, std::string>& config) = 0;
    virtual std::unordered_map<std::string, std::string> getIntegrationConfig() const = 0;
    
    // Integration lifecycle
    virtual bool preIntegrate() = 0;
    virtual bool postIntegrate() = 0;
    virtual bool preDeintegrate() = 0;
    virtual bool postDeintegrate() = 0;
    
    // Integration management
    virtual bool integrateWithSystem(const std::string& systemName, const std::unordered_map<std::string, std::string>& config) = 0;
    virtual bool deintegrateFromSystem(const std::string& systemName) = 0;
    virtual bool isIntegratedWithSystem(const std::string& systemName) const = 0;
    virtual std::vector<std::string> getIntegratedSystemsList() const = 0;
    
    // Event handling
    virtual bool handleEvent(const IntegrationEvent& event) = 0;
    virtual bool addEventHandler(const std::string& eventType, const std::function<bool(const IntegrationEvent&)>& handler) = 0;
    virtual bool removeEventHandler(const std::string& eventType, const std::function<bool(const IntegrationEvent&)>& handler) = 0;
    virtual std::vector<std::string> getEventTypes() const = 0;
    
    // Integration monitoring
    virtual uint64_t getIntegrationCount() const = 0;
    virtual double getAverageIntegrationTime() const = 0;
    virtual uint64_t getIntegrationErrors() const = 0;
    virtual std::string getLastIntegrationError() const = 0;
    
    // Integration extension points
    virtual void* getIntegrationExtension(const std::string& extensionName) = 0;
    virtual bool registerIntegrationExtension(const std::string& extensionName, void* extension) = 0;
    
    // Integration optimization
    virtual bool optimizeIntegration() = 0;
    virtual bool tuneIntegrationParameters(const std::unordered_map<std::string, double>& parameters) = 0;
    
    // Integration validation
    virtual bool validateIntegrationConfiguration() const = 0;
    virtual bool validateIntegrationDependencies() const = 0;
    
protected:
    std::unordered_map<std::string, void*> integrationExtensions_;
    std::unordered_map<std::string, std::function<bool(const IntegrationEvent&)>> eventHandlers_;
    std::unordered_map<std::string, std::string> config_;
    uint64_t integrationCount_ = 0;
    uint64_t integrationErrors_ = 0;
    std::chrono::duration<double> totalIntegrationTime_;
};

} // namespace plugin
} // namespace nlm

#endif // NLM_PLUGIN_ABSTRACTINTEGRATIONPLUGIN_HPP