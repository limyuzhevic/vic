#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <stdexcept>
#include <limits>

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

// Neuron configuration structure
struct NeuronConfig {
    // LIF parameters
    float restingPotential = -70.0f;      // mV
    float threshold = -55.0f;           // mV
    float resetPotential = -70.0f;      // mV
    float leakConductance = 10.0f;      // nS
    float refractoryPeriod = 5.0f;      // ms (converted to steps)
    float timeConstant = 20.0f;         // ms
    float membraneCapacitance = 1.0f;   // nF
    
    // Plasticity parameters
    float adaptationRate = 0.01f;
    float homeostasisTarget = 0.0f;
    float synapticScalingFactor = 1.0f;
    
    // Initialization validation
    void validate() const {
        if (threshold <= restingPotential) {
            throw std::invalid_argument("Threshold must be greater than resting potential");
        }
        if (resetPotential > threshold) {
            throw std::invalid_argument("Reset potential must be less than or equal to threshold");
        }
        if (refractoryPeriod <= 0.0f) {
            throw std::invalid_argument("Refractory period must be positive");
        }
        if (timeConstant <= 0.0f) {
            throw std::invalid_argument("Time constant must be positive");
        }
        if (membraneCapacitance <= 0.0f) {
            throw std::invalid_argument("Membrane capacitance must be positive");
        }
    }
};

// Advanced plasticity flags
struct AdvancedPlasticityFlags {
    bool hebbian = true;                     // Standard Hebbian learning
    bool stdp = true;                       // Spike-timing dependent plasticity
    bool rewardModulated = false;          // Reward-modulated plasticity
    bool homeostatic = false;               // Homeostatical plasticity
    bool synapticScaling = false;           // Synaptic scaling
    bool intrinsicPlasticity = false;       // Intrinsic plasticity
    bool adaptiveThreshold = false;         // Adaptive threshold plasticity
    
    // Validate plasticity configuration
    void validate() const {
        if (!hebbian && !stdp && !rewardModulated && !homeostatic && 
            !synapticScaling && !intrinsicPlasticity && !adaptiveThreshold) {
            throw std::invalid_argument("At least one plasticity mechanism must be enabled");
        }
    }
};

// Reward-modulated STDP parameters
struct RewardModulatedSTDP {
    float stdpLtpWeight = 0.02f;            // STDP LTP weight
    float stdpLtdWeight = 0.015f;          // STDP LTD weight
    float stdpTau = 20.0f;                 // STDP time constant (ms)
    float rewardLtpScale = 1.0f;           // Reward modulation of LTP
    float rewardLtdScale = 0.5f;           // Reward modulation of LTD
    float eligibilityDecayRate = 0.01f;   // Eligibility trace decay rate
    float predictionErrorWeight = 0.1f;   // Weight of prediction error
    
    // Validate parameters
    void validate() const {
        if (stdpTau <= 0.0f) {
            throw std::invalid_argument("STDP time constant must be positive");
        }
        if (eligibilityDecayRate <= 0.0f || eligibilityDecayRate >= 1.0f) {
            throw std::invalid_argument("Eligibility decay rate must be in (0, 1)");
        }
    }
};

// Homeostatical plasticity parameters
struct HomeostaticalPlasticity {
    float targetFiringRate = 10.0f;        // Target firing rate (Hz)
    float adaptationRate = 0.001f;         // Adaptation rate
    float scalingFactor = 0.1f;           // Scaling factor
    float minWeight = 0.1f;                // Minimum synaptic weight
    float maxWeight = 1.0f;                // Maximum synaptic weight
    
    // Validate parameters
    void validate() const {
        if (targetFiringRate < 0.0f) {
            throw std::invalid_argument("Target firing rate must be non-negative");
        }
        if (adaptationRate <= 0.0f || adaptationRate >= 1.0f) {
            throw std::invalid_argument("Adaptation rate must be in (0, 1)");
        }
        if (scalingFactor <= 0.0f) {
            throw std::invalid_argument("Scaling factor must be positive");
        }
        if (minWeight < 0.0f || minWeight > maxWeight) {
            throw std::invalid_argument("Invalid weight bounds");
        }
    }
};

// Neuron class representing a single neuron
// Implements Leaky Integrate-and-Fire (LIF) dynamics with advanced plasticity
class Neuron {
public:
    // Configuration for neuron creation
    struct Config {
        NeuronId id;
        NeuronType type = NeuronType::Internal;
        NeuronConfig neuronConfig;
        AdvancedPlasticityFlags plasticityFlags;
        RewardModulatedSTDP rewardStdp;
        HomeostaticalPlasticity homeostatic;
        
        // Validate configuration
        void validate() const {
            neuronConfig.validate();
            plasticityFlags.validate();
            rewardStdp.validate();
            homeostatic.validate();
        }
    };
    
    // Create neuron with ID and configuration
    explicit Neuron(NeuronId id);
    explicit Neuron(const Config& config);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // Identity
    NeuronId getId() const;
    
    // Configuration access
    const NeuronConfig& getNeuronConfig() const;
    void setNeuronConfig(const NeuronConfig& config);
    
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
    
    // Firing rate (for rate-based computation)
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike detection
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF step function - returns true if neuron fired
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Incoming signals (post-synaptic potentials)
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection (from external sources)
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history (recent spikes for STDP)
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse management (incoming and outgoing)
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity state
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Advanced plasticity control
    const AdvancedPlasticityFlags& getAdvancedPlasticityFlags() const;
    void setAdvancedPlasticityFlags(const AdvancedPlasticityFlags& flags);
    
    const RewardModulatedSTDP& getRewardModulatedSTDP() const;
    void setRewardModulatedSTDP(const RewardModulatedSTDP& params);
    
    const HomeostaticalPlasticity& getHomeostaticalPlasticity() const;
    void setHomeostaticalPlasticity(const HomeostaticalPlasticity& params);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Update neuron for one simulation step
    // TODO PHASE 2: Implement real integrate-and-fire dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
    // Advanced methods
    void applyHomeostaticPlasticity(float currentFiringRate);
    void applySynapticScaling(const std::vector<float>& weights);
    void adaptThreshold(float error);
    void updateEligibilityTraces(float reward, float predictionError);
    
    // Statistics and monitoring
    float getFiringRate() const;  // Average firing rate
    size_t getTotalSpikeCount() const;  // Total spikes in history
    float getWeightHistorySum() const;  // Sum of synaptic weights
    bool isStable() const;  // Check if neuron is in stable state
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
