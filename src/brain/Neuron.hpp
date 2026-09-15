#ifndef NLM_NEURON_HPP
#define NLM_NEURON_HPP

#include "../core/Types/Types.hpp"

namespace nlm {

class Neuron {
public:
    Neuron(NeuronId id, NeuronType type);
    ~Neuron();
    
    // Disable copying
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    // Enable moving
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Neuron identification
    NeuronId getId() const;
    NeuronType getType() const;
    
    // Membrane dynamics (LIF model)
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential v);
    
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    MembranePotential getRestingPotential() const;
    void setRestingPotential(MembranePotential v);
    
    MembranePotential getResetPotential() const;
    void setResetPotential(MembranePotential v);
    
    double getLeakConductance() const;
    void setLeakConductance(double g);
    
    // Firing state
    FiringState getFiringState() const;
    void setFiringState(FiringState state);
    
    // Refractory period
    Timestamp getRefractoryRemaining() const;
    void setRefractoryPeriod(Timestamp period);
    Timestamp getRefractoryPeriod() const;
    void setRefractoryRemaining(Timestamp remaining);
    
    // Spike management
    Timestamp getLastSpikeTime() const;
    void setLastSpikeTime(Timestamp t);
    bool isFiring() const;
    
    // Input and computation
    void injectCurrent(MembranePotential current);
    void stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Plasticity
    void recordPreSpike(Timestamp time);
    void recordPostSpike(Timestamp time);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    
    // Weight management
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // State
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_NEURON_HPP