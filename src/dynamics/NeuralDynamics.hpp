#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <cmath>

namespace nlm {

// NeuralDynamics: Abstract base for neuron dynamics models
// Provides interface for different neuron dynamics implementations
class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Initialize with neuron parameters
    virtual void initialize(const NeuronState& params) = 0;
    
    // Update dynamics for one timestep
    virtual void update(Timestamp currentTime, TimestepDuration dt) = 0;
    
    // Get current membrane potential
    virtual MembranePotential getMembranePotential() const = 0;
    
    // Get firing state
    virtual FiringState getFiringState() const = 0;
    
    // Check if neuron should fire
    virtual bool shouldFire() const = 0;
    
    // Reset to resting state
    virtual void reset() = 0;
    
    // Get neuron type
    virtual NeuronType getType() const = 0;
};

// Integrate-and-Fire (LIF) neuron dynamics - most commonly used model
class LIFNeuralDynamics : public NeuralDynamics {
public:
    LIFNeuralDynamics();
    ~LIFNeuralDynamics() override;
    
    void initialize(const NeuronState& params) override;
    void update(Timestamp currentTime, TimestepDuration dt) override;
    MembranePotential getMembranePotential() const override;
    FiringState getFiringState() const override;
    bool shouldFire() const override;
    void reset() override;
    NeuronType getType() const override;
    
    // Additional LIF-specific methods
    void setThreshold(MembranePotential threshold);
    void setResetPotential(MembranePotential reset);
    void setLeakConductance(MembranePotential conductance);
    void setRestingPotential(MembranePotential resting);
    
    // Get firing rate
    FiringRate getFiringRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Hindmarsh-Rose neuron dynamics - for oscillatory activity
class FHNNeuralDynamics : public NeuralDynamics {
public:
    FHNNeuralDynamics();
    ~FHNNeuralDynamics() override;
    
    void initialize(const NeuronState& params) override;
    void update(Timestamp currentTime, TimestepDuration dt) override;
    MembranePotential getMembranePotential() const override;
    FiringState getFiringState() const override;
    bool shouldFire() const override;
    void reset() override;
    NeuronType getType() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm