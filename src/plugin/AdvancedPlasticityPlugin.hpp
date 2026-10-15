#pragma once

#include "PluginRegistry.hpp"

namespace nlm {

// Advanced plasticity plugin implementation
class AdvancedPlasticityPlugin : public IPlugin {
public:
    AdvancedPlasticityPlugin();
    ~AdvancedPlasticityPlugin() override;
    
    std::string getName() const override;
    std::string getVersion() const override;
    std::string getDescription() const override;
    std::string getType() const override;
    
    bool initialize(Brain* brain, std::shared_ptr<Config> config) override;
    void update(double timestep) override;
    void shutdown() override;
    
    // Plugin-specific methods
    void setSTDPWeight(float ltp, float ltd);
    void enableHebbian(bool enable);
    void setLearningRate(float rate);
    
private:
    Brain* brain_;
    std::shared_ptr<Config> config_;
    float stdpLTPWeight_;
    float stdpLTDWeight_;
    bool hebbianEnabled_;
    float learningRate_;
};

}} // namespace nlm