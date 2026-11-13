#pragma once

#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// FitzHugh-Nagumo dynamics (simplified兴奋性模型)
// TODO PHASE 2: Implement FHN dynamics
class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    FitzHughNagumoDynamics();
    ~FitzHughNagumoDynamics() override;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override;
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override;
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override;
    bool shouldFire(const Neuron* neuron) const override;
    void reset() override;
    
    // FHN parameters
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    void setThreshold(float threshold);
    float getThreshold() const;
    void setResetValue(float resetValue);
    float getResetValue() const;
    void setConductance(float g);
    float getConductance() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
