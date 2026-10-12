# NLM Phase 2 Configuration Reference

## Overview

Phase 2 configuration parameters define the biophysical properties of neurons, synapses, and plasticity mechanisms in NLM. These parameters control the core neural computation dynamics including leaky integrate-and-fire (LIF) neuron behavior, synaptic transmission, and learning rules. Configuration is done via C++ structures and supports both per-synaptic and per-neuron parameter variations.

## Configuration Philosophy

### Biophysical Constraints
- Parameters grounded in experimental data where available
- Trade-offs between biological realism and computational efficiency
- Modular parameter organization for easy adjustment

### Configuration Organization
- **Global parameters**: System-wide settings (timestep, duration)
- **Per-neuron parameters**: Individual neuron properties (LIF dynamics, refractory period)
- **Per-synapse parameters**: Connection-specific properties (STP, plasticity)
- **Per-rule parameters**: Plasticity mechanism settings (STDP, Hebbian, reward-modulated)

## Neuron Configuration

### LIF Dynamics Parameters

#### Membrane Properties
```cpp
struct MembraneProperties {
    float time_constant = 20.0f;     // τ_m (membrane time constant) in ms
    float resting_potential = -70.0f; // V_rest in mV
    float threshold_potential = -55.0f; // V_threshold in mV
    float reset_potential = -65.0f;   // V_reset in mV
    float membrane_resistance = 150.0f; // R_m in MOhm
    float capacitance = 0.25f;        // C_m in nF (R_m * τ_m)
};
```

#### Refractory Dynamics
```cpp
struct RefractoryProperties {
    float absolute_refractory_period = 2.0f;   // τ_abs in ms
    float relative_refractory_period = 10.0f; // τ_rel in ms
    float recovery_slope = 0.1f;             // recovery rate parameter
};
```

### State Variables
```cpp
struct NeuronState {
    float membrane_potential;     // V in mV
    float refractory_timer;       // remaining refractory time in ms
    float input_accumulator;      // synaptic input charge
    float spike_history;          // spike frequency or recent spike count
    int spike_count;              // total spikes for activity metrics
    Timestamp last_spike_time;    // time of last spike
};
```

## Synapse Configuration

### Synaptic Properties
```cpp
struct SynapticProperties {
    // Transmission properties
    float baseline_weight = 0.5f;             // w (synaptic strength, -1 to 1)
    float reversal_potential = -70.0f;       // E_syn in mV
    float delay = 2.0f;                      // transmission delay in ms
    float conductance = 1.0f;                // g_syn (conductance factor)
    
    // Short-term plasticity (Tsodyks-Markram)
    float utilization = 0.3f;                 // U (release probability)
    float facilitation_time = 500.0f;        // τ_facilitation in ms
    float recovery_time = 300.0f;            // τ_recovery in ms
    bool enable_stp = true;                  // enable short-term plasticity
    
    // Structural properties
    float min_weight = -1.0f;                // minimum allowed weight
    float max_weight = 1.0f;                 // maximum allowed weight
    bool enable_pruning = true;              // enable pruning consideration
};
```

### STP State Variables
```cpp
struct STPState {
    float utilization;          // u (current utilization)
    float facilitation;        // x (current facilitation)
    float last_pre_spike;      // time of last presynaptic spike
    float last_post_spike;     // time of last postsynaptic spike
};
```

## Plasticity Configuration

### STDP Configuration
```cpp
struct STDPConfig {
    // Potentiation parameters (pre before post)
    float A_plus = 0.03f;                  // maximum potentiation
    float tau_plus = 20.0f;                // potentiation time constant (ms)
    float min_weight_increment = 1e-6f;     // minimum meaningful change
    
    // Depression parameters (post before pre)
    float A_minus = 0.035f;                // maximum depression magnitude
    float tau_minus = 30.0f;               // depression time constant (ms)
    
    // Constraints
    float max_weight_change = 0.1f;         // maximum weight change per event
    float weight_bound = 1.0f;             // absolute weight limit
    
    // Biological considerations
    bool include_saturation = true;        // weight saturation effects
    float saturation_threshold = 0.95f;    // weight saturation point
};
```

### Hebbian Configuration
```cpp
struct HebbianConfig {
    float learning_rate = 0.001f;           // η (learning rate)
    float trace_decay = 20.0f;             // trace time constant (ms)
    float normalization_factor = 0.01f;    // for weight normalization
    float max_trace_value = 1.0f;          // trace accumulation limit
    
    // Implementation variants
    enum Variant {
        Basic,           // Simple correlation
        Oja,             // Normalization variant
        Batch,           // Accumulated updates
        Online           // Continuous updates
    } variant = Variant::Basic;
};
```

### Reward-Modulated Configuration
```cpp
struct RewardModulatedConfig {
    float base_learning_rate = 0.01f;       // η_0 (base learning rate)
    float eligibility_decay = 0.99f;        // λ (eligibility trace decay)
    float reward_sensitivity = 1.0f;        // scaling factor for reward signals
    float min_eligibility = 1e-6f;         // minimum eligibility trace value
    
    // Neuromodulator effects
    float dopamine_gain = 1.0f;            // modulation strength
    bool enable_reward_gate = true;        // only learn on reward
    bool enable_prediction_error = true;   // use prediction error signals
};
```

### Synaptic Scaling Configuration
```cpp
struct SynapticScalingConfig {
    float target_rate = 10.0f;             // target firing rate (Hz)
    float scaling_rate = 0.01f;            // η (scaling speed)
    float scaling_threshold = 0.05f;       // activity threshold for scaling
    float max_scaling_factor = 2.0f;       // maximum scaling magnitude
    bool enable_homeostatic = true;        // enable homeostatic regulation
    
    // Implementation details
    enum Mode {
        Global,       // scale all synapses equally
        Local,        // scale within neural populations
        Adaptive     // adjust based on activity patterns
    } mode = Mode::Global;
};
```

## Structural Plasticity Configuration

### Synaptogenesis Configuration
```cpp
struct SynaptogenesisConfig {
    // Base formation rates
    float formation_rate = 0.01f;          // per ms per possible connection
    float activity_threshold = 0.1f;       // correlation threshold for activity
    
    // Developmental modulation
    float critical_period_multiplier = 2.0f; // increased plasticity early
    float maturation_decay = 0.995f;       // decay of formation rate over time
    float developmental_plasticity = 1.0f; // plasticity modulation factor
    
    // Constraints
    int max_outgoing_synapses = 1000;      // per neuron
    int max_incoming_synapses = 1000;      // per neuron
    float connection_probability = 0.1f;   // initial connection probability
    
    // Topological preferences
    float distance_weight = 1.0f;           // distance-based connection bias
    float similarity_weight = 0.5f;        // activity similarity bias
};
```

### Pruning Configuration
```cpp
struct PruningConfig {
    // Weight-based pruning
    float weight_threshold = 0.05f;        // minimum absolute weight
    float adult_weight_threshold = 0.02f;  // stricter threshold in adulthood
    
    // Activity-based pruning
    float activity_threshold = 0.01f;      // minimum firing rate
    float min_connections = 10;             // minimum required synapses
    
    // Age constraints
    float min_synapse_age = 100.0f;        // minimum age in ms
    float adult_pruning_age = 1000.0f;     // age for adult pruning regime
    
    // Developmental stages
    enum Stage {
        CriticalPeriod, // high plasticity, selective pruning
        Maturation,     // decreasing plasticity
        Adult           // stable pruning rates
    } stage = Stage::Adult;
    
    // Pruning rates
    float critical_period_rate = 0.02f;    // pruning rate during critical period
    float adult_rate = 0.01f;              // pruning rate in adulthood
};
```

## Development System Configuration

### Developmental Stages
```cpp
struct DevelopmentalStage {
    float plasticity_rate = 1.0f;          // modulation factor for plasticity
    float formation_rate_multiplier = 1.0f; // for synaptogenesis
    float pruning_rate_multiplier = 1.0f;  // for synapse elimination
    float critical_period_active = false;  // critical period status
    float maturation_progress = 0.0f;     // progress from immature to adult
};

struct DevelopmentConfig {
    // Stage durations
    float critical_period_duration = 1000.0f;  // ms
    float maturation_duration = 3000.0f;      // ms
    
    // Stage parameters
    DevelopmentalStage initial_stage;
    DevelopmentalStage critical_period_stage;
    DevelopmentalStage maturation_stage;
    DevelopmentalStage adult_stage;
    
    // Progression control
    float stage_advancement_threshold = 0.9f; // progress threshold
    bool enable_stage_locking = true;         // prevent stage regression
};
```

## Simulation Configuration

### Core Simulation Parameters
```cpp
struct SimulationConfig {
    // Time control
    float timestep = 0.1f;                // simulation step size (ms)
    float simulation_duration = 1000.0f;   // total simulation time (ms)
    bool use_event_driven = true;          // event-driven processing
    bool use_time_buckets = true;          // time-bucketed spike delivery
    
    // Performance settings
    int time_bucket_size = 10;             // ms per time bucket
    int max_spike_events = 1000000;        // maximum spike events
    int max_neurons = 100000;              // maximum neuron count
    int max_synapses = 10000000;            // maximum synapse count
    
    // Memory management
    bool enable_memory_pool = true;         // use pre-allocated pools
    size_t neuron_pool_size = 100000;       // neuron pool size
    size_t synapse_pool_size = 10000000;    // synapse pool size
    
    // Numerical integration
    enum IntegrationMethod {
        ForwardEuler,      // standard Euler method
        ForwardEulerCromer // semi-implicit Euler
    } integration_method = IntegrationMethod::ForwardEuler;
};
```

### STP Simulation Parameters
```cpp
struct STPSimulationConfig {
    // Time stepping
    float stp_timestep = 0.1f;             // must match simulation timestep
    bool use_analytic_stp = true;          // use analytical STP solutions
    
    // State update strategy
    enum UpdateStrategy {
        PerSpike,    // update on each spike event
        PerStep,     // update each timestep
        Batch        // accumulate then update
    } update_strategy = UpdateStrategy::PerSpike;
    
    // Numerical constraints
    float max_stp_change = 0.1f;           // maximum STP state change per step
    float min_stp_value = 0.0f;            // minimum STP state value
    float max_stp_value = 1.0f;            // maximum STP state value
};
```

## Neural Population Configuration

### Population Parameters
```cpp
struct NeuralPopulationConfig {
    // Population properties
    size_t neuron_count = 100;             // number of neurons in population
    size_t max_connections = 1000;         // maximum connections to other populations
    float connection_probability = 0.1f;   // connection probability
    
    // Functional properties
    float preferred_firing_rate = 10.0f;   // target firing rate (Hz)
    float tuning_width = 20.0f;            // tuning curve width
    float baseline_excitability = 1.0f;    // baseline neuronal excitability
    
    // Heterogeneity
    float neuron_variability = 0.1f;      // coefficient of variation for parameters
    bool enable_structural_heterogeneity = true; // varied connectivity patterns
};
```

### Region Configuration
```cpp
struct NeuralRegionConfig {
    // Regional properties
    std::string name = "UnnamedRegion";    // region identifier
    size_t neuron_count = 1000;            // total neurons in region
    size_t population_count = 10;          // number of neural populations
    
    // Structural constraints
    float connection_density = 0.05f;      // probability of connection
    float average_degree = 100.0f;         // average number of connections
    bool enable_feedback_loops = true;     // allow within-region connections
    
    // Functional specialization
    float global_excitability = 1.0f;      // region-level excitability
    float local_connectivity = 0.8f;       // strength of within-region connections
    float external_influence = 0.2f;       // strength of input from other regions
};
```

## Advanced Configuration

### Neuromodulation Integration
```cpp
struct NeuromodulationConfig {
    // Dopamine system
    float baseline_dopamine = 0.1f;        // baseline concentration
    float reward_gain = 1.0f;              // reward signal amplification
    float prediction_error_gain = 1.0f;    // prediction error signal strength
    
    // Novelty and curiosity
    float novelty_threshold = 0.05f;        // threshold for novelty detection
    float curiosity_decay = 0.99f;         // curiosity signal decay
    float exploration_bonus = 0.1f;        // bonus for exploration behavior
    
    // Integration with plasticity
    bool enable_dopamine_modulation = true; // dopamine effects on STDP
    bool enable_curiosity_guided_learning = true; // curiosity-driven exploration
};
```

### Research and Development Configuration
```cpp
struct ResearchConfig {
    // Ablation studies
    bool enable_ablation_studies = false;   // for mechanism testing
    std::vector<std::string> ablated_mechanisms; // list of ablated mechanisms
    
    // Scaling experiments
    bool enable_scaling_benchmarks = true;  // run performance benchmarks
    size_t scaling_target_neurons = 10000;  // target neuron count for scaling tests
    
    // Validation against biology
    bool enable_biological_validation = true; // validate against experimental data
    float biological_validation_threshold = 0.8f; // minimum validation accuracy
    
    // Reproducibility
    bool deterministic_mode = true;         // use fixed random seeds
    uint32_t random_seed = 12345;           // base random seed
    bool save_checkpoints = true;           // save simulation checkpoints
};
```

## Configuration Loading and Validation

### JSON Configuration Format
```json
{
    "neurons": {
        "membrane": {
            "time_constant": 20.0,
            "resting_potential": -70.0,
            "threshold_potential": -55.0
        },
        "refractory": {
            "absolute_refractory_period": 2.0,
            "relative_refractory_period": 10.0
        }
    },
    "synapses": {
        "baseline_weight": 0.5,
        "utilization": 0.3,
        "facilitation_time": 500.0
    },
    "plasticity": {
        "stdp": {
            "A_plus": 0.03,
            "A_minus": 0.035,
            "tau_plus": 20.0,
            "tau_minus": 30.0
        },
        "hebbian": {
            "learning_rate": 0.001,
            "trace_decay": 20.0
        }
    },
    "simulation": {
        "timestep": 0.1,
        "simulation_duration": 1000.0,
        "use_event_driven": true
    }
}
```

### Configuration Validation
```cpp
bool validateConfiguration(const NLMConfig& config) {
    // Check parameter ranges
    if (config.simulation.timestep <= 0.0) return false;
    if (config.simulation.timestep > config.neuron.membrane.time_constant / 10) return false;
    
    if (config.neuron.membrane.threshold_potential <= config.neuron.membrane.resting_potential) return false;
    if (config.synapses.baseline_weight < config.synapses.min_weight) return false;
    if (config.synapses.baseline_weight > config.synapses.max_weight) return false;
    
    // Check plasticity constraints
    if (config.plasticity.stdp.A_plus <= 0.0) return false;
    if (config.plasticity.stdp.tau_plus <= 0.0) return false;
    if (config.plasticity.stdp.A_minus >= 0.0) return false; // must be negative
    
    return true;
}
```

## Default Configuration File

The default configuration for NLM Phase 2 can be found in `config/nlm_phase2_default.json` and includes:

```json
{
    "name": "NLM Phase 2 Default",
    "version": "2.0.0",
    "description": "Default configuration for NLM Phase 2 neural computation",
    "parameters": {
        "neurons": {...},
        "synapses": {...},
        "plasticity": {...},
        "simulation": {...},
        "development": {...},
        "populations": {...}
    }
}
```

## Configuration Best Practices

### Parameter Selection Guidelines
1. **Timestep**: Start with 0.1ms, reduce for detailed dynamics
2. **Weights**: Normalize between -1 and 1 for stability
3. **Plasticity rates**: Start small (0.001-0.01), adjust based on learning speed
4. **Neuromodulation**: Use prediction error for biologically realistic learning
5. **Development**: Begin with higher plasticity, reduce over time

### Performance Optimization
- Use memory pools for large-scale simulations
- Configure time bucketing for spike delivery efficiency
- Adjust neuron count based on available memory
- Enable event-driven processing for sparse networks

### Biological Fidelity
- Parameters grounded in experimental data where possible
- Validate against known biological ranges
- Consider multiple scales (ms, mV, etc.)
- Account for biological variability in populations
