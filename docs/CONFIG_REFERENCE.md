# NLM Configuration Reference

## Overview

This document provides complete reference for all configuration parameters used in the NLM system. Configuration parameters control the structure, behavior, and performance of the brain simulation.

## Configuration System

### Configuration Class

The Configuration system is a key-value store that provides type-safe access to all brain parameters. All parameters are optional - defaults are provided for all configuration options.

```cpp
std::shared_ptr<Config> config = std::make_shared<Config>();
// Set parameters
config->set("neuron_count", 1000);
config->set("connection_probability", 0.1f);
```

## Core Neural Parameters

### neuron_count (size_t)

**Default:** 1000
**Valid Range:** 1 to 1,000,000

**Description:** Total number of neurons to create in the brain.

**Effect:** Larger values increase neural capacity but also increase computational cost.

**Mathematical Impact:** Affects the size of neural state vectors and memory requirements.

**Usage Example:**
```cpp
config->set("neuron_count", 5000); // Create 5000 neurons
```

### region_count (size_t)

**Default:** 1
**Valid Range:** 1 to 100

**Description:** Number of brain regions to create. Each region contains its own populations of neurons.

**Effect:** More regions enable better modular organization and reduce connection density.

**Mathematical Impact:** Neurons are distributed as `neurons_per_region = neuron_count / region_count`.

**Usage Example:**
```cpp
config->set("region_count", 10); // Create 10 regions
```

### connection_probability (float)

**Default:** 0.1
**Valid Range:** 0.0 to 1.0

**Description:** Probability that a potential synapse connection is formed during initialization.

**Effect:** Controls overall network connectivity. Higher values create denser, more interconnected networks.

**Mathematical Impact:** Expected number of synapses = `neuron_count × neuron_count × connection_probability / 2`.

**Usage Example:**
```cpp
config->set("connection_probability", 0.2); // 20% connection probability
```

## Simulation Parameters

### simulation_timestep (double)

**Default:** 0.001
**Valid Range:** 1e-6 to 0.1

**Description:** Time step for neural integration in seconds.

**Effect:** Smaller values provide more accurate simulation but slower performance.

**Mathematical Impact:** Determines the precision of LIF dynamics integration: `dV/dt = (-V + R·I)/τ_m`.

**Stability Constraint:** Must satisfy `Δt < τ_m · ln(1 + R·V_th/I)` for stability.

**Usage Example:**
```cpp
config->set("simulation_timestep", 0.0005); // 0.5 ms time step
```

## Plasticity Parameters

### STDP Parameters

#### stdp_ltp_weight (float)

**Default:** 0.01
**Valid Range:** 0.0 to 1.0

**Description:** Long-Term Potentiation weight change magnitude in STDP.

**Effect:** Controls strengthening of synapses when pre-synaptic spike precedes post-synaptic spike.

**Mathematical Formulation:**
```
if t_post > t_pre: Δw = stdp_ltp_weight × exp(-(t_post - t_pre)/stdp_tau)
```

**Usage Example:**
```cpp
config->set("stdp_ltp_weight", 0.02); // Double LTP strength
```

#### stdp_ltd_weight (float)

**Default:** 0.012
**Valid Range:** 0.0 to 1.0

**Description:** Long-Term Depression weight change magnitude in STDP.

**Effect:** Controls weakening of synapses when pre-synaptic spike follows post-synaptic spike.

**Mathematical Formulation:**
```
if t_post < t_pre: Δw = -stdp_ltd_weight × exp((t_post - t_pre)/stdp_tau)
```

**Usage Example:**
```cpp
config->set("stdp_ltd_weight", 0.015); // Stronger LTD
```

#### stdp_tau (float)

**Default:** 20.0
**Valid Range:** 1.0 to 100.0

**Description:** STDP time constant in milliseconds.

**Effect:** Controls the decay of STDP effect with spike timing difference.

**Mathematical Impact:** Larger values make STDP effects last longer in time.

**Usage Example:**
```cpp
config->set("stdp_tau", 50.0); // 50 ms time constant
```

### Structural Plasticity

#### synaptogenesis_rate (float)

**Default:** 0.0001
**Valid Range:** 0.0 to 0.01

**Description:** Rate of new synapse formation per neuron per step.

**Effect:** Controls network growth over time. Higher rates lead to denser connectivity.

**Mathematical Model:**
```
dN_syn/dt = synaptogenesis_rate × N_neurons
```

**Usage Example:**
```cpp
config->set("synaptogenesis_rate", 0.001); // Faster growth
```

#### pruning_rate (float)

**Default:** 0.00001
**Valid Range:** 0.0 to 0.001

**Description:** Rate of synapse removal per neuron per step.

**Effect:** Controls network pruning and optimization. Higher rates lead to sparser connectivity.

**Mathematical Model:**
```
dN_syn/dt = -pruning_rate × N_syn
```

**Usage Example:**
```cpp
config->set("pruning_rate", 0.00005); // More aggressive pruning
```

## Memory System Parameters

### Working Memory

#### working_memory_capacity (size_t)

**Default:** neuron_count / 10
**Valid Range:** 1 to neuron_count

**Description:** Maximum number of active items in working memory.

**Effect:** Controls how much information can be maintained temporarily.

**Mathematical Impact:** Memory capacity affects attention and working memory competition dynamics.

**Usage Example:**
```cpp
config->set("working_memory_capacity", 500); // 500-item working memory
```

### Episodic Memory

#### max_episodes (size_t)

**Default:** 1000
**Valid Range:** 10 to 100,000

**Description:** Maximum number of episodes that can be stored.

**Effect:** Controls memory duration and capacity. Larger values enable longer memory retention.

**Storage Requirements:** Each episode requires ~100 bytes per neuron, so 1000 neurons × 1000 episodes ≈ 100 MB.

**Usage Example:**
```cpp
config->set("max_episodes", 5000); // Store 5000 episodes
```

#### replay_interval (size_t)

**Default:** 100
**Valid Range:** 1 to 10000

**Description:** Number of simulation steps between episodic memory replay events.

**Effect:** Controls how often the brain replays past experiences for consolidation.

**Mathematical Impact:** Memory consolidation frequency: `f_replay = 1 / replay_interval`.

**Usage Example:**
```cpp
config->set("replay_interval", 50); // Replay every 50 steps
```

#### consolidation_interval (size_t)

**Default:** 1000
**Valid Range:** 100 to 100000

**Description:** Number of steps between memory consolidation cycles.

**Effect:** Controls how often weak memories are removed and strong ones strengthened.

**Mathematical Model:**
```
P(forget | activation) = exp(-activation / consolidation_threshold)
```

**Usage Example:**
```cpp
config->set("consolidation_interval", 500); // Consolidate every 500 steps
```

## Development System Parameters

### Developmental Stages

#### critical_period_start (size_t)

**Default:** 100
**Valid Range:** 1 to 1000

**Description:** Simulation step when critical period for learning begins.

**Effect:** Period of heightened plasticity when new experiences are most effective.

**Mathematical Model:**
```
plasticity_mod = 1.0 + 0.5 × exp(-(step - critical_period_start) / 100)
```

**Usage Example:**
```cpp
config->set("critical_period_start", 200); // Start critical period at step 200
```

#### critical_period_end (size_t)

**Default:** 1000
**Valid Range:** critical_period_start + 1 to 10000

**Description:** Simulation step when critical period ends.

**Effect:** After this step, plasticity gradually decreases to adult levels.

**Usage Example:**
```cpp
config->set("critical_period_end", 2000); // End critical period at step 2000
```

### Developmental Plasticity Modifiers

#### development_plasticity_factor (float)

**Default:** 1.0
**Valid Range:** 0.1 to 10.0

**Description:** Global multiplier for developmental plasticity effects.

**Effect:** Scales all plasticity changes during development.

**Mathematical Impact:** Applied to all plasticity rule weight changes during developmental stages.

**Usage Example:**
```cpp
config->set("development_plasticity_factor", 2.0); // Double developmental plasticity
```

## Neuromodulation Parameters

### Dopamine System

#### dopamine_baseline (float)

**Default:** 0.1
**Valid Range:** 0.0 to 10.0

**Description:** Baseline dopamine level when no reward prediction error is present.

**Effect:** Sets the neutral state for reward signaling.

**Mathematical Model:**
```
dopamine(t) = baseline + error_signal(t) × gain
```

**Usage Example:**
```cpp
config->set("dopamine_baseline", 0.2); // Higher baseline
```

#### dopamine_gain (float)

**Default:** 1.0
**Valid Range:** 0.0 to 5.0

**Description:** Sensitivity of dopamine to reward prediction errors.

**Effect:** Controls how strongly reward signals affect neural activity and plasticity.

**Mathematical Impact:** Δw × (1 + dopamine_gain × error).

**Usage Example:**
```cpp
config->set("dopamine_gain", 2.0); // More sensitive reward signaling
```

### Curiosity System

#### curiosity_rate (float)

**Default:** 0.01
**Valid Range:** 0.0 to 1.0

**Description:** Rate of novelty detection and exploration motivation.

**Effect:** Controls how quickly the brain responds to novel stimuli.

**Mathematical Model:**
```
exploration_drive = curiosity_rate × novelty_signal
```

**Usage Example:**
```cpp
config->set("curiosity_rate", 0.05); // Higher exploration motivation
```

#### novelty_threshold (float)

**Default:** 0.5
**Valid Range:** 0.0 to 1.0

**Description:** Threshold for considering an experience novel.

**Effect:** Controls sensitivity to new stimuli. Lower values = more sensitive.

**Mathematical Model:**
```
novelty = max(0, 1 - similarity_to_memory_pool)
if novelty > novelty_threshold: signal_as_novel = true
```

**Usage Example:**
```cpp
config->set("novelty_threshold", 0.3); // Lower threshold = more novelty
```

### Novelty Detection

#### novelty_sensitivity (float)

**Default:** 1.0
**Valid Range:** 0.0 to 10.0

**Description:** Sensitivity of novelty detection system.

**Effect:** Scales the strength of novelty signals.

**Usage Example:**
```cpp
config->set("novelty_sensitivity", 2.0); // More sensitive novelty detection
```

### Prediction Error

#### prediction_error_threshold (float)

**Default:** 0.1
**Valid Range:** 0.0 to 1.0

**Description:** Threshold for significant prediction errors.

**Effect:** Controls when prediction errors are considered large enough to drive learning.

**Usage Example:**
```cpp
config->set("prediction_error_threshold", 0.05); // More sensitive error detection
```

## Checkpoint System Parameters

### Checkpoint Directory

#### checkpoint_dir (std::string)

**Default:** "./checkpoints"
**Valid Range:** Any valid directory path

**Description:** Directory where brain checkpoints are saved.

**Effect:** Determines where brain state is persisted for recovery.

**Usage Example:**
```cpp
config->set("checkpoint_dir", "/data/brain_checkpoints");
```

### Checkpoint Management

#### max_checkpoints (size_t)

**Default:** 10000
**Valid Range:** 1 to 1000000

**Description:** Maximum number of checkpoints to keep.

**Effect:** Controls disk usage. Older checkpoints are deleted when limit is reached.

**Storage Impact:** Each checkpoint ≈ 150 bytes × neuron_count + 500 bytes × synapse_count.

**Usage Example:**
```cpp
config->set("max_checkpoints", 1000); // Limit to 1000 checkpoints
```

#### checkpoint_interval (size_t)

**Default:** 1000
**Valid Range:** 10 to 100000

**Description:** Steps between automatic checkpoint saves.

**Effect:** Controls how frequently brain state is saved.

**Trade-off:** More frequent checkpoints = more disk usage but better recovery.

**Usage Example:**
```cpp
config->set("checkpoint_interval", 500); // Save every 500 steps
```

#### compression_level (string)

**Default:** "balanced"
**Valid Range:** "none", "fast", "balanced", "best"

**Description:** Compression level for checkpoint files.

**Effect:** Trade-off between save speed and file size.

**Usage Example:**
```cpp
config->set("compression_level", "best"); // Best compression
```

## Configuration Validation

### Parameter Validation Rules

All configuration parameters are validated during brain initialization:

1. **Type validation:** Ensures parameter values have correct type
2. **Range validation:** Checks parameter values are within valid ranges
3. **Consistency validation:** Ensures parameters are consistent with each other
4. **Resource validation:** Checks if requested resources are available

### Common Validation Errors

- **InvalidValue:** Parameter outside valid range
- **InvalidType:** Wrong parameter type
- **InconsistentParams:** Conflicting parameter combinations
- **ResourceExhausted:** Requested configuration exceeds system resources

### Example Validation Code

```cpp
bool validate_config(const Config& config) {
    try {
        // Test parameter access
        auto neuron_count = config.get<size_t>("neuron_count");
        if (!neuron_count || *neuron_count == 0) {
            throw ConfigError("neuron_count must be positive");
        }
        
        auto connection_prob = config.get<float>("connection_probability");
        if (connection_prob && (*connection_prob < 0.0f || *connection_prob > 1.0f)) {
            throw ConfigError("connection_probability must be in [0, 1]");
        }
        
        auto timestep = config.get<double>("simulation_timestep");
        if (timestep && *timestep <= 0.0) {
            throw ConfigError("simulation_timestep must be positive");
        }
        
        return true;
    } catch (const ConfigError& e) {
        // Log error and return false
        return false;
    }
}
```

## Configuration Best Practices

### Configuration Organization

1. **Core parameters first:** Set neuron_count, region_count, connection_probability
2. **Simulation parameters:** Set simulation_timestep, plasticity parameters
3. **Memory parameters:** Set memory system capacities and intervals
4. **Development parameters:** Set critical period timing
5. **Neuromodulation:** Set reward and exploration parameters
6. **Checkpointing:** Set checkpoint intervals and compression

### Configuration Templates

#### Simple Brain (Phase 2)
```cpp
config->set("neuron_count", 1000);
config->set("region_count", 1);
config->set("connection_probability", 0.1);
config->set("simulation_timestep", 0.001);
config->set("stdp_ltp_weight", 0.01f);
config->set("stdp_ltd_weight", 0.012f);
config->set("stdp_tau", 20.0f);
```

#### Cognitive Brain (Phase 4)
```cpp
// Add working memory
config->set("working_memory_capacity", 500);
config->set("max_episodes", 1000);

// Add prediction
config->set("prediction_horizon", 100);

// Add neuromodulation
config->set("dopamine_baseline", 0.1f);
config->set("curiosity_rate", 0.01f);

// Add development
config->set("critical_period_start", 100);
config->set("critical_period_end", 1000);
```

#### Research Brain (Phase 6)
```cpp
// Large-scale brain
config->set("neuron_count", 50000);
config->set("region_count", 10);
config->set("connection_probability", 0.05);

// Advanced memory
config->set("max_episodes", 10000);
config->set("replay_interval", 50);
config->set("consolidation_interval", 500);

// Rich neuromodulation
config->set("dopamine_gain", 2.0f);
config->set("curiosity_rate", 0.05f);
config->set("novelty_threshold", 0.3f);

// Frequent checkpointing for long experiments
config->set("checkpoint_interval", 100);
config->set("max_checkpoints", 1000);
```

## Configuration Command Line Interface

NLM provides a command-line interface for setting configuration parameters:

```bash
./nlm --neuron-count 5000 --region-count 5 --connection-probability 0.2 \
     --stdp-ltp-weight 0.02 --stdp-ltd-weight 0.015
```

### Common CLI Options

- `--neuron-count`: Total number of neurons
- `--region-count`: Number of brain regions
- `--connection-probability`: Synapse formation probability
- `--simulation-timestep`: Integration time step
- `--checkpoint-dir`: Directory for checkpoints
- `--help`: Display all available options

## See Also

- [API Reference](API_REFERENCE.md)
- [Tutorial Documentation](TUTORIALS/README.md)
- [Cheat Sheet](CHEAT_SHEET.md)
- [Configuration API](src/core/Config/Config.hpp)
