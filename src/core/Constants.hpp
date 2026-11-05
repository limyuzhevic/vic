// Common constants for NLM neural network simulation
// This file provides named constants to replace magic numbers throughout the codebase

#pragma once

namespace nlm {

namespace Constants {
    // Simulation parameters
    constexpr double SIMULATION_TIMESTEP = 0.001;    // Time step for simulation in seconds
    constexpr size_t BASIC_CONNECTIVITY_TEST_STEPS = 50; // Number of steps for basic connectivity test
    
    // Neuron parameters
    constexpr float MEMBRANE_CAPACITANCE = 1.0f;     // nF - Membrane capacitance
    constexpr float TIME_CONSTANT = 20.0f;           // ms - Membrane time constant
    constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;  // mV - Minimum membrane potential
    constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;    // mV - Maximum membrane potential
    constexpr float MIN_THRESHOLD = -100.0f;         // mV - Minimum firing threshold
    constexpr float MAX_THRESHOLD = -20.0f;          // mV - Maximum firing threshold
    constexpr float MIN_RESTING_POTENTIAL = -100.0f; // mV - Minimum resting potential
    constexpr float MAX_RESTING_POTENTIAL = -20.0f;  // mV - Maximum resting potential
    constexpr float MIN_RESET_POTENTIAL = -100.0f;   // mV - Minimum reset potential
    constexpr float MAX_RESET_POTENTIAL = -20.0f;    // mV - Maximum reset potential
    constexpr float MIN_LEAK_CONDUCTANCE = 0.0f;     // nS - Minimum leak conductance
    constexpr float MAX_LEAK_CONDUCTANCE = 1000.0f;   // nS - Maximum leak conductance
    constexpr float MIN_CURRENT = -100.0f;           // Direct current injection minimum
    constexpr float MAX_CURRENT = 100.0f;            // Direct current injection maximum
    
    // Synaptic parameters
    constexpr float MIN_SYNAPTIC_WEIGHT = -1.0f;     // Minimum synaptic weight
    constexpr float MAX_SYNAPTIC_WEIGHT = 1.0f;      // Maximum synaptic weight
    constexpr float DEFAULT_SYNAPTIC_WEIGHT = 0.0f;  // Default synaptic weight
    
    // Time constants
    constexpr float STP_FACILITATION_TAU = 100.0f;   // ms - Short-term plasticity facilitation time constant
    constexpr float STP_DEPRESSION_TAU = 200.0f;     // ms - Short-term plasticity depression time constant
    constexpr float STP_U_MAX = 1.0f;                 // Maximum utilization factor for short-term plasticity
    
    // Simulation parameters
    constexpr size_t DEFAULT_MAX_SPIKE_HISTORY = 100; // Maximum number of spikes to store in history
    constexpr float DEFAULT_NOVITY_DECAY = 0.99f;     // Novelty decay factor
    constexpr float K_PREDICTION_ALPHA = 0.05f;       // Prediction adaptation rate
    
    // Learning parameters
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;  // STDP long-term potentiation weight
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f; // STDP long-term depression weight
    constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f; // STDP time constant for traces
    constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.01f; // Hebbian learning rate
    
    // Development parameters
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f; // Rate of new synapse formation
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;       // Rate of synapse elimination
    
    // Neuromodulation parameters
    constexpr float DEFAULT_DOPAMINE_BASELINE = 0.1f;      // Baseline dopamine level
    constexpr float MAX_DOPAMINE_LEVEL = 1.0f;             // Maximum dopamine level
    constexpr float MIN_DOPAMINE_LEVEL = -1.0f;            // Minimum dopamine level
    
    // Reward parameters
    constexpr float DEFAULT_REWARD_DISCOUNT_FACTOR = 0.99f; // Reward discount factor for temporal difference learning
    
    // Environment parameters
    constexpr float DEFAULT_INTERACTION_RADIUS = 1.0f;     // Default radius for agent-environment interaction
    constexpr float TOUCH_RANGE_THRESHOLD = 0.05f;        // Touch range threshold for detecting objects
    
    // Planning parameters
    constexpr float DEFAULT_PLAN_ACTION_QUALITY_THRESHOLD = 0.02f; // Action quality threshold for planning
    constexpr float PLANNER_BIAS_THRESHOLD = 0.15f;                   // Planner bias threshold for overriding motor decisions
    
    // Novelty and curiosity parameters
    constexpr float NOVELTY_THRESHOLD = 0.3f;                   // Novelty threshold for triggering curiosity
    constexpr float CURIOUS_EXPLORATION_RATE = 0.3f;             // Maximum exploration rate when fully curious
    constexpr float NOVELTY_SENSITIVITY = 2.0f;                   // Sensitivity of curiosity to novelty
    
    // Reflex parameters
    constexpr float INTERACT_REFLEX_BONUS = 20.0f;              // Bonus for interact action when resource detected
    constexpr float HAZARD_REFLEX_SUPPRESSION = 12.0f;          // Suppression for interact when hazard detected
    constexpr float TURN_REFLEX_BONUS = 12.0f;                  // Bonus for turning away from hazards
    constexpr float PHOTOTAXIS_BONUS = 10.0f;                   // Bonus for moving toward bright objects
    
    // Diversity and fatigue parameters
    constexpr int TURN_DIVERSITY_THRESHOLD = 25;                // Steps without turning before diversity bias
    constexpr float DIVERSITY_BONUS = 6.0f;                     // Bonus for turning when lacking diversity
    constexpr int REPEAT_COUNT_THRESHOLD = 3;                   // Repetitions before fatigue penalty
    constexpr float REPETITION_PENALTY = 8.0f;                  // Penalty for repeating actions
    constexpr float MOMENTUM_REVERSAL_PENALTY = 8.0f;           // Penalty for immediate direction reversal
}

} // namespace nlm
