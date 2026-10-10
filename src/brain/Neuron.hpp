#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Neuron state structure for efficient storage
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;       // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapseConductance;   // Synaptic conductance (nS)
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
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

class Neuron {
public:
    /**
     * @brief Create neuron with unique identifier
     * @param id Unique neuron identifier
     */
    explicit Neuron(NeuronId id);

    /**
     * @brief Destructor
     */
    ~Neuron();

    /**
     * @brief Disable copying - neurons are non-copyable
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;

    /**
     * @brief Enable moving - neurons can be moved but not copied
     * @param other Other neuron to move from
     */
    Neuron(Neuron&& other) noexcept;
    Neuron& operator=(Neuron&& other) noexcept;

    /**
     * @brief Get the neuron's unique identifier
     * @return Neuron identifier
     */
    NeuronId getId() const;

    /**
     * @brief Get the neuron's type (excitatory, inhibitory, etc.)
     * @return Neuron type
     */
    NeuronType getType() const;
    
    /**
     * @brief Set the neuron's type
     * @param type New neuron type
     */
    void setType(NeuronType type);

    /**
     * @brief Get the neuron's current state
     * @return Current neuron state (const reference)
     */
    const NeuronState& getState() const;

    /**
     * @brief Get the neuron's current state (non-const reference for modification)
     * @return Current neuron state (reference)
     */
    NeuronState& getState();

    /**
     * @brief Get the neuron's membrane potential
     * @return Membrane potential in millivolts (mV)
     */
    MembranePotential getMembranePotential() const;

    /**
     * @brief Set the neuron's membrane potential
     * @param potential New membrane potential in mV
     */
    void setMembranePotential(MembranePotential potential);
    
    /**
     * @brief Add to the neuron's membrane potential
     * @param delta Value to add in mV
     */
    void addToMembranePotential(MembranePotential delta);

    /**
     * @brief Get the neuron's firing threshold
     * @return Threshold potential in mV
     */
    MembranePotential getThreshold() const;

    /**
     * @brief Set the neuron's firing threshold
     * @param threshold New threshold in mV
     */
    void setThreshold(MembranePotential threshold);

    /**
     * @brief Check if neuron is currently firing
     * @return true if neuron is in firing state
     */
    bool isFiring() const;

    /**
     * @brief Check if neuron is in refractory period
     * @return true if neuron cannot fire due to refractory period
     */
    bool isRefractory() const;

    /**
     * @brief Set the neuron's firing state
     * @param state New firing state
     */
    void setFiringState(FiringState state);
    
    /**
     * @brief Set the neuron's refractory period
     * @param steps Number of simulation steps the neuron will be refractory
     */
    void setRefractoryPeriod(uint32_t steps);

    /**
     * @brief Decrement refractory period by one step
     */
    void decrementRefractory();

    /**
     * @brief Get the neuron's firing rate
     * @return Firing rate in Hertz (Hz)
     */
    FiringRate getFiringRate() const;

    /**
     * @brief Set the neuron's firing rate
     * @param rate New firing rate in Hz
     */
    void setFiringRate(FiringRate rate);

    /**
     * @brief Set the leak conductance parameter for LIF dynamics
     * @param conductance Leak conductance in nS
     */
    void setLeakConductance(MembranePotential conductance);

    /**
     * @brief Get the neuron's leak conductance
     * @return Leak conductance in nS
     */
    MembranePotential getLeakConductance() const;

    /**
     * @brief Get the neuron's refractory period
     * @return Refractory period in simulation steps
     */
    uint32_t getRefractoryPeriod() const;

    /**
     * @brief Set the neuron's resting potential
     * @param potential Resting potential in mV
     */
    void setRestingPotential(MembranePotential potential);

    /**
     * @brief Get the neuron's resting potential
     * @return Resting potential in mV
     */
    MembranePotential getRestingPotential() const;

    /**
     * @brief Set the neuron's reset potential (post-spike)
     * @param potential Reset potential in mV
     */
    void setResetPotential(MembranePotential potential);

    /**
     * @brief Check if neuron has reached firing threshold
     * @return true if membrane potential >= threshold
     */
    bool checkThreshold() const;

    /**
     * @brief Get the timestamp of the last spike
     * @return Last spike time (-1 if no spikes recorded)
     */
    float getLastSpikeTime() const;

    /**
     * @brief Perform one step of LIF neuron dynamics
     * @param currentTime Current simulation timestamp in milliseconds
     * @param dt Timestep duration in milliseconds
     * @return true if neuron fired during this step
     */
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);

    /**
     * @brief Update neuron for one simulation step using default timestep
     * @param currentTime Current simulation timestamp in milliseconds
     */
    void step(Timestamp currentTime);

    /**
     * @brief Reset neuron to initial state
     */
    void reset();

    /**
     * @brief Initialize neuron with random parameters based on biological constraints
     * @param rng Random number generator for parameter initialization
     */
    void initializeRandom(RandomGenerator& rng);

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
