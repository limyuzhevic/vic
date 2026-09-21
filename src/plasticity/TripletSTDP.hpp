// Triplet STDP (Spike-Timing-Dependent Plasticity with Triplet Interactions)
// 
// This file defines the TripletSTDP class which implements an enhanced
// STDP rule that incorporates triplet spike interactions. Unlike standard
// pair-wise STDP, triplet STDP includes facilitation and depression
// processes that depend on triplets of spikes (pre-pre-post and pre-post-post).
// 
// Biological inspiration:
// - Synaptic facilitation and depression dynamics
// - Calcium-based plasticity mechanisms
// - Complex spike timing dependencies in real neurons
// - Short-term plasticity mechanisms
// 
// Mathematical formulation (Brostowitz-style triplet STDP):
// 
// For each pre-synaptic spike:
// - Contributes to triplet potentiation if followed by post-synaptic spike
// - Depresses triplet depression trace if followed by another pre-synaptic spike
// 
// For each post-synaptic spike:
// - Contributes to triplet depression if preceded by pre-synaptic spike
// - Depresses triplet potentiation trace if followed by another post-synaptic spike
// 
// Final weight change:
// Δw = A× potentiation - B× depression
// 
// Key features:
// - Captures more complex spike timing dependencies
// - Implements synaptic resource dynamics
// - Provides better biological realism
// - Can produce metaplasticity effects
// 
// Applications in NLM:
// - Modeling real neuron synaptic dynamics
// - Capturing short-term plasticity interactions
// - Implementing advanced learning rules
// - Simulating calcium-based plasticity

class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    // Update synaptic weights based on triplet STDP dynamics
    // 
    // This method implements triplet STDP:
    // 1. Track recent pre-synaptic spike traces
    // 2. Track recent post-synaptic spike traces
    // 3. Update traces based on incoming spikes
    // 4. Compute weight change from trace interactions
    // 5. Apply change with biological bounds
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
    
    // Apply triplet STDP weight change with biological bounds
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get the name of this plasticity rule
    // 
    // Returns:
    // - String "TripletSTDP" identifying this rule type
    const char* getName() const override;
    
    // Triplet STDP parameters
    
    // Triplet potentiation amplitude (A+)
    // - Controls strength of pre-pre-post interactions
    // - Typical values: 0.01-0.1
    void setTripletPotentiation(float amplitude);
    float getTripletPotentiation() const;
    
    // Triplet depression amplitude (B-)
    // - Controls strength of pre-post-post interactions
    // - Typical values: 0.01-0.1
    void setTripletDepression(float amplitude);
    float getTripletDepression() const;
    
    // Potentiation trace time constant
    // - Controls decay of pre-synaptic facilitation trace
    // - Typical values: 100-500ms
    void setPotentiationTraceTimeConstant(float tau);
    float getPotentiationTraceTimeConstant() const;
    
    // Depression trace time constant
    // - Controls decay of post-synaptic depression trace
    // - Typical values: 100-500ms
    void setDepressionTraceTimeConstant(float tau);
    float getDepressionTraceTimeConstant() const;
    
    // Reset traces to initial state
    // - Useful for starting new learning episodes
    void resetTraces();
    
private:
    // Forward declaration of implementation details
    struct Impl;
    Impl* pImpl;  // Pointer to internal implementation
};