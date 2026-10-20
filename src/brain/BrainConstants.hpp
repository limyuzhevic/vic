// Brain configuration constants
// These replace magic numbers throughout Brain.cpp to improve maintainability

namespace nlm {

    // Simulation parameters
    namespace SimulationConstants {
        // Default simulation timestep (1ms)
        constexpr double DEFAULT_TIMESTEP = 0.001;
        
        // Default number of neurons
        constexpr size_t DEFAULT_NEURON_COUNT = 1000;
        
        // Default number of regions
        constexpr size_t DEFAULT_REGION_COUNT = 1;
        
        // Default connection probability
        constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.1f;
    }

    // Memory system parameters
    namespace MemoryConstants {
        // Default working memory capacity (10% of neurons)
        constexpr float DEFAULT_WORKING_MEMORY_CAPACITY_RATIO = 0.1f;
        
        // Default max episodic memory episodes
        constexpr size_t DEFAULT_MAX_EPISODES = 1000;
        
        // Working memory activation threshold
        constexpr float WORKING_MEMORY_ACTIVATION_THRESHOLD = 0.5f;
        
        // Working memory trace decay factor
        constexpr float WORKING_MEMORY_TRACE_DECAY = 0.1f;
    }

    // Neuromodulation parameters
    namespace NeuromodulationConstants {
        // Default dopamine and neuromodulator influence on neural excitability
        constexpr float NEUROMODULATION_EXCITABILITY_FACTOR = 0.5f;
        
        // Default neuromodulator influence on plasticity
        constexpr float NEUROMODULATION_PLASTICITY_FACTOR = 0.5f;
    }

    // Plasticity parameters
    namespace PlasticityConstants {
        // STDP learning rate defaults
        constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.01f;
        constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.012f;
        
        // STDP time constant
        constexpr float DEFAULT_STDP_TAU = 20.0f;
        
        // Synaptogenesis and pruning rates
        constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
        constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
        
        // Synaptic weight range for initialization
        constexpr float SYNAPSE_WEIGHT_MIN_EXCITATORY = 0.01f;
        constexpr float SYNAPSE_WEIGHT_MAX_EXCITATORY = 0.4f;
        constexpr float SYNAPSE_WEIGHT_MIN_INHIBITORY = -0.4f;
        constexpr float SYNAPSE_WEIGHT_MAX_INHIBITORY = -0.01f;
        
        // Default Hebbian learning rate
        constexpr float HEBBIAN_LEARNING_RATE = 0.1f;
    }

    // Development parameters
    namespace DevelopmentConstants {
        // Replay every N steps
        constexpr size_t DEFAULT_REPLAY_INTERVAL = 100;
        
        // Consolidate memory every N steps
        constexpr size_t DEFAULT_CONSOLIDATION_INTERVAL = 1000;
        
        // Update development every N steps
        constexpr size_t DEFAULT_DEVELOPMENT_INTERVAL = 1000;
        
        // Update structural plasticity every N steps
        constexpr size_t DEFAULT_STRUCTURAL_PLASTICITY_INTERVAL = 100;
    }

    // Neural parameters
    namespace NeuralConstants {
        // Membrane potential thresholds and clamping
        constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
        constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
        constexpr float DEFAULT_MEMBRANE_THRESHOLD = -55.0f;
        
        // Neuronal firing parameters
        constexpr size_t DEFAULT_REFRACTORY_PERIOD = 5;  // ms
        constexpr float DEFAULT_FIRING_RATE = 5.0f;
        
        // Spike history and recording
        constexpr size_t MAX_SPIKE_HISTORY = 100;
        
        // Spike-frequency adaptation
        constexpr float ADAPTATION_DECAY = 0.95f;
        constexpr float ADAPTATION_STRENGTH = 0.01f;
        constexpr float ADAPTATION_INITIAL_INCREMENT = 1.0f;
    }

    // Synapse parameters
    namespace SynapseConstants {
        // Synaptic transmission
        constexpr float SYNAPTIC_CURRENT_SCALE_FACTOR = 10.0f;
        
        // Synaptic delay defaults
        constexpr size_t DEFAULT_SYNAPTIC_DELAY = 1;  // steps
        
        // Synaptic density calculation
        constexpr float SYNAPTIC_DENSITY_EPSILON = 1e-6f;
    }

    // Concept and attention parameters
    namespace CognitionConstants {
        // Neural planner parameters
        constexpr size_t DEFAULT_PLANNING_DEPTH = 5;
        
        // Attentional selection parameters
        constexpr float DEFAULT_INHIBITION_STRENGTH = 0.5f;
        constexpr float DEFAULT_EXCITATION_STRENGTH = 1.5f;
        
        // Concept formation parameters
        constexpr float CONCEPT_FORMATION_THRESHOLD = 5.0f;
        constexpr float CONCEPT_FORMATION_ACTIVATION = 0.2f;
    }

    // Checkpoint and persistence parameters
    namespace CheckpointConstants {
        // Default checkpoint directory
        constexpr const char* DEFAULT_CHECKPOINT_DIR = "./checkpoints";
        
        // Checkpoint compression level
        constexpr size_t DEFAULT_COMPRESSION_LEVEL = 5;
        
        // Maximum number of checkpoints to retain
        constexpr size_t DEFAULT_MAX_CHECKPOINTS = 10000;
        
        // Checkpoint validation tolerance
        constexpr float DEFAULT_CHECKPOINT_TOLERANCE = 0.001f;
    }

    // Randomness and initialization parameters
    namespace RandomConstants {
        // Default random seed
        constexpr uint64_t DEFAULT_RANDOM_SEED = 42;
        
        // Neuron initialization ranges
        constexpr float DEFAULT_NEURON_POTENTIAL_RANGE = 3.0f;
        constexpr float DEFAULT_NEURON_THRESHOLD_RANGE = 2.0f;
        constexpr float DEFAULT_RESTING_POTENTIAL_RANGE = 2.0f;
        constexpr float DEFAULT_RESET_POTENTIAL_RANGE = 5.0f;
        
        // Refractory period range (2-10ms)
        constexpr size_t MIN_REFRACTORY_PERIOD = 2;
        constexpr size_t MAX_REFRACTORY_PERIOD = 10;
    }

} // namespace nlm
