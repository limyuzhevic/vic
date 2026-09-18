// Real Synaptic Dynamics Module
// ==============================
// 
// This module implements realistic synaptic modeling with multiple plasticity rules.
// It goes beyond simple Hebbian learning to capture the complex dynamics of
// biological synapses, including short-term plasticity, eligibility traces,
// and reward-modulated learning.
// 
// Key Features:
// - Multiple plasticity rules (STDP, Hebbian, reward-modulated)
// - Short-term synaptic dynamics (facilitation/depression)
// - Eligibility traces for delayed reward signals
// - Synaptic efficacy tracking
// - Spike-timing dependent plasticity with realistic windows
// - Structural plasticity (synaptogenesis, pruning)
// 
// Biological Plausibility:
// - STDP with exponential learning windows based on spike timing
// - Short-term plasticity following Tsodyks-Markram model
// - Neuromodulation by dopamine and other signals
// - Spike-frequency adaptation in synaptic efficacy
// 
// Mathematical Models:
// - STDP: Δw = A+·exp(-Δt/τ+) for pre-before-post, Δw = -A-·exp(Δt/τ-) for post-before-pre
// - STP: Facilitation and depression dynamics with time constants
// - Eligibility traces: Decay exponential traces that integrate rewards
// - Weight normalization to maintain stability
// 
// Performance Optimizations:
// - Event-driven updates (only when spikes occur)
// - Vectorized operations where possible
// - Efficient spike history management
// - Memory pools for frequent allocations

// Plasticity Rules Implementation
// -------------------------------
// 
// 1. Spike-Timing Dependent Plasticity (STDP)
//    - Pre-before-post spikes → Long-Term Potentiation (LTP)
//    - Post-before-pre spikes → Long-Term Depression (LTD)
//    - Time window: Exponential decay with τ = 20ms
//    - Parameters: A+ = 0.01, A- = 0.012
// 
// 2. Hebbian Learning
//    - "Neurons that fire together, wire together"
//    - Correlated activity within 10ms window increases connection
//    - Learning rate adjustable via configuration
// 
// 3. Reward-Modulated STDP (R-STDP)
//    - Integrates reward prediction errors
//    - Eligibility traces determine when to apply reward
//    - Dopamine modulates plasticity strength
// 
// 4. Structural Plasticity
//    - Synaptogenesis: New connections form spontaneously
//    - Pruning: Weak connections are eliminated
//    - Activity-dependent with homeostatic regulation

// Synaptic State Variables
// ------------------------
// - weight: Connection strength (-1.0 to 1.0)
// - efficacy: Short-term efficacy (0.0 to 2.0)
// - eligibility_trace: Reward integration signal (-1.0 to 1.0)
// - facilitation: Short-term facilitation (0.0 to 1.0)
// - depression: Short-term depression (0.0 to 1.0)
// - last_pre_spike: Time of last pre-synaptic spike
// - last_post_spike: Time of last post-synaptic spike

// Stability Mechanisms
// ---------------------
// - Weight bounds prevent runaway excitation
// - Eligibility trace decay prevents infinite accumulation
// - Synaptic efficacy normalization
// - Homeostatic scaling of overall connectivity
// - Spike-frequency adaptation in neurons

// Integration with Learning Systems
// ----------------------------------
// - Works with neuromodulation (dopamine, curiosity, novelty)
// - Integrates with episodic memory for reward associations
// - Links to prediction error signals
// - Interfaces with development system for activity-dependent growth

// Testing and Validation
// ---------------------
// - STDP experiments verify LTP/LTD timing rules
// - STP experiments verify facilitation/depression dynamics
// - Reward-modulated learning tested with temporal difference tasks
// - Structural plasticity validated against developmental data
// - Network stability assessed through long-running simulations

// Experimental Protocols
// ----------------------
// 1. Timing Experiments: Verify STDP window measurements
// 2. Activity Experiments: Test Hebbian learning rules
// 3. Reward Experiments: Validate reward-modulated learning
// 4. Developmental Experiments: Test structural plasticity
// 5. Stability Experiments: Long-running simulation validation

// Research Applications
// ---------------------
// - Motor learning and adaptation
// - Memory formation and consolidation
// - Decision making under uncertainty
// - Skill acquisition and expertise
// - Social learning and imitation
// - Cognitive development

// Extension Possibilities
// ----------------------
// - Different plasticity rule variants
// - Multiple neuromodulators
// - Synaptic compartmentalization
// - Activity-dependent spine dynamics
// - Activity-dependent myelination
// - Network-level plasticity rules

// Code Organization
// -----------------
// - PlasticityRule.hpp: Abstract base class
// - STDP.hpp/.cpp: Spike-timing dependent plasticity
// - Hebbian.hpp/.cpp: Correlated activity learning
// - RewardModulatedSTDP.hpp/.cpp: Reward integration
// - StructuralPlasticity.hpp/.cpp: Connection growth/regression
// - Synapse.hpp/.cpp: Synaptic state and dynamics

// Future Enhancements
// ------------------
// - Multiple plasticity mechanisms co-occurring
// - Plasticity interactions between synapses
// - Learning in non-spiking neural models
// - Meta-plasticity (learning how to learn)
// - Episodic memory integration
// - Causal inference in learning
