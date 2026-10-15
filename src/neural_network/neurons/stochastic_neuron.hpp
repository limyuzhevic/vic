#pragma once

#include "neuron_base.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"
#include <random>

namespace nlm {

/**
 * Stochastic Neuron Model
 * 
 * Neuron with stochastic dynamics for:
 * - Membrane potential noise
 * - Spike timing variability
 * - Stochastic synaptic inputs
 * - Probabilistic spiking
 * 
 * Based on probabilistic neuron models and point process theory
 */
class StochasticNeuron : public EnhancedNeuron {
public:
    StochasticNeuron(NeuronId id);
    ~StochasticNeuron() override;

    // ===== STOCHASTIC PARAMETERS =====

    // Membrane potential stochasticity
    struct MembraneStochasticity {
        float mean;           // Mean of voltage noise (mV)
        float stddev;         // Standard deviation of noise (mV)
        float correlation;    // Correlation with past voltage (0-1)
        float dt;            // Time step for process
    };

    void setMembraneStochasticity(const MembraneStochasticity& params) { 
        membraneStochasticity_ = params; 
    }

    // Spike timing stochasticity
    struct SpikeTimingStochasticity {
        float baseRate;       // Base firing rate (Hz)
        float rateModulator;  // Rate modulation factor
        float refractoryNoise; // Refractory period noise
        float adaptationNoise; // Adaptation noise
    };

    void setSpikeTimingStochasticity(const SpikeTimingStochasticity& params) { 
        spikeTimingStochasticity_ = params; 
    }

    // Synaptic input stochasticity
    struct SynapticStochasticity {
        float inputMean;      // Mean synaptic input (pA)
        float inputStddev;    // Standard deviation of input
        float inputCorrelation; // Input correlation
        bool synapticNoise;   // Whether to add synaptic noise
    };

    void setSynapticStochasticity(const SynapticStochasticity& params) { 
        synapticStochasticity_ = params; 
    }

    // ===== PROBABILISTIC SPIKING =====

    // Probabilistic spike generation
    struct ProbabilisticSpiking {
        float spikeProbability;   // Probability of spiking in each time step
        float spikeThreshold;     // Voltage threshold for deterministic spiking
        float adaptationRate;     // Rate of probability adaptation
        float refractoryProb;     // Probability of spiking during refractory period
    };

    void setProbabilisticSpiking(const ProbabilisticSpiking& params) { 
        probabilisticSpiking_ = params; 
    }

    // ===== BURSTING MODEL =====

    // Stochastic bursting model
    struct BurstingModel {
        float burstProbability;    // Probability of initiating a burst
        float burstDuration;       // Mean burst duration (ms)
        float burstRate;           // Firing rate during burst (Hz)
        float interBurstInterval;  // Mean interval between bursts (ms)
        float adaptationDuringBurst; // Adaptation strength during burst
    };

    void setBurstingModel(const BurstingModel& params) { 
        burstingModel_ = params; 
    }

    // ===== CRITICALITY MODEL =====

    // Neuronal criticality (edge of chaos) parameters
    struct CriticalityModel {
        float criticalityLevel;    // 0 = subcritical, 1 = critical, >1 = supercritical
        float branchingRatio;      // Expected number of offspring spikes
        float avalancheSize;        // Mean avalanche size
        float avalancheDuration;   // Mean avalanche duration
    };

    void setCriticalityModel(const CriticalityModel& params) { 
        criticalityModel_ = params; 
    }

    // ===== RANDOM NUMBER GENERATORS =====

    // Setup random number generators
    void setupRandomGenerators(RandomGenerator& rng);

    // Generate stochastic membrane potential
    float generateStochasticMembranePotential(float dt) const;

    // Generate stochastic spike time
    bool generateStochasticSpike(float currentPotential, float dt) const;

    // Generate stochastic burst event
    bool generateBurstEvent(float dt) const;

    // Generate criticality-consistent dynamics
    float generateCriticalityEvent(float input, float dt) const;

    // ===== STATE TRANSITION =====

    // Stochastic state
    enum class StochasticState {
        RESTING,
        SUBTHRESHOLD,
        FIRING,
        BURSTING,
        REFRACTORY,
        ADAPTING
    };

    StochasticState getState() const { return state_; }
    void setState(StochasticState state) { state_ = state; }

    // ===== STATISTICS =====

    // Get spiking statistics
    struct SpikingStatistics {
        float meanRate;        // Mean firing rate (Hz)
        float cvOfISI;         // Coefficient of variation of ISI
        float firingProbability; // Probability of firing in time window
        float burstFrequency;  // Bursts per second
        float criticalityScore; // Measure of criticality (0-1)
    };

    SpikingStatistics getStatistics() const { return statistics_; }

    // Update statistics
    void updateStatistics(Timestamp currentTime);

    // ===== ENHANCED STEP =====

    // Main stochastic update function
    bool stepAdvanced(Timestamp currentTime, TimestepDuration dt);

    // ===== BACKWARD COMPATIBILITY =====

    // Override EnhancedNeuron::step
    void step(Timestamp currentTime) override {
        stepAdvanced(currentTime, 1.0f); // Default dt = 1.0ms
    }

private:
    // Random number generators
    std::mt19937 rng_;
    std::normal_distribution<float> voltageNoise_;
    std::uniform_real_distribution<float> spikeProbability_;
    std::exponential_distribution<float> burstDuration_;

    // State
    StochasticState state_;
    float noiseAccumulator_;
    float lastSpikeTime_;
    
    // Burst tracking
    int32_t burstCounter_;
    float burstStartTime_;

    // Statistics
    SpikingStatistics statistics_;
    Timestamp lastSpikeTimestamp_;

    // Helper methods
    float calculateDeterministicInput() const;
    float calculateProbabilisticThreshold() const;
    bool checkCriticalityCondition(float input, float dt) const;
    void updateBurstState(float currentPotential, Timestamp currentTime);
    void updateStochasticParameters(Timestamp currentTime);
    float calculateFiringProbability(float currentPotential, float dt) const;
};

} // namespace nlm
