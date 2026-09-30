// Neuron.hpp

#ifndef NLM_NEURON_HPP
#define NLM_NEURON_HPP

#include <string>
#include <memory>
#include <vector>
#include <cstdint>
#include "../core/Types/Types.hpp"

namespace nlm {

class Neuron {
private:
    struct Impl;
    Impl* pImpl;

public:
    Neuron(NeuronId id);
    ~Neuron();
    
    // Prevent copy operations
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    // Move operations
    Neuron(Neuron&& other) noexcept;
    Neuron& operator=(Neuron&& other) noexcept;

    // ========== IDENTIFICATION ==========
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);

    // ========== STATE MANAGEMENT ==========
    NeuronState getState() const;
    void setState(const NeuronState& state);
    
    // Membrane potential properties
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    
    MembranePotential getRestingPotential() const;
    void setRestingPotential(MembranePotential potential);
    
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    MembranePotential getResetPotential() const;
    void setResetPotential(MembranePotential potential);
    
    float getLeakConductance() const;
    void setLeakConductance(float conductance);
    
    float getMembraneCapacitance() const;
    
    float getTimeConstant() const;

    // ========== FIRING STATE ==========
    FiringState getFiringState() const;
    void setFiringState(FiringState state);
    
    float getRefractoryRemaining() const;
    void setRefractoryRemaining(float time);
    
    float getRefractoryPeriod() const;
    void setRefractoryPeriod(float period);
    
    Timestamp getLastSpikeTime() const;
    void setLastSpikeTime(Timestamp time);
    
    bool isFiring() const;

    // ========== INPUTS ==========
    void injectCurrent(MembranePotential current);
    void receiveExcitatoryInput(MembranePotential current);
    void receiveInhibitoryInput(MembranePotential current);
    
    float getTotalCurrent() const;
    float getSynapticInput() const;
    void clearInputs();

    // ========== DYNAMICS ==========
    void stepLIF(Timestamp currentTime, TimestepDuration dt);

    // ========== SPIKE HANDLING ==========
    void recordSpike(Timestamp time);
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    // ========== PLASTICITY ==========
    PlasticityFlags getPlasticityFlags() const;
    void setPlasticityFlags(const PlasticityFlags& flags);
    
    // ========== REGION/POPULATION ==========
    RegionId getRegionId() const;
    void setRegionId(RegionId id);
    
    PopulationId getPopulationId() const;
    void setPopulationId(PopulationId id);

    // ========== STATISTICS ==========
    float getAverageFiringRate() const;
    size_t getSpikeCount() const;

    // ========== DEBUG/UTILITY ==========
    std::string debugInfo() const;
};

} // namespace nlm

#endif // NLM_NEURON_HPP