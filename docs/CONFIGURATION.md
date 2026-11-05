# NLM Configuration Schema

## Overview

NLM configuration file schema with parameter descriptions, types, and validation rules. This documentation covers all parameters defined in `configs/default.cfg` and provides comprehensive information for configuring the NLM neural simulation.

## Configuration Format

Configuration files use simple key = value pairs:
```
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
```

## Configuration Parameters

### random_seed
- **Description**: Random seed for reproducible experiments
- **Type**: int64
- **Default Value**: 42
- **Allowed Values**: Any integer
- **Validation Rules**: Must be positive
- **Example**: `random_seed = 42`

### simulation_timestep
- **Description**: Simulation timestep in seconds
- **Type**: double
- **Default Value**: 0.001
- **Allowed Values**: (0.0, ∞)
- **Validation Rules**: Must be positive
- **Example**: `simulation_timestep = 0.001`

### neuron_count
- **Description**: Number of neurons in the brain
- **Type**: int64
- **Default Value**: 1000
- **Allowed Values**: 1 to 10000
- **Validation Rules**: Must be positive
- **Example**: `neuron_count = 1000`

### region_count
- **Description**: Number of brain regions
- **Type**: int64
- **Default Value**: 2
- **Allowed Values**: 1 to 10
- **Validation Rules**: Must be positive
- **Example**: `region_count = 2`

### connection_probability
- **Description**: Connection probability for random connectivity
- **Type**: float
- **Default Value**: 0.1
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `connection_probability = 0.1`

### plasticity_learning_rate
- **Description**: Learning rate for plasticity rules
- **Type**: float
- **Default Value**: 0.01
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be positive
- **Example**: `plasticity_learning_rate = 0.01`

### stdp_ltp_weight
- **Description**: STDP long-term potentiation weight
- **Type**: float
- **Default Value**: 0.01
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be positive
- **Example**: `stdp_ltp_weight = 0.01`

### stdp_ltd_weight
- **Description**: STDP long-term depression weight
- **Type**: float
- **Default Value**: 0.012
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be positive
- **Example**: `stdp_ltd_weight = 0.012`

### stdp_time_constant
- **Description**: STDP time constant in milliseconds
- **Type**: double
- **Default Value**: 20.0
- **Allowed Values**: 1.0 to 1000.0
- **Validation Rules**: Must be positive
- **Example**: `stdp_time_constant = 20.0`

### development_synaptogenesis_rate
- **Description**: Rate of new synapse formation during development
- **Type**: float
- **Default Value**: 0.001
- **Allowed Values**: 0.0 to 0.01
- **Validation Rules**: Must be non-negative
- **Example**: `development_synaptogenesis_rate = 0.001`

### development_pruning_rate
- **Description**: Rate of synapse pruning during development
- **Type**: float
- **Default Value**: 0.0001
- **Allowed Values**: 0.0 to 0.005
- **Validation Rules**: Must be non-negative
- **Example**: `development_pruning_rate = 0.0001`

### dopamine_baseline
- **Description**: Baseline dopamine level
- **Type**: float
- **Default Value**: 0.1
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be non-negative
- **Example**: `dopamine_baseline = 0.1`

### reward_discount_factor
- **Description**: Reward discount factor for temporal discounting
- **Type**: float
- **Default Value**: 0.99
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `reward_discount_factor = 0.99`

### environment_name
- **Description**: Name of environment to use
- **Type**: string
- **Default Value**: GridWorld
- **Allowed Values**: GridWorld, RandomWorld, MazeWorld
- **Validation Rules**: Must be one of: GridWorld, RandomWorld, MazeWorld
- **Example**: `environment_name = GridWorld`

### environment_width
- **Description**: Width of the environment in cells
- **Type**: int64
- **Default Value**: 10
- **Allowed Values**: 1 to 100
- **Validation Rules**: Must be positive
- **Example**: `environment_width = 10`

### environment_height
- **Description**: Height of the environment in cells
- **Type**: int64
- **Default Value**: 10
- **Allowed Values**: 1 to 100
- **Validation Rules**: Must be positive
- **Example**: `environment_height = 10`

### log_level
- **Description**: Logging level for output
- **Type**: string
- **Default Value**: INFO
- **Allowed Values**: DEBUG, INFO, WARN, ERROR
- **Validation Rules**: Must be one of: DEBUG, INFO, WARN, ERROR
- **Example**: `log_level = INFO`

### log_to_file
- **Description**: Whether to log to file in addition to console
- **Type**: bool
- **Default Value**: false
- **Allowed Values**: true, false
- **Validation Rules**: Must be boolean
- **Example**: `log_to_file = false`

### log_filename
- **Description**: Filename for log file output
- **Type**: string
- **Default Value**: nlm.log
- **Allowed Values**: Any valid filename
- **Validation Rules**: Must not contain invalid characters
- **Example**: `log_filename = nlm.log`

### max_simulation_steps
- **Description**: Maximum number of simulation steps
- **Type**: int64
- **Default Value**: 10000
- **Allowed Values**: 1 to 1000000
- **Validation Rules**: Must be positive
- **Example**: `max_simulation_steps = 10000`

### simulation_time_limit
- **Description**: Maximum simulation time in seconds
- **Type**: double
- **Default Value**: 0.0
- **Allowed Values**: 0.0 to 86400.0 (24 hours)
- **Validation Rules**: Must be non-negative
- **Example**: `simulation_time_limit = 0.0`

### visualization_enabled
- **Description**: Whether visualization is enabled
- **Type**: bool
- **Default Value**: false
- **Allowed Values**: true, false
- **Validation Rules**: Must be boolean
- **Example**: `visualization_enabled = false`

### visualization_update_rate
- **Description**: Visualization update rate in Hz
- **Type**: double
- **Default Value**: 30.0
- **Allowed Values**: 1.0 to 1000.0
- **Validation Rules**: Must be positive
- **Example**: `visualization_update_rate = 30.0`

### synaptic_weight_max
- **Description**: Maximum synaptic weight
- **Type**: float
- **Default Value**: 1.0
- **Allowed Values**: 0.0 to 10.0
- **Validation Rules**: Must be positive
- **Example**: `synaptic_weight_max = 1.0`

### synaptic_weight_min
- **Description**: Minimum synaptic weight
- **Type**: float
- **Default Value**: 0.0
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be non-negative
- **Example**: `synaptic_weight_min = 0.0`

### neural_connectivity_pattern
- **Description**: Pattern of neural connectivity
- **Type**: string
- **Default Value**: random
- **Allowed Values**: random, grid, ring, small_world
- **Validation Rules**: Must be one of: random, grid, ring, small_world
- **Example**: `neural_connectivity_pattern = random`

### reward_prediction_error_gamma
- **Description**: Discount factor for reward prediction error
- **Type**: float
- **Default Value**: 0.95
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `reward_prediction_error_gamma = 0.95`

### reward_prediction_error_lambda
- **Description**: Eligibility trace decay for reward prediction error
- **Type**: float
- **Default Value**: 0.9
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `reward_prediction_error_lambda = 0.9`

### neuromodulation_strength
- **Description**: Overall strength of neuromodulation
- **Type**: float
- **Default Value**: 1.0
- **Allowed Values**: 0.0 to 5.0
- **Validation Rules**: Must be non-negative
- **Example**: `neuromodulation_strength = 1.0`

### learning_algorithm
- **Description**: Primary learning algorithm to use
- **Type**: string
- **Default Value**: stdp
- **Allowed Values**: stdp, hebbian, reward_modulated, mixed
- **Validation Rules**: Must be one of: stdp, hebbian, reward_modulated, mixed
- **Example**: `learning_algorithm = stdp`

### learning_rate
- **Description**: Base learning rate
- **Type**: float
- **Default Value**: 0.01
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be positive
- **Example**: `learning_rate = 0.01`

### learning_momentum
- **Description**: Momentum factor for learning updates
- **Type**: float
- **Default Value**: 0.9
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `learning_momentum = 0.9`

### development_stage_transition_age
- **Description**: Age at which development stage transitions occur
- **Type**: double
- **Default Value**: 1000.0
- **Allowed Values**: 0.0 to 10000.0
- **Validation Rules**: Must be positive
- **Example**: `development_stage_transition_age = 1000.0`

### development_plasticity_decay_rate
- **Description**: Rate at which plasticity decays with development
- **Type**: float
- **Default Value**: 0.99
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `development_plasticity_decay_rate = 0.99`

### working_memory_capacity
- **Description**: Capacity of working memory system
- **Type**: float
- **Default Value**: 100.0
- **Allowed Values**: 0.0 to 1000.0
- **Validation Rules**: Must be non-negative
- **Example**: `working_memory_capacity = 100.0`

### episodic_memory_max_episodes
- **Description**: Maximum number of episodes in episodic memory
- **Type**: int64
- **Default Value**: 1000
- **Allowed Values**: 0 to 100000
- **Validation Rules**: Must be non-negative
- **Example**: `episodic_memory_max_episodes = 1000`

### episodic_memory_memory_strength
- **Description**: Strength of episodic memory traces
- **Type**: float
- **Default Value**: 0.5
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `episodic_memory_memory_strength = 0.5`

### prediction_window_size
- **Description**: Window size for prediction system
- **Type**: int64
- **Default Value**: 10
- **Allowed Values**: 1 to 100
- **Validation Rules**: Must be positive
- **Example**: `prediction_window_size = 10`

### prediction_confidence_threshold
- **Description**: Confidence threshold for prediction system
- **Type**: float
- **Default Value**: 0.7
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `prediction_confidence_threshold = 0.7`

### concept_formation_threshold
- **Description**: Threshold for concept formation
- **Type**: float
- **Default Value**: 0.8
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `concept_formation_threshold = 0.8`

### self_model_confidence_decay
- **Description**: Confidence decay rate for self-model
- **Type**: float
- **Default Value**: 0.95
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `self_model_confidence_decay = 0.95`

### social_learning_weight
- **Description**: Weight of social learning in learning process
- **Type**: float
- **Default Value**: 0.5
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `social_learning_weight = 0.5`

### max_pending_spikes
- **Description**: Maximum number of pending spikes in event queue
- **Type**: int64
- **Default Value**: 10000
- **Allowed Values**: 0 to 1000000
- **Validation Rules**: Must be non-negative
- **Example**: `max_pending_spikes = 10000`

### event_queue_size
- **Description**: Size of event queue
- **Type**: int64
- **Default Value**: 1000
- **Allowed Values**: 0 to 100000
- **Validation Rules**: Must be non-negative
- **Example**: `event_queue_size = 1000`

### memory_pool_size
- **Description**: Size of memory pools
- **Type**: int64
- **Default Value**: 10000
- **Allowed Values**: 0 to 1000000
- **Validation Rules**: Must be non-negative
- **Example**: `memory_pool_size = 10000`

### checkpoint_format_version
- **Description**: Version number for checkpoint format
- **Type**: int64
- **Default Value**: 1
- **Allowed Values**: 1 to 10
- **Validation Rules**: Must be positive
- **Example**: `checkpoint_format_version = 1`

### checkpoint_compression
- **Description**: Whether to compress checkpoints
- **Type**: bool
- **Default Value**: true
- **Allowed Values**: true, false
- **Validation Rules**: Must be boolean
- **Example**: `checkpoint_compression = true`

### checkpoint_save_interval
- **Description**: How often to save checkpoints (in steps)
- **Type**: int64
- **Default Value**: 1000
- **Allowed Values**: 1 to 100000
- **Validation Rules**: Must be positive
- **Example**: `checkpoint_save_interval = 1000`

### dopamine_sensitivity
- **Description**: Sensitivity of dopamine system
- **Type**: float
- **Default Value**: 1.0
- **Allowed Values**: 0.0 to 10.0
- **Validation Rules**: Must be non-negative
- **Example**: `dopamine_sensitivity = 1.0`

### curiosity_decay_rate
- **Description**: Rate at which curiosity decays
- **Type**: float
- **Default Value**: 0.98
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `curiosity_decay_rate = 0.98`

### novelty_threshold
- **Description**: Threshold for novelty detection
- **Type**: float
- **Default Value**: 0.1
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `novelty_threshold = 0.1`

### touch_sensitivity
- **Description**: Sensitivity of touch sensory system
- **Type**: float
- **Default Value**: 1.0
- **Allowed Values**: 0.0 to 10.0
- **Validation Rules**: Must be non-negative
- **Example**: `touch_sensitivity = 1.0`

### vision_range
- **Description**: Visual range in cells
- **Type**: float
- **Default Value**: 5.0
- **Allowed Values**: 1.0 to 50.0
- **Validation Rules**: Must be positive
- **Example**: `vision_range = 5.0`

### vision_resolution
- **Description**: Resolution of vision system
- **Type**: int64
- **Default Value**: 16
- **Allowed Values**: 1 to 256
- **Validation Rules**: Must be positive
- **Example**: `vision_resolution = 16`

### motor_action_smoothness
- **Description**: Smoothness factor for motor actions
- **Type**: float
- **Default Value**: 0.9
- **Allowed Values**: 0.0 to 1.0
- **Validation Rules**: Must be between 0.0 and 1.0
- **Example**: `motor_action_smoothness = 0.9`

### experiment_seed
- **Description**: Seed for experimental reproducibility
- **Type**: int64
- **Default Value**: 42
- **Allowed Values**: Any integer
- **Validation Rules**: Must be positive
- **Example**: `experiment_seed = 42`

### experiment_max_trials
- **Description**: Maximum number of trials in experiment
- **Type**: int64
- **Default Value**: 100
- **Allowed Values**: 1 to 10000
- **Validation Rules**: Must be positive
- **Example**: `experiment_max_trials = 100`

### experiment_reward_function
- **Description**: Reward function type for experiments
- **Type**: string
- **Default Value**: exponential
- **Allowed Values**: exponential, linear, logarithmic
- **Validation Rules**: Must be one of: exponential, linear, logarithmic
- **Example**: `experiment_reward_function = exponential`

## Configuration Validation

### Parameter Types

Configuration parameters support the following types:

| Type | Description | Example |
|------|-------------|---------|
| int64 | 64-bit integer | `42` |
| double | Double precision float | `3.14` |
| float | Single precision float | `1.5` |
| bool | Boolean value | `true` or `false` |
| string | Text string | `"GridWorld"` |
| list<int> | List of integers | `[1, 2, 3]` |
| list<double> | List of doubles | `[1.0, 2.0, 3.0]` |
| list<string> | List of strings | `["option1", "option2"]` |

### Validation Rules

1. **Type Validation**: Parameter values must match their declared type
2. **Range Validation**: Numeric values must be within allowed ranges
3. **Enum Validation**: String values must be from allowed set
4. **Format Validation**: Filenames and paths must be valid
5. **Dependency Validation**: Some parameters require others to be set

### Example Configurations

#### Basic Configuration
```
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
log_level = INFO
```

#### Advanced Configuration
```
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 2000
region_count = 3
connection_probability = 0.15
plasticity_learning_rate = 0.02
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 25.0
development_synaptogenesis_rate = 0.002
development_pruning_rate = 0.0002

environment_name = GridWorld
environment_width = 20
environment_height = 20
vision_range = 10
vision_resolution = 32

log_level = DEBUG
log_to_file = true
log_filename = nlm_research.log

max_simulation_steps = 50000
checkpoint_save_interval = 5000
checkpoint_compression = true

neuromodulation_strength = 1.5
dopamine_baseline = 0.2
reward_discount_factor = 0.98
```

#### Production Configuration
```
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
log_level = INFO
log_to_file = true
log_filename = nlm.log

max_simulation_steps = 10000
visualization_enabled = false

checkpoint_save_interval = 1000
checkpoint_compression = true

experiment_seed = 42
experiment_max_trials = 100
experiment_reward_function = exponential
```

## Configuration Command Line Integration

The CLI provides the following configuration-related commands:

- `nlm config list` - List all configuration parameters
- `nlm config get <key>` - Get configuration value
- `nlm config set <key> <value>` - Set configuration value
- `nlm config validate` - Validate configuration
- `nlm config export <file>` - Export configuration to file

## Configuration File Examples

Configuration files should be placed in the `configs/` directory with `.cfg` or `.json` extensions.

## Configuration Documentation Index

For quick reference to specific parameters:
- [Alphabetical List](/docs/CONFIGURATION.md#configuration-parameters)
- [Parameter Types](#parameter-types)
- [Validation Rules](#configuration-validation)

---
*Configuration schema generated from configs/default.cfg*
