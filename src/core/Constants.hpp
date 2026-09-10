// Neural Computation Constants
// Core parameters for NLM Phase 2: Real Neural Computation
// 
// This file contains biologically-inspired constants organized by system component.
// All values include references to their scientific significance and trade-offs.

#pragma once

namespace nlm {

// ==============================================================================
// LIF NEURON DYNAMICS - Biophysical parameters
// ==============================================================================
namespace NeuronConstants {
    // Membrane capacitance: Typical mammalian cortical neuron
    // 1.0 nF represents a standard neuron size
    // Lower values = faster potential changes, higher values = slower dynamics
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    
    // Membrane time constant: Integration time scale
    // 20ms is typical for cortical neurons
    // Shorter tau = faster response, longer tau = more integration
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    
    // Maximum spike history to store
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Membrane potential limits (mV) - safety bounds
    static constexpr float MEMBRANE_POTENTIAL_MIN = -100.0f;  // Hyperpolarized
    static constexpr float MEMBRANE_POTENTIAL_MAX = 50.0f;   // Depolarized
    
    // Adaptation scaling factor
    static constexpr float ADAPTATION_DECAY_RATE = 0.05f;   // Per ms
    
    // Neuronal electrical properties
    static constexpr float DEFAULT_RESTING_POTENTIAL = -70.0f;  // mV - typical K+ equilibrium
    static constexpr float DEFAULT_THRESHOLD = -55.0f;          // mV - Na+ activation threshold
    static constexpr float DEFAULT_RESET_POTENTIAL = -70.0f;   // mV - after spike reset
    static constexpr float DEFAULT_LEAK_CONDUCTANCE = 10.0f;    // nS
    
    // Refractory periods (ms) - biological constraints
    static constexpr uint32_t MIN_REFRACTORY_PERIOD = 2;    // Minimum absolute refractory
    static constexpr uint32_t MAX_REFRACTORY_PERIOD = 100; // Maximum physiological
    static constexpr float DEFAULT_REFRACTORY_PERIOD = 5;  // 5ms typical
}

// ==============================================================================
// SYNAPTIC DYNAMICS - Plasticity and transmission properties
// ==============================================================================
namespace SynapseConstants {
    // Synaptic weight bounds - prevent runaway excitation/inhibition
    // Range [-1.0, 1.0] normalized for computational efficiency
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    static constexpr float DEFAULT_WEIGHT_RANGE_MIN = 0.1f;   // Excitatory
    static constexpr float DEFAULT_WEIGHT_RANGE_MAX = 0.4f;   // Excitatory
    static constexpr float DEFAULT_INHIBITORY_RANGE_MIN = -0.4f; // Inhibitory
    static constexpr float DEFAULT_INHIBITORY_RANGE_MAX = -0.1f; // Inhibitory
    
    // Synaptic delay range (simulation steps)
    static constexpr uint32_t MIN_SYNAPTIC_DELAY = 1;   // 1ms minimum
    static constexpr uint32_t MAX_SYNAPTIC_DELAY = 10; // 10ms maximum
    static constexpr uint32_t DEFAULT_SYNAPTIC_DELAY = 1; // Minimum realistic
    
    // Short-term plasticity parameters (Tsodyks-Markram model)
    static constexpr float STP_FACILITATION_TIME_CONSTANT = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TIME_CONSTANT = 200.0f;   // ms
    static constexpr float STP_U_MAX = 1.0f;                      // Max utilization
    
    // Eligibility trace parameters for reward-modulated learning
    static constexpr float ELIGIBILITY_TRACE_DECAY_RATE = 0.001f;  // Per ms
    
    // Synaptic efficacy bounds for use-dependent modulation
    static constexpr float MIN_EFFICIENCY = 0.0f;
    static constexpr float MAX_EFFICIENCY = 2.0f;
    static constexpr float DEFAULT_EFFICIENCY = 1.0f;
}

// ==============================================================================
// BRAIN SIMULATION - Temporal and structural parameters
// ==============================================================================
namespace BrainConstants {
    // Simulation timestep - balance between accuracy and performance
    // 1ms timestep matches typical neural recording resolution
    static constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;  // seconds
    
    // Checkpointing intervals - memory vs. state preservation trade-off
    static constexpr uint64_t DEFAULT_REPLAY_INTERVAL = 100;      // steps
    static constexpr uint64_t DEFAULT_CONSOLIDATION_INTERVAL = 1000; // steps
    static constexpr uint64_t DEFAULT_DEVELOPMENT_UPDATE_INTERVAL = 1000; // steps
    static constexpr uint64_t DEFAULT_STRUCTURAL_PLASTICITY_INTERVAL = 100; // steps
    
    // Synaptic current scaling - convert weights to currents
    static constexpr float SYNAPTIC_CURRENT_SCALE_FACTOR = 10.0f;  // nS per weight unit
    
    // Memory system parameters
    static constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 100;
    static constexpr size_t DEFAULT_EPISODIC_MEMORY_MAX_EPISODES = 1000;
    
    // Neuromodulation baseline levels
    static constexpr float DEFAULT_DOPAMINE_LEVEL = 0.5f;
    static constexpr float DEFAULT_NOVITY_THRESHOLD = 0.1f;
    static constexpr float DEFAULT_CURRENCY_DECAY_RATE = 0.01f;  // Per ms
}

// ==============================================================================
// PLASTICITY SYSTEMS - Learning rule parameters
// ==============================================================================
namespace PlasticityConstants {
    // STDP parameters (Biased towards LTP for stability)
    static constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;   // Positive change for pre-before-post
    static constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f; // Negative change for post-before-pre
    static constexpr float DEFAULT_STDP_TIME_CONSTANT = 20.0f; // ms - decay of spike influence
    
    // Structural plasticity rates - balance between growth and stability
    static constexpr float DEFAULT_SYNAPTONGENESIS_RATE = 0.0001f;  // Per step
    static constexpr float DEFAULT_PRUNING_RATE = 0.00001f;        // Per step
    static constexpr float MIN_PLATICTY_RATE = 1e-6f;              // Prevent numerical issues
    
    // Hebbian learning parameters
    static constexpr float DEFAULT_HEBBIAN_LEARNING_RATE = 0.01f;
    static constexpr float HEBBIAN_CONCURRENCE_FACTOR = 0.5f;
    
    // Neuromodulation effects on plasticity
    static constexpr float DOPAMINE_PLASTICITY_MODULATION = 1.0f;   // Linear scaling
    static constexpr float NOVELTY_PLASTICITY_THRESHOLD = 0.2f;    // Novelty boost factor
    static constexpr float REWARD_PLASTICITY_FACTOR = 2.0f;       // Reward modulation strength
}

// ==============================================================================
// DEVELOPMENT SYSTEM - Ontogenetic changes
// ==============================================================================
namespace DevelopmentConstants {
    // Plasticity modulation across developmental stages
    static constexpr float INITIAL_STAGE_PLASTICITY = 1.0f;      // High plasticity
    static constexpr float CRITICAL_PERIOD_PLASTICITY = 0.8f;   // Moderately reduced
    static constexpr float MATURATION_STAGE_PLASTICITY = 0.5f;   // Reduced
    static constexpr float ADULT_STAGE_PLASTICITY = 0.2f;        // Stable
    
    // Age-dependent parameter changes
    static constexpr float SYNAPTONGENESIS_RATE_MODULATION = 0.1f;   // Per developmental stage
    static constexpr float PRUNING_RATE_MODULATION = 0.5f;           // Per developmental stage
    
    // Developmental timing
    static constexpr uint64_t DEFAULT_DEVELOPMENT_UPDATE_FREQUENCY = 100;  // steps
}

// ==============================================================================
// COGNITION SYSTEMS - Information processing parameters
// ==============================================================================
namespace CognitionConstants {
    // Working memory parameters
    static constexpr float DEFAULT_WORKING_MEMORY_DECAY_RATE = 0.01f;  // Per ms
    static constexpr float DEFAULT_WORKING_MEMORY_STRENGTH_THRESHOLD = 0.1f;
    static constexpr float DEFAULT_WORKING_MEMORY_INHIBITION_STRENGTH = 0.5f;
    static constexpr float DEFAULT_WORKING_MEMORY_EXCITATION_STRENGTH = 1.5f;
    
    // Attention system parameters
    static constexpr float DEFAULT_ATTENTION_INHIBITION_STRENGTH = 0.5f;
    static constexpr float DEFAULT_ATTENTION_EXCITATION_STRENGTH = 1.5f;
    static constexpr float DEFAULT_ATTENTION_FOCUS_WIDTH = 0.2f;
    
    // Planning parameters
    static constexpr int DEFAULT_PLANNING_DEPTH = 5;
    static constexpr float DEFAULT_PLAN_EVALUATION_CONFIDENCE_THRESHOLD = 0.7f;
}

// ==============================================================================
// NEUROMODULATION SYSTEMS - Cognitive control
// ==============================================================================
namespace NeuromodulationConstants {
    // Dopamine system parameters
    static constexpr float DEFAULT_DOPAMINE_LEVEL = 0.5f;
    static constexpr float DOPAMINE_EXCITABILITY_MODULATION = 0.5f;  // Scale factor
    static constexpr float DOPAMINE_PLASTICITY_FACTOR_BASE = 1.0f;
    
    // Curiosity system parameters
    static constexpr float DEFAULT_CURRENCY_LEVEL = 0.5f;
    static constexpr float DEFAULT_NOVITY_SENSITIVITY = 0.5f;
    static constexpr float DEFAULT_EXPLORATION_BIAS = 0.3f;
    
    // Prediction error parameters
    static constexpr float DEFAULT_PREDICTION_ERROR_THRESHOLD = 0.1f;
    static constexpr float PREDICTION_ERROR_LEARNING_RATE = 0.01f;
    
    // Novelty detection parameters
    static constexpr float DEFAULT_NOVELTY_SENSITIVITY = 0.1f;
    static constexpr float DEFAULT_NOVITY_DECAY_RATE = 0.01f;
}

// ==============================================================================
// NEURAL POPULATION - Group dynamics
// ==============================================================================
namespace PopulationConstants {
    // Population composition ratios
    static constexpr float SENSORY_PROPORTION = 0.25f;  // 25% sensory
    static constexpr float INTERNAL_PROPORTION = 0.5f;   // 50% internal
    static constexpr float MOTOR_PROPORTION = 0.25f;     // 25% motor
    
    // Region scaling factors
    static constexpr float NEURONS_PER_POPULATION = 50.0f;   // Typical population size
    static constexpr float REGION_POPULATION_DENSITY = 0.1f; // Connections per neuron pair
}

// ==============================================================================
// CHECKPOINTING - Persistence and recovery
// ==============================================================================
namespace CheckpointConstants {
    // Checkpoint file format parameters
    static constexpr uint32_t CHECKPOINT_VERSION_MAJOR = 1;
    static constexpr uint32_t CHECKPOINT_VERSION_MINOR = 0;
    static constexpr uint32_t CHECKPOINT_VERSION_PATCH = 0;
    
    // Checkpointing strategy
    static constexpr uint64_t DEFAULT_CHECKPOINT_INTERVAL = 10000;  // steps
    static constexpr size_t DEFAULT_MAX_CHECKPOINTS = 10;            // Keep last 10
    static constexpr bool DEFAULT_CHECKPOINT_COMPRESSION = true;
    static constexpr uint64_t DEFAULT_MAX_CHECKPOINT_SIZE_MB = 5;   // 5MB per checkpoint
    
    // Recovery and validation
    static constexpr uint32_t CHECKPOINT_MAGIC = 0x4E4C4D434B545430ULL;  // "NLMCKT0"
}

} // namespace nlm
