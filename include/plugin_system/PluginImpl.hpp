#pragma once

#include "Plugin.hpp"
#include "PluginSystem_impl.hpp"

namespace nlm {

class PluginImpl : public Plugin {
public:
    explicit PluginImpl(std::unique_ptr<Plugin> pluginImpl);
    ~PluginImpl() override;
    
    const PluginMetadata& getMetadata() const override;
    bool initialize() override;
    void shutdown() override;
    bool isInitialized() const override;
    std::vector<ServiceInfo> getProvidedServices() const override;
    bool onServiceRequest(const ServiceRequest& request, ServiceResponse& response) override;
    bool onPluginEvent(const PluginEvent& event) override;
    bool execute(const ExecutionContext& context) override;
    void onBrainUpdate(const class BrainUpdateEvent& event) override;
    std::string getName() const override;
    std::string getVersion() const override;
    
private:
    std::unique_ptr<Plugin> pluginImpl_;
    PluginState state_;
    bool initialized_;
};

} // namespace nlm
