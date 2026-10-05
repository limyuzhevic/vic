// Hebbian.hpp - Enhanced with Oja's rule and normalization

#pragma once

#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

class Hebbian {
public:
    Hebbian();
    ~Hebbian();
    
    // Disable copying, enable moving
    Hebbian(const Hebbian&) = delete;
    Hebbian& operator=(const Hebbian&) = delete;
    Hebbian(Hebbian&&) noexcept;
    Hebbian& operator=(Hebbian&&) noexcept;
    
    // Main Hebbian update function
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt);
    
    // Apply weight change with bounds
    void applyWeightChange(Synapse* synapse, SynapticWeight delta);
    
    // Getters/setters
    const char* getName() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    // Biological features
    void enableOjasRule(bool enable);
    bool isOjasRuleEnabled() const;
    void setCovarianceThreshold(float threshold);
    float getCovarianceThreshold() const;
    void setLearningRule(const std::string& ruleName);
    std::string getLearningRuleName() const;
    
    // Statistics
    size_t getTotalHebbianUpdates() const;
    float getAverageHebbianChange() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
