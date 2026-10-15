#pragma once

#include "IPlugin.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Example memory management plugin
class MemoryManagementPlugin : public IPlugin {
public:
    MemoryManagementPlugin();
    ~MemoryManagementPlugin() override;
    
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getType() const override;
    
    bool initialize(Brain* brain, std::shared_ptr<Config> config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    // Plugin-specific methods
    void setCapacityMultiplier(float multiplier);
    void enableConsolidation(bool enable);
    void setConsolidationThreshold(float threshold);
    
private:
    Brain* brain_;
    std::shared_ptr<Config> config_;
    float capacityMultiplier_;
    bool consolidationEnabled_;
    float consolidationThreshold_;
    size_t originalCapacity_;
    size_t originalMaxEpisodes_;
};

}} // namespace nlm