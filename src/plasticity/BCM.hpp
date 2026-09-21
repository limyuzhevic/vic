// Bienenstock-Cooper-Munro (BCM) rule implementation header
// 
// This file defines the BCMRule class which implements the BCM theory of
// synaptic plasticity. BCM learning introduces a dynamic threshold (θ) that
// adapts based on post-synaptic activity, enabling selective potentiation
// and depression based on whether firing is above or below this threshold.
// 
// BCM theory is a biologically-inspired model that explains how neurons
// maintain stable synaptic weights while still being capable of learning.
// It addresses the problem of runaway potentiation in purely Hebbian networks
// by introducing activity-dependent threshold regulation.
// 
// Biological relevance:
// - Synaptic homeostasis (systems that maintain stability)
// - BCM theory in hippocampal neurons
// - Theta-neuron models
// - Regulation of synaptic strength during development
// 
// Mathematical formulation:
// If V_post > θ: Δw = η × (V_post - θ) × pre_activity  (potentiation)
// If V_post < θ: Δw = -η × (V_post - θ) × pre_activity  (depression)
// 
// Where:
// - V_post = post-synaptic membrane potential
// - θ = dynamic threshold (adapts to mean post-synaptic activity)
// - η = learning rate
// - pre_activity = pre-synaptic activity
// 
// Key features:
// - Activity-dependent threshold regulation
// - Stable synaptic weights around a setpoint
// - Enables both potentiation and depression
// - Self-organizing network dynamics
// 
// Applications in NLM:
// - Synaptic homeostasis and weight stabilization
// - Preventing runaway excitation in large networks
// - Activity-regulated learning during development
// - Implementing BCM-like mechanisms in artificial neural systems

class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    // Update synaptic weights based on BCM learning rule
    // 
    // This method implements the BCM rule:
    // 1. Track post-synaptic activity to maintain dynamic threshold
    // 2. Compare pre-synaptic activity with threshold
    // 3. Apply potentiation if above threshold, depression if below
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
    
    // Apply weight change with BCM-specific bounds
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get the name of this plasticity rule
    // 
    // Returns:
    // - String "BCM" identifying this rule type
    const char* getName() const override;
    
    // BCM parameters
    
    // BCM threshold parameter (θ)
    // - Controls the activity level at which synaptic changes switch
    // - Typically around 1.0-2.0 for normalized activity
    void setThreshold(float threshold);
    float getThreshold() const;
    
    // BCM learning rate parameter (η)
    // - Controls strength of weight changes
    // - Typical values: 0.001-0.01
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // BCM time constant for threshold adaptation
    // - Controls how quickly the threshold adapts to activity changes
    // - Typical values: 100-1000ms
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
private:
    // Forward declaration of implementation details
    struct Impl;
    Impl* pImpl;  // Pointer to internal implementation
};