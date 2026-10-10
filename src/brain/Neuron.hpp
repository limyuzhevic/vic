// Neuron class for NLM - Enhanced with modern C++ patterns
#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Types.hpp"
#include "Random/Random.hpp"

namespace nlm {

class Neuron {
public:
    // Constructor and destructor
    Neuron(NeuronId id);
    ~Neuron();
    
    // Copy and move operations
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&& other) noexcept;
    Neuron& operator=(Neuron&& other) noexcept;
    
    // Basic neuron properties
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // Membrane potential and dynamics
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Firing properties
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    bool checkThreshold() const;
    
    // Firing state management
    NeuronState getFiringState() const;
    void setFiringState(NeuronState state);
    bool isFiring() const;
    bool isRefractory() const;
    
    // Refractory period
    void setRefractoryPeriod(uint32_t steps);
    uint32_t getRefractoryPeriod() const;
    void decrementRefractory();
    
    // Firing rate
    void setFiringRate(FiringRate rate);
    FiringRate getFiringRate() const;
    
    // Leak properties
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    
    // Resting and reset potentials
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    MembranePotential getResetPotential() const;
    
    // Adaptation
    void setAdaptationVariable(float adaptation);
    float getAdaptationVariable() const;
    
    // Last spike time
    float getLastSpikeTime() const;
    void setLastSpikeTime(float time);
    
    // Synaptic input and current
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    void injectCurrent(MembranePotential current);
    void clearTotalCurrent();
    MembranePotential getTotalCurrent() const;
    
    // Spike management
    void recordSpike(Timestamp timestamp);
    void clearSpikeHistory();
    const std::vector<Timestamp>& getSpikeHistory() const;
    size_t getSpikeCount() const;
    
    // Synapse management
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // Region and population
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Core neuron dynamics
    bool stepLIF(Timestamp currentTime, TimestepDuration dt = 0.001);
    void step(Timestamp currentTime);
    
    // Reset and initialization
    void reset();
    void initializeRandom(RandomGenerator& rng);
    
    // Utility methods
    std::string getNeuronDescription() const;
    bool isValid() const;
    
    // Observer pattern for spike events
    class SpikeObserver {
    public:
        virtual ~SpikeObserver() = default;
        virtual void onSpike(Neuron* neuron, Timestamp time) = 0;
    };
    
    void addSpikeObserver(std::shared_ptr<SpikeObserver> observer);
    void removeSpikeObserver(std::shared_ptr<SpikeObserver> observer);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Neuron() = default; // Internal only
    friend class Brain;
};

} // namespace nlm