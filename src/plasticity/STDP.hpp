#pragma once

#include "PlasticityRule.hpp"
#include <memory>
#include <chrono>
#include <numeric>
#include <algorithm>

namespace nlm {

// STDP Variant enumeration for flexible configuration
enum class STDPVariant {
    Standard,     // Classic STDP
    RewardModulated, // Reward-modulated STDP (R-STDP)
    Triplet       // Triplet STDP with higher-order interactions
};

// STDP Configuration structure for parameter management
struct STDPConfig {
    // Core STDP parameters
    float ltpWeight;           // A+ learning rate for potentiation (default: 0.01)
    float ltdWeight;           // A- learning rate for depression (default: 0.012)
    float tauPlus;             // Potentiation time constant τ+ (ms)
    float tauMinus;            // Depression time constant τ- (ms)
    float wMax;               // Maximum synaptic weight (default: 1.0)
    float wMin;               // Minimum synaptic weight (default: -1.0)
    
    // Variant-specific parameters
    float rewardBaseline;      // Baseline reward for R-STDP (default: 0.0)
    float rewardModGain;       // Reward modulation gain (default: 1.0)
    float predictionErrorGain; // Prediction error gain for learning rate adaptation
    float tripletGluingTime;   // Time window for triplet interactions (ms)
    
    // Time window configuration
    float timeWindowSize;      // Configurable time window for spike pairing (ms)
    float minPairingWindow;    // Minimum allowed pairing window (ms)
    float maxPairingWindow;    // Maximum allowed pairing window (ms)
    
    // Advanced features
    bool enableLearningRateAdaptation; // Adaptive learning rate based on prediction error
    bool enableEfficacyModulation;     // Use synaptic efficacy in STDP (default: true)
    bool enableEligibilityTraces;     // Maintain eligibility traces for reward modulation
    float eligibilityDecayRate;        // Decay rate for eligibility traces
    
    // Performance optimization
    bool useBatchProcessing;          // Enable batch processing for efficiency
    bool useParallelProcessing;       // Enable parallel spike pair calculations
    size_t maxBatchSize;              // Maximum batch size for parallel processing
    bool enableProfiling;             // Enable performance profiling
    
    STDPConfig()
        : ltpWeight(0.01f), ltdWeight(0.012f), tauPlus(20.0f), tauMinus(20.0f),
          wMax(1.0f), wMin(-1.0f),
          rewardBaseline(0.0f), rewardModGain(1.0f), predictionErrorGain(0.1f),
          tripletGluingTime(30.0f),
          timeWindowSize(50.0f), minPairingWindow(1.0f), maxPairingWindow(100.0f),
          enableLearningRateAdaptation(true), enableEfficacyModulation(true),
          enableEligibilityTraces(true), eligibilityDecayRate(0.01f),
          useBatchProcessing(true), useParallelProcessing(true), maxBatchSize(1024),
          enableProfiling(false) {}
};

// Advanced STDP implementation with support for multiple variants
class STDP : public PlasticityRule {
public:
    // Constructor with configuration
    explicit STDP(const STDPConfig& config = STDPConfig());
    ~STDP() override;
    
    // Update synaptic weights based on spike timing using efficient algorithms
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Apply weight change with advanced bounds checking and safety mechanisms
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // Core STDP parameter management
    void setLTPWeight(float weight);    // Set A+ learning rate
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Set A- learning rate  
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // Set STDP time constant
    float getTimeConstant() const;
    void setTimeConstants(float tauPlus, float tauMinus); // Set both time constants
    
    // Variant management
    void setVariant(STDPVariant variant);
    STDPVariant getVariant() const;
    
    // Configuration management
    void configure(const STDPConfig& config);
    void configure(float ltpWeight, float ltdWeight, float tauPlus, float tauMinus);
    STDPConfig getConfig() const;
    
    // Advanced feature management
    void enableLearningRateAdaptation(bool enable, float gain = 0.1f);
    void enableEfficacyModulation(bool enable);
    void enableEligibilityTraces(bool enable, float decayRate = 0.01f);
    void enableBatchProcessing(bool enable, size_t maxBatchSize = 1024);
    void enableParallelProcessing(bool enable);
    void enableProfiling(bool enable);
    
    // Performance profiling accessors
    float getLastUpdateTime() const;
    size_t getSpikePairCount() const;
    float getAverageUpdateTime() const;
    
    // Unit test support functions
    void setPredictionError(float error);
    void resetPredictionError();
    
private:
    // Internal implementation structure for PIMPL pattern
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private helper functions
    void validateParameters() const;
    void validateSpikeHistory(const std::vector<Timestamp>& spikes) const;
    float calculateLearningRate(float baseRate) const;
    void computeSTDPSpikePairs(Synapse* synapse,
                              const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt);
    void computeStandardSTDP(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes);
    void computeRewardModulatedSTDP(Synapse* synapse,
                                   const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes);
    void computeTripletSTDP(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes);
    void computeBatchSTDPSpikePairs(Synapse* synapse,
                                   const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   TimestepDuration dt);
    void updateEligibilityTrace(Synapse* synapse, float delta);
    void decayEligibilityTrace(Synapse* synapse, TimestepDuration dt);
};

// Reward-modulated STDP (R-STDP) - now fully implemented
class RewardModulatedSTDP : public PlasticityRule {
public:
    explicit RewardModulatedSTDP(const STDPConfig& config = STDPConfig());
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "R-STDP"; }
    
    // Reward modulation parameters
    void setRewardBaseline(float reward);
    void setRewardModGain(float gain);
    float getRewardBaseline() const;
    float getRewardModGain() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Spike-timing dependent plasticity with triplet interactions (triplet STDP)
// Fully implements triplet STDP with higher-order spike interactions
class TripletSTDP : public PlasticityRule {
public:
    explicit TripletSTDP(const STDPConfig& config = STDPConfig());
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "TripletSTDP"; }
    
    // Triplet STDP specific parameters
    void setGluingTime(float time);
    float getGluingTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
