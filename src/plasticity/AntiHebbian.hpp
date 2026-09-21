// Anti-Hebbian learning implementation header
// 
// This file defines the AntiHebbian class which implements activity-dependent
// synaptic depression based on correlated neural firing. Anti-Hebbian learning
// is essential for competitive learning, lateral inhibition, and stabilizing
// neural networks to prevent runaway excitation.
// 
// The "anti-Hebbian" principle means that when pre- and post-synaptic neurons
// fire together, the synaptic connection weakens rather than strengthens.
// This is the opposite of standard Hebbian learning ("fire together, wire together").
// 
// Biological relevance:
// - Cortical lateral inhibition circuits
// - Competitive learning in visual cortex
// - Stabilizing synaptic weights during development
// - Preventing epilepsy by balancing excitation and inhibition
// 
// Mathematical formulation:
// Δw = -η × (coincidence count - baseline)
// 
// Where:
// - η = learning rate parameter
// - coincidence count = number of correlated pre/post spike pairs
// - baseline = expected coincidences under independence
// 
// Applications in NLM:
// - Competitive learning between neuron populations
// - Implementing lateral inhibition for attention
// - Preventing runaway excitation in developing networks
// - Balancing excitation/inhibition ratios
// 
// TODO PHASE 2: Complete AntiHebbian implementation
// - Integrate with synaptic eligibility traces
// - Add spike-timing components
// - Implement with other plasticity rules for complementary learning

class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    // Update synaptic weights based on correlated pre/post spike activity
    // 
    // This method implements anti-Hebbian learning:
    // 1. Count spike coincidences within a temporal window  
    // 2. Compute weight change based on coincidence count (negative)
    // 3. Apply change with bounds checking
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
    
    // Apply weight change with bounds checking
    // 
    // Parameters:
    // - synapse: The synapse to modify
    // - delta: Weight change to apply (can be positive or negative)
    // 
    // Ensures weight stays within [minWeight, maxWeight] range
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get the name of this plasticity rule
    // 
    // Returns:
    // - String "AntiHebbian" identifying this rule type
    const char* getName() const override;
    
    // Learning rate configuration
    // 
    // The learning rate (η) determines how strongly correlated activity
    // affects synaptic depression. Typical values range from 0.001 to 0.1.
    // 
    // Parameters:
    // - rate: Learning rate (clamped to [0.0, 1.0])
    void setLearningRate(float rate);
    
    // Get current learning rate
    // 
    // Returns:
    // - Current learning rate parameter
    float getLearningRate() const;
    
private:
    // Forward declaration of implementation details
    // This uses the Pimpl idiom to hide implementation details
    struct Impl;
    Impl* pImpl;  // Pointer to internal implementation
};