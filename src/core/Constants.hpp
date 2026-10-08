// Common constants for NLM neural simulation framework
// This centralizes magic numbers and makes them configurable

#pragma once

namespace nlm {

namespace Constants {
    // Basic physics and simulation constants
    constexpr float TIME_STEP = 0.001f;           // Default simulation timestep (ms)
    constexpr float MAX_POSITION = 100.0f;       // Maximum world position
    constexpr float MIN_POSITION = 0.0f;         // Minimum world position
    constexpr float WORLD_SIZE = 20.0f;           // Default world dimensions
    
    // Neuron and synaptic constants
    constexpr float RESTING_POTENTIAL = -65.0f;  // mV
    constexpr float THRESHOLD_POTENTIAL = -55.0f; // mV
    constexpr float MEMBRANE_CAPACITANCE = 1.0f;   // nF
    constexpr float MIN_WEIGHT = -1.0f;           // Minimum synaptic weight
    constexpr float MAX_WEIGHT = 1.0f;            // Maximum synaptic weight
    constexpr float MAX_EFFICACY = 1.0f;          // Maximum synaptic efficacy
    
    // Plasticity and learning constants
    constexpr float STDP_LTP_WEIGHT = 0.02f;      // STDP long-term potentiation weight
    constexpr float STDP_LTD_WEIGHT = 0.015f;     // STDP long-term depression weight
    constexpr float STDP_TIME_CONSTANT = 20.0f;   // STDP time constant (ms)
    constexpr float HEBBIAN_LEARNING_RATE = 0.01f; // Hebbian learning rate
    constexpr float HEBBIAN_MAX_WEIGHT = 1.0f;    // Hebbian max weight
    constexpr float HEBBIAN_MIN_WEIGHT = -1.0f;   // Hebbian min weight
    
    // Structural plasticity constants
    constexpr float SYNAPTOGENESIS_RATE = 0.0001f;  // Synaptogenesis rate
    constexpr float PRUNING_RATE = 0.00001f;         // Pruning rate
    constexpr float MAX_SYNAPSE_COUNT = 5000.0f;     // Maximum synapses per region
    
    // Neuromodulation and plasticity constants
    constexpr float DOPAMINE_EXCITABILITY_MOD = 0.5f;   // Dopamine modulation of excitability
    constexpr float PLASTICITY_MOD_BASE = 0.5f;          // Base plasticity modifier
    constexpr float PLASTICITY_MOD_RANGE = 0.5f;         // Range of plasticity modifier
    
    // Hebbian and STDP constants
    constexpr float SYNAPSE_WEIGHT_CHANGE_INCREMENT = 0.001f; // Synaptic weight change step
    
    // Development constants
    constexpr double DEVELOPMENT_STAGE_DURATION_INITIAL = 60.0;     // Initial stage duration (s)
    constexpr double DEVELOPMENT_STAGE_DURATION_CRITICAL = 300.0;   // Critical period duration (s)
    constexpr double DEVELOPMENT_STAGE_DURATION_MATURE = 900.0;     // Mature stage duration (s)
    constexpr float DEVELOPMENT_PLASTICITY_INITIAL = 1.0f;        // Initial plasticity modifier
    constexpr float DEVELOPMENT_PLASTICITY_CRITICAL = 0.8f;       // Critical period plasticity
    constexpr float DEVELOPMENT_PLASTICITY_MATURATION = 0.5f;     // Maturation plasticity
    constexpr float DEVELOPMENT_PLASTICITY_ADULT = 0.2f;          // Adult plasticity
    
    // Curiosity and exploration constants
    constexpr float NOVELTY_DECAY_RATE = 0.99f;        // Novelty decay per timestep
    constexpr float CURIOSITY_WEIGHT_NOVELTY = 2.0f;   // Curiosity weight on novelty
    constexpr float CURIOSITY_WEIGHT_PREDICTION = 0.5f; // Curiosity weight on prediction error
    constexpr float EXPLORATION_THRESHOLD = 0.5f;      // Curiosity threshold for exploration
    constexpr float EXPLORATION_CHANCE_MAX = 0.3f;    // Maximum exploration chance (30%)
    constexpr float CURIOSITY_CLAMP_MIN = 0.0f;        // Minimum curiosity level
    constexpr float CURIOSITY_CLAMP_MAX = 1.0f;        // Maximum curiosity level
    
    // Agent and behavior constants
    constexpr float AGENT_MOVE_SPEED = 3.0f;           // Movement speed
    constexpr float AGENT_TURN_SPEED = 2.0f;          // Turning speed
    constexpr float AGENT_INTERACTION_RANGE = 1.0f;   // Interaction range
    constexpr float AGENT_VISION_RANGE = 8.0f;        // Vision range
    constexpr float AGENT_VISION_FOV = 1.5708f;      // Field of view (90 degrees in radians)
    constexpr float AGENT_VISION_RAY_COUNT = 16.0f;   // Number of vision rays
    constexpr float AGENT_ENERGY_DECAY = 0.01f;       // Energy decay per timestep
    constexpr float AGENT_RESOURCE_ENERGY_GAIN = 30.0f; // Energy gained from resources
    constexpr float AGENT_HAZARD_DAMAGE = 0.2f;      // Damage from hazards
    constexpr float AGENT_PASSIVE_DAMAGE = 0.01f;     // Passive damage from hazards
    constexpr float AGENT_MAX_HEALTH = 1.0f;          // Maximum health
    constexpr float AGENT_HEALTH_DEGRADATION = 0.5f; // Health degradation rate
    
    // Memory and cognitive constants
    constexpr size_t MEMORY_CAPACITY_WORKING = 20;   // Working memory capacity
    constexpr size_t MEMORY_CAPACITY_EPISODIC = 1000; // Episodic memory capacity
    constexpr float MEMORY_DECAY_RATE = 0.9f;         // Memory decay rate
    constexpr float MEMORY_RECALL_THRESHOLD = 0.5f;   // Recall threshold
    constexpr size_t CONCEPT_GRID_SIZE = 10;         // Concept grid dimensions
    constexpr float CONCEPT_STABILITY_THRESHOLD = 0.8f; // Concept stability threshold
    constexpr float CONCEPT_ASSOCIATION_STRENGTH = 0.1f; // Association strength
    
    // Planning and decision constants
    constexpr float PLANNING_HORIZON = 10;           // Planning horizon
    constexpr float PLANNING_CONFIDENCE_MIN = 0.2f;  // Minimum planning confidence
    constexpr float PLANNING_CONFIDENCE_MAX = 1.0f;  // Maximum planning confidence
    constexpr float ACTION_SELECTION_THRESHOLD = 0.5f; // Activity threshold for action selection
    constexpr float ACTION_QUALITY_THRESHOLD = 0.5f; // Action quality threshold
    
    // Attention and cognitive control constants
    constexpr float ATTENTION_INHIBITION_STRENGTH = 0.5f;
    constexpr float ATTENTION_EXCITATION_STRENGTH = 1.5f;
    constexpr float PLANNING_DEPTH = 5;
    
    // Memory constants
    constexpr float MEMORY_RECALL_THRESHOLD = 0.5f;   // Recall threshold
    constexpr float MEMORY_CAPACITY_DEFAULT = 20;    // Default memory capacity
    
    // Plasticity constants
    constexpr float PLASTICITY_MOD_BASE = 0.5f;       // Base plasticity modifier
    
    // Testing and experiment constants
    constexpr float TEST_REWARD_SUCCESS = 1.0f;      // Reward for successful action
    constexpr float TEST_REWARD_FAILURE = -1.0f;     // Reward for failed action
    constexpr float TEST_REWARD_NEUTRAL = 0.0f;      // Neutral reward
    constexpr float TEST_THRESHOLD_HIGH = 0.8f;      // High threshold for success
    constexpr float TEST_THRESHOLD_LOW = 0.2f;       // Low threshold for failure
}

} // namespace nlm
