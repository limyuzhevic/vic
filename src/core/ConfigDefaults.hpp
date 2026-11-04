// Configuration constants and defaults
#pragma once

namespace nlm {

namespace ConfigDefaults {
    // Brain configuration
    constexpr const char* BRAIN_NEURON_COUNT = "brain.neuron_count";
    constexpr const char* BRAIN_SYNAPSE_DENSITY = "brain.synapse_density";
    constexpr const char* BRAIN_CONNECTION_PROBABILITY = "brain.connection_probability";
    constexpr const char* BRAIN_INITIAL_WEIGHT_MEAN = "brain.initial_weight_mean";
    constexpr const char* BRAIN_INITIAL_WEIGHT_STD = "brain.initial_weight_std";
    constexpr const char* BRAIN_V_THRESH = "brain.v_thresh";
    constexpr const char* BRAIN_V_REST = "brain.v_rest";
    constexpr const char* BRAIN_V_RESET = "brain.v_reset";
    constexpr const char* BRAIN_TAU_MEM = "brain.tau_mem";
    constexpr const char* BRAIN_TAU_REF = "brain.tau_ref";

    // Plasticity configuration
    constexpr const char* PLASTICITY_STDP_ENABLE = "plasticity.stdp.enable";
    constexpr const char* PLASTICITY_STDP_LEARNING_RATE = "plasticity.stdp.learning_rate";
    constexpr const char* PLASTICITY_STDP_TAU_PLUS = "plasticity.stdp.tau_plus";
    constexpr const char* PLASTICITY_STDP_TAU_MINUS = "plasticity.stdp.tau_minus";
    constexpr const char* PLASTICITY_HEBBIAN_ENABLE = "plasticity.hebbian.enable";
    constexpr const char* PLASTICITY_HEBBIAN_THRESHOLD = "plasticity.hebbian.threshold";
    constexpr const char* PLASTICITY_STRUCTURAL_ENABLE = "plasticity.structural.enable";

    // Neuromodulation configuration
    constexpr const char* NEUROMOD_DOPAMINE_SCALE = "neuromod.dopamine.scale";
    constexpr const char* NEUROMOD_CURiosity_ENABLE = "neuromod.curiosity.enable";
    constexpr const char* NEUROMOD_NOVELTY_ENABLE = "neuromod.novelty.enable";

    // Memory configuration
    constexpr const char* MEMORY_WORKING_CAPACITY = "memory.working.capacity";
    constexpr const char* MEMORY_EPISODIC_MAX_EPISODES = "memory.episodic.max_episodes";
    constexpr const char* MEMORY_ASSOCIATIVE_MAX_ASSOCIATIONS = "memory.associative.max_associations";

    // Prediction configuration
    constexpr const char* PREDICTION_ALPHA = "prediction.alpha";
    constexpr const char* PREDICTION_SURPRISE_THRESHOLD = "prediction.surprise_threshold";

    // Cognition configuration
    constexpr const char* COGNITION_PLANNING_DEPTH = "cognition.planning_depth";
    constexpr const char* COGNITION_ATTENTION_STRENGTH = "cognition.attention_strength";
    constexpr const char* COGNITION_CONCEPT_FORMATION_THRESHOLD = "cognition.concept_formation_threshold";

    // Development configuration
    constexpr const char* DEVELOPMENT_INITIAL_AGE = "development.initial_age";
    constexpr const char* DEVELOPMENT_CRITICAL_PERIOD_END = "development.critical_period_end";

    // Simulation configuration
    constexpr const char* SIMULATION_TIMESTEP = "simulation.timestep";
    constexpr const char* SIMULATION_STEPS = "simulation.steps";
    constexpr const char* RANDOM_SEED = "random.seed";

    // Performance configuration
    constexpr const char* PERFORMANCE_CHECKPOINT_DIR = "performance.checkpoint_dir";
    constexpr const char* PERFORMANCE_CHECKPOINT_INTERVAL = "performance.checkpoint_interval";
    constexpr const char* PERFORMANCE_REPLAY_INTERVAL = "performance.replay_interval";
    constexpr const char* PERFORMANCE_CONSOLIDATION_INTERVAL = "performance.consolidation_interval";

    // Default values
    constexpr int BRAIN_DEFAULT_NEURON_COUNT = 1000;
    constexpr float BRAIN_DEFAULT_SYNAPSE_DENSITY = 0.1f;
    constexpr float BRAIN_DEFAULT_CONNECTION_PROBABILITY = 0.05f;
    constexpr float BRAIN_DEFAULT_INITIAL_WEIGHT_MEAN = 0.5f;
    constexpr float BRAIN_DEFAULT_INITIAL_WEIGHT_STD = 0.1f;
    constexpr float BRAIN_DEFAULT_V_THRESH = -50.0f;
    constexpr float BRAIN_DEFAULT_V_REST = -70.0f;
    constexpr float BRAIN_DEFAULT_V_RESET = -75.0f;
    constexpr float BRAIN_DEFAULT_TAU_MEM = 20.0f;
    constexpr float BRAIN_DEFAULT_TAU_REF = 2.0f;

    constexpr bool PLASTICITY_DEFAULT_STDP_ENABLE = true;
    constexpr float PLASTICITY_DEFAULT_STDP_LEARNING_RATE = 0.001f;
    constexpr float PLASTICITY_DEFAULT_STDP_TAU_PLUS = 20.0f;
    constexpr float PLASTICITY_DEFAULT_STDP_TAU_MINUS = 20.0f;
    constexpr bool PLASTICITY_DEFAULT_HEBBIAN_ENABLE = true;
    constexpr float PLASTICITY_DEFAULT_HEBBIAN_THRESHOLD = 0.01f;
    constexpr bool PLASTICITY_DEFAULT_STRUCTURAL_ENABLE = true;

    constexpr float NEUROMOD_DEFAULT_DOPAMINE_SCALE = 1.0f;
    constexpr bool NEUROMOD_DEFAULT_CURIOUSITY_ENABLE = true;
    constexpr bool NEUROMOD_DEFAULT_NOVELTY_ENABLE = true;

    constexpr int MEMORY_DEFAULT_WORKING_CAPACITY = 100;
    constexpr int MEMORY_DEFAULT_MAX_EPISODES = 1000;
    constexpr int MEMORY_DEFAULT_MAX_ASSOCIATIONS = 1000;

    constexpr float PREDICTION_DEFAULT_ALPHA = 0.05f;
    constexpr float PREDICTION_DEFAULT_SURPRISE_THRESHOLD = 0.1f;

    constexpr int COGNITION_DEFAULT_PLANNING_DEPTH = 5;
    constexpr float COGNITION_DEFAULT_ATTENTION_STRENGTH = 0.5f;
    constexpr float COGNITION_DEFAULT_CONCEPT_FORMATION_THRESHOLD = 0.01f;

    constexpr float DEVELOPMENT_DEFAULT_INITIAL_AGE = 0.0f;
    constexpr float DEVELOPMENT_DEFAULT_CRITICAL_PERIOD_END = 60.0f;

    constexpr float SIMULATION_DEFAULT_TIMESTEP = 0.001f;
    constexpr int SIMULATION_DEFAULT_STEPS = 1000;
    constexpr int SIMULATION_DEFAULT_RANDOM_SEED = 42;

    constexpr const char* PERFORMANCE_DEFAULT_CHECKPOINT_DIR = "./checkpoints";
    constexpr int PERFORMANCE_DEFAULT_CHECKPOINT_INTERVAL = 100;
    constexpr int PERFORMANCE_DEFAULT_REPLAY_INTERVAL = 100;
    constexpr int PERFORMANCE_DEFAULT_CONSOLIDATION_INTERVAL = 1000;
}

} // namespace nlm