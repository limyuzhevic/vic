# NLM Configuration Parameters Documentation
# ============================================
# 
# This document describes all configuration parameters available in the NLM (Neural Learning Machine)
# simulation framework. Each parameter is documented with its purpose, valid values, effects,
# and relationships to other parameters.
# 
# Configuration parameters control the behavior of the neural simulation system,
# including neuron properties, synaptic dynamics, memory systems, neuromodulation,
# and simulation parameters. Parameters can be set via configuration files,
# command line arguments, or programmatically during runtime.
#
# Parameter Sources Priority (highest to lowest):
# 1. Command Line (--key=value or -k value)
# 2. Configuration File (default.cfg or custom .cfg files)
# 3. Runtime (set() calls during execution)
# 4. Built-in defaults

## Basic Simulation Parameters

### random_seed (int)
- **Default:** 42
- **Valid Range:** Any integer (typically positive)
- **Purpose:** Sets the random seed for reproducible experiments
- **Effects:** Ensures identical simulation results when run with the same seed
- **Usage:** Set to a fixed value for reproducible research, or use system time for varied results
- **Interactions:** Affects all random number generation throughout the simulation

### simulation_timestep (double)
- **Default:** 0.001
- **Valid Range:** (0.0, 0.1] seconds
- **Purpose:** Controls the duration of each simulation step
- **Effects:** 
  - Smaller timesteps = more accurate but slower simulation
  - Larger timesteps = faster but potentially unstable
- **Usage:** 1ms (0.001s) is typical for LIF neuron simulations
- **Interactions:** Affects stability of neural dynamics and spike timing precision

### neuron_count (int)
- **Default:** 1000
- **Valid Range:** Positive integers
- **Purpose:** Total number of neurons in the simulated brain
- **Effects:**
  - Larger neuron counts = more complex computation
  - Higher memory usage
  - Potentially more interesting emergent behaviors
- **Usage:** Start with 100 neurons for testing, scale up for complex tasks
- **Interactions:** Affects overall brain computation capacity and memory requirements

### region_count (int)
- **Default:** 2
- **Valid Range:** Positive integers
- **Purpose:** Number of neural regions/populations in the brain
- **Effects:**
  - More regions = better anatomical organization
  - Allows for specialized processing areas
  - May enable more realistic brain topology
- **Usage:** 1 for simple simulations, 3-10+ for biologically realistic models
- **Interactions:** Works with region_id assignment and regional connectivity patterns

### connection_probability (double)
- **Default:** 0.1
- **Valid Range:** [0.0, 1.0]
- **Purpose:** Probability of synaptic connections between neurons
- **Effects:**
  - Higher values = more connected, potentially chaotic networks
  - Lower values = sparser connectivity, more structured patterns
- **Usage:** 0.1 = sparse connectivity (typical for biological brains)
- **Interactions:** Affects network sparsity, dynamics, and computational capacity

## Plasticity Parameters

### plasticity_learning_rate (double)
- **Default:** 0.01
- **Valid Range:** (0.0, 1.0)
- **Purpose:** Global learning rate for plasticity mechanisms
- **Effects:**
  - Scales the magnitude of synaptic weight changes
  - Higher values = faster learning but potential instability
  - Lower values = slower but more stable learning
- **Usage:** Adjust based on task difficulty and desired learning speed
- **Interactions:** Affects STDP, Hebbian, and reward-modulated plasticity

### stdp_ltp_weight (double)
- **Default:** 0.01
- **Valid Range:** (0.0, 0.1)
- **Purpose:** Long-Term Potentiation (LTP) weight for STDP
- **Effects:**
  - Strength of synaptic strengthening when presynaptic spike precedes postsynaptic spike
  - Larger values = stronger LTP
  - Affects memory formation and learning speed
- **Usage:** Typical biological values range from 0.001 to 0.1
- **Interactions:** Balanced with stdp_ltd_weight for asymmetric STDP

### stdp_ltd_weight (double)
- **Default:** 0.012
- **Valid Range:** (0.0, 0.1)
- **Purpose:** Long-Term Depression (LTD) weight for STDP
- **Effects:**
  - Strength of synaptic weakening when postsynaptic spike precedes presynaptic spike
  - Larger values = stronger LTD
  - Essential for synaptic competition and homeostasis
- **Usage:** Should be slightly larger than LTP weight for asymmetry
- **Interactions:** Creates spike-timing-dependent asymmetry

### stdp_time_constant (double)
- **Default:** 20.0
- **Valid Range:** (0.0, 100.0) ms
- **Purpose:** Time constant for STDP learning window
- **Effects:**
  - Larger values = wider spike timing window
  - Smaller values = more precise timing dependence
  - Affects sensitivity to spike timing order
- **Usage:** 20ms typical for biological synapses
- **Interactions:** Defines effective range of spike timing influence

## Development Parameters

### development_synaptogenesis_rate (double)
- **Default:** 0.001
- **Valid Range:** (0.0, 0.01)
- **Purpose:** Rate of new synapse formation during development
- **Effects:**
  - Higher values = faster initial connectivity
  - Affects network formation and pruning balance
  - Influences overall brain connectivity
- **Usage:** Low values for gradual development, higher for rapid maturation
- **Interactions:** Balanced with development_pruning_rate

### development_pruning_rate (double)
- **Default:** 0.0001
- **Valid Range:** (0.0, 0.01)
- **Purpose:** Rate of synapse elimination during development
- **Effects:**
  - Higher values = faster pruning
  - Essential for synaptic homeostasis and efficiency
  - Balances synaptogenesis for optimal connectivity
- **Usage:** Typically much lower than synaptogenesis rate
- **Interactions:** Creates developmental trajectory toward mature connectivity

## Neuromodulation Parameters

### dopamine_baseline (double)
- **Default:** 0.1
- **Valid Range:** (0.0, 1.0)
- **Purpose:** Baseline dopamine level affecting neural excitability
- **Effects:**
  - Higher levels = increased neural excitability
  - Modulates plasticity and learning speed
  - Affects motivation and attention
- **Usage:** Adjust based on task demands and behavioral requirements
- **Interactions:** Affects neuromodulator system and plasticity rules

### reward_discount_factor (double)
- **Default:** 0.99
- **Valid Range:** [0.0, 1.0]
- **Purpose:** Discount factor for future rewards in reward prediction
- **Effects:**
  - Values closer to 1 = more patient (long-term planning)
  - Values closer to 0 = more impatient (short-term focus)
  - Critical for exploration-exploitation balance
- **Usage:** 0.99 typical for biological time horizons
- **Interactions:** Affects curiosity, motivation, and decision-making

## Environment Parameters

### environment_name (string)
- **Default:** "GridWorld"
- **Valid Values:** "GridWorld", "Continuous", "Custom"
- **Purpose:** Type of environment for agent simulation
- **Effects:**
  - Determines sensory input and motor output formats
  - Affects available behaviors and interactions
  - Changes simulation dynamics
- **Usage:** "GridWorld" for discrete environments, "Continuous" for smooth movement
- **Interactions:** Must match agent brain configuration

### environment_width (int)
- **Default:** 10
- **Valid Range:** Positive integers
- **Purpose:** Width of the simulation environment
- **Effects:**
  - Affects sensory input size and spatial resolution
  - Changes movement possibilities
  - Impacts navigation complexity
- **Usage:** Larger values = more space, more complex navigation
- **Interactions:** Must be compatible with vision configuration

### environment_height (int)
- **Default:** 10
- **Valid Range:** Positive integers
- **Purpose:** Height of the simulation environment
- **Effects:**
  - Same as environment_width but for vertical dimension
  - Together define environment dimensions
  - Affects agent movement capabilities
- **Usage:** Match width for square environments, different for rectangular
- **Interactions:** Combined with environment_width for full dimensions

## Vision Processing Parameters

### vision_width (int)
- **Default:** Not specified in default.cfg
- **Valid Range:** Positive integers
- **Purpose:** Width of visual field (usually specified in world config)
- **Note:** Typically set via world configuration, not global config

### vision_height (int)
- **Default:** Not specified in default.cfg
- **Valid Range:** Positive integers
- **Purpose:** Height of visual field (usually specified in world config)
- **Note:** Typically set via world configuration, not global config

## Logging and Simulation Parameters

### log_level (string)
- **Default:** "INFO"
- **Valid Values:** "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"
- **Purpose:** Controls logging verbosity
- **Effects:**
  - DEBUG = Detailed information for debugging
  - INFO = Normal operation messages
  - WARNING = Potential issues
  - ERROR = Errors and failures
  - CRITICAL = Critical system problems
- **Usage:** DEBUG for development, INFO for production
- **Interactions:** Affects log file size and debugging capability

### log_to_file (bool)
- **Default:** false
- **Valid Values:** true, false
- **Purpose:** Whether to write logs to file
- **Effects:**
  - true = logs saved to nlm.log file
  - false = logs only to console/standard output
- **Usage:** true for persistent logging, false for debugging
- **Interactions:** Works with log_level to control log output

### log_filename (string)
- **Default:** "nlm.log"
- **Valid Values:** Any valid filename
- **Purpose:** Filename for log output when log_to_file is true
- **Usage:** Can be customized for multiple simulation runs
- **Interactions:** Used only when log_to_file is true

### max_simulation_steps (int)
- **Default:** 10000
- **Valid Range:** Positive integers
- **Purpose:** Maximum number of simulation steps before automatic stop
- **Effects:**
  - Prevents infinite simulation loops
  - Limits total simulation time
  - Useful for automated testing
- **Usage:** Increase for longer simulations, decrease for quick tests
- **Interactions:** Combined with simulation_time_limit

### simulation_time_limit (double)
- **Default:** 0.0 (no limit)
- **Valid Range:** (0.0, ∞) seconds
- **Purpose:** Maximum simulation time before automatic stop
- **Effects:**
  - Provides time-based termination condition
  - More natural than step-based limits
  - Allows for variable timestep implementations
- **Usage:** Set to specific time limit for timed experiments
- **Interactions:** Combined with max_simulation_steps

### visualization_enabled (bool)
- **Default:** false
- **Valid Values:** true, false
- **Purpose:** Enable/disable visualization system
- **Effects:**
  - true = visualization system initialized (may require external libraries)
  - false = minimal visualization overhead
- **Usage:** true for visual debugging, false for production
- **Interactions:** Requires visualization libraries and may affect performance

### visualization_update_rate (double)
- **Default:** 30.0
- **Valid Range:** (0.0, ∞) Hz
- **Purpose:** Frequency of visualization updates
- **Effects:**
  - Higher values = smoother visualization
  - Lower values = better performance
  - Affects real-time rendering capability
- **Usage:** 30Hz typical for smooth animation
- **Interactions:** Requires visualization_enabled to be true

## Configuration File Format

### File Format
- **Format:** Simple key=value pairs
- **Comments:** Lines starting with # or /
- **Strings:** Enclosed in double quotes (optional)
- **Boolean:** true, false, yes, no, 1, 0
- **Numbers:** Any valid integer or floating-point format

### Example Configuration File
```
# NLM Configuration File
# This is the default configuration for Phase 1 skeleton

# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 1000

# Number of brain regions
region_count = 2

# Connection probability for random connectivity
connection_probability = 0.1

# Plasticity settings
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_time_constant = 20.0

# Development settings
development_synaptogenesis_rate = 0.001
development_pruning_rate = 0.0001

# Neuromodulation settings
dopamine_baseline = 0.1

# Reward settings
reward_discount_factor = 0.99

# Environment settings
environment_name = GridWorld
environment_width = 10
environment_height = 10

# Logging settings
log_level = INFO
log_to_file = false
log_filename = nlm.log

# Simulation settings
max_simulation_steps = 10000
simulation_time_limit = 0.0

# Visualization settings
visualization_enabled = false
visualization_update_rate = 30.0
```

## Configuration Parameter Guidelines

### Parameter Selection Strategy

1. **For Testing:** Use small neuron counts, simple environments, detailed logging
2. **For Research:** Use biologically realistic parameters, longer simulations
3. **For Production:** Use optimized parameters, minimal logging, visualization off

### Parameter Interactions

1. **neuron_count × connection_probability →** Network connectivity level
2. **simulation_timestep →** Numerical stability and speed
3. **plasticity_learning_rate × stdp_time_constant →** Learning dynamics
4. **development_× rates →** Developmental trajectory
5. **dopamine_baseline →** Overall neural excitability
6. **reward_discount_factor →** Temporal discounting behavior

### Best Practices

1. **Document Changes:** Keep track of parameter modifications for reproducibility
2. **Start Conservative:** Begin with default values and modify gradually
3. **Validate Ranges:** Ensure parameters are within biologically plausible ranges
4. **Test Independently:** Modify one parameter at a time to isolate effects
5. **Reproduce Results:** Use fixed random_seed for reproducible experiments
6. **Monitor Performance:** Watch for performance issues with large parameter values

## Configuration Parameter Validation

### Required Parameters
- **simulation_timestep:** Must be positive and small enough for numerical stability
- **neuron_count:** Must be positive for meaningful simulation
- **connection_probability:** Must be between 0.0 and 1.0

### Optional Parameters
- Many parameters have sensible defaults and can be omitted
- Missing parameters will use built-in defaults
- Custom configurations can override any default

### Parameter Conflicts
- **High connection_probability + high neuron_count →** Memory/performance issues
- **Small simulation_timestep + large max_simulation_steps →** Very long simulation times
- **High plasticity_learning_rate + unstable parameters →** Explosive weight growth

## Configuration Management Tips

1. **Save Configurations:** Use `saveToFile()` to capture current settings
2. **Compare Configurations:** Use `summary()` to inspect parameter values
3. **Parameter Validation:** Implement custom validation for critical parameters
4. **Environment-Specific:** Maintain different configs for development, testing, production
5. **Version Control:** Track configuration changes alongside code changes

This comprehensive configuration system allows for flexible control over NLM simulations while providing sensible defaults for common use cases. Proper configuration management is key to reproducible and meaningful neural simulation research.