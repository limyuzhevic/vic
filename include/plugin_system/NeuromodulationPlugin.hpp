#pragma once

#include "Service.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <unordered_map>
#include <vector>
#include <string>

namespace nlm {

class NeuromodulationPlugin : public Plugin {
public:
    NeuromodulationPlugin();
    ~NeuromodulationPlugin() override;
    
    const PluginMetadata& getMetadata() const override;
    bool initialize() override;
    void shutdown() override;
    bool isInitialized() const override;
    std::vector<ServiceInfo> getProvidedServices() const override;
    bool onServiceRequest(const ServiceRequest& request, ServiceResponse& response) override;
    bool onPluginEvent(const PluginEvent& event) override;
    bool execute(const ExecutionContext& context) override;
    void onBrainUpdate(const BrainUpdateEvent& event) override;
    std::string getName() const override;
    std::string getVersion() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
