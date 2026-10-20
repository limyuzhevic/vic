#pragma once

// NLM Neuron Class - Modern C++ Implementation
// Replaces PIMPL pattern with clean, modern C++ design
// Implements Leaky Integrate-and-Fire (LIF) dynamics with improved error handling
// and modern C++20 features

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <stdexcept>
#include <cmath>
#include <limits>
#include <algorithm>

namespace nlm {

// Custom exception types for neuron operations
class NeuronInitializationError : public std::runtime_error {
public:
    explicit NeuronInitializationError(const std::string& msg) : std::runtime_error(msg) {}
};

class NeuronSimulationError : public std::runtime_error {
public:
    explicit NeuronSimulationError(const std::string& msg) : std::runtime_error(msg) {}
};

class NeuronPlasticityError : public std::runtime_error {
public:
    explicit NeuronPlasticityError(const std::string& msg) : std::runtime_error(msg) {}
};

// Forward declarations
class RandomGenerator;
class Synapse;
class SpikeSystem;

// Neuron configuration structure
struct NeuronConfiguration {
    // Membrane properties
    float restingPotential = -70.0f;
    float threshold = -55.0f;
    float resetPotential = -70.0f;
    float leakConductance = 10.0f;
    float membraneCapacitance = 1.0f;
    float timeConstant = 20.0f;  // ms
    
    // Firing properties
    uint32_t refractoryPeriod = 5;  // steps
    float adaptationStrength = 1.0f;
    
    // Plasticity configuration
    bool enableSTDP = false;
    bool enableHebbian = false;
    bool enableRewardModulation = false;
    float stdpLearningRate = 0.01f;
    float hebbianStrength = 1.0f;
    
    // Sensory/Motor properties
    NeuronType neuronType = NeuronType::Internal;
    float sensoryGain = 1.0f;
    float motorStrength = 1.0f;
    
    // Development properties
    float developmentFactor = 1.0f;
    float plasticityModulation = 1.0f;
    
    // Random initialization
    float restingPotentialRandomization = 3.0f;  // ±3mV
    float thresholdRandomization = 2.0f;       // ±2mV
    float resetPotentialRandomization = 5.0f;   // ±5mV
    
    // Stability parameters
    float voltageClampMin = -100.0f;
    float voltageClampMax = 50.0f;
    float maxSynapticInput = 100.0f;
    
    // Spike timing precision
    float spikeTimePrecision = 0.001f;  // ms
    size_t maxSpikeHistory = 100;
    
    // Constructor
    NeuronConfiguration() = default;
    
    // Validate configuration
    void validate() const {
        if (threshold <= restingPotential) {
            throw NeuronInitializationError("Neuron threshold must be above resting potential");
        }
        if (resetPotential > restingPotential) {
            throw NeuronInitializationError("Reset potential should be at or below resting potential");
        }
        if (timeConstant <= 0.0f) {
            throw NeuronInitializationError("Time constant must be positive");
        }
        if (refractoryPeriod == 0) {
            throw NeuronInitializationError("Refractory period cannot be zero");
        }
    }
    
    // Get random value within range
    float getRandomValue(float mean, float stddev) const {
        static thread_local std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::normal_distribution<float> distribution(mean, stddev);
        return distribution(generator);
    }
    
    // Get random value within uniform range
    float getRandomValue(float min, float max) const {
        static thread_local std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(generator);
    }
    
    // Get random integer within range
    int getRandomInt(int min, int max) const {
        static thread_local std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(generator);
    }
};

// Neuron state structure (enhanced version)
struct NeuronState {
    MembranePotential membranePotential;
    MembranePotential restingPotential;
    MembranePotential threshold;
    MembranePotential resetPotential;
    MembranePotential leakConductance;
    FiringRate firingRate;
    FiringState firingState;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    float adaptationVariable;
    float lastSpikeTime;
    float lastInputTime;
    float totalInput;
    float synapticConductance;
    size_t spikeCount;
    
    // Enhanced for better tracking
    std::vector<Timestamp> spikeHistory;
    std::vector<MembranePotential> inputHistory;
    std::chrono::steady_clock::time_point lastUpdateTime;
    
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(5)
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f)
        , lastInputTime(0.0f)
        , totalInput(0.0f)
        , synapticConductance(0.0f)
        , spikeCount(0)
        , lastUpdateTime(std::chrono::steady_clock::now())
    {
        spikeHistory.reserve(100);
        inputHistory.reserve(100);
    }
    
    // Calculate firing rate from spike history
    float calculateFiringRate(Timestamp currentTime) const {
        if (spikeHistory.size() < 2 || currentTime <= spikeHistory.front()) {
            return 0.0f;
        }
        float timeSpan = currentTime - spikeHistory.front();
        if (timeSpan <= 0.0f) return 0.0f;
        return (spikeHistory.size() - 1) / timeSpan * 1000.0f;  // Hz
    }
    
    // Check if neuron is in stable state
    bool isStable() const {
        return firingState == FiringState::Resting && 
               refractoryRemaining == 0 && 
               std::abs(membranePotential - restingPotential) < 1.0f;
    }
    
    // Get membrane potential with bounds checking
    MembranePotential getClampedPotential() const {
        return std::clamp(membranePotential, -100.0f, 50.0f);
    }
};

// Plasticity flags structure (enhanced)
struct PlasticityFlags {
    bool stdp = false;
    bool hebbian = false;
    bool rewardModulated = false;
    bool structural = false;
    bool neuromodulated = false;
    
    // Check if any plasticity is enabled
    bool anyEnabled() const {
        return stdp || hebbian || rewardModulated || structural || neuromodulated;
    }
    
    // Get total plasticity strength (for scaling)
    float getTotalStrength(float baseSTDP = 1.0f, float baseHebbian = 1.0f) const {
        float strength = 0.0f;
        if (stdp) strength += baseSTDP;
        if (hebbian) strength += baseHebbian;
        if (rewardModulated) strength += baseHebbian * 0.5f;
        if (structural) strength += 0.3f;
        if (neuromodulated) strength += 0.5f;
        return strength;
    }
};

// Synapse handle (lightweight reference)
class SynapseHandle {
public:
    SynapseId id;
    SynapseType type;
    float weight;
    float delay;
    bool isValid;
    
    SynapseHandle() : id(), type(SynapseType::Excitatory), weight(0.0f), delay(1), isValid(false) {}
    
    SynapseHandle(SynapseId synapseId, SynapseType synapseType, float w, float d)
        : id(synapseId), type(synapseType), weight(w), delay(d), isValid(true) {}
    
    bool isActive() const { return isValid; }
    void invalidate() { isValid = false; }
    
    // Comparison operators for container use
    bool operator==(const SynapseHandle& other) const { return id == other.id; }
    bool operator!=(const SynapseHandle& other) const { return id != other.id; }
    bool operator<(const SynapseHandle& other) const { return id < other.id; }
};

// Neuron class representing a single neuron
// Modern implementation with LIF dynamics, plasticity, and development
class Neuron {
public:
    // === Constructors and Destructor ===
    
    // Default constructor - creates uninitialized neuron
    Neuron();
    
    // Constructor with ID and configuration
    explicit Neuron(NeuronId id, const NeuronConfiguration& config = NeuronConfiguration());
    
    // Move constructor
    Neuron(Neuron&& other) noexcept;
    
    // Move assignment operator
    Neuron& operator=(Neuron&& other) noexcept;
    
    // Destructor
    ~Neuron() = default;
    
    // Disable copying (neural networks are complex)
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;

    // === Core Identity ===
    
    // Get neuron ID
    NeuronId getId() const;
    
    // Get neuron type
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // Get region/population membership
    RegionId getRegionId() const;
    void setRegionId(RegionId region);
    
    PopulationId getPopulationId() const;
    void setPopulationId(PopulationId population);

    // === Configuration ===
    
    // Get current configuration
    const NeuronConfiguration& getConfiguration() const;
    
    // Update configuration
    void updateConfiguration(const NeuronConfiguration& config);
    
    // Reset configuration to defaults
    void resetConfiguration();

    // === State Management ===
    
    // Get neuron state
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Get current membrane potential
    MembranePotential getMembranePotential() const;
    
    // Set membrane potential with validation
    void setMembranePotential(MembranePotential potential);
    
    // Add to membrane potential
    void addToMembranePotential(MembranePotential delta);
    
    // Get membrane potential with bounds checking
    MembranePotential getClampedPotential() const;

    // === Firing State ===
    
    // Check if neuron is firing
    bool isFiring() const;
    
    // Check if neuron is in refractory period
    bool isRefractory() const;
    
    // Get remaining refractory time
    uint32_t getRefractoryRemaining() const;
    
    // Set refractory period
    void setRefractoryPeriod(uint32_t steps);
    
    // Decrement refractory counter
    void decrementRefractory();
    
    // Set firing state
    void setFiringState(FiringState state);

    // === Threshold and Firing ===
    
    // Get firing threshold
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Check if neuron has reached firing threshold
    bool checkThreshold() const;
    
    // Get last spike time
    float getLastSpikeTime() const;
    
    // Check if neuron can spike (considering refractory period)
    bool canSpike(Timestamp currentTime) const;

    // === Input Processing ===
    
    // Process external input
    void injectCurrent(MembranePotential current);
    
    // Process synaptic input (excitatory)
    void receiveExcitatoryInput(MembranePotential amplitude);
    
    // Process synaptic input (inhibitory)
    void receiveInhibitoryInput(MembranePotential amplitude);
    
    // Process modulatory input (for plasticity)
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Get total current accumulated
    MembranePotential getTotalCurrent() const;
    
    // Clear accumulated current
    void clearTotalCurrent();
    
    // Add to total current
    void addToTotalCurrent(MembranePotential delta);

    // === Spike Management ===
    
    // Record a spike
    void recordSpike(Timestamp timestamp);
    
    // Clear spike history
    void clearSpikeHistory();
    
    // Get spike history
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    // Get spike count
    size_t getSpikeCount() const;
    
    // Check if neuron spiked recently
    bool spikedRecently(Timestamp currentTime, float maxAge = 10.0f) const;
    
    // Get time since last spike
    float timeSinceLastSpike(Timestamp currentTime) const;

    // === Synapse Management ===
    
    // Add incoming synapse
    void addIncomingSynapse(const SynapseHandle& handle);
    
    // Add outgoing synapse
    void addOutgoingSynapse(const SynapseHandle& handle);
    
    // Get incoming synapses
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    
    // Get outgoing synapses
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Remove all synapses
    void clearSynapses();

    // === Plasticity ===
    
    // Get plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // Enable specific plasticity types
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    void enableStructuralPlasticity(bool enable);
    
    // Get total plasticity strength
    float getPlasticityStrength() const;
    
    // Apply neuromodulation to plasticity
    void applyNeuromodulation(float level);

    // === LIF Dynamics ===
    
    // Step LIF neuron forward
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Step neuron for one simulation step (uses default timestep)
    void step(Timestamp currentTime);
    
    // Check if neuron should spike based on current state
    bool checkForSpike(Timestamp currentTime) const;
    
    // Handle spike event
    void handleSpike(Timestamp currentTime);
    
    // Reset membrane potential after spike
    void resetAfterSpike();

    // === Reset and Re-initialization ===
    
    // Reset neuron to initial state
    void reset();
    
    // Re-initialize with current configuration
    void reinitialize();
    
    // Initialize with random parameters
    void initializeRandom(RandomGenerator& rng);
    
    // Set initial state
    void setInitialState(const NeuronState& initialState);

    // === Statistics and Analysis ===
    
    // Get firing rate
    FiringRate getFiringRate(Timestamp currentTime) const;
    
    // Get membrane potential statistics
    struct PotentialStats {
        float min;
        float max;
        float mean;
        float stddev;
        float current;
    };
    
    PotentialStats getPotentialStats() const;
    
    // Get input statistics
    struct InputStats {
        float total;
        float average;
        float min;
        float max;
        size_t count;
    };
    
    InputStats getInputStats() const;
    
    // Get synaptic statistics
    struct SynapseStats {
        size_t incomingCount;
        size_t outgoingCount;
        float averageWeight;
        float maxWeight;
        float minWeight;
    };
    
    SynapseStats getSynapseStats() const;

    // === Development ===
    
    // Get developmental stage (0.0 to 1.0)
    float getDevelopmentalStage() const;
    
    // Set developmental stage
    void setDevelopmentalStage(float stage);
    
    // Apply development effects
    void applyDevelopment(float dt);
    
    // Check if neuron is mature
    bool isMature() const;
    
    // Get age (in simulation steps)
    size_t getAge() const;
    void incrementAge(size_t steps = 1);

    // === Utility Methods ===
    
    // Get neuron age in human-readable format
    std::string getAgeString() const;
    
    // Check if neuron is healthy
    bool isHealthy() const;
    
    // Get neuron status summary
    std::string getStatusSummary() const;
    
    // === Comparison and Debugging ===
    
    // Equality comparison
    bool equals(const Neuron& other) const;
    
    // Debug output
    std::string debugString() const;

    // === Internal Implementation ===
    
private:
    // Internal implementation details
    class Implementation;
    std::unique_ptr<Implementation> pImpl;
    
    // Internal constructor for implementation use
    explicit Neuron(std::unique_ptr<Implementation> impl) : pImpl(std::move(impl)) {}
    
    // Friend declarations for testing and internal access
    friend class NeuronFactory;
    friend class TestNeuron;
    friend class Brain;
};

// Neuron factory for advanced creation
class NeuronFactory {
public:
    // Create standard neuron
    static std::unique_ptr<Neuron> createStandard(NeuronId id);
    
    // Create sensory neuron
    static std::unique_ptr<Neuron> createSensory(NeuronId id, float gain = 1.0f);
    
    // Create motor neuron
    static std::unique_ptr<Neuron> createMotor(NeuronId id, float strength = 1.0f);
    
    // Create excitatory neuron
    static std::unique_ptr<Neuron> createExcitatory(NeuronId id);
    
    // Create inhibitory neuron
    static std::unique_ptr<Neuron> createInhibitory(NeuronId id);
    
    // Create with custom configuration
    static std::unique_ptr<Neuron> createCustom(NeuronId id, const NeuronConfiguration& config);
    
    // Create neuron from configuration and random seed
    static std::unique_ptr<Neuron> createRandom(NeuronId id, uint64_t seed = 42);
};

// Exception types for neuron operations
class NeuronConfigurationException : public std::runtime_error {
public:
    explicit NeuronConfigurationException(const std::string& msg) : std::runtime_error(msg) {}
};

class NeuronSimulationException : public std::runtime_error {
public:
    explicit NeuronSimulationException(const std::string& msg) : std::runtime_error(msg) {}
};

class NeuronPlasticityException : public std::runtime_error {
public:
    explicit NeuronPlasticityException(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper functions
namespace NeuronHelpers {
    // Create neuron configuration from external parameters
    NeuronConfiguration createConfigurationFromParams(const std::string& type, float scale = 1.0f);
    
    // Validate neuron state
    bool validateState(const NeuronState& state);
    
    // Calculate membrane potential dynamics
    MembranePotential calculateLIFPotential(
        MembranePotential V,
        MembranePotential V_rest,
        MembranePotential I_syn,
        float tau,
        float dt
    );
    
    // Calculate synaptic current
    MembranePotential calculateSynapticCurrent(
        const std::vector<MembranePotential>& conductances,
        const std::vector<float>& reversalPotentials
    );
    
    // Interpolate membrane potential
    MembranePotential interpolatePotential(
        MembranePotential start,
        MembranePotential end,
        float ratio
    );
    
    // Calculate firing rate from spike times
    FiringRate calculateFiringRate(
        const std::vector<Timestamp>& spikeTimes,
        Timestamp startTime,
        Timestamp endTime
    );
    
    // Check if neuron should adapt firing rate
    bool shouldAdaptFiringRate(
        const NeuronState& state,
        Timestamp currentTime,
        float adaptationThreshold = 10.0f
    );
}}

; // namespace nlm