#ifndef NLM_STRUCTURAL_PLASTICITY_HPP
#define NLM_STRUCTURAL_PLASTICITY_HPP

#include "../core/Types/Types.hpp"
#include "PlasticityRule.hpp"

namespace nlm {

class Brain;

class StructuralPlasticity : public PlasticityRule {
public:
    StructuralPlasticity();
    ~StructuralPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Structural plasticity parameters
    void setSynaptogenesisRate(float rate);
    float getSynaptogenesisRate() const;
    
    void setPruningRate(float rate);
    float getPruningRate() const;
    
    void setMaxConnections(float max);
    float getMaxConnections() const;
    
    // Update structural connectivity based on activity
    void updateStructure(Brain* brain, const RandomGenerator& rng);
    
    // Check if structural plasticity is enabled
    bool isEnabled() const override { return enabled_; }
    void setEnabled(bool enabled) override { enabled_ = enabled; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    bool enabled_;
};

} // namespace nlm

#endif // NLM_STRUCTURAL_PLASTICITY_HPP