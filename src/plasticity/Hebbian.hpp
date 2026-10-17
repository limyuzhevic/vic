#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Enhanced Hebbian learning implementation with plugin architecture
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    const char* getDescription() const override;
    const char* getVersion() const override;
    
    // Basic parameters (backwards compatibility)
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    // Advanced parameters
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    void setBaseline(float baseline);
    float getBaseline() const;
    void setUseVariance(bool useVariance);
    bool getUseVariance() const;
    void setCorrelationWindow(float window);
    float getCorrelationWindow() const;
    
    // Plugin architecture support
    std::unordered_map<std::string, std::string> getParameters() const override;
    void setParameters(const std::unordered_map<std::string, std::string>& params) override;
    bool hasParameter(const std::string& name) const override;
    const char* getParameterName(size_t index) const override;
    std::unique_ptr<PlasticityRule> clone() const override;
    bool validate() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Legacy alias for backwards compatibility
using Hebbian = HebbianRule;

} // namespace nlm

// Register the enhanced Hebbian rule
REGISTER_PLASTICITY_RULE(HebbianRule, nlm::HebbianRule)
