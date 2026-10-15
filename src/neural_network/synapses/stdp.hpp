#pragma once

#include "synapse_base.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <array>

namespace nlm {

/**
 * Advanced STDP (Spike-Timing-Dependent Plasticity) implementation
 * 
 * Enhanced STDP with:
 * - Triplet interactions (pre-post-post, post-pre-pre)
 * - Normalization mechanisms
 * - Multiple time scales
 * - Neuromodulatory control
 * - Anti-Hebbian interactions
 * - Time-window asymmetric plasticities
 * 
 * Reference: Song et al. (2000), Abbott & Song (2000)
 */
class AdvancedSTDP : public EnhancedSynapse {
public:
    AdvancedSTDP(SynapseId id, NeuronId source, NeuronId destination);
    ~AdvancedSTDP() override;

    // ===== TRIPLET STDP =====

    // Enable triplet STDP (pre-post-post, post-pre-pre interactions)
    void enableTripletSTDP(bool enable) { tripletSTDP_ = enable; }
    bool isTripletSTDPEnabled() const { return tripletSTDP_; }

    // Triplet STDP parameters
    struct TripletSTDPParams {
        // Pairwise STDP (original implementation)
        float A_plus;            // LTP amplitude (positive)
        float A_minus;           // LTD amplitude (negative)
        float tau_plus;          // LTP time constant (ms)
        float tau_minus;         // LTD time constant (ms)
        
        // Triplet interactions
        float W_plus;            // Pre-post-post LTP
        float W_minus;           // Post-pre-pre LTD
        float tau_x;             // X trace time constant (ms)
        float tau_y;             // Y trace time constant (ms)
        
        // Normalization
        float W_max;             // Maximum weight
        float W_min;             // Minimum weight
        float normalization_tau; // Weight normalization time constant
    };

    void setTripletSTDPParams(const TripletSTDPParams& params) { 
        tripletParams_ = params; 
    }

    // ===== ANTI-HEBBIAN INTERACTIONS =====

    // Anti-Hebbian STDP (weight decrease for synchronous spiking)
    void enableAntiHebbian(bool enable) { antiHebbian_ = enable; }
    bool isAntiHebbianEnabled() const { return antiHebbian_; }

    struct AntiHebbianParams {
        float antiWeight;         // Anti-Hebbian weight change
        float antiTau;            // Anti-Hebbian time constant
        float antiThreshold;      // Threshold for anti-Hebbian effect
    };

    void setAntiHebbianParams(const AntiHebbianParams& params) { 
        antiHebbianParams_ = params; 
    }

    // ===== TIME-WINDOW ASYMMETRY =====

    // Time-window asymmetric STDP
    void enableTimeWindowAsymmetry(bool enable) { timeWindowAsym_ = enable; }
    bool isTimeWindowAsymmetryEnabled() const { return timeWindowAsym_; }

    struct TimeWindowAsymmetry {
        float asymmetryFactor;    // Time window asymmetry (0 = symmetric, >0 = asymmetric)
        float LTPWindow;          // LTP time window (ms)
        float LTDWindow;          // LTD time window (ms)
        float asymmetryTau;       // Asymmetry time constant (ms)
    };

    void setTimeWindowAsymmetryParams(const TimeWindowAsymmetry& params) { 
        timeWindowAsymParams_ = params; 
    }

    // ===== NEUROMODULATORY CONTROL =====

    // Neuromodulatory control of STDP
    struct NeuromodulatoryControl {
        float dopamineGain;       // Dopamine gain for STDP
        float acetylcholineGain;  // ACh gain for STDP
        float norepinephrineGain; // NE gain for STDP
        float serotoninGain;      // 5-HT gain for STDP
        float cortisolGain;        // Cortisol gain for STDP
    };

    void setNeuromodulatoryControl(const NeuromodulatoryControl& control) { 
        neuromodControl_ = control; 
    }

    // ===== MULTIPLE TIME SCALES =====

    // Multiple time scales for STDP
    struct MultiTimeScaleParams {
        std::vector<float> timeScales;      // Different time scales (ms)
        std::vector<float> timeScaleWeights; // Weights for each time scale
        float multiScaleTau;                // Multi-scale integration time constant
    };

    void setMultiTimeScaleParams(const MultiTimeScaleParams& params) { 
        multiTimeScaleParams_ = params; 
    }

    // ===== NORMALIZATION =====

    // Weight normalization to maintain stability
    void enableWeightNormalization(bool enable) { weightNormalization_ = enable; }
    bool isWeightNormalizationEnabled() const { return weightNormalization_; }

    struct NormalizationParams {
        float targetWeight;        // Target weight for normalization
        float normalizationRate;   // Rate of normalization (0-1)
        float homeostasisThreshold; // Threshold for homeostatic normalization
        float homeostaticTarget;   // Homeostatic target weight
    };

    void setNormalizationParams(const NormalizationParams& params) { 
        normalizationParams_ = params; 
    }

    // ===== TRACE IMPLEMENTATIONS =====

    // X trace (pre-synaptic activity)
    struct Trace {
        float value;           // Current trace value
        float decayRate;       // Trace decay rate (1/ms)
        float maxValue;        // Maximum trace value
        std::vector<float> history; // Trace history
        size_t maxHistorySize; // Maximum history size
    };

    // Pre-synaptic activity trace
    Trace preTrace_;
    // Post-synaptic activity trace  
    Trace postTrace_;
    // Triplet traces
    Trace xTrace_;           // Pre-post-pre trace
    Trace yTrace_;           // Post-pre-post trace

    // Initialize traces
    void initializeTraces(float decayRate = 0.01f, float maxValue = 1.0f);

    // Update traces
    void updatePreTrace(bool preSpike, float dt);
    void updatePostTrace(bool postSpike, float dt);
    void updateXTrace(bool preSpike, bool postSpike, float dt);
    void updateYTrace(bool preSpike, bool postSpike, float dt);

    // ===== CALCULATION METHODS =====

    // Calculate STDP weight change
    float calculateSTDPWeightChange(bool preSpike, bool postSpike,
                                   float preTime, float postTime, float dt) const;

    // Calculate triplet STDP contribution
    float calculateTripletContribution(bool preSpike, bool postSpike,
                                      float xValue, float yValue) const;

    // Calculate anti-Hebbian contribution
    float calculateAntiHebbianContribution(bool preSpike, bool postSpike,
                                          float synchronousMeasure) const;

    // Calculate time-window asymmetry contribution
    float calculateTimeWindowAsymmetryContribution(float deltaT) const;

    // Calculate neuromodulatory scaling
    float calculateNeuromodulatoryScaling() const;

    // Apply multiple time scales
    float applyMultiTimeScale(float weightChange, float deltaT) const;

    // Normalize weight
    void normalizeWeight(float currentWeight, float dt);

    // ===== LIFTS =====

    // Spike pairing statistics
    struct SpikePairStats {
        size_t totalPairs;          // Total spike pairs
        size_t symmetricPairs;      // Synchronous spike pairs
        size_t prePostPairs;        // Pre-then-post pairs
        size_t postPrePairs;        // Post-then-pre pairs
        float avgDeltaT;            // Average time difference
        float stdDevDeltaT;         // Standard deviation of time difference
    };

    SpikePairStats getSpikePairStats() const { return spikePairStats_; }

    // Update spike pairing statistics
    void updateSpikePairStats(bool preSpike, bool postSpike, float deltaT);

    // ===== UTILITY =====

    // Get current trace values
    float getPreTraceValue() const { return preTrace_.value; }
    float getPostTraceValue() const { return postTrace_.value; }
    float getXTraceValue() const { return xTrace_.value; }
    float getYTraceValue() const { return yTrace_.value; }

    // Set neuromodulatory levels
    void setNeuromodulatoryLevels(float dopamine, float acetylcholine,
                                 float norepinephrine, float serotonin, float cortisol);

    // ===== OVERRIDE =====

    // Override EnhancedSynapse::step
    void step(Timestamp currentTime) override {
        stepEnhanced(currentTime, 1.0f); // Default dt = 1.0ms
    }

    // Main enhanced STDP step
    bool stepEnhanced(Timestamp currentTime, TimestepDuration dt);

    // Apply STDP with all enhancements
    void applyAdvancedSTDP(bool preSpike, bool postSpike,
                          float preTime, float postTime, float dt);

private:
    // Configuration flags
    bool tripletSTDP_;
    bool antiHebbian_;
    bool timeWindowAsym_;
    bool weightNormalization_;

    // Parameters
    TripletSTDPParams tripletParams_;
    AntiHebbianParams antiHebbianParams_;
    TimeWindowAsymmetry timeWindowAsymParams_;
    NeuromodulatoryControl neuromodControl_;
    MultiTimeScaleParams multiTimeScaleParams_;
    NormalizationParams normalizationParams_;

    // Statistics
    SpikePairStats spikePairStats_;

    // Helper methods
    float calculatePairwiseSTDP(float deltaT) const;
    float calculateTripletEffect(bool preSpike, bool postSpike,
                                float xValue, float yValue) const;
    float calculateAntiHebbianEffect(bool preSpike, bool postSpike,
                                    float synchronousMeasure) const;
    float calculateAsymmetryEffect(float deltaT) const;
    float getNeuromodulatoryScalingFactor() const;
    float applyNormalization(float weight) const;
};

} // namespace nlm
