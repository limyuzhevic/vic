#ifndef NLM_BRAIN_NEURON_HPP
#define NLM_BRAIN_NEURON_HPP

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>

namespace nlm {

struct NeuronState {
    MembranePotential membranePotential;
    MembranePotential restingPotential;
    MembranePotential threshold;
    MembranePotential resetPotential;
    float leakConductance;
    FiringState firingState;
    Timestamp lastSpikeTime;
    float refractoryRemaining;
    float refractoryPeriod;
    
    NeuronState();
};

struct PlasticityFlags {
    bool hebbian;
    bool stdp;
    bool structural;
    bool neuromodulated;
    
    PlasticityFlags();
    PlasticityFlags(bool h, bool s, bool str, bool n);
};

class Neuron {
public:
    static NeuronId generateId();
    
    Neuron(NeuronId id);
    ~Neuron();
    
    // Non-copyable
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    // Movable
    Neuron(Neuron&& other) noexcept;
    Neuron& operator=(Neuron&& other) noexcept;
    
    // Core neuron operations
    void stepLIF(Timestamp currentTime, Timestamp timestep);
    void injectCurrent(MembranePotential current);
    void receiveExcitatoryInput(MembranePotential current);
    void receiveInhibitoryInput(MembranePotential current);
    
    // State management
    const NeuronState& getState() const { return pImpl->state; }
    void setState(const NeuronState& state);
    
    // Identification
    NeuronId getId() const { return pImpl->id; }
    NeuronType getType() const { return pImpl->type; }
    void setType(NeuronType type) { pImpl->type = type; }
    
    // Location
    RegionId getRegionId() const { return pImpl->regionId; }
    void setRegionId(RegionId id) { pImpl->regionId = id; }
    
    PopulationId getPopulationId() const { return pImpl->populationId; }
    void setPopulationId(PopulationId id) { pImpl->populationId = id; }
    
    // Plasticity
    const PlasticityFlags& getPlasticityFlags() const { return pImpl->plasticityFlags; }
    void setPlasticityFlags(const PlasticityFlags& flags) { pImpl->plasticityFlags = flags; }
    void enablePlasticity(bool hebbian, bool stdp, bool structural, bool neuromodulated);
    
    // State properties
    MembranePotential getMembranePotential() const { return pImpl->state.membranePotential; }
    void setMembranePotential(MembranePotential potential) { pImpl->state.membranePotential = potential; }
    
    MembranePotential getThreshold() const { return pImpl->state.threshold; }
    void setThreshold(MembranePotential threshold) { pImpl->state.threshold = threshold; }
    
    MembranePotential getRestingPotential() const { return pImpl->state.restingPotential; }
    void setRestingPotential(MembranePotential potential) { pImpl->state.restingPotential = potential; }
    
    float getRefractoryRemaining() const { return pImpl->state.refractoryRemaining; }
    void setRefractoryRemaining(float time) { pImpl->state.refractoryRemaining = time; }
    
    float getRefractoryPeriod() const { return pImpl->state.refractoryPeriod; }
    void setRefractoryPeriod(float period) { pImpl->state.refractoryPeriod = period; }
    
    // Spike detection
    bool isFiring() const;
    Timestamp getLastSpikeTime() const { return pImpl->state.lastSpikeTime; }
    void recordSpike(Timestamp time);
    
    // Current injection
    MembranePotential getTotalCurrent() const { return pImpl->totalCurrent; }
    void clearTotalCurrent() { pImpl->totalCurrent = 0.0f; }
    
    // Memory for spike history
    const std::vector<Timestamp>& getSpikeHistory() const { return pImpl->spikeHistory; }
    
    // Synapse management
    const std::vector<SynapseHandle>& getIncomingSynapses() const { return pImpl->incomingSynapses; }
    const std::vector<SynapseHandle>& getOutgoingSynapses() const { return pImpl->outgoingSynapses; }
    
    // Utility methods
    std::string toString() const;
    
    // LIF neuron constants
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr float RESTING_POTENTIAL = -70.0f;  // mV
    static constexpr float THRESHOLD = -50.0f;  // mV
    static constexpr float RESET_POTENTIAL = -65.0f;  // mV
    static constexpr float REFRACTORY_PERIOD = 5.0f;  // ms
    static constexpr float LEAK_CONDUCTANCE = 0.1f;  // nS

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_BRAIN_NEURON_HPP
