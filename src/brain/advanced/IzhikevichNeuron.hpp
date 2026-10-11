#pragma once

#include "AdvancedNeuronModels.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <any>

namespace nlm {

class IzhikevichNeuron : public AdvancedNeuron {
public:
    IzhikevichNeuron(NeuronId id, const IzhikevichParameters& params);
    ~IzhikevichNeuron() override = default;
    
    bool step(Timestamp currentTime) override;
    MembranePotential getMembranePotential() const override;
    bool isFiring() const override;
    float getLastSpikeTime() const override;
    void reset() override;
    AdvancedNeuronModelType getModelType() const override;
    const std::any& getParameters() const override;
    void injectCurrent(MembranePotential current) override;
    void receiveSynapticInput(MembranePotential amplitude) override;
    void recordSpike(Timestamp timestamp) override;
    const std::vector<Timestamp>& getSpikeHistory() const override;
    
    // Model-specific methods
    void setParameters(const IzhikevichParameters& params);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
