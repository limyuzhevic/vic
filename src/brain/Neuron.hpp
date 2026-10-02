#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include <functional>
#include <memory>

namespace nlm {

// Forward declarations
class Synapse;
class RandomGenerator;

// Enhanced NeuronState with additional metrics for advanced analysis
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
    
    // Advanced metrics for expert users
    float totalInput;                      // Total synaptic input accumulated
    float voltageDerivative;               // Rate of change of membrane potential
    float timeSinceLastSpike;              // Time since last spike
    float predictedNextSpike;              // Predicted next spike time (for rate-based neurons)
    uint32_t consecutiveSpikes;            // Count of consecutive spikes
    float averageVoltage;                  // Running average of membrane potential
    
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
        , lastSpikeTime(-1.0f)
        , totalInput(0.0f)
        , voltageDerivative(0.0f)
        , timeSinceLastSpike(0.0f)
        , predictedNextSpike(-1.0f)
        , consecutiveSpikes(0)
        , averageVoltage(0.0f) {}
};

// Enhanced Neuron class with improved lifecycle management and expert features
class Neuron {
public:
    // Create neuron with ID and optional type
    explicit Neuron(NeuronId id, NeuronType type = NeuronType::Internal);
    
    ~Neuron();
    
    // Disable copying, enable moving
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;
    
    // === IDENTITY & BASIC PROPERTIES ===
    
    // Neuron identity
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // Region/population membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // === ENHANCED STATE MANAGEMENT ===
    
    // Complete state access with validation
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Reset neuron to initial state with full cleanup
    void reset();
    
    // Initialize with random biological parameters
    void initializeRandom(RandomGenerator& rng, bool includeBiophysicalVariability = true);
    
    // === IMPROVED LIF DYNAMICS ===
    
    // Advanced LIF step with detailed metrics
    bool step(Timestamp currentTime, TimestepDuration dt = 0.001);
    
    // Basic threshold check with hysteresis support
    bool checkThreshold() const;
    bool checkThresholdWithHysteresis() const;
    
    // === INPUT & OUTPUT MANAGEMENT ===
    
    // Enhanced input handling with input history tracking
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Current injection with limits and clamping
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    
    // Spike history with enhanced tracking
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Advanced spike analysis
    size_t getRecentSpikeCount(Timestamp currentTime, TimestepDuration window) const;
    float getAverageInterSpikeInterval() const;
    float getFiringRate(TimestepDuration window) const;
    
    // === PLASTICITY MANAGEMENT ===
    
    // Enhanced plasticity flags with more granular control
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    void disablePlasticity();
    void setPlasticityFlag(PlasticityFlag flag, bool enabled);
    bool isPlastic(PlasticityFlag flag) const;
    
    // === NEUROPHYSIOLOGICAL PARAMETERS ===
    
    // Membrane potential control with validation
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Advanced membrane potential management
    void clampMembranePotential();           // Clamp to biologically plausible range
    bool isVoltageClampActive() const;
    void setVoltageClamp(bool active, MembranePotential minV, MembranePotential maxV);
    
    // LIF neuron parameters with biological defaults
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    void setMembraneCapacitance(float capacitance);
    float getMembraneCapacitance() const;
    void setTimeConstant(float timeConstant);
    float getTimeConstant() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    MembranePotential getResetPotential() const;
    void setThreshold(MembranePotential threshold);
    MembranePotential getThreshold() const;
    void setThresholdWithHysteresis(MembranePotential up, MembranePotential down);
    
    // === REFRACTORY PERIOD MANAGEMENT ===
    
    // Enhanced refractory period control
    bool isRefractory() const;
    uint32_t getRefractoryRemaining() const;
    uint32_t getRefractoryPeriod() const;
    void setRefractoryPeriod(uint32_t steps);
    void setAdaptiveRefractory(bool adaptive, float adaptationRate = 0.1f);
    void decrementRefractory();
    
    // === ADVANCED NEURAL DYNAMICS ===
    
    // Spike-frequency adaptation control
    void setAdaptationEnabled(bool enabled);
    void setAdaptationStrength(float strength);
    float getAdaptationStrength() const;
    float getAdaptationVariable() const;
    
    // Temperature compensation for LIF parameters
    void setTemperature(float celsius);
    float getTemperature() const;
    void applyTemperatureCompensation();
    
    // === SYNAPSE INTEGRATION ===
    
    // Enhanced synapse management
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    void removeIncomingSynapse(SynapseHandle handle);
    void removeOutgoingSynapse(SynapseHandle handle);
    size_t getIncomingSynapseCount() const;
    size_t getOutgoingSynapseCount() const;
    
    // === EXPERT FEATURES ===
    
    // Advanced neural configuration
    void configureForTask(NeuralTask task);
    void enableBurstMode(bool enabled, uint32_t burstFrequency = 100);
    void enableOscillatoryBehavior(bool enabled, float frequency = 10.0f);
    
    // Performance and monitoring
    void enablePerformanceMonitoring(bool enabled);
    float getAverageMembranePotential() const;
    float getMembranePotentialVariance() const;
    float getMembranePotentialMax() const;
    float getMembranePotentialMin() const;
    
    // External input processing
    void processExternalInput(float input, float weight = 1.0f);
    float getExternalInputHistory(size_t index = 0) const;
    
    // === SAFETY & STABILITY ===
    
    // Neuron health monitoring
    bool isHealthy() const;
    std::string getHealthStatus() const;
    void setHealthMonitor(std::function<bool(const Neuron&)> monitor);
    
    // Stability and convergence control
    bool isStable() const;
    float getStabilityMetric() const;
    void enableStabilityControl(bool enabled, float targetFiringRate = 10.0f);
    
    // === DEBUGGING & ANALYSIS ===
    
    // Advanced debugging features
    void enableDebugMode(bool enabled);
    void logState() const;
    void validateState() const;
    
    // Detailed spike analysis
    Timestamp getLastSpikeTime() const;
    float getLastSpikeInterval() const;
    std::vector<Timestamp> getSpikesInWindow(Timestamp start, Timestamp end) const;
    
    // Memory usage and optimization
    size_t getMemoryUsage() const;
    void optimizeMemoryUsage();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm