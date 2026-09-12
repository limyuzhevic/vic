// Phase 2.3 Implementation: Config Schema Validation Rules

// NLM Configuration Schema Definition

#include <vector>
#include <string>
#include <limits>
#include <utility>

// Basic Configuration Parameters
struct BasicConfig {
    // Neuron parameters
    uint32_t neuron_count = 1000;
    uint32_t neuron_size_min = 50;
    uint32_t neuron_size_max = 200;
    
    // Network parameters
    float connection_probability = 0.1f;
    uint32_t connection_delay_min = 1;
    uint32_t connection_delay_max = 10;
    
    // Simulation parameters
    float simulation_timestep = 0.001f;  // 1 ms
    uint64_t simulation_steps = 10000;
    float simulation_time = 10.0f;       // 10 seconds
    
    // Random seed
    uint64_t random_seed = 42;
};

// Plasticity Parameters
struct PlasticityConfig {
    // STDP parameters
    float stdp_ltp_weight = 0.01f;
    float stdp_ltd_weight = 0.012f;
    float stdp_tau = 20.0f;              // Time constant
    float stdp_max_weight_change = 0.1f;
    
    // Hebbian parameters
    float hebbian_learning_rate = 0.01f;
    float hebbian_threshold = 0.5f;
    
    // Structural plasticity
    float synaptogenesis_rate = 0.0001f;
    float pruning_rate = 0.00001f;
    float max_synaptic_weight = 1.0f;
    float min_synaptic_weight = -1.0f;
};

// Neuromodulation Parameters
struct NeuromodulationConfig {
    // Dopamine parameters
    float dopamine_baseline = 0.1f;
    float dopamine_peak = 1.0f;
    float dopamine_decay_rate = 0.99f;
    
    // Acetylcholine
    float acetylcholine_baseline = 0.05f;
    
    // Norepinephrine
    float norepinephrine_sharpening = 0.8f;
    
    // Serotonin
    float serotonin_inhibition = 0.3f;
    
    // Histamine
    float histamine_wake = 0.6f;
};

// Development Parameters
struct DevelopmentConfig {
    // Developmental stages
    float critical_period_start = 0.2f;   // Fraction of total development time
    float critical_period_end = 0.5f;     // Fraction of total development time
    
    // Maturation rates
    float maturation_rate = 0.01f;
    float pruning_rate = 0.001f;
    
    // Myelination
    float myelination_rate = 0.005f;
    
    // Connectivity refinement
    float refinement_rate = 0.02f;
};

// Memory Parameters
struct MemoryConfig {
    // Working memory
    size_t working_memory_capacity = 1000;
    float working_memory_decay_rate = 0.99f;
    
    // Episodic memory
    size_t max_episodes = 1000;
    float episode_decay_rate = 0.95f;
    
    // Semantic memory
    size_t semantic_memory_capacity = 5000;
    
    // Procedural memory
    size_t procedural_memory_capacity = 2000;
};

// Synaptic Dynamics Parameters
struct SynapticDynamicsConfig {
    // Conductance-based synapse
    float excitatory_reversal_potential = 0.0f;     // mV
    float inhibitory_reversal_potential = -70.0f;   // mV
    float membrane_capacitance = 1.0f;              // nF
    float time_constant = 20.0f;                    // ms
    
    // Spike dynamics
    float spike_threshold = -55.0f;                 // mV
    float spike_reset_potential = -70.0f;           // mV
    float refractory_period = 5.0f;                 // ms
    
    // Adaptation
    float adaptation_time_constant = 100.0f;        // ms
    float adaptation_strength = 0.1f;
};

// Integration Parameters
struct IntegrationConfig {
    // Sensory integration
    float sensory_weight = 1.0f;
    float sensory_threshold = 0.1f;
    
    // Motor output
    float motor_gain = 1.0f;
    float motor_threshold = 0.5f;
    
    // Attention
    float attentional_focus_width = 0.1f;
    float attentional_gain = 1.5f;
    
    // Prediction error
    float prediction_error_threshold = 0.1f;
    float prediction_error_decay = 0.9f;
};

// Checkpoint Parameters
struct CheckpointConfig {
    std::string directory = "./checkpoints";
    size_t max_checkpoints = 100;
    float checkpoint_interval = 0.1f;              // Fraction of simulation time
    bool compression_enabled = true;
    float compression_level = 0.5f;
};

// Visualization Parameters
struct VisualizationConfig {
    bool enabled = true;
    std::string output_directory = "./visualizations";
    float update_interval = 0.1f;
    std::string format = "png";
    size_t dpi = 300;
};

// Complete NLM Configuration Schema
struct NLMConfigSchema {
    // Required fields (no defaults)
    uint32_t neuron_count;
    uint32_t region_count;
    uint32_t population_per_region;
    
    // Optional fields with defaults
    BasicConfig basic;
    PlasticityConfig plasticity;
    NeuromodulationConfig neuromodulation;
    DevelopmentConfig development;
    MemoryConfig memory;
    SynapticDynamicsConfig synaptic;
    IntegrationConfig integration;
    CheckpointConfig checkpoint;
    VisualizationConfig visualization;
};

// Validation Functions

// Type validation
inline bool is_valid_number(float value, float min, float max) {
    return !std::isnan(value) && value >= min && value <= max;
}

inline bool is_valid_count(uint64_t value, uint64_t min, uint64_t max) {
    return value >= min && value <= max;
}

inline bool is_valid_string(const std::string& value, size_t min_len, size_t max_len) {
    return value.size() >= min_len && value.size() <= max_len;
}

// Configuration-specific validation

inline std::pair<bool, std::vector<std::string>> validate_neuron_parameters(const BasicConfig& config) {
    std::vector<std::string> errors;
    
    // Neuron count validation
    if (!is_valid_count(config.neuron_count, 1, 1000000)) {
        errors.push_back("Neuron count must be between 1 and 1,000,000");
    }
    
    // Neuron size validation
    if (!is_valid_count(config.neuron_size_min, 10, 100)) {
        errors.push_back("Minimum neuron size must be between 10 and 100");
    }
    
    if (!is_valid_count(config.neuron_size_max, config.neuron_size_min, 1000)) {
        errors.push_back("Maximum neuron size must be between minimum and 1000");
    }
    
    return std::make_pair(errors.empty(), errors);
}

inline std::pair<bool, std::vector<std::string>> validate_synaptic_parameters(const SynapticDynamicsConfig& config) {
    std::vector<std::string> errors;
    
    // Reversal potentials
    if (!is_valid_number(config.excitatory_reversal_potential, -100.0f, 50.0f)) {
        errors.push_back("Excitatory reversal potential must be between -100mV and 50mV");
    }
    
    if (!is_valid_number(config.inhibitory_reversal_potential, -100.0f, 10.0f)) {
        errors.push_back("Inhibitory reversal potential must be between -100mV and 10mV");
    }
    
    // Time constant
    if (!is_valid_number(config.time_constant, 1.0f, 1000.0f)) {
        errors.push_back("Time constant must be between 1ms and 1000ms");
    }
    
    // Membrane capacitance
    if (!is_valid_number(config.membrane_capacitance, 0.1f, 10.0f)) {
        errors.push_back("Membrane capacitance must be between 0.1nF and 10nF");
    }
    
    return std::make_pair(errors.empty(), errors);
}

inline std::pair<bool, std::vector<std::string>> validate_plasticity_parameters(const PlasticityConfig& config) {
    std::vector<std::string> errors;
    
    // STDP weights
    if (!is_valid_number(config.stdp_ltp_weight, 0.0f, 0.5f)) {
        errors.push_back("STDP LTP weight must be between 0.0 and 0.5");
    }
    
    if (!is_valid_number(config.stdp_ltd_weight, 0.0f, 0.5f)) {
        errors.push_back("STDP LTD weight must be between 0.0 and 0.5");
    }
    
    // STDP time constant
    if (!is_valid_number(config.stdp_tau, 1.0f, 1000.0f)) {
        errors.push_back("STDP tau must be between 1.0ms and 1000ms");
    }
    
    // Synaptic weight bounds
    if (!is_valid_number(config.max_synaptic_weight, 0.0f, 10.0f)) {
        errors.push_back("Maximum synaptic weight must be between 0.0 and 10.0");
    }
    
    if (!is_valid_number(config.min_synaptic_weight, -10.0f, 0.0f)) {
        errors.push_back("Minimum synaptic weight must be between -10.0 and 0.0");
    }
    
    // Ensure max > min
    if (config.max_synaptic_weight <= config.min_synaptic_weight) {
        errors.push_back("Maximum synaptic weight must be greater than minimum");
    }
    
    return std::make_pair(errors.empty(), errors);
}

inline std::pair<bool, std::vector<std::string>> validate_neuromodulation_parameters(const NeuromodulationConfig& config) {
    std::vector<std::string> errors;
    
    // All neuromodulator levels should be in [0, 1] range
    if (!is_valid_number(config.dopamine_baseline, 0.0f, 1.0f)) {
        errors.push_back("Dopamine baseline must be between 0.0 and 1.0");
    }
    
    if (!is_valid_number(config.dopamine_peak, 0.0f, 10.0f)) {
        errors.push_back("Dopamine peak must be between 0.0 and 10.0");
    }
    
    if (config.dopamine_peak <= config.dopamine_baseline) {
        errors.push_back("Dopamine peak must be greater than baseline");
    }
    
    return std::make_pair(errors.empty(), errors);
}

inline std::pair<bool, std::vector<std::string>> validate_development_parameters(const DevelopmentConfig& config) {
    std::vector<std::string> errors;
    
    // Ensure critical period is within [0, 1] and valid
    if (!is_valid_number(config.critical_period_start, 0.0f, 1.0f)) {
        errors.push_back("Critical period start must be between 0.0 and 1.0");
    }
    
    if (!is_valid_number(config.critical_period_end, 0.0f, 1.0f)) {
        errors.push_back("Critical period end must be between 0.0 and 1.0");
    }
    
    if (config.critical_period_start >= config.critical_period_end) {
        errors.push_back("Critical period start must be before end");
    }
    
    return std::make_pair(errors.empty(), errors);
}

inline std::pair<bool, std::vector<std::string>> validate_memory_parameters(const MemoryConfig& config) {
    std::vector<std::string> errors;
    
    // Memory capacities
    if (!is_valid_count(config.working_memory_capacity, 10, 1000000)) {
        errors.push_back("Working memory capacity must be between 10 and 1,000,000");
    }
    
    if (!is_valid_count(config.max_episodes, 10, 100000)) {
        errors.push_back("Max episodes must be between 10 and 100,000");
    }
    
    if (!is_valid_count(config.semantic_memory_capacity, 100, 1000000)) {
        errors.push_back("Semantic memory capacity must be between 100 and 1,000,000");
    }
    
    if (!is_valid_count(config.procedural_memory_capacity, 10, 100000)) {
        errors.push_back("Procedural memory capacity must be between 10 and 100,000");
    }
    
    return std::make_pair(errors.empty(), errors);
}

inline std::pair<bool, std::vector<std::string>> validate_integration_parameters(const IntegrationConfig& config) {
    std::vector<std::string> errors;
    
    // Sensory integration
    if (!is_valid_number(config.sensory_weight, 0.0f, 10.0f)) {
        errors.push_back("Sensory weight must be between 0.0 and 10.0");
    }
    
    if (!is_valid_number(config.sensory_threshold, 0.0f, 1.0f)) {
        errors.push_back("Sensory threshold must be between 0.0 and 1.0");
    }
    
    // Attention parameters
    if (!is_valid_number(config.attentional_focus_width, 0.0f, 1.0f)) {
        errors.push_back("Attention focus width must be between 0.0 and 1.0");
    }
    
    if (!is_valid_number(config.attentional_gain, 0.1f, 10.0f)) {
        errors.push_back("Attention gain must be between 0.1 and 10.0");
    }
    
    return std::make_pair(errors.empty(), errors);
}

inline std::pair<bool, std::vector<std::string>> validate_all_parameters(const NLMConfigSchema& config) {
    std::vector<std::string> errors;
    
    // Validate each section
    auto basic_result = validate_neuron_parameters(config.basic);
    if (!basic_result.first) {
        errors.insert(errors.end(), basic_result.second.begin(), basic_result.second.end());
    }
    
    auto synaptic_result = validate_synaptic_parameters(config.synaptic);
    if (!synaptic_result.first) {
        errors.insert(errors.end(), synaptic_result.second.begin(), synaptic_result.second.end());
    }
    
    auto plasticity_result = validate_plasticity_parameters(config.plasticity);
    if (!plasticity_result.first) {
        errors.insert(errors.end(), plasticity_result.second.begin(), plasticity_result.second.end());
    }
    
    auto neuromod_result = validate_neuromodulation_parameters(config.neuromodulation);
    if (!neuromod_result.first) {
        errors.insert(errors.end(), neuromod_result.second.begin(), neuromod_result.second.end());
    }
    
    auto dev_result = validate_development_parameters(config.development);
    if (!dev_result.first) {
        errors.insert(errors.end(), dev_result.second.begin(), dev_result.second.end());
    }
    
    auto memory_result = validate_memory_parameters(config.memory);
    if (!memory_result.first) {
        errors.insert(errors.end(), memory_result.second.begin(), memory_result.second.end());
    }
    
    auto integration_result = validate_integration_parameters(config.integration);
    if (!integration_result.first) {
        errors.insert(errors.end(), integration_result.second.begin(), integration_result.second.end());
    }
    
    // Cross-parameter validation
    if (config.basic.neuron_size_max < config.basic.neuron_size_min) {
        errors.push_back("Neuron size max must be >= min");
    }
    
    if (config.plasticity.max_synaptic_weight <= config.plasticity.min_synaptic_weight) {
        errors.push_back("Synaptic weight max must be > min");
    }
    
    if (config.plasticity.stdp_ltp_weight >= config.plasticity.stdp_ltd_weight) {
        errors.push_back("STDP LTP weight should typically be < LTD weight");
    }
    
    return std::make_pair(errors.empty(), errors);
}
