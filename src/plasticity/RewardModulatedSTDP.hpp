// Reward-modulated STDP (R-STDP) implementation header
// 
// This file defines the RewardModulatedSTDP class which implements spike-timing
// dependent plasticity with reward modulation. This combines the temporal
// precision of STDP with the global reward signal to create a powerful
// learning mechanism that bridges temporal and value-based learning.
// 
// Reward-modulated STDP is inspired by the interaction between dopamine
// and synaptic plasticity in biological systems. Dopamine signals often
// modulate the efficacy of STDP, making learning more efficient and
// task-relevant.
// 
// Biological relevance:
// - Dopamine modulation of STDP in hippocampus
// - Reward prediction error signals affecting synaptic change
// - Reinforcement learning with temporal credit assignment
// - Neuromodulation of synaptic plasticity
// 
// Mathematical formulation:
// Δw = δ × STDP(t_post - t_pre)
// 
// Where:
// - δ = reward prediction error signal (dopamine)
// - STDP(t_post - t_pre) = standard STDP weight change based on timing
// 
// Key features:
// - Combines temporal precision of STDP with value signals
// - Enables credit assignment across time for delayed rewards
// - Implements biological reward-modulated learning
// - Allows teaching signals to modulate plasticity
// 
// Applications in NLM:
// - Reward-modulated learning for goal-directed behavior
// - Integration of prediction errors with spike timing
// - Neuromodulation-based credit assignment
// - Reinforcement learning with temporal structure

class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    // Update synaptic weights based on reward-modulated STDP
    // 
    // This method implements reward-modulated STDP:
    // 1. Compute standard STDP weight change based on spike timing
    // 2. Modulate by reward prediction error signal
    // 3. Apply modulated weight change
    // 
    // Parameters:
    // - synapse: The synapse to update
    // - preSpikes: Vector of pre-synaptic spike timestamps
    // - postSpikes: Vector of post-synaptic spike timestamps
    // - dt: Simulation time step
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Apply reward-modulated weight change with bounds
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get the name of this plasticity rule
    // 
    // Returns:
    // - String "R-STDP" identifying this rule type
    const char* getName() const override;
    
    // Reward modulation parameters
    
    // Reward modulation strength
    // - Controls how strongly reward signals affect plasticity
    // - Typical values: 0.1-1.0
    void setModulationStrength(float strength);
    float getModulationStrength() const;
    
    // Reward baseline (for calculating prediction error)
    // - Reference reward level for computing prediction error
    void setRewardBaseline(float baseline);
    float getRewardBaseline() const;
    
    // Eligibility trace decay rate
    // - Controls how quickly eligibility traces decay without reward
    // - Typical values: 0.9-0.999 per time step
    void setEligibilityTraceDecay(float decayRate);
    float getEligibilityTraceDecay() const;
    
    // Calculate reward prediction error
    // 
    // Parameters:
    // - reward: Actual reward received
    // - predicted: Predicted reward (expected value)
    // 
    // Returns:
    // - Reward prediction error (δ = reward - predicted)
    float calculatePredictionError(float reward, float predicted) const;
    
private:
    // Forward declaration of implementation details
    struct Impl;
    Impl* pImpl;  // Pointer to internal implementation
};