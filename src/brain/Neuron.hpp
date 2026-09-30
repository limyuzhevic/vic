#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <algorithm>

namespace nlm {

// Neuron state structure
struct NeuronState {
    MembranePotential membranePotential;
    MembranePotential restingPotential;
    MembranePotential threshold;
    MembranePotential resetPotential;
    MembranePotential leakConductance;
    MembranePotential synapseConductance;
    FiringRate firingRate;
    FiringState firingState;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    float adaptationVariable;
    float lastSpikeTime;
    
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapseConductance(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

// Neuron class representing a single neuron
class Neuron {
public:
    explicit Neuron(NeuronId id);
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    NeuronId getId() const;
    
    // Type
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State access
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane potential
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing rate
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    MembranePotential getResetPotential() const;
    
    // Spike detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // Spike history
    void recordSpike(float time);
    float getSpikeCount() const;
    std::vector<Timestamp>& getSpikeHistory();
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    // Synaptic input
    void addSynapticInput(float input);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

