#pragma once

namespace nlm {

class NeuronProperty {
public:
    // Constructor/destructor
    explicit NeuronProperty(NeuronId id);
    ~NeuronProperty();
    
    // Copy/move disabled
    NeuronProperty(const NeuronProperty&) = delete;
    NeuronProperty& operator=(const NeuronProperty&) = delete;
    NeuronProperty(NeuronProperty&&) noexcept;
    NeuronProperty& operator=(NeuronProperty&&) noexcept;
    
    // Basic property getters/setters
    void setType(NeuronType type);
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    void setThreshold(MembranePotential threshold);
    void setFiringState(FiringState state);
    void decrementRefractory();
    void setFiringRate(FiringRate rate);
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    void setRefractoryPeriod(uint32_t steps);
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // State access
    const NeuronState& getState() const;
    NeuronState& getState();
    
private:
    struct Impl;
    Impl* pImpl;
};

}