#pragma once

#include "neuron_base.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"

namespace nlm {

/**
 * LIF Neuron with advanced features
 * 
 * Enhanced Leaky Integrate-and-Fire neuron with:
 * - Ion channel dynamics
 * - Stochastic current injection
 * - Adaptive exponential properties
 * - Bursting capabilities
 * - Dendritic integration
 */
class LIFNeuron : public EnhancedNeuron {
public:
    LIFNeuron(NeuronId id);
    ~LIFNeuron() override;

    // ===== LIF PARAMETERS =====

    // Standard LIF parameters
    void setStandardLIFParameters(float vRest = -70.0f, float vThresh = -55.0f,
                                  float vReset = -70.0f, float tau = 10.0f,
                                  uint32_t refractory = 5);

    // ===== ADAPTIVE EXPONENTIAL LIF =====

    // Enable adaptive exponential integrate-and-fire (AdEx) model
    void enableAdaptiveExponential(bool enable) { adexEnabled_ = enable; }
    bool isAdaptiveExponentialEnabled() const { return adexEnabled_; }

    // AdEx parameters
    struct AdExParameters {
        float delta_v;     // Spike threshold slope (mV/ms)
        float tau_mem;     // Membrane time constant (ms)
        float C_m;         // Membrane capacitance (pF)
        float I_e;         // Constant input current (pA)
    };

    void setAdExParameters(const AdExParameters& params) { adexParams_ = params; }
    const AdExParameters& getAdExParameters() const { return adexParams_; }

    // ===== STOCHASTIC LIF =====

    // Enable stochastic LIF dynamics
    void enableStochasticLIF(bool enable) { stochasticLIF_ = enable; }
    bool isStochasticLIFEnabled() const { return stochasticLIF_; }

    // Stochastic parameters
    struct StochasticLIFParams {
        float noiseAmplitude;     // Amplitude of voltage noise
        float noiseCorrelation;   // Correlation between neurons
        float thresholdNoise;     // Threshold fluctuation
        float resetNoise;         // Reset potential fluctuation
    };

    void setStochasticLIFParams(const StochasticLIFParams& params) { 
        stochasticLIFParams_ = params; 
    }

    // ===== BURSTING CAPABILITIES =====

    // Enable bursting behavior
    void enableBursting(bool enable) { burstingEnabled_ = enable; }
    bool isBurstingEnabled() const { return burstingEnabled_; }

    // Burst parameters
    struct BurstingParameters {
        float burstThreshold;     // Lower threshold for burst mode
        float interSpikeInterval; // Target ISI during burst (ms)
        float adaptationStrength; // How quickly adaptation builds during burst
        float refractoryBuster;   // How much burst mode reduces refractory period
    };

    void setBurstingParameters(const BurstingParameters& params) { 
        burstingParams_ = params; 
    }

    // Burst state
    struct BurstState {
        int32_t consecutiveSpikes;   // Number of spikes in current burst
        float lastISITime;           // Time since last spike in burst
        bool inBurst;               // Currently in burst mode
        float adaptationLevel;       // Current adaptation level
    };

    BurstState getBurstState() const { return burstState_; }

    // ===== DENDRITIC PROCESSING =====

    // Enable dendritic integration
    void enableDendriticProcessing(bool enable) { dendriticEnabled_ = enable; }
    bool isDendriticProcessingEnabled() const { return dendriticEnabled_; }

    // Dendritic processing parameters
    struct DendriticParameters {
        float dendriticTau;        // Dendritic time constant (ms)
        float dendriticDecay;      // Dendritic decay rate
        float dendriticGain;       // Synaptic weight scaling
        size_t dendriticBranches;  // Number of dendritic branches
    };

    void setDendriticParameters(const DendriticParameters& params) { 
        dendriticParams_ = params; 
    }

    // ===== RANDOMIZATION =====

    // Initialize neuron with random parameters from RNG
    void initializeRandom(RandomGenerator& rng, const std::string& distribution = "uniform");

    // ===== ENHANCED STEP =====

    // Enhanced LIF step with all advanced features
    bool stepAdvanced(Timestamp currentTime, TimestepDuration dt);

    // ===== BACKWARD COMPATIBILITY =====

    // Override EnhancedNeuron::step
    void step(Timestamp currentTime) override {
        stepAdvanced(currentTime, 1.0f); // Default dt = 1.0ms
    }

private:
    // Helper methods
    float computeStochasticInput(float dt) const;
    float computeDendriticContribution() const;
    bool checkBurstingConditions(float currentPotential, Timestamp currentTime);
    void updateAdaptation(Timestamp currentTime);

    // Configuration flags
    bool adexEnabled_;
    bool stochasticLIF_;
    bool burstingEnabled_;
    bool dendriticEnabled_;

    // Parameters
    AdExParameters adexParams_;
    StochasticLIFParams stochasticLIFParams_;
    BurstingParameters burstingParams_;
    DendriticParameters dendriticParams_;

    // State
    BurstState burstState_;
    float stochasticCurrent_;
};

} // namespace nlm
