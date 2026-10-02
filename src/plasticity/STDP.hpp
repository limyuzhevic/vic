#pragma once

#include "PlasticityRule.hpp"
#include <string>
#include <vector>

namespace nlm {

// Advanced STDP (Spike-Timing-Dependent Plasticity) learning rules
// Implements multiple advanced STDP variants and neuromodulated learning

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Get module name
    const char* getName() const override;
    
    // Configure basic STDP parameters
    void configure(float ltpWeight, float ltdWeight, float tau) override;
    
    // Update synapse based on spike history (basic STDP)
    void update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Apply weight change with bounds checking
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Advanced STDP variants
    
    // Triplet STDP (compatible with biological findings)
    // Accounts for premotor and postmotor spikes for better biological accuracy
    void updateTriplet(STDP* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt);
    
    // Soft STDP with continuous weights
    // Uses sigmoidal functions for more realistic weight dynamics
    void updateSoft(STDP* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt);
    
    // Neuromodulated STDP with eligibility traces
    // Integrates reward signals for credit assignment
    void updateNeuromodulated(STDP* synapse,
                              const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes,
                              TimestepDuration dt,
                              float reward,
                              float predictionError);
    
    // Homeostatic plasticity (synaptic scaling)
    // Maintains synaptic balance and prevents runaway excitation/inhibition
    void applyHomeostaticPlasticity(STDP* synapse,
                                    const std::vector<Synapse*>& allSynapses,
                                    float targetExcitationInhibitionRatio = 1.0f);
    
    // Advanced plasticity with meta-learning
    // Adapts learning rates based on performance and uncertainty
    void updateMetaLearning(STDP* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt,
                            float performance,        // Recent performance metric
                            float uncertainty);        // Uncertainty estimate
    
    // Configuration for different STDP variants
    struct STDPParameters {
        // Basic parameters
        float ltpWeight = 0.01f;          // Potentiation amplitude
        float ltdWeight = 0.012f;         // Depression amplitude
        float tau = 20.0f;                 // Time constant (ms)
        
        // Advanced parameters
        float tripletU = 0.005f;           // Triplet potentiation parameter
        float tripletV = 0.005f;           // Triplet depression parameter
        float tripletW = 0.0025f;          // Triplet interaction parameter
        float softSigma = 0.5f;            // Soft STDP sigma parameter
        float homeostaticRate = 0.01f;    // Homeostatic plasticity rate
        float learningRate = 1.0f;         // Overall learning rate
        float adaptationRate = 0.001f;    // Meta-learning adaptation rate
        
        // Reward modulation parameters
        float rewardGain = 1.0f;           // Reward signal gain
        float eligibilityDecay = 0.1f;    // Eligibility trace decay
        float predictionErrorGain = 1.0f; // Prediction error signal gain
    };
    
    void setParameters(const STDPParameters& params);
    const STDPParameters& getParameters() const;
    
    // Biological accuracy parameters
    struct BiologicalParameters {
        float nmdaThreshold = -55.0f;     // NMDA receptor activation threshold (mV)
        float calciumPeak = 1.0f;        // Peak calcium concentration
        float calciumDecay = 0.1f;       // Calcium decay rate
        float backpropStrength = 0.3f;   // Backpropagation strength
        float localLearning = 1.0f;      // Local learning rule strength
    };
    
    void setBiologicalParameters(const BiologicalParameters& params);
    const BiologicalParameters& getBiologicalParameters() const;
    
    // Learning rate adaptation
    void adaptLearningRate(float performanceError, float confidence);
    float getCurrentLearningRate() const;
    
    // State management for advanced features
    struct LearningState {
        float cumulativeLearning;
        float performanceHistory;
        float uncertaintyEstimate;
        float lastAdaptationStep;
        std::vector<float> eligibilityTrace;
    };
    
    LearningState getLearningState() const;
    void updateLearningState(const LearningState& state);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Internal helper methods
    void calculateTripletContribution(std::vector<float>& contributions,
                                      const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes,
                                      TimestepDuration dt);
    
    void calculateNeuromodulatedEligibility(float& eligibility,
                                             const std::vector<Timestamp>& preSpikes,
                                             const std::vector<Timestamp>& postSpikes,
                                             TimestepDuration dt,
                                             float reward,
                                             float predictionError);
    
    void calculateHomeostaticAdjustment(float& adjustment,
                                         const std::vector<Synapse*>& allSynapses,
                                         float targetRatio);
    
    void updateMetaLearningParameters(float performanceError, float confidence);
};

// Reward-modulated STDP (R-STDP)
// Advanced learning that integrates reward signals
class RewardModulatedSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "R-STDP"; }
    
    // Reward modulation parameters
    void configure(float rewardGain, float eligibilityDecay, float predictionErrorGain);
    void setRewardPredictor(class RewardPredictor* predictor);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Biologically accurate model with premotor and postmotor spike contributions
class TripletSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "TripletSTDP"; }
    
    // Triplet interaction parameters
    void configure(float u, float v, float w);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Homeostatic plasticity for synaptic scaling
// Maintains network stability through synaptic scaling
class HomeostaticSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "HomeostaticSTDP"; }
    
    // Homeostatic parameters
    void configure(float targetEiRatio, float scalingRate, float adaptationRate);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm